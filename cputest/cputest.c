#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// this is bad news

struct BUS {
	uint8_t *data;
};

uint8_t bus_read(struct BUS *bus, uint16_t addr) {
	if (addr < 0x2000) return bus->data[addr & 0x7ff];
	return bus->data[addr];
}

void bus_write(struct BUS *bus, uint16_t addr, uint8_t value) {
	bus->data[addr] = value;
}

int main() {
	FILE *fp = fopen("nestest.nes", "rb");
	struct BUS bus = {.data = malloc(0x10000)};
	memset(bus.data, 0, 0x10000);
	fseek(fp, 16, SEEK_SET);
	fread(&bus.data[0xc000], 1, 0x4000, fp); // yikes
	fclose(fp);
	struct CPU cpu = {
		.reg_pc = 0xC000,
		.reg_sp = 0xFD,
		.reg_c.raw = 0x24,
		.bus = &bus,
		.bus_read = (uint8_t (*)(void *, uint16_t))bus_read,
		.bus_write = (void (*)(void *, uint16_t, uint8_t))bus_write,
	};
	for (int cycles = 7; cycles <= 26554;) {
		printf("%04X %02X A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%d\n", cpu.reg_pc, bus_read(&bus, cpu.reg_pc), cpu.reg_a, cpu.reg_x, cpu.reg_y, cpu.reg_c.raw, cpu.reg_sp, cycles);
		cycles += cpu_tick(&cpu);
	}
}
