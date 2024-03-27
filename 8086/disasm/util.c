#include "util.h"
#include <stdlib.h>

/// Reads an entire file into memory from path and assigns it to *buf.
/// Returns -1 on error, cleanup is already done.
ssize_t read_file(const char* path, uint8_t** buf)
{
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "failed to open input: %s\n", path);
        return -1;
    }

    // for now just be lazy and read the entire file contents

    if (fseek(file, 0, SEEK_END) != 0) {
        perror("failed to seek to end of file");
        fclose(file);
        return -1;
    }

    size_t len = ftell(file);
    if (len == -1L) {
        perror("failed to get file length");
        fclose(file);
        return -1;
    }

    if (len & 1) {
        fprintf(stderr, "invalid file length, expected an even length, got %zu\n", len);
        fclose(file);
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("failed to seek to start of file");
        fclose(file);
        return -1;
    }

    *buf = malloc(len);
    if (*buf == NULL) {
        perror("failed to allocate buffer");
        fclose(file);
        return -1;
    }

    if (fread(*buf, sizeof(uint8_t), len, file) != len) {
        if (feof(file)) {
            fputs("unexpected eof\n", stderr);
        } else if (ferror(file)) {
            perror("failed to read file");
        }

        free(*buf);
        fclose(file);
        return -1;
    }

    fclose(file);
    return len;
}
