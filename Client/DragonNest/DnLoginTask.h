#pragma once

#include "Task.h"
#include "InputReceiver.h"
#include "LoginClientSession.h"
#include "EtMatrixEx.h"
#include "DnParts.h"
#include "MAPartsBody.h"
#include "DnHangCrashTask.h"



#ifdef PRE_MOD_SELECT_CHAR


class CDnWorld;


class CDnLoginTask : public CTask, public CLoginClientSession, public CInputReceiver, public CEtUICallback, public CDnHangMonitorObject
{
public:
	CDnLoginTask();
	virtual ~CDnLoginTask();

	enum LoginStateEnum 
	{
		Logo,
		RequestIDPass,
		CharSelect,
		CharCreate_SelectClass,
		CharCreate_ChangeParts,
		CharDelete,
		StartGame,
		ServerList,
		ChannelList,
#if defined(PRE_ADD_DWC)
		CharCreate_DWC,
		CharCreate_DWC_Success,
#endif
		CharCreate_SelectClassDark,
	};
#ifdef PRE_ADD_RELOGIN_PROCESS
	std::wstring m_strConnectID;
	std::wstring m_strConnectPassword;
#endif // PRE_ADD_RELOGIN_PROCESS

protected:
	enum UICallbackEnum 
	{
		ConnectLoginFailed = 110000,
		CheckVersionFailed,
		CheckLoginFailed,
		CharListFailed,
		CharCreateFailed,
		CharDeleteFailed,
		GameInfoFailed,
		VillageInfoFailed,
		DisconnectTCP,
		SkipTutorial,
		CharReviveFailed,
	};

	enum 
	{
		HAIR_COLOR_COUNT = 5,//rlkt_dark
		SKIN_COLOR_COUNT = 4,
		EYE_COLOR_COUNT = 5,//rlkt_dark
	};

	enum
	{
		CAM_IDLE = 0,
		CAM_SELECT,
		CAM_DESELECT,
		CAM_FADEOUT,
	};

	struct DefaultPartsStruct 
	{
		std::vector<int> nVecList[EQUIPMAX];
		int nSelectPartsIndex[EQUIPMAX];
		DWORD dwColor[MAPartsBody::PartsColorAmount];
		int nWeapon[2];
		std::vector<DnPartsHandle> hVecParts[EQUIPMAX];
		DnWeaponHandle hWeapon[2];
#ifdef PRE_FIX_WEAPON_DUPLICATE_RENDER
		DnWeaponHandle hDeafaultWeapon[2];
#endif // PRE_FIX_WEAPON_DUPLICATE_RENDER
	};

	struct ClassPropCamInfo
	{
		int					nPropIndex;
		std::vector<string> vecCamList;
		std::vector<int>	vecSoundList;

		~ClassPropCamInfo() { vecCamList.clear(); vecSoundList.clear(); }
	};

	DnCameraHandle					m_hCamera;
#ifndef _FINAL_BUILD
	DnCameraHandle					m_hDebugCamera;
#endif // _FINAL_BUILD
	SCCharList						m_CharList;					// ĳ���� ����Ʈ ��Ŷ �̸� �ޱ��

	int								m_nCurrentMapIndex;			// �� index
	std::vector<int>				m_vecTitleMapIndex;			// Ÿ��Ʋ �� index
	std::vector<TCharListData>		m_vecCharListData;			// ��ü ĳ���� ����Ʈ ������
	DnActorHandle					m_hSlotActorHandle;			// ���� ĳ���� or ĳ���� ������ ����� �ڵ�
	DnEtcHandle						m_hCommonEffectHandle;		// ĳ���� �������� �� ����Ʈ ��� �ڵ�
	EtOutlineHandle					m_hSelectOutline;			// ���� �ƿ����� �ڵ�
	std::vector<EtOutlineHandle>	m_hVecSelectCharOutline;	// ĳ���� ���� ������ �� �ѹ� ��¦�̱��
	int								m_nSlotActorMapID;			// ���� ĳ���� ��ID
	std::wstring					m_wstrCreateCharName;		// �ɸ��� ���� �Ϸ���Ŷ ���� �� ������ �̸�(�Ϸ� �� ���� ĳ������ �������� �̵��� �� ���)

