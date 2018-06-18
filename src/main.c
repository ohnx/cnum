#include <stdio.h>
/* exit() */
#include <stdlib.h>
#include "data.h"

#ifdef __I_KNOW_WHAT_IM_DOING

int main() {
    puts("ok, if you say so...");
    return 0;
}

#else

#include "perceptron.h"

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
    simplenet *network;
    int r, i, j, k;
    byte classification = 0;

    /* open the data */
    r = data_open("data/train-images-idx3-ubyte", &training.image_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/train-labels-idx1-ubyte", &training.label_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/t10k-images-idx3-ubyte", &testing.image_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/t10k-labels-idx1-ubyte", &testing.label_handle);
    if (r != 0) printerr_and_quit(r);

    /* create the neural network */
    r = simplenet_init(&network);

    /* train the neural network */
    printf("training network on image %05d of %05d...", 0, training.image_handle->count);fflush(stdout);
    for (i = 0; i < training.image_handle->count; i++) {
        r = data_read(training.image_handle, training.label_handle, i, &image);
        if (r != 0) printerr_and_quit(r);
        if (!(i % 93)) {
            printf("\rtraining network on image %05d of %05d...", i+1, training.image_handle->count);fflush(stdout);
        }
        (void)simplenet_train(network, image);
        image_cleanup(image);
    }
    printf(" done!\n");

    /* now we run real tests */
    printf("running tests on neural network... testing image %05d of %05d...", 0, testing.image_handle->count);fflush(stdout);
    k = 0;
    for (i = 0; i < testing.image_handle->count; i++) {
        r = data_read(testing.image_handle, testing.label_handle, i, &image);
        if (r != 0) printerr_and_quit(r);
        if (!(i % 52)) {
            printf("\rrunning tests on neural network... testing image %05d of %05d...", i+1, testing.image_handle->count);fflush(stdout);
        }
        (void)simplenet_classify(network, image, &classification);
        if (classification != image->label) k++;
        image_cleanup(image);
    }
    printf(" done with %f%% accuracy!\n", ((double)k/(double)(testing.image_handle->count))*100);

    /* train the neural network */
    printf("let's train again!\n");
    printf("training network on image %05d of %05d...", 0, training.image_handle->count);fflush(stdout);
    for (i = 0; i < training.image_handle->count; i++) {
        r = data_read(training.image_handle, training.label_handle, i, &image);
        if (r != 0) printerr_and_quit(r);
        if (!(i % 97)) {
            printf("\rtraining network on image %05d of %05d...", i+1, training.image_handle->count);fflush(stdout);
        }
        (void)simplenet_train(network, image);
        image_cleanup(image);
    }
    printf(" done!\n");

    printf("test again, too!\n");
    /* now we run real tests */
    printf("running tests on neural network... testing image %05d of %05d...", 0, testing.image_handle->count);fflush(stdout);
    k = 0;
    for (i = 0; i < testing.image_handle->count; i++) {
        r = data_read(testing.image_handle, testing.label_handle, i, &image);
        if (r != 0) printerr_and_quit(r);
        if (!(i % 49)) {
            printf("\rrunning tests on neural network... testing image %05d of %05d...", i+1, testing.image_handle->count);fflush(stdout);
        }
        (void)simplenet_classify(network, image, &classification);
        if (classification != image->label) k++;
        image_cleanup(image);
    }
    printf(" done with %f%% accuracy!\n", ((double)k/(double)(testing.image_handle->count))*100);

    /* now we run tests */
    printf("try the network for yourself!\n");
    do {
        printf("pick a number between 0 and %d, inclusive: ", testing.image_handle->count); fflush(stdout);
        scanf("%d", &k);
        if (k < 0 || k > testing.image_handle->count) break;

        r = data_read(training.image_handle, training.label_handle, k, &image);
        printf("------------------------------------------number: %d\n", image->label);
        if (r != 0) printerr_and_quit(r);
        for (i = 0; i < DATA_IMAGE_SIZE; i++) {
            for (j = 0; j < DATA_IMAGE_SIZE; j++) {
                if (image->data[i*DATA_IMAGE_SIZE+j] > 250) printf("X");
                else if (image->data[i*DATA_IMAGE_SIZE+j] > 200) printf("+");
                else printf(" ");
            }
            printf("\n");
        }
        (void)simplenet_classify(network, image, &classification);
        printf("--------------------%s network classification: %d\n", (classification == image->label ? "RIGHT" : "WRONG"), classification);
        image_cleanup(image);
    } while (1);

    printf("thanks :)\n");

    /* cleanup */
    data_cleanup(training.image_handle);
    data_cleanup(training.label_handle);
    data_cleanup(testing.image_handle);
    data_cleanup(testing.label_handle);

    return 0;
}


#endif
