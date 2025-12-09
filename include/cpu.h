#pragma once
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
	uint8_t (*bus_read)(void *, uint16_t);
	void (*bus_write)(void *, uint16_t, uint8_t);
	int inc_cycles;
};

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

int cpu_tick(struct CPU *);

void cpu_op_adc(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_alr(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_anc(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_and(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_ane(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_arr(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_asl(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_bcc(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_bcs(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_beq(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_bit(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_bmi(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_bne(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_bpl(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_brk(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_bvc(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_bvs(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_clc(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_cld(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_cli(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_clv(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_cmp(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_cpx(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_cpy(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_dcp(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_dec(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_dex(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_dey(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_eor(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_inc(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_inx(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_iny(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_isc(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_jam(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_jmp(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_jsr(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_las(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_lax(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_lda(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_ldx(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_ldy(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_lsr(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_lxa(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_nop(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_ora(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_pha(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_php(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_pla(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_plp(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_rla(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_rol(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_ror(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_rra(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_rti(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_rts(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sax(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sbc(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sbx(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sec(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sed(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sei(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sha(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_shx(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_shy(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_slo(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sre(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sta(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_stx(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_sty(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_tas(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_tax(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_tay(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_tsx(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_txa(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_txs(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_tya(struct CPU *, enum CPU_ADDRMODE);
void cpu_op_usb(struct CPU *, enum CPU_ADDRMODE);

