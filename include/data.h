/**
 * This file is the corresponding header file for data.c
 * It defines some basic data structures for use.
 */
#ifndef __DATA_H_INC
#define __DATA_H_INC

/* FILE */
#include <stdio.h>

#define DATA_IMAGE_SIZE 28

/**
 * Type of the data file - label or image
 */
enum data_file_type {
    IMAGE,
    LABEL
};
/**
 * Handle for the data
 */
typedef struct _data_handle {
    /* file pointer */
    FILE *f;
    /* data file type */
    enum data_file_type type;
    /* number of data points available */
    int count;
} data_handle;

typedef unsigned char byte;

typedef struct _mnist_image {
    byte label;
    byte data[DATA_IMAGE_SIZE*DATA_IMAGE_SIZE];
} mnist_image;

#endif /* __DATA_H_INC */
