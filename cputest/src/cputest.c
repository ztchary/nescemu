#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// this is bad news

int main(int argc, char **argv) {
	if (argc != 2) return 1;
	struct BUS bus = {.prg_ram = malloc(0x10000)};
	memset(bus.prg_ram, 0, 0x10000);
// yikes {
	FILE *fp = fopen(argv[1], "rb");
	if (fp == NULL) return 1;
	fseek(fp, 16, SEEK_SET);
	fread(&bus.prg_ram[0x8000], 1, 0x4000, fp);
	fclose(fp);
	memcpy(&bus.prg_ram[0xc000], &bus.prg_ram[0x8000], 0x4000);
// }
	struct CPU cpu = {
		.reg_pc = 0xC000,
		.reg_sp = 0xFD,
		.reg_c.raw = 0x24,
		.bus = &bus,
	};
	for (int cycles = 7; cycles <= 26554;) {
		printf("%04X %02X A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%d\n", cpu.reg_pc, bus_read_prg(&bus, cpu.reg_pc), cpu.reg_a, cpu.reg_x, cpu.reg_y, cpu.reg_c.raw, cpu.reg_sp, cycles);
		cycles += cpu_tick(&cpu);
	}
}
