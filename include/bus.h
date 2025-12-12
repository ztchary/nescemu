#pragma once
#include <stdint.h>
#include <stdlib.h>

struct BUS {
	uint8_t *prg_ram;
	void *cpu;  // this is probably bad practice idk
	void *ppu;
	void *apu;
	void *cart;
	uint8_t (*ppu_read)(void *, uint8_t);
	void (*ppu_write)(void *, uint8_t, uint8_t);
	uint8_t (*apu_read)(void *);
	void (*apu_write)(void *, uint8_t, uint8_t);
	uint8_t (*cart_read)(void *, uint8_t);
	void (*cart_write)(void *, uint8_t, uint8_t);
	void (*oam_dma)(void);  // idk yet
	uint8_t joy1_state;  // definitely not happening
	uint8_t joy2_state;
};

uint8_t bus_read_prg(struct BUS *, uint16_t);
void bus_write_prg(struct BUS *, uint16_t, uint8_t);

uint8_t bus_read_chr(struct BUS *, uint16_t);
void bus_write_chr(struct BUS *, uint16_t, uint8_t);

