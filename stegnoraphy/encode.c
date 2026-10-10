#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

// Function to get the size of the file
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
// Function to read and validate the command line arguments for encoding
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Check for valid source image file
    if(argv[2]==NULL)
    {
        printf("Error: Missing source image file\n");
        printf("Usage: Encode :- ./a.out -e <source_image.bmp> <secret_file.txt/.c/.sh> <dest_image.bmp>\n");
        return e_failure;
    }

    char *ptr=strstr(argv[2],".bmp");
    // Check for valid source image file extension
    if(ptr!=NULL)
    {
        if(strcmp(ptr,".bmp")==0)
            encInfo->src_image_fname=argv[2];
        else
        {
            printf("Error: Invalid source image file \"%s\" -- file extension should be .bmp\n", argv[2]);
            return e_failure;
        }
    }
    else
    {
         printf("Error: Invalid source image file \"%s\" -- file extension should be .bmp\n", argv[2]);
         return e_failure;
    }

    // Check for valid secret file
    if(argv[3]==NULL)
    {
        printf("Error: Missing secret file\n");
        printf("Usage: Encode :- ./a.out -e <source_image.bmp> <secret_file.txt/.c/.sh> <dest_image.bmp>\n");
        return e_failure;
    }

    ptr=strrchr(argv[3],'.');
    // Check for valid secret file extension
    if(ptr!=NULL &&
       (strcmp(ptr,".txt")==0))
    {
        encInfo->secret_fname=argv[3];
        strcpy(encInfo->extn_secret_file, ptr);
    }
    else
    {
        printf("Error: Invalid secret file \"%s\" -- file extension should be .txt\n", argv[3]);
        return e_failure;
    }

    // Check for valid destination image file
    if(argv[4]==NULL)
    {
        encInfo->dest_image_fname="default.bmp";
    }
    else
    {
        ptr=strstr(argv[4],".bmp");
        if(ptr!=NULL)
        {
            if(strcmp(ptr,".bmp")==0)
                encInfo->dest_image_fname=argv[4];
            else
            {
                printf("Error: Invalid destination image file \"%s\" -- file extension should be .bmp\n", argv[4]);
                return e_failure;
            }
        }
        else
        {
            printf("Error: Invalid destination image file \"%s\" -- file extension should be .bmp\n", argv[4]);
            return e_failure;
        }
    }
    return e_success;
}

// Function to read and validate the command line arguments for decoding
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_dest_image = fopen(encInfo->dest_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->dest_image_fname);

        return e_failure;
    }
    return e_success;
}
// Function to check if the image has enough capacity to hold the secret data
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    // Check if the image has enough capacity to hold the secret data
    if((encInfo->image_capacity) > (54+16+32+32+32+(strlen(encInfo->extn_secret_file)*8)))
        return e_success;
    else
        return e_failure;
}

