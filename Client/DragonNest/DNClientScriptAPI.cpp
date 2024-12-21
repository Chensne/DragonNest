#include "StdAfx.h"
#include "DNClientScriptAPI.h"
#include "DnLuaAPIDefine.h"
#include "DnTableDB.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "EtTriggerElement.h"
#include "DnInterface.h"
#include "DnCommonTask.h"
#include "EtworldEventArea.h"
#include "DnVillageTask.h"
#include "DnWorldGrid.h"
#include "DnWorldSector.h"
#include "DnWorldSound.h"
#include "DnTriggerCameraType1.h"
#include "DnTriggerCameraType2.h"
#include "DnNpcTalkCamera.h"
#include "DnDLGameTask.h"
#include "DnBlindDlg.h"
#include "DnWorldBrokenProp.h"
#include "DnLocalPlayerActor.h"
#include "DnNpcDlg.h"
#include "DnPartsMonsterActor.h"
#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
#include "DnImageBlindDlg.h"
#endif
#include "DnDungeonClearMoveDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

std::map<std::string, int> s_szMapScriptLoadSoundList;

class CDnTriggerWindow : public CEtUICallback
{
public:
	CDnTriggerWindow() {}
	~CDnTriggerWindow() {
		Reset();
	}

	void Reset() {
		m_nMapList.clear();
	};

public:
	std::map<int, std::vector<int>> m_nMapList;

public:

	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 ) 
	{
		if( nCommand != 0 ) return;
		std::map<int, std::vector<int>>::iterator it;
		for( it = m_nMapList.begin(); it != m_nMapList.end(); it++ ) {
			std::vector<int>::iterator it2;
			it2 = std::find( it->second.begin(), it->second.end(), nID );
			if( it2 == it->second.end() ) continue;

			it->second.erase( it2 );
			if( it->second.empty() ) {
				m_nMapList.erase( it );
			}
			break;
		}
	}

	void InsertWindow( int nIndex, int nValue ) {
		std::map<int, std::vector<int>>::iterator it = m_nMapList.find( nIndex ) ;
		if( it == m_nMapList.end() ) {
			std::vector<int> nVecList;
			nVecList.push_back( nValue );
			m_nMapList.insert( make_pair( nIndex, nVecList ) );
		}
		else {
			it->second.push_back( nValue );
		}
	}
	int RemoveWindow( int nIndex )
	{
		std::map<int, std::vector<int>>::iterator it = m_nMapList.find( nIndex ) ;
		if( it == m_nMapList.end() ) return -1;

		if( it->second.empty() ) return -1;

		int nResult = it->second[0];
		it->second.erase( it->second.begin() );

		if( it->second.empty() ) m_nMapList.erase( it );

		return nResult;
	}

};

CDnTriggerWindow g_TriggerTextureWnd;
CDnTriggerWindow g_TriggerAlarmWnd;

