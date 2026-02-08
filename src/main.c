#include "idx.h"

static void print_mnist(const void *const img, uint8_t label, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        printf("%c", ((uint8_t *)img)[i] != 0 ? '@' : ' ');
        if (i != (28 * 16) - 1) {
            if (!((i + 1) % 28)) puts("");
        } else {
            printf("\t%d\n", label);
        }
    }
}

int main(int argc, char **argv)
{

    IDX_File images, labels;

    idx_open(&images, "./t10k-images.idx3-ubyte");
    idx_open(&labels, "./t10k-labels.idx1-ubyte");

    uint8_t buff[28 * 28];
    uint8_t label;
    idx_read_block(&images, buff, sizeof(buff), 5464);
    idx_read_block(&labels, &label, 1, 5464);
    print_mnist(buff, label, sizeof(buff));

    idx_close(&images);
    idx_close(&labels);

    IDX_File tmp;
    idx_set_header(&tmp, IDX_UINT8, 2, (uint32_t[]){2, 2});
    idx_open(&tmp, "./tmp.idx");
    idx_write_block(&tmp, (uint8_t []){1, 2, 3, 4}, idx_block_size(&tmp), 2);
    idx_write_block(&tmp, (uint8_t []){1, 2, 3, 4}, idx_block_size(&tmp), 2);
    idx_write_block(&tmp, (uint8_t []){1, 2, 3, 4}, idx_block_size(&tmp), 2);
    idx_write_block(&tmp, (uint8_t []){1, 2, 3, 4}, idx_block_size(&tmp), 3);
    idx_write_block(&tmp, (uint8_t []){5, 6, 7, 8}, idx_block_size(&tmp), 2);
    idx_close(&tmp);

    return 0;
}