

#pragma once

#include <windows.h>
#include <winsock2.h>
#include "Define.h"


class CNetPacketParser
{

public:
	typedef DWORD (*LPFN_PACKET_NOTIFY_PROC) (
		LPVOID,			// 세션
		SHORT,		// 패킷 주코드
		SHORT,		// 패킷 보조코드
		LPBYTE,			// 버퍼 포인터
		SHORT	// 패킷 크기
		);

private:
	LPFN_PACKET_NOTIFY_PROC m_lpfnPacketNotifyProc;

public:
	CNetPacketParser();
	virtual ~CNetPacketParser();

	VOID SetPacketNotifyProc(LPFN_PACKET_NOTIFY_PROC lpfnPacketNotifyProc) {
		m_lpfnPacketNotifyProc = lpfnPacketNotifyProc;
	}

	static BOOL PacketParse(class CSessionBase* lpSession, CHAR* lpcBuffer, INT& iBufferPtr, UINT uIoBytes, UINT uTotalBufferSize, LPVOID lpRecvNotifyProcParam, IN_ADDR stIpAddress, WORD wPortNumber);

public:
	LPFN_PACKET_NOTIFY_PROC GetNotifyProc() const { return m_lpfnPacketNotifyProc; }

};

