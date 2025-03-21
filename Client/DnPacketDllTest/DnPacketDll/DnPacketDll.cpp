
#define DLLEXPORT

#include <time.h>
#include <iostream>
#include <windows.h>
#include "../../../Common/Protocol/DNPacket.h"
#include "DnPacketDll.h"

namespace DNTest
{
	//#############################################################################################
	// Random 클래스
	//#############################################################################################

	class CRandom
	{
		unsigned long m_state[624];
		int m_left;
		int m_initf;
		unsigned long *m_next;
		unsigned long m_seed;

		void next_state(void);
		float genrand_real2(void);

	public:
		CRandom();

		void srand(unsigned long seed);
		int rand(void);
		int rand(unsigned long range);
		float rand(float smallVal, float largeVal);
		int rand(int smallVal, int largeVal);
		int GetSeed() { return m_seed; }

		enum
		{
			N = 624,
			M = 397,
			MATRIX_A = 0x9908b0dfUL,
			UMASK = 0x80000000UL,
			LMASK = 0x7fffffffUL,
		};
	};

	CRandom::CRandom()
	{
		m_left = 1;
		m_initf = 0;
		m_seed = 0;
	}

	void CRandom::srand(unsigned long seed)
	{
		m_seed = seed;
		int j;
		m_state[0]= seed & 0xffffffffUL;
		for (j=1; j<N; j++) {
			m_state[j] = (1812433253UL * (m_state[j-1] ^ (m_state[j-1] >> 30)) + j);
			m_state[j] &= 0xffffffffUL;
		}
		m_left = 1;
		m_initf = 1;
	}

	#define MT_MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
	#define MT_TWIST(u,v)	((MT_MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))

	void CRandom::next_state(void)
	{
		unsigned long *p=m_state;
		int j;

		if (m_initf==0) srand((unsigned long)time(0));
		m_left = N;
		m_next = m_state;

		for (j=N-M+1; --j; p++)
			*p = p[M] ^ MT_TWIST(p[0], p[1]);

		for (j=M; --j; p++)
			*p = p[M-N] ^ MT_TWIST(p[0], p[1]);

		*p = p[M-N] ^ MT_TWIST(p[0], m_state[0]);
	}

	float CRandom::genrand_real2(void)
	{
		return (float)rand() * (1.f/4294967296.f);
		/* divided by 2^32 */
	}

	int CRandom::rand(void)
	{
		int y;
		if (--m_left == 0) next_state();
		y = *m_next++;

		/* Tempering */
		y ^= (y >> 11);
		y ^= (y << 7) & 0x9d2c5680UL;
		y ^= (y << 15) & 0xefc60000UL;
		y ^= (y >> 18);
		if( y < 0 ) y = -y;
		return y;
	}

	int CRandom::rand(unsigned long range)
	{
		// Find which bits are used in n
		// Optimized by Magnus Jonsson (magnus@smartelectronix.com)
		unsigned int used = range;
		used |= used >> 1;
		used |= used >> 2;
		used |= used >> 4;
		used |= used >> 8;
		used |= used >> 16;

		// Draw numbers until one is found in [0, range-1]
		int n;
		do {
			n = (unsigned int)rand() & used;  // toss unused bits to shorten search
		}
		while( n >= (int)range );
		return n;

		/*return rand() % range;*/
	}

	float CRandom::rand(float smallVal, float largeVal)
	{
		float interval = genrand_real2();
		return smallVal * interval + largeVal * (1-interval);	
	}

	int CRandom::rand(int smallVal, int largeVal)
	{
		return rand(largeVal-smallVal+1) + smallVal;
	}

	//#############################################################################################
	// DNCrypt 클래스
	//#############################################################################################

	class DNCrypt
	{
	public:

		DNCrypt();
		void encrypt( char* pBuf, const UINT uiSize );
		void decrypt( char* pBuf, const UINT uiSize );

	private:

		enum
		{
			TEA_KEYCOUNT	= 256,
			TEA_DELTA		= 0x9E3779B9,
			TEA_BLOCKSIZE	= 8,
			TEA_KEYSEED		= 0x84873294,

		};

