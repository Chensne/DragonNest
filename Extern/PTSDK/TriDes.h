#pragma once

#include <memory>

typedef const unsigned char * CPBYTE;
typedef unsigned char * PBYTE;
typedef const char * CPCHAR;
typedef char * PCHAR;
typedef unsigned char UCHAR;


class TriDes
{
public:
	class CharArray
	{
	public:
		CharArray(size_t size) 
		{
			size == 0 ? m_pArray = NULL:m_pArray = new UCHAR[size];
			memset(m_pArray,0,size);
		}
		~CharArray(void){ if (!m_pArray) {delete[] m_pArray;} };	
		PBYTE m_pArray;
	};


	TriDes(void);
	~TriDes(void);

	static int Encrypt(CPBYTE pbPlain, int nPlainLen, CPBYTE pbKey, int nKeyLen, PBYTE pbCryptograph, int &nCryptographLen);
	static int EncryptBase64(CPCHAR pbPlain, CPCHAR pbKey, PCHAR pbCryptographBase64);
	static int EncryptBase64(CPBYTE pbPlain, int nPlainLen, CPCHAR strKey, PCHAR strCryptographBase64);
	static int EncryptBase64(CPCHAR pbPlain, CPBYTE strKey, int nKeyLen, PCHAR strCryptographBase64);
	static int EncryptBase64(CPBYTE pbPlain, int nPlainLen, CPBYTE strKey, int nKeyLen, PCHAR strCryptographBase64);

	static int Decrypt(CPBYTE pbCryptograph, int nCryptographLen, CPBYTE pbKey, int nKeyLen, PBYTE pbPlain, int &nPlainLen);
	static int DecryptBase64(CPCHAR strCryptographBase64, CPCHAR strKey, PCHAR pbPlainBase64);
	static int DecryptBase64(CPCHAR strCryptographBase64, CPCHAR strKey, PBYTE pbPlain, int &nPlainLen);
	static int DecryptBase64(CPCHAR strCryptographBase64, CPBYTE pbKey, int nKeyLen, PBYTE pbPlain, int &nPlainLen);
private:
	static int CountBlockSize(int size);
};
