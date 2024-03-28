CC ?= clang
NASM ?= nasm
DISASM_BIN = ./disasm/disasm
DISASM_SRC = ./disasm/main.c ./disasm/util.c ./disasm/asm.c ./disasm/inst/mov.c

ASM_SRCS=$(wildcard ./asm/*.asm)
ASM_BINS=$(ASM_SRCS:%.asm=%.bin)

.PHONY: all clean clean-bins clean-disasm

all: $(DISASM_BIN) $(ASM_BINS)

$(DISASM_BIN):
	$(CC) $(DISASM_SRC) -o $(DISASM_BIN)

%.bin: %.asm
	$(NASM) $< -o $@

clean-bins:
	rm -f $(ASM_BINS)

clean-disasm:
	rm -f $(DISASM_BIN)

clean: clean-bins clean-disasm
