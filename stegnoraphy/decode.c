#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "common.h"
#include "types.h"

/* Function definations */
// Function to read and validate the command line arguments for decoding
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *ptr=strstr(argv[2],".bmp");
    // Check for valid stego image file extension
    if(ptr!=NULL)
    {
        // Check if the stego image file extension is .bmp
        if(strcmp(ptr,".bmp")==0)
            decInfo->dest_image_fname=argv[2];
        else
        {
            printf("Error: Invalid stego image file \"%s\" -- file extension should be .bmp\n", argv[2]);
            return d_failure;
        }
    }
    else
    {
        printf("Error: Invalid stego image file \"%s\" -- file extension should be .bmp\n", argv[2]);
        return d_failure;
    }

    return d_success;
}

// Function to open the stego image file for decoding
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_dest_image = fopen(decInfo->dest_image_fname, "r");
    // Do Error handling
    if(decInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->dest_image_fname);

        return d_failure;
    }
    return d_success;
}

// Function to skip the BMP header in the stego image file
Status skip_bmp_header(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_dest_image, 54, SEEK_SET);
    // Check if the file pointer is at the correct position after skipping the header
    if(ftell(decInfo->fptr_dest_image)==54)
    {
        return d_success;
    }
    else
    {
        return d_failure;
    }
}

// Function to decode the magic string from the stego image file
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char imageBuffer[8];
    for(int i=0;i<strlen(magic_string);i++)
    {
        fread(imageBuffer,8,1,decInfo->fptr_dest_image);
        char data;
        decode_byte_from_lsb(&data,imageBuffer);
        // Check if the decoded character matches the expected magic string character
        if(data!=magic_string[i])
        {
            return d_failure;
        }
    }
    return d_success;
}

// Function to decode the size of the secret file extension from the stego image file
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char imageBuffer[32];
    // Read 32 bytes from the stego image file into the image buffer
    if (fread(imageBuffer, 32, 1, decInfo->fptr_dest_image) != 1)
        return d_failure;
    int* size= &(decInfo->extn_size);
    decode_size_from_lsb(size, imageBuffer);
    return d_success;
}

// Function to decode the secret file extension from the stego image file
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char imageBuffer[8];
    // Read the secret file extension from the stego image file into the DecodeInfo structure
    for(int i=0;i<decInfo->extn_size;i++)
    {
        fread(imageBuffer,8,1,decInfo->fptr_dest_image);
        char data;
        decode_byte_from_lsb(&data,imageBuffer);
        decInfo->extn_secret_file[i]=data;
    }
    decInfo->extn_secret_file[decInfo->extn_size]='\0';
    return d_success;
}

// Function to decode the size of the secret file from the stego image file
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char imageBuffer[32];
    fread(imageBuffer,32,1,decInfo->fptr_dest_image);
    decode_size_from_lsb(&(decInfo->size_secret_file),imageBuffer);

    return d_success;
}

// Function to decode the secret file data from the stego image file and write it to the output file
Status decode_secret_file_data(char *argv[], DecodeInfo *decInfo)
{
    // Check if the size of the secret file is valid
    if (decInfo->size_secret_file < 0)
        return d_failure;

    char *imageBuffer = malloc((size_t)decInfo->size_secret_file);
    // Check if memory allocation was successful
    if (decInfo->size_secret_file > 0 && imageBuffer == NULL)
        return d_failure;

    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        char imageBuffer1[8];
        // Read 8 bytes from the stego image file into the image buffer
        if (fread(imageBuffer1, 8, 1, decInfo->fptr_dest_image) != 1)
        {
            free(imageBuffer);
            return d_failure;
        }
        char data;
        decode_byte_from_lsb(&data, imageBuffer1);
        imageBuffer[i] = data;
    }

    char ptr[260];
    // Check if the output file name is provided in the command line arguments  
    if (argv[3] != NULL)
    {
        strncpy(ptr, argv[3], sizeof(ptr) - 1);
        ptr[sizeof(ptr) - 1] = '\0';

        // Check if the output file name already has an extension
        if (strchr(ptr, '.') == NULL)
        {
            strncat(ptr, decInfo->extn_secret_file, sizeof(ptr) - strlen(ptr) - 1);
        }
        else
        {
            strncat(ptr, decInfo->extn_secret_file, sizeof(ptr) - strlen(ptr) - 1);
        }
    }
    else
    {
        strncpy(ptr, "default", sizeof(ptr) - 1);
        ptr[sizeof(ptr) - 1] = '\0';
        strncat(ptr, decInfo->extn_secret_file, sizeof(ptr) - strlen(ptr) - 1);
    }

    FILE *fptr_secret = fopen(ptr, "wb");
    // Check if the output file was opened successfully
    if (fptr_secret == NULL)
    {
        free(imageBuffer);
        return d_failure;
    }

    fwrite(imageBuffer, decInfo->size_secret_file, 1, fptr_secret);
    fclose(fptr_secret);
    free(imageBuffer);

    return d_success;
}