void DefAllAPIFunc( lua_State *pLuaState )
{
	DefineLuaAPI(pLuaState);

	LUA_TINKER_DEF(pLuaState, api_trigger_FadeBGM);
	LUA_TINKER_DEF(pLuaState, api_trigger_PlaySound);
	LUA_TINKER_DEF(pLuaState, api_trigger_Delay);
	LUA_TINKER_DEF(pLuaState, api_trigger_OpenTextureDialog);
	LUA_TINKER_DEF(pLuaState, api_trigger_OpenTextureDialogEx);
	LUA_TINKER_DEF(pLuaState, api_trigger_CloseTextureDialog);
	LUA_TINKER_DEF(pLuaState, api_trigger_CloseAllTextureDialog);
	LUA_TINKER_DEF(pLuaState, api_trigger_OpenAlarmDialog);
	LUA_TINKER_DEF(pLuaState, api_trigger_OpenAlarmDialogEx);
	LUA_TINKER_DEF(pLuaState, api_trigger_OpenCountDownAlarmDialogEx);
	LUA_TINKER_DEF(pLuaState, api_trigger_CloseAlarmDialog);
	LUA_TINKER_DEF(pLuaState, api_trigger_CloseAllAlarmDialog);
	LUA_TINKER_DEF(pLuaState, api_trigger_BeginCountDown);
	LUA_TINKER_DEF(pLuaState, api_trigger_BeginCountDownEx);
	LUA_TINKER_DEF(pLuaState, api_trigger_EndCountDown);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableTriggerElement);
	LUA_TINKER_DEF(pLuaState, api_trigger_PlayCutSceneByTrigger);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowNotifyArrowByEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowNotifyArrowByProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_HideNotifyArrow);
	LUA_TINKER_DEF(pLuaState, api_trigger_ChangeBGM);
	LUA_TINKER_DEF(pLuaState, api_trigger_ChangeEnviVolume);
	LUA_TINKER_DEF(pLuaState, api_trigger_CameraWork1);
	LUA_TINKER_DEF(pLuaState, api_trigger_CameraWork2);
	LUA_TINKER_DEF(pLuaState, api_trigger_CameraWork3);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLChangeRound);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLGetRound);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLGetTotalRound);
	LUA_TINKER_DEF(pLuaState, api_trigger_OpenBlind);
	LUA_TINKER_DEF(pLuaState, api_trigger_CloseBlind);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowBlindCaption);
	LUA_TINKER_DEF(pLuaState, api_trigger_AddCameraEffect_Quake);
	LUA_TINKER_DEF(pLuaState, api_trigger_AddCameraEffect_DepthOfField);
	LUA_TINKER_DEF(pLuaState, api_trigger_AddCameraEffect_RadialBlur);
	LUA_TINKER_DEF(pLuaState, api_trigger_ChangeEnvironment);
	LUA_TINKER_DEF(pLuaState, api_trigger_InitializeMODDialog);
	LUA_TINKER_DEF(pLuaState, api_trigger_LinkMODValue);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowMODDialog);

	LUA_TINKER_DEF(pLuaState, api_trigger_GetMonsterAreaLiveCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetMonsterAreaTotalCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetMonsterActorFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowBossAlertDialog);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetPropDurability);
	LUA_TINKER_DEF(pLuaState, api_trigger_MyObseverInEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_MyObseverEventAreaMove);
	LUA_TINKER_DEF(pLuaState, api_trigger_BattleBGMToggle);

	LUA_TINKER_DEF(pLuaState, api_trigger_TriggerVariableTable);

	LUA_TINKER_DEF(pLuaState, api_trigger_HurryUpTime);
	LUA_TINKER_DEF(pLuaState, api_trigger_DungeonClear_AddReward);
#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
	LUA_TINKER_DEF(pLuaState, api_trigger_LinkMODPartsValue);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetMonsterParts);
#endif 
#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
	LUA_TINKER_DEF(pLuaState, api_trigger_OpenImageBlind);
	LUA_TINKER_DEF(pLuaState, api_trigger_CloseImageBlind);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowCationImageBlind);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowNPCImageBlind);
#endif // PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
	LUA_TINKER_DEF(pLuaState, api_trigger_DungeonSynchroDialog);
#ifdef PRE_ADD_PART_SWAP_RESTRICT_TRIGGER
	LUA_TINKER_DEF(pLuaState, api_trigger_PartSwapRestrict);
#endif
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowAlteaBoard);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowAlteaClear);
	LUA_TINKER_DEF(pLuaState, api_trigger_DisableReturnVillage);
#ifdef PRE_ADD_FADE_TRIGGER
	LUA_TINKER_DEF(pLuaState, api_trigger_FadeScreen);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowFadeScreenCaption);
#endif // PRE_ADD_FADE_TRIGGER
}


void ClearScriptLoadResource()
{
	for( std::map<std::string, int>::iterator it = s_szMapScriptLoadSoundList.begin(); it != s_szMapScriptLoadSoundList.end(); it++ ) {
		CEtSoundEngine::GetInstance().RemoveSound( it->second );
	}
	SAFE_DELETE_VEC( s_szMapScriptLoadSoundList );
	g_TriggerTextureWnd.Reset();
	g_TriggerAlarmWnd.Reset();
}

void api_trigger_FadeBGM( float fTargetVolume, float fDelta )
{
	CEtSoundEngine::GetInstance().FadeVolume( "BGM", fTargetVolume, fDelta, false );
}

void api_trigger_ChangeEnviVolume( float fVolume )
{
	CDnWorldEnvironment *pEnvi = CDnWorld::GetInstance().GetEnvironment();
	if( !pEnvi ) return;
	pEnvi->SetEnviBGMRatio( fVolume );

	CDnWorldSector *pSector;
	float fTargetVolume = 1.f;
	for( DWORD i=0; i<CDnWorld::GetInstance().GetGrid()->GetActiveSectorCount(); i++ ) {
		pSector = (CDnWorldSector*)CDnWorld::GetInstance().GetGrid()->GetActiveSector(i);
		pSector->GetSoundInfo()->SetVolume( fTargetVolume * ( 1.f - fVolume ) );
	}
}

