#include "asm.h"
#include "inst/mov.h"
#include "util.h"
#include <stdint.h>
#include <stdlib.h>

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

	struct ReaderCtx ctx = {
		.buf = buf,
		.i = 0,
		.len = len,
	};

	printf("; %s\n", path);
	puts("bits 16\n");

	// NOTE: bounds checking does not occur besides this check. Machine code must be well formed.
	while (ctx.i < ctx.len) {
		uint8_t byte_a = ctx.buf[ctx.i++];
		if ((byte_a & 0b11111100) == 0b10001000) {
			inst_mov_regmem_to_from_reg(&ctx, byte_a);
		} else if ((byte_a & 0b11111110) == 0b11000110) {
			inst_mov_immediate_to_regmem(&ctx, byte_a);
		} else if ((byte_a & 0b11110000) == 0b10110000) {
			inst_mov_immediate_to_reg(&ctx, byte_a);
		} else if ((byte_a & 0b11111110) == 0b10100000) {
			inst_mov_mem_to_accumulator(&ctx, byte_a);
		} else if ((byte_a & 0b11111110) == 0b10100010) {
			inst_mov_accumulator_to_mem(&ctx, byte_a);
		} else {
			puts("; unknown instruction");
			break;
		}
	}

	free(ctx.buf);
	return 0;
}
