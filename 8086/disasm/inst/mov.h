#pragma once

#include "../asm.h"

void inst_mov_regmem_to_from_reg(struct ReaderCtx *ctx, uint8_t byte_a);
void inst_mov_immediate_to_regmem(struct ReaderCtx *ctx, uint8_t byte_a);
void inst_mov_immediate_to_reg(struct ReaderCtx *ctx, uint8_t byte_a);
void inst_mov_mem_to_accumulator(struct ReaderCtx *ctx, uint8_t byte_a);
void inst_mov_accumulator_to_mem(struct ReaderCtx *ctx, uint8_t byte_a);