	int								m_nCharSlotCount;			// ĳ���ͽ��� ����
	int								m_nCharSlotMaxCount;		// ĳ���ͽ��� �ִ� ����
	int								m_nSelectCharIndex;			// ������ ĳ���� index
	int								m_nSelectedServerIndex;		// ������ ���� index
	int								m_nSelectedChannelIndex;	// ������ ä�� index

	std::vector<DnActorHandle>		m_hVecCreateActorList;			// ĳ���� ���� â ���� ����Ʈ
	std::vector<DefaultPartsStruct> m_VecCreateDefaultPartsList;	// ĳ���� ����â ������ �⺻ ���� ����Ʈ
	std::vector<DefaultPartsStruct>	m_VecPreviewCostumePartsList;	// ĳ���� ����â �̸����� ����(30���� �⺻ ���, ��� �ڽ�Ƭ, ���� ���)
#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM
	std::vector<DefaultPartsStruct>	m_vecCharSelectDlgPreviewCostumePartsList;  // ĳ���� ����â �̸����� ����( 30���� �⺻ ��� ������ �ִ� )
#endif
	
	EtOutlineHandle					m_hOutline;						// �ƿ����� �ڵ�
	MatrixEx						m_CreateOffset;					// ĳ���� ����â ���� ��ġ (���� �������� �ʿ� ������)
	
	std::map<int, ClassPropCamInfo>	m_mapPropCamListByClass;		// ĳ���ͺ� ī�޶� �ִ� ����Ʈ

	int								m_nSelectClassIndex;		// ������ ���� index
	bool							m_bCompleteCharList;		// �ɸ��� ����Ʈ �ε� �Ϸ� ����
	float							m_fChangePartsFadeInTime;	// �ɸ��� ���� ���̵��� ������ ����

	LoginStateEnum					m_State;					// �α��� �׽�ũ ����
	LoginStateEnum					m_PrevState;				// ���� ����
	CDnWorld*						m_pWorld;					// ����

	int								m_nLoginTitleMapIndex;		// Ÿ��Ʋ �� �ε���
	int								m_nLoginCharMapIndex;		// ĳ���� ����&���� �� �ε���
	int								m_nLoginTitlePropIndex;		// Ÿ��Ʋ ���� ���� �ε���

	float							m_fOldDOFNearBlurSize;
	float							m_fOldDOFFarBlurSize;
	float							m_fOriginalFarStart;
	float							m_fOriginalFarEnd;
	float							m_fOriginalFocusDist;

	float							m_fShowOutlineTime;			// ��ü �ƿ����� ������ ���ߴ� �ð�
	bool							m_bRequestWait;

	int								m_nTitleSoundIndex;			// Ÿ��Ʋ BGM
	int								m_nSelectSoundIndex;
	EtSoundChannelHandle			m_hTitleSoundChannel;		// Ÿ��Ʋ BGM Channel
#ifdef PRE_ADD_RELOGIN_PROCESS
	int								m_nReLoginCount;
#endif // PRE_ADD_RELOGIN_PROCESS
	std::string						m_strTitleIdleCamera;			// Ÿ��Ʋ �⺻ ī�޶�
	std::string						m_strTitleFadeOutCamera;		// Ÿ��Ʋ ���̵�ƿ� ī�޶�
	std::string						m_strSelectCamera;				// ĳ���� ���� ī�޶�
	std::string						m_strCharCreateSelectCamera;	// ĳ���� ���� ���� ���� ī�޶�
	std::string						m_strCharCreateStartCamera;		// ĳ���� ���� ���� ī�޶�
	int								m_nCharCreateStartSoundIndex;	// ĳ���� ���� ���� ���� �ε���
	std::string						m_strMakingCamera;				// ���� ī�޶�
	std::string						m_strMakingCameraFadeIn;		// ���� ī�޶� ���̵���

	bool							m_bStartSelectMove;				// ĳ���� ���� �̵�
	bool							m_bStartCharSelect;				// ĳ���� �������� �պκ� �ִ� ������ UI ����
	bool							m_bShowBlackDragonRadialBlur;	// ���巡�� ��ȿ ī�޶� ȿ��
	bool							m_bCharSelectToServerList;		// ĳ���� ����â���� ��������â���� ���� (fade in���� state�� ���� ���ܷ� ó����;)
	bool							m_bLoginInitParts;
protected:
	void InitSelectCharacter();
	void ShowSelectCharacter( bool bShow );
	void ShowSelectCharOutline( bool bShow, float fSpeed );
	void InitCreateCharacter();
	void ShowCreateCharacter( bool bShow, int nClassID = -1 );
	int GetCreateSelectClass();
	void SetSelectClassOutLine( int nSelectIndex );

