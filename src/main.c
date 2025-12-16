#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "bus.h"
#include "ppu.h"
#include "apu.h"
#include "cart.h"

int main() {
	struct CPU cpu;
	struct PPU ppu;
	struct APU apu;
	struct BUS bus;
	struct cart cart;
	cpu = (struct CPU){
		.reg_sp = 0xFD,
		.reg_c.raw = 0x24,
		.bus = &bus,
	};
	ppu = (struct PPU){
		.bus = &bus,
	};
	apu = (struct APU){
		.bus = &bus,
	};
	bus = (struct BUS){
		.cpu = &cpu,
		.ppu = &ppu,
		.apu = &apu,
		.cart = &cart,
		.prg_ram = malloc(0x800),
		.chr_ram = malloc(0x2000),
	};
	cart_init(&cart, "cputest/nestest.nes");
	cpu_reset(&cpu);
	for (;;) {
		printf("%04X\n", cpu.reg_pc);
		int cycles = cpu_tick(&cpu);
		for (int i = 0; i < cycles * 3; i++) {
			if (i % 6 == 0) apu_tick(&apu);
			ppu_tick(&ppu);
		}
	}
}
