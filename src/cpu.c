#include <stdint.h>
#include "cpu.h"

uint16_t cpu_get_opaddr(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	switch (addrmode) {
	case CPU_ADDR_IMM:
		return (uint16_t)cpu->pc++;
	case CPU_ADDR_REL:
		int8_t off = (int8_t)bus_read(cpu->bus, cpu->pc++);
		return (uint16_t)(cpu->pc + off);
	case CPU_ADDR_ZPG:
		return bus_read(cpu->bus, cpu->pc++);
	case CPU_ADDR_ZPX:
		return (uint8_t)(bus_read(cpu->bus, cpu->pc++) + cpu->reg_x);
	case CPU_ADDR_ZPY:
		return (uint8_t)(bus_read(cpu->bus, cpu->pc++) + cpu->reg_y);
	case CPU_ADDR_ABS:
		uint8_t lo = bus_read(cpu->bus, cpu->pc++);
		uint8_t hi = bus_read(cpu->bus, cpu->pc++);
		return hi << 8 | lo;
	case CPU_ADDR_ABX:
		uint8_t lo = bus_read(cpu->bus, cpu->pc++);
		uint8_t hi = bus_read(cpu->bus, cpu->pc++);
		cpu->page_crossed = lo + cpu->reg_x >> 8;
		return (uint16_t)((hi << 8 | lo) + cpu->reg_x);
	case CPU_ADDR_ABY:
		uint8_t lo = bus_read(cpu->bus, cpu->pc++);
		uint8_t hi = bus_read(cpu->bus, cpu->pc++);
		cpu->page_crossed = lo + cpu->reg_y >> 8;
		return (uint16_t)((hi << 8 | lo) + cpu->reg_y);
	case CPU_ADDR_IND:
		uint8_t ptrlo = bus_read(cpu->bus, cpu->pc++);
		uint8_t ptrhi = bus_read(cpu->bus, cpu->pc++);
		uint8_t lo = bus_read(cpu->bus, ptrhi << 8 | ptrlo++);
		uint8_t hi = bus_read(cpu->bus, ptrhi << 8 | ptrlo++);
		return hi << 8 | lo;
	case CPU_ADDR_INX:
		uint8_t ptr = bus_read(cpu->bus, cpu->pc++) + cpu->reg_x;
		uint8_t lo = bus_read(cpu->bus, ptr++);
		uint8_t hi = bus_read(cpu->bus, ptr++);
		return hi << 8 | lo;
	case CPU_ADDR_INY:
		uint8_t ptr = bus_read(cpu->bus, cpu->pc++);
		uint8_t lo = bus_read(cpu->bus, ptr++);
		uint8_t hi = bus_read(cpu->bus, ptr++);
		cpu->page_crossed = (lo + cpu->reg_y) >> 8;
		return (hi << 8 | lo) + cpu->reg_y;
	}
	return 0;
}

int cpu_tick(struct CPU *cpu) {
	uint8_t op = bus_read(cpu->bus, cpu->pc++);
	struct cpu_instruction inst = cpu_instruction_map[op];
	cpu->inc_cycles = 0;
	inst.opf(cpu, inst.addrmode);
	return inst.cycles + cpu->inc_cycles;
}

void cpu_push(struct CPU *cpu, uint8_t value) {
	bus_write(cpu->bus, (cpu->sp--) | 0x100, value);
}

uint8_t cpu_pull(struct CPU *cpu) {
	bus_read(cpu->bus, (++cpu->sp) | 0x100);
}

void cpu_push16(struct CPU *cpu, uint16_t value) {
	cpu_push(cpu, value >> 8);
	cpu_push(cpu, value & 0xff);
}

uint16_t cpu_pull16(struct CPU *cpu) {
	uint16_t lo = cpu_pull(cpu);
	uint16_t hi = cpu_pull(cpu);
	return (hi << 8) | lo;
}

void cpu_set_zn(struct CPU *cpu, uint8_t value) {
	cpu->reg_c.Z = value == 0;
	cpu->reg_c.N = value >> 7;
}

uint8_t cpu_get_status(struct CPU *cpu, uint8_t value, bool b) {
	return (cpu->reg_c.raw & 0b11101111) | 0b00100000 | (b ? 0 : 0b00010000);
}

void cpu_set_status(struct CPU *cpu, uint8_t value) {
	cpu->reg_c.raw = (value & 0b11101111) | 0b00100000;
}

