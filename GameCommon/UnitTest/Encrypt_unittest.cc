
#include "stdafx.h"
#include "DNSecure.h"
#include "CryptPacket.h"
#include "zlib.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

class Encrypt_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
	}
	virtual void TearDown()
	{
	}

	static void SetUpTestCase()
	{
	}

public:

};

void MakeRandomData( BYTE* pData, int iSize )
{
	srand( timeGetTime() );

	for( int i=0 ; i<iSize ; ++i )
	{
		*(pData+i) = rand()%256;
	}
}

void SecureEncryptTestFunc( int iSize )
{
	BYTE* pTest = new BYTE[iSize+16];
	ClientSA ClientSA;
	SecureLib_InitSA(&ClientSA);

	char szBuf[MAX_PATH];
	{
		MakeRandomData( pTest, iSize );
		
		DNEncryptPacketSeq EnPacket = { 0, };
		EnPacket.cSeq = 0;
		EnPacket.Packet.iLen = static_cast<unsigned short>(sizeof(DNTPacketHeader)+iSize);
		EnPacket.Packet.cMainCmd = static_cast<unsigned char>(0);
		EnPacket.Packet.cSubCmd = static_cast<unsigned char>(0);
		memcpy(&EnPacket.Packet.buf, pTest, iSize );

		// Encrypt
		sprintf( szBuf, "SecureEncryptTestFunc Size:%d", iSize );
		{
			CPerformanceLog log( szBuf );
			EnPacket.nLen = static_cast<USHORT>(SecureEncrypt((BYTE*)&EnPacket.Packet, EnPacket.Packet.iLen));
		}

		// Decrypt
		sprintf( szBuf, "SecureDecryptTestFunc Size:%d", iSize );
		{
			CPerformanceLog log( szBuf );
			(SecureDecrypt((BYTE*)&EnPacket.Packet, EnPacket.nLen, &ClientSA) );
		}
	}

	delete[] pTest;
}

TEST_F( Encrypt_unittest, TCPEncrypt )
{
	for( UINT i=1 ; i<10 ; ++i )
	{
		SecureEncryptTestFunc( 100*i );
	}
}

void UDPEncryptTestFunc( int iSize )
{
	BYTE* pTest = new BYTE[iSize+16];
	BYTE* pDest = new BYTE[iSize+16];

	CCryptoPacket cEncrypt;

	char szBuf[MAX_PATH];
	{
		MakeRandomData( pTest, iSize );
		DNGAME_PACKET packet;

		sprintf( szBuf, "UDPEncryptTestFunc Size:%d", iSize );
		{
			CPerformanceLog log( szBuf );
			cEncrypt.EncodeGamePacket( &packet, 1, 2, pTest, iSize );
		}
		sprintf( szBuf, "UDPDecryptTestFunc Size:%d", iSize );
		{
			CPerformanceLog log( szBuf );
			bool bRet = cEncrypt.DecodeGamePacket( &packet );
			if( !bRet )
				std::cout << "DecodeError" << std::endl;
		}
	}

	delete[] pTest;
	delete[] pDest;
}

TEST_F( Encrypt_unittest, UDPEncrypt )
{
	for( UINT i=1 ; i<10 ; ++i )
	{
		UDPEncryptTestFunc( 100*i );
	}
}

void TEAEncryptTestFunc( int iSize )
{
	BYTE* pTest = new BYTE[iSize+16];
	MakeRandomData( pTest, iSize );
	char szBuf[MAX_PATH];

	sprintf( szBuf, "TeaEncryptTestFunc Size:%d", iSize );
	{
		CPerformanceLog log( szBuf );
		CDNSecure::GetInstance().Tea_encrypt( reinterpret_cast<char*>(pTest), iSize );
	}

	delete[] pTest;
}

TEST_F( Encrypt_unittest, TEAEncrypt )
{
	CDNSecure::CreateInstance();

	for( UINT i=1 ; i<10 ; ++i )
	{
		TEAEncryptTestFunc( 100*i );
	}

	CDNSecure::DestroyInstance();
}

TEST_F( Encrypt_unittest, VerifyTea )
{
	CMtRandom  rand;
	rand.srand( timeGetTime() );

	CDNSecure::CreateInstance();

	UINT uiTestCount	= 10000000;
	UINT uiMinSize		= 1;
	UINT uiMaxSize		= 2048;

	for( UINT i=0 ; i<uiTestCount ; ++i )
	{
		UINT uiSize = (rand.rand()%uiMaxSize)+uiMinSize;

		char* pBuf = new char[uiSize];
		char* pOrg = new char[uiSize];

		MakeRandomData( reinterpret_cast<BYTE*>(pBuf), uiSize );
		memcpy( pOrg, pBuf, uiSize );

		CDNSecure::GetInstance().Tea_encrypt( pBuf, uiSize );
		CDNSecure::GetInstance().Tea_decrypt( pBuf, uiSize );

		EXPECT_TRUE( memcmp( pBuf, pOrg, uiSize ) == 0 ) << uiSize;

		delete[] pBuf;
		delete[] pOrg;
	}

	CDNSecure::DestroyInstance();
}
/*
void CompressTestFunc( int iSize )
{
	BYTE* pTest = new BYTE[iSize+16];
	BYTE* pDest = new BYTE[iSize+16];

	CCryptoPacket cEncrypt;

	char szBuf[MAX_PATH];
	sprintf( szBuf, "CompressTestFunc Size:%d", iSize );
	{
		MakeRandomData( pTest, iSize );
		DNGAME_PACKET packet;
		CPerformanceLog log( szBuf );
		UINT nCompPacketSize = sizeof(packet.data);
		compress2((Bytef*)&packet.data, (uLongf*)&nCompPacketSize, (Bytef*)pTest, (uLongf)iSize, Z_BEST_SPEED);
	}

	delete[] pTest;
	delete[] pDest;
}

TEST_F( Encrypt_unittest, Compress )
{
	for( UINT i=1 ; i<10 ; ++i )
	{
		CompressTestFunc( 100*i );
	}
}
*/

#endif // #if !defined( _FINAL_BUILD )
