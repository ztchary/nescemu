#include "cpu.h"

#define cpu_read(addr) cpu->bus_read(cpu->bus, addr)
#define cpu_write(addr, value) cpu->bus_write(cpu->bus, addr, value)

#define cpu_set_zn(value) uint8_t v = value; cpu->reg_c.Z = v == 0; cpu->reg_c.N = v >> 7

#define cpu_push(value) cpu_write((cpu->reg_sp--) | 0x100, value)
#define cpu_pull() cpu_read((++cpu->reg_sp) | 0x100)

#define cpu_push16(value) uint16_t v = value; cpu_push(v >> 8); cpu_push(v & 0xff)
#define cpu_pull16() (cpu_pull() | (cpu_pull() << 8))

enum CPU_ADDRMODE {
	CPU_ADDR_IMM,
	CPU_ADDR_REL,
	CPU_ADDR_ZPG,
	CPU_ADDR_ZPX,
	CPU_ADDR_ZPY,
	CPU_ADDR_ABS,
	CPU_ADDR_ABX,
	CPU_ADDR_ABY,
	CPU_ADDR_IND,
	CPU_ADDR_INX,
	CPU_ADDR_INY,
	CPU_ADDR_IMP,
	CPU_ADDR_ACC,
};

struct cpu_instruction {
	enum CPU_ADDRMODE addrmode;
	void (*opf)(struct CPU *, enum CPU_ADDRMODE);
	int cycles;
	bool inc_cross;
};

extern struct cpu_instruction cpu_instruction_map[];

int cpu_tick(struct CPU *cpu) {
	uint8_t op = cpu_read(cpu->reg_pc++);
	struct cpu_instruction inst = cpu_instruction_map[op];
	cpu->inc_cycles = 0;
	inst.opf(cpu, inst.addrmode);
	return inst.cycles + inst.inc_cross * cpu->inc_cycles;
}

uint16_t cpu_get_opaddr(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint8_t lo, hi, ptr, ptrhi;
	switch (addrmode) {
	case CPU_ADDR_IMM:
		return (uint16_t)cpu->reg_pc++;
	case CPU_ADDR_REL:
		int8_t off = (int8_t)cpu_read(cpu->reg_pc++);
		return (uint16_t)(cpu->reg_pc + off);
	case CPU_ADDR_ZPG:
		return cpu_read(cpu->reg_pc++);
	case CPU_ADDR_ZPX:
		return (uint8_t)(cpu_read(cpu->reg_pc++) + cpu->reg_x);
	case CPU_ADDR_ZPY:
		return (uint8_t)(cpu_read(cpu->reg_pc++) + cpu->reg_y);
	case CPU_ADDR_ABS:
		lo = cpu_read(cpu->reg_pc++);
		hi = cpu_read(cpu->reg_pc++);
		return hi << 8 | lo;
	case CPU_ADDR_ABX:
		lo = cpu_read(cpu->reg_pc++);
		hi = cpu_read(cpu->reg_pc++);
		cpu->inc_cycles = (lo + cpu->reg_x) >> 8;
		return (uint16_t)((hi << 8 | lo) + cpu->reg_x);
	case CPU_ADDR_ABY:
		lo = cpu_read(cpu->reg_pc++);
		hi = cpu_read(cpu->reg_pc++);
		cpu->inc_cycles = (lo + cpu->reg_y) >> 8;
		return (uint16_t)((hi << 8 | lo) + cpu->reg_y);
	case CPU_ADDR_IND:
		ptr = cpu_read(cpu->reg_pc++);
		ptrhi = cpu_read(cpu->reg_pc++);
		lo = cpu_read(ptrhi << 8 | ptr++);
		hi = cpu_read(ptrhi << 8 | ptr++);
		return hi << 8 | lo;
	case CPU_ADDR_INX:
		ptr = cpu_read(cpu->reg_pc++) + cpu->reg_x;
		lo = cpu_read(ptr++);
		hi = cpu_read(ptr++);
		return hi << 8 | lo;
	case CPU_ADDR_INY:
		ptr = cpu_read(cpu->reg_pc++);
		lo = cpu_read(ptr++);
		hi = cpu_read(ptr++);
		cpu->inc_cycles = (lo + cpu->reg_y) >> 8;
		return (hi << 8 | lo) + cpu->reg_y;
	default:
		return 0;
	}
}

void cpu_add_acc(struct CPU *cpu, uint8_t value) {
	int x = cpu->reg_a + value + cpu->reg_c.C;
	cpu->reg_c.C = x >> 8;
	cpu->reg_c.V = ((value ^ x) & (cpu->reg_a ^ x)) >> 7;
	cpu->reg_a = x;
	cpu_set_zn(cpu->reg_a);
}

