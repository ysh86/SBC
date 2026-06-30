#pragma once

#include <stdbool.h>
#include <stdint.h>

enum {
    PPU_VRAM_SIZE = 16 * 1024,
    PPU_VRAM_MASK = PPU_VRAM_SIZE - 1,
    PPU_ADDRESS_MASK = 0x3fff,
    PPU_VRAM_BG = 0x2000,
    PPU_NAMETABLE_SIZE = 0x400,
    PPU_BG_INDEX_MASK = 0x03u,
    PPU_NMI_BIT = 0x80u,
    PPU_VBLANK_BIT = 0x80u,
    PPU_INC32_BIT = 0x04u,
    PPU_SHOW_BG_BIT = 0x08u,
    PPU_REGISTER_BASE = 0x2000,
    PPU_REGISTER_RANGE_MASK = 0xe000,
    PPU_REGISTER_COUNT = 8,
    PPU_VBLANK_BEGIN_COMMAND = 0x10000000u,
    PPU_VBLANK_END_COMMAND = 0x20000000u,
    PPU_WRITE_COMMAND = 0x40000000u,
    PPU_READ_COMMAND = 0x80000000u,
    PPU_COMMAND_INDEX_SHIFT = 8,
    PPU_COMMAND_INDEX_MASK = PPU_REGISTER_COUNT - 1,
    PPU_REG_PPUCTRL = 0,
    PPU_REG_PPUMASK = 1,
    PPU_REG_PPUSTATUS = 2,
    PPU_REG_OAMADDR = 3,
    PPU_REG_OAMDATA = 4,
    PPU_REG_PPUSCROLL = 5,
    PPU_REG_PPUADDR = 6,
    PPU_REG_PPUDATA = 7,
    PPU_SCROLL_X = 0,
    PPU_SCROLL_Y = 1,
    PPU_ADDR_H = 0,
    PPU_ADDR_L = 1,
};

typedef struct {
    uint8_t vram[PPU_VRAM_SIZE];
    uint8_t registers[PPU_REGISTER_COUNT];
    uint8_t scroll[2];
    uint8_t addr[2];
    bool write_latch;
} Ppu;

static volatile Ppu ppu = {
    .vram = { [0 ... (PPU_VRAM_SIZE / 2) - 1] = 0xff },
};

static const uint8_t ppu_addr_masks[2] = {
    [PPU_ADDR_H] = (uint8_t)(PPU_ADDRESS_MASK >> 8),
    [PPU_ADDR_L] = 0xff,
};

static inline __attribute__((always_inline)) uint32_t ppu_make_read_command(uint8_t index) {
    return PPU_READ_COMMAND | ((uint32_t)index << PPU_COMMAND_INDEX_SHIFT);
}

static inline __attribute__((always_inline)) uint32_t ppu_make_write_command(uint8_t index,
                                                                             uint8_t value) {
    return PPU_WRITE_COMMAND | ((uint32_t)index << PPU_COMMAND_INDEX_SHIFT) | value;
}

static inline __attribute__((always_inline)) uint16_t ppu_vram_addr(void) {
    return ((uint16_t)ppu.addr[PPU_ADDR_H] << 8) | ppu.addr[PPU_ADDR_L];
}

static inline __attribute__((always_inline)) void ppu_set_vram_addr(uint16_t address) {
    address &= PPU_ADDRESS_MASK;
    ppu.addr[PPU_ADDR_L] = (uint8_t)address;
    ppu.addr[PPU_ADDR_H] = (uint8_t)(address >> 8);
}

static inline __attribute__((always_inline)) bool ppu_nmi_output_is_low(void) {
    return (ppu.registers[PPU_REG_PPUSTATUS] & PPU_VBLANK_BIT) != 0u &&
           (ppu.registers[PPU_REG_PPUCTRL] & PPU_NMI_BIT) != 0u;
}

static inline __attribute__((always_inline, warn_unused_result)) bool ppu_vblank_begin(void) {
    ppu.registers[PPU_REG_PPUSTATUS] |= PPU_VBLANK_BIT;
    return ppu_nmi_output_is_low();
}

static inline __attribute__((always_inline)) void ppu_vblank_end(void) {
    ppu.registers[PPU_REG_PPUSTATUS] &= (uint8_t)~PPU_VBLANK_BIT;
}

static inline __attribute__((always_inline)) void ppu_read_register(uint8_t index) {
    if (index == PPU_REG_PPUDATA) {
        uint16_t vram_addr = ppu_vram_addr();
        ppu.registers[PPU_REG_PPUDATA] = ppu.vram[vram_addr & PPU_VRAM_MASK];
        ppu_set_vram_addr(vram_addr + ((ppu.registers[PPU_REG_PPUCTRL] & PPU_INC32_BIT) ? 32u : 1u));
    } else if (index == PPU_REG_PPUSTATUS) {
        ppu.registers[PPU_REG_PPUSTATUS] &= (uint8_t)~PPU_VBLANK_BIT;
        ppu.write_latch = false;
    }
}

static inline __attribute__((always_inline, warn_unused_result)) bool ppu_write_register(uint8_t index, uint8_t value) {
    ppu.registers[index] = value;

    switch (index) {
    case PPU_REG_PPUCTRL:
        break;
    case PPU_REG_PPUSCROLL: {
        uint8_t latch = ppu.write_latch;
        ppu.scroll[latch] = value;
        ppu.write_latch = !latch;
        break;
    }
    case PPU_REG_PPUADDR: {
        uint8_t latch = ppu.write_latch;
        ppu.addr[latch] = value & ppu_addr_masks[latch];
        ppu.write_latch = !latch;
        break;
    }
    case PPU_REG_PPUDATA: {
        uint16_t vram_addr = ppu_vram_addr();
        ppu.vram[vram_addr & PPU_VRAM_MASK] = value;
        ppu_set_vram_addr(vram_addr + ((ppu.registers[PPU_REG_PPUCTRL] & PPU_INC32_BIT) ? 32u : 1u));
        break;
    }
    default:
        break;
    }

    return ppu_nmi_output_is_low();
}

