#ifndef _IDX_H_
#define _IDX_H_

#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

#define IDX_HEAD_SIZE 4

typedef struct {
    uint16_t magic;
    uint8_t type_size;
    uint8_t dimention_num;
    uint32_t dimentions[UINT8_MAX];
} IDX_Header;

typedef struct {
    FILE *fd;
    IDX_Header head;
    size_t data_offset;
} IDX_File;

typedef enum {
    IDX_OK,
    IDX_ERROR,
    IDX_ARG_ERROR,
    IDX_FILE_ERROR,
} IDX_Status;

IDX_Status idx_open(IDX_File *file, const char *path);
void idx_close();

IDX_Status idx_read_block(const IDX_File *const file, void *const buffer, size_t size, size_t index);
IDX_Status idx_write_block(IDX_File *file, const void *const buffer, size_t size, size_t index);

#endif /* _IDX_H_ */