#pragma once

#include "Timer.h"
#include "MultiRoom.h"

#include "DNGameRoom.h"

class CFrameSync;
class CDNGameRoom;

class CTask : virtual public CMultiElement
{
public:
	CTask(CDNGameRoom * pRoom);
	virtual ~CTask();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}
	virtual UINT __stdcall BeginThread( void *pParam ) { return 0; }

	void DestroyTask( bool bDelete ) { m_bDestroyTask = true; m_bDeleteThis = bDelete; }

	void SetLocalTime( LOCAL_TIME LocalTime ) { m_LocalTime = LocalTime; }
	LOCAL_TIME GetLocalTime() { return m_LocalTime; }
	void SetPrevLocalTime( LOCAL_TIME LocalTime ) { m_PrevLocalTime = LocalTime; }
	LOCAL_TIME GetPrevLocalTime() { return m_PrevLocalTime; }

	void SetDeltaTime( float fValue ) { m_fDelta = fValue; }
	float GetDeltaTime() { return m_fDelta; }

	void EnableTaskProcess( bool bEnable ) { m_bEnableTaskProcess = bEnable; }
	bool IsEnableTaskProcess() { return m_bEnableTaskProcess; }

	const char *GetTaskName() { return m_szTaskName.c_str(); }
	void SetTaskName( const char *szName ) { m_szTaskName = szName; }

	void SetLocalTimeSpeed( float fSpeed, DWORD dwDelay = 0 );

	void ResetTimer();

	__forceinline DWORD GetUserCountWithoutGM() { return GetRoom()->GetUserCountWithoutGM(); }
	__forceinline CDNGameRoom *GetRoom() { return (CDNGameRoom*)CMultiElement::GetRoom(); }
	__forceinline DWORD GetUserCount() { return GetRoom()->GetUserCount(); }
	__forceinline DWORD GetLiveUserCount() { return GetRoom()->GetLiveUserCount(); }
	__forceinline DWORD GetUserCountWithoutPartyOutUser() { return GetRoom()->GetUserCountWithoutPartyOutUser(); }
	__forceinline CDNUserSession *GetUserData( DWORD dwIndex ) { return GetRoom()->GetUserData(dwIndex); }
	__forceinline CDNGameRoom::PartyStruct *GetPartyData( DWORD dwIndex ) { return GetRoom()->GetPartyData(dwIndex); }
	__forceinline CDNGameRoom::PartyStruct *GetPartyData(CDNUserSession *pSession ) { return GetRoom()->GetPartyData(pSession); }
	__forceinline CDNGameRoom::PartyStruct *GetPartyData( WCHAR *pwszCharacterName ) { return GetRoom()->GetPartyData(pwszCharacterName); }
	__forceinline DWORD GetPartyUserCount(CDNGameRoom::eGetUserCountType type) { return GetRoom()->GetPartyUserCount(type); }
	bool IsPartyLeader( CDNUserSession* pSession );

public:
	DWORD m_dwHandle;
	bool m_bUseThread;
	HANDLE m_hThreadHandle;
	unsigned m_dwThreadID;
	bool m_bEnableTaskProcess;
	std::string m_szTaskName;

	LOCAL_TIME m_LocalTime;
	LOCAL_TIME m_PrevLocalTime;
	float m_fDelta;

	bool m_bDestroyTask;
	bool m_bDeleteThis;
	bool m_bResetTimer;

	CFrameSync *m_pFrameSync;
};
