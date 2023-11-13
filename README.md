# Text-file-compressor-using-Huffman-encoding

# Overview
This C++ program implements Huffman coding, a widely used algorithm for lossless data compression. Huffman coding is efficient for compressing files by assigning variable-length codes to input characters based on their frequencies in the file. It works on the principle of assigning smaller bit lengths to more frequent data, thus decreasing the average length of encoded file.

The program provides functionalities for both compression and decompression.

# Usage
compile the code using a cpp compiler or using command line: g++ -o main compdecomp.cpp    
It will create the main.exe file.

The program is used from the command line and supports two operations:

Compress:
./main compress input_file output_file

Decompress:
./main decompress input_file output_file


Replace input_file and output_file with the desired file names. The program will display a message indicating whether the operation was successful.

# File Extensions
Uncompressed File Extension: .txt

Compressed File Extension: .huf





