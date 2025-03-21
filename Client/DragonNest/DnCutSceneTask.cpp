#include "StdAfx.h"

#include "DnWorld.h"
#include "DnCutSceneTask.h"
#include "DnCutSceneData.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnGameTask.h"
#include "DnActor.h"
#include "DnCamera.h"
#include "EternityEngine.h"
#include "DnCutSceneActorProcessor.h"
#include "DnTableDB.h"
#include "DnActorClassDefine.h"
#include "DnPlayerCamera.h"
#include "DnCutSceneData.h"
#include "DnInterface.h"
#include "DnWorldSector.h"
#include "DnPlayerActor.h"
#include "DnCEDof.h"
#include "DnWorldEnvironment.h"
#include "DnQuestTask.h"
#include "DnLocalPlayerActor.h"
#include "GameSendPacket.h"
#include "DnCommonTask.h"
#include "DnFadeInOutDlg.h"
#include "DnMonsterActor.h"
#include "DnCharStatusDlg.h"
#ifdef PRE_ADD_CUTSCENE_PADSKIP
#include "InputWrapper.h"
#endif

//#include "SmartPtr.h"
//#include ""


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 



CDnCutSceneTask::CDnCutSceneTask(void) : m_pCutSceneData( new CDnCutSceneData ), CInputReceiver( true ),
#if defined(TEST_CUTSCENE)
										 m_bFirstLoop( true ),
#endif
										 m_pWorld( NULL ),
										 m_pActorProcessor(	NULL ),
										 m_pDOFFilter( NULL ),
										 m_fNearStart( 0.0f ),
										 m_fNearEnd( 0.0f ),
										 m_fFarStart( 0.0f ),
										 m_fFarEnd( 0.0f ),
										 m_fFocusDist( 0.0f ),
										 m_fNearBlurSize( 0.0f ),
										 m_fFarBlurSize( 0.0f ),
										 m_bSetFadeModeCalledForSkip( false ),
										 m_bCheatCutScene( false ),
										m_pSkipFade( NULL )
{
	m_bRequestSkip = false;
	m_bAutoFadeIn = true;
	m_bLoadingComplete = false;
}

CDnCutSceneTask::~CDnCutSceneTask(void)
{
        // #60295
	m_arrCharacterName.clear();
	m_vecPartsInfo.clear();

	if( CTaskManager::IsActive() ) {
		CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
		if( pTask ) {
			pTask->EndCutScene();
		}
	}
	SAFE_DELETE( m_pSkipFade );
	SAFE_DELETE( m_pCutSceneData );
	SAFE_DELETE( m_pActorProcessor );
}

bool CDnCutSceneTask::Initialize( const char* pFileName, int nQuestIndex /*= -1*/, int nQuestStep /*= -1 */, 
								  bool bAutoFadeIn/* = true*/, DWORD dwQuestPlayerUniqueID/* = UINT_MAX*/ )
{
	// #60295
	m_arrCharacterName.push_back( "WARRIOR" );
	m_arrCharacterName.push_back( "ARCHER" );
	m_arrCharacterName.push_back( "SOCERESS" );
	m_arrCharacterName.push_back( "CLERIC" );
#ifdef PRE_ADD_ACADEMIC
	m_arrCharacterName.push_back( "ACADEMIC" );
#endif
#ifdef PRE_ADD_KALI
	m_arrCharacterName.push_back( "KALI" );
#endif
//[OK]
#ifdef PRE_ADD_ASSASSIN
	m_arrCharacterName.push_back( "ASSASSIN" );
#endif
#ifdef PRE_ADD_LENCEA
	m_arrCharacterName.push_back( "LENCEA" );
#endif
#ifdef PRE_ADD_MACHINA
	m_arrCharacterName.push_back( "MACHINA" );
#endif
	bool bResult = false;

	m_bLoadingComplete = false;
	if( CDnLoadingTask::IsActive() ) {
		CDnLoadingTask::GetInstance().EnableBackgroundLoading( false );
		CDnLoadingTask::GetInstance().Initialize( this, 0 );
	}

	if( strlen(pFileName) == 0 )
		return bResult;

	//bool bSuccess = m_pCutSceneData->LoadFromFile( pFileName );
	CStream* pStream = CEtResourceMng::GetInstance().GetStream( pFileName );
	_ASSERT( pStream && "컷신 파일 스트림 로드 실패!" );
	if( NULL == pStream )
		return bResult;

	int iSize = pStream->Size();
	char* pBuffer = new char[iSize];
	ZeroMemory( pBuffer, iSize );
	pStream->ReadBuffer( (void*)pBuffer, iSize );

	// ReadBuffer 호출 후에 끝에 메모리가 좀 더 달려 나와서 제거해 줌...
	int i = 0;
	for( i = iSize-1; i > 0; --i )
	{
		if( '}' == pBuffer[ i ] && i < iSize-2 )
		{
			pBuffer[ i+1 ] = NULL;
			break;
		}
	}

	_ASSERT( 0 < i && "잘못된 컷신 파일입니다." );
	
	bool bSuccess = m_pCutSceneData->LoadFromStringBuffer( pBuffer );
	_ASSERT( bSuccess && "컷신 파일 로드 실패!" );

	delete [] pBuffer;

	SAFE_DELETE( pStream );

	// 퀘스트 번호가 있는 경우와 없는 경우로 나뉜다. 제대로 들어왔는지 확인.
	bool bValidQuestInfo = !((-1 == nQuestIndex && -1 != nQuestStep) || (-1 != nQuestIndex && -1 == nQuestStep));
	_ASSERT( bValidQuestInfo && "퀘스트 번호는 있으나 스텝이 입력이 없거나 스텝은 있으나 번호가 없습니다!" );
	if( false == bValidQuestInfo )
		return bResult;

	m_bAutoFadeIn = bAutoFadeIn;
	m_pWorld = CDnWorld::GetInstancePtr();
	bool bVillage = (CDnWorld::MapTypeEnum::MapTypeVillage == m_pWorld->GetMapType());

	if( bSuccess )
	{
		m_CutScenePlayer.SetWorld( m_pWorld );

		// 부활하자마자 컷신 나올 수 있다. 따라서 여기서 컬라값을 원래대로 돌려놓아야 흑백으로 나오지 않게 된다.
		//SGraphicOption Option;
		//GetEtOptionController()->GetGraphicOption( Option );
		
		//Option.bEnableDOF
		CDnWorldEnvironment* pWorldEnv = m_pWorld->GetEnvironment();

		// 마을 컷신이라면 무조건 색조값을 1로 해준다. 게임으로 안나가고 마을에서 곧바로 컷신 재상하면 
		// CDnLocalPlayerActor::GetSaturationColor() 이 0을 리턴함.
		if( bVillage )
			pWorldEnv->ControlColorFilter( CDnWorldEnvironment::Saturation, 1.0f, 500 );
		else
			pWorldEnv->ControlColorFilter( CDnWorldEnvironment::Saturation, CDnLocalPlayerActor::GetSaturationColor(), 500 );

		//if( CDnActor::s_hLocalActor )
		//	CDnActor::s_hLocalActor->SetPlayingCutScene( true );
		CGlobalInfo::GetInstance().SetPlayingCutScene( true );

		SAFE_DELETE( m_pActorProcessor );
		m_pActorProcessor = new CDnCutSceneActorProcessor;

		// 현재 화면에 이미 렌더링 되고 있는 액터들을 모아 둠
		int iNumLiveActor = (int)CDnActor::s_pVecProcessList.size();
		for( int iActor = 0; iActor < iNumLiveActor; ++iActor )
		{
			DnActorHandle hLiveActor = CDnActor::s_pVecProcessList.at( iActor )->GetMySmartPtr();
			if( hLiveActor->IsNpcActor() && !hLiveActor->IsShow() )
				continue;

			m_dqhLiveActors.push_back( hLiveActor );

			// 원래 EtMatrixEx 저장
			m_vlOriMatExWorlds.push_back( *hLiveActor->GetMatEx() );

			const char* pCurrentAction = hLiveActor->GetCurrentAction();

			//#43481 스킬 사용중이면 스킬 동작이 추가되지 않도록한다.
			//현재 동작을 담고 있다가 Finalize에서 여기에서 저장된 동작을 호출하도록 되어 있음.
			if( NULL == pCurrentAction || 
				strlen(pCurrentAction) == 0 || 
				strcmp(pCurrentAction, "Summon_On") == 0
				|| hLiveActor->IsProcessSkill()
				)
				m_szOriAction.push_back( "Stand" );
			else
				m_szOriAction.push_back( pCurrentAction );
			
			//#43481 스킬 사용중 컷씬 시작 될때 클라이언트 스킬 취소 하고, 동기화 위해 CmdStop패킷 전송.
			if (hLiveActor && hLiveActor->IsProcessSkill())
			{
				hLiveActor->CancelUsingSkill();
				hLiveActor->CmdStop("Stand");				
			}
			
			CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>(hLiveActor.GetPointer());
			if( pPlayer )
				m_bVecPlayerBattleMode.push_back( pPlayer->IsBattleMode() );
			else m_bVecPlayerBattleMode.push_back( false );
		}

		// 처음엔 제외된 액터 리스트에 받아놓고 상용하는 액터들은 찾아서 삭제하도록 한다.
		m_dqhExceptActors = m_dqhLiveActors;

		DNTableFileFormat*  pActorTable = CDnTableDB::GetInstancePtr()->GetTable( CDnTableDB::TACTOR );

		m_CutScenePlayer.Initialize( m_pCutSceneData );

		// 현재 떠 있는 월드 이름
		const char* pGridName = NULL;
		if( CDnWorld::IsActive() && CDnWorld::GetInstance().GetGrid() )
			pGridName = CDnWorld::GetInstance().GetGrid()->GetName();

		const char* pCutSceneMapName = NULL;
		int iNumRes = m_pCutSceneData->GetNumRegResource();
		for( int iRes = 0; iRes < iNumRes; ++iRes )
		{
			int iResKind = m_pCutSceneData->GetRegResKindByIndex( iRes );
			if( CDnCutSceneData::RT_RES_MAP == iResKind )
			{
				pCutSceneMapName = m_pCutSceneData->GetRegResNameByIndex( iRes );
				break;
			}
		}

#if defined(_DEBUG)

		string strResMapName( pCutSceneMapName );
		transform( strResMapName.begin(), strResMapName.end(), strResMapName.begin(), tolower );

		bool bSameMap = (strResMapName == pGridName);

		//assert( bSameMap && "컷 신에 저장된 맵과 현재 맵의 이름이 다릅니다." );
#endif
		// 떨어진 아이템이 있다면 숨김
		CDnDropItem::ShowDropItems( false );

		// 카메라 셋팅
		DnCameraHandle hDnCamera = CDnCamera::GetActiveCamera();

		// #38660 카메라를 못 갖고 오는 경우가 있어 이런 경우엔 플레이어 카메라를 직접 가져오도록 수정.
		if( !hDnCamera )
			hDnCamera = CDnCamera::FindCamera( CDnCamera::PlayerCamera );
		m_CutScenePlayer.SetCamera( hDnCamera->GetCameraHandle() );

		SCameraInfo* pCameraInfo = hDnCamera->GetCameraInfo();
		m_OriginalCameraInfo = *pCameraInfo;
		//pCameraInfo->fFogFar += 20000.0f;
		const SceneInfo* pSceneInfo = m_pCutSceneData->GetSceneInfo();
		pCameraInfo->fFogFar += pSceneInfo->fFogFarDelta;
		pCameraInfo->fFar = pCameraInfo->fFogFar + 10000.0f;
		hDnCamera->SetCameraInfo( *pCameraInfo );

		// DOF 필터 물려줌
		int iDOFEffectIndex = m_pWorld->GetEnvironment()->GetDOFEffectIndex();
		DnCameraHandle hPlayerCamera = CDnCamera::FindCamera( CDnCamera::PlayerCamera );
		// 라디얼 블러 카메라 이펙트 리셋
		if( hPlayerCamera ) {
			hPlayerCamera->ResetRadialBlurEffect();
		}
		CDnCEDof* pDofEffect = NULL;
		if( hPlayerCamera ) {
			pDofEffect = static_cast<CDnCEDof*>(hPlayerCamera->FindCameraEffect( iDOFEffectIndex ));
		}
		if( pDofEffect )
		{
			m_pDOFFilter = pDofEffect->GetFilter();

			m_fNearStart =  m_pDOFFilter->GetNearDOFStart();
			m_fNearEnd = m_pDOFFilter->GetNearDOFEnd();
			m_fFarStart = m_pDOFFilter->GetFarDOFStart();
			m_fFarEnd = m_pDOFFilter->GetFarDOFEnd();
			m_fFocusDist = m_pDOFFilter->GetFocusDistance();
			m_fNearBlurSize = m_pDOFFilter->GetNearBlurSize();
			m_fFarBlurSize = m_pDOFFilter->GetFarBlurSize();
			
			m_CutScenePlayer.SetDOFFilter( m_pDOFFilter );
		}

		// 컷신 플레이어에서 카메라를 제어하기 위해서 카메라를 액터에게서 디태치 시킴.
		if( hDnCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
			CDnPlayerCamera* pPlayerCamera = static_cast<CDnPlayerCamera*>(hDnCamera.GetPointer());
			pPlayerCamera->DetachActor();
		}

		// 퀘스트 정보가 있는 컷신과 없는 컷신, 플레이어들 등장 여부에 따라 방식이 조금 나뉩니다..

		// 플레이어 캐릭터가 유령 상태이면 원래 캐릭터대로 살려놓는다.
		// 내 파티 정보 받아둔다.
		if( false == bVillage )
		{
			CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
			DWORD dwNumParty = pPartyTask->GetPartyCount();
			for( DWORD dwMember = 0; dwMember < dwNumParty; ++dwMember )
			{
				CDnPartyTask::PartyStruct* pParty = pPartyTask->GetPartyData( dwMember );
				if( !pParty->hActor ) continue;
				CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(pParty->hActor.GetPointer());
				if( pPlayerActor->IsGhost() )
				{
					pPlayerActor->SwapSingleSkin( -1 );

					CDnWorldEnvironment* pWorldEnv = CDnWorld::GetInstance().GetEnvironment();
					pWorldEnv->ControlColorFilter( CDnWorldEnvironment::Saturation, CDnLocalPlayerActor::GetSaturationColor(), 500 );
				}

			}
		}

		// 퀘스트 정보가 없는 컷신, 플레이어들이 몇 명인지, 마을에서 재생되는 건지 등등 판단.
		bool bIsQuestCutScene = (-1 != nQuestIndex && -1 != nQuestStep);
		if( !bIsQuestCutScene )
		{
			PrepareNormalCutScene( pActorTable );
		}
		else
		{
			PrepareQuestCutScene( pActorTable, nQuestIndex, nQuestStep, dwQuestPlayerUniqueID );
		}

		// 현재 액션프랍들을 골라내서 default 액션을 저장해 두자... 
		m_vlActPropDefault.clear();
		CEtWorldSector* pSector = m_pWorld->GetSector( 0.0f, 0.0f );
		int iNumProp = pSector->GetPropCount();
		for( int iProp = 0; iProp < iNumProp; ++iProp )
		{
			CEtWorldProp* pProp = pSector->GetPropFromIndex( iProp );
			CDnWorldActProp* pActProp = dynamic_cast<CDnWorldActProp*>(pProp);
			if( pActProp )
			{
				S_ACT_PROP_DEFAULT PropDefault;
				PropDefault.pActProp = pActProp;
				PropDefault.hProp = pActProp->GetMySmartPtr();
				if( pActProp->GetCurrentAction() )
					PropDefault.strDefaultAction.assign( pActProp->GetCurrentAction() );
				PropDefault.bShow = PropDefault.hProp->IsShow();

				m_vlActPropDefault.push_back( PropDefault );
			}
		}

		// 시퀀스 구성함~~
		m_CutScenePlayer.MakeUpSequences( m_pActorProcessor );

		// 시퀀스에 포함되는 녀석들은 전부 show 로 처리
		int iNumActor = m_pActorProcessor->GetNumActors();
		for( int i = 0; i < iNumActor; ++i )
		{
			S_CS_ACTOR_INFO* pInfo = m_pActorProcessor->GetActorInfoByIndex( i );
			CDnActor* pActor = static_cast<CDnActor*>( pInfo->pActor );
			pActor->Show( true );
			pActor->SetAlphaBlend( 1.0f, AL_SIGNAL );
			pActor->SetAlphaBlend( 1.0f, AL_NORMAL );
			pActor->SetAlphaBlend( 1.0f, AL_APPEAR );
			pActor->SetAlphaBlend( 1.0f, AL_STATEEFFECT );

			// 컷신 플레이어에서 ani distance 를 직접 계산해서 넣어줌.
			// 월드 매트릭스에 스케일 값이 들어갈 수가 있으므로 엔진에서 애니메이션 계산시 
			// 디폴트로 켜져 있는 Y 축 ani distance 에 적용되어 나오기 때문이다.
			// 현재는 몬스터 스케일에 관계없이 애니메이션 키 값에 영향이 없게 하도록 결정되어있다.
			if( pActor->GetAniObjectHandle() )
				pActor->GetAniObjectHandle()->SetCalcPositionFlag( 0 );
		}

		// 제외된 액터들은 모두 숨김
		int iNumActors = (int)m_dqhExceptActors.size();
		for( int iActor = 0; iActor < iNumActors; ++iActor )
		{
			DnActorHandle hActor = m_dqhExceptActors.at( iActor );
			hActor->Show( false );
			hActor->ReleaseSignalImp();
		}

		CEtSoundEngine::GetInstance().FadeVolume( NULL, 0.0f, CGlobalInfo::GetInstance().m_fFadeDelta, false );
		

#if defined(TEST_CUTSCENE)
		m_bFirstLoop = true;
#endif
	}
#if defined(TEST_CUTSCENE)
	else
		m_bFirstLoop = false;
#endif

	// 컷신 시작하기 전에 UI를 숨겨두었다면, 다시 나오게 해야 일반적이지만,
	// Finalize때 g_bEtUIRender값이 false일 경우 자연스럽게 전환되지 않고, 캐릭터가 서있는 모습이 보이게 된다.
	// UI렌더링이 안되면서 페이드아웃 화면이 안보이기 때문이다.
	// 그래서 우선 이전의 값을 복구하지 않기로 한다.
	//
	// 아무래도 g_bEtUIRender가 true더라도 페이드아웃다이얼로그는 보여줄 수 있는 방법이 필요할 듯 하다.
	//m_bPrevEtUIRender = EtInterface::g_bEtUIRender;
	EtInterface::g_bEtUIRender = true;
	CEtCustomRender::s_bEnableCustomRender = true;

#ifndef _FINAL_BUILD
#ifdef _RDEBUG
	// 파이널빌드 아닐땐 쉬프트Z 누르고 있으면 UI숨기자. UI없이 컷신 캡쳐하는 용도.
	if( GetKeyState( VK_LSHIFT )&0x80 && GetKeyState( 'Z' )&0x80 )
		EtInterface::g_bEtUIRender = false;
#endif
#endif
	m_bLoadingComplete = true;
	if( CDnLoadingTask::IsActive() ) {
		CDnLoadingTask::GetInstance().Initialize( NULL, 0 );
		CDnLoadingTask::GetInstance().EnableBackgroundLoading( true );
	}

	m_pSkipFade = new CDnFadeInOutDlg( UI_TYPE_MOST_TOP, NULL, 4444 );
	m_pSkipFade->Initialize( true );

	CInputDevice::GetInstance().HoldAbsenceCheckTimer(CInputDevice::eABSENCE_PARTY, true);
//	CDnInterface::GetInstance().UseEventMessage( false );

	bResult = true;
	return bResult;
}