void cpu_branch(struct CPU *cpu, bool cond) {
	int8_t off = cpu_read(cpu->reg_pc++);
	if (!cond) return;
	cpu->inc_cycles++;
	uint16_t target = cpu->reg_pc + off;
	if ((target ^ cpu->reg_pc) >> 8) cpu->inc_cycles++;
	cpu->reg_pc = target;
}

void cpu_op_adc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu_add_acc(cpu, cpu_read(addr));
}

void cpu_op_and(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a &= cpu_read(addr);
	cpu_set_zn(cpu->reg_a);
}

void cpu_op_asl(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	if (addrmode == CPU_ADDR_ACC) {
		cpu->reg_c.C = cpu->reg_a >> 7;
		cpu->reg_a <<= 1;
		cpu_set_zn(cpu->reg_a);
		return;
	}
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	cpu->reg_c.C = val >> 7;
	val <<= 1;
	cpu_write(addr, val);
	cpu_set_zn(val);
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
	uint8_t val = cpu_read(addr);
	cpu->reg_c.Z = !(cpu->reg_a & val);
	cpu->reg_c.N = !!(val & 0x80);
	cpu->reg_c.V = !!(val & 0x40);
	
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
	cpu_push16(cpu->reg_pc + 1);
	cpu_push(cpu->reg_c.raw | 0b00110000);
	uint8_t lo = cpu_read(0xFFFC);
	uint8_t hi = cpu_read(0xFFFD);
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
	uint8_t val = cpu_read(addr);
	cpu->reg_c.C = cpu->reg_a >= val;
	cpu_set_zn(cpu->reg_a - val);
}

void cpu_op_cpx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	cpu->reg_c.C = cpu->reg_x >= val;
	cpu_set_zn(cpu->reg_x - val);
}

void cpu_op_cpy(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	cpu->reg_c.C = cpu->reg_y >= val;
	cpu_set_zn(cpu->reg_y - val);
}

void cpu_op_dec(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr) - 1;
	cpu_write(addr, val);
	cpu_set_zn(val);
}

void cpu_op_dex(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_set_zn(--cpu->reg_x);
}

void cpu_op_dey(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_set_zn(--cpu->reg_y);
}

void cpu_op_eor(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a ^= cpu_read(addr);
	cpu_set_zn(cpu->reg_a);
}

void cpu_op_inc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr) + 1;
	cpu_write(addr, val);
	cpu_set_zn(val);
}

void cpu_op_inx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_set_zn(++cpu->reg_x);
}

void cpu_op_iny(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_set_zn(++cpu->reg_y);
}

void cpu_op_jmp(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_pc = cpu_get_opaddr(cpu, addrmode);
}

void cpu_op_jsr(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_push16(cpu->reg_pc + 1);
	cpu->reg_pc = cpu_get_opaddr(cpu, addrmode);
}

void cpu_op_lda(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a = cpu_read(addr);
	cpu_set_zn(cpu->reg_a);
}

void cpu_op_ldx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_x = cpu_read(addr);
	cpu_set_zn(cpu->reg_x);
}

void cpu_op_ldy(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_y = cpu_read(addr);
	cpu_set_zn(cpu->reg_y);
}

void cpu_op_lsr(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	if (addrmode == CPU_ADDR_ACC) {
		cpu->reg_c.C = cpu->reg_a;
		cpu->reg_a >>= 1;
		cpu_set_zn(cpu->reg_a);
		return;
	}
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	cpu->reg_c.C = val;
	val >>= 1;
	cpu_write(addr, val);
	cpu_set_zn(val);
}

void cpu_op_nop(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)cpu_get_opaddr(cpu, addrmode);
}

void cpu_op_ora(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a |= cpu_read(addr);
	cpu_set_zn(cpu->reg_a);
}

void cpu_op_pha(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_push(cpu->reg_a);
}

void cpu_op_php(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_push(cpu->reg_c.raw | 0b00110000);
}

void cpu_op_pla(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_a = cpu_pull();
	cpu_set_zn(cpu->reg_a);
}

void cpu_op_plp(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_c.raw = (cpu_pull() & 0b11101111) | 0b00100000;
}

void cpu_op_rol(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint8_t c = cpu->reg_c.C;
	if (addrmode == CPU_ADDR_ACC) {
		cpu->reg_c.C = cpu->reg_a >> 7;
		cpu->reg_a = (cpu->reg_a << 1) | c;
		cpu_set_zn(cpu->reg_a);
		return;
	}
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	cpu->reg_c.C = val >> 7;
	val = (val << 1) | c;
	cpu_write(addr, val);
	cpu_set_zn(val);
}

