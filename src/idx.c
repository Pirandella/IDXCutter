#include "idx.h"

IDX_Status idx_open(IDX_File *file, const char *path)
{
    if ((file == NULL) || (path == NULL)) return IDX_ARG_ERROR;
    file->head.dimentions = NULL;

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
    size_t dim_size = sizeof(uint32_t) * file->head.dimention_num;
    file->head.dimentions = (uint32_t *)malloc(dim_size);
    if (file->head.dimentions == NULL) {
        printf("Memory allocation error!\n");
        return IDX_ERROR;
    }
    fread(file->head.dimentions, dim_size, 1, file->fd);
    if (ferror(file->fd) != 0) {
        printf("Dimention read error!\n");
        return IDX_FILE_ERROR;
    }

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
    if (file->head.dimentions == NULL) return;

    fclose(file->fd);
    free(file->head.dimentions);
}

IDX_Status idx_read(const IDX_File *const file, void *buffer, size_t size, IDX_Position whence, off_t offset)
{
    return IDX_OK;
}

IDX_Status idx_write(IDX_File *file, const void *const buffer, size_t size, IDX_Position whence, off_t offset)
{
    return IDX_OK;
}
