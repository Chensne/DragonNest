
#pragma once
#include "DNPacket.h"
#include "MtRandom.h"

// 2009.01.19 김밥
// DNGAME_PACKET 해더 크기 정의
const int DNGAME_PACKET_HEADERSIZE = 7;

class CCryptoPacket
{
public:
	CCryptoPacket();
	virtual ~CCryptoPacket() {}

	int EncodeGamePacket(void *p, int main_header, int sub_header, const void *message, int size, BYTE cSeq = 0);
	bool DecodeGamePacket(DNGAME_PACKET *p);
	int CalcGamePacketSize(void *p, int len); // 아직 읽은 내용이 적으면 -1 (알 수 없다)
};

class CCryptoKey
{
public:
	CCryptoKey();
	~CCryptoKey() {}

	//for server
	bool ServerMakeKey(char * pKey, bool bCheck);
	int ServerVerifyKey(char * pKey);

	//for client
	bool ClientVerifyKey(char * pKey);

private:
	CMtRandom m_Random;
	bool m_bMakeKey;

	bool MakeCryptoKey(char * pBuf);
	int VerifyCryptoKey(char * pBuf, bool bFullLenth);
};