void CDnCutSceneTask::PrepareNormalCutScene( DNTableFileFormat*  pActorTable )
{

// #61781 그늘숲 기슭 보스(오크 크롬쉬) 컷씬에서 무기와 방패를 들고있지 않음.
//#ifdef _WORK
	enum EWeaponError
	{
		EWeaponError_None = 0,		
		EWeaponError_Other = 1,
		EWeaponError_WeaponIndex,
		EWeaponError_Create,
		EWeaponError_Attach,		

		EWeaponError_Max
	};
	char * arrStrError[ EWeaponError::EWeaponError_Max ] = { "None", "Other", "WeaponIndex", "Create", "Attach" };
	struct STempLog
	{
		EWeaponError errorType;
		bool bCutsceneWeapon[2];
		int MonsterID;
		int weapon[2];		
		STempLog(){
			errorType = EWeaponError::EWeaponError_None; 
			bCutsceneWeapon[0] = bCutsceneWeapon[1] = true;
			MonsterID = weapon[0] = weapon[1] = 0;
		}
	};
	STempLog tempLog;	
//#endif // _WORK


	// 현재 장소가 게임인지 마을인지 체크
	bool bVillage = false;
	if( CDnWorld::MapTypeEnum::MapTypeVillage == m_pWorld->GetMapType() )
		bVillage = true;

	// 내 파티 정보 받아둔다.
	deque<CDnPartyTask::PartyStruct*> dqParty;
	if( false == bVillage )
	{
		CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
		DWORD dwNumParty = pPartyTask->GetPartyCount();
		for( DWORD dwMember = 0; dwMember < dwNumParty; ++dwMember )
		{
			CDnPartyTask::PartyStruct* pParty = pPartyTask->GetPartyData( dwMember );
			dqParty.push_back( pParty );
		}
	}

	// 주인공 액터가 하나 뿐인가
	vector<string> vlActorResNames;
	bool bSinglePlayerCharacter = false;
	int iNumMainCharacter = 0;
	int iNumActors = m_pCutSceneData->GetNumActors();
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		string strActorResName = m_pCutSceneData->GetActorByIndex( iActor );
		int iOffset = (int)strActorResName.find_last_of( "_", strActorResName.length() );
		strActorResName = strActorResName.substr( 0, iOffset );
		vlActorResNames.push_back( strActorResName );

		int iItemID = pActorTable->GetItemIDFromFieldCaseFree( "_StaticName", strActorResName.c_str() );
		CDnActorState::ActorTypeEnum eActorType = (CDnActorState::ActorTypeEnum)pActorTable->GetFieldFromLablePtr( iItemID, "_Class" )->GetInteger();
		if( eActorType <= CDnActorState::Reserved6 )
		{
			++iNumMainCharacter;
		}
	}

	if( 1 == iNumMainCharacter )
		bSinglePlayerCharacter = true;
	
	bool bMyCharacterIsMain = false;

	// 컷신에서 정해져있는 주인공 액터의 이름을 받아온다.
	const SceneInfo* pSceneInfo = m_pCutSceneData->GetSceneInfo();
	const string& strMainCharName = pSceneInfo->strMainCharacterName;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );	


	

	// 컷신에서 사용되는 액터들을 기준으로 현재 렌더링 되고 있는 액터들은 그대로 쓰고 아닌 애들은 새롭게 로딩토록 한다.		
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		S_CS_ACTOR_INFO ActorToPlay;
		const char* pActorName = m_pCutSceneData->GetActorByIndex( iActor );
		
		ActorToPlay.strActorName.assign( pActorName );

		string& strActorResName = vlActorResNames.at( iActor );

		// 툴쪽에서는 대문자로 저장되어있다. 테이블에서 찾을 때는 첫 글자만 대문자로 찾아야 한다.
		//transform( strActorResName.begin()+1, strActorResName.end(), strActorResName.begin()+1, tolower );

		int iItemID = pActorTable->GetItemIDFromFieldCaseFree( "_StaticName", strActorResName.c_str() );
		//assert( -1 != iItemID );

		CDnActorState::ActorTypeEnum eActorType = (CDnActorState::ActorTypeEnum)pActorTable->GetFieldFromLablePtr( iItemID, "_Class" ) ->GetInteger();
		bool bPlayerCharacter = eActorType <= CDnActorState::Reserved6;
		ActorToPlay.bPlayer = bPlayerCharacter; // bintitle.
		
		// 주인공 액터가 하나 뿐이라면 내 캐릭터를 주인공으로 셋팅해준다.
		DnActorHandle hFindedActor;		
		if( bPlayerCharacter )
		{
			// 등장하는 플레이어 캐릭터가 하나면 이름과 상관없이 무조건 내 캐릭을 주인공으로.
			if( bSinglePlayerCharacter )
			{
				hFindedActor = CDnActor::s_hLocalActor;
				bMyCharacterIsMain = true;
			}
			else
			{
				string strActorResName = m_pCutSceneData->GetActorByIndex( iActor );

				// #60295 칼리 오프닝 컷씬 칼리 모습이 보이지 않음. - 칼리 모습이 보이지 않음 - 복제 플레이어 액터여부 검사.
				int nItemID = static_cast< CDnActor * >(CDnActor::s_hLocalActor)->GetClassID();						
				if( pSox->IsExistItem( nItemID ) ) {		
					//const wchar_t * pStrLocalActorName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_EnglishName" )->GetInteger() );
					int cntName = (int)m_arrCharacterName.size();
					for( int i=0; i<cntName; ++i )
					{		
						int characterNameIndex = nItemID - 1;
						if (characterNameIndex < 0 || characterNameIndex >= cntName)
						{
							_ASSERT(0);
							break;
						}

						/*if( strstr( m_arrCharacterName[characterNameIndex].c_str(), m_arrCharacterName[ i ].c_str() ) &&
							strstr( strActorResName.c_str(), m_arrCharacterName[ i ].c_str() ) )
						{
							std::string::size_type iOffset = strActorResName.find( "_" );
							if( iOffset != std::string::npos )
							{
								strActorResName = strActorResName.substr( ++iOffset, strActorResName.size() );
								int number = atoi( strActorResName.c_str() );
								if( number > 1 )
									ActorToPlay.bClone = true;
							}
						}*/
						if( strstr( m_arrCharacterName[characterNameIndex].c_str(), m_arrCharacterName[ i ].c_str() ) &&
							strstr( strActorResName.c_str(), m_arrCharacterName[ i ].c_str() ) )
						{
							if( strstr( strActorResName.c_str(), "CLONE" ) )
								ActorToPlay.bClone = true;
						}
							
					}	
				}	

				if( ActorToPlay.bClone == false ) // #60295 칼리 오프닝 컷씬 - 칼리 모습이 보이지 않음.
				{
					// 현재 주인공 자리라면 내 캐릭터로 주인공 삼아줌..
					if( false == bMyCharacterIsMain && strMainCharName == pActorName )
					{
						hFindedActor = CDnActor::s_hLocalActor;
						bMyCharacterIsMain = true;
					}
				}
			}	
			
		}

		// 주인공 액터로 내가 셋팅이 안되면 제외 리스트에서 찾는다
		if( NULL == hFindedActor && ActorToPlay.bClone == false )
		{
			// 같은 ItemID 를 갖는 액터를 찾는다. 제외된 액터리스트에서 찾고 나서 삭제.
			// 보스 셋팅이 있는 녀석인 경우엔 몬스터 테이블 ID까지 같아야 한다.
			int iNumLiveActor = (int)m_dqhExceptActors.size();
			for( int iActor = 0; iActor < iNumLiveActor; ++iActor )
			{
				DnActorHandle hLiveActor = m_dqhExceptActors.at( iActor );
				if( hLiveActor->GetClassID() == iItemID )
				{
					bool bScaleLock = m_pCutSceneData->GetThisActorsScaleLock( pActorName );
					if( bScaleLock )
					{
						// 스케일 락인 경우엔 기본 스케일이 아니게 생성된 액터는 컷신에서 사용하지 않는다.
						if( hLiveActor->GetScale()->x != 1.0f )
							continue;
					}
					else
					{
						// 플레이어는 당연히 0이고 몬스터중에서도 보스로 셋팅된 경우만 실제 몬스터 테이블 ID가 있다.
						// 해당 몬스터 테이블의 ID 까지 맞은 경우에 화면에 있는 이 액터를 선택하도록 한다.
						int iMonsterTableID = m_pCutSceneData->GetMonsterTableID( pActorName );
						if( 0 != iMonsterTableID && false == bPlayerCharacter )
						{
							// 몬스터 액터이고 툴에서 셋팅된 몬스터 테이블의 아이디와 같다면 통과~ 
							// 아니라면 continue 로 패스.
							CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>( hLiveActor.GetPointer() );
							if( false == (pMonsterActor && (iMonsterTableID == pMonsterActor->GetMonsterClassID())) )
							{
								continue;
							}
						}
					}

					hFindedActor = hLiveActor;
					deque<DnActorHandle>::iterator iter = m_dqhExceptActors.begin();
					advance( iter, iActor );
					m_dqhExceptActors.erase( iter );
					break;
				}
			}
		}
		else
		{
			// 같은 ItemID 를 갖는 액터를 찾는다. 포함되게 되었으므로 제외 리스트에서 삭제만 해줌.
			int iNumLiveActor = (int)m_dqhExceptActors.size();
			for( int iActor = 0; iActor < iNumLiveActor; ++iActor )
			{
				DnActorHandle hLiveActor = m_dqhExceptActors.at( iActor );
				if( hLiveActor == hFindedActor )
				{
					deque<DnActorHandle>::iterator iter = m_dqhExceptActors.begin();
					advance( iter, iActor );
					m_dqhExceptActors.erase( iter );
					break;
				}
			}
		}
		//else
		//{
		//	int iNumLiveActor = (int)m_dqhExceptActors.size();
		//	for( int iActorInner = 0; iActorInner < iNumLiveActor; ++iActorInner )
		//	{
		//		DnActorHandle hLiveActor = m_dqhExceptActors.at( iActorInner );

		//		if( hLiveActor == hFindedActor )
		//		{
		//			deque<DnActorHandle>::iterator iter = m_dqhExceptActors.begin();
		//			advance( iter, iActorInner );
		//			m_dqhExceptActors.erase( iter );
		//			break;
		//		}
		//	}
		//}

		bool bMonster = (eActorType > CDnActorState::Reserved6 && eActorType < CDnActorState::Npc);

		if( NULL == hFindedActor )
		{
			// 기획쪽에서 데이터 제대로 넣기 전까지 
			if( bPlayerCharacter )
			{
				// // #60295 칼리 오프닝 컷씬 칼리 모습이 보이지 않음 - 플레이어 액터가 두개 이상인 경우 복제한다.				
				if( ActorToPlay.bClone )
					AddClonePlayer( hFindedActor );

				else
				{
					// 아직 내 캐릭터 등록 안되어있으면 바로 등록함
					if( false == bMyCharacterIsMain )
					{
						hFindedActor = CDnActor::s_hLocalActor;
						bMyCharacterIsMain = true;
					}
					else
					{
						// 플레이어를 못 찾았다면 주인공 캐릭 제외하고, 파티 순서대로 아무나 맵핑해줌.
						do
						{
							if( dqParty.empty() )
								break;

							hFindedActor = dqParty.front()->hActor;
							dqParty.pop_front();
						}
						while( hFindedActor == CDnActor::s_hLocalActor );

						if( dqParty.empty() )
							continue;
					}
				}
				
			}
			
			else
			{			
				
				//// NOTE: 플레이어 캐릭터들은 새로 로딩할 필요 없습니다!1
				//// 주인공들은 디폴트 무기를 들려줌
				//if( (eActorType == CDnActorState::Warrior) ||
				//	(eActorType == CDnActorState::Archer) ||
				//	(eActorType == CDnActorState::Soceress) ||
				//	(eActorType == CDnActorState::Cleric) )
				//{
				//	hFindedActor = CreateActor( iItemID, false, true );
				//	assert( hFindedActor && "컷신용 액터 생성 실패" ); 

				//	const char* astrDefaultSkinFileName[ 4 ] = { "Warrior.skn", "Archer.skn", "Soceress.skn", "Cleric.skn" };
				//	const char* astrDefaultAniFileName[ 4 ] = { "Warrior.ani", "Archer.ani", "Soceress.ani", "Cleric.ani" };
				//	const char* astrDefaultActFileName[ 4 ] = { "Warrior.act", "Archer.act", "Soceress.act", "Cleric.act" };

				//	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TDEFAULTCREATE );

				//	//MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody *>(hActor.GetPointer());

				//	//char *pStaticName = pActorSOX->GetFieldFromLablePtr( nClassID, "_StaticName" )->GetString();
				//	//char *szDummySkinName = pActorSOX->GetFieldFromLablePtr( nClassID, "_SkinName" )->GetString();

				//	MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody *>(hFindedActor.GetPointer());
				//	_ASSERT( pPartsBody );
				//	pPartsBody->LoadSkin( CEtResourceMng::GetInstance().GetFullName( astrDefaultSkinFileName[eActorType] ).c_str(),
				//		CEtResourceMng::GetInstance().GetFullName( astrDefaultAniFileName[eActorType] ).c_str() );
				//	hFindedActor->LoadAction( CEtResourceMng::GetInstance().GetFullName( astrDefaultActFileName[eActorType] ).c_str() );

				//	char szLabel[32];
				//	for( int j=0; j<2; j++ ) {
				//		sprintf_s( szLabel, "_Weapon%d", j + 1 );
				//		int nWeaponIndex = pSox->GetFieldFromLablePtr( iItemID, szLabel )->GetInteger();
				//		if( nWeaponIndex != -1 ) {
				//			DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeaponIndex, 0 );
				//			hFindedActor->AttachWeapon( hWeapon, j, true );
				//		}
				//	}

				////	CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(hFindedActor.GetPointer());
				////	_ASSERT( pPlayerActor );
				////	pPlayerActor->SetBattleMode( true );

				//	//hFindedActor->CombineParts();
				//}
				//else


				// 몬스터나 npc 못 찾았으면 따로 로딩 한다.
				DNTableFileFormat* pActorSOX = GetDNTable( CDnTableDB::TACTOR );

				if( eActorType == CDnActorState::Npc )
				{
					hFindedActor = CreateActor( iItemID );
					assert( hFindedActor && "컷신용 액터 생성 실패" ); 

					// NPC
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNPC );

					vector<int> vlNPCIDList;
					pSox->GetItemIDListFromField( "_ActorIndex", iItemID, vlNPCIDList );
					_ASSERT( !vlNPCIDList.empty() && "해당 NPC 액터의 정보가 NPC 테이블에 없습니다" );

					if( false == vlNPCIDList.empty() )
					{
						int iNPCTableID = (int)vlNPCIDList.front();
						char szWeaponLable[32];
						for( int j=0; j<2; j++ ) {
							sprintf_s( szWeaponLable, "_Weapon%dIndex", j + 1 );
							int nWeapon = pSox->GetFieldFromLablePtr( iNPCTableID, szWeaponLable )->GetInteger();
							if( nWeapon < 1 ) continue;
							DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeapon, _rand(), 0, 0, 0, 0, false, false, false );
							hFindedActor->AttachWeapon( hWeapon, j, true );
						}
					}
				}
				else
				{
					hFindedActor = CreateActor( iItemID );
					assert( hFindedActor && "컷신용 액터 생성 실패" ); 

					// 몬스터들은 몬스터 테이블을 참조해서 무기를 들려줌
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );

					vector<int> vlMonsterIDList;
					pSox->GetItemIDListFromField( "_ActorTableID", iItemID, vlMonsterIDList );
					//_ASSERT( !vlMonsterIDList.empty() && "해당 Monster 액터의 정보가 Monster 테이블에 없습니다" );
					if( false == vlMonsterIDList.empty() )
					{
						int iMonsterTableID = (int)vlMonsterIDList.front();

						// 플레이어는 당연히 0이고 몬스터중에서도 보스로 셋팅된 경우만 실제 몬스터 테이블 ID가 있다.
						// 설정된 몬스터의 ID로 로드하도록 한다. 없으면 assert 하나 띄우고 그냥 몬스터 로드한다.
						int iToolSelectedMonsterTableID = m_pCutSceneData->GetMonsterTableID( pActorName );
						if( 0 != iToolSelectedMonsterTableID )
						{
							bool bScaleLock = m_pCutSceneData->GetThisActorsScaleLock( pActorName );
							_ASSERT( false == bScaleLock );

							vector<int>::iterator iter = find( vlMonsterIDList.begin(), vlMonsterIDList.end(), iMonsterTableID );
							if( vlMonsterIDList.end() != iter )
							{
								iMonsterTableID = iToolSelectedMonsterTableID;

								// 스케일 셋팅. 그냥 랜덤 안돌리고 최대 크기로 로딩.
								float fScale = pSox->GetFieldFromLablePtr( iMonsterTableID, "_SizeMax" )->GetFloat();
								EtVector3 vScale = EtVector3( fScale, fScale, fScale );
								static_cast<CDnMonsterActor*>(hFindedActor.GetPointer())->SetScale( fScale );
								hFindedActor->SetScale( vScale );
								hFindedActor->SetActionSignalScale( vScale );
							}
						}

						char szWeaponLable[32];
						for( int j=0; j<2; j++ )
						{
							sprintf_s( szWeaponLable, "_Weapon%dIndex", j + 1 );

							/*int nWeapon = pSox->GetFieldFromLablePtr( iMonsterTableID, szWeaponLable )->GetInteger();
							if( nWeapon < 1 ) continue;
							DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeapon, _rand(), 0, 0, 0, 0, false, false, false );
							hFindedActor->AttachWeapon( hWeapon, j, true );*/

							int nWeapon = pSox->GetFieldFromLablePtr( iMonsterTableID, szWeaponLable )->GetInteger();
							if( nWeapon < 1 )
							{
#ifdef _WORK
								tempLog.errorType = EWeaponError::EWeaponError_WeaponIndex;
								tempLog.MonsterID = iMonsterTableID;
								tempLog.bCutsceneWeapon[ j ] = false;
								tempLog.weapon[ j ] = nWeapon;
#endif // _WORK
								continue;							
							}

							DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeapon, _rand(), 0, 0, 0, 0, false, false, false );
							hFindedActor->AttachWeapon( hWeapon, j, true );
						
#ifdef _WORK
							if( !hWeapon || !hFindedActor->GetWeapon(j) )
							{
								if( !hWeapon )
									tempLog.errorType = EWeaponError::EWeaponError_Create;
								if( !hFindedActor->GetWeapon(j) )
									tempLog.errorType = EWeaponError::EWeaponError_Attach;
								
								tempLog.MonsterID = iMonsterTableID;
								tempLog.bCutsceneWeapon[ j ] = false;
								tempLog.weapon[ j ] = nWeapon;
							}
#endif // _WORK
							
						}
					}
				}

				m_vlhAdditionalLoadedActors.push_back( hFindedActor );
			}
			//else
			//if( false == bVillage )
			//{
			//	// 주인공들 캐릭터들 같은 경우는 남의 파티 캐릭터들이 들어가지 않게 하기 위해서 자신의 파티정보로 인증을 받는다.
			//	if( (hFindedActor->GetActorType() == CDnActorState::Warrior) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Archer) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Soceress) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Cleric) )
			//	{
			//		CDnPartyTask::PartyStruct* pSelectedMember = NULL;

			//		// 파티에 해당 액터 타입이 있나 찾아보자.
			//		int iNumParty = (int)vlParty.size();
			//		for( int iMember = 0; iMember < iNumParty; ++iMember )
			//		{
			//			CDnPartyTask::PartyStruct* pMember = vlParty.at( iMember );
			//			if( pMember->hActor == hFindedActor )
			//			{				
			//				pSelectedMember = pMember;
			//				break;
			//			}
			//		}
			//	}
			//}

		}

		//if( false == bVillage )
		//	continue;

		// 아카데믹 지원 안하는 컷신인데 데이터가 들어있으면 걸러냄. /////////////