void api_trigger_PlaySound( int nFileIndex )
{
	const char *szFileName = CDnTableDB::GetInstance().GetFileName( nFileIndex );
	if( szFileName == NULL ) return;

	std::map<std::string, int>::iterator it = s_szMapScriptLoadSoundList.find( szFileName );

	if( it != s_szMapScriptLoadSoundList.end () ) {
		CEtSoundEngine::GetInstance().PlaySound( "2D", it->second );
	}
	else {
		int nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
		if( nSoundIndex == -1 ) return;
		s_szMapScriptLoadSoundList.insert( make_pair( szFileName, nSoundIndex ) );

		CEtSoundEngine::GetInstance().PlaySound( "2D", nSoundIndex );
	}	
}

bool api_trigger_Delay( void *pRoom, CEtTriggerElement *pElement, int nDelay )
{
	if( !pElement ) return true;

	CDnGameTask *pTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pTask ) return true;
	if( !pTask->IsSyncComplete() ) return false;

	if( (DWORD)pTask->GetLocalTime() - pElement->GetLastExcuteTime() < nDelay ) return false;

	return true;
}

void api_trigger_OpenTextureDialog( int nFileIndex, float fX, float fY, int nTime, int nDialogIndex )
{
	const char *szFileName = CDnTableDB::GetInstance().GetFileName( nFileIndex );
	if( szFileName == NULL ) return;

	if( nTime <= 0 ) nTime = INT_MAX;
	EtTextureHandle hTexture = EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
	int nResult = GetInterface().DrawTextureWindow( hTexture, fX, fY, (float)nTime, nDialogIndex, &g_TriggerTextureWnd );
	if( nDialogIndex == 0 ) {
		g_TriggerTextureWnd.InsertWindow( nFileIndex, nResult );
	}
}

void api_trigger_OpenTextureDialogEx( int nFileIndex, float fX, float fY, int nPos, int nTime, int nDialogIndex )
{
	const char *szFileName = CDnTableDB::GetInstance().GetFileName( nFileIndex );
	if( szFileName == NULL ) return;

	if( nTime <= 0 ) nTime = INT_MAX;
	EtTextureHandle hTexture = EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
	int nResult = GetInterface().DrawTextureWindow( hTexture, fX, fY, nPos, (float)nTime, nDialogIndex, &g_TriggerTextureWnd );
	if( nDialogIndex == 0 ) {
		g_TriggerTextureWnd.InsertWindow( nFileIndex, nResult );
	}
}

void api_trigger_CloseTextureDialog( bool bIsDialogIndex, int nIndex )
{
	int nResult = -1;

	if( bIsDialogIndex ) {
		nResult = nIndex;
	}
	else {
		nResult = g_TriggerTextureWnd.RemoveWindow( nIndex );
	}
	if( nResult == -1 ) return;
	GetInterface().CloseTextureWindow( nResult, bIsDialogIndex );
}

void api_trigger_CloseAllTextureDialog()
{
	GetInterface().ClearTextureWindow();
	g_TriggerTextureWnd.Reset();
}

void api_trigger_OpenAlarmDialog( int nPanelIndex, float fX, float fY, float fWidth, int nTime, int nUIStringIndex, int nDialogIndex )
{
	if( nTime <= 0 ) nTime = INT_MAX;

	wstring szStr = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex );

	int nResult = GetInterface().ShowAlarmWindow( nPanelIndex, fX, fY, fWidth, (float)nTime, szStr.c_str(), nDialogIndex, &g_TriggerAlarmWnd );
	if( nDialogIndex == 0 ) {
		g_TriggerAlarmWnd.InsertWindow( nUIStringIndex, nResult );
	}
}

void api_trigger_OpenAlarmDialogEx( int nPanelIndex, float fX, float fY, int nPos, int nFormat, int nTime, int nUIStringIndex, int nDialogIndex )
{
	if( nTime <= 0 ) nTime = INT_MAX;

	wstring szStr = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex );

	int nResult = GetInterface().ShowAlarmWindow( nPanelIndex, fX, fY, nPos, nFormat, (float)nTime, szStr.c_str(), nDialogIndex, &g_TriggerAlarmWnd );
	if( nDialogIndex == 0 ) {
		g_TriggerAlarmWnd.InsertWindow( nUIStringIndex, nResult );
	}
}

void api_trigger_OpenCountDownAlarmDialogEx( int nPanelIndex, float fX, float fY, int nPos, int nFormat, int nTime, int nCountDown, int nDialogIndex )
{
	if( nTime <= 0 ) nTime = INT_MAX;

	int nResult = GetInterface().ShowCountDownAlarmWindow( nPanelIndex, fX, fY, nPos, nFormat, (float)nTime, nCountDown, nDialogIndex, &g_TriggerAlarmWnd );
	if( nDialogIndex == 0 ) {
		g_TriggerAlarmWnd.InsertWindow( nCountDown, nResult );
	}
}

