#include "crypto.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/sha.h>

#include <string.h>
#include <cstring>
#include <stdlib.h>

#include <iostream>

using namespace std;

/**
 * The two functions bellow are taken from the openssl official website :
 * https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
 * You can read more there
 */

namespace TON::crypto::aes
{

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        throw false;

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
    {
        EVP_CIPHER_CTX_free(ctx);
        throw false;
    }

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    {
        EVP_CIPHER_CTX_free(ctx);
        throw false;
    }
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    {
        EVP_CIPHER_CTX_free(ctx);
        throw false;
    }
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext)
{

    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        throw false;

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
    {
        EVP_CIPHER_CTX_free(ctx);
        throw false;
    }

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    {
        EVP_CIPHER_CTX_free(ctx);
        throw false;
    }
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        plaintext_len = 0;
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

void hashPassword(const char *string, char outputBuffer[65])
{

    #if OPENSSL_VERSION_NUMBER < 0x10100000L
    #  define EVP_MD_CTX_new   EVP_MD_CTX_create
    #  define EVP_MD_CTX_free  EVP_MD_CTX_destroy
    #endif

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    unsigned int hashSize = 65;
    unsigned char * tempOutBuff =  (unsigned char *) malloc(hashSize * sizeof(unsigned char));

    EVP_DigestInit(ctx, EVP_sha256());
    EVP_DigestUpdate(ctx, string, strlen(string));
    EVP_DigestFinal(ctx, tempOutBuff, &hashSize);

    for(unsigned int i = 0 ; i < hashSize ; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", tempOutBuff[i]);
    }

    outputBuffer[64] = 0;
    free(tempOutBuff);
}

string myEncrypt(string plainText, string pass)
{
    // Allocate memory for hash bytes (chars) array with 65 elements
    char *hash = (char *) malloc(sizeof(char) * 65);

    // Allocate memory for cipher bytes (chars) array with 1024 elements
    unsigned char *cipher = (unsigned char *) malloc(sizeof(unsigned char) * 2048);

    // Here we hash the plain text password
    hashPassword((const char *)pass.c_str(), hash);

    // The hash contains the encrytion key and the initialization vector -> (iv)
    // so we divide it between the key and the iv using std::string method substr()
    string hashStr(hash);
    const auto keyStr = hashStr.substr(0, 16);
    const auto iv = hashStr.substr(16, 16);

    string res;
    try {
        // here we initiate the encryption process
        int cipherSize = encrypt(
            (unsigned char *) plainText.c_str(),
            plainText.size(),
            (unsigned char *) keyStr.c_str(),
            (unsigned char *) iv.c_str(),
            cipher
        );

        res = string((char *) cipher, cipherSize);
    } catch(...) {
    }

    free(cipher);
    free(hash);
    return res;
}

string myDecrypt(string cipher, string pass)
{
    // Converting string to unsigned character (bytes) array
    unsigned char *plainTextBytes = (unsigned char *) malloc(sizeof(unsigned char) * 2048);

    // Allocate memory for hash bytes (chars) array with 65 elements
    char *hash = (char *) malloc(sizeof(char) * 65);

    // Here we hash the plain text password
    hashPassword((const char *)pass.c_str(), hash);

    // The hash contains the encrytion key and the initialization vector -> (iv)
    // so we divide it between the key and the iv using std::string method substr()
    string hashStr(hash);
    const auto keyStr = hashStr.substr(0, 16);
    const auto iv = hashStr.substr(16, 16);

    string plainText;
    try {
        auto len = decrypt(
            (unsigned char *) cipher.c_str(),
            cipher.size(),
            (unsigned char *) keyStr.c_str(),
            (unsigned char *) iv.c_str(),
            plainTextBytes
        );

        plainText = string((char *) plainTextBytes, len);
    } catch(...) {
    }

    free(hash);
    free(plainTextBytes);
    return plainText;
}

}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


#pragma GCC diagnostic pop

TON::Tools::CryptoAES::CryptoAES(const QString &key)
    : mKey(key)
{

}

QByteArray TON::Tools::CryptoAES::encrypt(const QByteArray &data) const
{
    return mKey.isEmpty()? data : QByteArray::fromStdString( TON::crypto::aes::myEncrypt(data.toStdString(), mKey.toStdString()) );
}

QByteArray TON::Tools::CryptoAES::decrypt(const QByteArray &data) const
{
    return mKey.isEmpty()? data : QByteArray::fromStdString( TON::crypto::aes::myDecrypt(data.toStdString(), mKey.toStdString()) );
}
