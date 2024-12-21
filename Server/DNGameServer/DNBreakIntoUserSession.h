
#pragma once

#include "DNUserSession.h"

class CDNBreakIntoUserSession:public CDNUserSession
{
public:

	CDNBreakIntoUserSession( UINT uiUID, CDNRUDPGameServer* pServer, CDNGameRoom* pRoom );
	virtual ~CDNBreakIntoUserSession();

	virtual bool bIsBreakIntoSession(){ return m_bBreakIntoSession; }
#if defined(PRE_ADD_MULTILANGUAGE)
	virtual void PreInitializeUser(WCHAR * wszAccountName, UINT nAccountDBID, UINT nSessionID, INT64 biCharacterDBID, int iTeam, int nWorldID, int nVillageID, bool bTutorial, bool bAdult, char cPCBangGrade, char cSelectedLanguage, TMemberVoiceInfo * pInfo = NULL);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	virtual void PreInitializeUser(WCHAR * wszAccountName, UINT nAccountDBID, UINT nSessionID, INT64 biCharacterDBID, int iTeam, int nWorldID, int nVillageID, bool bTutorial, bool bAdult, char cPCBangGrade, TMemberVoiceInfo * pInfo = NULL);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	// 난입 프로세스
	virtual bool BreakIntoProcess();

private:

	bool m_bBreakIntoSession;
	UINT	m_uiGameRoomState;
	ULONG	m_uiNextGameRoomStateTick;

	void _SetGameRoomState( const UINT uiState ){ m_uiGameRoomState = uiState; }
};

class IBoostPoolDNBreakIntoUserSession : public CDNBreakIntoUserSession, public TBoostMemoryPool<IBoostPoolDNBreakIntoUserSession>
{
public :
	IBoostPoolDNBreakIntoUserSession( UINT uiUID, CDNRUDPGameServer* pServer, CDNGameRoom* pRoom )
		:CDNBreakIntoUserSession(uiUID,pServer,pRoom){}
	virtual ~IBoostPoolDNBreakIntoUserSession(){}
};
