#pragma once

#include <stdint.h>
#include <stdio.h>

/// Reads an entire file into memory from path and assigns it to *buf.
/// Returns -1 on error, cleanup is already done if errored.
ssize_t read_file(const char* path, uint8_t** buf);
