#ifndef _IDX_H_
#define _IDX_H_

#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

#define IDX_HEAD_SIZE 4

typedef enum {
    IDX_UINT8  = 0x08,
    IDX_INT8   = 0x09,
    IDX_INT16  = 0x0B,
    IDX_INT32  = 0x0C,
    IDX_FLOAT  = 0x0D,
    IDX_DOUBLE = 0x0E
} IDX_Types;

typedef struct {
    uint16_t magic;
    uint8_t type;
    uint8_t dimension_num;
    uint32_t dimensions[UINT8_MAX];
} IDX_Header;

typedef struct {
    FILE *fd;
    IDX_Header head;
    size_t data_offset;
    size_t block_size;
} IDX_File;

typedef enum {
    IDX_OK,
    IDX_ERROR,
    IDX_ARG_ERROR,
    IDX_FILE_ERROR,
} IDX_Status;

IDX_Status idx_open(IDX_File *file, const char *path);
void idx_close();

IDX_Status idx_set_header(IDX_File *file, IDX_Types type, uint8_t dimensions, const uint32_t *const sizes);

IDX_Status idx_read_block(const IDX_File *const file, void *const buffer, size_t size, size_t index);
IDX_Status idx_write_block(IDX_File *file, const void *const buffer, size_t size, size_t index);

#endif /* _IDX_H_ */