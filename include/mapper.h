#pragma once
#include <stdint.h>
#include "cart.h"

struct cart;

struct mapper {
	uint8_t (*read_prg)(struct cart *, uint16_t);
	void (*write_prg)(struct cart *, uint16_t, uint8_t);
	uint8_t (*read_chr)(struct cart *, uint16_t);
	void (*write_chr)(struct cart *, uint16_t, uint8_t);
};

extern struct mapper *mappers[];

extern struct mapper mapper_nrom;

