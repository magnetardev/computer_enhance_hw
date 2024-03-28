#include "util.h"
#include <stdint.h>
#include <stdlib.h>

#pragma mark - type/enum/const definitions

enum ModeField {
	MOD_MEM = 0b00,
	MOD_MEM_8BIT = 0b01,
	MOD_MEM_16BIT = 0b10,
	MOD_REG = 0b11,
};

const char* REGISTER_NAMES[8][2] = {
	{ "al", "ax" },
	{ "cl", "cx" },
	{ "dl", "dx" },
	{ "bl", "bx" },
	{ "ah", "sp" },
	{ "ch", "bp" },
	{ "dh", "si" },
	{ "bh", "di" },
};

const char* REGISTER_EFFECTIVE_ACCESS[8] = {
	"bx + si",
	"bx + di",
	"bp + si",
	"bp + di",
	"si",
	"di",
	"bp",
	"bx",
};

/// One bit to indicate the direction of the instruction.
/// 0 => instruction src is in reg;
/// 1 => instruction dst is in reg;
typedef uint8_t Direction;

/// One bit to indicate the width of the instruction.
/// 0 => instruction operates on byte data;
/// 1 => instruction operates on word data;
typedef uint8_t Width;

#pragma mark - implementation

int main(int argc, const char** argv)
{
	if (argc != 2) {
		fputs("expected one input\n", stderr);
		return 1;
	}

	const char* path = argv[1];

	uint8_t* buf = NULL;
	ssize_t len = read_file(path, &buf);
	if (len == -1) {
		return 1;
	}

	printf("; %s\n", path);
	puts("bits 16\n");

	size_t i = 0;
	// NOTE: bounds checking does not occur besides this check. Machine code must be well formed.
	while (i < len) {
		// now handle the opcode
		uint8_t byte_a = buf[i++];
		// MOV (register/memory <-> register)
		if ((byte_a & 0b11111100) == 0b10001000) {
			Direction dir = (byte_a >> 1) & 0b1;
			Width is_word = byte_a & 0b1;

			uint8_t byte_b = buf[i++];
			enum ModeField mod = byte_b >> 6;
			uint8_t reg = (byte_b >> 3) & 0b111;
			uint8_t rm = byte_b & 0b111;

			switch (mod) {
			case MOD_MEM: {
				const char* reg_str = REGISTER_NAMES[reg][is_word];
				if (rm == 0b110) {
					uint8_t disp_lo = buf[i++];
					uint8_t disp_hi = buf[i++];
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
				uint8_t offset = buf[i++];
				const char* reg_str = REGISTER_NAMES[reg][is_word];
				const char* ea = REGISTER_EFFECTIVE_ACCESS[rm];
				if (dir) {
					printf("mov %s, [%s + %d]\n", reg_str, ea, offset);
				} else {
					printf("mov [%s + %d], %s\n", ea, offset, reg_str);
				}
				break;
			}
			case MOD_MEM_16BIT: {
				uint8_t disp_lo = buf[i++];
				uint8_t disp_hi = buf[i++];
				uint16_t offset = (disp_hi << 8) | disp_lo;

				const char* reg_str = REGISTER_NAMES[reg][is_word];
				const char* ea = REGISTER_EFFECTIVE_ACCESS[rm];
				if (dir) {
					printf("mov %s, [%s + %d]\n", reg_str, ea, offset);
				} else {
					printf("mov [%s + %d], %s\n", ea, offset, reg_str);
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
		// MOV (immediate -> register)
		else if ((byte_a & 0b11110000) == 0b10110000) {
			Width is_word = (byte_a >> 3) & 0b1;
			uint8_t reg = byte_a & 0b111;

			uint16_t immediate = 0;
			if (is_word) {
				uint8_t data_a = buf[i++];
				uint8_t data_b = buf[i++];
				immediate = (data_b << 8) | data_a;
			} else {
				immediate = buf[i++];
			}

			printf("mov %s, %d\n", REGISTER_NAMES[reg][is_word], immediate);
		}
		// Unknown instruction handling
		else {
			puts("; unknown instruction");
		}
	}

	free(buf);
	return 0;
}
