#ifndef __ERROR_H_INC
#define __ERROR_H_INC

/* no error */
#define ERROR_OK                0
/* error reading file */
#define ERROR_READFILE          1
/* out of memory */
#define ERROR_OOM               2
/* invalid or corrupt file */
#define ERROR_INVALIDFILE       4
/* non-standard (28x28) image size */
#define ERROR_NONSTANDARDFILE   8
/* invalid input parameters */
#define ERROR_INVALIDINPUT      16

#endif /* __ERROR_H_INC */
