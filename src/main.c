/* printf() */
#include <stdio.h>
/* exit() */
#include <stdlib.h>
/* clock() */
#include <time.h>

#include "data.h"
#include "perceptron.h"

#define printerr_and_quit(x) do {\
    fprintf(stderr, "Line %d: Error code: %d\n", __LINE__, x);\
    exit(x);\
} while(0);

#define PERCEPTRON_NET_NAME "perceptron.net"

struct dataset {
    data_handle *image_handle;
    data_handle *label_handle;
};

int main(int argc, char **argv) {
    struct dataset training, testing;
    mnist_image *image;
    simplenet *network;
    int r, i, j, k;
    clock_t begin, end;
    byte classification = 0;
    char input[3];

    /* open the data */
    r = data_open("data/train-images-idx3-ubyte", &training.image_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/train-labels-idx1-ubyte", &training.label_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/t10k-images-idx3-ubyte", &testing.image_handle);
    if (r != 0) printerr_and_quit(r);
    r = data_open("data/t10k-labels-idx1-ubyte", &testing.label_handle);
    if (r != 0) printerr_and_quit(r);

    do {
        printf("load existing net? (Y/n) "); fflush(stdout);
        r = scanf("%2s", input);

        if (*input == 'Y') {
            FILE *f;
            byte *bytes;
            int length;

            f = fopen(PERCEPTRON_NET_NAME, "rb");
            if (f == NULL) {
                printf("failed to open file.\n");
                continue;
            }

            fseek(f, 0, SEEK_END);
            length = ftell(f);
            fseek(f, 0, SEEK_SET);

            bytes = (byte *)malloc(length);
            if (length != fread(bytes, sizeof(byte), length, f)) {
                free(bytes);
                printf("failed to read file.\n");
                continue;
            }

            fclose(f);

            r = simplenet_deserialize(bytes, length, &network);
            free(bytes);
            if (r) {
                printf("can't parse the network!");
                continue;
            }

            goto testing;
        } else if (*input == 'n') {
            /* create the neural network */
            r = simplenet_init(&network);
            break;
        } else {
            printf("Invalid input.\n");
        }
    } while (1);

    /* train the neural network */
    printf("training network on image %05d of %05d...", 0, training.image_handle->count);fflush(stdout);
    begin = clock();
    for (i = 0; i < training.image_handle->count; i++) {
        r = data_read(training.image_handle, training.label_handle, i, &image);
        if (r != 0) printerr_and_quit(r);
        if (!(i % 93)) {
            printf("\rtraining network on image %05d of %05d...", i+1, training.image_handle->count);fflush(stdout);
        }
        (void)simplenet_train(network, image);
        image_cleanup(image);
    }
    end = clock();
    printf("\rtraining network on image %05d of %05d...", training.image_handle->count, training.image_handle->count);fflush(stdout);
    printf(" done in %fs!\n", ((double)(end - begin) / CLOCKS_PER_SEC));

    /* now we run real tests */
    printf("running tests on neural network... testing image %05d of %05d...", 0, testing.image_handle->count);fflush(stdout);
    k = 0;
    begin = clock();
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
    end = clock();
    printf("\rrunning tests on neural network... testing image %05d of %05d...", testing.image_handle->count, testing.image_handle->count);fflush(stdout);
    printf(" done in %fs with %f%% accuracy!\n", ((double)(end - begin) / CLOCKS_PER_SEC), (1-(double)k/(double)(testing.image_handle->count))*100);

    /* train the neural network */
    printf("let's train again!\n");
    printf("training network on image %05d of %05d...", 0, training.image_handle->count);fflush(stdout);
    begin = clock();
    for (i = training.image_handle->count-1; i >= 0; i--) {
        r = data_read(training.image_handle, training.label_handle, i, &image);
        if (r != 0) printerr_and_quit(r);
        if (!(i % 97)) {
            printf("\rtraining network on image %05d of %05d...", training.image_handle->count-i, training.image_handle->count);fflush(stdout);
        }
        (void)simplenet_train(network, image);
        image_cleanup(image);
    }
    end = clock();
    printf("\rtraining network on image %05d of %05d...", training.image_handle->count, training.image_handle->count);fflush(stdout);
    printf(" done in %fs!\n", ((double)(end - begin) / CLOCKS_PER_SEC));

    printf("test again, too!\n");
    /* now we run real tests */
    printf("running tests on neural network... testing image %05d of %05d...", 0, testing.image_handle->count);fflush(stdout);
    k = 0;
    begin = clock();
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
    end = clock();
    printf("\rrunning tests on neural network... testing image %05d of %05d...", testing.image_handle->count, testing.image_handle->count);fflush(stdout);
    printf(" done in %fs with %f%% accuracy!\n", ((double)(end - begin) / CLOCKS_PER_SEC), (1-(double)k/(double)(testing.image_handle->count))*100);
testing:

    /* now we allow the user to test the network for themself */
    printf("try the network for yourself!\n");
    do {
        printf("pick a number between 0 and %d, inclusive: ", testing.image_handle->count); fflush(stdout);
        r = scanf("%d", &k);
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

    if (*input != 'Y') {
        byte *array;
        int length;
        FILE *f;

        printf("saving network...\n");

        r = simplenet_serialize(network, &array, &length);
        if (r) {
            printf("failed to deserialize :(\n");
            goto byebye;
        }

        f = fopen(PERCEPTRON_NET_NAME, "wb");
        if (f == NULL) {
            printf("failed to open output file :(\n");
            free(array);
            fclose(f);
            goto byebye;
        }

        if (length != fwrite(array, sizeof(byte), length, f)) {
            printf("failed to write file :(\n");
            free(array);
            fclose(f);
            goto byebye;
        }

        printf("serialized the network!\n");
        fclose(f);
        free(array);
    }

byebye:
    /* cleanup */
    data_cleanup(training.image_handle);
    data_cleanup(training.label_handle);
    data_cleanup(testing.image_handle);
    data_cleanup(testing.label_handle);
    free(network);

    return 0;
}
