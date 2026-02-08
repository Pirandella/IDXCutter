#include "idx.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

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

#define DATASET_SIZE 100

int main(int argc, char **argv)
{
    srand(time(NULL));

    IDX_File src_images, src_labels, dest_images, dest_labels;

    if (idx_open(&src_images, "./t10k-images.idx3-ubyte") != IDX_OK) return -1;
    if (idx_open(&src_labels, "./t10k-labels.idx1-ubyte") != IDX_OK) return -1;

    memcpy(&dest_images, &src_images, sizeof(IDX_File));
    memcpy(&dest_labels, &src_labels, sizeof(IDX_File));
    dest_images.head.dimensions[0] = DATASET_SIZE;
    dest_labels.head.dimensions[0] = DATASET_SIZE;

    if (idx_open(&dest_images, "./images-dataset.idx3-ubyte") != IDX_OK) return -1;
    if (idx_open(&dest_labels, "./labels-dataset.idx3-ubyte") != IDX_OK) return -1;

    uint8_t buff[src_images.block_size];
    uint8_t label;

    size_t rand_index = 0;
    for (size_t i = 0; i < DATASET_SIZE; i++) {
        rand_index = rand() % src_images.head.dimensions[0];
        idx_read_block(&src_images, buff, src_images.block_size, rand_index);
        idx_read_block(&src_labels, &label, 1, rand_index);
        idx_write_block(&dest_images, buff, dest_images.block_size, i);
        idx_write_block(&dest_labels, &label, 1, i);
        print_mnist(buff, label, src_images.block_size);
    }

    idx_close(&src_images);
    idx_close(&src_labels);
    idx_close(&dest_images);
    idx_close(&dest_labels);

    return 0;
}
