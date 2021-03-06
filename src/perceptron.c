/* this file implements a simple perceptron network and some utility functions */

/* malloc(), free(), rand(), etc. */
#include <stdlib.h>
/* time() */
#include <time.h>
/* memcpy(), memset() */
#include <string.h>
/* bswap_64() */
#include <byteswap.h>
/* cblas_dgemv() */
#include <cblas.h>

#include "error.h"
#include "perceptron.h"

int simplenet_init(simplenet **net) {
    int i, j;
    static char c = 0;

    /* allocate memory */
    *net = malloc(sizeof(simplenet));
    if (!(*net)) return ERROR_OOM;

    /* seed time */
    if (!c++) srand(time(NULL));

    /* for each neuron */
    for (i = 0; i < 10; i++) {
        /* for each weight */
        for (j = 0; j < 784; j++) {
            /* initialize to a random number */
            (*net)->neurons[i].vec[j] = rand()/((double)(RAND_MAX));
        }
    }

    return ERROR_OK;
}

void simplenet_run(simplenet *self, vec784d *input, vec10d *output) {
    /* clear the output vector */
    memset(output, 0, sizeof(vec10d));

    /**
     * Here's a visual representation of what's happening here...
     * 
     *              | w0n0  w1n0    ... w783n0  |   | p0    |
     *              | w0n1  w1n1    ... w783n1  |   | p1    |
     *  (1/784)  *  | ...   ...     ... ...     | * | p2    |
     *              | w0n8  w1n8    ... w783n8  |   | ...   |
     *              | w0n9  w1n9    ... w783n9  |   | p783  |
     *              {    10 rows, 784 columns   }   { 784 rows, 1 col }
     */

    /* multiply the matrix */
    cblas_dgemv(
        CblasRowMajor, /* row-major since we have 1 row of 784, then another row of 784, etc... */
        CblasNoTrans, /* no transposition */
        10, /* Matrix A has 10 rows */
        784, /* ...and 784 columns */
        1.0/784, /* We want to divide the resultant sum by 784 */
        (double *)self, /* Matrix A */
        784, /* A[m][n] = A[m*784 + n] */
        (double *)input, /* vector X should be a 784x1 matrix */
        1, /* no skip */
        0.0, /* scaling factor for vector Y is 0 since we don't use it */
        (double *)output, /* output vector Y */
        1 /* no skip */
    );
}

/* Todo: more fiddling to see how the RATE could affect accuracy */
#define RATE 0.02
void simplenet_train(simplenet *self, mnist_image *input) {
    byte i;
    int j;
    /*static vec784d */
    vec10d neuron_outputs;
    vec784d input_d;
    double error;

    /* convert the input */
    for (j = 0; j < 784; j++)
        input_d.vec[j] = (double)input->data[j]/255.0f;

    /* run the network to see what the results are */
    simplenet_run(self, &input_d, &neuron_outputs);

    /* loop through and perform corrections as necessary */
    for (i = 0; i < 10; i++) {
        /* error is how far away from the correct answer this was */
        error = (input->label == i ? 1 : 0) - neuron_outputs.vec[i];

        /* update the error */
        cblas_daxpy(
            784, /* 784 elements in the vector */
            RATE * error, /* multiply the input value by RATE * error */
            (double *)&input_d, /* input vector */
            1, /* no skip */
            (double *)&self->neurons[i], /* output is self */
            1 /* no skip */
        );
    }
}

void simplenet_classify(simplenet *self, mnist_image *input, byte *classification) {
    vec10d results;
    vec784d input_d;
    double max = 0;
    int j;
    byte i, li = 0;

    /* convert the input */
    for (j = 0; j < 784; j++)
        input_d.vec[j] = (double)input->data[j]/255.0f;

    /* run the network */
    (void)simplenet_run(self, &input_d, &results);

    /* interpret the results */
    for (i = 0; i < 10; i++) {
        if (results.vec[i] > max) {
            max = results.vec[i];
            li = i;
        }
    }

    *classification = li;
}

int simplenet_serialize(simplenet *self, byte **array, int *length) {
    static const unsigned short magicnum = 0xEFBE;

    /* easy calculation here */
    *length = sizeof(unsigned short) + sizeof(simplenet);

    /* allocate the memory */
    *array = malloc(*length);
    if (!(*array)) return ERROR_OOM;

    /* magic number */
    *((short *)*array) = magicnum;

    /* copy over the data */
    memcpy(*array+sizeof(unsigned short), self, *length - sizeof(unsigned short));

    return ERROR_OK;
}

int simplenet_deserialize(byte *array, int length, simplenet **net) {
    static const unsigned short magicnum = 0xEFBE;
    byte swap_endianness = 0;

    /* check length */
    if (length != sizeof(unsigned short) + sizeof(simplenet)) return ERROR_CANTDESERIALIZE;

    /* check magic number */
    if (array[0] == 0xBE && array[1] == 0xEF) { /* little endian file saved */
        /* fetch system endianness */
        if (((byte *)&magicnum)[0] == 0xBE) {
            /* system is also little-endian, no work needed */
        } else {
            /* system is big-endian but file is little */
            swap_endianness = 1;
        }
    } else if (array[0] == 0xEF && array[1] == 0xBE) { /* big endian file saved */
        /* fetch system endianness */
        if (((byte *)&magicnum)[0] == 0xBE) {
            /* system is little-endian but file is big */
            swap_endianness = 1;
        } else {
            /* system is also big-endian, no work needed */
        }
    } else {
        /* not magic number */
        return ERROR_CANTDESERIALIZE;
    }

    /* allocate memory */
    *net = malloc(sizeof(simplenet));
    if (!(*net)) return ERROR_OOM;

    /* copy over data */
    memcpy(*net, array+2, length - sizeof(byte)*2);

    /* swap endianness if necessary */
    if (swap_endianness) {
        int i;
        double *ptr = (double *)(*net);

        for (i = 0; i < 7840; i++) {
            /* get the exact byte address */
            ptr[i] = (double)bswap_64(ptr[i]);
        }
    }

    return ERROR_OK;
}