// Function to copy the header of the BMP image from source to destination
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    char imageBuffer[54];
    fread(imageBuffer,54,1,fptr_src_image);
    fwrite(imageBuffer,54,1,fptr_dest_image);

    // Check if the header was copied successfully
    if(ftell(fptr_src_image)==ftell(fptr_dest_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
// Function to encode the magic string into the image
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char imageBuffer[8];
    // Loop through each character of the magic string and encode it into the image
    for (size_t i = 0; i < strlen(magic_string); i++)
    {
        fread(imageBuffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i],imageBuffer);
        fwrite(imageBuffer,8,1,encInfo->fptr_dest_image);
    }
    // Check if the magic string was encoded successfully
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    else
    {
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char imageBuffer[32];

    fread(imageBuffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, imageBuffer);
    fwrite(imageBuffer, 32, 1, encInfo->fptr_dest_)
        return e_failure;
    }
}
// Function to encode the size of the secret file extension into the imageimage);
    // Check if the size of the secret file extension was encoded successfully
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
// Function to encode the secret file extension into the image
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char imageBuffer[8];
    // Loop through each character of the secret file extension and encode it into the image
    for (size_t i = 0; i < strlen(encInfo->extn_secret_file); i++)
    {
        fread(imageBuffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],imageBuffer);
        fwrite(imageBuffer,8,1,encInfo->fptr_dest_image);
    }
    // Check if the secret file extension was encoded successfully
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
// Function to encode the size of the secret file into the image
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char imageBuffer[32];

    fread(imageBuffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, imageBuffer);
    fwrite(imageBuffer, 32, 1, encInfo->fptr_dest_image);
    // Check if the size of the secret file was encoded successfully
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
// Function to encode the secret file data into the image
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    char imageBuffer[encInfo->size_secret_file];
    fread(imageBuffer,encInfo->size_secret_file,1,encInfo->fptr_secret);
    // Loop through each byte of the secret file data and encode it into the image
    for (size_t i = 0; i < encInfo->size_secret_file; i++)
    {
        char imageBuffer1[8];
        fread(imageBuffer1,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(imageBuffer[i],imageBuffer1);
        fwrite(imageBuffer1,8,1,encInfo->fptr_dest_image);
    }
    // Check if the secret file data was encoded successfully
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
// Function to encode a byte of data into the least significant bits of the image buffer
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        char bit = (data >> i) & 1;
        image_buffer[i] &= ~1;
        image_buffer[i] |= bit;
    }
    return e_success;
}
// Function to encode the size of the secret file into the least significant bits of the image buffer
Status encode_size_to_lsb(int size, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        char bit = (size >> i) & 1;
        image_buffer[i] &= ~1;
        image_buffer[i] |= bit;
    }
    return e_success;
}
// Function to copy the remaining image data from the source image to the destination image
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char imageBuffer[1];
    while(fread(imageBuffer,1,1,fptr_src)==1)
    {
        fwrite(imageBuffer,1,1,fptr_dest);
    }
    return e_success;
}
// Function to perform the encoding operation
Status do_encoding(EncodeInfo *encInfo)
{
    // Open the source image, secret file, and destination image files
    if(open_files(encInfo)==e_success)
    {
        printf("INFO : Files opened successfully\n");
        printf("INFO : DONE \n");
        // Check if the image has enough capacity to hold the secret data
        if(check_capacity(encInfo)==e_success)
        {
            printf("INFO : Image capacity is sufficient\n");
            printf("\n## Encoding Process Started ##\n");
            // Copy the header of the BMP image from source to destination
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_dest_image)==e_success)
            {
                printf("\nINFO : Header copied successfully\n");
                printf("INFO : DONE \n");
                // Encode the magic string into the image
                if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
                {
                    printf("INFO : Magic String encoded successfully\n");
                    printf("INFO : DONE \n");
                    // Encode the size of the secret file extension into the image
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success)
                    {
                        printf("INFO : Secret file extension size encoded successfully\n");
                        printf("INFO : DONE \n");
                        // Encode the secret file extension into the image
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
                        {
                            printf("INFO : Secret file extension encoded successfully\n");
                            printf("INFO : DONE \n");
                            // Encode the size of the secret file into the image
                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
                            {
                                printf("INFO : Secret file size encoded successfully\n");
                                printf("INFO : DONE \n");
                                // Encode the secret file data into the image
                                if(encode_secret_file_data(encInfo)==e_success)
                                {
                                    printf("INFO : Secret file data encoded successfully\n");
                                    printf("INFO : DONE \n");
                                    // Copy the remaining image data from the source image to the destination image
                                    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_dest_image)==e_success)
                                    {
                                        printf("Remaining image data copied successfully\n");
                                        printf("\n## Encoding Process Completed ##\n");
                                    }
                                    else
                                    {
                                        printf("INFO : Copying of remaining image data failed\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("INFO : Encoding of secret file data failed\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("INFO : Encoding of secret file size failed\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("INFO : Encoding of secret file extension failed\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("INFO : Encoding of secret file extension size failed\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("INFO : Encoding of magic string failed\n");
                    return e_failure;
                }
            }
            else
            {
                printf("INFO : Copying of header failed\n");
                return e_failure;
            }
        }
        else
        {
            printf("INFO : Image Capacity insufficient\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO : File opening failed\n");
        return e_failure;
    }
}