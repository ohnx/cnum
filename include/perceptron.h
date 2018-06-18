/**
 * This file is the corresponding header for perceptron.c
 * It defines the required data structures and external functions.
 */

#ifndef __PERCEPTRON_H_INC
#define __PERCEPTRON_H_INC

#ifdef __I_KNOW_WHAT_IM_DOING
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

#else

#include "data.h"

typedef struct __attribute__((packed)) _vec784 {
    byte vec[784];
} vec784;

typedef struct __attribute__((packed)) _vec784d {
    double vec[784];
} vec784d;

typedef struct __attribute__((packed)) _simplenet {
    vec784d neurons[10];
} simplenet;

typedef struct __attribute__((packed)) _vec10d {
    double vec[10];
} vec10d;

typedef struct __attribute__((packed)) _vec10 {
    byte vec[10];
} vec10;

int simplenet_init(simplenet **net);
int simplenet_train(simplenet *self, mnist_image *input);
int simplenet_classify(simplenet *self, mnist_image *input, byte *classification);
int simplenet_serialize(simplenet *self, byte **array, int *length);
int simplenet_deserialize(byte *array, int length, simplenet **net);

#endif

#endif /* __PERCEPTRON_H_INC */
