#pragma once

struct PPU {
	void *bus;
};

void ppu_tick(struct PPU *);

