/* 
   STEGANOGRAPHY 

Name        : Santosh Tarnallikar
Date        : 24/07/2026
Batch No    : 26005B

Description :
-------------
This project implements a Steganography system in C
using the Least Significant Bit (LSB) technique to hide and extract secret data inside a BMP image. 
It supports encoding a text file into an image and decoding it back using command-line arguments.
The system ensures data integrity by using a magic string for verification. 
It also handles file extension and file size during the process. 
Proper validation and file handling are implemented to ensure reliable and secure operation.

Features:
---------
1. Encoding (Hiding Data)
   -Reads and validates input arguments for encoding
    -Opens source image, secret file, and output stego file
    -Accepts magic string from user for data verification
    -Checks image capacity 
    -Copies BMP header to preserve image structure
    -Encodes magic string into image using LSB technique
    -Encodes secret file extension and its size
    -Encodes secret file size and actual file data
    -Copies remaining image data to output file
    -Closes all files after successful encoding
2. Decoding (Extracting Data)
    - Extracts hidden data from stego BMP image
    - Uses LSB (Least Significant Bit) technique
    - Verifies magic string for authentication
    - Decodes file extension and file size
    - Reconstructs original secret file from image

3. Command-Line Based Execution
   - Uses arguments like -e for encoding and -d for decoding
   - Example:
     ./a.out -e input.bmp secret.txt
     ./a.out -d stego.bmp

4. File Handling
   - Opens files in binary mode
   - Reads and writes image and secret data properly
   - Generates output file with correct extension

5. Validation
   - Accepts only .bmp files for image input
   - Accepts only .txt file for encoding
   - Validates magic string during decoding
   - Checks for invalid file size and extension

6. Error Handling
   - Handles file opening errors
   - Handles invalid arguments
   - Handles insufficient image capacity
   - Displays clear success and error messages

===========================================================
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
OperationType check_operation_type(char *);

// Main function used to perform encoding and decoding of secret files into bmp images
int main(int argc, char *argv[])
{
   EncodeInfo encodeInfo;
   DecodeInfo decodeInfo;

   // Check for valid number of arguments less than 2
   if(argc < 2)
   {
      printf("Error: Invalid number of arguments\n");
      printf("Usage: Encode :- ./a.out -e <source_image.bmp> <secret_file.txt> <dest_image.bmp>\n"); 
      printf("Usage: Decode :- ./a.out -d <stego_image.bmp> <output_secret_file.txt>\n");  
      return 0;
   }
   // Check for valid number of arguments greater than 5
   else if(argc > 5)
   {
      printf("Error: Invalid number of arguments\n");
      printf("Usage: Encode :- ./a.out -e <source_image.bmp> <secret_file.txt> <dest_image.bmp>\n"); 
      printf("Usage: Decode :- ./a.out -d <stego_image.bmp> <output_secret_file.txt>\n");  
      return 0;
   }
   // Check for valid operation type
   if(check_operation_type(argv[1]) == e_unsupported)
   {
      printf("Error: Invalid option \"%s\"\n", argv[1]);
      printf("Please give a proper input for operation:\n");
      printf("  -e : Encode (hide a secret file inside a .bmp image)\n");
      printf("  -d : Decode (extract a hidden secret file from a .bmp image)\n");
      return 0;
   }

   // Encoding branch: needs at least 4 args (a.out -e src.bmp secret.txt [dest.bmp])
   if(check_operation_type(argv[1]) == e_encode)
   {
      if(argc >= 4 && read_and_validate_encode_args(argv, &encodeInfo) == e_success)
      {
         // Perform encoding operation
         do_encoding(&encodeInfo);
      }
      else
      {
         printf("Error: Invalid arguments\n");
         printf("Usage: Encode :- ./a.out -e <source_image.bmp> <secret_file.txt> <dest_image.bmp>\n");
         return 0;
      }
   }
   // Decoding branch: needs at least 3 args (a.out -d stego.bmp [output_name])
   else if(check_operation_type(argv[1]) == e_decode)
   {
      if(argc >= 3 && read_and_validate_decode_args(argv, &decodeInfo) == d_success)
      {
         // Perform decoding operation
         do_decoding(argv, &decodeInfo);
      }
      else
      {
         printf("Error: Invalid arguments\n");
         printf("Usage: Decode :- ./a.out -d <stego_image.bmp> <output_secret_file.txt>\n");
         return 0;
      }
   }
   else
   {
      printf("Error: Invalid arguments\n");
      return 0;
   }

   return e_success;
}

// Function to check the operation type based on the symbol provided 
OperationType check_operation_type(char *symbol)
{
   // Check for encoding operation 
   if(strcmp(symbol, "-e")==0)
   {
    return e_encode;
   }
   // Check for decoding operation
   else if(strcmp(symbol, "-d")==0)
   {
    return e_decode;
   }
   else
   {
    return e_unsupported;
   }
}