#include "idx.h"
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define IDX_TYPE_INDEX(type) ((uint8_t)(type - 8))

static const char idx_data_type_str[7][7] = {"UINT8", "INT8", "", "INT16", "INT32", "FLOAT", "DOUBLE"};
static const uint8_t idx_data_type_size[7] = {sizeof(uint8_t), sizeof(int8_t), 0, sizeof(int16_t),
    sizeof(int32_t), sizeof(float), sizeof(double)};
    
    static void _print_info(const IDX_File *const file, const char *const path)
    {
        printf("FILE INFO:\n - Path: %s\n - Type: %s\n - Dimentions: %d\n", (path != NULL ? path : "---"), 
        idx_data_type_str[IDX_TYPE_INDEX(file->head.type)], file->head.dimension_num);
        for (uint8_t i = 0; i < file->head.dimension_num; i++)
        printf("   -  D%-3d: %d\n", (i + 1), file->head.dimensions[i]);
    printf(" - Offset: %ld\n - Block: %ld\n", file->data_offset, file->block_size);
}

IDX_Status idx_open(IDX_File *file, const char *path)
{
    if ((file == NULL) || (path == NULL)) return IDX_ARG_ERROR;
    
    if (!access(path, F_OK)) {
        file->fd = fopen(path, "r");
        if (file->fd == NULL) {
            perror("Cannot open IDX file:");
            return IDX_FILE_ERROR;
        }
        struct stat st;
        if (stat(path, &st)) {
            printf("Cannot get file stats!\n");
            return IDX_FILE_ERROR;
        }
        file->file_size = (size_t)st.st_size;
        if (file->file_size == 0) {
            printf("Empty file!\n");
            return IDX_FILE_ERROR;
        }

        fread(&file->head, IDX_HEAD_SIZE, 1, file->fd);
        if (ferror(file->fd) != 0) {
            printf("File read error!\n");
            return IDX_FILE_ERROR;
        }
        if (file->head.dimension_num == 0) {
            printf("File without data structure!\n");
            return IDX_FILE_ERROR;
        }
        
        fread(file->head.dimensions, sizeof(uint32_t) * file->head.dimension_num, 1, file->fd);
        if (ferror(file->fd) != 0) {
            printf("Dimention read error!\n");
            return IDX_FILE_ERROR;
        }
        file->block_size = 1;
        for (uint8_t i = 0; i < file->head.dimension_num; i++) {
            file->head.dimensions[i] = ntohl(file->head.dimensions[i]);
            if (((file->head.dimension_num > 2) && (i == 0)) || (file->head.dimension_num == 1)) continue;
            file->block_size *= file->head.dimensions[i];
        }
        file->block_size *= idx_data_type_size[IDX_TYPE_INDEX(file->head.type)];
        file->data_offset = IDX_HEAD_SIZE + (sizeof(uint32_t) * file->head.dimension_num);
    } else {
        file->fd = fopen(path, "w");
        if (file->fd == NULL) {
            perror("Cannot create IDX file:");
            return IDX_FILE_ERROR;
        }
        for (uint8_t i = 0; i < file->head.dimension_num; i++)
            file->head.dimensions[i] = htonl(file->head.dimensions[i]);
        fwrite(&file->head, file->data_offset, 1, file->fd);
        for (uint8_t i = 0; i < file->head.dimension_num; i++)
            file->head.dimensions[i] = ntohl(file->head.dimensions[i]);
    }

    _print_info(file, path);

    return IDX_OK;
}

void idx_close(IDX_File *file)
{
    if (file == NULL) return;

    fclose(file->fd);
}

IDX_Status idx_set_header(IDX_File *file, IDX_Types type, uint8_t dimensions, const uint32_t *const sizes)
{
    if ((file == NULL) || (dimensions == 0) || (sizes == NULL)) return IDX_ARG_ERROR;

    file->head.magic = 0;
    file->head.type = (uint8_t)type;
    file->head.dimension_num = dimensions;
    file->data_offset = IDX_HEAD_SIZE + (sizeof(uint32_t) * dimensions);
    file->block_size = 1;
    for (uint8_t i = 0; i < file->head.dimension_num; i++) {
        file->head.dimensions[i] = sizes[i];
        if (((file->head.dimension_num > 2) && (i == 0)) || (file->head.dimension_num == 1)) continue;
        file->block_size *= file->head.dimensions[i];
    }
    file->block_size *= idx_data_type_size[IDX_TYPE_INDEX(file->head.type)];
    file->file_size = file->data_offset;

    return IDX_OK;
}

size_t idx_block_size(const IDX_File *const file)
{
    return file->block_size;
}

IDX_Status idx_read_block(const IDX_File *const file, void *const buffer, size_t size, size_t index)
{
    if ((file == NULL) || (buffer == NULL)) {
        printf("Nonnullable input argument is NULL!\n");
        return IDX_ARG_ERROR;
    }
    if (file->head.dimensions[0] <= index) {
        printf("Out of bounds index!\n");
        return IDX_ARG_ERROR;
    }
    if (file->block_size > size) {
        printf("Input buffer is smaller then block size! Block size: %ld; Buffer size: %ld\n", file->block_size, size);
        return IDX_ARG_ERROR;
    }

    fseek(file->fd, file->data_offset + (index * file->block_size), SEEK_SET);
    fread(buffer, file->block_size, 1, file->fd);
    if (ferror(file->fd) != 0) {
        printf("Block read error!\n");
        return IDX_FILE_ERROR;
    }

    return IDX_OK;
}

IDX_Status idx_write_block(IDX_File *file, const void *const buffer, size_t size, size_t index)
{    
    if ((file == NULL) || (buffer == NULL)) {
        printf("Nonnullable input argument is NULL!\n");
        return IDX_ARG_ERROR;
    }
    if (file->block_size > size) {
        printf("Input buffer is smaller then block size! Block size: %ld; Buffer size: %ld\n", file->block_size, size);
        return IDX_ARG_ERROR;
    }
    if ((fcntl(file->fd->_fileno, F_GETFL) & O_ACCMODE) == O_RDONLY) {
        printf("You cannnot write to READ ONLY file!\n");
        return IDX_FILE_ERROR;
    }
    size_t offset = file->data_offset + (index * file->block_size);
    if (offset >= file->file_size) offset = file->file_size;

    fseek(file->fd, offset, SEEK_SET);
    fwrite(buffer, file->block_size, 1, file->fd);
    if (ferror(file->fd) != 0) {
        printf("Block write error!\n");
        return IDX_FILE_ERROR;
    }
    file->file_size += file->block_size;

    return IDX_OK;
}