	void InitShowPartsList();
	void ResetShowPartsList();
	void ShowDefaultPartsList( bool bFirstRandom = false );
	void DetachAllParts();

	void SetRequestWait( bool bWait );
	bool LoadTitle();
	bool LoadWorld( int nMapIndex );
	void PlayDefaultCamera();

	void GetOriginalDOFInfo();
	void RestoreOriginalDOFInfo();
	void SetDOFInfo( float fDOFStart, float fDOFEnd, float fDOFFocusDistance, float fNearBlurSize, float fFarBlurSize );
	static bool SortByServerIndex( const TCharListData& a, const TCharListData& b );
#ifdef PRE_CHARLIST_SORTING
	static bool SortByCreate( const TCharListData& a, const TCharListData& b );
	static bool SortByName( const TCharListData& a, const TCharListData& b );
	static bool SortByLevel( const TCharListData& a, const TCharListData& b );
	static bool SortByJob( const TCharListData& a, const TCharListData& b );
	static bool SortByLatelyLogin( const TCharListData& a, const TCharListData& b );
#endif // PRE_CHARLIST_SORTING

public:
	void PreInitialize( bool bReload = false );
	bool Initialize( LoginStateEnum State );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// CInputReceiver
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime = -1 );

	// Client Session
	virtual void OnConnectTcp();
	virtual void OnDisconnectTcp( bool bValidDisconnect );
	// Tcp
	virtual void OnRecvLoginCheckVersion( SCCheckVersion *pPacket );
	virtual void OnRecvLoginCheckLogin( SCCheckLogin *pPacket );
	virtual void OnRecvLoginCharList( SCCharList *pPacket );
	void _OnRecvLoginCharList( SCCharList *pPacket );
	virtual void OnRecvLoginWaitUser( SCWaitUser *pPacket );
	virtual void OnRecvLoginBlock( SCCHeckBlock *pPacket );
	virtual void OnRecvLoginCharCreate( SCCreateChar *pPacket );
	virtual void OnRecvLoginCharDelete( SCDeleteChar *pPacket );
	virtual void OnRecvLoginServerList( SCServerList *pPacket );
	virtual void OnRecvLoginChannelList( SCChannelList *pPacket );
	virtual void OnRecvLoginBackButton();
	virtual void OnRecvLoginReviveChar( SCReviveChar *pPacket );
#if defined(PRE_ADD_DWC)
	virtual void OnRecvDWCCharCreate(SCCreateDWCChar *pPacket);
#endif // PRE_ADD_DWC
	
#if defined (_TH) && defined(_AUTH)
	virtual void OnRecvLoginAsiaSoftReqOTP();
	virtual void OnRecvLoginAsiaSoftRetOTP(SCOTRPResult * pPacket);
#endif	// _TH && _AUTH

	virtual void OnRecvSystemGameInfo( SCGameInfo *pPacket );
	virtual void OnRecvSystemVillageInfo( SCVillageInfo *pPacket );

	// CEtUICallback
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

	// Local
	bool IsEmptySelectActorSlot();
	int GetSelectActorSlot() { return m_nSelectCharIndex; }
	void ChangeState( LoginStateEnum State, bool bAutoSendMessage = true );

	void CreateCharacter( TCHAR* szName, int nSelectServerIndex );
	void DeleteCharacter();
	bool CheckSelectCharDeleteWait();
	bool IsSelectClass() { return m_nSelectClassIndex != -1 ? 1 : 0; }
	void SetAuthPassword( int nSeed, int nValue[] ) 
	{
		m_nAuthPasswordSeed = 1234;
		memcpy( m_nAuthPasswordValue, nValue, sizeof(m_nAuthPasswordValue) );
	}

	int m_nAuthPasswordSeed;
	int m_nAuthPasswordValue[4];

	void SetCharSelect( int nSelectIndex );
	void ReviveCharacter( int nSlotIndex );
	void SelectServer( int nServerIndex );
	void SelectChannel( int nChannelIndex );
	int  GetSelectServerIndex(){return m_nSelectedServerIndex;}

	void SetCharColor( MAPartsBody::PartsColorEnum colorType, int nColorIndex, int nClassIndex = -1);
	void RotateCreateParts( CDnParts::PartsTypeEnum PartsIndex, bool bAdd );

	bool IsCompleteCharList() { return m_bCompleteCharList; }
	bool IsRequestWait() { return m_bRequestWait; }

	int GetCurrentSelectLevel();
	LoginStateEnum GetState() { return m_State; }

	virtual bool OnCloseTask();

	void DeSelectCharacter();
	void StartCharacterChangeParts();
	void SelectMoveCharacter( int nSelectIndex );

	INT64 GetSelectCharacterDBID( int nSelectIndex );
