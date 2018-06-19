/**
 * This file is the corresponding header for perceptron.c
 * It defines the required data structures and external functions.
 */

#ifndef __PERCEPTRON_H_INC
#define __PERCEPTRON_H_INC

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
void simplenet_train(simplenet *self, mnist_image *input);
void simplenet_classify(simplenet *self, mnist_image *input, byte *classification);
int simplenet_serialize(simplenet *self, byte **array, int *length);
int simplenet_deserialize(byte *array, int length, simplenet **net);

#endif /* __PERCEPTRON_H_INC */
