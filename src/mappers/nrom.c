#include "mapper.h"
#include "cart.h"

uint8_t mapper_nrom_read_prg(struct cart *cart, uint16_t addr) {
	if (addr < 0x8000) return 0;
	return cart->prg_rom[addr % cart->prg_rom_size];
}

void mapper_nrom_write_prg(struct cart *cart, uint16_t addr, uint8_t value) {
	(void)addr;
	(void)value;
}

uint8_t mapper_nrom_read_chr(struct cart *cart, uint16_t addr) {
	return cart->prg_rom[addr & 0x1fff];
}

void mapper_nrom_write_chr(struct cart *cart, uint16_t addr, uint8_t value) {
	(void)addr;
	(void)value;
}

struct mapper mapper_nrom = {
	mapper_nrom_read_prg,
	mapper_nrom_write_prg,
	mapper_nrom_read_chr,
	mapper_nrom_write_chr,
};

