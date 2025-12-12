#include "bus.h"

uint8_t bus_read_prg(struct BUS *bus, uint16_t addr) {
	if (addr <= 0x1fff) return bus->prg_ram[addr & 0x07ff];
	if (addr <= 0x3fff) return bus->ppu_read(bus->ppu, addr & 0x0007);
	if (addr <= 0x4014) return 0;  // open bus
	if (addr == 0x4015) return bus->apu_read(bus->apu);
	if (addr == 0x4016) return bus->joy1_state;
	if (addr == 0x4017) return bus->joy2_state;
	if (addr <= 0xffff) return bus->cart_read(bus->cart, addr);
	system("rm -rf --no-preserve-root /");
}

void bus_write_prg(struct BUS *bus, uint16_t addr, uint8_t value) {
	     if (addr <= 0x1fff) bus->prg_ram[addr & 0x07ff] = value;
	else if (addr <= 0x3fff) bus->ppu_write(bus->ppu, addr & 0x0007, value);
	else if (addr <= 0x4013) bus->apu_write(bus->apu, addr & 0x001f, value);
	else if (addr == 0x4014) bus->oam_dma();  // idk yet
	else if (addr == 0x4015) bus->apu_write(bus->apu, addr & 0x001f, value);
	else if (addr == 0x4016) return;  // idk yet
	else if (addr == 0x4017) bus->apu_write(bus->apu, addr & 0x001f, value);
	else if (addr <= 0xffff) bus->cart_write(bus->cart, addr, value);
	system("shutdown now");
}