void api_trigger_CloseAlarmDialog( bool bIsDialogIndex, int nIndex )
{
	int nResult = -1;

	if( bIsDialogIndex ) {
		nResult = nIndex;
	}
	else {
		nResult = g_TriggerAlarmWnd.RemoveWindow( nIndex );
	}
	if( nResult == -1 ) return;
	GetInterface().CloseAlarmWindow( nResult, bIsDialogIndex );
}

void api_trigger_CloseAllAlarmDialog()
{
	GetInterface().ClearAlarmWindow();
	g_TriggerAlarmWnd.Reset();
}

void api_trigger_BeginCountDown( int nSec, int nUIStringIndex )
{
	wstring szStr = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex );
	GetInterface().BeginCountDown( nSec, szStr.c_str() );
}

void api_trigger_BeginCountDownEx( int nSec, int nUIStringIndex, int type )
{
	wstring szStr = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex );
	GetInterface().BeginCountDown( nSec, szStr.c_str(), (CDnInterface::emCountDownType)type );
}

void api_trigger_EndCountDown()
{
	GetInterface().EndCountDown();
}

void api_trigger_EnableTriggerElement( void *pRoom, CEtTriggerElement *pElement, bool bEnable )
{
	if( !pElement ) return;
	pElement->SetEnable( bEnable );
}

void api_trigger_PlayCutSceneByTrigger( void *pRoom, int nCutSceneTableID, bool bFadeIn )
{
	CDnCommonTask *pCommonTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
	if( !pCommonTask ) return;

	if( !pCommonTask->IsPlayedCutScene( nCutSceneTableID ) )
		CDnWorld::GetInstance().EnableTriggerEventCallback( false );
}

void api_trigger_ShowNotifyArrowByEventArea( int nEventAreaHandle, int nDelay )
{
	std::vector<CEtWorldEventArea *> pVecList;
	if( CDnWorld::GetInstance().FindEventAreaFromCreateUniqueID( nEventAreaHandle, &pVecList ) == 0 ) return;

	EtVector3 vPos = pVecList[0]->GetOBB()->Center;
	GetInterface().OpenNotifyArrowDialog( vPos, nDelay * 1000, true );
}

void api_trigger_ShowNotifyArrowByProp( int nPropHandle, int nDelay )
{
	std::vector<CEtWorldProp *> pVecList;
	CDnWorld::GetInstance().FindPropFromCreateUniqueID( nPropHandle, &pVecList );
	if( pVecList.empty() ) return;

	CDnWorldProp *pProp = (CDnWorldProp *)pVecList[0];
	EtVector3 vPos = pProp->GetMatEx()->m_vPosition;
	GetInterface().OpenNotifyArrowDialog( vPos, nDelay * 1000, true );
}

void api_trigger_HideNotifyArrow()
{
	GetInterface().CloseNotifyArrowDialog();
}


void api_trigger_ChangeBGM( int nFileIndex )
{
	CDnWorldGrid *pGrid = (CDnWorldGrid *)CDnWorld::GetInstance().GetGrid();
	if( !pGrid ) return;
	const char *szFileName = CDnTableDB::GetInstance().GetFileName( nFileIndex );
	if( szFileName == NULL ) return;

	for( DWORD i=0; i<pGrid->GetActiveSectorCount(); i++ ) {
		CDnWorldSector *pSector = (CDnWorldSector *)pGrid->GetActiveSector(i);
		if( !pSector ) continue;
		CDnWorldSound *pSound = (CDnWorldSound *)pSector->GetSoundInfo();
		if( !pSound ) continue;
		pSound->ChangeBGM( szFileName );
	}
}

DnCameraHandle CheckTriggerCameraSource()
{
	DnCameraHandle hActiveCamera = CDnCamera::GetActiveCamera();
	DnCameraHandle hSourceCamera = CDnCamera::GetActiveCamera();
	if( hActiveCamera ) {
		switch( hActiveCamera->GetCameraType() ) {
			case CDnCamera::TriggerControl1Camera:
				hActiveCamera->SetDestroy();
				hSourceCamera = ((CDnTriggerCameraType1*)hActiveCamera.GetPointer())->GetSourceCamera();
				break;
			case CDnCamera::TriggerControl2Camera:
				hActiveCamera->SetDestroy();
				hSourceCamera = ((CDnTriggerCameraType2*)hActiveCamera.GetPointer())->GetSourceCamera();
				break;
			case CDnCamera::NpcTalkCamera:
				{
					hActiveCamera->SetDestroy();
					hSourceCamera = ((CDnNpcTalkCamera*)hActiveCamera.GetPointer())->GetSourceCamera();
					CDnLocalPlayerActor::LockInput( false );
//					if( GetInterface().GetNpcDialog()->IsShow() ) {
						GetInterface().CloseNpcDialog();
						CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
						if( pTask ) pTask->EndNpcTalk();

						// #54501 퀘스트 진행 중 컷신 출력된 후, 파티원의 모습이 보이지 않음.
						GetInterface().GetBlindDialog()->CallBackClosed();
	
						GetInterface().GetBlindDialog()->RemoveBlindCallback( GetInterface().GetNpcDialog() );
//					}
				}
				break;
		}
	}
	return hSourceCamera;
}

