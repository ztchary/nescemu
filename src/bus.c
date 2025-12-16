#include "bus.h"

void oam_dma(void) {}

uint8_t ppu_read(struct PPU *, uint8_t);
uint8_t apu_read(struct APU *);
uint8_t cart_read(struct cart *, uint16_t);

uint8_t bus_read_prg(struct BUS *bus, uint16_t addr) {
	if (addr <= 0x1fff) return bus->prg_ram[addr & 0x07ff];
	if (addr <= 0x3fff) return ppu_read(bus->ppu, addr & 0x0007);
	if (addr <= 0x4014) return 0;  // open bus
	if (addr == 0x4015) return apu_read(bus->apu);
	if (addr == 0x4016) return bus->joy1_state;
	if (addr == 0x4017) return bus->joy2_state;
	if (bus->cart != NULL) return bus->cart->mapper->read_prg(bus->cart, addr);
	return 0;
}

void ppu_write(struct PPU *, uint8_t, uint8_t);
void apu_write(struct APU *, uint8_t, uint8_t);
void cart_write(struct cart *, uint16_t, uint8_t);

void bus_write_prg(struct BUS *bus, uint16_t addr, uint8_t value) {
	     if (addr <= 0x1fff) bus->prg_ram[addr & 0x07ff] = value;
	else if (addr <= 0x3fff) ppu_write(bus->ppu, addr & 0x0007, value);
	else if (addr <= 0x4013) apu_write(bus->apu, addr & 0x001f, value);
	else if (addr == 0x4014) oam_dma();  // idk yet
	else if (addr == 0x4015) apu_write(bus->apu, addr & 0x1f, value);
	else if (addr == 0x4016) return;  // idk yet
	else if (addr == 0x4017) apu_write(bus->apu, addr & 0x1f, value);
	else if (bus->cart != NULL) bus->cart->mapper->write_prg(bus->cart, addr, value);
}

uint8_t bus_read_chr(struct BUS *bus, uint16_t addr) {
	return bus->chr_ram[addr & 0x1fff];
}

void bus_write_chr(struct BUS *bus, uint16_t addr, uint8_t value) {
	bus->chr_ram[addr & 0x1fff] = value;
}

