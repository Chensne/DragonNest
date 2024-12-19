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
	_ASSERT( pStream && "�ƽ� ���� ��Ʈ�� �ε� ����!" );
	if( NULL == pStream )
		return bResult;

	int iSize = pStream->Size();
	char* pBuffer = new char[iSize];
	ZeroMemory( pBuffer, iSize );
	pStream->ReadBuffer( (void*)pBuffer, iSize );

	// ReadBuffer ȣ�� �Ŀ� ���� �޸𸮰� �� �� �޷� ���ͼ� ������ ��...
	int i = 0;
	for( i = iSize-1; i > 0; --i )
	{
		if( '}' == pBuffer[ i ] && i < iSize-2 )
		{
			pBuffer[ i+1 ] = NULL;
			break;
		}
	}

	_ASSERT( 0 < i && "�߸��� �ƽ� �����Դϴ�." );
	
	bool bSuccess = m_pCutSceneData->LoadFromStringBuffer( pBuffer );
	_ASSERT( bSuccess && "�ƽ� ���� �ε� ����!" );

	delete [] pBuffer;

	SAFE_DELETE( pStream );

	// ����Ʈ ��ȣ�� �ִ� ���� ���� ���� ������. ����� ���Դ��� Ȯ��.
	bool bValidQuestInfo = !((-1 == nQuestIndex && -1 != nQuestStep) || (-1 != nQuestIndex && -1 == nQuestStep));
	_ASSERT( bValidQuestInfo && "����Ʈ ��ȣ�� ������ ������ �Է��� ���ų� ������ ������ ��ȣ�� �����ϴ�!" );
	if( false == bValidQuestInfo )
		return bResult;

	m_bAutoFadeIn = bAutoFadeIn;
	m_pWorld = CDnWorld::GetInstancePtr();
	bool bVillage = (CDnWorld::MapTypeEnum::MapTypeVillage == m_pWorld->GetMapType());

	if( bSuccess )
	{
		m_CutScenePlayer.SetWorld( m_pWorld );

		// ��Ȱ���ڸ��� �ƽ� ���� �� �ִ�. ���� ���⼭ �ö��� ������� �������ƾ� ������� ������ �ʰ� �ȴ�.
		//SGraphicOption Option;
		//GetEtOptionController()->GetGraphicOption( Option );
		
		//Option.bEnableDOF
		CDnWorldEnvironment* pWorldEnv = m_pWorld->GetEnvironment();

		// ���� �ƽ��̶�� ������ �������� 1�� ���ش�. �������� �ȳ����� �������� ��ٷ� �ƽ� ����ϸ� 
		// CDnLocalPlayerActor::GetSaturationColor() �� 0�� ������.
		if( bVillage )
			pWorldEnv->ControlColorFilter( CDnWorldEnvironment::Saturation, 1.0f, 500 );
		else
			pWorldEnv->ControlColorFilter( CDnWorldEnvironment::Saturation, CDnLocalPlayerActor::GetSaturationColor(), 500 );

		//if( CDnActor::s_hLocalActor )
		//	CDnActor::s_hLocalActor->SetPlayingCutScene( true );
		CGlobalInfo::GetInstance().SetPlayingCutScene( true );

		SAFE_DELETE( m_pActorProcessor );
		m_pActorProcessor = new CDnCutSceneActorProcessor;

		// ���� ȭ�鿡 �̹� ������ �ǰ� �ִ� ���͵��� ��� ��
		int iNumLiveActor = (int)CDnActor::s_pVecProcessList.size();
		for( int iActor = 0; iActor < iNumLiveActor; ++iActor )
		{
			DnActorHandle hLiveActor = CDnActor::s_pVecProcessList.at( iActor )->GetMySmartPtr();
			if( hLiveActor->IsNpcActor() && !hLiveActor->IsShow() )
				continue;

			m_dqhLiveActors.push_back( hLiveActor );

			// ���� EtMatrixEx ����
			m_vlOriMatExWorlds.push_back( *hLiveActor->GetMatEx() );

			const char* pCurrentAction = hLiveActor->GetCurrentAction();

			//#43481 ��ų ������̸� ��ų ������ �߰����� �ʵ����Ѵ�.
			//���� ������ ��� �ִٰ� Finalize���� ���⿡�� ����� ������ ȣ���ϵ��� �Ǿ� ����.
			if( NULL == pCurrentAction || 
				strlen(pCurrentAction) == 0 || 
				strcmp(pCurrentAction, "Summon_On") == 0
				|| hLiveActor->IsProcessSkill()
				)
				m_szOriAction.push_back( "Stand" );
			else
				m_szOriAction.push_back( pCurrentAction );
			
			//#43481 ��ų ����� �ƾ� ���� �ɶ� Ŭ���̾�Ʈ ��ų ��� �ϰ�, ����ȭ ���� CmdStop��Ŷ ����.
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

		// ó���� ���ܵ� ���� ����Ʈ�� �޾Ƴ��� ����ϴ� ���͵��� ã�Ƽ� �����ϵ��� �Ѵ�.
		m_dqhExceptActors = m_dqhLiveActors;

		DNTableFileFormat*  pActorTable = CDnTableDB::GetInstancePtr()->GetTable( CDnTableDB::TACTOR );

		m_CutScenePlayer.Initialize( m_pCutSceneData );

		// ���� �� �ִ� ���� �̸�
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

		//assert( bSameMap && "�� �ſ� ����� �ʰ� ���� ���� �̸��� �ٸ��ϴ�." );
#endif
		// ������ �������� �ִٸ� ����
		CDnDropItem::ShowDropItems( false );

		// ī�޶� ����
		DnCameraHandle hDnCamera = CDnCamera::GetActiveCamera();

		// #38660 ī�޶� �� ���� ���� ��찡 �־� �̷� ��쿣 �÷��̾� ī�޶� ���� ���������� ����.
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

		// DOF ���� ������
		int iDOFEffectIndex = m_pWorld->GetEnvironment()->GetDOFEffectIndex();
		DnCameraHandle hPlayerCamera = CDnCamera::FindCamera( CDnCamera::PlayerCamera );
		// ���� �� ī�޶� ����Ʈ ����
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

		// �ƽ� �÷��̾�� ī�޶� �����ϱ� ���ؼ� ī�޶� ���Ϳ��Լ� ����ġ ��Ŵ.
		if( hDnCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
			CDnPlayerCamera* pPlayerCamera = static_cast<CDnPlayerCamera*>(hDnCamera.GetPointer());
			pPlayerCamera->DetachActor();
		}

		// ����Ʈ ������ �ִ� �ƽŰ� ���� �ƽ�, �÷��̾�� ���� ���ο� ���� ����� ���� �����ϴ�..

		// �÷��̾� ĳ���Ͱ� ���� �����̸� ���� ĳ���ʹ�� ������´�.
		// �� ��Ƽ ���� �޾Ƶд�.
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

		// ����Ʈ ������ ���� �ƽ�, �÷��̾���� �� ������, �������� ����Ǵ� ���� ��� �Ǵ�.
		bool bIsQuestCutScene = (-1 != nQuestIndex && -1 != nQuestStep);
		if( !bIsQuestCutScene )
		{
			PrepareNormalCutScene( pActorTable );
		}
		else
		{
			PrepareQuestCutScene( pActorTable, nQuestIndex, nQuestStep, dwQuestPlayerUniqueID );
		}

		// ���� �׼��������� ��󳻼� default �׼��� ������ ����... 
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

		// ������ ������~~
		m_CutScenePlayer.MakeUpSequences( m_pActorProcessor );

		// �������� ���ԵǴ� �༮���� ���� show �� ó��
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

			// �ƽ� �÷��̾�� ani distance �� ���� ����ؼ� �־���.
			// ���� ��Ʈ������ ������ ���� �� ���� �����Ƿ� �������� �ִϸ��̼� ���� 
			// ����Ʈ�� ���� �ִ� Y �� ani distance �� ����Ǿ� ������ �����̴�.
			// ����� ���� �����Ͽ� ������� �ִϸ��̼� Ű ���� ������ ���� �ϵ��� �����Ǿ��ִ�.
			if( pActor->GetAniObjectHandle() )
				pActor->GetAniObjectHandle()->SetCalcPositionFlag( 0 );
		}

		// ���ܵ� ���͵��� ��� ����
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

	// �ƽ� �����ϱ� ���� UI�� ���ܵξ��ٸ�, �ٽ� ������ �ؾ� �Ϲ���������,
	// Finalize�� g_bEtUIRender���� false�� ��� �ڿ������� ��ȯ���� �ʰ�, ĳ���Ͱ� ���ִ� ����� ���̰� �ȴ�.
	// UI�������� �ȵǸ鼭 ���̵�ƿ� ȭ���� �Ⱥ��̱� �����̴�.
	// �׷��� �켱 ������ ���� �������� �ʱ�� �Ѵ�.
	//
	// �ƹ����� g_bEtUIRender�� true���� ���̵�ƿ����̾�α״� ������ �� �ִ� ����� �ʿ��� �� �ϴ�.
	//m_bPrevEtUIRender = EtInterface::g_bEtUIRender;
	EtInterface::g_bEtUIRender = true;
	CEtCustomRender::s_bEnableCustomRender = true;

#ifndef _FINAL_BUILD
#ifdef _RDEBUG
	// ���̳κ��� �ƴҶ� ����ƮZ ������ ������ UI������. UI���� �ƽ� ĸ���ϴ� �뵵.
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

// #61781 �״ý� �⽾ ����(��ũ ũ�ҽ�) �ƾ����� ����� ���и� ������� ����.
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


	// ���� ��Ұ� �������� �������� üũ
	bool bVillage = false;
	if( CDnWorld::MapTypeEnum::MapTypeVillage == m_pWorld->GetMapType() )
		bVillage = true;

	// �� ��Ƽ ���� �޾Ƶд�.
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

	// ���ΰ� ���Ͱ� �ϳ� ���ΰ�
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

	// �ƽſ��� �������ִ� ���ΰ� ������ �̸��� �޾ƿ´�.
	const SceneInfo* pSceneInfo = m_pCutSceneData->GetSceneInfo();
	const string& strMainCharName = pSceneInfo->strMainCharacterName;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );	


	

	// �ƽſ��� ���Ǵ� ���͵��� �������� ���� ������ �ǰ� �ִ� ���͵��� �״�� ���� �ƴ� �ֵ��� ���Ӱ� �ε���� �Ѵ�.		
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		S_CS_ACTOR_INFO ActorToPlay;
		const char* pActorName = m_pCutSceneData->GetActorByIndex( iActor );
		
		ActorToPlay.strActorName.assign( pActorName );

		string& strActorResName = vlActorResNames.at( iActor );

		// ���ʿ����� �빮�ڷ� ����Ǿ��ִ�. ���̺��� ã�� ���� ù ���ڸ� �빮�ڷ� ã�ƾ� �Ѵ�.
		//transform( strActorResName.begin()+1, strActorResName.end(), strActorResName.begin()+1, tolower );

		int iItemID = pActorTable->GetItemIDFromFieldCaseFree( "_StaticName", strActorResName.c_str() );
		//assert( -1 != iItemID );

		CDnActorState::ActorTypeEnum eActorType = (CDnActorState::ActorTypeEnum)pActorTable->GetFieldFromLablePtr( iItemID, "_Class" ) ->GetInteger();
		bool bPlayerCharacter = eActorType <= CDnActorState::Reserved6;
		ActorToPlay.bPlayer = bPlayerCharacter; // bintitle.
		
		// ���ΰ� ���Ͱ� �ϳ� ���̶�� �� ĳ���͸� ���ΰ����� �������ش�.
		DnActorHandle hFindedActor;		
		if( bPlayerCharacter )
		{
			// �����ϴ� �÷��̾� ĳ���Ͱ� �ϳ��� �̸��� ������� ������ �� ĳ���� ���ΰ�����.
			if( bSinglePlayerCharacter )
			{
				hFindedActor = CDnActor::s_hLocalActor;
				bMyCharacterIsMain = true;
			}
			else
			{
				string strActorResName = m_pCutSceneData->GetActorByIndex( iActor );

				// #60295 Į�� ������ �ƾ� Į�� ����� ������ ����. - Į�� ����� ������ ���� - ���� �÷��̾� ���Ϳ��� �˻�.
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

				if( ActorToPlay.bClone == false ) // #60295 Į�� ������ �ƾ� - Į�� ����� ������ ����.
				{
					// ���� ���ΰ� �ڸ���� �� ĳ���ͷ� ���ΰ� �����..
					if( false == bMyCharacterIsMain && strMainCharName == pActorName )
					{
						hFindedActor = CDnActor::s_hLocalActor;
						bMyCharacterIsMain = true;
					}
				}
			}	
			
		}

		// ���ΰ� ���ͷ� ���� ������ �ȵǸ� ���� ����Ʈ���� ã�´�
		if( NULL == hFindedActor && ActorToPlay.bClone == false )
		{
			// ���� ItemID �� ���� ���͸� ã�´�. ���ܵ� ���͸���Ʈ���� ã�� ���� ����.
			// ���� ������ �ִ� �༮�� ��쿣 ���� ���̺� ID���� ���ƾ� �Ѵ�.
			int iNumLiveActor = (int)m_dqhExceptActors.size();
			for( int iActor = 0; iActor < iNumLiveActor; ++iActor )
			{
				DnActorHandle hLiveActor = m_dqhExceptActors.at( iActor );
				if( hLiveActor->GetClassID() == iItemID )
				{
					bool bScaleLock = m_pCutSceneData->GetThisActorsScaleLock( pActorName );
					if( bScaleLock )
					{
						// ������ ���� ��쿣 �⺻ �������� �ƴϰ� ������ ���ʹ� �ƽſ��� ������� �ʴ´�.
						if( hLiveActor->GetScale()->x != 1.0f )
							continue;
					}
					else
					{
						// �÷��̾�� �翬�� 0�̰� �����߿����� ������ ���õ� ��츸 ���� ���� ���̺� ID�� �ִ�.
						// �ش� ���� ���̺��� ID ���� ���� ��쿡 ȭ�鿡 �ִ� �� ���͸� �����ϵ��� �Ѵ�.
						int iMonsterTableID = m_pCutSceneData->GetMonsterTableID( pActorName );
						if( 0 != iMonsterTableID && false == bPlayerCharacter )
						{
							// ���� �����̰� ������ ���õ� ���� ���̺��� ���̵�� ���ٸ� ���~ 
							// �ƴ϶�� continue �� �н�.
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
			// ���� ItemID �� ���� ���͸� ã�´�. ���Եǰ� �Ǿ����Ƿ� ���� ����Ʈ���� ������ ����.
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
			// ��ȹ�ʿ��� ������ ����� �ֱ� ������ 
			if( bPlayerCharacter )
			{
				// // #60295 Į�� ������ �ƾ� Į�� ����� ������ ���� - �÷��̾� ���Ͱ� �ΰ� �̻��� ��� �����Ѵ�.				
				if( ActorToPlay.bClone )
					AddClonePlayer( hFindedActor );

				else
				{
					// ���� �� ĳ���� ��� �ȵǾ������� �ٷ� �����
					if( false == bMyCharacterIsMain )
					{
						hFindedActor = CDnActor::s_hLocalActor;
						bMyCharacterIsMain = true;
					}
					else
					{
						// �÷��̾ �� ã�Ҵٸ� ���ΰ� ĳ�� �����ϰ�, ��Ƽ ������� �ƹ��� ��������.
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
				
				//// NOTE: �÷��̾� ĳ���͵��� ���� �ε��� �ʿ� �����ϴ�!1
				//// ���ΰ����� ����Ʈ ���⸦ �����
				//if( (eActorType == CDnActorState::Warrior) ||
				//	(eActorType == CDnActorState::Archer) ||
				//	(eActorType == CDnActorState::Soceress) ||
				//	(eActorType == CDnActorState::Cleric) )
				//{
				//	hFindedActor = CreateActor( iItemID, false, true );
				//	assert( hFindedActor && "�ƽſ� ���� ���� ����" ); 

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


				// ���ͳ� npc �� ã������ ���� �ε� �Ѵ�.
				DNTableFileFormat* pActorSOX = GetDNTable( CDnTableDB::TACTOR );

				if( eActorType == CDnActorState::Npc )
				{
					hFindedActor = CreateActor( iItemID );
					assert( hFindedActor && "�ƽſ� ���� ���� ����" ); 

					// NPC
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNPC );

					vector<int> vlNPCIDList;
					pSox->GetItemIDListFromField( "_ActorIndex", iItemID, vlNPCIDList );
					_ASSERT( !vlNPCIDList.empty() && "�ش� NPC ������ ������ NPC ���̺� �����ϴ�" );

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
					assert( hFindedActor && "�ƽſ� ���� ���� ����" ); 

					// ���͵��� ���� ���̺��� �����ؼ� ���⸦ �����
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );

					vector<int> vlMonsterIDList;
					pSox->GetItemIDListFromField( "_ActorTableID", iItemID, vlMonsterIDList );
					//_ASSERT( !vlMonsterIDList.empty() && "�ش� Monster ������ ������ Monster ���̺� �����ϴ�" );
					if( false == vlMonsterIDList.empty() )
					{
						int iMonsterTableID = (int)vlMonsterIDList.front();

						// �÷��̾�� �翬�� 0�̰� �����߿����� ������ ���õ� ��츸 ���� ���� ���̺� ID�� �ִ�.
						// ������ ������ ID�� �ε��ϵ��� �Ѵ�. ������ assert �ϳ� ���� �׳� ���� �ε��Ѵ�.
						int iToolSelectedMonsterTableID = m_pCutSceneData->GetMonsterTableID( pActorName );
						if( 0 != iToolSelectedMonsterTableID )
						{
							bool bScaleLock = m_pCutSceneData->GetThisActorsScaleLock( pActorName );
							_ASSERT( false == bScaleLock );

							vector<int>::iterator iter = find( vlMonsterIDList.begin(), vlMonsterIDList.end(), iMonsterTableID );
							if( vlMonsterIDList.end() != iter )
							{
								iMonsterTableID = iToolSelectedMonsterTableID;

								// ������ ����. �׳� ���� �ȵ����� �ִ� ũ��� �ε�.
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
			//	// ���ΰ��� ĳ���͵� ���� ���� ���� ��Ƽ ĳ���͵��� ���� �ʰ� �ϱ� ���ؼ� �ڽ��� ��Ƽ������ ������ �޴´�.
			//	if( (hFindedActor->GetActorType() == CDnActorState::Warrior) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Archer) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Soceress) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Cleric) )
			//	{
			//		CDnPartyTask::PartyStruct* pSelectedMember = NULL;

			//		// ��Ƽ�� �ش� ���� Ÿ���� �ֳ� ã�ƺ���.
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

		// ��ī���� ���� ���ϴ� �ƽ��ε� �����Ͱ� ��������� �ɷ���. /////////////
#if defined (PRE_ADD_ACADEMIC)
		if( pSceneInfo->bAcademicNotSupported )
		{
			if( CDnActorState::Academic == hFindedActor->GetActorType() )
				continue;
		}
#endif
		//////////////////////////////////////////////////////////////////////////

		hFindedActor->Show( true );

		// �ٷ� ������ �ȵǱ� ������ HP �� �������ش�.
		if( 0 == hFindedActor->GetHP() )
		{
			if( bMonster )
				m_vlhDeadMonster.push_back( hFindedActor );

			hFindedActor->SetHP( 10 );
		}

		// Rotha : �÷��̾ ���� �پ��ִµ� �ƽ� ����� �Ǹ� �ȵǹǷ� , ������ ��������
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

		// �׼��� stand �� �ٲ��ش�. ��ų ����� �ñ׳��� ������ ī�޶� ȿ���� �����ų� ���� �ʵ���.
		// �ƽſ����� ���� �׼��� �ٲٴ°� �ƴ϶� AniFrame �� ���� �����ϱ� ������ ���� �׼��� �ñ׳��� ó������ �ʵ��� stand �� �ٲ��ش�.
		hFindedActor->CmdAction( "Stand" );

		// �������� ����Ǵ� ������ ���͵��� �ø��� ����.
		if( ActorToPlay.hObject ) 
			ActorToPlay.hObject->EnableCull( false );

		if( ActorToPlay.hMergedObject )
			ActorToPlay.hMergedObject->EnableCull( false );

		
		// #71465 -���� �ܡ��ҷ��� ����Ų ������ �׷����� �̺�Ʈ ���񿡼� ǥ�õ��� �ʴ´�.
		// - 
		if( pPlayer )
		{
			DnPartsHandle hParts = pPlayer->GetCashParts( CDnParts::PartsTypeEnum::CashFairy );
			if( hParts )
				hParts->ShowRenderBase( false );
		}


		// �ƽ� ������ ���ȿ� ������ STE_Input/STE_InputHasPassiveSkill �� ó������ �ʴ´�.
		//hFindedActor->SetPlayingCutScene( true );

		// �ƽ� ������ ���ȿ� ���Ϳ� ���� ���� �ñ׳��� ������� �ʴ´�.
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


// ����Ʈ �ƽ��� �������� ����Ǵ� ���� ����.
void CDnCutSceneTask::PrepareQuestCutScene( DNTableFileFormat*  pActorTable, int nQuestIndex, int nQuestStep, DWORD dwQuestPlayerUniqueID )
{
	// �� ��Ƽ ���� �޾Ƶд�.
	deque<CDnPartyTask::PartyStruct*> dqParty;
	CDnPartyTask* pPartyTask = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	DWORD dwNumParty = pPartyTask->GetPartyCount();
	for( DWORD dwMember = 0; dwMember < dwNumParty; ++dwMember )
	{
		CDnPartyTask::PartyStruct* pParty = pPartyTask->GetPartyData( dwMember );
		dqParty.push_back( pParty );
	}

	// ���� ����Ʈ �������ΰ�. ����Ʈ �ƽſ����� ����Ʈ �����ڰ� ���ΰ��� �ȴ�.
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

	// ���� ���Ͱ� �ϳ� ���ΰ�
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

	// �ƽſ��� �������ִ� ���ΰ� ������ �̸��� �޾ƿ´�.
	const SceneInfo* pSceneInfo = m_pCutSceneData->GetSceneInfo();
	const string& strMainCharName = pSceneInfo->strMainCharacterName;
	// �ƽſ��� ���Ǵ� ���͵��� �������� ���� ������ �ǰ� �ִ� ���͵��� �״�� ���� �ƴ� �ֵ��� ���Ӱ� �ε���� �Ѵ�.	
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		S_CS_ACTOR_INFO ActorToPlay;
		const char* pActorName = m_pCutSceneData->GetActorByIndex( iActor );

		ActorToPlay.strActorName.assign( pActorName );

		string& strActorResName = vlActorResNames.at( iActor );

		// ���ʿ����� �빮�ڷ� ����Ǿ��ִ�. ���̺��� ã�� ���� ù ���ڸ� �빮�ڷ� ã�ƾ� �Ѵ�.
		//transform( strActorResName.begin()+1, strActorResName.end(), strActorResName.begin()+1, tolower );

		int iItemID = pActorTable->GetItemIDFromFieldCaseFree( "_StaticName", strActorResName.c_str() );
		//assert( -1 != iItemID );

		CDnActorState::ActorTypeEnum eActorType = (CDnActorState::ActorTypeEnum)pActorTable->GetFieldFromLablePtr( iItemID, "_Class" )->GetInteger();
		bool bPlayerCharacter = eActorType <= CDnActorState::Reserved6;

		ActorToPlay.bPlayer = bPlayerCharacter; // bintitle.

		DnActorHandle hFindedActor;

		// ���� ���Ͱ� �� ���̰ų� ���ΰ� �ڸ��� ��, ���� ����Ʈ �����ڶ�� ���� ���ΰ����� �ǵ���..
		// ���� ����Ʈ �����ڰ� �ƴ϶�� ��Ƽ�� �߿� ����Ʈ �����ڷ� �������ش�.
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

		// ���ΰ� ���Ͱ� �ϳ� ���̶�� �� ĳ���͸� ���ΰ����� �������ش�.
		//DnActorHandle hFindedActor;
		//if( false == bMyCharacterIsMain && bPlayerCharacter )
		//{
		//	// ���ΰ� �ڸ��̰ų� �÷��̾ �Ѹ� ���´ٰų��ϰ� ���� ����Ʈ �����ڶ�� �� ĳ���ͷ� ���ΰ� �����..
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
				// �÷��̾ �� ã�Ҵٸ� ���ΰ� ĳ�� �����ϰ�, ��Ƽ ������� �ƹ��� ��������.
				// ���ɵ� �ִ� �־����� �ʴ´�.
				if( false == dqParty.empty() )
				{
					while( false == dqParty.empty() )
					{
						DnActorHandle hActor = dqParty.front()->hActor;
						dqParty.pop_front();
				
						// ��ī���� ���� ���ϴ� �ƽ��ε� �����Ͱ� ��������� �ɷ���. /////////////
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
			// ���� ItemID �� ���� ���͸� ã�´�. ���ܵ� ���͸���Ʈ���� ã�� ���� ����.
			int iNumLiveActor = (int)m_dqhExceptActors.size();
			for( int iActorInner = 0; iActorInner < iNumLiveActor; ++iActorInner )
			{
				DnActorHandle hLiveActor = m_dqhExceptActors.at( iActorInner );

				if( hLiveActor->GetClassID() == iItemID )
				{
					bool bScaleLock = m_pCutSceneData->GetThisActorsScaleLock( pActorName );
					if( bScaleLock )
					{
						// ������ ���� ��쿣 �⺻ �������� �ƴϰ� ������ ���ʹ� �ƽſ��� ������� �ʴ´�.
						if( hLiveActor->GetScale()->x != 1.0f )
							continue;
					}
					else
					{
						// �÷��̾�� �翬�� 0�̰� �����߿����� ������ ���õ� ��츸 ���� ���� ���̺� ID�� �ִ�.
						// �ش� ���� ���̺��� ID ���� ���� ��쿡 ȭ�鿡 �ִ� �� ���͸� �����ϵ��� �Ѵ�.
						int iMonsterTableID = m_pCutSceneData->GetMonsterTableID( pActorName );
						if( 0 != iMonsterTableID && false == bPlayerCharacter )
						{
							// ���� �����̰� ������ ���õ� ���� ���̺��� ���̵�� ���ٸ� ���~ 
							// �ƴ϶�� continue �� �н�.
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
			// �÷��̾� ���͸� ���� ����Ʈ���� ã�� ���ߴٸ� ���͸� �߰����� �ʴ´�.
			if( bPlayerCharacter )
			{
				//m_pActorProcessor->AddObjectToPlay( &ActorToPlay );
				m_CutScenePlayer.HideActor( ActorToPlay.strActorName.c_str() );
			}
			
			{
				// ���ͳ� npc �� ã������ ���� �ε� �Ѵ�.
				DNTableFileFormat* pActorSOX = GetDNTable( CDnTableDB::TACTOR );

				// NOTE: �÷��̾� ĳ���͵��� ���� �ε��� �ʿ� �����ϴ�!1
				// ���ΰ����� ����Ʈ ���⸦ �����
#ifdef PRE_ADD_ACADEMIC
				if( CDnActorState::Warrior <= eActorType &&
					eActorType <= CDnActorState::Academic )
#else
				if( CDnActorState::Warrior <= eActorType &&
					eActorType <= CDnActorState::Cleric )
#endif // #ifdef PRE_ADD_ACADEMIC
				{
					hFindedActor = CreateActor( iItemID, false, true );
					assert( hFindedActor && "�ƽſ� ���� ���� ����" ); 

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
					assert( hFindedActor && "�ƽſ� ���� ���� ����" ); 

					// NPC
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNPC );

					vector<int> vlNPCIDList;
					pSox->GetItemIDListFromField( "_ActorIndex", iItemID, vlNPCIDList );
					_ASSERT( !vlNPCIDList.empty() && "�ش� NPC ������ ������ NPC ���̺� �����ϴ�" );

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
					assert( hFindedActor && "�ƽſ� ���� ���� ����" ); 

					// ���͵��� ���� ���̺��� �����ؼ� ���⸦ �����
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );

					vector<int> vlMonsterIDList;
					pSox->GetItemIDListFromField( "_ActorTableID", iItemID, vlMonsterIDList );
					//_ASSERT( !vlMonsterIDList.empty() && "�ش� Monster ������ ������ Monster ���̺� �����ϴ�" );
					if( false == vlMonsterIDList.empty() )
					{
						int iMonsterTableID = (int)vlMonsterIDList.front();

						// �÷��̾�� �翬�� 0�̰� �����߿����� ������ ���õ� ��츸 ���� ���� ���̺� ID�� �ִ�.
						// ������ ������ ID�� �ε��ϵ��� �Ѵ�. ������ assert �ϳ� ���� �׳� ���� �ε��Ѵ�.
						int iToolSelectedMonsterTableID = m_pCutSceneData->GetMonsterTableID( pActorName );
						if( 0 != iToolSelectedMonsterTableID )
						{
							vector<int>::iterator iter = find( vlMonsterIDList.begin(), vlMonsterIDList.end(), iMonsterTableID );
							if( vlMonsterIDList.end() != iter )
							{
								iMonsterTableID = iToolSelectedMonsterTableID;

								// ������ ����. �׳� ���� �ȵ����� �ִ� ũ��� �ε�.
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
			//	// ���ΰ��� ĳ���͵� ���� ���� ���� ��Ƽ ĳ���͵��� ���� �ʰ� �ϱ� ���ؼ� �ڽ��� ��Ƽ������ ������ �޴´�.
			//	if( (hFindedActor->GetActorType() == CDnActorState::Warrior) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Archer) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Soceress) ||
			//		(hFindedActor->GetActorType() == CDnActorState::Cleric) )
			//	{
			//		CDnPartyTask::PartyStruct* pSelectedMember = NULL;

			//		// ��Ƽ�� �ش� ���� Ÿ���� �ֳ� ã�ƺ���.
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

		// �ٷ� ������ �ȵǱ� ������ HP �� �������ش�.
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

		// �������� ����Ǵ� ������ ���͵��� �ø��� ����.
		if( ActorToPlay.hObject ) 
			ActorToPlay.hObject->EnableCull( false );

		if( ActorToPlay.hMergedObject ) 
			ActorToPlay.hMergedObject->EnableCull( false );

		// �׼��� stand �� �ٲ��ش�. ��ų ����� �ñ׳��� ������ ī�޶� ȿ���� �����ų� ���� �ʵ���.
		// �ƽſ����� ���� �׼��� �ٲٴ°� �ƴ϶� AniFrame �� ���� �����ϱ� ������ ���� �׼��� �ñ׳��� ó������ �ʵ��� stand �� �ٲ��ش�.
		hFindedActor->CmdAction( "Stand" );

		// �ƽ� ������ ���ȿ� ������ STE_Input/STE_InputHasPassiveSkill �� ó������ �ʴ´�.
		//hFindedActor->SetPlayingCutScene( true );

		// �ƽ� ������ ���ȿ� ���Ϳ� ���� ���� �ñ׳��� ������� �ʴ´�.
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

	// �г��� ��� �� ������
	int iNumActors = (int)m_dqhLiveActors.size();
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		DnActorHandle hActor = m_dqhLiveActors.at( iActor );
		hActor->ShowHeadName( false );
	}

	// �÷��̾� ���ʹ� �ƽ� �����Ҷ� ���⸦ ��� �ִ��� �ְ� �ִ���, � �������� �𸥴�.
	// �ƽſ� �����ϴ� �÷��̾� ���͵��� ���� ���¸� �ƽ� ���۽��� �׼��� �ٰŷ� �����ش�.
	// �ƽ��� ������ ������� ���� ���� ��.
	int iNumActor = m_pActorProcessor->GetNumActors();
	m_vlIsPlayerBattleMode.assign( iNumActor, 0 );
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		S_CS_ACTOR_INFO* pActorInfo = m_pActorProcessor->GetActorInfoByIndex( iActor );
		
		CDnActor* pDnActor = static_cast<CDnActor*>(pActorInfo->pActor);
		CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(pDnActor);
		if( pPlayerActor && false == pPlayerActor->IsDie() )
		{
			// ù��° �׼��� "Normal" �� ���� ���� �׼��� �� ���� �׼���
			// ���� ������ ���°� ��ĸ� �ҹ��ϰ� Normal ���� �׼��̸� ���� ���ְ� 
			// �ƴ϶�� ���ݻ��·� �ٲ��ش�.
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

			// ���� ���÷� �ٲ���.
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

			// Note: ���� ����ȿ�� ���� ���·� �ƽ� ������ �÷��̾� ĳ������ ������ �ٲ� ���� �� ����.
			//pPlayerActor->RemoveAllBlow();

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
			// �÷��̾��� ��쿣 �׼ǿ� ���� ����Ʈ �ñ׳� ��� ���ҽ� �籸��.
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

	// Process Input Device - ��ȭâ ó��
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

		// #38660 ī�޶� �� ���� ���� ��찡 �־� �̷� ��쿣 �÷��̾� ī�޶� ���� ���������� ����.
		if( !m_CutScenePlayer.GetCamera() )
			m_CutScenePlayer.SetCamera( hCamera );
	}

	// Etc Object Process
	CDnEtcObject::ProcessClass( LocalTime, fDelta );

	m_pWorld->Process( LocalTime, fDelta );

	m_CutScenePlayer.Process( LocalTime );

	// ī�޶��� ��ġ�� Listner �� ��������

	// Process Sound
	if( CEtSoundEngine::IsActive() ) 
	{
		CEtSoundEngine::GetInstance().Process( fDelta );
		CEtSoundEngine::GetInstance().SetListener( matEx.m_vPosition, matEx.m_vZAxis, matEx.m_vYAxis );
	}

	// Note : UI Process
	EtInterface::Process( fDelta );

	// ���� �ƽſ��� ������Ÿ�� ������ �� ������� �ʴ´�.
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
						// ��ڴ� ��ŵ�� �� ����.
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
	// ����� ������ �ٽ� ������
	CDnDropItem::ShowDropItems( true );

	// �ƽſ��� �����ߴ� ����Ʈ�� ��� ����
	int iNumActor = m_pActorProcessor->GetNumActors();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		CDnActor* pActor = static_cast<CDnActor*>(m_pActorProcessor->GetActorInfoByIndex( iActor )->pActor);
		pActor->ReleaseSignalImp();

		// �÷��̾� ���� ���� ���� ����
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
		// �÷��̾��� ��쿣 �׼ǿ� ���� ����Ʈ �ñ׳� ��� ���ҽ� �籸��.
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
	 
	// ���ܵ� ���͵�, ���Ե� ���͵� �߿� ���� ���� ���� �༮���� �ſ��� ��� ���̶� �����! 
	// �ȱ׷��� �ƽ� ������ �׳� �����ְ� ��

	// ���� �Ǿ��� ���͵��� �ٽ� ��Ÿ����
	int iNumExceptActor = (int)m_dqhExceptActors.size();
	for( int iActor = 0; iActor < iNumExceptActor; ++iActor )
	{
		DnActorHandle hActor = m_dqhExceptActors.at( iActor );
		if( !hActor ) continue;
		if( !hActor->IsPlayerActor() && hActor->IsDie() ) continue;

		if( hActor->IsShow() ) hActor->Show( false );
		hActor->Show( true );

		// �������� ���ʹ� �ǵ������ �����Ƿ�..
		//bool bPlayerCharacter = hActor->GetActorType() <= CDnActorState::Reserved6;
		//if( bPlayerCharacter )
		//{
		//	// �÷��̾� ������ ��� ���� ��忴���� ���� �ٴ� ���� �޶����� ��찡 �����Ƿ� ��Ʋ���� ����
		//	CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
		//	if( pPlayerActor->IsBattleMode() )
		//		pPlayerActor->SetBattleMode( true );
		//}
	}

	// �ø� �� ��ġ ����
	int iNumLiveActors = (int)m_dqhLiveActors.size();
	for( int iActor = 0; iActor < iNumLiveActors; ++iActor )
	{
		DnActorHandle hActor = m_dqhLiveActors.at( iActor );
		if( !hActor ) continue;
		if( !hActor->IsPlayerActor() && hActor->IsDie() ) 
		{
			// ���� ���Ͱ� Ȥ�� �ƽſ��� ������ �� �ְ�, ���� �� ó���� 
			// �ȵǾ �����ִ� ��찡 �����Ƿ� ���� �ִ� �ٷ� ����.
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

		// �ƽ� �÷��̾�� ani distance �� ���� ����ؼ� �־���.
		// ���� ��Ʈ������ ������ ���� �� ���� �����Ƿ� �������� �ִϸ��̼� ���� 
		// ����Ʈ�� ���� �ִ� Y �� ani distance �� ����Ǿ� ������ �����̴�.
		// ����� ���� �����Ͽ� ������� �ִϸ��̼� Ű ���� ������ ���� �ϵ��� �����Ǿ��ִ�.
		if( hActor->GetAniObjectHandle() )
			hActor->GetAniObjectHandle()->SetCalcPositionFlag( CALC_POSITION_Y );

		CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
		if( pPlayer )
		{
			pPlayer->SetBattleMode( m_bVecPlayerBattleMode.at( iActor ) );

			// Rotha �ƽ� ����߿� ��Ÿ�������� �ȵǹǷ� �����ִµ� , �ƽų������� �ٽ��¿��ش� , VehicleInfo ������� ���� Ÿ�⶧���� , ��ź��Ȳ�� ��������.
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

		// #71465 -���� �ܡ��ҷ��� ����Ų ������ �׷����� �̺�Ʈ ���񿡼� ǥ�õ��� �ʴ´�.
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
				// �ƽ� �÷��̾�� �׼� �ε����� ���� �����ϱ� ������ ActionQueue �� ����� �׼��� ���ð� �Ǿ����.
				// �׷��� ���� �׼� ��ٷ� ����. (#14014)
				hActor->SetAction( m_szOriAction.at( iActor ).c_str(), 0.0f, 0.0f );
			}

			hActor->ResetStateEffects();
		}
	}

	// �߰� �ε��ߴ� ���͵� ����
	int iAdditionalLoadActors = (int)m_vlhAdditionalLoadedActors.size();
	for( int iActor = 0; iActor < iAdditionalLoadActors; ++iActor ) {
		DnActorHandle hActor = m_vlhAdditionalLoadedActors.at( iActor );
		//		hActor->Show( false );
		hActor->ShowWeapon( 0, false );
		hActor->ShowWeapon( 1, false );
		if( !hActor ) continue;
		SAFE_RELEASE_SPTR( hActor );
	}

	// �׼� ������ ���ϰ� �ִ� �׼��� ������
	int iNumActProp = (int)m_vlActPropDefault.size();
	for( int iActProp = 0; iActProp < iNumActProp; ++iActProp )
	{
		S_ACT_PROP_DEFAULT& PropDefault = m_vlActPropDefault.at( iActProp );
		if( PropDefault.hProp ) 
		{
			PropDefault.pActProp->SetAction( PropDefault.strDefaultAction.empty() ? "" : PropDefault.strDefaultAction.c_str(), 0.0f, 0.0f );
			PropDefault.hProp->Show( PropDefault.bShow );

			// FinalTest ���� hObject �� invalid �� ��찡 �ִ� �� ���Ƽ� ����ڵ� �߰�.
			EtAniObjectHandle hObject = PropDefault.hProp->GetObjectHandle();
			if( hObject )
				hObject->ShowObject( PropDefault.bShow );
		}
	}

	// ī�޶� ����
	DnCameraHandle hDnCamera = CDnCamera::GetActiveCamera();
	hDnCamera->SetCameraInfo( m_OriginalCameraInfo);

	// DOF �� ����
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
		// �÷��̾�� Attach �����ش�.
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
	// ���� ���Ϳ��� "Stand" �׼��� Ŀ�ǵ�� ��
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

			// ���͸� �״�� ���°ű� ������ ���ٸ� ó�� ���ص� �� �� �˾Ҵµ�,
			// �ӵ������� Combine���� �ʰ� ó���ؾ��ϴٺ���, �ѹ��� �����ؼ� ������ �־���.
			// Combine�Ҷ��� �ѹ��� ó���� ��� �� �Ǿ�������, Combine���� �������� �ѹ��� ó���� �ȵ��ִ� ��.
			// �̰� Combine���� �ʴ� �α��� ĳ���� ����ȭ�鿡���� �������� ������.
			// ��� �α��� ȭ�鿡���� �Ϲ����� ĳ�ÿʵ� �� ������ �� �ѹ����� �����ٰų�,(�� �ݴ��� ���. �ѹ��� ���� �� �Ϲ����� ĳ������ ����)
			// ���� �ʱ⶧���� �� ���� ������ ���̴�.
			//
			// ���� Combine���� �ʱ⶧���� ���ο� ������ �����Ҷ� ���� ������ ���� ����ص��� �ʰ� ����µ�,
			// �̷� ���� �ѹ��������� ���¿��� �ٸ� ĳ�û��������� AttachItem�Ҷ� ������ �Դ� ����, �尩, ������ ������ ���ٴ� ���̴�.
			//
			// �̰� �����Ϸ���, ĳ���� ����� ������ ����ִ� ��ó�� �븻����, ĳ������ �� �����ؼ� ����ص־��ϴµ�,
			// �̰� �������½�ũ�� ������� �� ������ ���� �ǹ�����.
			// �ִ��� ��������ƾ�����θ� �ǵ帮���ߴ� �����ƹ�Ÿ�ڵ��,(�׷��� AttachItem�� �����ص� DetachItem�� �Ϻη� ���Ŵ�.)
			// �� ����� �������� �ʱ�� �Ѵ�.
			//
			// �׷��� �ִ��� �ڿ������� ǥ���ϱ� ���� �ѹ��ʿ� ���ؼ��� ����ó���ؼ� ���� ����ֵ��� �ϰڴ�.

			if( !bCash )
			{
				// �����ƹ�Ÿ������ CombineParts �� ���ϱ⶧���� �븻�� ���� ��� ��Ī�Ǵ� ĳ������ �˻���
				// ĳ������ �ִٸ� �������� �ʰ� �ѱ��.
				bool bEquip = true;
				if( hParts->GetPartsType() >= CDnParts::Helmet && hParts->GetPartsType() <= CDnParts::Foot )
				{
					if( pActor->GetCashParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() - 2 ) ) )
					{
						bEquip = false;
					}
				}

				//// Ȥ�� ������ �ѹ��� �������� Ȯ��.(���ϱ� �ѹ��� ������������ �������� �����µ�?)
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
						// �Ӹ�ī���� ��쿡 ������ ���ĺ����� ���� �ȵ��������� �ִ�. �׳� ������ �����Ѵ�.
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
				// ���� �ѹ��� �԰� �־��� ���̶��,
				//if( m_vecPartsInfo.size() )
				//{
				//	// ���� �����ϴ� ������ �ѹ��ʿ� �ش�Ǵ� �������� Ȯ���Ѵ�.
				//	// ������ ��� �ѹ����� �� ���� ���������� ������ ������, ���� �̰� �޶����ٸ�,
				//	// hParts->GetPartsType()�� �˻��ϴ°� �ƴ϶�, ������������ �� �����ͼ� �˻��ؾ��ҰŴ�.
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

				//		// ���� �ѹ����� �����ϰ�,(ù��° �ε����� �ѹ��� ���� ������.)
				//		pActor->DetachCashParts( (CDnParts::PartsTypeEnum)( vecPartsInfo[0].nType - 2 ) );

				//		// ���� ������ �� ���� ��
				//		// ���� Ÿ���� �� �־������ �ش��ϴ� Ÿ�Կ� �´� ������ID�� ������ �������ϸ� �ȴ�.
				//		for( int i = 0; i < (int)vecPartsInfo.size(); ++i )
				//			AttachItem( vecPartsInfo[i].nItemID );

				//		// �Ϲ����� ĳ���� �����ϵ��� �Ʒ� ��ƾ �� ����.
				//	}
				//}

#ifdef PRE_ADD_TRANSPARENCY_COSTUME
				bool bTransparentSkin = CDnParts::IsTransparentSkin( hParts->GetClassID(), pActor->GetClassID() );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME

				// �������� ���� �ѹ����̶��, �ѹ��ʿ� �ش�Ǵ� �ڸ��� �������� ������ ����صд�.
				// ���������� �ϸ� �ڽŽ����� ������ �ڽŵ� ���� �߰��صд�.
				// (�̰� ����ص־� �ѹ��� �԰� �ִٰ� ĳ���尩 ������ ������ �Դ� ���Ƿ� �ǵ��� �� �ִ�.)
				if( hParts->IsExistSubParts() )
				{
					SPartsInfo Info;
					Info.nType = (int)hParts->GetPartsType() + 2;	// �ѹ����� ĳ�����̴� +2�ؼ� �븻�� Ÿ������ �����.
					Info.nItemID = 0;
					m_vecPartsInfo.push_back( Info );
					for( int i = 0; i < hParts->GetSubPartsCount(); ++i )
					{
						Info.nType = (int)hParts->GetSubPartsIndex(i) + 2;	// ����Ÿ�� �����͵� ĳ������ Ÿ���̴� +2�ؼ� �븻�� Ÿ������ �����.
						m_vecPartsInfo.push_back( Info );
					}

					// ���� Ÿ���� �� �־������ �ش��ϴ� Ÿ�Կ� �´� ������ID�� ������ �������ϸ� �ȴ�.
					for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
					{
						if( pActor->GetCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) ) )
						{
							m_vecPartsInfo[i].nItemID = pActor->GetCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) )->GetClassID();
							pActor->DetachCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) );
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
							// �����ѹ������� �������Ҷ��� �Ϲ���������� ��������Ѵ�.
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
						// ���� ����� �����ʿ��� ����ġó�� �Ǿ��ٸ� �� �����̴��� ����Ʈ���� �� ������� ���̴�.
						// �׷��� Ȥ�� �𸣴� �Ʒ� ó�� �صд�.
						else if( m_vecPartsInfo[i].nType >= CDnParts::DefaultPartsType_Min && m_vecPartsInfo[i].nType <= CDnParts::DefaultPartsType_Max )
						{
							m_vecPartsInfo[i].nItemID = pActor->GetDefaultPartsInfo( (CDnParts::PartsTypeEnum)m_vecPartsInfo[i].nType );
						}
					}
				}

				// �����ƹ�Ÿ������ CombineParts �� ���ϱ⶧���� ���⼭ �븻���� ���ִ½����� �մϴ�.
				if( hParts->GetPartsType() >= CDnParts::CashHelmet && hParts->GetPartsType() <= CDnParts::CashFoot )
				{
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
					// ���� �ѹ��ʶ����� Detach���� �ʰ� FreeObject�ϴ°ɷ� �ٲ۴�. �׷��� ���߿� �ʿ��Ҷ� �ٽ� CreateObject�� �� �ִ�.
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

				// �켱 ������ ���ؼ��� �Ѵ�. ������ �̷� ��� ���� ���� ��.
				if( hParts->GetPartsType() == CDnParts::CashHelmet )
				{
					// �Ӹ���� �� �Ϻ� �����鿣 �޽ó��κп� �����ؽ�ó�� ����ϴµ��� ���ĺ����� ������ ���� �ȵ��ִ� ��찡 �ִ�.
					// �̷��� �ص� ����� �������� ������ Ȱ��ȭ��Ų��.
					// ������ ���ϱ�, PostProcess���� ���İ� �״�� �����ϵ��� ó���ϰ���,
					// �׵θ� ���� �̻��ϰ� ���̴� ������ ���� �ذ�Ȱ� ����.
					// �׷��� ������ ���ĺ��� �Ѵ°� �����ϵ��� �ϰڴ�.
					//if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
					//	hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
				}
				else if( hParts->GetPartsType() == CDnParts::CashWing )
				{
					// ������ ��쿣 ������ ���ĺ���, ���н� �Ѽ� ����ϰ� ������ �Ѵ�.
					if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
					{
						hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
						hParts->GetObjectHandle()->GetSkinInstance()->SetRenderAlphaTwoPass( true );
					}
				}
			}
		}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		// �α��� ĳ���� ����ȭ��� ���� CombineParts �� ���ϱ⶧���� ������ ��Ʋ�������� ������ �尩�޽� ���̵� �ϴ� ���� ������ ȣ���Ѵ�.
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
				// �Ϲݸ��̽��� ĳ���������� ������ ��찡 ������.
				// ���̽��ִϸ� ����ؼ� ���̽� ��Ʈ���ϱ��� ���� �������ϴ� ��, ���ҽ��ܿ��� ������ �ʹ� �������Ƿ� ������ �ùķ��̼� �Ⱦ��°� ���� �ʴ´�.
				//hWeapon->GetObjectHandle()->DisableSimulation();
				//hWeapon->GetObjectHandle()->SetRenderType( RT_TARGET );
			}
		}
	}
	// �󱼻�ǰ�̶� �Ӹ���� ��ǰ.
	else if( ItemType == ITEMTYPE_FACIAL || ItemType == ITEMTYPE_HAIRDRESS )
	{
		int nParam = pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
		AttachItem( nParam );

		// ���� ��ü���� �Ÿ��� ���� �������� �ʱ� ������, Hair�ٲܰ�� ĳ�ø��ڸ� �ٽ� �������� Hair���� �� �ڿ� �������ǰ� �Ѵ�.
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
			// �α��� ĳ���� ����ȭ��� ���� CombineParts �� ���ϱ⶧���� ������ ��Ʋ�������� ������ �尩�޽� ���̵� �ϴ� ���� ������ ȣ���Ѵ�.
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

			//pActor->CombineParts();		// �����ƹ�Ÿ���� �Ĺ��� ���� �ʴ´�.
			//pClonePlayerActor->SetHP( 1 );		// ��� �ڵ忡�� HP 0�����ΰɷ� ���� �Ǵ��ϴ� ���� �־ �н����� ����� ������ 1 �־��ش�.
			//hClonePlayer->Show( true );	// ����Ÿ�ٿ� �׸���� �ؾ��ϱ� ������,
			

			//MAPartsBody *pActorPartsBody = dynamic_cast<MAPartsBody*>(hClonePlayer.GetPointer());
			//pActorPartsBody->SetIgnoreDeleteWaitOnepiece( true );
			//AddObject( pActorPartsBody->GetObjectHandle() );
			//AddObject( pActorPartsBody->GetCombineObject() );	// �Ĺ��ο�����Ʈ�� ����(�Ӹ�)�� ���Ⱑ ������.
			//SetRenderFrameCount( -1 );	// -1�� �����صθ� ������Ʈ Ǯ�������� ��� �������Ѵ�.
		}
	

		//// �� ������ ��ũ�� ó���κп��� ������ ��ġ�� ���ؿ� ��ũ�� �ڽĵ��� ��� ����� �ϴµ�,
		//// ���μ����� ������ �ʱ� ������ �̷��� ������ �ʱ���ġ�� �����س��ƾ��Ѵ�.
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