bool IsPlayingTriggerCamera()
{
	if( GetInterface().IsOpenBlind() ) {
		DnCameraHandle hActiveCamera = CDnCamera::GetActiveCamera();
		if( hActiveCamera ) {
			switch( hActiveCamera->GetCameraType() ) {
			case CDnCamera::TriggerControl1Camera:
			case CDnCamera::TriggerControl2Camera:
				return true;
			}
		}
	}
	return false;
}

void api_trigger_CameraWork1( int nStartPropHandle, int nEndPropHandle, int nDelay, bool bStartVel, bool bEndVel )
{
	CDnWorldGrid *pGrid = (CDnWorldGrid *)CDnWorld::GetInstance().GetGrid();
	if( !pGrid ) return;
	MatrixEx *pCross[2];

	std::vector<CEtWorldProp *> VecResult;
	pGrid->FindPropFromCreateUniqueID( nStartPropHandle, &VecResult );
	if( VecResult.empty() ) return;
	pCross[0] = ((CDnWorldProp*)VecResult[0])->GetMatEx();
	VecResult.clear();

	pGrid->FindPropFromCreateUniqueID( nEndPropHandle, &VecResult );
	if( VecResult.empty() ) return;
	pCross[1] = ((CDnWorldProp*)VecResult[0])->GetMatEx();

	DnCameraHandle hSourceCamera = CheckTriggerCameraSource();
	CDnTriggerCameraType1 *pCamera = new CDnTriggerCameraType1( hSourceCamera, *pCross[0], *pCross[1], nDelay, bStartVel, bEndVel );
	CDnCamera::SetActiveCamera( pCamera->GetMySmartPtr() );

	// 카메라 워크 시작할때 로컬 플레이어 Stand 로 만들어주도록 하자.
	if( CDnActor::s_hLocalActor ) {
		if( CDnActor::s_hLocalActor->IsMove() /*&& CDnActor::s_hLocalActor->IsMovable()*/ )			// #19791 이슈관련 대쉬 중 카메라웍 실행시 계속 진행되는 문제 수정. -한기
			CDnActor::s_hLocalActor->CmdStop( "Stand", 0, 3.f, 0.f, false, true );
	}
}

void api_trigger_CameraWork2( int nWorkPropHandle, int nTargetPropHandle, int nTotalAngle, int nDelay, bool bStartVel, bool bEndVel )
{
	CDnWorldGrid *pGrid = (CDnWorldGrid *)CDnWorld::GetInstance().GetGrid();
	if( !pGrid ) return;
	MatrixEx *pCross[2];

	std::vector<CEtWorldProp *> VecResult;
	pGrid->FindPropFromCreateUniqueID( nWorkPropHandle, &VecResult );
	if( VecResult.empty() ) return;
	pCross[0] = ((CDnWorldProp*)VecResult[0])->GetMatEx();
	VecResult.clear();

	pGrid->FindPropFromCreateUniqueID( nTargetPropHandle, &VecResult );
	if( VecResult.empty() ) return;
	pCross[1] = ((CDnWorldProp*)VecResult[0])->GetMatEx();

	DnCameraHandle hSourceCamera = CheckTriggerCameraSource();
	CDnTriggerCameraType2 *pCamera = new CDnTriggerCameraType2( hSourceCamera, *pCross[0], *pCross[1], nDelay, nTotalAngle, bStartVel, bEndVel );
	CDnCamera::SetActiveCamera( pCamera->GetMySmartPtr() );

	// 카메라 워크 시작할때 로컬 플레이어 Stand 로 만들어주도록 하자.
	if( CDnActor::s_hLocalActor ) {
		if( CDnActor::s_hLocalActor->IsMove() /*&& CDnActor::s_hLocalActor->IsMovable()*/ )			// #19791 이슈관련 대쉬 중 카메라웍 실행시 계속 진행되는 문제 수정. -한기
			CDnActor::s_hLocalActor->CmdStop( "Stand" );
	}
}

