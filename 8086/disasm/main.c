#include "util.h"
#include <stdlib.h>

#pragma mark - type/enum/const definitions

enum ModeField {
    MOD_MEM = 0b00,
    MOD_MEM_8BIT = 0b01,
    MOD_MEM_16BIT = 0b10,
    MOD_REG = 0b11,
};

enum Opcode {
    OPCODE_MOV = 0b100010,
};

const char REGISTER_NAMES[8][2][2] = {
    { "al", "ax" },
    { "cl", "cx" },
    { "dl", "dx" },
    { "bl", "bx" },
    { "ah", "sp" },
    { "ch", "bp" },
    { "dh", "si" },
    { "bh", "di" },
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

    // we increment by 2 here but may increment more depending on the instruction.
    for (size_t i = 0; i < len; i += 2) {
        uint8_t byte_a = buf[i];
        uint8_t opcode = byte_a >> 2;
        Direction d = (byte_a >> 1) & 0b1;
        Width w = byte_a & 0b1;

        uint8_t byte_b = buf[i + 1];
        enum ModeField mod = byte_b >> 6;
        uint8_t reg = (byte_b >> 3) & 0b111;
        uint8_t rm = byte_b & 0b111;

        // now handle the opcode
        switch (opcode) {
        case OPCODE_MOV:
            if (mod == MOD_REG) {
                const char* src = REGISTER_NAMES[d ? rm : reg][w];
                const char* dst = REGISTER_NAMES[d ? reg : rm][w];
                printf("mov %c%c, %c%c\n", dst[0], dst[1], src[0], dst[1]);
            }
            break;
        default:
            break;
        }
    }

    free(buf);
    return 0;
}
