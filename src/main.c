#include <stdio.h>
/* exit() */
#include <stdlib.h>
#include "data.h"

#define printerr_and_quit(x) do {\
    fprintf(stderr, "Line %d: Error code: %d\n", __LINE__, x);\
    exit(x);\
} while(0);

int main(int argc, char **argv) {
    data_handle *image_handle;
    data_handle *label_handle;
    mnist_image *image;
    int r, i, j, k;

    r = data_open("data/t10k-images-idx3-ubyte", &image_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/t10k-labels-idx1-ubyte", &label_handle);
    if (r != 0) printerr_and_quit(r);

    for (i = 0; i < 10; i++) {
        r = data_read(image_handle, label_handle, i, &image);
        printf("-----------------------------------------------\n%d:\n", image->label);
        if (r != 0) printerr_and_quit(r);
        for (j = 0; j < DATA_IMAGE_SIZE; j++) {
            for (k = 0; k < DATA_IMAGE_SIZE; k++) {
                if (image->data[j*DATA_IMAGE_SIZE+k] > 220) printf("X");
                else printf(" ");
            }
            printf("\n");
        }
    }

    return 0;
}
