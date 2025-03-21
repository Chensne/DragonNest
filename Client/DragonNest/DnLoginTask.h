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
	SCCharList						m_CharList;					// 캐릭터 리스트 패킷 미리 받기용

	int								m_nCurrentMapIndex;			// 맵 index
	std::vector<int>				m_vecTitleMapIndex;			// 타이틀 맵 index
	std::vector<TCharListData>		m_vecCharListData;			// 전체 캐릭터 리스트 데이터
	DnActorHandle					m_hSlotActorHandle;			// 선택 캐릭터 or 캐릭터 생성시 사용할 핸들
	DnEtcHandle						m_hCommonEffectHandle;		// 캐릭터 선택했을 때 이펙트 사용 핸들
	EtOutlineHandle					m_hSelectOutline;			// 선택 아웃라인 핸들
	std::vector<EtOutlineHandle>	m_hVecSelectCharOutline;	// 캐릭터 직업 선택할 때 한번 반짝이기용
	int								m_nSlotActorMapID;			// 선택 캐릭터 맵ID
	std::wstring					m_wstrCreateCharName;		// 케릭터 생성 완료페킷 받을 때 저장할 이름(완료 후 생성 캐릭터의 페이지로 이동할 때 사용)

	int								m_nCharSlotCount;			// 캐릭터슬롯 개수
	int								m_nCharSlotMaxCount;		// 캐릭터슬롯 최대 개수
	int								m_nSelectCharIndex;			// 선택한 캐릭터 index
	int								m_nSelectedServerIndex;		// 선택한 서버 index
	int								m_nSelectedChannelIndex;	// 선택한 채널 index

	std::vector<DnActorHandle>		m_hVecCreateActorList;			// 캐릭터 생성 창 액터 리스트
	std::vector<DefaultPartsStruct> m_VecCreateDefaultPartsList;	// 캐릭터 생성창 액터의 기본 파츠 리스트
	std::vector<DefaultPartsStruct>	m_VecPreviewCostumePartsList;	// 캐릭터 생성창 미리보기 파츠(30레벨 기본 장비, 길드 코스튬, 씨드 장비)
#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM
	std::vector<DefaultPartsStruct>	m_vecCharSelectDlgPreviewCostumePartsList;  // 캐릭터 선택창 미리보기 파츠( 30레벨 기본 장비만 가지고 있다 )
#endif
	
	EtOutlineHandle					m_hOutline;						// 아웃라인 핸들
	MatrixEx						m_CreateOffset;					// 캐릭터 생성창 액터 위치 (투명 프랍으로 맵에 설정됨)
	
	std::map<int, ClassPropCamInfo>	m_mapPropCamListByClass;		// 캐릭터별 카메라 애니 리스트

	int								m_nSelectClassIndex;		// 선택한 직업 index
	bool							m_bCompleteCharList;		// 케릭터 리스트 로드 완료 여부
	float							m_fChangePartsFadeInTime;	// 케릭터 생성 페이드인 시작전 여백

	LoginStateEnum					m_State;					// 로그인 테스크 상태
	LoginStateEnum					m_PrevState;				// 이전 상태
	CDnWorld*						m_pWorld;					// 월드

	int								m_nLoginTitleMapIndex;		// 타이틀 맵 인덱스
	int								m_nLoginCharMapIndex;		// 캐릭터 선택&생성 맵 인덱스
	int								m_nLoginTitlePropIndex;		// 타이틀 메인 프랍 인덱스

	float							m_fOldDOFNearBlurSize;
	float							m_fOldDOFFarBlurSize;
	float							m_fOriginalFarStart;
	float							m_fOriginalFarEnd;
	float							m_fOriginalFocusDist;

	float							m_fShowOutlineTime;			// 전체 아웃라인 보였다 감추는 시간
	bool							m_bRequestWait;

	int								m_nTitleSoundIndex;			// 타이틀 BGM
	int								m_nSelectSoundIndex;
	EtSoundChannelHandle			m_hTitleSoundChannel;		// 타이틀 BGM Channel
#ifdef PRE_ADD_RELOGIN_PROCESS
	int								m_nReLoginCount;
