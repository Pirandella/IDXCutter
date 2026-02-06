#include "idx.h"

IDX_Status idx_open(IDX_File *file, const char *path)
{
    if ((file == NULL) || (path == NULL)) return IDX_ARG_ERROR;

    file->fd = fopen(path, "rwb");
    if (file->fd == NULL) {
        perror("Cannot open IDX file:");
        return IDX_FILE_ERROR;
    }

    fread(&file->head, IDX_HEAD_SIZE, 1, file->fd);
    if (ferror(file->fd) != 0) {
        printf("File read error!\n");
        return IDX_FILE_ERROR;
    }
    fread(file->head.dimentions, sizeof(uint32_t) * file->head.dimention_num, 1, file->fd);
    if (ferror(file->fd) != 0) {
        printf("Dimention read error!\n");
        return IDX_FILE_ERROR;
    }
    file->data_offset = IDX_HEAD_SIZE + (sizeof(uint32_t) * file->head.dimention_num);
    printf("Data offset: %ld\n", file->data_offset);

    printf("FILE INFO:\n - Path: %s\n - Type: %d\n - Dimentions: %d\n", path, file->head.type_size, file->head.dimention_num);
    for (uint8_t i = 0; i < file->head.dimention_num; i++) {
        file->head.dimentions[i] = ntohl(file->head.dimentions[i]);
        printf("   -  D%-3d: %d\n", (i + 1), file->head.dimentions[i]);
    }

    return IDX_OK;
}

void idx_close(IDX_File *file)
{
    if (file == NULL) return;

    fclose(file->fd);
}

IDX_Status idx_read_block(const IDX_File *const file, void *const buffer, size_t size, size_t index)
{
    if ((file == NULL) || (buffer == NULL)) {
        printf("Nonnullable input argument is NULL!\n");
        return IDX_ARG_ERROR;
    }
    if (file->head.dimentions[0] <= index) {
        printf("Out of bounds index!\n");
        return IDX_ARG_ERROR;
    }

    fseek(file->fd, file->data_offset + (index * size), SEEK_SET);
    fread(buffer, size, 1, file->fd);
    if (ferror(file->fd) != 0) {
        printf("Block read error!\n");
        return IDX_FILE_ERROR;
    }

    return IDX_OK;
}

IDX_Status idx_write_block(IDX_File *file, const void *const buffer, size_t size, size_t index)
{
    return IDX_OK;
}
