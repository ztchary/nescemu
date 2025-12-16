#pragma once

#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "cart.h"

struct BUS {
	uint8_t *prg_ram;
	uint8_t *chr_ram;
	struct CPU *cpu;
	struct PPU *ppu;
	struct APU *apu;
	struct cart *cart;
	uint8_t joy1_state;  // definitely not happening
	uint8_t joy2_state;
};

uint8_t bus_read_prg(struct BUS *, uint16_t);
void bus_write_prg(struct BUS *, uint16_t, uint8_t);

uint8_t bus_read_chr(struct BUS *, uint16_t);
void bus_write_chr(struct BUS *, uint16_t, uint8_t);

