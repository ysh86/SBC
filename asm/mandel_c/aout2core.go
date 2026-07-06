// aout2core loads a MINIX/m68k a.out image into a 1 MiB address space and
// writes that address space as a raw core image.
//
// $ srec_cat aout_00000400-00000bb8.bin -Binary \
// -offset 0x400 -Execution_Start_Address 0x400 \
// -Output aout_00000400-00000bb8.srec \
// -Motorola -disable=header -disable=data-count -disable=footer
package main

import (
	"encoding/binary"
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

const (
	headerSize        = 32
	memorySize        = 1024 * 1024
	textStart  uint32 = 0x0400
	magicMask         = 0xff0fffff
	m68kMagic         = 0x04000301
)

type header struct {
	Magic   uint32
	HdrLen  uint32
	Text    uint32
	Data    uint32
	BSS     uint32
	Entry   uint32
	Total   uint32
	Symbols uint32
}

func parseHeader(image []byte) (header, error) {
	if len(image) < headerSize {
		return header{}, errors.New("file is shorter than the 32-byte a.out header")
	}

	h := header{
		Magic:   binary.BigEndian.Uint32(image[0:4]),
		HdrLen:  binary.BigEndian.Uint32(image[4:8]) & 0xff,
		Text:    binary.BigEndian.Uint32(image[8:12]),
		Data:    binary.BigEndian.Uint32(image[12:16]),
		BSS:     binary.BigEndian.Uint32(image[16:20]),
		Entry:   binary.BigEndian.Uint32(image[20:24]),
		Total:   binary.BigEndian.Uint32(image[24:28]),
		Symbols: binary.BigEndian.Uint32(image[28:32]),
	}
	if h.Magic&magicMask != m68kMagic {
		return header{}, fmt.Errorf("not a MINIX/m68k a.out (magic %#08x)", h.Magic)
	}
	if h.HdrLen != headerSize {
		return header{}, fmt.Errorf("unsupported a.out header length %d (want %d)", h.HdrLen, headerSize)
	}
	if h.Text == 0 {
		return header{}, errors.New("a.out has an empty text segment")
	}
	if h.Total > memorySize {
		return header{}, fmt.Errorf("a.out total size %#x exceeds 1 MiB", h.Total)
	}
	return h, nil
}

func checkedAdd(a, b uint32) (uint32, bool) {
	c := a + b
	return c, c >= a
}

func load(image []byte) ([]byte, header, error) {
	h, err := parseHeader(image)
	if err != nil {
		return nil, header{}, err
	}

	textEnd, ok := checkedAdd(textStart, h.Text)
	if !ok {
		return nil, header{}, errors.New("text segment address overflow")
	}
	dataEnd, ok := checkedAdd(textEnd, h.Data)
	if !ok {
		return nil, header{}, errors.New("data segment address overflow")
	}
	bssStart := dataEnd
	bssEnd, ok := checkedAdd(bssStart, h.BSS)
	if !ok || bssEnd > memorySize-2 {
		return nil, header{}, fmt.Errorf("text/data/BSS end %#x is outside 1 MiB memory", bssEnd)
	}
	if bssEnd&1 != 0 {
		return nil, header{}, fmt.Errorf("program break %#x is not even", bssEnd)
	}
	mem := make([]byte, memorySize)
	payload := image[headerSize:]
	if len(payload) == 0 {
		return nil, header{}, errors.New("a.out contains no loadable payload")
	}
	maxPayload := memorySize - int(textStart)
	if len(payload) > maxPayload {
		payload = payload[:maxPayload]
	}
	copy(mem[textStart:], payload)

	// The file layout after text+data is: symbols, then relocation data.
	relocStart64 := uint64(bssStart) + uint64(h.Symbols)
	if relocStart64 > memorySize {
		return nil, header{}, fmt.Errorf("relocation table offset %#x is outside memory", relocStart64)
	}
	relocStart := uint32(relocStart64)
	copy(mem[bssEnd:], mem[relocStart:])
	clear(mem[bssStart:bssEnd])

	if err := relocate(mem, h, bssEnd, dataEnd); err != nil {
		return nil, header{}, err
	}
	return mem, h, nil
}

func relocate(mem []byte, h header, relocStart, dataEnd uint32) error {
	if relocStart > memorySize-4 {
		return errors.New("no room for relocation table header")
	}
	pos := relocStart
	addr := binary.BigEndian.Uint32(mem[pos : pos+4])
	pos += 4
	if textStart == h.Entry || addr == 0 {
		return nil
	}
	addr, ok := checkedAdd(addr, textStart)
	if !ok {
		return errors.New("first relocation address overflow")
	}

	for {
		if addr > memorySize-4 || addr >= dataEnd {
			return fmt.Errorf("relocation address %#x is outside text/data", addr)
		}
		v := binary.BigEndian.Uint32(mem[addr : addr+4])
		binary.BigEndian.PutUint32(mem[addr:addr+4], v+textStart)

		var delta uint32
		for {
			if pos >= memorySize {
				return errors.New("unterminated relocation table")
			}
			b := mem[pos]
			pos++
			if b == 1 {
				delta += 254
				continue
			}
			if b == 0 {
				return nil
			}
			if b&1 != 0 {
				return fmt.Errorf("invalid odd relocation delta %#x", b)
			}
			delta += uint32(b)
			break
		}
		addr, ok = checkedAdd(addr, delta)
		if !ok {
			return errors.New("relocation address overflow")
		}
	}
}

func run(args []string) error {
	if len(args) < 1 || len(args) > 2 {
		return errors.New("usage: aout2core input.aout [output.bin]")
	}
	output := "core.bin"
	if len(args) == 2 {
		output = args[1]
	}

	image, err := os.ReadFile(args[0])
	if err != nil {
		return fmt.Errorf("read %q: %w", args[0], err)
	}
	mem, h, err := load(image)
	if err != nil {
		return fmt.Errorf("load %q: %w", args[0], err)
	}
	bssEnd := textStart + h.Text + h.Data + h.BSS
	stem := strings.TrimSuffix(args[0], filepath.Ext(args[0]))
	rangeOutput := fmt.Sprintf("%s_%08x-%08x.bin", stem, textStart, bssEnd)
	if filepath.Clean(output) == filepath.Clean(rangeOutput) {
		return fmt.Errorf("core output and range output resolve to the same file %q", output)
	}
	if err := os.WriteFile(output, mem, 0o644); err != nil {
		return fmt.Errorf("write %q: %w", output, err)
	}
	rangeImage := mem[textStart:bssEnd]
	if err := os.WriteFile(rangeOutput, rangeImage, 0o644); err != nil {
		return fmt.Errorf("write %q: %w", rangeOutput, err)
	}
	fmt.Printf("%s: wrote %d bytes (text=%#x data=%#x bss=%#x entry=%#x)\n",
		output, len(mem), h.Text, h.Data, h.BSS, h.Entry)
	fmt.Printf("%s: wrote %d bytes (memory [%#x, %#x))\n",
		rangeOutput, len(rangeImage), textStart, bssEnd)
	return nil
}

func main() {
	if err := run(os.Args[1:]); err != nil {
		fmt.Fprintln(os.Stderr, "aout2core:", err)
		os.Exit(1)
	}
}