void cpu_add_acc(struct CPU *cpu, uint8_t value) {
	int x = cpu->reg_a + value + cpu->reg_c.C;
	cpu->reg_c.C = x >> 8;
	cpu->reg_c.V = (~(cpu->reg_a ^ value) & (cpu->reg_a ^ x)) >> 7;
	cpu->reg_a = x;
	cpu_set_zn(cpu, cpu->reg_a);
}

void cpu_branch(struct CPU *cpu, bool cond) {
	int8_t off = bus_read(cpu->bus, cpu->reg_pc++);
	if (!cond) return;
	cpu->inc_cycles++;
	uint16_t target = cpu->reg_pc + off;
	if ((off ^ cpu->reg_pc) >> 8) cpu->inc_cycles++;
	cpu->reg_pc = target;
}

void cpu_op_adc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu_add_acc(cpu, bus_read(cpu->bus, addr));
}

void cpu_op_and(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a &= bus_read(cpu->bus, addr);
	cpu_set_zn(cpu, cpu->reg_a);
}

void cpu_op_asl(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	if (addrmode == CPU_ADDR_ACC) {
		cpu->reg_c.C = cpu->reg_a >> 7;
		cpu->reg_a <<= 1;
		cpu_set_zn(cpu, cpu->reg_a);
		return;
	}
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr);
	cpu->reg_c.C = val >> 7;
	val <<= 1;
	bus_write(cpu->bus, addr, val);
	cpu_set_zn(cpu->bus, val);
}

void cpu_op_bcc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu_branch(cpu, !cpu->reg_c.C);
}

void cpu_op_bcs(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu_branch(cpu, cpu->reg_c.C);
}

void cpu_op_beq(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu_branch(cpu, cpu->reg_c.Z);
}

void cpu_op_bit(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr);
	cpu->reg_c.Z = !(cou->reg_a & val);
	cpu->reg_c.N = !(val & 0x80);
	cpu->reg_c.V = !(val & 0x40);
	
}

void cpu_op_bne(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu_branch(cpu, !cpu->reg_c.Z);
}

void cpu_op_bmi(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu_branch(cpu, cpu->reg_c.N);
}

void cpu_op_bpl(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu_branch(cpu, !cpu->reg_c.N);
}

void cpu_op_brk(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_push16(cpu, cpu->reg_pc + 1);
	cpu_push(cpu, cpu->reg_c.raw | 0b00110000);
	uint8_t lo = bus_read(cpu->bus, 0xFFFC);
	uint8_t hi = bus_read(cpu->bus, 0xFFFD);
	cpu->reg_pc = hi << 8 | lo;
}

void cpu_op_bvc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu_branch(cpu, !cpu->reg_c.V);
}

void cpu_op_bvs(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu_branch(cpu, cpu->reg_c.V);
}

void cpu_op_clc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu->reg_c.C = 0;
}

void cpu_op_cld(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu->reg_c.D = 0;
}

void cpu_op_cli(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu->reg_c.I = 0;
}

void cpu_op_clv(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)addrmode;
	cpu->reg_c.V = 0;
}

void cpu_op_cmp(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr);
	cpu->reg_c.C = cpu->reg_a >= val;
	cpu_set_zn(cpu, cpu->reg_a - val);
}

void cpu_op_cpx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr);
	cpu->reg_c.C = cpu->reg_x >= val;
	cpu_set_zn(cpu, cpu->reg_x - val);
}

void cpu_op_cpy(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr);
	cpu->reg_c.C = cpu->reg_y >= val;
	cpu_set_zn(cpu, cpu->reg_y - val);
}

void cpu_op_dec(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr) - 1;
	bus_write(cpu->bus, addr, val);
	cpu_set_zn(cpu, val);
}

void cpu_op_dex(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_set_zn(cpu, cpu->reg_x--);
}

void cpu_op_dey(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_set_zn(cpu, cpu->reg_y--);
}

void cpu_op_eor(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a ^= bus_read(cpu->bus, addr);
	cpu_set_zn(cpu, cpu->reg_a);
}

void cpu_op_inc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr) + 1;
	bus_write(cpu->bus, addr, val);
	cpu_set_zn(cpu, val);
}

void cpu_op_inx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_set_zn(cpu, cpu->reg_x++);
}

void cpu_op_iny(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_set_zn(cpu, cpu->reg_y++);
}

void cpu_op_jmp(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_pc = cpu_get_opaddr(addrmode);
}

