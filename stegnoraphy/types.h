#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;
// User defined structure to hold encoding and decoding information
typedef enum
{
    e_failure,
    e_success,
    d_failure,
    d_success
} Status;
// User defined structure to hold encoding and decoding information
typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif