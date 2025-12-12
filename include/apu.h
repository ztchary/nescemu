#pragma once

struct APU {
	void *bus;
};

void apu_tick(struct APU *);

