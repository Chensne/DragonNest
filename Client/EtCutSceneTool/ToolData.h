#pragma once

#include "Singleton.h"
#include "Observable.h"
#include "ICmdReceiver.h"
#include "DnCutSceneData.h"
#include "IResourcePathFinder.h"
#include "DNTableFile.h"
#include "TEditData.h"


#define TL_ACTION_POSTFIX wxT("_ACTION")
#define TL_KEY_POSTFIX wxT("_KEY")


class CCmdProcessor;
class ICommand;
class cwxPropertyPanel;
class CRTCutSceneRenderer;
class cwxTimeLineCtrl;
class cwxEventTimeLinePanel;





struct S_ACTOR_RES_INFO
{
	int			iTableID;
	wxString	strActorName;
	wxString	strAniFileName;
	wxString	strSkinFileName;
	wxString	strActionFileName;
};


struct S_PROP_INFO
{
	wxString strPropName;
	int iPropID;
	wxArrayString strActionList;
	vector<DWORD> vldwLength;

	S_PROP_INFO( void ) : iPropID( -1 )
	{

	}
};

struct S_MONSTER_INFO
{
	int			iActorTableID;
	int			iMonsterTableID;		// 실제 저장되어 클라이언트에서 참조하게 되는 인덱스. 몬스터별로 유니크한 몬스터 테이블 인덱스임.
	wxString	strName;
	float		m_fScale;

	S_MONSTER_INFO( void ) : iActorTableID( 0 ), iMonsterTableID( 0 ), m_fScale( 0.0f )
	{

	}
};


