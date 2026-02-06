#ifndef _IDX_H_
#define _IDX_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#define IDX_HEAD_SIZE 4

typedef struct {
    uint16_t magic;
    uint8_t type_size;
    uint8_t dimention_num;
    uint32_t *dimentions;
} IDX_Header;

typedef struct {
    FILE *fd;
    IDX_Header head;
} IDX_File;

typedef enum {
    IDX_OK,
    IDX_ERROR,
    IDX_ARG_ERROR,
    IDX_FILE_ERROR,
} IDX_Status;

typedef enum {
    IDX_SEEK_SET,
    IDX_SEEK_CUR,
    IDX_SEEK_END
} IDX_Position;

IDX_Status idx_open(IDX_File *file, const char *path);
void idx_close();

IDX_Status idx_read(const IDX_File *const file, void *buffer, size_t size, IDX_Position whence, off_t offset);
IDX_Status idx_write(IDX_File *file, const void *const buffer, size_t size, IDX_Position whence, off_t offset);

#endif /* _IDX_H_ */