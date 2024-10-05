package main

import (
	"bytes"
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/gif"
	"os"

	"github.com/marcinbor85/gohex"
)

const (
	canvasW = 80 * 8
	canvasH = 200
	stride  = 80 * 8
	padding = 384
)

type PlaneType int

const (
	B       PlaneType = 0
	R       PlaneType = 1
	G       PlaneType = 2
	invalid PlaneType = 3
)

func (p PlaneType) String() string {
	switch p {
	case B:
		return "B"
	case R:
		return "R"
	case G:
		return "G"
	default:
		return "invalid"
	}
}

var canvasRect image.Rectangle
var palette color.Palette

func init() {
	canvasRect = image.Rect(0, 0, canvasW, canvasH)
	palette = color.Palette{
		color.Black,                  // GRB
		color.RGBA{0, 0, 255, 255},   // 001b
		color.RGBA{255, 0, 0, 255},   // 010b
		color.RGBA{255, 0, 255, 255}, // 011b
		color.RGBA{0, 255, 0, 255},   // 100b
		color.RGBA{0, 255, 255, 255}, // 101b
		color.RGBA{255, 255, 0, 255}, // 110b
		color.White,                  // 111b
	}
}

func main() {
	// args
	var (
		outFile   string
		header    string
		ihexFile0 string
		ihexFile1 string
		ihexFile2 string
	)
	flag.Parse()
	if len(flag.Args()) < 5 {
		flag.Usage()
		return
	}
	outFile = flag.Args()[0]
	header = flag.Args()[1]
	ihexFile0 = flag.Args()[2]
	ihexFile1 = flag.Args()[3]
	ihexFile2 = flag.Args()[4]

	// header
	encBytes, err := os.ReadFile(header)
	if err != nil {
		panic(err)
	}
	offsetXInChar := 0 // TODO
	offsetX := offsetXInChar << 3
	offsetY := 0
	fmt.Printf("length: %d\n", len(encBytes))
	fmt.Printf("canvas: W=%d,H=%d,stride=%d\n", canvasW, canvasH, stride)
	fmt.Printf("offset: XinChar=%d,X=%d,Y=%d\n", offsetXInChar, offsetX, offsetY)
	fmt.Println()

	flag0 := (encBytes[0] >> 0) & 3
	flag1 := (encBytes[0] >> 2) & 3
	flag2 := (encBytes[0] >> 4) & 3
	flags := []byte{flag0, flag1, flag2}
	fmt.Printf("%02x: flags 221100: %02b%02b%02b\n", encBytes[0], flags[2], flags[1], flags[0])

	wInChar := encBytes[1]
	w := int(wInChar) << 3
	h := int(encBytes[2])
	fmt.Printf("%02x: width  [char]: %d, [dot]: %d\n", encBytes[1], wInChar, w)
	fmt.Printf("%02x: height [line]: %d\n", encBytes[2], h)

	planes := []PlaneType{PlaneType((encBytes[3] >> 0) & 3), PlaneType((encBytes[3] >> 2) & 3), PlaneType((encBytes[3] >> 4) & 3)}
	fmt.Printf("%02x: plane order: %d,%d,%d\n", encBytes[3], planes[0], planes[1], planes[2])

	offset1 := int16(encBytes[4]) | (int16(encBytes[5]) << 8)
	offset2 := int16(encBytes[6]) | (int16(encBytes[7]) << 8)
	encodedSlices := [][]byte{encBytes[8:offset1], encBytes[offset1:offset2], encBytes[offset2:]}
	fmt.Printf("%02x%02x: offset1: %d\n", encBytes[5], encBytes[4], offset1)
	fmt.Printf("%02x%02x: offset2: %d\n", encBytes[7], encBytes[6], offset2)
	fmt.Printf("  plane0 length: %d\n", len(encodedSlices[0]))
	fmt.Printf("  plane1 length: %d\n", len(encodedSlices[1]))
	fmt.Printf("  plane2 length: %d\n", len(encodedSlices[2]))
	fmt.Println()

	// load
	data, err := loadHex(ihexFile0)
	if err != nil {
		fmt.Fprintf(os.Stderr, "loadHex: %s\n", err)
		return
	}
	data1, err := loadHex(ihexFile1)
	if err != nil {
		fmt.Fprintf(os.Stderr, "loadHex: %s\n", err)
		return
	}
	data = append(data, data1...)
	data1, err = loadHex(ihexFile2)
	if err != nil {
		fmt.Fprintf(os.Stderr, "loadHex: %s\n", err)
		return
	}
	data = append(data, data1...)
	fmt.Println()

	// canvas & ROI
	canvas := image.NewPaletted(canvasRect, palette)
	roi := image.Rectangle{image.Point{offsetX, offsetY}, image.Point{offsetX + w, offsetY + h}}

	for i, plane := range planes {
		fmt.Printf("draw %s(%d): ", plane, plane)
		if flags[i]&1 == 0 {
			if flags[i]&2 == 0 {
				fmt.Printf("fill zero\n")
				fillZero(canvas, plane, &roi)
			} else {
				fmt.Printf("do nothing\n")
			}
		} else {
			algo := (i << 2) | (int(flags[i]))
			fmt.Printf("algo=%04b\n", algo)

			for y := roi.Min.Y; y < roi.Max.Y; y++ {
				for x8 := 0; x8 < int(wInChar); x8++ {
					x := (x8 << 3) + roi.Min.X
					dots := data[(canvasH*(stride>>3)+padding)*i+(stride>>3)*y+x8]

					dst := canvas.ColorIndexAt(x+0, y) & ^(1 << plane)
					canvas.SetColorIndex(x+0, y, dst|((dots>>7)<<plane))

					dst = canvas.ColorIndexAt(x+1, y) & ^(1 << plane)
					canvas.SetColorIndex(x+1, y, dst|(((dots>>6)&1)<<plane))

					dst = canvas.ColorIndexAt(x+2, y) & ^(1 << plane)
					canvas.SetColorIndex(x+2, y, dst|(((dots>>5)&1)<<plane))

					dst = canvas.ColorIndexAt(x+3, y) & ^(1 << plane)
					canvas.SetColorIndex(x+3, y, dst|(((dots>>4)&1)<<plane))

					dst = canvas.ColorIndexAt(x+4, y) & ^(1 << plane)
					canvas.SetColorIndex(x+4, y, dst|(((dots>>3)&1)<<plane))

					dst = canvas.ColorIndexAt(x+5, y) & ^(1 << plane)
					canvas.SetColorIndex(x+5, y, dst|(((dots>>2)&1)<<plane))

					dst = canvas.ColorIndexAt(x+6, y) & ^(1 << plane)
					canvas.SetColorIndex(x+6, y, dst|(((dots>>1)&1)<<plane))

					dst = canvas.ColorIndexAt(x+7, y) & ^(1 << plane)
					canvas.SetColorIndex(x+7, y, dst|(((dots)&1)<<plane))
				}
			}
		}
	}

	// result
	result, err := doubleVert(canvas)
	if err != nil {
		panic(err)
	}

	fw, err := os.Create(outFile)
	if err != nil {
		panic(err)
	}
	defer fw.Close()

	err = gif.Encode(fw, result, &gif.Options{NumColors: len(palette)})
	if err != nil {
		panic(err)
	}
}

