#pragma once
//blondymarry start
// 일반 빌리지 태스크를 상속받아 오직 UI만 띄우는 마을 역활을 한다 빌리지 상속받는 이유는 오직 커넥션유지만을 위해서 클라이언트디자인을 
// 헤치지 않기위해 상속받게 됐다(나중에 서버에따로 붙을 수 있는 구조가 되면 리팩토링을 할지도 )
//blondymarry end
#include "DnVillageTask.h"

class CDnPVPLobbyVillageTask :public CTask, public CVillageClientSession, public CDnLoadingStateMachine, public CEtUICallback, public CInputReceiver
{

public:
	CDnPVPLobbyVillageTask(void);
	~CDnPVPLobbyVillageTask(void);
protected:
	DnCameraHandle m_hCamera;
	bool		m_bFirstConnect;
	bool		m_bEntered;

	SCEnter     m_sUserInfo;

	EtTextureHandle m_hTexture;
	// 로비 BGM
	int m_nLobbyBGMIndex;
	EtSoundChannelHandle m_hLobbyBGM;

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	static DnActorHandle m_hPreloadPlayer[10];
	static bool s_bPreloadComplete;
#endif

protected:
	void PlayLobbyBGM();
	void StopLobbyBGM();

public:
	bool Initialize( bool bFirstConnect = true );
	//bool InitializeStage( const char *szGridName, const char *szEnviName, int nMapTableID );
	void FinalizeStage();

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	static void InitializePreloadPlayer();
	static void ReleasePreloadPlayer();
#endif

	bool RenderScreen( LOCAL_TIME LocalTime, float fDelta );
	bool GetEnterd(){return m_bEntered;};
#ifdef PRE_ADD_PVPRANK_INFORM
	const SCEnter& GetUserInfo() const { return m_sUserInfo; }
#else
	SCEnter GetUserInfo(){return m_sUserInfo;}
#endif
	void SetUserInfo(SCEnter sUserInfo){m_sUserInfo = sUserInfo;}

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void OnDisconnectTcp( bool bValidDisconnect );


	virtual void OnRecvCharMapInfo( SCMapInfo *pPacket );
	virtual void OnRecvCharEntered( SCEnter *pPacket );
	virtual void OnRecvCharUserEntered( SCEnterUser *pPacket );
	virtual void OnRecvCharUserLeave( SCLeaveUser *pPacket );

#ifdef PRE_PARTY_DB
	virtual bool OnRecvPartyListInfo( SCPartyListInfo *pPacket );
#else
	virtual void OnRecvPartyListInfo( SCPartyListInfo *pPacket );
#endif
	virtual void OnRecvPartyMemberInfo( SCPartyMemberInfo *pPacket );

	virtual void OnRecvPartyCreateParty( SCCreateParty *pPacket );
	virtual void OnRecvPartyRefresh( SCRefreshParty *pPacket );
	virtual void OnRecvPartyJoinParty( SCJoinParty *pPacket );
	virtual void OnRecvPartyOutParty( SCPartyOut *pPacket );

	virtual void OnRecvPartyRefreshGateInfo( SCRefreshGateInfo *pPacket );
	virtual void OnRecvPartyReadyGate( SCGateInfo *pPacket );	
	virtual void OnRecvPartyStageCancel( SCCancelStage *pPacket );

	virtual void OnRecvStartMsg( SCPVP_STARTMSG * pData );
};
