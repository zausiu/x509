/*
 * Anaconda.h
 *
 *  Created on: Jun 21, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#ifndef ANACONDA_ANACONDA_H_
#define ANACONDA_ANACONDA_H_

extern "C"
{
// 必须先调
void Anaconda_Init();

// 传入明文和它的长度以及密钥(用Anaconda_Passphrase得到)
// 返回二进制密文和密文长度.ret小于0时表示失败，大于0时表示密文长度
int Anaconda_Encrypt(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, const unsigned char* key);
// 解密到plaintext指向的内存，返回码小于0时表示失败，大于0时表示明文长度
int Anaconda_Decrypt(const unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, const unsigned char* key);

// 传入明文和它的长度以及密钥
// 返回base64编码的密文和密文长度,ret小于0时表示失败，大于0时表示密文长度
int Anaconda_EncryptBase64(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, const unsigned char* key);
// 解密到plaintext指向的内存，返回码小于0时表示失败，大于0时表示明文长度
int Anaconda_DecryptBase64(const char *base64str, unsigned char *plaintext, const unsigned char* key);

// buff给256 bytes长度， 返回一个null结尾的密钥长度（单位字节）
int Anaconda_Passphrase(unsigned char* buff);
};

#endif /* ANACONDA_ANACONDA_H_ */