func loadHex(ihexFile string) ([]byte, error) {
	r, err := os.Open(ihexFile)
	if err != nil {
		return nil, err
	}
	defer r.Close()

	ihex := gohex.NewMemory()
	err = ihex.ParseIntelHex(r)
	if err != nil {
		return nil, err
	}

	var w bytes.Buffer
	i := 0
	for _, segment := range ihex.GetDataSegments() {
		b := int(segment.Address)
		e := b + len(segment.Data)
		data := segment.Data[0:]

		fmt.Printf("segment: %06x-%06x: %7d [bytes]\n", b, e, e-b)

		w.Write(data)
		i += len(data)
	}

	return w.Bytes(), nil
}

func fillZero(canvas *image.Paletted, plane PlaneType, roi *image.Rectangle) error {
	for t := roi.Min.Y; t < roi.Max.Y; t++ {
		for s := roi.Min.X; s < roi.Max.X; s++ {
			idx := canvas.ColorIndexAt(s, t)
			idx = idx & ^(1 << plane)
			canvas.SetColorIndex(s, t, idx)
		}
	}
	return nil
}

func doubleVert(canvas *image.Paletted) (*image.Paletted, error) {
	rect := image.Rectangle{
		Min: image.Point{canvas.Rect.Min.X, canvas.Rect.Min.Y * 2},
		Max: image.Point{canvas.Rect.Max.X, canvas.Rect.Max.Y * 2},
	}

	result := image.NewPaletted(rect, canvas.Palette)
	for t := canvas.Rect.Min.Y; t < canvas.Rect.Max.Y; t++ {
		for s := canvas.Rect.Min.X; s < canvas.Rect.Max.X; s++ {
			idx := canvas.ColorIndexAt(s, t)
			result.SetColorIndex(s, t*2+0, idx)
			result.SetColorIndex(s, t*2+1, idx)
		}
	}
	return result, nil
}
