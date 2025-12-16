#pragma once

#include <stdint.h>

struct APU {
	void *bus;
};

void apu_tick(struct APU *);

uint8_t apu_read(struct APU *);
void apu_write(struct APU *, uint8_t, uint8_t);

