#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *dest_image_fname; // To store the dest file name
    FILE *fptr_dest_image;  // To store the address of stego image

    /* Secret File Info */
    int extn_size; // To store the size of the secrect file extension
    char extn_secret_file[32]; // To store the Secret file extension
    int size_secret_file; // To store the size of the secret data
} DecodeInfo;

/* Decode Functions prototypes */

/*Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform decoding */
Status do_decoding(char *argv[],DecodeInfo *decInfo);

/* open files for decoding */
Status open_files_decode(DecodeInfo *decInfo);

/* Skip bmp header */
Status skip_bmp_header(DecodeInfo *decInfo);

/* Decode magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extn size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extn */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(char *argv[],DecodeInfo *decInfo);

/* Decode byte from lsb ofdest image */
Status decode_byte_from_lsb(char *data, char *image_buffer);

/* Decode size from lsb of dest image */
Status decode_size_from_lsb(int *size, char *image_buffer);

#endif
