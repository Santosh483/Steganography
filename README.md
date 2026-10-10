# Steganography

🖼️ Image Steganography Project in C
📌 Project Overview

Image Steganography is a technique used to hide secret information inside an image without significantly changing its appearance.

This project implements Least Significant Bit (LSB) steganography using the C programming language. It allows users to encode a secret message into a BMP image and decode the hidden message from the encoded image.

The secret data is embedded into the least significant bits of image pixel data, making the changes difficult to notice visually.

🎯 Objectives
Understand the concept of image steganography.
Learn file handling in C.
Implement encoding and decoding techniques.
Hide secret text inside an image.
Retrieve the hidden message from an encoded image.
Understand bitwise operations and binary data manipulation.
✨ Features
Encoding: Hide a secret message inside a BMP image.
Decoding: Extract the hidden message from an encoded image.
File Validation: Validate input files and file extensions.
Capacity Check: Verify whether the image can store the secret message.
Output Generation: Generate a new image containing the hidden data.
Magic String Validation: Use a predefined signature to identify encoded data during decoding.
🛠️ Technologies Used
Programming Language: C
File Format: BMP images
Concepts: File Handling, Bitwise Operations, Pointers, Structures, Command-Line Arguments
Compiler: GCC
📂 Project Structure
Steganography/
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── types.h
├── common.h
├── test_encode.bmp
├── secret.txt
├── output.bmp
└── README.md


Note: The filenames may vary depending on your implementation.

⚙️ Compilation

Compile the source files using GCC:

gcc *.c -o steganography

▶️ Execution
1. Encoding

Use the following command to hide a secret message in an image:

./steganography -e input.bmp secret.txt output.bmp

-e: Selects encoding mode.
input.bmp: Original image used to hide the message.
secret.txt: File containing the secret message.
output.bmp: Generated image containing the hidden data.
2. Decoding

Extract the hidden message using:

./steganography -d output.bmp decoded.txt

-d: Selects decoding mode.
output.bmp: Image containing the encoded message.
decoded.txt: Destination file for the extracted message.

The command-line arguments may need adjustment to match your program.

🔍 How It Works
Encoding Process
Read and validate the input BMP image.
Open the secret file and determine its size.
Copy the BMP header and required image data to the output file.
Embed a predefined magic string to identify the hidden message.
Encode the secret file extension and size, if required.
Convert the secret message bytes into bits.
Embed the bits into the least significant bits of the image data.
Save the resulting stego image.
Decoding Process
Open the encoded BMP image.
Read the image data.
Extract the magic string and validate the encoded content.
Decode the secret file extension and size, if applicable.
Extract the hidden message bits.
Reconstruct the original message bytes.
Write the recovered message into the output file.
🧠 LSB Technique

In the Least Significant Bit technique, the last bit of a byte is modified to store one bit of secret information.

For example:

Original pixel byte:  10110100
Secret data bit:              1
Modified pixel byte: 10110101


Only the least significant bit changes, so the numerical difference is just one. When applied across image data, these small changes are generally difficult to notice.

🧪 Example

Secret file (secret.txt)

Hello! This is a secret message.


Encoding command

./steganography -e input.bmp secret.txt output.bmp


Decoding command

./steganography -d output.bmp decoded.txt


Recovered message (decoded.txt)

Hello! This is a secret message.

📚 Concepts Learned
Image file processing
LSB encoding and decoding
Bitwise operators and bit manipulation
File pointers and binary file operations
Structures and modular programming
Command-line argument handling
Error handling and input validation
Capacity calculation and data reconstruction
🚀 Future Enhancements
Support PNG and other image formats.
Encrypt the secret message before encoding.
Hide binary files in addition to text files.
Improve payload capacity and encoding efficiency.
Add password-based message extraction.
⚠️ Limitations
The image must have sufficient capacity for the secret data.
Modifying or compressing the encoded image may corrupt the hidden message.
LSB steganography alone does not encrypt the secret information.
This implementation supports BMP images only, if BMP is the sole format handled by the program.
👨‍💻 Author

Name: Santosh

Project: Image Steganography

Language: C

📄 License

This project is intended for educational purposes and can be modified and extended for learning and experimentation.
