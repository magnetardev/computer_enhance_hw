#include "mov.h"
#include <stdio.h>

const uint8_t I8_SIGNED_BIT = 0b10000000;
const uint16_t I16_SIGNED_BIT = 0b1000000000000000;

uint16_t read_immediate(struct ReaderCtx* ctx, Width is_word)
{
	size_t pos = ctx->i;
	ctx->i += is_word + 1;
	if (is_word) {
		uint8_t data_lo = ctx->buf[pos];
		uint8_t data_hi = ctx->buf[pos + 1];
		return (data_hi << 8) | data_lo;
	} else {
		return ctx->buf[pos];
	}
}

void inst_mov_regmem_to_from_reg(struct ReaderCtx* ctx, uint8_t byte_a)
{
	Direction dir = (byte_a >> 1) & 0b1;
	Width is_word = byte_a & 0b1;

	uint8_t byte_b = ctx->buf[ctx->i++];
	enum ModeField mod = byte_b >> 6;
	uint8_t reg = (byte_b >> 3) & 0b111;
	uint8_t rm = byte_b & 0b111;

	switch (mod) {
	case MOD_MEM: {
		const char* reg_str = REGISTER_NAMES[reg][is_word];
		if (rm == 0b110) {
			uint8_t disp_lo = ctx->buf[ctx->i++];
			uint8_t disp_hi = ctx->buf[ctx->i++];
			uint16_t addr = (disp_hi << 8) | disp_lo;
			if (dir) {
				printf("mov %s, [%d]\n", reg_str, addr);
			} else {
				printf("mov [%d], %s\n", addr, reg_str);
			}
		} else {
			const char* ea = REGISTER_EFFECTIVE_ACCESS[rm];
			if (dir) {
				printf("mov %s, [%s]\n", reg_str, ea);
			} else {
				printf("mov [%s], %s\n", ea, reg_str);
			}
		}
		break;
	}
	case MOD_MEM_8BIT: {
		uint8_t offset = ctx->buf[ctx->i++];
		const char* reg_str = REGISTER_NAMES[reg][is_word];
		const char* ea = REGISTER_EFFECTIVE_ACCESS[rm];
		if ((offset & I8_SIGNED_BIT) == 0) {
			if (dir) {
				printf("mov %s, [%s + %d]\n", reg_str, ea, offset);
			} else {
				printf("mov [%s + %d], %s\n", ea, offset, reg_str);
			}
		} else {
			int16_t off = 0xFF00 | offset;
			if (dir) {
				printf("mov %s, [%s - %d]\n", reg_str, ea, -off);
			} else {
				printf("mov [%s - %d], %s\n", ea, -off, reg_str);
			}
		}
		break;
	}
	case MOD_MEM_16BIT: {
		uint8_t disp_lo = ctx->buf[ctx->i++];
		uint8_t disp_hi = ctx->buf[ctx->i++];
		uint16_t offset = (disp_hi << 8) | disp_lo;

		const char* reg_str = REGISTER_NAMES[reg][is_word];
		const char* ea = REGISTER_EFFECTIVE_ACCESS[rm];

		if ((offset & I16_SIGNED_BIT) == 0) {
			if (dir) {
				printf("mov %s, [%s + %d]\n", reg_str, ea, offset);
			} else {
				printf("mov [%s + %d], %s\n", ea, offset, reg_str);
			}
		} else {
			int16_t off = offset;
			if (dir) {
				printf("mov %s, [%s - %d]\n", reg_str, ea, -off);
			} else {
				printf("mov [%s - %d], %s\n", ea, -off, reg_str);
			}
		}
		break;
	}
	case MOD_REG: {
		const char* src = REGISTER_NAMES[dir ? rm : reg][is_word];
		const char* dst = REGISTER_NAMES[dir ? reg : rm][is_word];
		printf("mov %s, %s\n", dst, src);
		break;
	}
	}
}