void cpu_op_ror(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint8_t c = cpu->reg_c.C << 7;
	if (addrmode == CPU_ADDR_ACC) {
		cpu->reg_c.C = cpu->reg_a;
		cpu->reg_a = (cpu->reg_a >> 1) | c;
		cpu_set_zn(cpu->reg_a);
		return;
	}
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	cpu->reg_c.C = val;
	val = (val >> 1) | c;
	cpu_write(addr, val);
	cpu_set_zn(val);
}

void cpu_op_rti(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_c.raw = (cpu_pull() & 0b11101111) | 0b00100000;
	cpu->reg_pc = cpu_pull16();
}

void cpu_op_rts(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_pc = cpu_pull16() + 1;
}

void cpu_op_sbc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu_add_acc(cpu, ~cpu_read(addr));
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
	cpu_write(cpu_get_opaddr(cpu, addrmode), cpu->reg_a);
}

void cpu_op_stx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_write(cpu_get_opaddr(cpu, addrmode), cpu->reg_x);
}

void cpu_op_sty(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_write(cpu_get_opaddr(cpu, addrmode), cpu->reg_y);
}

void cpu_op_tax(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_x = cpu->reg_a;
	cpu_set_zn(cpu->reg_x);
}

void cpu_op_tay(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_y = cpu->reg_a;
	cpu_set_zn(cpu->reg_y);
}

void cpu_op_tsx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_x = cpu->reg_sp;
	cpu_set_zn(cpu->reg_x);
}

void cpu_op_txa(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_a = cpu->reg_x;
	cpu_set_zn(cpu->reg_a);
}

void cpu_op_txs(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_sp = cpu->reg_x;
}

void cpu_op_tya(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->reg_a = cpu->reg_y;
	cpu_set_zn(cpu->reg_a);
}

// @trevor do these for me

void cpu_op_alr(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a &= cpu_read(addr);
	cpu->reg_c.C = cpu->reg_a;
	cpu->reg_a >>= 1;
	cpu_set_zn(cpu->reg_a);
}

void cpu_op_anc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a &= cpu_read(addr);
	cpu_set_zn(cpu->reg_a);
	cpu->reg_c.C = cpu->reg_c.N;
};

void cpu_op_ane(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)cpu_get_opaddr(cpu, addrmode);
};

void cpu_op_arr(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a &= cpu_read(addr);
	cpu->reg_a = (cpu->reg_a >> 1) | (cpu->reg_c.C << 7);
	cpu->reg_c.C = cpu->reg_a >> 6;
	cpu->reg_c.V = ((cpu->reg_a >> 1) ^ (cpu->reg_a)) >> 5;
};

void cpu_op_dcp(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr) - 1;
	cpu_write(addr, val);
	cpu_set_zn(cpu->reg_a - val);
};

void cpu_op_isc(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr) + 1;
	cpu_write(addr, val);
	cpu_add_acc(cpu, ~val);
};

void cpu_op_jam(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu->jammed = true;
};

void cpu_op_las(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a = cpu->reg_x = cpu->reg_sp = cpu->reg_sp & cpu_read(addr);
	cpu_set_zn(cpu->reg_a);
};

void cpu_op_lax(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a = cpu->reg_x = cpu_read(addr);
	cpu_set_zn(cpu->reg_a);
};

void cpu_op_lxa(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_a = cpu->reg_x = cpu_read(addr);
	cpu_set_zn(cpu->reg_a);
};

void cpu_op_rla(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	uint8_t c = cpu->reg_c.C;
	cpu->reg_c.C = val >> 7;
	val = (val << 1) | c;
	cpu_write(addr, val);
	cpu->reg_a &= val;
	cpu_set_zn(cpu->reg_a);
};

void cpu_op_rra(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	uint8_t c = cpu->reg_c.C << 7;
	cpu->reg_c.C = val;
	val = (val >> 1) | c;
	cpu_write(addr, val);
	cpu_add_acc(cpu, val);
};

void cpu_op_sax(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu_write(addr, cpu->reg_a & cpu->reg_x);
};

void cpu_op_sbx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu->reg_x = (cpu->reg_a & cpu->reg_x) - cpu_read(addr);
	cpu->reg_c.C = !(cpu->reg_x >> 8);
	cpu_set_zn(cpu->reg_x);
};

void cpu_op_sha(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu_write(addr, cpu->reg_a);
};

void cpu_op_shx(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu_write(addr, cpu->reg_x);
};

void cpu_op_shy(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	cpu_write(addr, cpu->reg_y);
};

void cpu_op_slo(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	cpu->reg_c.C = val >> 7;
	val <<= 1;
	cpu_write(addr, val);
	cpu->reg_a |= val;
	cpu_set_zn(cpu->reg_a);
};

void cpu_op_sre(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	uint16_t addr = cpu_get_opaddr(cpu, addrmode);
	uint8_t val = cpu_read(addr);
	cpu->reg_c.C = val;
	val >>= 1;
	cpu_write(addr, val);
	cpu->reg_a ^= val;
	cpu_set_zn(cpu->reg_a);
};

