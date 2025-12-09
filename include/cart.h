#pragma once
#include <stdio.h>
#include <stdint.h>

struct cart_header {
	char magic[4];
	uint8_t prg_rom;
	uint8_t chr_rom;
	uint8_t ctrl1;
	uint8_t ctrl2;
	uint8_t tv_sys;
	uint8_t prg_ram;
	char padding[6];
};

void read_prg_rom(char *path, uint8_t *data) {
	FILE *fp = fopen(path, "rb");
	struct cart_header header;
	fread(header, sizeof(header), 1, fp);
}

