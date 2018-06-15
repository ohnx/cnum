/**
 * This file is the corresponding header for perceptron.c
 * It defines the required data structures and external functions.
 */

#ifndef __PERCEPTRON_H_INC
#define __PERCEPTRON_H_INC

typedef struct _neuron_perceptron {
    /* number of inputs (also number of weights) */
    int n;
    /* pointer to first element in an array of the weights */
    double *weights;
} neuron_perceptron;

typedef struct _layer_perceptron {
    /* number of perceptrons in this layer */
    int n;
    /* pointer to first element in array of perceptrons */
    neuron_perceptron *neurons;
} layer_perceptron;

typedef struct _network_perceptron {
    /* number of layers in this network */
    int n;
    /* pointer to first element in array of layers */
    layer_perceptron *layers;
} network_perceptron;

#endif /* __PERCEPTRON_H_INC */