#ifdef PRE_MOD_CREATE_CHAR
	void SetCharCreateBasePartsName();	// ĳ���� ���� �� ���� �⺻ ������ UI ����
#endif // PRE_MOD_CREATE_CHAR
	void CharacterAddRotateYaw( float fAngle );
	void CharCreateGestureAction( const char* pGestureActionName, bool bCharCreate );
	void ShowPartsList( int nPartsListIndex );

#ifdef PRE_ADD_DOORS
	void OnRecvDoorsAuthKey(SCDoorsGetAuthKey * pPacket);
	void OnRecvDoorsCancelAuth(SCDoorsCancelAuth * pPacket);
	void OnRecvDoorsAuthFlag(SCDoorsGetAuthFlag* pPacket);
#endif		//#ifdef PRE_ADD_DOORS

#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM
	enum {
		PREVIEW_CASH_COSTUME_RESET = 0 ,
		PREVIEW_CASH_COSTUME_ATTACH = 1,
	};

	void ResetPreviewCashCostume();
	void ShowPreviewCashCostume(int nRadioBtnIndex);
	void AttachPreviewCashCostume(int nRadioBtnIndex);
	void DetachPreviewCashCostume();

	void InitPreviewCashCostumeList();
	void ResetPreviewCashCostumeList();
#endif
	void SetSelectedCharacterDark(int JobID);
	LoginStateEnum	GetLastState(){ return m_PrevState; }
#ifdef PRE_CHARLIST_SORTING
	enum eCharacterListSortCommand
	{
		CHARACTERLIST_SORT_BY_NONE = 0,
		CHARACTERLIST_SORT_BY_SERVER = 1,
		CHARACTERLIST_SORT_BY_CREATE,
		CHARACTERLIST_SORT_BY_NAME,
		CHARACTERLIST_SORT_BY_LEVEL,
		CHARACTERLIST_SORT_BY_JOB,
		CHARACTERLIST_SORT_BY_LATELYLOGIN,
	};
	eCharacterListSortCommand	m_eCharacterListSortCommand;
	std::vector<TCharListData>	m_vecOriginCharListData;			// ��ü ĳ���� ����Ʈ ������ (���� ����Ʈ �����)
	void SortCharacterList( eCharacterListSortCommand sortCommand );
	int GetCharacterListSortCommand() { return m_eCharacterListSortCommand; }
#endif // PRE_CHARLIST_SORTING

#ifdef PRE_ADD_DWC
	std::vector<TCharListData> GetMyCharListData() { return m_vecCharListData; } // ��ü ĳ���� ����Ʈ ������
#endif
};


#else // PRE_MOD_SELECT_CHAR


#define SHOWCHARNUMPERONEPAGE	4		// ���������� �������� ĳ���� ��

class CDnWorld;

class CDnLoginTask : public CTask, public CLoginClientSession, public CInputReceiver, public CEtUICallback, public CDnHangMonitorObject
{
public:
	CDnLoginTask();
	virtual ~CDnLoginTask();

	enum LoginStateEnum {
		Logo,
		RequestIDPass,
		CharSelect,
		CharCreate_SelectClass,
		CharCreate_ChangeParts,
		CharDelete,
		StartGame,
		ServerList,
		ChannelList,
	};
#ifdef PRE_ADD_RELOGIN_PROCESS
	std::wstring m_strConnectID;
	std::wstring m_strConnectPassword;
#endif // PRE_ADD_RELOGIN_PROCESS
	
protected:
	enum UICallbackEnum {
		ConnectLoginFailed = 110000,
		CheckVersionFailed,
		CheckLoginFailed,
		CharListFailed,
		CharCreateFailed,
		CharDeleteFailed,
		GameInfoFailed,
		VillageInfoFailed,
		DisconnectTCP,
		SkipTutorial,
		CharReviveFailed,
		ComeBack,
	};