void api_trigger_CameraWork3( int nTargetPropHandle, int nDelay, bool bStartVel, bool bEndVel )
{
	CDnWorldGrid *pGrid = (CDnWorldGrid *)CDnWorld::GetInstance().GetGrid();
	if( !pGrid ) return;
	MatrixEx *pMatEx[2];

	DnCameraHandle hCamera = CDnCamera::FindCamera( CDnCamera::PlayerCamera );
	if( !hCamera ) return;
	pMatEx[0] = hCamera->GetMatEx();
	std::vector<CEtWorldProp *> VecResult;
	pGrid->FindPropFromCreateUniqueID( nTargetPropHandle, &VecResult );
	if( VecResult.empty() ) return;
	pMatEx[1] = ((CDnWorldProp*)VecResult[0])->GetMatEx();

	DnCameraHandle hSourceCamera = CheckTriggerCameraSource();
	CDnTriggerCameraType1 *pCamera = new CDnTriggerCameraType1( hSourceCamera, *pMatEx[0], *pMatEx[1], nDelay, bStartVel, bEndVel );
	CDnCamera::SetActiveCamera( pCamera->GetMySmartPtr() );

	// 카메라 워크 시작할때 로컬 플레이어 Stand 로 만들어주도록 하자.
	if( CDnActor::s_hLocalActor ) {
		if( CDnActor::s_hLocalActor->IsMove() /*&& CDnActor::s_hLocalActor->IsMovable()*/ )			// #19791 이슈관련 대쉬 중 카메라웍 실행시 계속 진행되는 문제 수정. -한기
			CDnActor::s_hLocalActor->CmdStop( "Stand" );
	}
}

void api_trigger_AddCameraEffect_Quake( int nDelay, float fBeginRatio, float fEndRatio, float fValue )
{
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( !hCamera ) return;

	hCamera->Quake( nDelay, fBeginRatio, fEndRatio, fValue );
}

void api_trigger_AddCameraEffect_DepthOfField( int nDelay, float fFocusDist, float fNearStart, float fNearEnd, float fFarStart, float fFarEnd, float fNearBlur, float fFarBlur )
{
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( !hCamera ) return;

	hCamera->DepthOfField( nDelay, fFocusDist, fNearStart, fNearEnd, fFarStart, fFarEnd, fNearBlur, fFarBlur );
}

void api_trigger_AddCameraEffect_RadialBlur( int nDelay, float fBlurCenterX, float fBlurCenterY, float fBlurSize, float fBeginRatio, float fEndRatio )
{
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( !hCamera ) return;

	hCamera->RadialBlur( nDelay, EtVector2( fBlurCenterX, fBlurCenterY ), fBlurSize, fBeginRatio, fEndRatio );
}

void api_trigger_DLChangeRound( void *pRoom, bool bBoss )
{
	CDnGameTask *pTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pTask ) return;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) return;

	CDnDLGameTask *pDLTask = (CDnDLGameTask *)pTask;
	pDLTask->UpdateRound( bBoss );
}

int api_trigger_DLGetRound( void *pRoom )
{
	CDnGameTask *pTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pTask ) return 0;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) return 0;

	return ((CDnDLGameTask *)pTask)->GetRound();
}

int api_trigger_DLGetTotalRound( void *pRoom )
{
	CDnGameTask *pTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pTask ) return 0;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) return 0;

	return ((CDnDLGameTask *)pTask)->GetTotalRound();
}

void api_trigger_OpenBlind()
{
	GetInterface().OpenBlind();
}

void api_trigger_CloseBlind()
{
	GetInterface().CloseBlind( true );
}

void api_trigger_ShowBlindCaption( int nUIStringIndex, int nDelay )
{
	CDnBlindDlg *pDlg = GetInterface().GetBlindDialog();
	if( !pDlg ) return;
	if( pDlg->GetBlindMode() > CDnBlindDlg::modeOpened ) return;

	pDlg->SetCaption( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), nDelay );
}

#ifdef PRE_ADD_FADE_TRIGGER
void api_trigger_FadeScreen( int nSourceAlpha, int nSourceRed, int nSourceGreen, int nSourceBlue,
							int nDestAlpha, int nDestRed, int nDestGreen, int nDestBlue, float fDelta )
{
	GetInterface().FadeDialog( D3DCOLOR_ARGB( nSourceAlpha, nSourceRed, nSourceGreen, nSourceBlue ), 
							D3DCOLOR_ARGB( nDestAlpha, nDestRed, nDestGreen, nDestBlue ), fDelta / 1000.0f );

	bool bFadeOut = false;
	if( nSourceAlpha < nDestAlpha )
		bFadeOut = true;

	CDnLocalPlayerActor* pLocalActor = dynamic_cast<CDnLocalPlayerActor*>( CDnLocalPlayerActor::s_hLocalActor.GetPointer() );
	if( pLocalActor )
	{
		if( bFadeOut )
			pLocalActor->LockInput( true );
		else
			pLocalActor->LockInput( false );
	}
}

