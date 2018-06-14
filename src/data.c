/* this file contains methods to parse the MNIST data */

/* FILE, NULL, fopen(), fclose(), fread(), etc. */
#include <stdio.h>
/* malloc, free */
#include <stdlib.h>
/* ntohl() */
#include <arpa/inet.h>

/* error codes */
#include "error.h"
/* structs */
#include "data.h"

/* sizes of the headers, in bytes */
#define IMAGE_HEADER_SIZE 16L
#define LABEL_HEADER_SIZE 8L

/**
 * Open a new file as a data source
 */
int data_open(const char *location, data_handle **handle) {
    FILE *fp;
    int magicnum, rcn;
    long int size;
    size_t r;

    /* read file */
    fp = fopen(location, "rb");
    if (fp == NULL) return ERROR_READFILE;

    /* allocate memory */
    *handle = malloc(sizeof(handle));
    if (!(*handle)) { fclose(fp); return ERROR_OOM; }
    (*handle)->f = fp;

    /* read header magic number */
    r = fread(&magicnum, sizeof(int), 1, fp);
    if (r != 1) { fclose(fp); free(*handle); return ERROR_READFILE; }

    /* check header magic number */
    magicnum = ntohl(magicnum); /* file is big-endian */
    if (magicnum == 0x00000803) (*handle)->type = IMAGE;
    else if (magicnum == 0x00000801) (*handle)->type = LABEL;
    else { fclose(fp); free(*handle); return ERROR_INVALIDFILE; }

    /* read number of values */
    r = fread(&((*handle)->count), sizeof(int), 1, fp);
    if (r != 1) { fclose(fp); free(*handle); return ERROR_READFILE; }
    (*handle)->count = ntohl((*handle)->count); /* file is big-endian */

    /* check if image size is compatible */
    if ((*handle)->type == IMAGE) {
        /* TODO: code duplication */
        r = fread(&rcn, sizeof(int), 1, fp);
        if (r != 1) { fclose(fp); free(*handle); return ERROR_READFILE; }
        rcn = ntohl(rcn);
        if (rcn != DATA_IMAGE_SIZE) return ERROR_NONSTANDARDFILE;

        r = fread(&rcn, sizeof(int), 1, fp);
        if (r != 1) { fclose(fp); free(*handle); return ERROR_READFILE; }
        rcn = ntohl(rcn);
        if (rcn != DATA_IMAGE_SIZE) return ERROR_NONSTANDARDFILE;
    }

    /* time to check if the file is as large as it should be! */
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    if ((*handle)->type == IMAGE) {
        /* image */
        size -= IMAGE_HEADER_SIZE;
        /* 28x28 always */
        size -= (long int)(DATA_IMAGE_SIZE*DATA_IMAGE_SIZE) * (((long int)(*handle)->count));
        if (size != 0) {
            fclose(fp);
            free(*handle);
            return ERROR_INVALIDFILE;
        }
    } else {
        /* label */
        size -= LABEL_HEADER_SIZE;
        size -= (*handle)->count;
        if (size != 0) {
            fclose(fp);
            free(*handle);
            return ERROR_INVALIDFILE;
        }
    }

    /* at this point the file has been parsed successfully */
    return ERROR_OK;
}

/**
 * Read the nth image from the data
 */
int data_read(data_handle *handle_image, data_handle *handle_label, int index, mnist_image **image) {
    int r;
    size_t n;

    /* ensure the handles are what they should be */
    if (handle_image->type != IMAGE || handle_label->type != LABEL) return ERROR_INVALIDINPUT;

    /* ensure there are enough images in the files */
    if (handle_image->count < index || handle_label->count < index) return ERROR_INVALIDINPUT;

    /* seek to the correct index for image */
    r = fseek(handle_image->f,
        IMAGE_HEADER_SIZE + (long int)(DATA_IMAGE_SIZE * DATA_IMAGE_SIZE) * (index),
        SEEK_SET);
    if (r) return ERROR_READFILE;

    /* seek to the correct index for labels */
    r = fseek(handle_label->f,
        LABEL_HEADER_SIZE + index,
        SEEK_SET);
    if (r) return ERROR_READFILE;

    /* allocate the memory for the data */
    *image = malloc(sizeof(mnist_image));
    if (!(*image)) return ERROR_OOM;

    /* read image data */
    n = fread(&((*image)->data), DATA_IMAGE_SIZE * DATA_IMAGE_SIZE, 1, handle_image->f);
    if (n != 1) {
        free(*image);
        return ERROR_READFILE;
    }

    /* read label data */
    n = fread(&((*image)->label), 1, 1, handle_label->f);
    if (n != 1) {
        free(*image);
        return ERROR_READFILE;
    }

    /* all done */
    return ERROR_OK;
}

/**
 * Clean up everything
 */
void data_cleanup(data_handle *handle) {
    fclose(handle->f);
    free(handle);
}
