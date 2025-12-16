#pragma once
#include "bus.h"
#include <stdint.h>

struct CPU {
	uint8_t reg_a;
	uint8_t reg_x;
	uint8_t reg_y;
	uint8_t reg_sp;
	uint16_t reg_pc;
	union {
		struct {
			uint8_t C: 1;
			uint8_t Z: 1;
			uint8_t I: 1;
			uint8_t D: 1;
			uint8_t B: 1;
			uint8_t U: 1;
			uint8_t V: 1;
			uint8_t N: 1;
		};
		uint8_t raw;
	} reg_c;
	void *bus;
	int inc_cycles;
	bool jammed;
};

int cpu_tick(struct CPU *);