// Function to decode a byte of data from the least significant bits of the image buffer
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data=0;
    for(int i=0;i<8;i++)
    {
        *data|=(image_buffer[i]&1)<<i;
    }
    return d_success;
}

// Function to decode the size of the secret file from the least significant bits of the image buffer
Status decode_size_from_lsb(int *size, char *image_buffer)
{
    *size=0;
    for(int i=0;i<32;i++)
    {
        *size|=(image_buffer[i]&1)<<i;
    }
    return d_success;
}

// Function to perform the decoding operation
Status do_decoding(char *argv[],DecodeInfo *decInfo)
{
    // Open the stego image file for decoding
    if(open_files_decode(decInfo)==d_success)
    {
        printf("INFO : Files opened successfully\n");
        printf("INFO : Decoding Process Started ##\n");

        // Skip the BMP header in the stego image file
        if(skip_bmp_header(decInfo)==d_success)
        {
            printf("INFO : Header skipped successfully\n");
            printf("INFO : DONE \n");
            // Decode the magic string from the stego image file
            if(decode_magic_string(MAGIC_STRING,decInfo)==d_success)
            {
                printf("INFO : Magic String decoded successfully\n");
                printf("INFO : DONE \n");
                // Decode the size of the secret file extension from the stego image file
                if(decode_secret_file_extn_size(decInfo)==d_success)
                {
                    printf("INFO : Secret file extension size decoded successfully\n");
                    printf("INFO : DONE \n");
                    // Decode the secret file extension from the stego image file 
                    if(decode_secret_file_extn(decInfo)==d_success)
                    {
                        printf("INFO : Secret file extension decoded successfully\n");
                        printf("INFO : DONE \n");
                        // Decode the size of the secret file from the stego image file
                        if(decode_secret_file_size(decInfo)==d_success)
                        {
                            printf("INFO : Secret file size decoded successfully\n");
                            printf("INFO : DONE \n");
                            // Decode the secret file data from the stego image file and write it to the output file
                            if(decode_secret_file_data(argv,decInfo)==d_success)
                            {
                                printf("INFO : Secret file data decoded successfully\n");
                                printf("INFO : DONE \n");
                                printf("## Decoding Process Completed ##\n");
                                return d_success;
                            }
                            else
                            {
                                printf("INFO : Secret file data decoding failed\n");
                                return d_failure;
                            }
                        }
                        else
                        {
                            printf("INFO : Secret file size decoding failed\n");
                            return d_failure;
                        }
                        
                    }
                    else
                    {
                        printf("INFO : Secret file extension decoding failed\n");
                        return d_failure;
                    }
                }
                else
                {
                    printf("INFO : Secret file extension size decoding failed\n");
                    return d_failure;
                }
            }
            else
            {
                printf("INFO : Magic String decoding failed\n");
                return d_failure;
            }
        }
        else
        {
            printf("INFO : Header skipping failed\n");
            return d_failure;
        }
    }
    else
    {
        printf("INFO : File opening failed\n");
        return d_failure;
    }
}