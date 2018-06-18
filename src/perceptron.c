/* this file implements a simple perceptron network and some utility functions */

/* malloc(), free(), rand(), etc. */
#include <stdlib.h>
/* time() */
#include <time.h>
/* memcpy() */
#include <string.h>

#include "error.h"
#include "perceptron.h"

#ifdef __I_KNOW_WHAT_IM_DOING

/**
 * Initialize a single neuron with random weights
 */
int neuron_perceptron_init(int num_inputs, neuron_perceptron **neuron) {
    int i;

    /* allocate all memory required */
    *neuron = malloc(sizeof(neuron_perceptron) + sizeof(double) * num_inputs);
    if (!(*neuron)) return ERROR_OOM;

    /* set number of inputs and array location */
    (*neuron)->n = num_inputs;
    (*neuron)->weights = *neuron + sizeof(neuron_perceptron);

    /* initialize neurons to random values */
    for (i = 0; i < num_inputs; i++) {
        (*neuron)->weights[i] = rand()/((double)(RAND_MAX));
    }

    return ERROR_OK;
}

/**
 * Initialize a layer of neurons (but not each individual neuron)
 */
int layer_perceptron_init(int num_neurons, layer_perceptron **layer) {
    /* allocate memory for layer */
    *layer = malloc(sizeof(layer_perceptron));
    if (!(*layer)) return ERROR_OOM;

    (*layer)->n = num_neurons;

    /* allocate memory for neurons */
    (*layer)->neurons = malloc(sizeof(neuron_perceptron) * num_neurons);
    if (!((*layer)->neurons)) goto cleanup;
    allocations[allocations_n++] = (*layer)->neurons;

    /* allocate memory for each set of weights */
    for (i = 0; i < num_neurons; i++) {
        /* get current neuron pointer */
        neuron = (*layer)->neurons + i;

        neuron->n = num_inputs;

        /* allocate memory for weights */
        neuron->weights = malloc(sizeof(double) * num_inputs);
        if (!(neuron->weights)) goto cleanup;
        allocations[allocations_n++] = neuron->weights;

        /* initialize the weights to random numbers */
        for (j = 0; j < num_inputs; j++)
            neuron->weights[j] = rand()/((double)(RAND_MAX));
    }

    /* all done! */
    return ERROR_OK;

cleanup:
    /* free all memory allocations */
    for (i = 0; i < allocations_n; i++) {
        free(allocations[i]);
    }
    free(allocations);

    return ERROR_OOM;
}

int network_perceptron_init() {
    
}


#else

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
        if (self->vec[i] > 1.0f) self->vec[i] = 1;
        else if (self->vec[i] < -1.0f) self->vec[i] = -1;
        /*printf("%f = %f * %d * %f\n", self->vec[i], RATE, input->vec[i], error);*/
    }

    return ERROR_OK;
}

int simplenet_run(simplenet *self, mnist_image *input, vec10d *output) {
    byte i;

    /*
     * this is the equivalent of multiplying a 1x784 matrix (input) by a 784x10 matrix (weights)
     * (the only missing step is dividing each output by 784)
     * one day, I could use BLAS to multiply the matrices and maybe it will be faster.
     * for now, with -O3, I reach speeds of ~3s to train all 60k images, which is
     * fast enough :)
     */

    /* calculate for each neuron */
    for (i = 0; i < 10; i++) {
        (void)simplenet_vec784d_run(&self->neurons[i], (vec784 *)&input->data, &output->vec[i]);
    }

    return ERROR_OK;
}

int simplenet_train(simplenet *self, mnist_image *input) {
    byte i;
    vec10d neuron_outputs;
    double error;

    /* run the network to see what the results are */
    simplenet_run(self, input, &neuron_outputs);

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
    (void)simplenet_run(self, input, &results);

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
    /* easy calculation here */
    *length = sizeof(byte)*2 + sizeof(simplenet);

    /* allocate the memory */
    *array = malloc(*length);
    if (!(*array)) return ERROR_OOM;

    /* magic number */
    (*array)[0] = 0xBE;
    (*array)[1] = 0xEF;

    /* copy over the data */
    memcpy(*array+2, self, *length - sizeof(byte)*2);

    return ERROR_OK;
}

int simplenet_deserialize(byte *array, int length, simplenet **net) {
    /* check length */
    if (length != sizeof(byte)*2 + sizeof(simplenet)) return ERROR_CANTDESERIALIZE;

    /* check magic number */
    if (array[0] != 0xBE || array[1] != 0xEF) return ERROR_CANTDESERIALIZE;

    /* allocate memory */
    *net = malloc(sizeof(simplenet));
    if (!(*net)) return ERROR_OOM;

    /* copy over data */
    memcpy(*net, array+2, length - sizeof(byte)*2);

    return ERROR_OK;
}

#endif

