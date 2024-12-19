
#pragma once

#include "DNPacket.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"

using namespace MasterSystem;

inline void SendReqMasterList(const UINT uiPage, const BYTE cJob, const BYTE cGender )
{
	CSMasterList packet;
	memset( &packet, 0, sizeof(CSMasterList) );

	packet.uiPage = uiPage;
	packet.cJob = cJob;
	packet.cGender = cGender;

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_MASTERLIST, (char*)&packet, sizeof(CSMasterList) );
}

inline void SendReqMasterCharacterInfo()
{
	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_MASTERCHARACTERINFO, NULL, 0 );
}

inline void SendMasterIntroduction( const bool bRegister, const WCHAR * wszSelfIntroduction )
{
	CSIntroduction packet;
	memset( &packet, 0, sizeof(CSIntroduction) );

	packet.bRegister = bRegister;
	if( bRegister )
		_wcscpy(packet.wszSelfIntroduction, _countof(packet.wszSelfIntroduction), wszSelfIntroduction, (int)wcslen(wszSelfIntroduction));

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_INTRODUCTION_ONOFF, (char*)&packet, bRegister ? sizeof(packet) : sizeof(packet.bRegister) );
}

inline void SendMasterApplication( INT64 biCharacterDBID )
{
	CSMasterApplication packet;
	memset( &packet, 0, sizeof(CSMasterApplication) );

	packet.biCharacterDBID = biCharacterDBID;

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_MASTER_APPLICATION, (char*)&packet, sizeof(CSMasterApplication) );
}

inline void SendMasterJoin( INT64 biMasterCharacterDBID, const WCHAR * wszMasterCharName )
{
	CSJoin packet;
	memset( &packet, 0, sizeof(CSJoin) );

	packet.biMasterCharacterDBID = biMasterCharacterDBID;
	if (wszMasterCharName)
		_wcscpy( packet.wszMasterCharName, _countof(packet.wszMasterCharName), wszMasterCharName, (int)wcslen(wszMasterCharName) );

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_JOIN, (char*)&packet, sizeof(CSJoin) );
}

inline void SendReqMasterAndClassmate()
{
	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_MASTERANDCLASSMATE, NULL, 0 );
}

inline void SendReqMyMasterInfo( INT64 biMasterCharacterDBID )
{
	CSMyMasterInfo packet;
	memset( &packet, 0, sizeof(CSMyMasterInfo) );

	packet.biMasterCharacterDBID = biMasterCharacterDBID;

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_MYMASTERINFO, (char*)&packet, sizeof(CSMyMasterInfo) );
}

inline void SendReqClassmateInfo( INT64 biClassmateCharacterDBID )
{
	CSClassmateInfo packet;
	memset( &packet, 0, sizeof(CSClassmateInfo) );

	packet.biClassmateCharacterDBID = biClassmateCharacterDBID;

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_CLASSMATEINFO, (char*)&packet, sizeof(CSClassmateInfo) );
}

inline void SendReqLeave( INT64 biDestCharacterDBID, bool bIsMaster )
{
	CSLeave packet;
	memset( &packet, 0, sizeof(CSLeave) );

	packet.biDestCharacterDBID = biDestCharacterDBID;
	packet.bIsMaster = bIsMaster;

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_LEAVE, (char*)&packet, sizeof(CSLeave) );
}

inline void SendReqInvitePupil( const WCHAR * wszCharName )
{
	CSInvitePupil packet;
	memset( &packet, 0, sizeof(CSInvitePupil) );

	_wcscpy(packet.wszCharName, _countof(packet.wszCharName), wszCharName, (int)wcslen(wszCharName));

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_INVITE_PUPIL, (char*)&packet, sizeof(CSInvitePupil) );
}

inline void SendReqInvitePupilConfirm( const bool bIsAccept, const WCHAR * wszMasterCharName )
{
	CSInvitePupilConfirm packet;
	memset( &packet, 0, sizeof(CSInvitePupilConfirm) );

	packet.bIsAccept = bIsAccept;

	_wcscpy(packet.wszMasterCharName, _countof(packet.wszMasterCharName), wszMasterCharName, (int)wcslen(wszMasterCharName));

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_INVITE_PUPIL_CONFIRM, (char*)&packet, sizeof(CSInvitePupilConfirm) );
}

inline void SendReqJoinDirect( const WCHAR * wszMasterCharName )
{
	CSJoinDirect packet;
	memset( &packet, 0, sizeof(CSJoinDirect) );

	_wcscpy(packet.wszMasterCharName, _countof(packet.wszMasterCharName), wszMasterCharName, (int)wcslen(wszMasterCharName));

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_JOIN_DIRECT, (char*)&packet, sizeof(CSJoinDirect) );
}
inline void SendReqJoinDirectConfirm( const bool bIsAccept, const WCHAR * wszPupilCharName )
{
	CSJoinDirectConfirm packet;
	memset( &packet, 0, sizeof(CSJoinDirectConfirm) );

	packet.bIsAccept = bIsAccept;

	_wcscpy(packet.wszPupilCharName, _countof(packet.wszPupilCharName), wszPupilCharName, (int)wcslen(wszPupilCharName));

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_JOIN_DIRECT_CONFIRM, (char*)&packet, sizeof(CSJoinDirectConfirm) );
}

inline void SendReqRecallMaster( const WCHAR * wszMasterCharName )
{
	CSRecallMaster packet;
	memset( &packet, 0, sizeof(CSRecallMaster) );

	_wcscpy(packet.wszMasterCharName, _countof(packet.wszMasterCharName), wszMasterCharName, (int)wcslen(wszMasterCharName));

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_RECALL_MASTER, (char*)&packet, sizeof(CSRecallMaster) );
}

inline void SendReqBreakInto( const int iRet, const WCHAR * wszPupilCharName )
{
	CSBreakInto packet;
	memset( &packet, 0, sizeof(CSBreakInto) );

	packet.iRet = iRet;
	_wcscpy(packet.wszPupilCharName, _countof(packet.wszPupilCharName), wszPupilCharName, (int)wcslen(wszPupilCharName));

	CClientSessionManager::GetInstance().SendPacket(CS_MASTERSYSTEM, eMasterSystem::CS_BREAKINTO_PUPIL, (char*)&packet, sizeof(packet) );
}

inline void SendReqJoinConfirm( const bool bIsAccept, const WCHAR * wszPupilCharName )
{
	CSJoinComfirm packet;
	memset( &packet, 0, sizeof(CSJoinComfirm) );

	packet.bIsAccept = bIsAccept;

	_wcscpy( packet.wszPupilCharName, _countof(packet.wszPupilCharName), wszPupilCharName, (int)wcslen(wszPupilCharName) );

	CClientSessionManager::GetInstance().SendPacket( CS_MASTERSYSTEM, eMasterSystem::CS_JOIN_CONFIRM, (char*)&packet, sizeof(CSJoinComfirm) );
}