		UINT m_TeaKey[TEA_KEYCOUNT][4];
	};

	DNCrypt::DNCrypt()
	{
		CRandom rand;
		rand.srand( TEA_KEYSEED );

		for( UINT i=0 ; i<TEA_KEYCOUNT ; ++i )
		{
			for( UINT j=0 ;j<4 ; ++j )
			{
				m_TeaKey[i][j] = rand.rand();
			}
		}
	}

	void DNCrypt::encrypt( char* pBuf, const UINT uiSize )
	{
		UINT uiEncodeSize = uiSize;

		// BLOCK크기 계산
		int iBlockCount = uiSize/TEA_BLOCKSIZE;
		if( uiSize%TEA_BLOCKSIZE )
			iBlockCount++;

		UINT y,z,uiSum,uiLimit,uiRound;
		// round 얻기
		uiRound = (uiSize%2)+1;

		for( int i=0 ; i<iBlockCount ; i++ )
		{
			UINT* pKey = m_TeaKey[uiSize%TEA_KEYCOUNT];

			uiSum		= 0;					//초기화 
			uiLimit     = uiRound * TEA_DELTA;	

			// BLOCK 단위 encrypt
			if( uiEncodeSize >= TEA_BLOCKSIZE )
			{
				memcpy( &y, pBuf+(i*TEA_BLOCKSIZE),		sizeof(UINT) );
				memcpy( &z, pBuf+((i*TEA_BLOCKSIZE)+4), sizeof(UINT) );

				// start cycle
				while( uiSum != uiLimit )
				{
					y += (z << 4 ^ z >> 5) + z ^ uiSum + pKey[uiSum&3];
					uiSum += TEA_DELTA;
					z += (y << 4 ^ y >> 5) + y ^ uiSum + pKey[uiSum>>11 & 3];
				}
				// end cycle

				memcpy( pBuf+(i*TEA_BLOCKSIZE),		&y, sizeof(UINT) );
				memcpy( pBuf+(i*TEA_BLOCKSIZE)+4,	&z, sizeof(UINT) );

				uiEncodeSize -= TEA_BLOCKSIZE;
			}
			// BYTE 단위 encrypt
			else
			{
				char* pTempBuf = pBuf+(i*TEA_BLOCKSIZE);

				for( UINT j=0 ; j<uiEncodeSize ; ++j )
				{
					pTempBuf[j] ^= (reinterpret_cast<char*>(pKey))[j];
				}
			}
		}
	}

	void DNCrypt::decrypt( char* pBuf, const UINT uiSize )
	{
		UINT uiDecryptSize = uiSize;

		// BLOCK크기 계산
		int iBlockCount = uiSize/TEA_BLOCKSIZE;
		if( uiSize%TEA_BLOCKSIZE )
			iBlockCount++;

		UINT y,z,uiSum,uiRound;
		// round 얻기
		uiRound = (uiSize%2)+1;

		for( int i=0 ; i<iBlockCount ; i++ )
		{
			UINT* pKey = m_TeaKey[uiSize%TEA_KEYCOUNT];

			uiSum = uiRound*TEA_DELTA;		// 초기화 

			// BLOCK 단위 decrypt
			if( uiDecryptSize >= TEA_BLOCKSIZE )
			{
				memcpy( &y, pBuf+(i*TEA_BLOCKSIZE),		sizeof(UINT) );
				memcpy( &z, pBuf+((i*TEA_BLOCKSIZE)+4), sizeof(UINT) );

				// start cycle
				while( uiSum )
				{
					z -= (y << 4 ^ y >> 5) + y ^ uiSum + pKey[uiSum>>11 & 3];
					uiSum -= TEA_DELTA;
					y -= (z << 4 ^ z >> 5) + z ^ uiSum + pKey[uiSum&3];
				}
				// end cycle

				memcpy( pBuf+(i*TEA_BLOCKSIZE),		&y, sizeof(UINT) );
				memcpy( pBuf+(i*TEA_BLOCKSIZE)+4,	&z, sizeof(UINT) );

				uiDecryptSize -= TEA_BLOCKSIZE;
			}
			// BYTE 단위 decrypt
			else
			{
				char* pTempBuf = pBuf+(i*TEA_BLOCKSIZE);

				for( UINT j=0 ; j<uiDecryptSize ; ++j )
				{
					pTempBuf[j] ^= (reinterpret_cast<char*>(pKey))[j];
				}
			}
		}
	}

