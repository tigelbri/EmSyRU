#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "base64.h"
#include "Crypto.h"

// Uncomment to write the encrypted file as base64
//#define CONVERT_TO_BASE64

// Note: This isn't a good way to encrypt large file (anything that can't be read into
// memory in a single buffer). A better approach for this is to read in one block at a type,
// encrypt it, write it to a file and so on.

using namespace std;

void writeFile(char *filename, unsigned char *file, size_t fileLength);
int readFile(char *filename, unsigned char **file);


int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "No file argument supplied.\n");
        return 1;
    }

    char *filename = argv[1];

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

    // Read the file to encrypt
    unsigned char *file;
    size_t fileLength = readFile(filename, &file);
    printf("%d bytes to be encrypted\n", (int)fileLength);

    // Encrypt the file
    unsigned char *encryptedFile;
    int encryptedFileLength;
    if((encryptedFileLength = crypto.aesEncrypt((const unsigned char*)file, fileLength, &encryptedFile)) == -1) {
        fprintf(stderr, "Encryption failed\n");
        return 1;
    }
    printf("%d bytes encrypted\n", encryptedFileLength);

    // Append .enc to the filename
    char *encryptedFilename = (char*)malloc(strlen(filename) + 5);
    if(encryptedFilename == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }
    sprintf(encryptedFilename, "%s.enc", filename);

    #ifdef CONVERT_TO_BASE64
        // Encode the encrypted file to base64
        char *base64Buffer = base64Encode(encryptedFile, encryptedFileLength);

        // Change the encrypted file pointer to the base64 string and update
        // the length (we can use strlen() now since the base64 string is ASCII data)
        free(encryptedFile);
        encryptedFile = (unsigned char*)base64Buffer;
        encryptedFileLength = strlen((char*)encryptedFile);
    #endif
    
    // Write the encrypted file to its own file
    writeFile(encryptedFilename, encryptedFile, encryptedFileLength);
    printf("Encrypted message written to \"%s\"\n", encryptedFilename);

	//FILE *fd = fopen("key.key", "wb");
	//crypto.writeKeyToFile(fd, KEY_AES);
	//fclose(fd);

    //fd = fopen("keyiv.key", "wb");
	//crypto.writeKeyToFile(fd, KEY_AES_IV);
	//fclose(fd);

	 // Write the AES key to stdout in hex
        unsigned char *aesKey;
        size_t aesLength = crypto.getAESKey(&aesKey);
        printf("AES key: ");
        for(unsigned int i=0; i<aesLength; i++) {
            printf("%d ", aesKey[i]);
        }
        printf("\n");
        printf("Size: %d \n", aesLength);
         // Write the AES key to stdout in hex
        unsigned char *aes_iv_Key;
        aesLength = crypto.getAESIv(&aes_iv_Key);
        printf("AES IV: ");
        for(unsigned int i=0; i<aesLength; i++) {
            printf("%d ", aes_iv_Key[i]);
        }
        printf("\n");
		printf("Size: %d \n", aesLength);
    free(file);

    // Read the encrypted file back
    /*fileLength = readFile(encryptedFilename, &file);

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
    int decryptedFileLength;
    if((decryptedFileLength = crypto.aesDecrypt(file, fileLength, &decryptedFile)) == -1) {
        fprintf(stderr, "Decryption failed\n");
        return 1;
    }
    printf("%d bytes decrypted\n", (int)decryptedFileLength);

    // Append .dec to the filename
    char *decryptedFilename = (char*)malloc(strlen(filename) + 5);
    if(decryptedFilename == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }
    sprintf(decryptedFilename, "%s.dec", filename);
    
    // Write the decrypted file to its own file
    writeFile(decryptedFilename, decryptedFile, decryptedFileLength);
    printf("Decrypted file written to \"%s\"\n", decryptedFilename);

    free(decryptedFile);
    free(file);*/

    return 0;
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


int readFile(char *filename, unsigned char **file) {
    FILE *fd = fopen(filename, "rb");
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