	enum {
		HAIR_COLOR_COUNT = 4,
		SKIN_COLOR_COUNT = 4,
		EYE_COLOR_COUNT = 4,
	};

	struct DefaultPartsStruct {
		std::vector<int> nVecList[EQUIPMAX];
		int nSelectPartsIndex[EQUIPMAX];
		DWORD dwColor[MAPartsBody::PartsColorAmount];
		int nWeapon[2];
		std::vector<DnPartsHandle> hVecParts[EQUIPMAX];
		DnWeaponHandle hWeapon[2];
	};

	DnCameraHandle m_hCamera;

	std::vector<TCharListData> m_vecCharListData;	// ��ü ĳ���� ����Ʈ ������
	int m_nTotalCharSelectPage;						// ĳ���� ���� ������ ��
	int m_nCurrentCharSelectPage;					// ���� ĳ���� ���� ������
	int m_nTotalCharSlotCount;						// ��ü ĳ���� ���� ��
	int m_nEmptyCharSlotCount;						// �� ĳ���� ���� ��
	int m_nDeleteCharPage;							// ĳ���� ���� ���� ���� ���� ������ ����

	DnActorHandle m_hSlotActorHandle[SHOWCHARNUMPERONEPAGE];
	int m_nSlotActorMapID[SHOWCHARNUMPERONEPAGE];
	EtVector3 m_vSlotActorPos[SHOWCHARNUMPERONEPAGE];
	EtVector3 m_vSlotActorLook[SHOWCHARNUMPERONEPAGE];
	bool m_bSlotDeleteWait[SHOWCHARNUMPERONEPAGE];
	bool m_bSlotOpen[SHOWCHARNUMPERONEPAGE];	
	__time64_t m_SlotDelteWaitRemainTime[SHOWCHARNUMPERONEPAGE];
	
	std::wstring m_wstrCreateCharName;				// �ɸ��� ���� �Ϸ���Ŷ ���� �� ������ �̸�(�Ϸ� �� ���� ĳ������ �������� �̵��� �� ���)

	int m_nCharSlotCount;
	int m_nSelectIndex;
	int m_nSelectedServerIndex;
	int m_nSelectedChannelIndex;

	std::vector< DnActorHandle > m_hVecCreateActorList;
	std::vector< DefaultPartsStruct > m_VecCreateDefaultPartsList;	
	std::vector<EtOutlineHandle> m_vecOutlineSlot;
	std::vector<EtOutlineHandle> m_vecOutlineCreate;

	MatrixEx m_TreeOffset;

	int m_nSelectClassIndex;

//	EtParticleHandle m_hParticle;
//	int m_nParticleDataIndex;
//	MatrixEx m_ParticleCross;

	float m_fCameraDelta;
	float m_fCameraYaw;
//	bool m_bRecvCharList;
	bool m_bProcessCamera;
	bool m_bShowHideActor;
	bool m_bCompleteCharList;

	LoginStateEnum		m_State;
	LoginStateEnum		m_PrevState;
	CDnWorld			*m_pWorld;	

	SCCharList			m_CharList;
	
	float				m_fOldDOFNearBlurSize;
	float				m_fOldDOFFarBlurSize;	
	bool				m_bRequestWait;
#ifdef PRE_ADD_RELOGIN_PROCESS
	int					m_nReLoginCount;
#endif // PRE_ADD_RELOGIN_PROCESS
	
protected:
	void CalcCharSlotPosition();
	bool SelectActorSlot( int nIndex );
	void ShowSlotInfoDlg();
	void ShowSlotInfoDlg( int nSlotIndex );
	void ProcessDeleteWait( LOCAL_TIME LocalTime, float fDelta );

	void ShowCreateCharacter( bool bShow );
	int GetCreateSelectClass();

	void SetRequestWait( bool bWait );
	void SetCharSelectPage( int nSelectPage );	// ĳ���� ���� ������ ����

public:
	void PreInitialize();
	// Task 
	bool Initialize( LoginStateEnum State );