#if defined (PRE_ADD_ACADEMIC)
		if( pSceneInfo->bAcademicNotSupported )
		{
			if( CDnActorState::Academic == hFindedActor->GetActorType() )
				continue;
		}
#endif
		//////////////////////////////////////////////////////////////////////////

		hFindedActor->Show( true );

		// 바로 죽으면 안되기 때문에 HP 를 셋팅해준다.
		if( 0 == hFindedActor->GetHP() )
		{
			if( bMonster )
				m_vlhDeadMonster.push_back( hFindedActor );

			hFindedActor->SetHP( 10 );
		}

		// Rotha : 플레이어가 말에 붙어있는데 컷신 재생이 되면 안되므로 , 말에서 내려주자
		CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(hFindedActor.GetPointer());
		if(pPlayer && pPlayer->IsVehicleMode() && pPlayer->GetMyVehicleActor())
			pPlayer->UnRideVehicle(true);			

		ActorToPlay.pActor = (void*)hFindedActor.GetPointer();
		ActorToPlay.hObject = hFindedActor->GetObjectHandle();
		if( hFindedActor->IsPlayerActor() )
			ActorToPlay.hMergedObject = static_cast<CDnPlayerActor*>(hFindedActor.GetPointer())->GetCombineObject();
		ActorToPlay.fRotYDegree = m_pCutSceneData->GetRegResRot( ActorToPlay.strActorName.c_str() );
		EtMatrixRotationY( &ActorToPlay.matRotY, ActorToPlay.fRotYDegree*ET_PI / 180.0f );
		ActorToPlay.vPos = m_pCutSceneData->GetRegResPos( ActorToPlay.strActorName.c_str() );
		hFindedActor->GetCachedAniIndexList( ActorToPlay.vlAniIndex );
		ActorToPlay.iNowAniIndex = 0;

		// 액션을 stand 로 바꿔준다. 스킬 등등의 시그널이 엮여서 카메라 효과가 나오거나 하지 않도록.
		// 컷신에서는 따로 액션을 바꾸는게 아니라 AniFrame 을 직접 조절하기 때문에 기존 액션의 시그널이 처리되지 않도록 stand 를 바꿔준다.
		hFindedActor->CmdAction( "Stand" );

		// 동영상이 재생되는 동안은 액터들의 컬링을 끈다.
		if( ActorToPlay.hObject ) 
			ActorToPlay.hObject->EnableCull( false );

		if( ActorToPlay.hMergedObject )
			ActorToPlay.hMergedObject->EnableCull( false );

		
		// #71465 -【그 외】할로윈 펌프킨 정령의 그래픽이 이벤트 무비에서 표시되지 않는다.
		// - 
		if( pPlayer )
		{
			DnPartsHandle hParts = pPlayer->GetCashParts( CDnParts::PartsTypeEnum::CashFairy );
			if( hParts )
				hParts->ShowRenderBase( false );
		}


		// 컷신 나오는 동안엔 액터의 STE_Input/STE_InputHasPassiveSkill 을 처리하지 않는다.
		//hFindedActor->SetPlayingCutScene( true );

		// 컷신 나오는 동안엔 액터에 박힌 사운드 시그널을 재생하지 않는다.
		hFindedActor->SetPlaySignalSound( m_pCutSceneData->GetSceneInfo()->bUseSignalSound );

		m_pActorProcessor->AddObjectToPlay( &ActorToPlay );		



