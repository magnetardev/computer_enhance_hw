#include "asm.h"

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
