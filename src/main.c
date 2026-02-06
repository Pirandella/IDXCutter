#include "idx.h"

int main(int argc, char **argv)
{

    IDX_File images, labels;

    idx_open(&images, "./t10k-images.idx3-ubyte");
    idx_open(&images, "./t10k-labels.idx1-ubyte");

    idx_close(&images);
    idx_close(&labels);

    return 0;
}