	//#############################################################################################
	// DNPacketDll 클래스
	//#############################################################################################

	DNPacketDll::DNPacketDll()
	:m_pCrypt(new DNCrypt())
	{
		m_strIP = L"127.0.0.1";
	}

	DNPacketDll::~DNPacketDll()
	{
		delete m_pCrypt;
	}

	void DNPacketDll::OnDisconnect()
	{
		std::cout << "Disconnect Server" << std::endl;
	}

	void DNPacketDll::OnConnect()
	{
		std::cout << "Connect Server Success" << std::endl;

		CSCheckVersion Check;
		memset(&Check, 0, sizeof(CSCheckVersion));

		Check.cNation	= 1;
		Check.cVersion	= 6;
		Check.bCheck	= true;
		
		_DoEncrypt( CS_LOGIN, eLogin::CS_CHECKVERSION, (char*)&Check, sizeof(Check) );
	}

	UINT DNPacketDll::OnReceive( const char* pBuf, const UINT uiSize )
	{
		char buffer[32768];
		UINT uiRecvSize		= uiSize;
		UINT uiParseSize	= 0;

		while( uiRecvSize > sizeof(USHORT) )
		{
			USHORT unPacketSize;
			memcpy( &unPacketSize, pBuf, sizeof(USHORT) );	// 패킷 길이 얻기
			if( unPacketSize > uiRecvSize )					// 패킷 길이 검사
				break;

			memset( buffer, 0, sizeof(buffer) );
			memcpy( buffer, pBuf, unPacketSize );

			pBuf		+= unPacketSize;
			uiParseSize += unPacketSize;
			uiRecvSize  -= unPacketSize;

			DNEncryptPacketSeq* pEnPacket = reinterpret_cast<DNEncryptPacketSeq*>(buffer);

			int iDecryptLen = pEnPacket->nLen-sizeof(BYTE)-sizeof(USHORT);
			m_pCrypt->decrypt( reinterpret_cast<char*>(&pEnPacket->Packet), iDecryptLen );
			_DoParse( pEnPacket );
		}

		return uiParseSize;
	}

