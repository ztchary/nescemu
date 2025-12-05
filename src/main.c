#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "bus.h"

int main() {
	struct CPU cpu = {0};
	struct PPU ppu = {0};
	struct APU apu = {0};
	struct BUS bus = {0};
	bus.cpu = &cpu;
	cpu.bus = &bus;
	bus.ppu = &ppu;
	ppu.bus = &bus;
	bus.apu = &apu;
	apu.bus = &bus;
	for (;;) {
		int cycles = cpu_tick(&cpu);
		for (int i = 0; i < cycles * 3; i++) {
			if (i % 6 == 0) apu_tick(&apu);
			ppu_tick(&ppu);
		}
	}
}