void api_trigger_ShowFadeScreenCaption( int nUIStringIndex, int nDelay )
{
	GetInterface().ShowFadeScreenCaption( nUIStringIndex, nDelay );
}
#else // PRE_ADD_FADE_TRIGGER
void api_trigger_FadeScreen( DWORD dwTargetColor, float fDelta )
{
	GetInterface().FadeDialog( 0, dwTargetColor, fDelta );
}
#endif // PRE_ADD_FADE_TRIGGER

void api_trigger_ChangeEnvironment( const char *szEnviFileName )
{
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pGameTask ) return;
	DnCameraHandle hCamera = pGameTask->GetPlayerCamera();
	if( !hCamera ) return;

	CFileNameString szFileName = szEnviFileName;
	char szExt[32] = { 0, };
	_GetExt( szExt, _countof(szExt), szEnviFileName );
	if( strlen(szExt) == 0 ) szFileName += ".env";

	CDnWorld::GetInstance().InitializeEnviroment( szFileName.c_str(), hCamera );
}

void api_trigger_InitializeMODDialog( int nMODTableID )
{
	GetInterface().InitializeMODDialog( nMODTableID );
}

void api_trigger_LinkMODValue( CEtTriggerElement *pElement, int nIndex, int nDefineValueIndex )
{
	GetInterface().SetLinkMODValue( pElement, nIndex, nDefineValueIndex );
}

void api_trigger_ShowMODDialog( bool bShow )
{
	GetInterface().ShowMODDialog( bShow );
}

int api_trigger_GetMonsterAreaLiveCount( void *pRoom, int nEventAreaHandle )
{
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pTask ) return 0;

	return pTask->GetBirthAreaLiveCount( nEventAreaHandle );
}

int api_trigger_GetMonsterAreaTotalCount( void *pRoom, int nEventAreaHandle )
{
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pTask ) return 0;

	return pTask->GetBirthAreaTotalCount( nEventAreaHandle );
}


int api_trigger_GetMonsterActorFromEventArea( void *pRoom, int nEventAreaHandle, int nIndex )
{
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pTask ) return 0;

	DNVector(DnActorHandle) hVecResult;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecResult );

	if( nIndex < 0 || nIndex >= (int)hVecResult.size() ) return 0;
	return hVecResult[nIndex]->GetUniqueID();
}

void api_trigger_ShowBossAlertDialog( bool bShow, int nUIStringIndex, int nFileIndex )
{
	WCHAR *wszName = ( nUIStringIndex ) ? (WCHAR*)GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ) : NULL;
	const char *szFileName = ( nFileIndex ) ? CDnTableDB::GetInstance().GetFileName( nFileIndex ) : NULL;
	GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex );

	if( szFileName == NULL ) return;
	GetInterface().ShowBossAlertDialog( bShow, wszName, szFileName );

}

int api_trigger_GetPropDurability( void *pRoom, int nPropHandle )
{
	std::vector<CEtWorldProp *> pVecList;
	CDnWorld::GetInstance().FindPropFromCreateUniqueID( nPropHandle, &pVecList );
	if( pVecList.empty() ) return 0;

	CDnWorldProp *pProp = static_cast<CDnWorldProp *>( pVecList[0] );
	if( !pProp ) return 0;

	int nPropType = pProp->GetPropType();
	switch( nPropType ) {
		case PTE_Broken:
		case PTE_BrokenDamage:
		case PTE_BuffBroken:
		case PTE_HitMoveDamageBroken:
		case PTE_ShooterBroken:
			return ((CDnWorldBrokenProp*)pProp)->GetDurability();
		default:
			break;
	}
	return 0;
}

bool api_trigger_MyObseverInEventArea( int nSrcEventAreaHandle )
{
	// 자신이 옵져버일 경우 nSrcEventAreaHandle Area에 들어왔을 경우 TRUE

	if( !CDnActor::s_hLocalActor ) return false;
	CDnLocalPlayerActor *pLocalActor = dynamic_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
	if( !pLocalActor || !pLocalActor->IsObserver() ) return false;

	std::vector<CEtWorldEventArea *> pSrcVecList;
	if( CDnWorld::GetInstance().FindEventAreaFromCreateUniqueID( nSrcEventAreaHandle, &pSrcVecList ) == 0 ) return false;
	if( pSrcVecList.empty() ) return false;

	if( pSrcVecList[0]->CheckArea( *pLocalActor->GetPosition(), true ) )
		return true;

	return false;
}

