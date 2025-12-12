#include "cart.h"

struct __attribute__((packed)) cart_header {
	char magic[4];
	uint8_t prg_rom;
	uint8_t chr_rom;
	struct {
		uint8_t vertical: 1;
		uint8_t battery: 1;
		uint8_t trainer: 1;
		uint8_t four_screen: 1;
		uint8_t mapper_lo: 4;
		uint8_t format: 4;
		uint8_t mapper_hi: 4;
	};
	uint8_t tv_sys;
	uint8_t prg_ram;
	char padding[6];
};

void cart_init(struct cart *cart, char *path) {
	FILE *fp = fopen(path, "rb");
	if (fp == NULL) return;
	struct cart_header header;
	fread((char *)&header, sizeof(header), 1, fp);
	if (header.trainer) {
		cart->trainer = malloc(512);
		fread(cart->trainer, sizeof(512), 1, fp);
	}
	int prg_size = (header.prg_rom | !header.prg_rom) * 0x4000;
	int chr_size = (header.chr_rom | !header.chr_rom) * 0x2000;
	cart->prg_rom = malloc(prg_size);
	cart->chr_rom = malloc(chr_size);
	fread(cart->prg_rom, 1, prg_size, fp);
	fread(cart->chr_rom, 1, chr_size, fp);
	uint8_t mapper = (header.mapper_hi << 4) | header.mapper_lo;
	cart->mapper = mappers[mapper];
}