	void DNPacketDll::_DoParse( void* _pEnPacket )
	{
		DNEncryptPacketSeq* pEnPacket = reinterpret_cast<DNEncryptPacketSeq*>(_pEnPacket);

		switch( pEnPacket->Packet.cMainCmd )
		{
			case SC_SYSTEM:
			{
				switch( pEnPacket->Packet.cSubCmd )
				{
					case eSystem::SC_VILLAGEINFO:
					{
						std::cout << "Finish Login Process" << std::endl;
						OnFinishLoginProcess();
						break;
					}
				}
				break;
			}
			case SC_LOGIN:
			{
				switch( pEnPacket->Packet.cSubCmd )
				{
					case eLogin::SC_CHECKVERSION:
					{
						SCCheckVersion* pPacket = reinterpret_cast<SCCheckVersion*>(pEnPacket->Packet.buf);
						std::string strRet = (pPacket->nRet == ERROR_NONE) ? "Success" : "Fail";
						std::cout << "CheckVersion " << strRet.c_str() << std::endl;

						if( pPacket->nRet == ERROR_NONE )
						{
							CSCheckLogin Login;
							memset(&Login, 0, sizeof(CSCheckLogin));

							wcsncpy_s( Login.wszUserId, _countof(Login.wszUserId), m_strAccount.c_str(), sizeof(WCHAR)*(IDLENMAX-1) );
							wcsncpy_s( Login.wszPassword, _countof(Login.wszPassword), m_strPW.c_str(), sizeof(WCHAR)*(PASSWORDLENMAX-1) );
							wcsncpy_s( Login.wszVirtualIp, _countof(Login.wszVirtualIp), m_strIP.c_str(), sizeof(WCHAR)*(IPLENMAX-1) );

							_DoEncrypt( CS_LOGIN, eLogin::CS_CHECKLOGIN, (char*)&Login, sizeof(Login) );
						}

						break;
					}
					case eLogin::SC_CHECKLOGIN:
					{
						SCCheckLogin* pPacket = reinterpret_cast<SCCheckLogin*>(pEnPacket->Packet.buf);
						std::string strRet = (pPacket->nRet == ERROR_NONE) ? "Success" : "Fail";
						std::cout << "CheckLogin " << strRet.c_str() << std::endl;

						if( pPacket->nRet == ERROR_NONE )
						{
							_DoEncrypt( CS_LOGIN, eLogin::CS_SERVERLIST, NULL, 0 );
						}

						break;
					}
					case eLogin::SC_SERVERLIST:
					{
						SCServerList* pPacket = reinterpret_cast<SCServerList*>(pEnPacket->Packet.buf);
						std::cout << "ServerCount:" << static_cast<int>(pPacket->cServerCount) << std::endl;

						if( pPacket->cServerCount > 0 )
						{
							CSSelectServer selectServer;
							memset(&selectServer, 0, sizeof(CSSelectServer));

							selectServer.cServerIndex = pPacket->ServerListData[0].cServerIndex;

							_DoEncrypt( CS_LOGIN, eLogin::CS_SELECTSERVER, (char*)&selectServer, sizeof(CSSelectServer) );
						}

						break;
					}
					case eLogin::SC_CHARLIST:
					{
						SCCharList* pPacket = reinterpret_cast<SCCharList*>(pEnPacket->Packet.buf);
						std::string strRet = (pPacket->nRet == ERROR_NONE) ? "Success" : "Fail";
						std::cout << "CharList " << strRet.c_str() << std::endl;

						if( pPacket->nRet == ERROR_NONE )
						{
							std::cout << "CharCount:" << static_cast<int>(pPacket->cCharCount) << std::endl;

							if( pPacket->cCharCount > 0 )
							{
								CSSelectChar Select;
								memset(&Select, 0, sizeof(CSSelectChar));

								Select.cCharIndex = pPacket->CharListData[0].cCharIndex;

								_DoEncrypt( CS_LOGIN, eLogin::CS_SELECTCHAR, (char*)&Select, sizeof(CSSelectChar));								
							}
						}

						break;
					}
					case eLogin::SC_CHANNELLIST:
					{
						SCChannelList* pPacket = reinterpret_cast<SCChannelList*>(pEnPacket->Packet.buf);
						std::cout << "ChannelCount:" << static_cast<int>(pPacket->cCount) << std::endl;
						
						if( pPacket->cCount > 0 )
						{
							CSSelectChannel selectChannel;
							memset(&selectChannel, 0, sizeof(CSSelectChannel));

							selectChannel.cChannelID = pPacket->Info[0].nChannelID;

							_DoEncrypt(CS_LOGIN, eLogin::CS_SELECTCHANNEL, (char*)&selectChannel, sizeof(CSSelectChannel));
						}

						break;
					}
				}
				break;
			}
		}
	}

	void DNPacketDll::_DoEncrypt( int iMainCmd, int iSubCmd, char* pData, int iLen )
	{
		DNEncryptPacketSeq EnPacket;
		memset(&EnPacket, 0, sizeof(EnPacket));

		EnPacket.Packet.iLen = static_cast<unsigned short>(sizeof(DNTPacketHeader)+iLen);
		EnPacket.Packet.cMainCmd = static_cast<unsigned char>(iMainCmd);
		EnPacket.Packet.cSubCmd = static_cast<unsigned char>(iSubCmd);
		memcpy(&EnPacket.Packet.buf, pData, iLen);

		EnPacket.nLen = EnPacket.Packet.iLen + sizeof(BYTE) + sizeof(USHORT);
		m_pCrypt->encrypt( reinterpret_cast<char*>(&EnPacket.Packet), EnPacket.Packet.iLen );

		OnSend( reinterpret_cast<char*>(&EnPacket), EnPacket.nLen );
	}

} // namespace DNTest
