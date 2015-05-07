#include "Crypto.h"
#include "Crypto.cpp"
#include <errno.h>
#include <string.h>
#include "base64.h"


int readFile(char *filename, unsigned char **file, int mode) {
    FILE *fd;
    if(mode == 0)
		fd = fopen(filename, "rb");
    else
		fd = fopen(filename, "r");
    if(fd == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
        exit(1);
    }

    // Determine size of the file
    fseek(fd, 0, SEEK_END);
    size_t fileLength = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // Allocate space for the file
    *file = (unsigned char*)malloc(fileLength);
    if(*file == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }

    // Read the file into the buffer
    size_t bytesRead = fread(*file, 1, fileLength, fd);

    if(bytesRead != fileLength) {
        fprintf(stderr, "Error reading file\n");
        exit(1);
    }

    fclose(fd);

    return fileLength;
}

void writeFile(char *filename, unsigned char *file, size_t fileLength) {
    FILE *fd = fopen(filename, "wb");
    if(fd == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
        exit(1);
    }

    size_t bytesWritten = fwrite(file, 1, fileLength, fd);

    if(bytesWritten != fileLength) {
        fprintf(stderr, "Failed to write file\n");
        exit(1);
    }

    fclose(fd);
}

int decryptFile(char* encryptedFilename, char* keyfileName, char* keyivfileName, int& decryptedFileLength)
{
	unsigned char key_file[] = {79, 138, 126, 232, 102, 56, 154, 43, 169, 225, 199, 175, 35, 135, 123, 21, 255, 196, 44, 254, 115, 206, 33, 145, 188, 40, 40, 211, 118, 34, 83 , 63};
	//size_t key_length = readFile(keyfileName, &key_file, 0);
	size_t key_length =  32;
	
	unsigned char key_iv_file[] = {5, 178, 209, 178, 37, 195, 104, 154, 150, 145, 4, 70, 138, 96, 7, 212};
	//size_t key_iv_length = readFile(keyivfileName, &key_iv_file, 0);
	size_t key_iv_length = 16;
	
	// Create our crypto object
    Crypto crypto;
    
    if(crypto.setAESKey(key_file, key_length) != 0)
		fprintf(stderr, "key failed\n");
    if(crypto.setAESIv(key_iv_file, key_iv_length) != 0)
		fprintf(stderr, "key iv failed\n");
   
	unsigned char *file;
    // Read the encrypted file back
    size_t fileLength = readFile(encryptedFilename, &file, 0);

    #ifdef CONVERT_TO_BASE64
        // Decode the encrypted file from base64
        unsigned char *binaryBuffer;
        fileLength = base64Decode((char*)file, fileLength, &binaryBuffer);

        // Change the pointer of the string containing the file info to the decoded base64 string
        free(file);
        file = binaryBuffer;
    #endif

    // Decrypt the encrypted file
    unsigned char *decryptedFile;
    if((decryptedFileLength = crypto.aesDecrypt(file, fileLength, &decryptedFile)) == -1) {
        fprintf(stderr, "Decryption failed\n");
        return 0;
    }
   // printf("%d bytes decrypted\n", (int)decryptedFileLength);

    // Append .dec to the filename
    char *decryptedFilename = (char*)malloc(strlen(encryptedFilename) + 5);
    if(decryptedFilename == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }
    sprintf(decryptedFilename, "%s.dec", encryptedFilename);
    
    // Write the decrypted file to its own file
    writeFile(decryptedFilename, decryptedFile, decryptedFileLength);

    free(decryptedFile);
    free(file);

    return 1;
}