void inst_mov_immediate_to_regmem(struct ReaderCtx* ctx, uint8_t byte_a)
{
	Width is_word = byte_a & 0b1;
	uint8_t byte_b = ctx->buf[ctx->i++];
	uint8_t mod = byte_b >> 6;
	uint8_t rm = byte_b & 0b111;
	const char* width_string = is_word ? "word" : "byte";

	switch (mod) {
	case MOD_MEM: {
		if (rm == 0b110) {
			uint8_t disp_lo = ctx->buf[ctx->i++];
			uint8_t disp_hi = ctx->buf[ctx->i++];
			uint16_t addr = (disp_hi << 8) | disp_lo;
			uint16_t data = read_immediate(ctx, is_word);
			printf("mov [%d], %s %d\n", addr, width_string, data);
		} else {
			const char* ea = REGISTER_EFFECTIVE_ACCESS[rm];
			uint16_t data = read_immediate(ctx, is_word);
			printf("mov [%s], %s %d\n", ea, width_string, data);
		}
		break;
	}
	case MOD_MEM_8BIT: {
		uint8_t disp_lo = ctx->buf[ctx->i++];
		uint8_t disp_hi = ctx->buf[ctx->i++];
		uint16_t addr = (disp_hi << 8) | disp_lo;
		const char* ea = REGISTER_EFFECTIVE_ACCESS[rm];
		uint16_t data = read_immediate(ctx, is_word);

		if ((addr & I8_SIGNED_BIT) == 0) {
			printf("mov [%s + %d], %s %d\n", ea, addr, width_string, data);
		} else {
			int16_t off = 0xFF00 | addr;
			printf("mov [%s - %d], %s %d\n", ea, -off, width_string, data);
		}
		break;
	}
	case MOD_MEM_16BIT: {
		uint8_t disp_lo = ctx->buf[ctx->i++];
		uint8_t disp_hi = ctx->buf[ctx->i++];
		uint16_t addr = (disp_hi << 8) | disp_lo;
		const char* ea = REGISTER_EFFECTIVE_ACCESS[rm];
		uint16_t data = read_immediate(ctx, is_word);

		if ((addr & I16_SIGNED_BIT) == 0) {
			printf("mov [%s + %d], %s %d\n", ea, addr, width_string, data);
		} else {
			int16_t off = addr;
			printf("mov [%s - %d], %s %d\n", ea, -off, width_string, data);
		}
		break;
	}
	case MOD_REG: {
		const char* reg = REGISTER_NAMES[rm][is_word];
		uint16_t data = read_immediate(ctx, is_word);
		printf("mov %s, %s %d\n", reg, width_string, data);
		break;
	}
	}
}

void inst_mov_immediate_to_reg(struct ReaderCtx* ctx, uint8_t byte_a)
{
	Width is_word = (byte_a >> 3) & 0b1;
	uint8_t reg = byte_a & 0b111;

	uint16_t data = 0;
	if (is_word) {
		uint8_t data_a = ctx->buf[ctx->i++];
		uint8_t data_b = ctx->buf[ctx->i++];
		data = (data_b << 8) | data_a;
	} else {
		data = ctx->buf[ctx->i++];
	}

	printf("mov %s, %d\n", REGISTER_NAMES[reg][is_word], data);
}

void inst_mov_mem_to_accumulator(struct ReaderCtx* ctx, uint8_t byte_a)
{
	Width is_word = byte_a & 0b1;
	uint8_t addr_lo = ctx->buf[ctx->i++];
	uint8_t addr_hi = ctx->buf[ctx->i++];
	uint16_t addr = (addr_hi << 8) | addr_lo;
	printf("mov a%c, [%d]\n", is_word ? 'x' : 'l', addr);
}

void inst_mov_accumulator_to_mem(struct ReaderCtx* ctx, uint8_t byte_a)
{
	Width is_word = byte_a & 0b1;
	uint8_t addr_lo = ctx->buf[ctx->i++];
	uint8_t addr_hi = ctx->buf[ctx->i++];
	uint16_t addr = (addr_hi << 8) | addr_lo;
	printf("mov [%d], a%c\n", addr, is_word ? 'x' : 'l');
}