// 툴 전체에서 사용하는 전역 변수를 모아놓은 것 
class CToolData : public CSingleton<CToolData>,
				  public ICmdReceiver,
				  public CObservable,
				  public IResourcePathFinder			// from RTCutSceneCore
{
public:
	// 시퀀스의 사용 타입. 타임 라인 컨트롤을 사용할 때 쓰인다.
	enum
	{
		ACTION,
		KEY,
		SUBKEY,

		// 이벤트
		MAX_CAMERA,
		PARTICLE,
		DOF,
		FADE,
		PROP,
		SOUND_1,
		SOUND_2,
		SOUND_3,
		SOUND_4,
		SOUND_5,
		SOUND_6,
		SOUND_7,
		SOUND_8,
		SUBTITLE,
		IMAGE,

#ifdef PRE_ADD_FILTEREVENT
		COLORFILTER,
#endif // PRE_ADD_FILTEREVENT

		COUNT,
	};


	// 툴의 편집 모드
	enum
	{
		EM_EDIT,
		EM_PLAY,
		EM_FREE_TIME,
		EM_COUNT,
	};

	// 현재 클라에 정의되어있는 몬스터 등급
	enum GradeEnum 
	{
		Minion,
		Normal,
		Champion,
		Elite,
		Named,
		Boss,
		NestBoss,
	};


private:
	wxWindow*			m_pMainFrame;
	cwxPropertyPanel*	m_pPropertyPanel;
	CRTCutSceneRenderer* m_pRenderer;
	cwxTimeLineCtrl*	m_pActionTimeLineCtrl;
	cwxEventTimeLinePanel* m_pEventTLPanel;

	CDnCutSceneData*	m_pCoreData;
	CCmdProcessor*		m_pCmdProcessor;
	bool				m_bModified;

	// 리소스 테이블. SOX 파일을 읽어들인다.
	DNTableFileFormat*			m_pFileTable;
	DNTableFileFormat*			m_pActorTable;
	DNTableFileFormat*			m_pMapTable;
	DNTableFileFormat*			m_pWeatherTable;
	DNTableFileFormat*			m_pFaceAniTable;
	DNTableFileFormat*			m_pMonsterTable;
	DNTableFileFormat*			m_pWeaponTable;
	DNTableFileFormat*			m_pNPCTable;

	//struct S_RES_PAIR
	//{
	//	wxString strResFullPath;
	//	int iResourceType;
	//};

	vector<S_PROP_INFO>						m_vlPropInfo;
	vector<S_ACTOR_RES_INFO*>				m_vlpActorResInfo;
	vector<S_MONSTER_INFO*>			m_vlpBossMonsterInfo;
	map<wxString, S_ACTOR_RES_INFO*>		m_mapActorResInfo;
	vector<wxString>						m_vlMapResNames;
	map<wxString, wxString>	m_mapMapNameToEnvFile;
	map<wxString, int>		m_mapResKindDB;
	map<wxString, wxString> m_mapResFullPath;

	int					m_iNowSelectedRegRes;
	
	int					m_iSelectedObjectID;
	int					m_iSelectedObjectUseType;

	DWORD				m_dwLocalTime;

	int					m_iEditMode;

	char				m_caBuf[ 256 ];

	wxString			m_strOpenFilePath;
	wxString			m_strOpenFileName;

	bool				m_bEdited;
	bool				m_bWritable;

	ActionInfo*			m_pActionInfoClipboard;
	KeyInfo*			m_pKeyInfoClipboard;
	EventInfo*			m_pEventInfoClipboard;

public:
	CToolData(void);
	virtual ~CToolData(void);

	void Initialize( void );

	void MakeUIStringUseVariableParam( wstring &wszStr, int nMessageId, char *szParam );

	int GetNumActorResInfo( void ) { return (int)m_vlpActorResInfo.size(); };
	const S_ACTOR_RES_INFO* GetActorResInfoByIndex( int iIndex );
	const S_ACTOR_RES_INFO* GetActorResInfoByName( const wxString& strActorName );

	int GetNumMapResName( void ) { return (int)m_vlMapResNames.size(); };
	const wxChar* GetMapResNameByIndex( int iIndex );

	const wxChar* GetEnvFileName( const wxString& strMapName );

	void SetMainFrame( wxWindow* pMainFrame );
	wxWindow* GetMainFrame( void ) { return m_pMainFrame; };

	void SetPropertyPanel( cwxPropertyPanel* pPropertyPanel ) { m_pPropertyPanel = pPropertyPanel; };
	cwxPropertyPanel* GetPropertyPanel( void ) { return m_pPropertyPanel; };

	void SetRenderer( CRTCutSceneRenderer* pRenderer ) { m_pRenderer = pRenderer; };
	CRTCutSceneRenderer* GetRenderer( void ) { return m_pRenderer; };

	void SetLocalTime( DWORD dwLocalTime ) { m_dwLocalTime = dwLocalTime; };
	DWORD GetLocalTime( void ) { return m_dwLocalTime; };

	CDnCutSceneData* GetCoreData( void ) { return m_pCoreData; };

	void SetActionTimeLine( cwxTimeLineCtrl* pTimeLineCtrl ) { m_pActionTimeLineCtrl = pTimeLineCtrl; };
	cwxTimeLineCtrl* GetActionTimeLine( void ) { return m_pActionTimeLineCtrl; };
	
	void SetEventTLPanel( cwxEventTimeLinePanel* pPanel ) { m_pEventTLPanel = pPanel; };
	cwxEventTimeLinePanel* GetEventTLPanel( void ) { return m_pEventTLPanel; };

	void WideCharToMultiByte( const wxChar* pSource, char* pResult, int iSize = 256 );

	void SetResourceType( const wxChar* pResFileName, int iResType );
	int GetResourceKind( const wxChar* pResFileName );
	//int GetResourceTypeByIndex( int iResIndex );

	int GetNumRegRes( void );
	int GetRegResKindByIndex( int iIndex );
	void GetRegResNameByIndex( int iIndex, /*IN OUT*/ wxString& wxResName );

	void SetFullPath( const wxChar* pResName, const wxChar* pFullPath );
	const wxChar* GetFullPath( const wxChar* pResName );
	void GetFullPathA( const wxChar* pResName, /*IN OUT*/ string& strFullPath );
	
	// from IResourcePathFinder ///////
	void GetFullPath( const char* pFileName, /*IN OUT*/ string& strFullPath );
	////////////////////////////////////////////////////////////////

	bool IsRegResource( const wxChar* pResName );

	void SelectRegResource( int iSelect ) { m_iNowSelectedRegRes = iSelect; };
	int GetSelectedRegResIndex( void ) { return m_iNowSelectedRegRes; };

	void SetSelectedObjectID( int iSelectedObjectID ) { m_iSelectedObjectID = iSelectedObjectID; };
	int GetSelectedObjectID( void ) { return m_iSelectedObjectID; };

	void SetSelectedObjectUseType( int iSelectedObjectUseType ) { m_iSelectedObjectUseType = iSelectedObjectUseType; };
	int GetSelectedObjectUseType( void ) { return m_iSelectedObjectUseType; };

	// from ICmdReceiver
	void SetModified( bool bModified );
	bool GetModified( void ) { return m_bModified; };

	bool RegisterResource( const wxChar* pResName, int iResourceKind );
	bool UnRegisterResource( const wxChar* pResName, int iResourceKind ); 

	bool SetMonsterTableID( const wxChar* pActorName, int iMonsterTableID ); // 보스 몬스터 정보 셋팅.
	int GetMonsterTableID( const wxChar* pActorName );

	int GenerateID( void );

	// 신 전체 속성 관련
	const SceneInfo* GetSceneInfo( void ) { return m_pCoreData->GetSceneInfo(); };
	bool ModifySceneInfo( SceneInfo* pSceneInfo ) { return m_pCoreData->ModifySceneInfo( pSceneInfo ); };
	
	// 액션 관련
	bool InsertAction( ActionInfo* pActionInfo );
	bool RemoveAction( int iID );
	bool ModifyActionInfo( ActionInfo& Action );

	// 키 데이터 관련
	bool InsertKey( KeyInfo* pKeyInfo );
	bool RemoveKey( int iID );
	bool ModifyKeyInfo( KeyInfo& Key );

	// 이벤트 관련
	bool InsertEvent( EventInfo* pEventInfo );
	bool RemoveEvent( int iID );
	bool ModifyEventInfo( EventInfo* pEventInfo );

	int GetNumActors( void );
	void GetActorNameByIndex( int iActorIndex, /*IN OUT*/ wxString& strActorName  );
	
	const ActionInfo* GetActionInfoByID( int iID );
	int GetThisActorsActionNum( const wxChar* pActorName );
	const ActionInfo* GetThisActorsActionInfoByIndex( const wxChar* pActorName, int iActionIndex );

	const KeyInfo* GetKeyInfoByID( int iID );
	int GetThisActorsKeyNum( const wxChar* pActorName );
	const KeyInfo* GetThisActorsKeyInfoByIndex( const wxChar* pActorName, int iKeyIndex );
	
	int GetThisActorsAnimationNum( const wxChar* pActorName );
	const ActionEleInfo* GetThisActorsAnimation( const wxChar* pActorName, int iAnimationIndex );

	// 현재 띄운 맵에 관하여,, 프랍 관련 정보 조회
	void AddActionPropInfo( const S_PROP_INFO& PropInfo ) { m_vlPropInfo.push_back(PropInfo); };
	int GetNumActionProp( void ) { return (int)m_vlPropInfo.size(); };
	const S_PROP_INFO* GetActionPropInfo( int iIndex ) { return (iIndex < (int)m_vlPropInfo.size()) ? &m_vlPropInfo.at(iIndex) : NULL; };
	void ClearPropInfo( void ) { m_vlPropInfo.clear(); };

	bool GetThisActorsFitYPosToMap( const wxChar* pActorName );
	bool SetActorsFitYPosToMap( const wxChar* pActorName, bool bFitYPosToMap );	

	bool GetThisActorsInfluenceLightmap( const wxChar* pActorName );
	bool SetActorsInfluenceLightmap( const wxChar* pActorName, bool bInfluenceLightmap );

	bool GetThisActorsScaleLock( const wxChar* pActorName );
	bool SetActorsScaleLock( const wxChar* pActorName, bool bScaleLock );

	const EventInfo* GetEventInfoByID( int iID );
	int GetThisTypesEventNum( int iEventType );
	const EventInfo* GetEventInfoByIndex( int iEventType, int iIndex );

	const EtVector3& GetRegResPos( const wxChar* pResName );
	float GetRegResRot( const wxChar* pResName );
	bool ChangeRegResName( const wxChar* pOriResName, const wxChar* pNewResName );
	bool AddActorsAnimation( const wxChar* pActorName, ActionEleInfo* pAnimationElement );
	bool ClearActorsAnimations( const wxChar* pActorName );
	bool SetRegResPos( const wxChar* pResName, EtVector3& vPos );
	bool SetRegResRot( const wxChar* pResName, float fRot );
	//

	// 툴 제어 관련 메서드
	void RunCommand( ICommand* pCommand );
	void UndoCommand( void );
	void RedoCommand( void );
	void UpdateToThisHistory( int iHistory );
	ICommand* GetLastDidCommand( void );
	CCmdProcessor* GetCmdProcessor( void ) { return m_pCmdProcessor; };

	void ClearToolTempData( void );

	void SetEditMode( int iEditMode ) { m_iEditMode = iEditMode; };
	int GetEditMode( void ) { return m_iEditMode;  };

	//// 리소스 타입을 받아서 이벤트 정보를 생성
	//EventInfo* CreateEventInfo( int iResType );

	bool SaveToFile( const wxChar* pFilePath );
	bool SaveAsToFile( const wxChar* pFilePath );
	bool LoadFromFile( const wxChar* pFilePath );

	bool IsFileOpen( void );
	bool IsFileEdited( void );
	bool IsWritable( void ) { return m_bWritable; };
	void SetEdited( bool bEdited ) { m_bEdited = bEdited; };
	const wxChar* GetOpenedFilePath( void );
	const wxChar* GetOpenedFileName( void );

	// 파일 로드 후 다른 리소스 데이터들에 의존적인 데이터들의 인덱스가 변경되었는지 감지한다.
	void SyncWithDependencyResources();
	void SyncActionIndexWithActFile( void );	// 액션이 삭제되거나 인덱스가 변경된 경우를 감지. 처리해준다.
	void SyncCameraEventActorList( void );		// 현재 로드된 파일과 카메라 이벤트의 액터리스트의 싱크를 맞춰준다.
	void SyncMonsterTableID( void );			// 보스 셋팅 정보를 가져다 쓰는 몬스터 테이블 ID 가 바뀌었는지 확인한다.

	// 클립보드 관련 ///////////////////////////////////////////////////
	void CopyToClipboard( const ActionInfo* pActionInfo );
	void CopyToClipboard( const KeyInfo* pKeyInfo );
	void CopyToClipboard( const EventInfo* pEventInfo );

	const ActionInfo* GetActionClipboard( void ) { return m_pActionInfoClipboard; };
	const KeyInfo* GetKeyClipboard( void ) { return m_pKeyInfoClipboard; };
	const EventInfo* GetEventClipboard( void ) { return m_pEventInfoClipboard; };

	void ResetActionClipboard( void ) { SAFE_DELETE( m_pActionInfoClipboard ); };
	void ResetKeyClipboard( void ) { SAFE_DELETE( m_pKeyInfoClipboard ); };
	void ResetEventClipboard( void ) { SAFE_DELETE( m_pEventInfoClipboard ); };

	bool IsActionCopied( void ) { return m_pActionInfoClipboard ? true : false; };
	bool IsKeyCopied( void ) { return m_pKeyInfoClipboard ? true : false; };
	bool IsEventCopied( void ) { return m_pEventInfoClipboard ? true : false; };
	////////////////////////////////////////////////////////////////////

	// 보스 몬스터 관련 정보를 긁어옴.
	void GatherBossInfoOfThisActor( const wxString& strActorName, /*IN OUT*/ vector<const S_MONSTER_INFO*>& vlResult );
	void GatherBossInfoOfThisActor( int iActorTableIDToFind, /*IN OUT*/ vector<const S_MONSTER_INFO*>& vlResult );

	DNTableFileFormat* GetActorTable( void ) { return m_pActorTable; };
	DNTableFileFormat* GetWeaponTable( void ) { return m_pWeaponTable; };
	DNTableFileFormat* GetMonsterTable( void ) { return m_pMonsterTable; };
	DNTableFileFormat* GetNPCTable( void ) { return m_pNPCTable; };
	DNTableFileFormat* GetFileTable( void ) { return m_pFileTable; };
};


#define TOOL_DATA CToolData::GetInstance()


//// 시퀀스 데이터의 추상화..
//class IEditData
//{
//	IEditData() {};
//	virtual ~IEditData() {};
//};

//class CActionSeqData : public IEditData
//{
//private:
//	ActionInfo* m_pEntity
//};
//
//class CKeySeqSData : public IEditData
//{
//
//};
//
//class CSubKeySeqData : public IEditData
//{
//
//};
//
//class CMaxCameraEventData : public IEditData
//{
//
//};
//
//class CPropEventData : public IEditData
//{
//
//};
//
//class CPartileEventData : public IEditData
//{
//
//};
//
//class CSoundEventData : public IEditData
//{
//
//};
//
//class CFadeEventData : public IEditData
//{
//
//};
//
//class CEditDataHelper
//{
//
//};