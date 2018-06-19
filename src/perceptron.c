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

int simplenet_vec784d_run(vec784d *self, vec784 *input, double *output) {
    double runningtotal = 0;
    int i;

    /* loop through each weight and multiply the weight by the input value */
    for (i = 0; i < 784; i++) {
        runningtotal += ((double)input->vec[i] / 255.0f) * self->vec[i];
    }

    /* average */
    *output = runningtotal / 784.0;

    return ERROR_OK;
}

/* 0.02 reaches 79.5% accuracy in the first round and 80.1% in the second. */
#define RATE 0.02
int simplenet_vec784d_learnsup(vec784d *self, vec784 *input, double error) {
    int i = 0;

    /* loop through each weight and increase or decrease the weight */
    for (i = 0; i < 784; i++) {
        self->vec[i] += RATE * ((double)input->vec[i] / 255.0f) * error;
        /* temporary fix... clip the max/min weights */
        /*if (self->vec[i] > 1.0f) self->vec[i] = 1;
        else if (self->vec[i] < -1.0f) self->vec[i] = -1;*/
        /*printf("%f = %f * %d * %f\n", self->vec[i], RATE, input->vec[i], error);*/
    }

    return ERROR_OK;
}

int simplenet_run(simplenet *self, vec784 *input, vec10d *output) {
    int i;
    vec784d input_d;

    /*
     * this is the equivalent of multiplying a 1x784 matrix (input) by a 784x10 matrix (weights)
     * (the only missing step is dividing each output by 784)
     * one day, I could use BLAS to multiply the matrices and maybe it will be faster.
     * for now, with -O3, I reach speeds of ~3s to train all 60k images, which is
     * fast enough :)
     */

    /* convert the input */
    for (i = 0; i < 784; i++)
        input_d.vec[i] = (double)input->vec[i]/255.0f;

    /* clear the input */
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
        1.0/784, /* No scaling factor for now; TODO: scale to 1/784 */
        (double *)self, /* Matrix A */
        784, /* A[m][n] = A[m*784 + n] */
        (double *)&input_d, /* vector X should be a 784x1 matrix */
        1, /* no skip */
        0.0, /* scaling factor for vector Y is 0 since we don't use it */
        (double *)output, /* output vector Y */
        1 /* no skip */
    );

    return ERROR_OK;
}

int simplenet_train(simplenet *self, mnist_image *input) {
    byte i;
    /*static vec784d */
    vec10d neuron_outputs;
    double error;

    /* run the network to see what the results are */
    simplenet_run(self, (vec784 *)input->data, &neuron_outputs);

    /* loop through and perform corrections as necessary */
    for (i = 0; i < 10; i++) {
        /* error is how far away from the correct answer this was */
        error = (input->label == i ? 1 : 0) - neuron_outputs.vec[i];
        /*printf("output: %f; expected: %d; error: %f\n", neuron_outputs.vec[i], input->label == i, error);*/
        (void)simplenet_vec784d_learnsup(&self->neurons[i], (vec784 *)&input->data, error);
    }

    return ERROR_OK;
}

int simplenet_classify(simplenet *self, mnist_image *input, byte *classification) {
    vec10d results;
    double max = 0;
    byte i, li = 0;

    /* run the network */
    (void)simplenet_run(self, (vec784 *)input->data, &results);

    /* interpret the results */
    for (i = 0; i < 10; i++) {
        if (results.vec[i] > max) {
            max = results.vec[i];
            li = i;
        }
    }

    *classification = li;

    return ERROR_OK;
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
