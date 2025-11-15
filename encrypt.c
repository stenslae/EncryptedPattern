#include <stdio.h>
#include "encrypt.h"

/**
 * encrypt() - Uses openSSL library to decript cyphertext
 * @*ciphertext: Number of user args
 * @ciphertext_len: Length of ciphertext
 * @*key: AES-128 key
 * @**iv: AES-128 initialization vector
 * @*plaintext: Points to *filename in main
 *
 * Initializes a EVP Cipher process to encrypt the ciphertext
 * with the Key and IV using AES-128-CBC. Encrypts by going
 * block by block.
 *
 * Return: 0
 */
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, 
    unsigned char *iv, unsigned char *ciphertext){
    
    EVP_CIPHER_CTX *ctx;
    int len = 0;
    int ciphertext_len = 0;

    // Create and initialise the context
    if(!(ctx = EVP_CIPHER_CTX_new())){
        print_errs(ERR_BAD_ENCRYPT);
    }

    // Initialise the encryption operation.
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)){
        print_errs(ERR_BAD_ENCRYPT);
    }
    EVP_CIPHER_CTX_set_padding(ctx, 1);

    // Loop in chunks to encrypt full text
    int chunk_size = 1024;
    int offset = 0;
    while (offset < plaintext_len) {
        int to_encrypt = (plaintext_len - offset > chunk_size) ? chunk_size : (plaintext_len - offset);
        if (1 != EVP_EncryptUpdate(ctx, ciphertext + ciphertext_len, &len, plaintext + offset, to_encrypt)){
            print_errs(ERR_BAD_ENCRYPT);
        }
        ciphertext_len += len;
        offset += to_encrypt;
    }

    // Add the final padding
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)){
        print_errs(ERR_BAD_ENCRYPT);
    }
    ciphertext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

/**
 * decrypt() - Uses openSSL library to decript cyphertext
 * @*ciphertext: Number of user args
 * @ciphertext_len: Length of ciphertext
 * @*key: AES-128 key
 * @**iv: AES-128 initialization vector
 * @*plaintext: Points to *filename in main
 *
 * Initializes a EVP Cipher process to decrypt the ciphertext
 * with the Key and IV using AES-128-CBC. Decrypts by going
 * block by block.
 *
 * Return: 0
 */
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, 
    unsigned char *iv, unsigned char *plaintext){
    
    EVP_CIPHER_CTX *ctx;

    int len = 0;
    int plaintext_len = 0;

    // Create and initialise the context
    if(!(ctx = EVP_CIPHER_CTX_new())){
        print_errs(ERR_BAD_ENCRYPT);
    }

    // Initialise the decryption operation.
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)){
        print_errs(ERR_BAD_ENCRYPT);
    }
    
    // Loop in blocks to encrypt full text
    int chunk_size = 1024;
    int offset = 0;
    while (offset < ciphertext_len) {
        int to_decrypt = (ciphertext_len - offset > chunk_size) ? chunk_size : (ciphertext_len - offset);
        if (1 != EVP_DecryptUpdate(ctx, plaintext + plaintext_len, &len, ciphertext + offset, to_decrypt)){
            print_errs(ERR_BAD_ENCRYPT);
        }
        plaintext_len += len;
        offset += to_decrypt;
    }

    // Add the final padding
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)){
        print_errs(ERR_BAD_ENCRYPT);
    }
    plaintext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}
