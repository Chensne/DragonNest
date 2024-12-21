

#include "StdAfx.h"
#include "NetSession.h"

#include <vector>
#include <map>

#define DNPRJSAFE	// DNPrjSafe.h ����
#include "CriticalSection.h"
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"


CNetSession::CNetSession()
{
	TP_SUPER::SetParam(static_cast<LPVOID>(this));
	Reset();

	m_pServiceInfo = new CServiceInfo();
	m_pServiceInfo->Open ();

	m_bConnect = false;
}

CNetSession::~CNetSession()
{
	SAFE_DELETE(m_pServiceInfo);
}

VOID CNetSession::Reset()
{
	m_MonitorLevel = 0;		// SERVERMONITOR_LEVEL_NONE
	TP_SUPER::Reset();
}

DWORD CNetSession::Connect(UINT pIpAddress, WORD pPortNumber)
{
	if (IsConnect())
		return(HASERROR+1);

	BOOL aRetVal = 	TP_SUPER::Connect(pIpAddress, pPortNumber);
	if (!aRetVal) {
		return(HASERROR+1);
	}

	m_pIpAddress = pIpAddress;
	m_pPortNumber = pPortNumber;

	return NOERROR;
}

VOID CNetSession::Disconnect()
{
	m_MonitorLevel = 0;
	TP_SUPER::Disconnect();
}

VOID CNetSession::Reconnect()
{
	if (!m_bConnect) return;

	Disconnect();
	Connect(m_pIpAddress, m_pPortNumber);
}

BOOL CNetSession::IsConnect()
{
	return(INVALID_SOCKET != TP_SUPER::GetSocket());
}

DWORD CNetSession::SendData(SHORT pMainCmd, SHORT pSubCmd, PCHAR pBuffer, INT pLen)
{
	DNTPacketHeader aPacket;
	aPacket.iLen = static_cast<unsigned short>(sizeof(aPacket) + pLen);
	aPacket.cMainCmd = static_cast<unsigned char>(pMainCmd);
	aPacket.cSubCmd = static_cast<unsigned char>(pSubCmd);

	BOOL aRetVal = TP_SUPER::SendData(reinterpret_cast<LPBYTE>(&aPacket), sizeof(aPacket), TRUE);
	if (!aRetVal) {
		return(HASERROR+1);
	}

	if (pBuffer && 0 < pLen) {
		aRetVal = TP_SUPER::SendData(reinterpret_cast<LPBYTE>(pBuffer), pLen, TRUE);
		if (!aRetVal) {
			return(HASERROR+2);
		}
	}

	TRACE(CVarArg<MAX_PATH>(_T("Send Packet : %d / %d\n"), pMainCmd, pSubCmd));

	return NOERROR;
}

