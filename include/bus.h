#include <stdint.h>
#include "cpu.h"
#include "ppu.h"

struct BUS {
	uint8_t *mem;
	struct CPU *cpu;
	struct PPU *ppu;
	struct APU *apu;
	struct Cart *cart;
};

uint8_t bus_read8(struct BUS *bus, uint16_t addr);