//#ifdef _WORK		
		if( tempLog.errorType != EWeaponError::EWeaponError_None ) 
		{
			DNTableFileFormat* pSoxWeapon = GetDNTable( CDnTableDB::TWEAPON );
			DNTableFileFormat*  pFileNameSox = GetDNTable( CDnTableDB::TFILE );

			if( pSoxWeapon && pFileNameSox )
			{
				std::string szSkinName;
				char buff[512] = {0,};	
				for( int i=0; i<2; ++i )
				{
					int weaponIndex = tempLog.weapon[i];
					if( tempLog.bCutsceneWeapon[ i ] == false &&
						pSoxWeapon->IsExistItem( weaponIndex ))
					{						
						CommonUtil::GetFileNameFromFileEXT(szSkinName, pSoxWeapon, weaponIndex, "_SkinName", pFileNameSox);			

						sprintf_s( buff, 512, "$$$$$ CutSceneLog >> ErrorType:%s, Monster:%d %s, Weapon:%s %d %s \n",
							arrStrError[ tempLog.errorType ], //tempLog.errorType  strErrorType,
							tempLog.MonsterID, ActorToPlay.strActorName.c_str(), 
							i==0?L"Right":L"Left", weaponIndex, szSkinName.c_str() );
						g_Log.Log( LogType::_FILELOG, buff );
					}

					memset( buff, 0, 512 );
				}
			}
		}

//#endif // _WORK



	}

}


