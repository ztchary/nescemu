#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "mapper.h"

enum CART_MIRRORING {
	CART_VERT,
	CART_HORI,
	CART_FOUR,
};

struct mapper;

struct cart {
	uint32_t prg_rom_size;
	uint32_t chr_rom_size;
	uint8_t *prg_rom;
	uint8_t *chr_rom;
	uint8_t *trainer;
	struct mapper *mapper;
	enum CART_MIRRORING mirroring;
};

void cart_init(struct cart *, char *);
void cart_destroy(struct cart *);

