#include <stdint.h>
#include "bus.h"

struct CPU {
	uint8_t reg_a;
	uint8_t reg_x;
	uint8_t reg_y;
	uint16_t reg_pc;
	uint8_t reg_sp;
	union {
		struct {
			int C: 1;
			int Z: 1;
			int I: 1;
			int D: 1;
			int B: 1;
			int U: 1;
			int V: 1;
			int N: 1;
		};
		uint8_t raw;
	} reg_c;
	struct BUS *bus;
	int inc_cycles;
};

enum CPU_ADDRMODE {
	CPU_ADDR_IMM,
	CPU_ADDR_IMM,
	CPU_ADDR_ACC,
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
};

struct cpu_instruction {
	enum CPU_ADDRMODE addrmode;
	void (*opf)(struct CPU *, enum CPU_ADDRMODE);
	int cycles;
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

struct cpu_instruction cpu_instruction_map[256] = {
	{CPU_ADDR_IMP, cpu_op_brk, 7}, // 0x00
	{CPU_ADDR_INX, cpu_op_ora, 6}, // 0x01
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0x02
	{CPU_ADDR_INX, cpu_op_slo, 8}, // 0x03
	{CPU_ADDR_ZPG, cpu_op_nop, 3}, // 0x04
	{CPU_ADDR_ZPG, cpu_op_ora, 3}, // 0x05
	{CPU_ADDR_ZPG, cpu_op_asl, 5}, // 0x06
	{CPU_ADDR_ZPG, cpu_op_slo, 5}, // 0x07
	{CPU_ADDR_IMP, cpu_op_php, 3}, // 0x08
	{CPU_ADDR_IMM, cpu_op_ora, 2}, // 0x09
	{CPU_ADDR_ACC, cpu_op_asl, 2}, // 0x0a
	{CPU_ADDR_IMM, cpu_op_anc, 2}, // 0x0b
	{CPU_ADDR_ABS, cpu_op_nop, 4}, // 0x0c
	{CPU_ADDR_ABS, cpu_op_ora, 4}, // 0x0d
	{CPU_ADDR_ABS, cpu_op_asl, 6}, // 0x0e
	{CPU_ADDR_ABS, cpu_op_slo, 6}, // 0x0f
	{CPU_ADDR_REL, cpu_op_bpl, 2}, // 0x10
	{CPU_ADDR_INY, cpu_op_ora, 5}, // 0x11
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0x12
	{CPU_ADDR_INY, cpu_op_slo, 8}, // 0x13
	{CPU_ADDR_ZPX, cpu_op_nop, 4}, // 0x14
	{CPU_ADDR_ZPX, cpu_op_ora, 4}, // 0x15
	{CPU_ADDR_ZPX, cpu_op_asl, 6}, // 0x16
	{CPU_ADDR_ZPX, cpu_op_slo, 6}, // 0x17
	{CPU_ADDR_IMP, cpu_op_clc, 2}, // 0x18
	{CPU_ADDR_ABY, cpu_op_ora, 4}, // 0x19
	{CPU_ADDR_IMP, cpu_op_nop, 2}, // 0x1a
	{CPU_ADDR_ABY, cpu_op_slo, 7}, // 0x1b
	{CPU_ADDR_ABX, cpu_op_nop, 4}, // 0x1c
	{CPU_ADDR_ABX, cpu_op_ora, 4}, // 0x1d
	{CPU_ADDR_ABX, cpu_op_asl, 7}, // 0x1e
	{CPU_ADDR_ABX, cpu_op_slo, 7}, // 0x1f
	{CPU_ADDR_ABS, cpu_op_jsr, 6}, // 0x20
	{CPU_ADDR_INX, cpu_op_and, 6}, // 0x21
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0x22
	{CPU_ADDR_INX, cpu_op_rla, 8}, // 0x23
	{CPU_ADDR_ZPG, cpu_op_bit, 3}, // 0x24
	{CPU_ADDR_ZPG, cpu_op_and, 3}, // 0x25
	{CPU_ADDR_ZPG, cpu_op_rol, 5}, // 0x26
	{CPU_ADDR_ZPG, cpu_op_rla, 5}, // 0x27
	{CPU_ADDR_IMP, cpu_op_plp, 4}, // 0x28
	{CPU_ADDR_IMM, cpu_op_and, 2}, // 0x29
	{CPU_ADDR_ACC, cpu_op_rol, 2}, // 0x2a
	{CPU_ADDR_IMM, cpu_op_anc, 2}, // 0x2b
	{CPU_ADDR_ABS, cpu_op_bit, 4}, // 0x2c
	{CPU_ADDR_ABS, cpu_op_and, 4}, // 0x2d
	{CPU_ADDR_ABS, cpu_op_rol, 6}, // 0x2e
	{CPU_ADDR_ABS, cpu_op_rla, 6}, // 0x2f
	{CPU_ADDR_REL, cpu_op_bmi, 2}, // 0x30
	{CPU_ADDR_INY, cpu_op_and, 5}, // 0x31
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0x32
	{CPU_ADDR_INY, cpu_op_rla, 8}, // 0x33
	{CPU_ADDR_ZPX, cpu_op_nop, 4}, // 0x34
	{CPU_ADDR_ZPX, cpu_op_and, 4}, // 0x35
	{CPU_ADDR_ZPX, cpu_op_rol, 6}, // 0x36
	{CPU_ADDR_ZPX, cpu_op_rla, 6}, // 0x37
	{CPU_ADDR_IMP, cpu_op_sec, 2}, // 0x38
	{CPU_ADDR_ABY, cpu_op_and, 4}, // 0x39
	{CPU_ADDR_IMP, cpu_op_nop, 2}, // 0x3a
	{CPU_ADDR_ABY, cpu_op_rla, 7}, // 0x3b
	{CPU_ADDR_ABX, cpu_op_nop, 4}, // 0x3c
	{CPU_ADDR_ABX, cpu_op_and, 4}, // 0x3d
	{CPU_ADDR_ABX, cpu_op_rol, 7}, // 0x3e
	{CPU_ADDR_ABX, cpu_op_rla, 7}, // 0x3f
	{CPU_ADDR_IMP, cpu_op_rti, 6}, // 0x40
	{CPU_ADDR_INX, cpu_op_eor, 6}, // 0x41
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0x42
	{CPU_ADDR_INX, cpu_op_sre, 8}, // 0x43
	{CPU_ADDR_ZPG, cpu_op_nop, 3}, // 0x44
	{CPU_ADDR_ZPG, cpu_op_eor, 3}, // 0x45
	{CPU_ADDR_ZPG, cpu_op_lsr, 5}, // 0x46
	{CPU_ADDR_ZPG, cpu_op_sre, 5}, // 0x47
	{CPU_ADDR_IMP, cpu_op_pha, 3}, // 0x48
	{CPU_ADDR_IMM, cpu_op_eor, 2}, // 0x49
	{CPU_ADDR_ACC, cpu_op_lsr, 2}, // 0x4a
	{CPU_ADDR_IMM, cpu_op_alr, 2}, // 0x4b
	{CPU_ADDR_ABS, cpu_op_jmp, 3}, // 0x4c
	{CPU_ADDR_ABS, cpu_op_eor, 4}, // 0x4d
	{CPU_ADDR_ABS, cpu_op_lsr, 6}, // 0x4e
	{CPU_ADDR_ABS, cpu_op_sre, 6}, // 0x4f
	{CPU_ADDR_REL, cpu_op_bvc, 2}, // 0x50
	{CPU_ADDR_INY, cpu_op_eor, 5}, // 0x51
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0x52
	{CPU_ADDR_INY, cpu_op_sre, 8}, // 0x53
	{CPU_ADDR_ZPX, cpu_op_nop, 4}, // 0x54
	{CPU_ADDR_ZPX, cpu_op_eor, 4}, // 0x55
	{CPU_ADDR_ZPX, cpu_op_lsr, 6}, // 0x56
	{CPU_ADDR_ZPX, cpu_op_sre, 6}, // 0x57
	{CPU_ADDR_IMP, cpu_op_cli, 2}, // 0x58
	{CPU_ADDR_ABY, cpu_op_eor, 4}, // 0x59
	{CPU_ADDR_IMP, cpu_op_nop, 2}, // 0x5a
	{CPU_ADDR_ABY, cpu_op_sre, 7}, // 0x5b
	{CPU_ADDR_ABX, cpu_op_nop, 4}, // 0x5c
	{CPU_ADDR_ABX, cpu_op_eor, 4}, // 0x5d
	{CPU_ADDR_ABX, cpu_op_lsr, 7}, // 0x5e
	{CPU_ADDR_ABX, cpu_op_sre, 7}, // 0x5f
	{CPU_ADDR_IMP, cpu_op_rts, 6}, // 0x60
	{CPU_ADDR_INX, cpu_op_adc, 6}, // 0x61
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0x62
	{CPU_ADDR_INX, cpu_op_rra, 8}, // 0x63
	{CPU_ADDR_ZPG, cpu_op_nop, 3}, // 0x64
	{CPU_ADDR_ZPG, cpu_op_adc, 3}, // 0x65
	{CPU_ADDR_ZPG, cpu_op_ror, 5}, // 0x66
	{CPU_ADDR_ZPG, cpu_op_rra, 5}, // 0x67
	{CPU_ADDR_IMP, cpu_op_pla, 4}, // 0x68
	{CPU_ADDR_IMM, cpu_op_adc, 2}, // 0x69
	{CPU_ADDR_ACC, cpu_op_ror, 2}, // 0x6a
	{CPU_ADDR_IMM, cpu_op_arr, 2}, // 0x6b
	{CPU_ADDR_IND, cpu_op_jmp, 5}, // 0x6c
	{CPU_ADDR_ABS, cpu_op_adc, 4}, // 0x6d
	{CPU_ADDR_ABS, cpu_op_ror, 6}, // 0x6e
	{CPU_ADDR_ABS, cpu_op_rra, 6}, // 0x6f
	{CPU_ADDR_REL, cpu_op_bvs, 2}, // 0x70
	{CPU_ADDR_INY, cpu_op_adc, 5}, // 0x71
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0x72
	{CPU_ADDR_INY, cpu_op_rra, 8}, // 0x73
	{CPU_ADDR_ZPX, cpu_op_nop, 4}, // 0x74
	{CPU_ADDR_ZPX, cpu_op_adc, 4}, // 0x75
	{CPU_ADDR_ZPX, cpu_op_ror, 6}, // 0x76
	{CPU_ADDR_ZPX, cpu_op_rra, 6}, // 0x77
	{CPU_ADDR_IMP, cpu_op_sei, 2}, // 0x78
	{CPU_ADDR_ABY, cpu_op_adc, 4}, // 0x79
	{CPU_ADDR_IMP, cpu_op_nop, 2}, // 0x7a
	{CPU_ADDR_ABY, cpu_op_rra, 7}, // 0x7b
	{CPU_ADDR_ABX, cpu_op_nop, 4}, // 0x7c
	{CPU_ADDR_ABX, cpu_op_adc, 4}, // 0x7d
	{CPU_ADDR_ABX, cpu_op_ror, 7}, // 0x7e
	{CPU_ADDR_ABX, cpu_op_rra, 7}, // 0x7f
	{CPU_ADDR_IMM, cpu_op_nop, 2}, // 0x80
	{CPU_ADDR_INX, cpu_op_sta, 6}, // 0x81
	{CPU_ADDR_IMM, cpu_op_nop, 2}, // 0x82
	{CPU_ADDR_INX, cpu_op_sax, 6}, // 0x83
	{CPU_ADDR_ZPG, cpu_op_sty, 3}, // 0x84
	{CPU_ADDR_ZPG, cpu_op_sta, 3}, // 0x85
	{CPU_ADDR_ZPG, cpu_op_stx, 3}, // 0x86
	{CPU_ADDR_ZPG, cpu_op_sax, 3}, // 0x87
	{CPU_ADDR_IMP, cpu_op_dey, 2}, // 0x88
	{CPU_ADDR_IMM, cpu_op_nop, 2}, // 0x89
	{CPU_ADDR_IMP, cpu_op_txa, 2}, // 0x8a
	{CPU_ADDR_IMM, cpu_op_ane, 2}, // 0x8b
	{CPU_ADDR_ABS, cpu_op_sty, 4}, // 0x8c
	{CPU_ADDR_ABS, cpu_op_sta, 4}, // 0x8d
	{CPU_ADDR_ABS, cpu_op_stx, 4}, // 0x8e
	{CPU_ADDR_ABS, cpu_op_sax, 4}, // 0x8f
	{CPU_ADDR_REL, cpu_op_bcc, 2}, // 0x90
	{CPU_ADDR_INY, cpu_op_sta, 6}, // 0x91
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0x92
	{CPU_ADDR_INY, cpu_op_sha, 6}, // 0x93
	{CPU_ADDR_ZPX, cpu_op_sty, 4}, // 0x94
	{CPU_ADDR_ZPX, cpu_op_sta, 4}, // 0x95
	{CPU_ADDR_ZPY, cpu_op_stx, 4}, // 0x96
	{CPU_ADDR_ZPY, cpu_op_sax, 4}, // 0x97
	{CPU_ADDR_IMP, cpu_op_tya, 2}, // 0x98
	{CPU_ADDR_ABY, cpu_op_sta, 5}, // 0x99
	{CPU_ADDR_IMP, cpu_op_txs, 2}, // 0x9a
	{CPU_ADDR_ABY, cpu_op_tas, 5}, // 0x9b
	{CPU_ADDR_ABX, cpu_op_shy, 5}, // 0x9c
	{CPU_ADDR_ABX, cpu_op_sta, 5}, // 0x9d
	{CPU_ADDR_ABX, cpu_op_shx, 5}, // 0x9e
	{CPU_ADDR_ABX, cpu_op_sha, 5}, // 0x9f
	{CPU_ADDR_IMM, cpu_op_ldy, 2}, // 0xa0
	{CPU_ADDR_INX, cpu_op_lda, 6}, // 0xa1
	{CPU_ADDR_IMM, cpu_op_ldx, 2}, // 0xa2
	{CPU_ADDR_INX, cpu_op_lax, 6}, // 0xa3
	{CPU_ADDR_ZPG, cpu_op_ldy, 3}, // 0xa4
	{CPU_ADDR_ZPG, cpu_op_lda, 3}, // 0xa5
	{CPU_ADDR_ZPG, cpu_op_ldx, 3}, // 0xa6
	{CPU_ADDR_ZPG, cpu_op_lax, 3}, // 0xa7
	{CPU_ADDR_IMP, cpu_op_tay, 2}, // 0xa8
	{CPU_ADDR_IMM, cpu_op_lda, 2}, // 0xa9
	{CPU_ADDR_IMP, cpu_op_tax, 2}, // 0xaa
	{CPU_ADDR_IMM, cpu_op_lxa, 2}, // 0xab
	{CPU_ADDR_ABS, cpu_op_ldy, 4}, // 0xac
	{CPU_ADDR_ABS, cpu_op_lda, 4}, // 0xad
	{CPU_ADDR_ABS, cpu_op_ldx, 4}, // 0xae
	{CPU_ADDR_ABS, cpu_op_lax, 4}, // 0xaf
	{CPU_ADDR_REL, cpu_op_bcs, 2}, // 0xb0
	{CPU_ADDR_INY, cpu_op_lda, 5}, // 0xb1
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0xb2
	{CPU_ADDR_INY, cpu_op_lax, 5}, // 0xb3
	{CPU_ADDR_ZPX, cpu_op_ldy, 4}, // 0xb4
	{CPU_ADDR_ZPX, cpu_op_lda, 4}, // 0xb5
	{CPU_ADDR_ZPY, cpu_op_ldx, 4}, // 0xb6
	{CPU_ADDR_ZPY, cpu_op_lax, 4}, // 0xb7
	{CPU_ADDR_IMP, cpu_op_clv, 2}, // 0xb8
	{CPU_ADDR_ABY, cpu_op_lda, 4}, // 0xb9
	{CPU_ADDR_IMP, cpu_op_tsx, 2}, // 0xba
	{CPU_ADDR_ABY, cpu_op_las, 4}, // 0xbb
	{CPU_ADDR_ABX, cpu_op_ldy, 4}, // 0xbc
	{CPU_ADDR_ABX, cpu_op_lda, 4}, // 0xbd
	{CPU_ADDR_ABY, cpu_op_ldx, 4}, // 0xbe
	{CPU_ADDR_ABY, cpu_op_lax, 4}, // 0xbf
	{CPU_ADDR_IMM, cpu_op_cpy, 2}, // 0xc0
	{CPU_ADDR_INX, cpu_op_cmp, 6}, // 0xc1
	{CPU_ADDR_IMM, cpu_op_nop, 2}, // 0xc2
	{CPU_ADDR_INX, cpu_op_dcp, 8}, // 0xc3
	{CPU_ADDR_ZPG, cpu_op_cpy, 3}, // 0xc4
	{CPU_ADDR_ZPG, cpu_op_cmp, 3}, // 0xc5
	{CPU_ADDR_ZPG, cpu_op_dec, 5}, // 0xc6
	{CPU_ADDR_ZPG, cpu_op_dcp, 5}, // 0xc7
	{CPU_ADDR_IMP, cpu_op_iny, 2}, // 0xc8
	{CPU_ADDR_IMM, cpu_op_cmp, 2}, // 0xc9
	{CPU_ADDR_IMP, cpu_op_dex, 2}, // 0xca
	{CPU_ADDR_IMM, cpu_op_sbx, 2}, // 0xcb
	{CPU_ADDR_ABS, cpu_op_cpy, 4}, // 0xcc
	{CPU_ADDR_ABS, cpu_op_cmp, 4}, // 0xcd
	{CPU_ADDR_ABS, cpu_op_dec, 6}, // 0xce
	{CPU_ADDR_ABS, cpu_op_dcp, 6}, // 0xcf
	{CPU_ADDR_REL, cpu_op_bne, 2}, // 0xd0
	{CPU_ADDR_INY, cpu_op_cmp, 5}, // 0xd1
	{CPU_ADDR_IMP, cpu_op_jam, 0}, // 0xd2
	{CPU_ADDR_INY, cpu_op_dcp, 8}, // 0xd3
	{CPU_ADDR_ZPX, cpu_op_nop, 4}, // 0xd4
	{CPU_ADDR_ZPX, cpu_op_cmp, 4}, // 0xd5
	{CPU_ADDR_ZPX, cpu_op_dec, 6}, // 0xd6
	{CPU_ADDR_ZPX, cpu_op_dcp, 6}, // 0xd7
	{CPU_ADDR_IMP, cpu_op_cld, 2}, // 0xd8
	{CPU_ADDR_ABY, cpu_op_cmp, 4}, // 0xd9
	{CPU_ADDR_IMP, cpu_op_nop, 2}, // 0xda
	{CPU_ADDR_ABY, cpu_op_dcp, 7}, // 0xdb
	{CPU_ADDR_ABX, cpu_op_nop, 4}, // 0xdc
	{CPU_ADDR_ABX, cpu_op_cmp, 4}, // 0xdd
	{CPU_ADDR_ABX, cpu_op_dec, 7}, // 0xde
	{CPU_ADDR_ABX, cpu_op_dcp, 7}, // 0xdf
	{CPU_ADDR_IMM, cpu_op_cpx, 2}, // 0xe0
	{CPU_ADDR_INX, cpu_op_sbc, 6}, // 0xe1
	{CPU_ADDR_IMM, cpu_op_nop, 2}, // 0xe2
	{CPU_ADDR_INX, cpu_op_isc, 8}, // 0xe3
	{CPU_ADDR_ZPG, cpu_op_cpx, 3}, // 0xe4
	{CPU_ADDR_ZPG, cpu_op_sbc, 3}, // 0xe5
	{CPU_ADDR_ZPG, cpu_op_inc, 5}, // 0xe6
	{CPU_ADDR_ZPG, cpu_op_isc, 5}, // 0xe7
	{CPU_ADDR_IMP, cpu_op_inx, 2}, // 0xe8
	{CPU_ADDR_IMM, cpu_op_sbc, 2}, // 0xe9
	{CPU_ADDR_IMP, cpu_op_nop, 2}, // 0xea
	{CPU_ADDR_IMM, cpu_op_usb, 2}, // 0xeb
	{CPU_ADDR_ABS, cpu_op_cpx, 4}, // 0xec
	{CPU_ADDR_ABS, cpu_op_sbc, 4}, // 0xed
	{CPU_ADDR_ABS, cpu_op_inc, 6}, // 0xee
	{CPU_ADDR_ABS, cpu_op_isc, 6}, // 0xef
	{CPU_ADDR_REL, cpu_op_beq, 2}, // 0xf0
	{CPU_ADDR_INY, cpu_op_sbc, 5}, // 0xf1
	{CPU_ADDR_IMP, cpu_op_nop, 0}, // 0xf2
	{CPU_ADDR_INY, cpu_op_isc, 8}, // 0xf3
	{CPU_ADDR_ZPX, cpu_op_nop, 4}, // 0xf4
	{CPU_ADDR_ZPX, cpu_op_sbc, 4}, // 0xf5
	{CPU_ADDR_ZPX, cpu_op_inc, 6}, // 0xf6
	{CPU_ADDR_ZPX, cpu_op_isc, 6}, // 0xf7
	{CPU_ADDR_IMP, cpu_op_sed, 2}, // 0xf8
	{CPU_ADDR_ABY, cpu_op_sbc, 4}, // 0xf9
	{CPU_ADDR_IMP, cpu_op_nop, 2}, // 0xfa
	{CPU_ADDR_ABY, cpu_op_isc, 7}, // 0xfb
	{CPU_ADDR_ABX, cpu_op_nop, 4}, // 0xfc
	{CPU_ADDR_ABX, cpu_op_sbc, 4}, // 0xfd
	{CPU_ADDR_ABX, cpu_op_inc, 7}, // 0xfe
	{CPU_ADDR_ABX, cpu_op_isc, 7}, // 0xff
};