// 퀘스트 컷신은 마을에서 재생되는 일이 없다.
void CDnCutSceneTask::PrepareQuestCutScene( DNTableFileFormat*  pActorTable, int nQuestIndex, int nQuestStep, DWORD dwQuestPlayerUniqueID )
{
	// 내 파티 정보 받아둔다.
	deque<CDnPartyTask::PartyStruct*> dqParty;
	CDnPartyTask* pPartyTask = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	DWORD dwNumParty = pPartyTask->GetPartyCount();
	for( DWORD dwMember = 0; dwMember < dwNumParty; ++dwMember )
	{
		CDnPartyTask::PartyStruct* pParty = pPartyTask->GetPartyData( dwMember );
		dqParty.push_back( pParty );
	}

	// 내가 퀘스트 수행자인가. 퀘스트 컷신에서는 퀘스트 수행자가 주인공이 된다.
	bool bMyQuestCutScene = false;
	CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));
	const TQuest* pQuest = pQuestTask->GetQuestIndex( nQuestIndex );
	if( pQuest )
	{
		if( pQuest->nQuestStep == nQuestStep )
			bMyQuestCutScene = true;
	}

	//bool bMyCharacterIsMain = false;

	bool bQuestPlayerSelected = false;

	// 등장 액터가 하나 뿐인가
	int iNumPlayerActor = 0;
	vector<string> vlActorResNames;
	int iNumActors = m_pCutSceneData->GetNumActors();
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		string strActorResName = m_pCutSceneData->GetActorByIndex( iActor );
		int iOffset = (int)strActorResName.find_last_of( "_", strActorResName.length() );
		strActorResName = strActorResName.substr( 0, iOffset );
		vlActorResNames.push_back( strActorResName );

		int iItemID = pActorTable->GetItemIDFromFieldCaseFree( "_StaticName", strActorResName.c_str() );
		CDnActorState::ActorTypeEnum eActorType = (CDnActorState::ActorTypeEnum)pActorTable->GetFieldFromLablePtr( iItemID, "_Class" )->GetInteger();
		bool bPlayerCharacter = (eActorType <= CDnActorState::Reserved6);
		if( bPlayerCharacter )
		{
			++iNumPlayerActor;
		}
	}

	// 컷신에서 정해져있는 주인공 액터의 이름을 받아온다.
	const SceneInfo* pSceneInfo = m_pCutSceneData->GetSceneInfo();
	const string& strMainCharName = pSceneInfo->strMainCharacterName;
	// 컷신에서 사용되는 액터들을 기준으로 현재 렌더링 되고 있는 액터들은 그대로 쓰고 아닌 애들은 새롭게 로딩토록 한다.	
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		S_CS_ACTOR_INFO ActorToPlay;
		const char* pActorName = m_pCutSceneData->GetActorByIndex( iActor );

		ActorToPlay.strActorName.assign( pActorName );

		string& strActorResName = vlActorResNames.at( iActor );

		// 툴쪽에서는 대문자로 저장되어있다. 테이블에서 찾을 때는 첫 글자만 대문자로 찾아야 한다.
		//transform( strActorResName.begin()+1, strActorResName.end(), strActorResName.begin()+1, tolower );

		int iItemID = pActorTable->GetItemIDFromFieldCaseFree( "_StaticName", strActorResName.c_str() );
		//assert( -1 != iItemID );

		CDnActorState::ActorTypeEnum eActorType = (CDnActorState::ActorTypeEnum)pActorTable->GetFieldFromLablePtr( iItemID, "_Class" )->GetInteger();
		bool bPlayerCharacter = eActorType <= CDnActorState::Reserved6;

		ActorToPlay.bPlayer = bPlayerCharacter; // bintitle.

		DnActorHandle hFindedActor;

		// 등장 액터가 한 명이거나 주인공 자리일 때, 내가 퀘스트 수행자라면 내가 주인공으로 되도록..
		// 내가 퀘스트 수행자가 아니라면 파티원 중에 퀘스트 수행자로 셋팅해준다.
		if( bPlayerCharacter &&
			((1 == iNumPlayerActor) || (strMainCharName == pActorName)) &&
			false == bQuestPlayerSelected )
		{
			if( true == bMyQuestCutScene )
			{
				deque<CDnPartyTask::PartyStruct*>::iterator iter = dqParty.begin();
				for( iter; iter != dqParty.end(); ++iter )
				{
					if( (*iter)->hActor == CDnActor::s_hLocalActor )
					{
						hFindedActor = CDnActor::s_hLocalActor;
						dqParty.erase( iter );
						bQuestPlayerSelected = true;
						break;
					}
				}
			}
			else
			{
				deque<CDnPartyTask::PartyStruct*>::iterator iter = dqParty.begin();
				for( iter; iter != dqParty.end(); ++iter )
				{
					if( (*iter)->hActor->GetUniqueID() == dwQuestPlayerUniqueID )
					{
						hFindedActor = (*iter)->hActor;
						dqParty.erase( iter );
						bQuestPlayerSelected = true;
						break;
					}
				}
			}
		}

		// 주인공 액터가 하나 뿐이라면 내 캐릭터를 주인공으로 셋팅해준다.
		//DnActorHandle hFindedActor;
		//if( false == bMyCharacterIsMain && bPlayerCharacter )
		//{
		//	// 주인공 자리이거나 플레이어가 한명만 나온다거나하고 내가 퀘스트 수행자라면 내 캐릭터로 주인공 삼아줌..
		//	if( strMainCharName == pActorName ||
		//		1 == iNumPlayerActor )
		//	{
		//		if( bMyQuestCutScene )
		//		{
		//			hFindedActor = CDnActor::s_hLocalActor;
		//			deque<CDnPartyTask::PartyStruct*>::iterator iter = dqParty.begin();
		//			for( iter; iter != dqParty.end(); ++iter )
		//			{
		//				if( (*iter)->hActor == hFindedActor )
		//				{
		//					dqParty.erase( iter );
		//					break;
		//				}
		//			}
		//			bMyCharacterIsMain = true;
		//		}
		//	}
		//}

		if( NULL == hFindedActor )
		{
			if( bPlayerCharacter )
			{
				// 플레이어를 못 찾았다면 주인공 캐릭 제외하고, 파티 순서대로 아무나 맵핑해줌.
				// 유령된 애는 넣어주지 않는다.
				if( false == dqParty.empty() )
				{
					while( false == dqParty.empty() )
					{
						DnActorHandle hActor = dqParty.front()->hActor;
						dqParty.pop_front();
				
						// 아카데믹 지원 안하는 컷신인데 데이터가 들어있으면 걸러냄. /////////////
#if defined (PRE_ADD_ACADEMIC)
						if( pSceneInfo->bAcademicNotSupported )
						{
							if( CDnActorState::Academic == hActor->GetActorType() )
								continue;
						}
#endif
						//////////////////////////////////////////////////////////////////////////
						hFindedActor = hActor;
						break;
					}
				}
			}
		}

		if( NULL == hFindedActor )
		{
			// 같은 ItemID 를 갖는 액터를 찾는다. 제외된 액터리스트에서 찾고 나서 삭제.
			int iNumLiveActor = (int)m_dqhExceptActors.size();
			for( int iActorInner = 0; iActorInner < iNumLiveActor; ++iActorInner )
			{
				DnActorHandle hLiveActor = m_dqhExceptActors.at( iActorInner );

				if( hLiveActor->GetClassID() == iItemID )
				{
					bool bScaleLock = m_pCutSceneData->GetThisActorsScaleLock( pActorName );
					if( bScaleLock )
					{
						// 스케일 락인 경우엔 기본 스케일이 아니게 생성된 액터는 컷신에서 사용하지 않는다.
						if( hLiveActor->GetScale()->x != 1.0f )
							continue;
					}
					else
					{
						// 플레이어는 당연히 0이고 몬스터중에서도 보스로 셋팅된 경우만 실제 몬스터 테이블 ID가 있다.
						// 해당 몬스터 테이블의 ID 까지 맞은 경우에 화면에 있는 이 액터를 선택하도록 한다.
						int iMonsterTableID = m_pCutSceneData->GetMonsterTableID( pActorName );
						if( 0 != iMonsterTableID && false == bPlayerCharacter )
						{
							// 몬스터 액터이고 툴에서 셋팅된 몬스터 테이블의 아이디와 같다면 통과~ 
							// 아니라면 continue 로 패스.
							CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>( hLiveActor.GetPointer() );
							if( false == (pMonsterActor && (iMonsterTableID == pMonsterActor->GetMonsterClassID())) )
							{
								continue;
							}
						}
					}

					hFindedActor = hLiveActor;
					deque<DnActorHandle>::iterator iter = m_dqhExceptActors.begin();
					advance( iter, iActorInner );
					m_dqhExceptActors.erase( iter );
					break;
				}
			}
		}
		else
		{
			int iNumLiveActor = (int)m_dqhExceptActors.size();
			for( int iActorInner = 0; iActorInner < iNumLiveActor; ++iActorInner )
			{
				DnActorHandle hLiveActor = m_dqhExceptActors.at( iActorInner );

				if( hLiveActor == hFindedActor )
				{
					deque<DnActorHandle>::iterator iter = m_dqhExceptActors.begin();
					advance( iter, iActorInner );
					m_dqhExceptActors.erase( iter );
					break;
				}
			}
		}

		bool bMonster = (eActorType > CDnActorState::Reserved6 && eActorType < CDnActorState::Npc);
		if( NULL == hFindedActor )
		{
			// 플레이어 액터를 제외 리스트에서 찾지 못했다면 액터를 추가하지 않는다.
			if( bPlayerCharacter )
			{
				//m_pActorProcessor->AddObjectToPlay( &ActorToPlay );
				m_CutScenePlayer.HideActor( ActorToPlay.strActorName.c_str() );
			}
			
			{
				// 몬스터나 npc 못 찾았으면 따로 로딩 한다.
				DNTableFileFormat* pActorSOX = GetDNTable( CDnTableDB::TACTOR );

				// NOTE: 플레이어 캐릭터들은 새로 로딩할 필요 없습니다!1
				// 주인공들은 디폴트 무기를 들려줌
#ifdef PRE_ADD_ACADEMIC
				if( CDnActorState::Warrior <= eActorType &&
					eActorType <= CDnActorState::Academic )
#else
				if( CDnActorState::Warrior <= eActorType &&
					eActorType <= CDnActorState::Cleric )
#endif // #ifdef PRE_ADD_ACADEMIC
				{
					hFindedActor = CreateActor( iItemID, false, true );
					assert( hFindedActor && "컷신용 액터 생성 실패" ); 

#ifdef PRE_ADD_ACADEMIC
					const char* astrDefaultSkinFileName[ CDnActorState::Academic+1 ] = { "Warrior.skn", "Archer.skn", "Soceress.skn", "Cleric.skn", "Academic.skn" };
					const char* astrDefaultAniFileName[ CDnActorState::Academic+1 ] = { "Warrior.ani", "Archer.ani", "Soceress.ani", "Cleric.ani", "Academic.ani" };
					const char* astrDefaultActFileName[ CDnActorState::Academic+1 ] = { "Warrior.act", "Archer.act", "Soceress.act", "Cleric.act", "Academic.act" };
#else
					const char* astrDefaultSkinFileName[ CDnActorState::Cleric+1 ] = { "Warrior.skn", "Archer.skn", "Soceress.skn", "Cleric.skn" };
					const char* astrDefaultAniFileName[ CDnActorState::Cleric+1 ] = { "Warrior.ani", "Archer.ani", "Soceress.ani", "Cleric.ani" };
					const char* astrDefaultActFileName[ CDnActorState::Cleric+1 ] = { "Warrior.act", "Archer.act", "Soceress.act", "Cleric.act" };
#endif // #ifdef PRE_ADD_ACADEMIC

					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TDEFAULTCREATE );

					MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody *>(hFindedActor.GetPointer());
					_ASSERT( pPartsBody );
					pPartsBody->LoadSkin( CEtResourceMng::GetInstance().GetFullName( astrDefaultSkinFileName[eActorType] ).c_str(),
						CEtResourceMng::GetInstance().GetFullName( astrDefaultAniFileName[eActorType] ).c_str() );
					hFindedActor->LoadAction( CEtResourceMng::GetInstance().GetFullName( astrDefaultActFileName[eActorType] ).c_str() );

					char szLabel[32];
					for( int j=0; j<2; j++ ) {
						sprintf_s( szLabel, "_Weapon%d", j + 1 );
						int nWeaponIndex = pSox->GetFieldFromLablePtr( iItemID, szLabel )->GetInteger();
						if( nWeaponIndex != -1 ) {
							DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeaponIndex, _rand(), 0, 0, 0, 0, false, false, false );
							hFindedActor->AttachWeapon( hWeapon, j, true );
						}
					}
				}
				else
				if( eActorType == CDnActorState::Npc )
				{
					hFindedActor = CreateActor( iItemID );
					assert( hFindedActor && "컷신용 액터 생성 실패" ); 

					// NPC
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNPC );

					vector<int> vlNPCIDList;
					pSox->GetItemIDListFromField( "_ActorIndex", iItemID, vlNPCIDList );
					_ASSERT( !vlNPCIDList.empty() && "해당 NPC 액터의 정보가 NPC 테이블에 없습니다" );

					if( false == vlNPCIDList.empty() )
					{
						int iNPCTableID = (int)vlNPCIDList.front();
						char szWeaponLable[32];
						for( int j=0; j<2; j++ ) {
							sprintf_s( szWeaponLable, "_Weapon%dIndex", j + 1 );
							int nWeapon = pSox->GetFieldFromLablePtr( iNPCTableID, szWeaponLable )->GetInteger();
							if( nWeapon < 1 ) continue;
							DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeapon, _rand(), 0, 0, 0, 0, false, false, false );
							hFindedActor->AttachWeapon( hWeapon, j, true );
						}
					}
				}
				else
				{
					bMonster = true;
					hFindedActor = CreateActor( iItemID );
					assert( hFindedActor && "컷신용 액터 생성 실패" ); 

					// 몬스터들은 몬스터 테이블을 참조해서 무기를 들려줌
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );

					vector<int> vlMonsterIDList;
					pSox->GetItemIDListFromField( "_ActorTableID", iItemID, vlMonsterIDList );
					//_ASSERT( !vlMonsterIDList.empty() && "해당 Monster 액터의 정보가 Monster 테이블에 없습니다" );
					if( false == vlMonsterIDList.empty() )
					{
						int iMonsterTableID = (int)vlMonsterIDList.front();

						// 플레이어는 당연히 0이고 몬스터중에서도 보스로 셋팅된 경우만 실제 몬스터 테이블 ID가 있다.
						// 설정된 몬스터의 ID로 로드하도록 한다. 없으면 assert 하나 띄우고 그냥 몬스터 로드한다.
						int iToolSelectedMonsterTableID = m_pCutSceneData->GetMonsterTableID( pActorName );
						if( 0 != iToolSelectedMonsterTableID )
						{
							vector<int>::iterator iter = find( vlMonsterIDList.begin(), vlMonsterIDList.end(), iMonsterTableID );
							if( vlMonsterIDList.end() != iter )
							{
								iMonsterTableID = iToolSelectedMonsterTableID;

								// 스케일 셋팅. 그냥 랜덤 안돌리고 최대 크기로 로딩.
								float fScale = pSox->GetFieldFromLablePtr( iMonsterTableID, "_SizeMax" )->GetFloat();
								EtVector3 vScale = EtVector3( fScale, fScale, fScale );
								static_cast<CDnMonsterActor*>(hFindedActor.GetPointer())->SetScale( fScale );
								hFindedActor->SetScale( vScale );
								hFindedActor->SetActionSignalScale( vScale );
							}
						}

						char szWeaponLable[32];
						for( int j=0; j<2; j++ )
						{
							sprintf_s( szWeaponLable, "_Weapon%dIndex", j + 1 );

							int nWeapon = pSox->GetFieldFromLablePtr( iMonsterTableID, szWeaponLable )->GetInteger();
							if( nWeapon < 1 ) continue;
							DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeapon, _rand(), 0, 0, 0, 0, false, false, false );
							hFindedActor->AttachWeapon( hWeapon, j, true );
						}
					}
				}

				m_vlhAdditionalLoadedActors.push_back( hFindedActor );
			}
			//else
			//if( false == bVillage )
			//{
			//	// 주인공들 캐릭터들 같은 경우는 남의 파티 캐릭터들이 들어가지 않게 하기 위해서 자신의 파티정보로 인증을 받는다.
			//	if( (hFindedActor->GetActorType() == CDnActorState::Warrior) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Archer) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Soceress) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Cleric) )
			//	{
			//		CDnPartyTask::PartyStruct* pSelectedMember = NULL;

			//		// 파티에 해당 액터 타입이 있나 찾아보자.
			//		int iNumParty = (int)vlParty.size();
			//		for( int iMember = 0; iMember < iNumParty; ++iMember )
			//		{
			//			CDnPartyTask::PartyStruct* pMember = vlParty.at( iMember );
			//			if( pMember->hActor == hFindedActor )
			//			{				
			//				pSelectedMember = pMember;
			//				break;
			//			}
			//		}
			//	}
			//}

		}

		hFindedActor->Show( true );

		// 바로 죽으면 안되기 때문에 HP 를 셋팅해준다.
		if( 0 == hFindedActor->GetHP() )
		{
			if( bMonster )
				m_vlhDeadMonster.push_back( hFindedActor );

			hFindedActor->SetHP( 10 );
		}

		ActorToPlay.pActor = (void*)hFindedActor.GetPointer();
	
		ActorToPlay.hObject = hFindedActor->GetObjectHandle();
		ActorToPlay.fRotYDegree = m_pCutSceneData->GetRegResRot( ActorToPlay.strActorName.c_str() );
		EtMatrixRotationY( &ActorToPlay.matRotY, ActorToPlay.fRotYDegree*ET_PI / 180.0f );
		ActorToPlay.vPos = m_pCutSceneData->GetRegResPos( ActorToPlay.strActorName.c_str() );
		hFindedActor->GetCachedAniIndexList( ActorToPlay.vlAniIndex );
		ActorToPlay.iNowAniIndex = 0;

		// 동영상이 재생되는 동안은 액터들의 컬링을 끈다.
		if( ActorToPlay.hObject ) 
			ActorToPlay.hObject->EnableCull( false );

		if( ActorToPlay.hMergedObject ) 
			ActorToPlay.hMergedObject->EnableCull( false );

		// 액션을 stand 로 바꿔준다. 스킬 등등의 시그널이 엮여서 카메라 효과가 나오거나 하지 않도록.
		// 컷신에서는 따로 액션을 바꾸는게 아니라 AniFrame 을 직접 조절하기 때문에 기존 액션의 시그널이 처리되지 않도록 stand 를 바꿔준다.
		hFindedActor->CmdAction( "Stand" );

		// 컷신 나오는 동안엔 액터의 STE_Input/STE_InputHasPassiveSkill 을 처리하지 않는다.
		//hFindedActor->SetPlayingCutScene( true );

		// 컷신 나오는 동안엔 액터에 박힌 사운드 시그널을 재생하지 않는다.
		hFindedActor->SetPlaySignalSound( m_pCutSceneData->GetSceneInfo()->bUseSignalSound );

		m_pActorProcessor->AddObjectToPlay( &ActorToPlay );
	}
}


void CDnCutSceneTask::StartPlay( void )
{
	if( NULL == m_pActorProcessor )
		return;

	m_CutScenePlayer.StartPlay( m_LocalTime );

	GetInterface().ShowCutSceneBlindDialog( true );

	// 닉네임 찍는 거 숨기자
	int iNumActors = (int)m_dqhLiveActors.size();
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		DnActorHandle hActor = m_dqhLiveActors.at( iActor );
		hActor->ShowHeadName( false );
	}

	// 플레이어 액터는 컷신 시작할때 무기를 들고 있는지 넣고 있는지, 어떤 상태인지 모른다.
	// 컷신에 등장하는 플레이어 액터들의 무기 상태를 컷신 시작시의 액션을 근거로 맞춰준다.
	// 컷신이 끝나면 원래대로 복구 시켜 줌.
	int iNumActor = m_pActorProcessor->GetNumActors();
	m_vlIsPlayerBattleMode.assign( iNumActor, 0 );
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		S_CS_ACTOR_INFO* pActorInfo = m_pActorProcessor->GetActorInfoByIndex( iActor );
		
		CDnActor* pDnActor = static_cast<CDnActor*>(pActorInfo->pActor);
		CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(pDnActor);
		if( pPlayerActor && false == pPlayerActor->IsDie() )
		{
			// 첫번째 액션이 "Normal" 이 붙지 않은 액션은 다 공격 액션임
			// 현재 유저의 상태가 어떠냐를 불문하고 Normal 붙은 액션이면 무기 빼주고 
			// 아니라면 공격상태로 바꿔준다.
			const ActionInfo* pActionInfo = m_pCutSceneData->GetThisActorsActionInfoByIndex( pActorInfo->strActorName.c_str(), 0 );

			size_t iFound = pActionInfo->strAnimationName.find( "Normal" );
			if( iFound == string::npos )
			{
				pPlayerActor->SetBattleMode( true );
				m_vlIsPlayerBattleMode.at( iActor ) = 1;
			}
			else
			{
				pPlayerActor->SetBattleMode( false );
			}

			// 얼굴을 평상시로 바꿔줌.
			switch( pPlayerActor->GetActorType() )
			{
				case CDnActorState::Warrior:
					pPlayerActor->SetFaceAction( "Warrior" );
					break;

				case CDnActor::Archer:
					pPlayerActor->SetFaceAction( "Archer" );
					break;

				case CDnActor::Soceress:
					pPlayerActor->SetFaceAction( "Soceress" );
					break;

				case CDnActor::Cleric:
					pPlayerActor->SetFaceAction( "Cleric" );
					break;

#ifdef PRE_ADD_ACADEMIC
				case CDnActor::Academic:
					pPlayerActor->SetFaceAction( "Academic" );
					break;
#endif //#ifdef PRE_ADD_ACADEMIC

#ifdef PRE_ADD_KALI
				case CDnActor::Kali:
					pPlayerActor->SetFaceAction( "Kali" );
					break;
#endif // #ifdef PRE_ADD_KALI
//[OK]
#ifdef PRE_ADD_ASSASSIN
				case CDnActor::Assassin:
					pPlayerActor->SetFaceAction( "Assassin" );
					break;
#endif 
#ifdef PRE_ADD_LENCEA
				case CDnActor::Lencea:
					pPlayerActor->SetFaceAction( "Lencea" );
					break;
#endif 
#ifdef PRE_ADD_MACHINA
				case CDnActor::Machina:
					pPlayerActor->SetFaceAction( "Machina" );
					break;
#endif 
					// #ifdef PRE_ADD_ASSASSIN
			}

			// Note: 현재 상태효과 먹은 상태로 컷신 나오면 플레이어 캐릭터의 색깔이 바뀌어서 나올 수 있음.
			//pPlayerActor->RemoveAllBlow();

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
			// 플레이어인 경우엔 액션에 딸린 이펙트 시그널 등등 리소스 재구성.
			pPlayerActor->RecomposeAction();
#endif // #ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		}
	}

	CEtSoundEngine::GetInstance().FadeVolume( "NULL", 1.f, CGlobalInfo::GetInstance().m_fFadeDelta, false );
}



void CDnCutSceneTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
//#if defined(TEST_CUTSCENE)
//	if( m_bFirstLoop )
//	{
//		StartPlay();
//		m_bFirstLoop = false;
//	}
//#endif

	//if( m_CutScenePlayer.IsEndScene() )
	//{
	//	delete this;
	//	return;
	//}

	// Process Input Device - 대화창 처리
	CInputDevice::GetInstance().Process( LocalTime, fDelta );

	MatrixEx matEx;
	EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
	if( hCamera )
		matEx = *hCamera->GetInvViewMat();
	else {
		DnCameraHandle hGameCam = CDnCamera::GetActiveCamera();
		if( hGameCam ) matEx = *hGameCam->GetMatEx();
	}

	if( hCamera )
	{
		m_pWorld->Update( matEx.m_vPosition, matEx.m_vZAxis, hCamera->GetCameraFar() * 1000.f );

		// #38660 카메라를 못 갖고 오는 경우가 있어 이런 경우엔 플레이어 카메라를 직접 가져오도록 수정.
		if( !m_CutScenePlayer.GetCamera() )
			m_CutScenePlayer.SetCamera( hCamera );
	}

	// Etc Object Process
	CDnEtcObject::ProcessClass( LocalTime, fDelta );

	m_pWorld->Process( LocalTime, fDelta );

	m_CutScenePlayer.Process( LocalTime );

	// 카메라의 위치를 Listner 로 셋팅해줌

	// Process Sound
	if( CEtSoundEngine::IsActive() ) 
	{
		CEtSoundEngine::GetInstance().Process( fDelta );
		CEtSoundEngine::GetInstance().SetListener( matEx.m_vPosition, matEx.m_vZAxis, matEx.m_vYAxis );
	}

	// Note : UI Process
	EtInterface::Process( fDelta );

	// 현재 컷신에서 프로젝타일 나가는 건 고려하지 않는다.
	// Weapon(Projectile) Process
	//CDnWeapon::ProcessClass( LocalTime, fDelta );

	EternityEngine::ClearScreen( 0xFFFFFFFF, 1.0f, 0, false );
	EternityEngine::RenderFrame( fDelta );
	EtInterface::Render( fDelta );
	EternityEngine::ShowFrame( NULL );
}

void CDnCutSceneTask::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime /* = -1  */ )
{
	if( !m_bRequestSkip ) {
		if( (nReceiverState & IR_KEY_DOWN) 
#ifdef PRE_ADD_CUTSCENE_PADSKIP
			|| (nReceiverState & IR_JOYPAD_DOWN)
#endif
			)
		{
			if( IsPushKey( DIK_ESCAPE ) 
#ifdef PRE_ADD_CUTSCENE_PADSKIP
				|| IsPushJoypadButton( IW_UI(IW_SYSTEM) )
#endif
				) {
				CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
//				if( CDnWorld::IsActive() && CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage ) {
				if( pTask ) {
					if( pTask->IsControlQuestScriptCutScene() )
						pTask->SkipCutScene();
					else {
						// 운영자는 스킵할 수 없다.
						if( CDnPartyTask::IsActive() ) {
							CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyDataFromSessionID( CDnBridgeTask::GetInstance().GetSessionID() );
							if( pStruct && pStruct->bGMTrace )
								return;
						}
						m_bRequestSkip = true;
						SendSkipCutScene( CDnBridgeTask::GetInstance().GetSessionID() );
					}
				}
			}
		}
	}
}

void CDnCutSceneTask::FadeOutSkip()
{
	if( m_bSetFadeModeCalledForSkip )
		return;

	CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
	m_pSkipFade->SetCallback( pTask );
	m_pSkipFade->SetFadeColor( 0x00000000, 0xff000000, true );
	m_pSkipFade->SetFadeTime( 1.f );
	m_pSkipFade->SetFadeMode( CDnFadeInOutDlg::modeBegin );

	m_bSetFadeModeCalledForSkip = true;
}

void CDnCutSceneTask::Finalize( void )
{
	if( NULL == m_pActorProcessor )
		return;

	if( !m_CutScenePlayer.IsEndScene() ) m_CutScenePlayer.StopPlay();
//	CDnInterface::GetInstance().UseEventMessage( true );
	// 숨겼던 아이템 다시 보여줌
	CDnDropItem::ShowDropItems( true );

	// 컷신에서 생성했던 이펙트들 모두 제거
	int iNumActor = m_pActorProcessor->GetNumActors();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		CDnActor* pActor = static_cast<CDnActor*>(m_pActorProcessor->GetActorInfoByIndex( iActor )->pActor);
		pActor->ReleaseSignalImp();

		// 플레이어 액터 무기 상태 복구
		CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(pActor);
		if( pPlayerActor && m_vlIsPlayerBattleMode.at(iActor) == 1 )
		{
			//pPlayerActor->SetActionQueue( "PullOut_Weapon" );
			pPlayerActor->SetBattleMode( true );
			for( int j=0; j<2; j++ ) {
				pPlayerActor->ShowWeapon( j, true );
				pPlayerActor->ShowCashWeapon( j, true );
			}
		}

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		// 플레이어인 경우엔 액션에 딸린 이펙트 시그널 등등 리소스 재구성.
		if( pPlayerActor )
		{
			static_cast<CDnPlayerActor*>(pActor)->RecomposeAction();
		}
#endif // #ifdef PRE_FIX_CLIENT_MEMOPTIMIZE

		if( pPlayerActor ) pPlayerActor->ResetHideWeaponBySignalInCutScene();
	}

	int iNumMonster = (int)m_vlhDeadMonster.size();
	for( int iMonster = 0; iMonster < iNumMonster; ++iMonster )
	{
		DnActorHandle hDeadMonster = m_vlhDeadMonster.at( iMonster );
		hDeadMonster->SetHP( 0 );
		if( !hDeadMonster->IsShow() ) hDeadMonster->Show( true );
		hDeadMonster->Show( false );
		hDeadMonster->ShowWeapon( 0, false );
		hDeadMonster->ShowWeapon( 1, false );
	}
	 
	// 제외된 액터들, 포함된 액터들 중에 몬스터 액터 죽은 녀석들은 신에서 썼던 놈이라도 숨긴다! 
	// 안그러면 컷신 끝나고 그냥 남아있게 됨

	// 제외 되었던 액터들을 다시 나타내줌
	int iNumExceptActor = (int)m_dqhExceptActors.size();
	for( int iActor = 0; iActor < iNumExceptActor; ++iActor )
	{
		DnActorHandle hActor = m_dqhExceptActors.at( iActor );
		if( !hActor ) continue;
		if( !hActor->IsPlayerActor() && hActor->IsDie() ) continue;

		if( hActor->IsShow() ) hActor->Show( false );
		hActor->Show( true );

		// 숨겨졌던 액터는 건드려지지 않으므로..
		//bool bPlayerCharacter = hActor->GetActorType() <= CDnActorState::Reserved6;
		//if( bPlayerCharacter )
		//{
		//	// 플레이어 액터인 경우 전투 모드였으면 무기 붙는 본이 달라졌을 경우가 있으므로 배틀모드로 셋팅
		//	CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
		//	if( pPlayerActor->IsBattleMode() )
		//		pPlayerActor->SetBattleMode( true );
		//}
	}

	// 컬링 및 위치 복구
	int iNumLiveActors = (int)m_dqhLiveActors.size();
	for( int iActor = 0; iActor < iNumLiveActors; ++iActor )
	{
		DnActorHandle hActor = m_dqhLiveActors.at( iActor );
		if( !hActor ) continue;
		if( !hActor->IsPlayerActor() && hActor->IsDie() ) 
		{
			// 죽은 몬스터가 혹시 컷신에서 쓰였을 수 있고, 죽은 후 처리가 
			// 안되어서 남아있는 경우가 있으므로 죽은 애는 바로 숨김.
			if( !hActor->IsShow() ) hActor->Show( true );
			hActor->Show( false );
			continue;
		}

		if( hActor->GetObjectHandle() )
			hActor->GetObjectHandle()->EnableCull( true );

		if( hActor->IsPlayerActor() )
		{
			EtAniObjectHandle hMergedObject = static_cast<CDnPlayerActor*>(hActor.GetPointer())->GetCombineObject();
			if( hMergedObject )
				hMergedObject->EnableCull( true );
		}
		hActor->ShowHeadName( true );
		//hActor->SetPlayingCutScene( false );
		hActor->SetPlaySignalSound( true );
		if( hActor->IsShow() )  hActor->Show( false );
		hActor->Show( true );
		*(hActor->GetMatEx()) = m_vlOriMatExWorlds.at( iActor );
		hActor->ResetMove();
		hActor->ResetLook();
		hActor->SetStaticPosition( *hActor->GetPosition() );

		// 컷신 플레이어에서 ani distance 를 직접 계산해서 넣어줌.
		// 월드 매트릭스에 스케일 값이 들어갈 수가 있으므로 엔진에서 애니메이션 계산시 
		// 디폴트로 켜져 있는 Y 축 ani distance 에 적용되어 나오기 때문이다.
		// 현재는 몬스터 스케일에 관계없이 애니메이션 키 값에 영향이 없게 하도록 결정되어있다.
		if( hActor->GetAniObjectHandle() )
			hActor->GetAniObjectHandle()->SetCalcPositionFlag( CALC_POSITION_Y );

		CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
		if( pPlayer )
		{
			pPlayer->SetBattleMode( m_bVecPlayerBattleMode.at( iActor ) );

			// Rotha 컷신 재생중에 말타고잇으면 안되므로 내려주는데 , 컷신끝날때는 다시태워준다 , VehicleInfo 기반으로 말을 타기때문에 , 안탄상황도 걱정없다.
			if(pPlayer && !pPlayer->IsDestroy() && !pPlayer->IsDie() && pPlayer->IsCanVehicleMode() && (pPlayer->GetVehicleInfo().Vehicle[Vehicle::Slot::Body].nItemID != 0))
				pPlayer->RideVehicle(pPlayer->GetVehicleInfo(),true);
		}

#ifdef PRE_ADD_CASH_COSTUME_AURA 
		if( pPlayer )
		{
			if( pPlayer->IsEnableCostumeAura() )
			{
				pPlayer->LoadCostumeAuraEffect(true);
			}
		}
#endif

		// #71465 -【그 외】할로윈 펌프킨 정령의 그래픽이 이벤트 무비에서 표시되지 않는다.
		if( pPlayer )
		{
			DnPartsHandle hParts = pPlayer->GetCashParts( CDnParts::PartsTypeEnum::CashFairy );
			if( hParts )
				hParts->ShowRenderBase( true );
		}


		if( !hActor->IsDie() ) 
		{
			if( hActor->IsNpcActor() )
			{
				int iActionIndex = hActor->GetElementIndex( m_szOriAction.at( iActor ).c_str() );
				hActor->SetActionIndex( iActionIndex );
			}
				//hActor->SetActionQueue( m_szOriAction.at( iActor ).c_str() );
			else
			{
				hActor->CmdStop( m_szOriAction.at( iActor ).c_str() );
				// 컷신 플레이어에서 액션 인덱스를 직접 셋팅하기 때문에 ActionQueue 로 예약된 액션이 무시가 되어버림.
				// 그래서 현재 액션 곧바로 셋팅. (#14014)
				hActor->SetAction( m_szOriAction.at( iActor ).c_str(), 0.0f, 0.0f );
			}

			hActor->ResetStateEffects();
		}
	}

	// 추가 로드했던 액터들 해제
	int iAdditionalLoadActors = (int)m_vlhAdditionalLoadedActors.size();
	for( int iActor = 0; iActor < iAdditionalLoadActors; ++iActor ) {
		DnActorHandle hActor = m_vlhAdditionalLoadedActors.at( iActor );
		//		hActor->Show( false );
		hActor->ShowWeapon( 0, false );
		hActor->ShowWeapon( 1, false );
		if( !hActor ) continue;
		SAFE_RELEASE_SPTR( hActor );
	}

	// 액션 프랍이 취하고 있던 액션을 복구함
	int iNumActProp = (int)m_vlActPropDefault.size();
	for( int iActProp = 0; iActProp < iNumActProp; ++iActProp )
	{
		S_ACT_PROP_DEFAULT& PropDefault = m_vlActPropDefault.at( iActProp );
		if( PropDefault.hProp ) 
		{
			PropDefault.pActProp->SetAction( PropDefault.strDefaultAction.empty() ? "" : PropDefault.strDefaultAction.c_str(), 0.0f, 0.0f );
			PropDefault.hProp->Show( PropDefault.bShow );

			// FinalTest 에서 hObject 가 invalid 한 경우가 있는 거 같아서 방어코드 추가.
			EtAniObjectHandle hObject = PropDefault.hProp->GetObjectHandle();
			if( hObject )
				hObject->ShowObject( PropDefault.bShow );
		}
	}

	// 카메라 복구
	DnCameraHandle hDnCamera = CDnCamera::GetActiveCamera();
	hDnCamera->SetCameraInfo( m_OriginalCameraInfo);

	// DOF 값 복구
	if( m_pDOFFilter )
	{
		m_pDOFFilter->SetNearDOFStart( m_fNearStart );
		m_pDOFFilter->SetNearDOFEnd( m_fNearEnd );
		m_pDOFFilter->SetFarDOFStart( m_fFarStart );
		m_pDOFFilter->SetFarDOFEnd( m_fFarEnd );
		m_pDOFFilter->SetFocusDistance( m_fFocusDist );
		m_pDOFFilter->SetNearBlurSize( m_fNearBlurSize );
		m_pDOFFilter->SetFarBlurSize( m_fFarBlurSize );
	}

	if( hDnCamera ) {
		// 플레이어에게 Attach 시켜준다.
		if( hDnCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
			CDnPlayerCamera* pPlayerCamera = static_cast<CDnPlayerCamera*>(hDnCamera.GetPointer());
			
			if(CDnActor::s_hLocalActor)
			{
				CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

				if(pPlayer && pPlayer->IsVehicleMode() && pPlayer->GetMyVehicleActor())
				{
					pPlayerCamera->AttachActor(pPlayer->GetMyVehicleActor()->GetActorHandle());
				}
				else
				{
					pPlayerCamera->AttachActor( CDnActor::s_hLocalActor );
				}
			}
		}
	}

	GetInterface().ShowCutSceneBlindDialog( false );

	bool bVillage = (CDnWorld::MapTypeEnum::MapTypeVillage == m_pWorld->GetMapType());
	if( false == bVillage )
	{
		CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
		DWORD dwNumParty = pPartyTask->GetPartyCount();
		for( DWORD dwMember = 0; dwMember < dwNumParty; ++dwMember )
		{
			CDnPartyTask::PartyStruct* pParty = pPartyTask->GetPartyData( dwMember );
			CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(pParty->hActor.GetPointer());
			if( pPlayerActor->IsGhost() )
			{
				pPlayerActor->SwapSingleSkin( 499 + pPlayerActor->GetClassID() );
				pPlayerActor->ResetCustomAction();
				pPlayerActor->ResetMixedAnimation( false );

				pPlayerActor->SetHP( 0 );

				if( CDnActor::s_hLocalActor == pParty->hActor )
				{
					CDnWorldEnvironment* pWorldEnv = CDnWorld::GetInstance().GetEnvironment();
					pWorldEnv->ControlColorFilter( CDnWorldEnvironment::Saturation, 0.0f, 500 );
				}

				CDnActor::s_hLocalActor->CmdAction( "Stand" );
			}
		}
	}
	else
	// 로컬 액터에게 "Stand" 액션을 커맨드로 줌
	if( CDnActor::s_hLocalActor ) 
	{
		CDnActor::s_hLocalActor->CmdAction( "Stand" );
	}

	//if( CDnActor::s_hLocalActor )
	//	CDnActor::s_hLocalActor->SetPlayingCutScene( false );
	CGlobalInfo::GetInstance().SetPlayingCutScene( false );

	SAFE_DELETE( m_pActorProcessor );
	CDnWorld::GetInstance().EnableTriggerEventCallback( true );

	if( m_bAutoFadeIn )
		CEtSoundEngine::GetInstance().FadeVolume( NULL, 1.f, CGlobalInfo::GetInstance().m_fFadeDelta, false );

	CInputDevice::GetInstance().HoldAbsenceCheckTimer(CInputDevice::eABSENCE_PARTY, false);

	if( m_bCheatCutScene )
	{
		FadeOutSkip();
	}
	
