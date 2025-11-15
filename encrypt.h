#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/evp.h>
#include "patterns.h"

#define ERR_BAD_OPT 0
#define ERR_FILE_NOT_FOUND 1
#define HELP_MSSG 2

int encrypt(unsigned char *plaintext, int plaintext_len, 
    unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, 
    unsigned char *key, unsigned char *iv, unsigned char *plaintext);

#endif