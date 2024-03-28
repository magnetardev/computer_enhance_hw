#pragma once

#include <stddef.h>
#include <stdint.h>

/// One bit to indicate the direction of the instruction.
/// 0 => instruction src is in reg;
/// 1 => instruction dst is in reg;
typedef uint8_t Direction;

/// One bit to indicate the width of the instruction.
/// 0 => instruction operates on byte data;
/// 1 => instruction operates on word data;
typedef uint8_t Width;

struct ReaderCtx {
  uint8_t *buf;
  size_t len;
  size_t i;
};

enum ModeField {
  MOD_MEM = 0b00,
  MOD_MEM_8BIT = 0b01,
  MOD_MEM_16BIT = 0b10,
  MOD_REG = 0b11,
};

extern const char *REGISTER_NAMES[8][2];
extern const char *REGISTER_EFFECTIVE_ACCESS[8];