void api_trigger_MyObseverEventAreaMove( int nDestEventAreaHandle )
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor *pLocalActor = dynamic_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
	if( !pLocalActor || !pLocalActor->IsObserver() ) return;

	std::vector<CEtWorldEventArea *> pDestVecList;
	if( CDnWorld::GetInstance().FindEventAreaFromCreateUniqueID( nDestEventAreaHandle, &pDestVecList ) == 0 ) return;
	if( pDestVecList.empty() ) return;

	pLocalActor->SetPosition( pDestVecList[0]->GetOBB()->Center );
}

void api_trigger_BattleBGMToggle( bool bToggle )
{
	CDnWorld::GetInstance().SetProcessBattleBGM( bToggle );
}

int  api_trigger_TriggerVariableTable( void * pRoom, int nItemID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TTRIGGERVARIABLETABLE );

	if( !pSox ) return -1;

	return pSox->GetFieldFromLablePtr( nItemID, "_Value" )->GetInteger();
}

void api_trigger_HurryUpTime( int nTime )
{
	GetInterface().SetHardCoreModeHurryUpTime( nTime );
}

void api_trigger_DungeonClear_AddReward( int nPropHandle )
{
#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )
	GetInterface().OpenDungeonExtraRewardDlg( nPropHandle );
#endif	// PRE_ADD_STAGE_CLEAR_ADD_REWARD
}

#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
void api_trigger_LinkMODPartsValue( CEtTriggerElement *pElement, int nIndex, int nDefineValueIndex, int nGaugeType, int nPartsIndex )
{
	GetInterface().SetLinkMODValue( pElement, nIndex, nDefineValueIndex, nGaugeType, nPartsIndex);
}

int api_trigger_GetMonsterParts( void *pRoom, int nEventAreaHandle, int nIndex, int nPartsID )
{
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pTask ) return 0;

	DNVector(DnActorHandle) hVecResult;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecResult );

	if( nIndex < 0 || nIndex >= (int)hVecResult.size() ) return 0;
	DnActorHandle pActor = hVecResult[nIndex];

	if( pActor )
	{
		if( pActor->IsPartsMonsterActor() )
		{
			CDnPartsMonsterActor* pPartsMonster = static_cast<CDnPartsMonsterActor*>( pActor.GetPointer() );	
			if( pPartsMonster )
			{
				return pPartsMonster->GetPartsIndexFromPartsID( nPartsID ) ;
			}
		}
	}

	return 0;
}
#endif 

#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
void api_trigger_OpenImageBlind()
{
	GetInterface().OpenImageBlind();
}

void api_trigger_CloseImageBlind()
{
	GetInterface().CloseImageBlind(true);
}

void api_trigger_ShowCationImageBlind(int nUIStringIndex, int nDelay)
{
	CDnImageBlindDlg* pDlg = GetInterface().GetImageBlindDialog();
	if(!pDlg) return;
	
	std::wstring wszString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex );

	if(!wszString.empty())		
		pDlg->SetCaption(wszString.c_str(), nDelay);	
}

void api_trigger_ShowNPCImageBlind(bool bShow, int nUIStringIndex, int nFileIndex, int nImageType)
{
	CDnImageBlindDlg* pDlg = GetInterface().GetImageBlindDialog();
	if(!pDlg) return;

	WCHAR*		wszName		= ( nUIStringIndex ) ? (WCHAR*)GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ) : NULL;
	const char*	szFileName  = ( nFileIndex ) ? CDnTableDB::GetInstance().GetFileName( nFileIndex ) : NULL;
	
	pDlg->SetNPCImage(bShow, wszName, szFileName, nImageType);
}
#endif // PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER

void api_trigger_DungeonSynchroDialog( bool bShow, bool bSynchro )
{
	GetInterface().ShowDungeonSynchroDialog( bShow, bSynchro );
}

#ifdef PRE_ADD_PART_SWAP_RESTRICT_TRIGGER
void api_trigger_PartSwapRestrict(bool bSwitch)
{
	GetInterface().SetPartSwapRestrict(bSwitch);
}
#endif

void api_trigger_ShowAlteaBoard()
{
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	GetInterface().Show_AlteaBoard( true );
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )
}

void api_trigger_ShowAlteaClear( int nClear )
{
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	GetInterface().ShowAlteaClearDlg( nClear == 1 ? true : false );
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )
}

void api_trigger_DisableReturnVillage()
{
	CDnDungeonClearMoveDlg* pDungeonClearMoveDlg = GetInterface().GetDungeonMoveDlg();

	if( NULL == pDungeonClearMoveDlg )
		return;

	pDungeonClearMoveDlg->TriggeredByUser_DisableWorld();
}