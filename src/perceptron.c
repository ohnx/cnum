/* this file implements a simple perceptron network and some utility functions */

/* malloc(), free(), rand(), etc. */
#include <stdlib.h>

#include "error.h"
#include "perceptron.h"

int layer_perceptron_init(int num_neurons, int num_inputs, layer_perceptron **layer) {
    neuron_perceptron *neuron;
    void **allocations;
    int i, j, allocations_n = 0;

    /* allocate memory to keep track of allocations */
    allocations = malloc(sizeof(void *) * (2 + num_neurons));
    if (!allocations) return ERROR_OOM;

    /* allocate memory for layer */
    *layer = malloc(sizeof(layer_perceptron));
    if (!(*layer)) goto cleanup;
    allocations[allocations_n++] = *layer;

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