	void ReduceFarDOFBlurSize();	// ĳ���� ���� ȭ���� ���ؼ� Ư���� �ϵ��ڵ����� DOF Far BlurSize�� �ٿ��ش�..
	void RestoreFarDOFBlurSize();			// (ĳ���� �Ӹ��κп� �� ���� �Դ°� ����) 
	
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// CInputReceiver
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime = -1 );

	// Client Session
	virtual void OnConnectTcp();
	virtual void OnDisconnectTcp( bool bValidDisconnect );
	// Tcp
	virtual void OnRecvLoginCheckVersion( SCCheckVersion *pPacket );
	virtual void OnRecvLoginCheckLogin( SCCheckLogin *pPacket );
	virtual void OnRecvLoginCharList( SCCharList *pPacket );
	void _OnRecvLoginCharList( SCCharList *pPacket );
	virtual void OnRecvLoginWaitUser( SCWaitUser *pPacket );
	virtual void OnRecvLoginBlock( SCCHeckBlock *pPacket );
	virtual void OnRecvLoginCharCreate( SCCreateChar *pPacket );
	virtual void OnRecvLoginCharDelete( SCDeleteChar *pPacket );
	virtual void OnRecvLoginServerList( SCServerList *pPacket );
	virtual void OnRecvLoginChannelList( SCChannelList *pPacket );
	virtual void OnRecvLoginBackButton();
	virtual void OnRecvLoginReviveChar( SCReviveChar *pPacket );
#if defined (_TH) && defined(_AUTH)
	virtual void OnRecvLoginAsiaSoftReqOTP();
	virtual void OnRecvLoginAsiaSoftRetOTP(SCOTRPResult * pPacket);
#endif	// _TH && _AUTH

	virtual void OnRecvSystemGameInfo( SCGameInfo *pPacket );
	virtual void OnRecvSystemVillageInfo( SCVillageInfo *pPacket );

	// CEtUICallback
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

	// Local
	bool IsEmptySelectActorSlot();
	int GetSelectActorSlot() { return m_nSelectIndex; }
	void SetSelectActorSlot( int nValue ) { m_nSelectIndex = nValue; }
	void ChangeState( LoginStateEnum State, bool bAutoSendMessage = true );
	
	void CreateCharacter( TCHAR *szName );
	void DeleteCharacter();
	void SetAuthPassword( int nSeed, int nValue[] ) {
		m_nAuthPasswordSeed = nSeed;
		memcpy( m_nAuthPasswordValue, nValue, sizeof(m_nAuthPasswordValue) );
	}
	int m_nAuthPasswordSeed;
	int m_nAuthPasswordValue[4];
	void ReviveCharacter( int nSlotIndex );
	void GetCharSelectMousePos( std::vector<EtVector2> &vecPos, float fScreenWidthRatio, float fScreenHeightRatio );
	
	void SelectServer( int nServerIndex );
	void SelectChannel( int nChannelIndex );
	int  GetServerIndex(){return m_nSelectedServerIndex;}
	void ChangeCreateClass( int nClassID );
	void SetCharColor( MAPartsBody::PartsColorEnum colorType, int nColorIndex, int nClassIndex = -1);
	void RotateCreateParts( CDnParts::PartsTypeEnum PartsIndex, bool bAdd );
	void TurnCharacter();

	bool IsCompleteCharList() { return m_bCompleteCharList; }
	bool IsRequestWait() { return m_bRequestWait; }

	int GetCurrentSelectLevel();
	LoginStateEnum GetState() { return m_State; }

	CEtOutlineFilter *GetOutlineFilter();

	virtual bool OnCloseTask();

#ifdef PRE_MOD_CREATE_CHAR
	void SetCharCreateBasePartsName();	// ĳ���� ���� �� ���� �⺻ ������ UI ����
#endif // PRE_MOD_CREATE_CHAR
	void NextCharListPage();			// ĳ���� ����â���� ���� ĳ���� �������� �ѱ��
	void PrevCharListPage();			// ĳ���� ����â���� ���� ĳ���� �������� �ѱ��
	int GetSelectExtendActorSlot();

#ifdef PRE_ADD_DOORS
	void OnRecvDoorsAuthKey(SCDoorsGetAuthKey * pPacket);
	void OnRecvDoorsCancelAuth(SCDoorsCancelAuth * pPacket);
	void OnRecvDoorsAuthFlag(SCDoorsGetAuthFlag* pPacket);
#endif		//#ifdef PRE_ADD_DOORS
};


#endif // PRE_MOD_SELECT_CHAR