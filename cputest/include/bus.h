#pragma once
#include "cpu.h"
#include <stdint.h>

struct BUS {
	uint8_t *prg_ram;
	struct CPU *cpu;
};

uint8_t bus_read_prg(struct BUS *, uint16_t);
void bus_write_prg(struct BUS *, uint16_t, uint8_t);