void cpu_op_tas(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	(void)cpu_get_opaddr(cpu, addrmode);
};

void cpu_op_usb(struct CPU *cpu, enum CPU_ADDRMODE addrmode) {
	cpu_op_sbc(cpu, addrmode);
};

struct cpu_instruction cpu_instruction_map[256] = {
	{CPU_ADDR_IMP, cpu_op_brk, 7, 0}, // 0x00
	{CPU_ADDR_INX, cpu_op_ora, 6, 0}, // 0x01
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0x02
	{CPU_ADDR_INX, cpu_op_slo, 8, 0}, // 0x03
	{CPU_ADDR_ZPG, cpu_op_nop, 3, 0}, // 0x04
	{CPU_ADDR_ZPG, cpu_op_ora, 3, 0}, // 0x05
	{CPU_ADDR_ZPG, cpu_op_asl, 5, 0}, // 0x06
	{CPU_ADDR_ZPG, cpu_op_slo, 5, 0}, // 0x07
	{CPU_ADDR_IMP, cpu_op_php, 3, 0}, // 0x08
	{CPU_ADDR_IMM, cpu_op_ora, 2, 0}, // 0x09
	{CPU_ADDR_ACC, cpu_op_asl, 2, 0}, // 0x0a
	{CPU_ADDR_IMM, cpu_op_anc, 2, 0}, // 0x0b
	{CPU_ADDR_ABS, cpu_op_nop, 4, 0}, // 0x0c
	{CPU_ADDR_ABS, cpu_op_ora, 4, 0}, // 0x0d
	{CPU_ADDR_ABS, cpu_op_asl, 6, 0}, // 0x0e
	{CPU_ADDR_ABS, cpu_op_slo, 6, 0}, // 0x0f
	{CPU_ADDR_REL, cpu_op_bpl, 2, 1}, // 0x10
	{CPU_ADDR_INY, cpu_op_ora, 5, 1}, // 0x11
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0x12
	{CPU_ADDR_INY, cpu_op_slo, 8, 0}, // 0x13
	{CPU_ADDR_ZPX, cpu_op_nop, 4, 0}, // 0x14
	{CPU_ADDR_ZPX, cpu_op_ora, 4, 0}, // 0x15
	{CPU_ADDR_ZPX, cpu_op_asl, 6, 0}, // 0x16
	{CPU_ADDR_ZPX, cpu_op_slo, 6, 0}, // 0x17
	{CPU_ADDR_IMP, cpu_op_clc, 2, 0}, // 0x18
	{CPU_ADDR_ABY, cpu_op_ora, 4, 1}, // 0x19
	{CPU_ADDR_IMP, cpu_op_nop, 2, 0}, // 0x1a
	{CPU_ADDR_ABY, cpu_op_slo, 7, 0}, // 0x1b
	{CPU_ADDR_ABX, cpu_op_nop, 4, 1}, // 0x1c
	{CPU_ADDR_ABX, cpu_op_ora, 4, 1}, // 0x1d
	{CPU_ADDR_ABX, cpu_op_asl, 7, 0}, // 0x1e
	{CPU_ADDR_ABX, cpu_op_slo, 7, 0}, // 0x1f
	{CPU_ADDR_ABS, cpu_op_jsr, 6, 0}, // 0x20
	{CPU_ADDR_INX, cpu_op_and, 6, 0}, // 0x21
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0x22
	{CPU_ADDR_INX, cpu_op_rla, 8, 0}, // 0x23
	{CPU_ADDR_ZPG, cpu_op_bit, 3, 0}, // 0x24
	{CPU_ADDR_ZPG, cpu_op_and, 3, 0}, // 0x25
	{CPU_ADDR_ZPG, cpu_op_rol, 5, 0}, // 0x26
	{CPU_ADDR_ZPG, cpu_op_rla, 5, 0}, // 0x27
	{CPU_ADDR_IMP, cpu_op_plp, 4, 0}, // 0x28
	{CPU_ADDR_IMM, cpu_op_and, 2, 0}, // 0x29
	{CPU_ADDR_ACC, cpu_op_rol, 2, 0}, // 0x2a
	{CPU_ADDR_IMM, cpu_op_anc, 2, 0}, // 0x2b
	{CPU_ADDR_ABS, cpu_op_bit, 4, 0}, // 0x2c
	{CPU_ADDR_ABS, cpu_op_and, 4, 0}, // 0x2d
	{CPU_ADDR_ABS, cpu_op_rol, 6, 0}, // 0x2e
	{CPU_ADDR_ABS, cpu_op_rla, 6, 0}, // 0x2f
	{CPU_ADDR_REL, cpu_op_bmi, 2, 1}, // 0x30
	{CPU_ADDR_INY, cpu_op_and, 5, 1}, // 0x31
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0x32
	{CPU_ADDR_INY, cpu_op_rla, 8, 0}, // 0x33
	{CPU_ADDR_ZPX, cpu_op_nop, 4, 0}, // 0x34
	{CPU_ADDR_ZPX, cpu_op_and, 4, 0}, // 0x35
	{CPU_ADDR_ZPX, cpu_op_rol, 6, 0}, // 0x36
	{CPU_ADDR_ZPX, cpu_op_rla, 6, 0}, // 0x37
	{CPU_ADDR_IMP, cpu_op_sec, 2, 0}, // 0x38
	{CPU_ADDR_ABY, cpu_op_and, 4, 1}, // 0x39
	{CPU_ADDR_IMP, cpu_op_nop, 2, 0}, // 0x3a
	{CPU_ADDR_ABY, cpu_op_rla, 7, 0}, // 0x3b
	{CPU_ADDR_ABX, cpu_op_nop, 4, 1}, // 0x3c
	{CPU_ADDR_ABX, cpu_op_and, 4, 1}, // 0x3d
	{CPU_ADDR_ABX, cpu_op_rol, 7, 0}, // 0x3e
	{CPU_ADDR_ABX, cpu_op_rla, 7, 0}, // 0x3f
	{CPU_ADDR_IMP, cpu_op_rti, 6, 0}, // 0x40
	{CPU_ADDR_INX, cpu_op_eor, 6, 0}, // 0x41
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0x42
	{CPU_ADDR_INX, cpu_op_sre, 8, 0}, // 0x43
	{CPU_ADDR_ZPG, cpu_op_nop, 3, 0}, // 0x44
	{CPU_ADDR_ZPG, cpu_op_eor, 3, 0}, // 0x45
	{CPU_ADDR_ZPG, cpu_op_lsr, 5, 0}, // 0x46
	{CPU_ADDR_ZPG, cpu_op_sre, 5, 0}, // 0x47
	{CPU_ADDR_IMP, cpu_op_pha, 3, 0}, // 0x48
	{CPU_ADDR_IMM, cpu_op_eor, 2, 0}, // 0x49
	{CPU_ADDR_ACC, cpu_op_lsr, 2, 0}, // 0x4a
	{CPU_ADDR_IMM, cpu_op_alr, 2, 0}, // 0x4b
	{CPU_ADDR_ABS, cpu_op_jmp, 3, 0}, // 0x4c
	{CPU_ADDR_ABS, cpu_op_eor, 4, 0}, // 0x4d
	{CPU_ADDR_ABS, cpu_op_lsr, 6, 0}, // 0x4e
	{CPU_ADDR_ABS, cpu_op_sre, 6, 0}, // 0x4f
	{CPU_ADDR_REL, cpu_op_bvc, 2, 1}, // 0x50
	{CPU_ADDR_INY, cpu_op_eor, 5, 1}, // 0x51
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0x52
	{CPU_ADDR_INY, cpu_op_sre, 8, 0}, // 0x53
	{CPU_ADDR_ZPX, cpu_op_nop, 4, 0}, // 0x54
	{CPU_ADDR_ZPX, cpu_op_eor, 4, 0}, // 0x55
	{CPU_ADDR_ZPX, cpu_op_lsr, 6, 0}, // 0x56
	{CPU_ADDR_ZPX, cpu_op_sre, 6, 0}, // 0x57
	{CPU_ADDR_IMP, cpu_op_cli, 2, 0}, // 0x58
	{CPU_ADDR_ABY, cpu_op_eor, 4, 1}, // 0x59
	{CPU_ADDR_IMP, cpu_op_nop, 2, 0}, // 0x5a
	{CPU_ADDR_ABY, cpu_op_sre, 7, 0}, // 0x5b
	{CPU_ADDR_ABX, cpu_op_nop, 4, 1}, // 0x5c
	{CPU_ADDR_ABX, cpu_op_eor, 4, 1}, // 0x5d
	{CPU_ADDR_ABX, cpu_op_lsr, 7, 0}, // 0x5e
	{CPU_ADDR_ABX, cpu_op_sre, 7, 0}, // 0x5f
	{CPU_ADDR_IMP, cpu_op_rts, 6, 0}, // 0x60
	{CPU_ADDR_INX, cpu_op_adc, 6, 0}, // 0x61
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0x62
	{CPU_ADDR_INX, cpu_op_rra, 8, 0}, // 0x63
	{CPU_ADDR_ZPG, cpu_op_nop, 3, 0}, // 0x64
	{CPU_ADDR_ZPG, cpu_op_adc, 3, 0}, // 0x65
	{CPU_ADDR_ZPG, cpu_op_ror, 5, 0}, // 0x66
	{CPU_ADDR_ZPG, cpu_op_rra, 5, 0}, // 0x67
	{CPU_ADDR_IMP, cpu_op_pla, 4, 0}, // 0x68
	{CPU_ADDR_IMM, cpu_op_adc, 2, 0}, // 0x69
	{CPU_ADDR_ACC, cpu_op_ror, 2, 0}, // 0x6a
	{CPU_ADDR_IMM, cpu_op_arr, 2, 0}, // 0x6b
	{CPU_ADDR_IND, cpu_op_jmp, 5, 1}, // 0x6c
	{CPU_ADDR_ABS, cpu_op_adc, 4, 0}, // 0x6d
	{CPU_ADDR_ABS, cpu_op_ror, 6, 0}, // 0x6e
	{CPU_ADDR_ABS, cpu_op_rra, 6, 0}, // 0x6f
	{CPU_ADDR_REL, cpu_op_bvs, 2, 1}, // 0x70
	{CPU_ADDR_INY, cpu_op_adc, 5, 1}, // 0x71
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0x72
	{CPU_ADDR_INY, cpu_op_rra, 8, 0}, // 0x73
	{CPU_ADDR_ZPX, cpu_op_nop, 4, 0}, // 0x74
	{CPU_ADDR_ZPX, cpu_op_adc, 4, 0}, // 0x75
	{CPU_ADDR_ZPX, cpu_op_ror, 6, 0}, // 0x76
	{CPU_ADDR_ZPX, cpu_op_rra, 6, 0}, // 0x77
	{CPU_ADDR_IMP, cpu_op_sei, 2, 0}, // 0x78
	{CPU_ADDR_ABY, cpu_op_adc, 4, 1}, // 0x79
	{CPU_ADDR_IMP, cpu_op_nop, 2, 0}, // 0x7a
	{CPU_ADDR_ABY, cpu_op_rra, 7, 0}, // 0x7b
	{CPU_ADDR_ABX, cpu_op_nop, 4, 1}, // 0x7c
	{CPU_ADDR_ABX, cpu_op_adc, 4, 1}, // 0x7d
	{CPU_ADDR_ABX, cpu_op_ror, 7, 0}, // 0x7e
	{CPU_ADDR_ABX, cpu_op_rra, 7, 0}, // 0x7f
	{CPU_ADDR_IMM, cpu_op_nop, 2, 0}, // 0x80
	{CPU_ADDR_INX, cpu_op_sta, 6, 0}, // 0x81
	{CPU_ADDR_IMM, cpu_op_nop, 2, 0}, // 0x82
	{CPU_ADDR_INX, cpu_op_sax, 6, 0}, // 0x83
	{CPU_ADDR_ZPG, cpu_op_sty, 3, 0}, // 0x84
	{CPU_ADDR_ZPG, cpu_op_sta, 3, 0}, // 0x85
	{CPU_ADDR_ZPG, cpu_op_stx, 3, 0}, // 0x86
	{CPU_ADDR_ZPG, cpu_op_sax, 3, 0}, // 0x87
	{CPU_ADDR_IMP, cpu_op_dey, 2, 0}, // 0x88
	{CPU_ADDR_IMM, cpu_op_nop, 2, 0}, // 0x89
	{CPU_ADDR_IMP, cpu_op_txa, 2, 0}, // 0x8a
	{CPU_ADDR_IMM, cpu_op_ane, 2, 0}, // 0x8b
	{CPU_ADDR_ABS, cpu_op_sty, 4, 0}, // 0x8c
	{CPU_ADDR_ABS, cpu_op_sta, 4, 0}, // 0x8d
	{CPU_ADDR_ABS, cpu_op_stx, 4, 0}, // 0x8e
	{CPU_ADDR_ABS, cpu_op_sax, 4, 0}, // 0x8f
	{CPU_ADDR_REL, cpu_op_bcc, 2, 1}, // 0x90
	{CPU_ADDR_INY, cpu_op_sta, 6, 0}, // 0x91
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0x92
	{CPU_ADDR_INY, cpu_op_sha, 6, 0}, // 0x93
	{CPU_ADDR_ZPX, cpu_op_sty, 4, 0}, // 0x94
	{CPU_ADDR_ZPX, cpu_op_sta, 4, 0}, // 0x95
	{CPU_ADDR_ZPY, cpu_op_stx, 4, 0}, // 0x96
	{CPU_ADDR_ZPY, cpu_op_sax, 4, 0}, // 0x97
	{CPU_ADDR_IMP, cpu_op_tya, 2, 0}, // 0x98
	{CPU_ADDR_ABY, cpu_op_sta, 5, 0}, // 0x99
	{CPU_ADDR_IMP, cpu_op_txs, 2, 0}, // 0x9a
	{CPU_ADDR_ABY, cpu_op_tas, 5, 0}, // 0x9b
	{CPU_ADDR_ABX, cpu_op_shy, 5, 0}, // 0x9c
	{CPU_ADDR_ABX, cpu_op_sta, 5, 0}, // 0x9d
	{CPU_ADDR_ABX, cpu_op_shx, 5, 0}, // 0x9e
	{CPU_ADDR_ABX, cpu_op_sha, 5, 0}, // 0x9f
	{CPU_ADDR_IMM, cpu_op_ldy, 2, 0}, // 0xa0
	{CPU_ADDR_INX, cpu_op_lda, 6, 0}, // 0xa1
	{CPU_ADDR_IMM, cpu_op_ldx, 2, 0}, // 0xa2
	{CPU_ADDR_INX, cpu_op_lax, 6, 0}, // 0xa3
	{CPU_ADDR_ZPG, cpu_op_ldy, 3, 0}, // 0xa4
	{CPU_ADDR_ZPG, cpu_op_lda, 3, 0}, // 0xa5
	{CPU_ADDR_ZPG, cpu_op_ldx, 3, 0}, // 0xa6
	{CPU_ADDR_ZPG, cpu_op_lax, 3, 0}, // 0xa7
	{CPU_ADDR_IMP, cpu_op_tay, 2, 0}, // 0xa8
	{CPU_ADDR_IMM, cpu_op_lda, 2, 0}, // 0xa9
	{CPU_ADDR_IMP, cpu_op_tax, 2, 0}, // 0xaa
	{CPU_ADDR_IMM, cpu_op_lxa, 2, 0}, // 0xab
	{CPU_ADDR_ABS, cpu_op_ldy, 4, 0}, // 0xac
	{CPU_ADDR_ABS, cpu_op_lda, 4, 0}, // 0xad
	{CPU_ADDR_ABS, cpu_op_ldx, 4, 0}, // 0xae
	{CPU_ADDR_ABS, cpu_op_lax, 4, 0}, // 0xaf
	{CPU_ADDR_REL, cpu_op_bcs, 2, 1}, // 0xb0
	{CPU_ADDR_INY, cpu_op_lda, 5, 1}, // 0xb1
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0xb2
	{CPU_ADDR_INY, cpu_op_lax, 5, 1}, // 0xb3
	{CPU_ADDR_ZPX, cpu_op_ldy, 4, 0}, // 0xb4
	{CPU_ADDR_ZPX, cpu_op_lda, 4, 0}, // 0xb5
	{CPU_ADDR_ZPY, cpu_op_ldx, 4, 0}, // 0xb6
	{CPU_ADDR_ZPY, cpu_op_lax, 4, 0}, // 0xb7
	{CPU_ADDR_IMP, cpu_op_clv, 2, 0}, // 0xb8
	{CPU_ADDR_ABY, cpu_op_lda, 4, 1}, // 0xb9
	{CPU_ADDR_IMP, cpu_op_tsx, 2, 0}, // 0xba
	{CPU_ADDR_ABY, cpu_op_las, 4, 0}, // 0xbb
	{CPU_ADDR_ABX, cpu_op_ldy, 4, 1}, // 0xbc
	{CPU_ADDR_ABX, cpu_op_lda, 4, 1}, // 0xbd
	{CPU_ADDR_ABY, cpu_op_ldx, 4, 1}, // 0xbe
	{CPU_ADDR_ABY, cpu_op_lax, 4, 1}, // 0xbf
	{CPU_ADDR_IMM, cpu_op_cpy, 2, 0}, // 0xc0
	{CPU_ADDR_INX, cpu_op_cmp, 6, 0}, // 0xc1
	{CPU_ADDR_IMM, cpu_op_nop, 2, 0}, // 0xc2
	{CPU_ADDR_INX, cpu_op_dcp, 8, 0}, // 0xc3
	{CPU_ADDR_ZPG, cpu_op_cpy, 3, 0}, // 0xc4
	{CPU_ADDR_ZPG, cpu_op_cmp, 3, 0}, // 0xc5
	{CPU_ADDR_ZPG, cpu_op_dec, 5, 0}, // 0xc6
	{CPU_ADDR_ZPG, cpu_op_dcp, 5, 0}, // 0xc7
	{CPU_ADDR_IMP, cpu_op_iny, 2, 0}, // 0xc8
	{CPU_ADDR_IMM, cpu_op_cmp, 2, 0}, // 0xc9
	{CPU_ADDR_IMP, cpu_op_dex, 2, 0}, // 0xca
	{CPU_ADDR_IMM, cpu_op_sbx, 2, 0}, // 0xcb
	{CPU_ADDR_ABS, cpu_op_cpy, 4, 0}, // 0xcc
	{CPU_ADDR_ABS, cpu_op_cmp, 4, 0}, // 0xcd
	{CPU_ADDR_ABS, cpu_op_dec, 6, 0}, // 0xce
	{CPU_ADDR_ABS, cpu_op_dcp, 6, 0}, // 0xcf
	{CPU_ADDR_REL, cpu_op_bne, 2, 1}, // 0xd0
	{CPU_ADDR_INY, cpu_op_cmp, 5, 1}, // 0xd1
	{CPU_ADDR_IMP, cpu_op_jam, 0, 0}, // 0xd2
	{CPU_ADDR_INY, cpu_op_dcp, 8, 0}, // 0xd3
	{CPU_ADDR_ZPX, cpu_op_nop, 4, 0}, // 0xd4
	{CPU_ADDR_ZPX, cpu_op_cmp, 4, 0}, // 0xd5
	{CPU_ADDR_ZPX, cpu_op_dec, 6, 0}, // 0xd6
	{CPU_ADDR_ZPX, cpu_op_dcp, 6, 0}, // 0xd7
	{CPU_ADDR_IMP, cpu_op_cld, 2, 0}, // 0xd8
	{CPU_ADDR_ABY, cpu_op_cmp, 4, 1}, // 0xd9
	{CPU_ADDR_IMP, cpu_op_nop, 2, 0}, // 0xda
	{CPU_ADDR_ABY, cpu_op_dcp, 7, 0}, // 0xdb
	{CPU_ADDR_ABX, cpu_op_nop, 4, 1}, // 0xdc
	{CPU_ADDR_ABX, cpu_op_cmp, 4, 1}, // 0xdd
	{CPU_ADDR_ABX, cpu_op_dec, 7, 0}, // 0xde
	{CPU_ADDR_ABX, cpu_op_dcp, 7, 0}, // 0xdf
	{CPU_ADDR_IMM, cpu_op_cpx, 2, 0}, // 0xe0
	{CPU_ADDR_INX, cpu_op_sbc, 6, 0}, // 0xe1
	{CPU_ADDR_IMM, cpu_op_nop, 2, 0}, // 0xe2
	{CPU_ADDR_INX, cpu_op_isc, 8, 0}, // 0xe3
	{CPU_ADDR_ZPG, cpu_op_cpx, 3, 0}, // 0xe4
	{CPU_ADDR_ZPG, cpu_op_sbc, 3, 0}, // 0xe5
	{CPU_ADDR_ZPG, cpu_op_inc, 5, 0}, // 0xe6
	{CPU_ADDR_ZPG, cpu_op_isc, 5, 0}, // 0xe7
	{CPU_ADDR_IMP, cpu_op_inx, 2, 0}, // 0xe8
	{CPU_ADDR_IMM, cpu_op_sbc, 2, 0}, // 0xe9
	{CPU_ADDR_IMP, cpu_op_nop, 2, 0}, // 0xea
	{CPU_ADDR_IMM, cpu_op_usb, 2, 0}, // 0xeb
	{CPU_ADDR_ABS, cpu_op_cpx, 4, 0}, // 0xec
	{CPU_ADDR_ABS, cpu_op_sbc, 4, 0}, // 0xed
	{CPU_ADDR_ABS, cpu_op_inc, 6, 0}, // 0xee
	{CPU_ADDR_ABS, cpu_op_isc, 6, 0}, // 0xef
	{CPU_ADDR_REL, cpu_op_beq, 2, 1}, // 0xf0
	{CPU_ADDR_INY, cpu_op_sbc, 5, 1}, // 0xf1
	{CPU_ADDR_IMP, cpu_op_nop, 0, 0}, // 0xf2
	{CPU_ADDR_INY, cpu_op_isc, 8, 0}, // 0xf3
	{CPU_ADDR_ZPX, cpu_op_nop, 4, 0}, // 0xf4
	{CPU_ADDR_ZPX, cpu_op_sbc, 4, 0}, // 0xf5
	{CPU_ADDR_ZPX, cpu_op_inc, 6, 0}, // 0xf6
	{CPU_ADDR_ZPX, cpu_op_isc, 6, 0}, // 0xf7
	{CPU_ADDR_IMP, cpu_op_sed, 2, 0}, // 0xf8
	{CPU_ADDR_ABY, cpu_op_sbc, 4, 1}, // 0xf9
	{CPU_ADDR_IMP, cpu_op_nop, 2, 0}, // 0xfa
	{CPU_ADDR_ABY, cpu_op_isc, 7, 0}, // 0xfb
	{CPU_ADDR_ABX, cpu_op_nop, 4, 1}, // 0xfc
	{CPU_ADDR_ABX, cpu_op_sbc, 4, 1}, // 0xfd
	{CPU_ADDR_ABX, cpu_op_inc, 7, 0}, // 0xfe
	{CPU_ADDR_ABX, cpu_op_isc, 7, 0}, // 0xff
};

