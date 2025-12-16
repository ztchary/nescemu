#include "bus.h"

uint8_t bus_read_prg(struct BUS *bus, uint16_t addr) {
	return bus->prg_ram[addr];
}

void bus_write_prg(struct BUS *bus, uint16_t addr, uint8_t value) {
	bus->prg_ram[addr] = value;
}

