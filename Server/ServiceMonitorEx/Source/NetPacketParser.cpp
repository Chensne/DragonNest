

#include "StdAfx.h"
#include "NetPacketParser.h"

#include <vector>
#include <map>

#define DNPRJSAFE	// DNPrjSafe.h 제외
#include "CriticalSection.h"
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"

#include "SysBaseBase.h"


CNetPacketParser::CNetPacketParser()
{
	m_lpfnPacketNotifyProc = NULL;
}


CNetPacketParser::~CNetPacketParser()
{
	
}


BOOL CNetPacketParser::PacketParse(class CSessionBase* lpSession, CHAR* lpcBuffer, INT& iBufferPtr, UINT uIoBytes, UINT uTotalBufferSize, LPVOID lpRecvNotifyProcParam, IN_ADDR /*stIpAddress*/, WORD /*wPortNumber*/)
{
	CNetPacketParser* lpThis = reinterpret_cast<CNetPacketParser*>(lpRecvNotifyProcParam);

	if (uIoBytes < sizeof(DNTPacketHeader))
		return TRUE;

	// 패킷의 기본 헤더를 받은 상태임
	UINT aParseProcessSize = 0;

	WHILE_INFINITE 
	{
		SHORT aMainCmd = reinterpret_cast<DNTPacketHeader*>(lpcBuffer + iBufferPtr)->cMainCmd;
		SHORT aSubCmd = reinterpret_cast<DNTPacketHeader*>(lpcBuffer + iBufferPtr)->cSubCmd;
		SHORT aPacketLen = reinterpret_cast<DNTPacketHeader*>(lpcBuffer + iBufferPtr)->iLen;

		if (static_cast<UINT>(aPacketLen) >  uTotalBufferSize) 
		{
			iBufferPtr = uIoBytes;
			return FALSE;
		}

		if (static_cast<UINT>(aPacketLen) <= uIoBytes - iBufferPtr) 
		{
			// 특정 패킷을 다 받음
			if (lpThis->GetNotifyProc()) 
			{
				if (NOERROR != lpThis->GetNotifyProc()(lpSession, aMainCmd, aSubCmd, 
					reinterpret_cast<LPBYTE>((aPacketLen > sizeof(DNTPacketHeader))?(lpcBuffer + iBufferPtr + sizeof(DNTPacketHeader)):(NULL)),
					aPacketLen))
				{
 					iBufferPtr = uIoBytes;
 					return FALSE;
				}
			}

			iBufferPtr += aPacketLen;
			aParseProcessSize += aPacketLen;
		}
		else 
		{
			break;
		}
		
		if (aParseProcessSize >= uIoBytes) 
		{	
			// 패킷을 다 처리했다.
			break;
		}
	}

	return TRUE;
}

