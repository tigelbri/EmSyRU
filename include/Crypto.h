//## Copyright (c) 2015, Tristan Igelbrink
//## All rights reserved.
//## 
//## Redistribution and use in source and binary forms, with or without
//## modification, are permitted provided that the following conditions 
//## are met:
//##
//## 1. Redistributions of source code must retain the above copyright
//## notice, this list of conditions and the following disclaimer.
//##
//## 2. Redistributions in binary form must reproduce the above copyright
//## notice, this list of conditions and the following disclaimer in the
//## documentation and/or other materials provided with the distribution.
//## 
//## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//## COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT
//## , INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//## SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//## STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
//## OF THE POSSIBILITY OF SUCH DAMAGE.

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include <stdio.h>
#include <string>
#include <string.h>

#ifndef CRYPTO_H
#define CRYPTO_H

#define RSA_KEYLEN 2048
#define AES_KEYLEN 256
#define AES_ROUNDS 6

#define PSUEDO_CLIENT

//#define USE_PBKDF

#define SUCCESS 0
#define FAILURE -1

#define KEY_SERVER_PRI 0
#define KEY_SERVER_PUB 1
#define KEY_CLIENT_PUB 2
#define KEY_AES        3
#define KEY_AES_IV     4

class Crypto {

public:
    Crypto();

    Crypto(unsigned char *remotePubKey, size_t remotePubKeyLen);

    ~Crypto();

    int rsaEncrypt(const unsigned char *msg, size_t msgLen, unsigned char **encMsg, unsigned char **ek, size_t *ekl, unsigned char **iv, size_t *ivl);

    int aesEncrypt(const unsigned char *msg, size_t msgLen, unsigned char **encMsg);

    int rsaDecrypt(unsigned char *encMsg, size_t encMsgLen, unsigned char *ek, size_t ekl, unsigned char *iv, size_t ivl, unsigned char **decMsg);

    int aesDecrypt(unsigned char *encMsg, size_t encMsgLen, unsigned char **decMsg);

    int writeKeyToFile(FILE *fd, int key);

    int getRemotePubKey(unsigned char **pubKey);

    int setRemotePubKey(unsigned char *pubKey, size_t pubKeyLen);

    int getLocalPubKey(unsigned char **pubKey);

    int getLocalPriKey(unsigned char **priKey);

    int getAESKey(unsigned char **aesKey);

    int setAESKey(unsigned char *aesKey, size_t aesKeyLen);

    int getAESIv(unsigned char **aesIv);

    int setAESIv(unsigned char *aesIv, size_t aesIvLen);

private:
    static EVP_PKEY *localKeypair;
    EVP_PKEY *remotePubKey;

    EVP_CIPHER_CTX *rsaEncryptCtx;
    EVP_CIPHER_CTX *aesEncryptCtx;

    EVP_CIPHER_CTX *rsaDecryptCtx;
    EVP_CIPHER_CTX *aesDecryptCtx;

    unsigned char *aesKey;
    unsigned char *aesIV;

    int init();
    int genTestClientKey();
};

#endif