#ifdef PRE_FIX_SUMMON_ACTOR_HPBAR 
	if(!m_dqhLiveActors.empty())
	{
		for(int i = 0 ; i < (int)m_dqhLiveActors.size() ; ++i)
		{
			DnActorHandle hActor = m_dqhLiveActors.at(i);
			if(!hActor) continue;
			if(hActor->IsDie()) continue;
			GetInterface().ShowEnemyGauge( hActor, true );
		}
	}
#endif // PRE_FIX_SUMMON_ACTOR_HPBAR
}

bool CDnCutSceneTask::RenderScreen( LOCAL_TIME LocalTime, float fDelta )
{
	return !m_bLoadingComplete;
}


bool CDnCutSceneTask::AttachItem( int nItemID, int nEnchantLevel, int nSubArg1, int nLookItemID )
{
	if( !m_hActor ) return true;

	
	CDnPlayerActor *pActor = (CDnPlayerActor *)m_hActor.GetPointer();
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
	if( !pSox ) return false;
	if( !pSox->IsExistItem( nItemID ) ) return false;

	eItemTypeEnum ItemType = CDnItem::GetItemType( nItemID );
	if( ItemType == ITEMTYPE_HAIRDYE )
	{
		DWORD dwColor = (DWORD)pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
		pActor->SetPartsColor( MAPartsBody::HairColor, dwColor );
	}
	else if( ItemType == ITEMTYPE_EYEDYE )
	{
		DWORD dwColor = (DWORD)pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
		pActor->SetPartsColor( MAPartsBody::EyeColor, dwColor );
	}
	else if( ItemType == ITEMTYPE_SKINDYE )
	{
		DWORD dwColor = (DWORD)pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
		pActor->SetPartsColor( MAPartsBody::SkinColor, dwColor );
	}
	else if( ItemType == ITEMTYPE_PARTS )
	{
		DnPartsHandle hParts = CDnParts::CreateParts( nItemID, 0, 0, 0, 0, 0, false, nLookItemID);
		if( hParts )
		{
			//hParts->SetRTTRenderMode( true, this );
			bool bCash = CDnItem::IsCashItem( nItemID );

			// 액터를 그대로 쓰는거기 때문에 별다른 처리 안해도 될 줄 알았는데,
			// 속도때문에 Combine하지 않고 처리해야하다보니, 한벌옷 관련해서 문제가 있었다.
			// Combine할때의 한벌옷 처리는 모두 다 되어있지만, Combine하지 않을때의 한벌옷 처리는 안되있는 것.
			// 이건 Combine하지 않는 로그인 캐릭터 선택화면에서도 마찬가지 문제다.
			// 대신 로그인 화면에서는 일반적인 캐시옷들 쭉 입혀본 후 한벌옷을 입힌다거나,(그 반대의 경우. 한벌옷 입은 후 일반적인 캐시파츠 장착)
			// 하지 않기때문에 별 문제 없었던 것이다.
			//
			// 현재 Combine하지 않기때문에 새로운 파츠를 장착할때 예전 정보를 따로 기억해두지 않고 지우는데,
			// 이로 인해 한벌옷장착된 상태에서 다른 캐시상의파츠를 AttachItem할때 예전에 입던 하의, 장갑, 바지의 정보가 없다는 것이다.
			//
			// 이걸 방지하려면, 캐릭터 장비템 정보를 들고있는 것처럼 노말파츠, 캐시파츠 다 구분해서 기억해둬야하는데,
			// 이건 아이템태스크의 장비기능을 다 빼오는 격이 되버린다.
			// 최대한 렌더링루틴쪽으로만 건드리려했던 렌더아바타코드라,(그래서 AttachItem은 지원해도 DetachItem은 일부러 뺀거다.)
			// 이 기능은 지원하지 않기로 한다.
			//
			// 그래도 최대한 자연스럽게 표현하기 위해 한벌옷에 대해서만 예외처리해서 정보 들고있도록 하겠다.

			if( !bCash )
			{
				// 렌더아바타에서는 CombineParts 를 안하기때문에 노말템 들어올 경우 매칭되는 캐시템을 검사해
				// 캐시템이 있다면 장착하지 않고 넘긴다.
				bool bEquip = true;
				if( hParts->GetPartsType() >= CDnParts::Helmet && hParts->GetPartsType() <= CDnParts::Foot )
				{
					if( pActor->GetCashParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() - 2 ) ) )
					{
						bEquip = false;
					}
				}

				//// 혹시 장착된 한벌옷 부위인지 확인.(보니까 한벌옷 서브파츠들이 이쪽으로 들어오는듯?)
				//for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
				//{
				//	if( m_vecPartsInfo[i].nType == hParts->GetPartsType() )
				//	{
				//		bEquip = false;
				//		break;
				//	}
				//}

				if( bEquip )
				{
					pActor->DetachParts( hParts->GetPartsType() );
					pActor->AttachParts( hParts, hParts->GetPartsType(), true, false );
					if( hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_SCREEN );

					if( hParts->GetPartsType() == CDnParts::Hair )
					{
						// 머리카락의 경우에 툴에서 알파블렌딩이 설정 안되있을때가 있다. 그냥 강제로 설정한다.
						if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
							hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
					}
				}
				else
				{
					SAFE_RELEASE_SPTR( hParts );
				}
			}
			else
			{
				// 만약 한벌옷 입고 있었던 중이라면,
				//if( m_vecPartsInfo.size() )
				//{
				//	// 현재 장착하는 파츠가 한벌옷에 해당되는 부위인지 확인한다.
				//	// 지금은 모든 한벌옷이 다 같은 서브파츠를 가지고 있지만, 만약 이게 달라진다면,
				//	// hParts->GetPartsType()만 검사하는게 아니라, 서브파츠까지 다 꺼내와서 검사해야할거다.
				//	bool bDetachOnepiece = false;
				//	for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
				//	{
				//		if( (m_vecPartsInfo[i].nType - 2) == hParts->GetPartsType() )
				//		{
				//			bDetachOnepiece = true;
				//			break;
				//		}
				//	}

				//	if( bDetachOnepiece )
				//	{
				//		std::vector<SPartsInfo> vecPartsInfo;
				//		vecPartsInfo = m_vecPartsInfo;
				//		m_vecPartsInfo.clear();

				//		// 먼저 한벌옷을 해제하고,(첫번째 인덱스가 한벌옷 파츠 부위다.)
				//		pActor->DetachCashParts( (CDnParts::PartsTypeEnum)( vecPartsInfo[0].nType - 2 ) );

				//		// 예전 정보로 다 돌린 후
				//		// 먼저 타입을 다 넣어뒀으니 해당하는 타입에 맞는 아이템ID만 가져와 재저장하면 된다.
				//		for( int i = 0; i < (int)vecPartsInfo.size(); ++i )
				//			AttachItem( vecPartsInfo[i].nItemID );

				//		// 일반적인 캐시템 장착하듯이 아래 루틴 쭉 수행.
				//	}
				//}

#ifdef PRE_ADD_TRANSPARENCY_COSTUME
				bool bTransparentSkin = CDnParts::IsTransparentSkin( hParts->GetClassID(), pActor->GetClassID() );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME

				// 입으려는 옷이 한벌옷이라면, 한벌옷에 해당되는 자리의 파츠들을 별도로 기억해둔다.
				// 서브파츠만 하면 자신슬롯이 빠지니 자신도 따로 추가해둔다.
				// (이걸 기억해둬야 한벌옷 입고 있다가 캐시장갑 입을때 이전에 입던 상의로 되돌릴 수 있다.)
				if( hParts->IsExistSubParts() )
				{
					SPartsInfo Info;
					Info.nType = (int)hParts->GetPartsType() + 2;	// 한벌옷은 캐시템이니 +2해서 노말템 타입으로 맞춘다.
					Info.nItemID = 0;
					m_vecPartsInfo.push_back( Info );
					for( int i = 0; i < hParts->GetSubPartsCount(); ++i )
					{
						Info.nType = (int)hParts->GetSubPartsIndex(i) + 2;	// 서브타입 적힌것도 캐시템의 타입이니 +2해서 노말템 타입으로 맞춘다.
						m_vecPartsInfo.push_back( Info );
					}

					// 먼저 타입을 다 넣어뒀으니 해당하는 타입에 맞는 아이템ID만 가져와 재저장하면 된다.
					for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
					{
						if( pActor->GetCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) ) )
						{
							m_vecPartsInfo[i].nItemID = pActor->GetCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) )->GetClassID();
							pActor->DetachCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) );
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
							// 투명한벌옷으로 입혀야할때는 일반파츠모습을 보여줘야한다.
							if( bTransparentSkin )
							{
								DnPartsHandle hNormalParts = pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) );
								if( hNormalParts && !hNormalParts->GetObjectHandle() )
								{
									DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );
#ifdef PRE_FIX_MEMOPT_EXT
									std::string szAniName;
									CommonUtil::GetFileNameFromFileEXT(szAniName, pActorSox, pActor->GetClassID(), "_AniName");
#else
									std::string szAniName = pActorSox->GetFieldFromLablePtr( pActor->GetClassID(), "_AniName" )->GetString();
