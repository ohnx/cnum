#include <stdio.h>
/* exit() */
#include <stdlib.h>
#include "data.h"

#define printerr_and_quit(x) do {\
    fprintf(stderr, "Line %d: Error code: %d\n", __LINE__, x);\
    exit(x);\
} while(0);

struct dataset {
    data_handle *image_handle;
    data_handle *label_handle;
};

int main(int argc, char **argv) {
    struct dataset training, testing;
    mnist_image *image;
    int r, i, j, k;

    /* open the data */
    r = data_open("data/train-images-idx3-ubyte", &training.image_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/train-labels-idx1-ubyte", &training.label_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/t10k-images-idx3-ubyte", &testing.image_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/t10k-labels-idx1-ubyte", &testing.label_handle);
    if (r != 0) printerr_and_quit(r);

    /* print out some numbers */
    for (i = 0; i < 10; i++) {
        r = data_read(training.image_handle, training.label_handle, i, &image);
        printf("-----------------------------------------------\n%d:\n", image->label);
        if (r != 0) printerr_and_quit(r);
        for (j = 0; j < DATA_IMAGE_SIZE; j++) {
            for (k = 0; k < DATA_IMAGE_SIZE; k++) {
                if (image->data[j*DATA_IMAGE_SIZE+k] > 250) printf("X");
                else if (image->data[j*DATA_IMAGE_SIZE+k] > 220) printf("+");
                else printf(" ");
            }
            printf("\n");
        }
        image_cleanup(image);
    }

    /* cleanup */
    data_cleanup(training.image_handle);
    data_cleanup(training.label_handle);
    data_cleanup(testing.image_handle);
    data_cleanup(testing.label_handle);

    return 0;
}
