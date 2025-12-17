#pragma once

#include <stdint.h>

#include "bus.h"

struct PPU {
	union {
		struct {
			uint8_t nametable:   2;
			uint8_t addr_inc:    1;
			uint8_t spritetable: 1;
			uint8_t bgtable:     1;
			uint8_t spritesize:  1;
			uint8_t master:      1;
			uint8_t vblank_nmi:  1;
		};
		uint8_t raw;
	} reg_ctrl;
	struct {
		union {
			uint8_t grayscale:      1;
			uint8_t crop_bg:        1;
			uint8_t crop_sprites:   1;
			uint8_t enable_bg:      1;
			uint8_t enable_sprites: 1;
			uint8_t red:            1;
			uint8_t green:          1;
			uint8_t blue:           1;
		};
		uint8_t raw;
	} reg_mask;
	uint8_t scroll_x;
	uint8_t scroll_y;
	struct BUS *bus;
};

void ppu_tick(struct PPU *);

uint8_t ppu_read(struct PPU *, uint8_t);
void ppu_write(struct PPU *, uint8_t, uint8_t);