#endif
									hNormalParts->CreateObject( m_hActor, szAniName.c_str() );
								}
							}
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
						}
						else if( pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) ) )
						{
							m_vecPartsInfo[i].nItemID = pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) )->GetClassID();
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
							if( !bTransparentSkin ) pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) )->FreeObject();
#else
							pActor->DetachParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
						}
						// 원래 제대로 액터쪽에서 디태치처리 되었다면 빈 슬롯이더라도 디폴트파츠 다 들어있을 것이다.
						// 그래도 혹시 모르니 아래 처리 해둔다.
						else if( m_vecPartsInfo[i].nType >= CDnParts::DefaultPartsType_Min && m_vecPartsInfo[i].nType <= CDnParts::DefaultPartsType_Max )
						{
							m_vecPartsInfo[i].nItemID = pActor->GetDefaultPartsInfo( (CDnParts::PartsTypeEnum)m_vecPartsInfo[i].nType );
						}
					}
				}

				// 렌더아바타에서는 CombineParts 를 안하기때문에 여기서 노말템을 빼주는식으로 합니다.
				if( hParts->GetPartsType() >= CDnParts::CashHelmet && hParts->GetPartsType() <= CDnParts::CashFoot )
				{
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
					// 투명 한벌옷때문에 Detach하지 않고 FreeObject하는걸로 바꾼다. 그래야 나중에 필요할때 다시 CreateObject할 수 있다.
					DnPartsHandle hNormalParts = pActor->GetParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) );
					if( hNormalParts && !bTransparentSkin ) hNormalParts->FreeObject();
#else
					if( pActor->GetParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) ) )
						pActor->DetachParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
				}

				if( nSubArg1 == 0 )
				{
					pActor->DetachCashParts( hParts->GetPartsType() );
					pActor->AttachCashParts( hParts, hParts->GetPartsType(), true, false );
				}
				else if( nSubArg1 == 1 )
				{
					if( hParts->GetPartsType() == CDnParts::CashRing )
					{
						pActor->DetachCashParts( CDnParts::CashRing2 );
						pActor->AttachCashParts( hParts, CDnParts::CashRing2, true, false );
					}
				}
				//if( hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
				else if( bTransparentSkin )
				{
					DnPartsHandle hNormalParts = pActor->GetParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) );
					if( hNormalParts && !hNormalParts->GetObjectHandle() )
					{
						DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );
#ifdef PRE_FIX_MEMOPT_EXT
						std::string szAniName;
						CommonUtil::GetFileNameFromFileEXT(szAniName, pActorSox, pActor->GetClassID(), "_AniName");
#else
						std::string szAniName = pActorSox->GetFieldFromLablePtr( pActor->GetClassID(), "_AniName" )->GetString();
#endif
						hNormalParts->CreateObject( m_hActor, szAniName.c_str() );
					}
				}
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME

				// 우선 파츠에 대해서만 한다. 웨폰은 이런 경우 거의 없는 듯.
				if( hParts->GetPartsType() == CDnParts::CashHelmet )
				{
					// 머리장식 중 일부 파츠들엔 메시끝부분에 알파텍스처를 사용하는데도 알파블렌딩이 툴에서 설정 안되있는 경우가 있다.
					// 이렇게 해도 제대로 나오도록 강제로 활성화시킨다.
					// 하지만 보니까, PostProcess에서 알파값 그대로 유지하도록 처리하고나선,
					// 테두리 알파 이상하게 보이던 문제가 거의 해결된거 같다.
					// 그래서 강제로 알파블렌드 켜는걸 해제하도록 하겠다.
					//if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
					//	hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
				}
				else if( hParts->GetPartsType() == CDnParts::CashWing )
				{
					// 날개의 경우엔 강제로 알파블렌드, 투패스 켜서 깔끔하게 나오게 한다.
					if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
					{
						hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
						hParts->GetObjectHandle()->GetSkinInstance()->SetRenderAlphaTwoPass( true );
					}
				}
			}
		}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		// 로그인 캐릭터 선택화면과 같이 CombineParts 를 안하기때문에 워리어 건틀렛꼈을때 오른쪽 장갑메시 하이드 하는 것을 강제로 호출한다.
		pActor->OnCombineParts();
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
	}
	else if( ItemType == ITEMTYPE_WEAPON )
	{
		DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nItemID, 0, 0, nEnchantLevel, 0, 0, false, false, true, ITEMCLSID_NONE );
		if( hWeapon )
		{			
			bool bCash = CDnItem::IsCashItem( nItemID );

			int nEquipIndex = CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX( hWeapon->GetEquipType() ) - EQUIP_WEAPON1;
			ASSERT( nEquipIndex == 0 || nEquipIndex == 1 );

			if( !bCash )
			{
				pActor->DetachWeapon( nEquipIndex );
				pActor->AttachWeapon( hWeapon, nEquipIndex, true );
			}
			else
			{
				pActor->DetachCashWeapon( nEquipIndex );
				pActor->AttachCashWeapon( hWeapon, nEquipIndex, true, false );
			}

			if( hWeapon->GetObjectHandle() )
			{
				// 일반메이스와 캐시프레일을 조합할 경우가 문제다.
				// 메이스애니를 비롯해서 메이스 액트파일까지 새로 만들어야하는 등, 리소스단에서 할일이 너무 많아지므로 강제로 시뮬레이션 안쓰는거 하지 않는다.
				//hWeapon->GetObjectHandle()->DisableSimulation();
				//hWeapon->GetObjectHandle()->SetRenderType( RT_TARGET );
			}
		}
	}
	// 얼굴상품이랑 머리모양 상품.
	else if( ItemType == ITEMTYPE_FACIAL || ItemType == ITEMTYPE_HAIRDRESS )
	{
		int nParam = pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
		AttachItem( nParam );

		// 알파 객체들을 거리에 따라 정렬하지 않기 때문에, Hair바꿀경우 캐시모자를 다시 장착시켜 Hair보다 더 뒤에 렌더링되게 한다.
		if( ItemType == ITEMTYPE_HAIRDRESS )
		{
			if( pActor->GetCashParts( CDnParts::CashHelmet ) )
			{
				DnPartsHandle hHelmet = pActor->GetCashParts( CDnParts::CashHelmet );
				if (!hHelmet)
				{
#ifdef _WORK
					std::wstring str = FormatW(L"DnRenderAvatar::AttachItem - Character CashHelmet does not exist : (%d)", nItemID);
					GetInterface().AddChatMessage(CHATTYPE_NOTICE, L"", str.c_str(), true);
#endif
					return false;
				}

				int nAttachHelmetId = (hHelmet->GetLookItemID() != ITEMCLSID_NONE) ? hHelmet->GetLookItemID() : hHelmet->GetClassID();
				pActor->DetachCashParts( CDnParts::CashHelmet );
				AttachItem(nAttachHelmetId);
			}
		}
	}
	

	return true;
}



void CDnCutSceneTask::AddClonePlayer( DnActorHandle & hFindedActor )
{	
	CDnPlayerActor * pLocalPlayer = static_cast< CDnPlayerActor * >( CDnActor::s_hLocalActor.GetPointer() );
	DnActorHandle hClonePlayer = CreateActor( CDnActor::s_hLocalActor->GetClassID(), false, false, false );
	if( hClonePlayer )
	{
		m_hActor = hClonePlayer;
		hClonePlayer->Show( false );
		hClonePlayer->Initialize();

		CDnPlayerActor * pClonePlayerActor = static_cast< CDnPlayerActor * >( hClonePlayer.GetPointer() );
		pClonePlayerActor->SetWeaponViewOrder( 0, true );
		pClonePlayerActor->SetWeaponViewOrder( 1, true );						
		for( int i=0; i<CDnParts::SwapParts_Amount; ++i )
		{
			//pClonePlayerActor->SetPartsViewOrder( i, true );
			//pClonePlayerActor->SetHideHelmet( false );
			pClonePlayerActor->SetPartsViewOrder( i, pLocalPlayer->IsViewPartsOrder( i ) );

			int nDefaultPartsInfo[ 4 ];
			for( int i=0; i<4; ++i )
				nDefaultPartsInfo[ i ] = pLocalPlayer->GetDefaultPartsInfo( (CDnParts::PartsTypeEnum)(CDnParts::Body + i) );

			pClonePlayerActor->SetDefaultPartsInfo( nDefaultPartsInfo );
			pClonePlayerActor->SetPartsColor( MAPartsBody::HairColor, pLocalPlayer->GetPartsColor( MAPartsBody::HairColor) );
			pClonePlayerActor->SetPartsColor( MAPartsBody::EyeColor, pLocalPlayer->GetPartsColor( MAPartsBody::EyeColor) );
			pClonePlayerActor->SetPartsColor( MAPartsBody::SkinColor, pLocalPlayer->GetPartsColor( MAPartsBody::SkinColor) );
			pClonePlayerActor->SetBattleMode( false );
	
			for( int i=0; i<CDnParts::PartsTypeEnum_Amount; ++i )
			{
				DnPartsHandle hParts = pLocalPlayer->GetParts( (CDnParts::PartsTypeEnum)i );
				//DnPartsHandle hParts = pClonePlayerActor->GetParts( (CDnParts::PartsTypeEnum)i );
				if( !hParts )
					continue;
				if( (CDnParts::PartsTypeEnum)i == CDnParts::Helmet && pLocalPlayer->IsHideHelmet() )
					continue;
				AttachItem( hParts->GetClassID(), 0, 0, hParts->GetLookItemID() );
			}

			
			for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
			{
				DnWeaponHandle hWeapon = pLocalPlayer->GetWeapon( i );
				//DnWeaponHandle hWeapon = pClonePlayerActor->GetWeapon( i );
				if( !hWeapon ) continue;
				AttachItem( hWeapon->GetClassID(), hWeapon->GetEnchantLevel() );
			}
			

			for( int i = 0; i < CDnParts::CashPartsTypeEnum_Amount; i++ )
			{
				DnPartsHandle hParts = pLocalPlayer->GetCashParts( (CDnParts::PartsTypeEnum)i );
				//DnPartsHandle hParts = pClonePlayerActor->GetCashParts( (CDnParts::PartsTypeEnum)i );
				if( !hParts ) continue;
				if( i < CDnParts::SwapParts_Amount && pLocalPlayer->IsViewPartsOrder(i) == false )
					continue;
				if( (CDnParts::PartsTypeEnum)i == CDnParts::CashHelmet && pLocalPlayer->IsHideHelmet() )
					continue;
				AttachItem( hParts->GetClassID(), 0, ( (i == CDnParts::CashRing2) ? 1 : 0 ), hParts->GetLookItemID() );
			}

			
			for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
			{
				DnWeaponHandle hWeapon = pLocalPlayer->GetCashWeapon( i );
				//DnWeaponHandle hWeapon = pClonePlayerActor->GetCashWeapon( i );
				if( !hWeapon ) continue;
				if( pClonePlayerActor->IsViewWeaponOrder(i) == false )
					continue;
				AttachItem( hWeapon->GetClassID(), hWeapon->GetEnchantLevel(), 0, hWeapon->GetLookItemID() );
			}
			

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
			// 로그인 캐릭터 선택화면과 같이 CombineParts 를 안하기때문에 워리어 건틀렛꼈을때 오른쪽 장갑메시 하이드 하는 것을 강제로 호출한다.
			pClonePlayerActor->OnCombineParts();
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
						
			/*EtVector2 vDir( 0.0f, 1.0f );
			pClonePlayerActor->Look( vDir );

			if( CDnActor::s_hLocalActor )
			{
				LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
				EtMatrix WorldMat = *pClonePlayerActor->GetMatEx();
				pClonePlayerActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
			}*/

			//pClonePlayerActor->SetActionQueue( "Normal_Stand" );
			//pClonePlayerActor->SetGuildSelfView( pLocalPlayer->GetGuildSelfView() );

			//pActor->CombineParts();		// 렌더아바타에선 컴바인 하지 않는다.
			//pClonePlayerActor->SetHP( 1 );		// 몇몇 코드에선 HP 0이하인걸로 직접 판단하는 데가 있어서 패스되지 말라고 강제로 1 넣어준다.
			//hClonePlayer->Show( true );	// 렌더타겟에 그리기는 해야하기 때문에,
			

			//MAPartsBody *pActorPartsBody = dynamic_cast<MAPartsBody*>(hClonePlayer.GetPointer());
			//pActorPartsBody->SetIgnoreDeleteWaitOnepiece( true );
			//AddObject( pActorPartsBody->GetObjectHandle() );
			//AddObject( pActorPartsBody->GetCombineObject() );	// 컴바인오브젝트엔 알파(머리)와 무기가 빠진다.
			//SetRenderFrameCount( -1 );	// -1로 설정해두면 오브젝트 풀릴때까지 계속 렌더링한다.
		}
	

		//// 각 파츠의 링크본 처리부분에서 액터의 위치를 구해와 링크된 자식들의 행렬 계산을 하는데,
		//// 프로세스를 돌리지 않기 때문에 이렇게 별도로 초기위치를 설정해놓아야한다.
		//if( hClonePlayer->GetObjectHandle() )
		//{
		//	EtMatrix Mat;
		//	EtMatrixIdentity( &Mat );
		//	hClonePlayer->GetObjectHandle()->SetWorldMat( Mat );
		//}		

		hFindedActor = hClonePlayer;
	}
	
	m_hActor.Identity();
	m_vecPartsInfo.clear();	
}