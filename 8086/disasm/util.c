#include "util.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/// Reads an entire file into memory from path and assigns it to *buf.
/// Returns -1 on error, cleanup is already done.
ssize_t read_file(const char* path, uint8_t** buf)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("failed to open file");
        return -1;
    }

    off_t len = lseek(fd, 0, SEEK_END);
    if (len == -1) {
        perror("failed to seek to end of file");
        return -1;
    }

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("failed to seek to end of file");
        return -1;
    }

    *buf = malloc(len);

    int amt_read_total = 0;
    while (amt_read_total != len) {
        ssize_t amt_read = read(fd, &*buf[amt_read_total], len);
        if (amt_read == -1) {
            perror("failed to read from file");
            free(*buf);
            close(fd);
            return -1;
        }
        amt_read_total += amt_read;
    }

    close(fd);
    return len;
}