#endif // PRE_ADD_RELOGIN_PROCESS
	std::string						m_strTitleIdleCamera;			// 타이틀 기본 카메라
	std::string						m_strTitleFadeOutCamera;		// 타이틀 페이드아웃 카메라
	std::string						m_strSelectCamera;				// 캐릭터 선택 카메라
	std::string						m_strCharCreateSelectCamera;	// 캐릭터 생성 직업 선택 카메라
	std::string						m_strCharCreateStartCamera;		// 캐릭터 생성 시작 카메라
	int								m_nCharCreateStartSoundIndex;	// 캐릭터 생성 시작 사운드 인덱스
	std::string						m_strMakingCamera;				// 생성 카메라
	std::string						m_strMakingCameraFadeIn;		// 생성 카메라 페이드인

	bool							m_bStartSelectMove;				// 캐릭터 선택 이동
	bool							m_bStartCharSelect;				// 캐릭터 직업선택 앞부분 애니 나오고 UI 띄우기
	bool							m_bShowBlackDragonRadialBlur;	// 블랙드래곤 포효 카메라 효과
	bool							m_bCharSelectToServerList;		// 캐릭터 선택창에서 서버선택창으로 가기 (fade in관련 state가 꼬여 예외로 처리함;)
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
	void SetCharCreateBasePartsName();	// 캐릭터 생성 시 최초 기본 파츠명 UI 셋팅
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
	std::vector<TCharListData>	m_vecOriginCharListData;			// 전체 캐릭터 리스트 데이터 (최초 리스트 백업용)
	void SortCharacterList( eCharacterListSortCommand sortCommand );
	int GetCharacterListSortCommand() { return m_eCharacterListSortCommand; }
#endif // PRE_CHARLIST_SORTING

#ifdef PRE_ADD_DWC
	std::vector<TCharListData> GetMyCharListData() { return m_vecCharListData; } // 전체 캐릭터 리스트 데이터
#endif
};


#else // PRE_MOD_SELECT_CHAR


#define SHOWCHARNUMPERONEPAGE	4		// 한페이지당 보여지는 캐릭터 수

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

	std::vector<TCharListData> m_vecCharListData;	// 전체 캐릭터 리스트 데이터
	int m_nTotalCharSelectPage;						// 캐릭터 선택 페이지 수
	int m_nCurrentCharSelectPage;					// 현재 캐릭터 선택 페이지
	int m_nTotalCharSlotCount;						// 전체 캐릭터 슬롯 수
	int m_nEmptyCharSlotCount;						// 빈 캐릭터 슬롯 수
	int m_nDeleteCharPage;							// 캐릭터 삭제 했을 때의 현재 페이지 저장

	DnActorHandle m_hSlotActorHandle[SHOWCHARNUMPERONEPAGE];
	int m_nSlotActorMapID[SHOWCHARNUMPERONEPAGE];
	EtVector3 m_vSlotActorPos[SHOWCHARNUMPERONEPAGE];
	EtVector3 m_vSlotActorLook[SHOWCHARNUMPERONEPAGE];
	bool m_bSlotDeleteWait[SHOWCHARNUMPERONEPAGE];
	bool m_bSlotOpen[SHOWCHARNUMPERONEPAGE];	
	__time64_t m_SlotDelteWaitRemainTime[SHOWCHARNUMPERONEPAGE];
	
	std::wstring m_wstrCreateCharName;				// 케릭터 생성 완료페킷 받을 때 저장할 이름(완료 후 생성 캐릭터의 페이지로 이동할 때 사용)

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
	void SetCharSelectPage( int nSelectPage );	// 캐릭터 선택 페이지 설정

public:
	void PreInitialize();
	// Task 
	bool Initialize( LoginStateEnum State );

	void ReduceFarDOFBlurSize();	// 캐릭터 선택 화면을 위해서 특별히 하드코딩으로 DOF Far BlurSize를 줄여준다..
	void RestoreFarDOFBlurSize();			// (캐릭터 머리부분에 블러 많이 먹는거 방지) 
	
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
	void SetCharCreateBasePartsName();	// 캐릭터 생성 시 최초 기본 파츠명 UI 셋팅
#endif // PRE_MOD_CREATE_CHAR
	void NextCharListPage();			// 캐릭터 선택창에서 다음 캐릭터 페이지로 넘기기
	void PrevCharListPage();			// 캐릭터 선택창에서 이전 캐릭터 페이지로 넘기기
	int GetSelectExtendActorSlot();

#ifdef PRE_ADD_DOORS
	void OnRecvDoorsAuthKey(SCDoorsGetAuthKey * pPacket);
	void OnRecvDoorsCancelAuth(SCDoorsCancelAuth * pPacket);
	void OnRecvDoorsAuthFlag(SCDoorsGetAuthFlag* pPacket);
#endif		//#ifdef PRE_ADD_DOORS
};


#endif // PRE_MOD_SELECT_CHAR