void cpu_op_jsr(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_push16(cpu, cpu->reg_pc + 1);
	cpu->reg_pc = cpu_get_opaddr(addrmode);
}

void cpu_op_lda(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(addrmode);
	cpu->reg_a = bus_read(cpu->bus, addr);
	cpu_set_zn(cpu, cpu->reg_a);
}

void cpu_op_ldx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(addrmode);
	cpu->reg_x = bus_read(cpu->bus, addr);
	cpu_set_zn(cpu, cpu->reg_x);
}

void cpu_op_ldy(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(addrmode);
	cpu->reg_y = bus_read(cpu->bus, addr);
	cpu_set_zn(cpu, cpu->reg_y);
}

void cpu_op_lsr(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	if (addrmode == CPU_ADDR_ACC) {
		cpu->reg_c.C = cpu->reg_a;
		cpu->reg_a >>= 1;
		cpu_set_zn(cpu, cpu->reg_a);
		return;
	}
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr);
	cpu->reg_c.C = val;
	val >>= 1;
	bus_write(cpu->bus, addr, val);
	cpu_set_zn(cpu->bus, val);
}

void cpu_op_nop(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)cpu_get_opaddr(addrmode);
}

void cpu_op_ora(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a |= bus_read(cpu->bus, addr);
	cpu_set_zn(cpu, cpu->reg_a);
}

void cpu_op_pha(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_push(cpu, cpu->reg_a);
}

void cpu_op_php(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_push(cpu, cpu->reg_c.raw | 0b00110000);
}

void cpu_op_pla(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_a = cpu_pull(cpu);
	cpu_set_zn(cpu, cpu->reg_a);
}

void cpu_op_plp(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_c.raw = cpu_pull(cpu) & 0b11101111 | 0b00100000;
}

void cpu_op_rol(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint8_t c = cpu->reg_c.C;
	if (addrmode == CPU_ADDR_ACC) {
		cpu->reg_c.C = cpu->reg_a >> 7;
		cpu->reg_a = (cpu->reg_a << 1) | c;
		cpu_set_zn(cpu, cpu->reg_a);
		return;
	}
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr);
	cpu->reg_c.C = val >> 7;
	val = (val << 1) | c;
	bus_write(cpu->bus, addr, val);
	cpu_set_zn(cpu, val);
}

void cpu_op_rol(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint8_t c = cpu->reg_c.C << 7;
	if (addrmode == CPU_ADDR_ACC) {
		cpu->reg_c.C = cpu->reg_a;
		cpu->reg_a = (cpu->reg_a >> 1) | c;
		cpu_set_zn(cpu, cpu->reg_a);
		return;
	}
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = bus_read(cpu->bus, addr);
	cpu->reg_c.C = val;
	val = (val >> 1) | c;
	bus_write(cpu->bus, addr, val);
	cpu_set_zn(cpu, val);
}

void cpu_op_rti(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_c.raw = cpu_pull(cpu) & 0b11101111 | 0b00100000;
	cpu->reg_pc = cpu_pull16(cpu);
}

void cpu_op_rts(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_pc = cpu_pull16(cpu) + 1;
}

void cpu_op_sbc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu_add_acc(cpu, ~bus_read(cpu->bus, addr));
}

void cpu_op_sec(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_c.C = 1;
}

void cpu_op_sed(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_c.D = 1;
}

void cpu_op_sei(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_c.I = 1;
}

void cpu_op_sta(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	bus_write(cpu->bus, cpu_get_opaddr(cpu, addrmode), cpu->reg_a);
}

void cpu_op_stx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	bus_write(cpu->bus, cpu_get_opaddr(cpu, addrmode), cpu->reg_x);
}

void cpu_op_sty(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	bus_write(cpu->bus, cpu_get_opaddr(cpu, addrmode), cpu->reg_y);
}

void cpu_op_tax(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_x = cpu->reg_a;
	cpu_set_zn(cpu, cpu->reg_x);
}

void cpu_op_tay(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_y = cpu->reg_a;
	cpu_set_zn(cpu, cpu->reg_y);
}

void cpu_op_tsx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_x = cpu->reg_sp;
	cpu_set_zn(cpu, cpu->reg_x);
}

void cpu_op_txa(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_a = cpu->reg_x;
	cpu_set_zn(cpu, cpu->reg_a);
}

void cpu_op_txs(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_sp = cpu->reg_x;
}

void cpu_op_tya(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_a = cpu->reg_y;
	cpu_set_zn(cpu, cpu->reg_a);
}

