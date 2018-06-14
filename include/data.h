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
    byte data[DATA_IMAGE_SIZE*DATA_IMAGE_SIZE];
    byte label;
} mnist_image;

int data_open(const char *location, data_handle **handle);
int data_read(data_handle *handle_image, data_handle *handle_label, int index, mnist_image **image);
void data_cleanup(data_handle *handle);

#endif /* __DATA_H_INC */
