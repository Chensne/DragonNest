#include "StdAfx.h"
#include "DnDarklairClearRewardDlg.h"
#include "DnDungeonClearMoveDlg.h"
#include "DnInterfaceDlgID.h"
#include "DnLocalPlayerActor.h"
#include "DnDLGameTask.h"
#include "TaskManager.h"
#include "GameSendPacket.h"
#include "MAPartsBody.h"
#include "DnTableDB.h"
#include "GlobalValue.h"
#include "DnDarklairClearImp.h"
#include "DnItem.h"

#include "DnStageClearMoveDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnDarklairClearRewardDlg::CDnDarklairClearRewardDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pFace(NULL)
, m_pStaticAllRound( NULL )
, m_pName(NULL)
, m_pStaticRemainSelectBox(NULL)
, m_pTime(NULL)
, m_pTimeBase(NULL)
, m_fElapsedTime(0.0f)
, m_nCount(0)
, m_bTimer(false)
, m_nCurPhase(0)
, m_nBoxCount(0)
, m_Phase(PHASE_NONE)
, m_CurBoxIdxPresented(0)
, m_fPhaseTime(0.f)
, m_pStageClearMoveDlg(NULL)
, m_pIdentifySlot(NULL)
, m_pIdentifySlotBase(NULL)
, m_pIdentifyMsg(NULL)
, m_pIdentifyItem(NULL)
, m_bRequestIdentify(false)
{
	memset( m_pIdentifyBack, 0, sizeof(m_pIdentifyBack) );
	memset( m_pTextureRound, 0, sizeof(m_pTextureRound) );
	m_nSelectRemainCount = 0;
}

CDnDarklairClearRewardDlg::~CDnDarklairClearRewardDlg()
{
	SAFE_DELETE( m_pIdentifyItem );
	SAFE_RELEASE_SPTR( m_hRoundTexture );
	SAFE_DELETE(m_pStageClearMoveDlg);

	for ( int i = 0 ; i < SoundType::MAX ; i++ )
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIndexArray[i] );
	}
}

void CDnDarklairClearRewardDlg::InitialUpdate()
{
	m_pFace = GetControl<CDnGaugeFace>("ID_FACE");
	m_pFace->SetFaceType( FT_PLAYER );

	m_pStaticAllRound = GetControl<CEtUIStatic>("ID_TEXT_ARC");
	m_pStaticRemainSelectBox = GetControl<CEtUIStatic>("ID_SELECT");

	m_pTextureRound[0] = GetControl<CEtUITextureControl>("ID_TEXTURE_ROUND0");
	m_pTextureRound[1] = GetControl<CEtUITextureControl>("ID_TEXTURE_ROUND1");

	m_pName = GetControl<CEtUIStatic>("ID_NAME");
	m_pTime = GetControl<CEtUIStatic>("ID_TIME");
	m_pTimeBase = GetControl<CEtUIStatic>("ID_STATIC_TIMEBASE");

	m_pIdentifySlot = GetControl<CDnItemSlotButton>("ID_ITEM_IDENTIFY");
	m_pIdentifySlotBase = GetControl<CEtUIStatic>("ID_IDENTIFY_BASE1");
	m_pIdentifyMsg = GetControl<CEtUIStatic>("ID_STATIC_MSG");
	m_pIdentifyBack[0] = GetControl<CEtUIStatic>("ID_IDENTIFY_BASE0");
	m_pIdentifyBack[1] = GetControl<CEtUIStatic>("ID_IDENTIFY_LINE");
	m_pIdentifySlot->Show( false );
	m_pIdentifySlotBase->Show( false );
	m_pIdentifyMsg->Show( false );
	m_pIdentifyBack[0]->Show( false );
	m_pIdentifyBack[1]->Show( false );

	m_sBonusBoxButton[0].m_pBonusBoxButton = GetControl<CDnBonusBoxButton>("ID_BONUSBOX_00_00");
	m_sBonusBoxButton[1].m_pBonusBoxButton = GetControl<CDnBonusBoxButton>("ID_BONUSBOX_00_01");
	m_sBonusBoxButton[2].m_pBonusBoxButton = GetControl<CDnBonusBoxButton>("ID_BONUSBOX_00_02");
	m_sBonusBoxButton[3].m_pBonusBoxButton = GetControl<CDnBonusBoxButton>("ID_BONUSBOX_00_03");

	m_sBonusBoxButton[0].m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEM_00_00");
	m_sBonusBoxButton[1].m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEM_00_01");
	m_sBonusBoxButton[2].m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEM_00_02");
	m_sBonusBoxButton[3].m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEM_00_03");

	char szBase[32]={0};
	char szFace[32]={0};
	char szRank[32]={0};
	char szName[32]={0};
	char szBonusBox[32]={0};
	char szItem[32]={0};

	for( int i=0; i<3; i++ )
	{
		SStageClearList stageClearList;

		sprintf_s( szBase, 32, "ID_BASE_%02d", i );
		sprintf_s( szFace, 32, "ID_FACE_%02d", i );
		sprintf_s( szName, 32, "ID_NAME_%02d", i );

		stageClearList.m_pBase = GetControl<CEtUIStatic>(szBase);
		stageClearList.m_pFace = GetControl<CDnGaugeFace>(szFace);
		stageClearList.m_pFace->SetFaceType( FT_PLAYER );
		stageClearList.m_pName = GetControl<CEtUIStatic>(szName);

		for( int j=0; j<4; j++ )
		{
			sprintf_s( szBonusBox, 32, "ID_BONUSBOX_%02d_%02d", i+1, j );
			sprintf_s( szItem, 32, "ID_ITEM_%02d_%02d", i+1, j );

			stageClearList.m_sBonusBoxButton[j].m_pBonusBoxButton = GetControl<CDnBonusBoxButton>(szBonusBox);
			stageClearList.m_sBonusBoxButton[j].m_pBonusBoxButton->SetUseHandle( false );
			stageClearList.m_sBonusBoxButton[j].m_pItemSlotButton = GetControl<CDnItemSlotButton>(szItem);
		}

		stageClearList.Show( false );
		m_vecStageClearList.push_back(stageClearList);
	}

	m_pStageClearMoveDlg = new CDnStageClearMoveDlg(UI_TYPE_CHILD, this, DUNGEON_CLEAR_MOVE_DIALOG);
	m_pStageClearMoveDlg->Initialize(false);

	m_hRoundTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "DarkRareRankIcon.dds" ).c_str(), RT_TEXTURE );

	m_nSoundIndexArray[SoundType::RECOMPENSE_WINDOW]	= CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10021 ), false, false );
	m_nSoundIndexArray[SoundType::BOX_MOUSEON]			= CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10022 ), false, false );
	m_nSoundIndexArray[SoundType::BOX_CLICK]			= CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10023 ), false, false );
	m_nSoundIndexArray[SoundType::BOX_OPEN]				= CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10024 ), false, false );
}

void CDnDarklairClearRewardDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DarkRareClearDlg.ui" ).c_str(), bShow );
}

void CDnDarklairClearRewardDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		if( strstr( pControl->GetControlName(), "ID_BONUSBOX_00_" ) && bTriggeredByUser )
		{
			CDnBonusBoxButton *pButton = (CDnBonusBoxButton*)pControl;
			if( m_Phase == PHASE_NONE && m_nSelectRemainCount > 0 )
			{
				int nSelectBoxIndex = pButton->GetTabID();
				SendSelectRewardItem( (char)nSelectBoxIndex );

				m_nSelectRemainCount--;
				if( m_nSelectRemainCount == 0 ) {
					for( int i=0; i<4; i++ ) {
						if( m_sBonusBoxButton[i].m_pBonusBoxButton )
							m_sBonusBoxButton[i].m_pBonusBoxButton->Enable( false );
					}
					m_pStaticRemainSelectBox->SetText( L"" );
				}
				else {
					pButton->Enable( false );
					WCHAR wszStr[64];
					swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2377 ), m_nSelectRemainCount );
					m_pStaticRemainSelectBox->SetText( wszStr );
				}

				pButton->SetChecked( false );
				CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndexArray[SoundType::BOX_CLICK] );
			}
			return;
		}
	}
	if( nCommand == EVENT_BUTTON_RCLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BONUSBOX_00_" ) && bTriggeredByUser ) {
			CDnBonusBoxButton *pButton = (CDnBonusBoxButton*)pControl;
			if( m_pIdentifyItem && !m_bRequestIdentify && pButton->GetBonusBoxButtonType() == CDnBonusBoxButton::typeQuestion && m_nSelectRemainCount > 0 )
			{
				SendIdentifyRewardItem( (char)pButton->GetTabID() );
				m_bRequestIdentify = true;
			}
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnDarklairClearRewardDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndexArray[SoundType::RECOMPENSE_WINDOW] );
		SetClearInfo();

		m_bTimer = true;
		m_nCount = 5;
		m_fElapsedTime = 0.f;
		m_pTimeBase->Show( true );

		SetAllQuestionBox();
	}
	else
	{
		m_nCurPhase = 0;
		m_nBoxCount = 0;

		for( int i=0; i<(int)m_vecStageClearList.size(); i++ )
		{
			for( int j=0; j<4; j++ )
			{
				m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->Show( false );
				m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetItemLight( false );
				m_vecStageClearList[i].m_sBonusBoxButton[j].m_pItemSlotButton->Show( false );
			}

			m_vecStageClearList[i].Show( false );
			m_vecStageClearList[i].Clear();
		}

		for( int i=0; i<4; i++ )
		{
			m_sBonusBoxButton[i].m_pBonusBoxButton->Show( false );
			m_sBonusBoxButton[i].m_pBonusBoxButton->SetItemLight( false );
			m_sBonusBoxButton[i].m_pItemSlotButton->Show( false );
		}
	}

	CEtUIDialog::Show( bShow );
}

void CDnDarklairClearRewardDlg::Process( float fElapsedTime )
{
	if( m_bTimer )
	{
		if( m_nCount > 0 )
		{
			m_fElapsedTime += fElapsedTime;

			if( m_fElapsedTime > 1.0f )
			{
				m_nCount--;

				wchar_t wszTemp[64] = {0};
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2367 ), m_nCount );
				m_pTime->SetText( wszTemp );

				m_fElapsedTime -= 1.0f;
			}
		}
		else
		{
			m_bTimer = false;
			m_pTime->SetText( L" " );
			m_pStaticRemainSelectBox->SetText( L"" );
			m_pTimeBase->Show( false );
		}
	}
	else if( m_fPhaseTime > 0.0f )
	{
		m_fPhaseTime -= fElapsedTime;
	}
	else
	{
		if (m_Phase != PHASE_NONE)
		{
			if (m_Phase == PHASE_CLOSED_BOX)
			{
				SetClosedBonusBox(m_CurBoxIdxPresented);
				m_fPhaseTime = 0.1f;
			}
			else if (m_Phase == PHASE_OPEN_BOX)
			{
				SetOpenBonusBox(m_CurBoxIdxPresented);
				m_fPhaseTime = 0.1f;
			}

			m_CurBoxIdxPresented++;
			if (m_CurBoxIdxPresented >= m_nBoxCount)
			{
				m_Phase = PHASE_NONE;
				m_CurBoxIdxPresented = 0;
			}
		}
	}

	CEtUIDialog::Process( fElapsedTime );	
}

void CDnDarklairClearRewardDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );
}

void CDnDarklairClearRewardDlg::OnBlindClose()
{
	SetCallback( NULL );
	Show( false );
}

void CDnDarklairClearRewardDlg::OnBlindClosed()
{
	GetInterface().OpenBaseDialog();

	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
	if( !pGameTask )
	{
		CDebugSet::ToLogFile( "CDnDarklairClearRewardDlg::OnBlindClosed, pGameTask is NULL!" );
		return;
	}

	CDnDarklairClearImp *pClearImp = (CDnDarklairClearImp *)pGameTask->GetStageClearImp();
	if( !pClearImp ) return;

	CDnLocalPlayerActor::LockInput(!pClearImp->GetDungeonClearValue().bClear);
	if( !pClearImp->GetDungeonClearValue().bClear )
	{
		CDnMouseCursor::GetInstance().ShowCursor( true ); // #56823 락인풋 된 상태에서 마우스커서를 토글 할 수 없기 때문에 커서를 보여줍니다.
		CDnMouseCursor::GetInstance().ReleaseShowRefCnt(); // #74125 마우스Ref카운트가 올라가는 문제때문에, 블라인드 종료시 Ref리셋
	}
}

#if defined ( PRE_ADD_NAMEDITEM_SYSTEM )
void CDnDarklairClearRewardDlg::ShowBonusBox()
{
	m_pStaticRemainSelectBox->SetText( L"" );
	m_Phase = PHASE_CLOSED_BOX;
	m_CurBoxIdxPresented = 0;

	SetRewardBoxOrRewardItemInfo(true);
	SetAllPartyBonusBox();
}
#endif

void CDnDarklairClearRewardDlg::OpenBonusBox()
{
#if defined ( PRE_ADD_NAMEDITEM_SYSTEM )
	SetRewardBoxOrRewardItemInfo(false);
#else
	m_pStaticRemainSelectBox->SetText( L"" );
	m_Phase = PHASE_CLOSED_BOX;
	m_CurBoxIdxPresented = 0;
	SetRewardItemInfo();
	SetAllPartyBonusBox();
#endif
}

void CDnDarklairClearRewardDlg::OpenBonusItem()
{
	m_Phase = PHASE_OPEN_BOX;
	m_CurBoxIdxPresented = 0;
	SetAllPartyOpenBonusBox();
}

void CDnDarklairClearRewardDlg::SetClearInfo()
{
	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
	if( !pGameTask )
	{
		CDebugSet::ToLogFile( "CDnDarklairClearRewardDlg::SetClearInfo, pGameTask is NULL!" );
		return;
	}

	CDnDarklairClearImp *pClearImp = (CDnDarklairClearImp *)pGameTask->GetStageClearImp();
	if( !pClearImp ) return;

	m_pStaticAllRound->Show( ( pClearImp->GetDungeonClearValue().nClearRound == ((CDnDLGameTask*)pGameTask)->GetTotalRound() ) ? true : false );
	std::vector<CDnDarklairClearImp::SDungeonClearInfo> &dungeonClearInfoList = pClearImp->GetDungeonClearInfoList();

	for( int i=0, k=0; i<(int)dungeonClearInfoList.size(); i++ )
	{
		if( dungeonClearInfoList[i].sBaseInfo.nSessionID == CDnBridgeTask::GetInstance().GetSessionID() )
		{
			m_pFace->SetFaceID( dungeonClearInfoList[i].nClassID-1 );

			EtTextureHandle hTexture = GetInterface().CloneMyPortrait();
			m_pFace->SetTexture( hTexture );

			int nRound = pClearImp->GetDungeonClearValue().nClearRound;
			if( nRound >= 10 ) m_pTextureRound[0]->SetTexture( m_hRoundTexture, ( nRound / 10 ) * 115, 0, 115, 150 );
			m_pTextureRound[1]->SetTexture( m_hRoundTexture, ( nRound % 10 ) * 115, 0, 115, 150 );

			m_pName->SetText( dungeonClearInfoList[i].strName );

			m_nSelectRemainCount = dungeonClearInfoList[i].sBaseInfo.cSelectBoxCount;
			WCHAR wszStr[64];
			swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2377 ), m_nSelectRemainCount );
			m_pStaticRemainSelectBox->SetText( wszStr );

			m_nBoxCount = dungeonClearInfoList[i].sBaseInfo.cShowBoxCount;

			/*
			for( int j=0; j<m_nBoxCount; j++ )
			{
				m_sBonusBoxButton[j].SetBoxInfo( dungeonClearInfoList[i].sBaseInfo.cRewardItemType[j], dungeonClearInfoList[i].sBaseInfo.nRewardItemID[j] );
			}
			*/
		}
		else
		{
			m_vecStageClearList[k].m_pFace->SetFaceID( dungeonClearInfoList[i].nClassID-1 );

			for( DWORD m=0; m<GetPartyTask().GetPartyCount(); m++ )
			{
				CDnPartyTask::PartyStruct *pInfo = GetPartyTask().GetPartyData(m);
				if( !pInfo ) continue;
				if( pInfo->nSessionID == dungeonClearInfoList[i].sBaseInfo.nSessionID ) 
				{
					MAPartsBody *pPartsBody = dynamic_cast< MAPartsBody * >( pInfo->hActor.GetPointer() );
					if( ( pPartsBody ) && ( pPartsBody->GetObjectHandle() ) )
					{
						int nAniIndex = pInfo->hActor->GetCachedAniIndex( "Normal_Stand" );
						if( nAniIndex == -1 ) nAniIndex = pInfo->hActor->GetCachedAniIndex( "Stand" );	// 유령일 경우 Normal_Stand가 없을 수 있다.
						if( nAniIndex != -1 )
						{
							m_vecStageClearList[k].m_pFace->UpdatePortrait( pPartsBody->GetObjectHandle(), false, false, nAniIndex, 0.0f );
						}
						break;
					}
				}
			}

			m_vecStageClearList[k].m_pName->SetText( dungeonClearInfoList[i].strName );
			m_vecStageClearList[k].m_nSessionID = dungeonClearInfoList[i].sBaseInfo.nSessionID;

			m_vecStageClearList[k].m_nBoxCount = dungeonClearInfoList[i].sBaseInfo.cShowBoxCount;

			/*
			for( int j=0; j<m_vecStageClearList[k].m_nBoxCount; j++ )
			{
				m_vecStageClearList[k].m_sBonusBoxButton[j].SetBoxInfo( dungeonClearInfoList[i].sBaseInfo.cRewardItemType[j], dungeonClearInfoList[i].sBaseInfo.nRewardItemID[j] );
			}
			*/

			m_vecStageClearList[k].Show( true );
			k++;
		}
	}
}

#if defined ( PRE_ADD_NAMEDITEM_SYSTEM )
void CDnDarklairClearRewardDlg::SetRewardBoxOrRewardItemInfo(bool bBoxInfo)
#else
void CDnDarklairClearRewardDlg::SetRewardItemInfo()
#endif
{
	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
	if( !pGameTask )
	{
		CDebugSet::ToLogFile( "CDnStageClearDlg::SetClearInfo, pGameTask is NULL!" );
		return;
	}

	CDnDarklairClearImp *pClearImp = (CDnDarklairClearImp *)pGameTask->GetStageClearImp();
	if( !pClearImp ) return;

	std::vector<CDnDarklairClearImp::SDungeonClearInfo> &dungeonClearInfoList = pClearImp->GetDungeonClearInfoList();

	for( int i=0; i<(int)dungeonClearInfoList.size(); i++ )
	{
		if( dungeonClearInfoList[i].sBaseInfo.nSessionID == CDnBridgeTask::GetInstance().GetSessionID() )
		{
			for( int j=0; j<m_nBoxCount; j++ )
			{
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
				if (bBoxInfo)
					m_sBonusBoxButton[j].SetBoxType( dungeonClearInfoList[i].cRewardItemType[j] );
				else
					m_sBonusBoxButton[j].SetBoxItem( dungeonClearInfoList[i].RewardItem[j] );
#else
				m_sBonusBoxButton[j].SetBoxInfo( dungeonClearInfoList[i].cRewardItemType[j], dungeonClearInfoList[i].RewardItem[j] );
#endif
			}
		}
		else
		{
			for( DWORD k=0; k<m_vecStageClearList.size(); k++ ) {
				if( m_vecStageClearList[k].m_nSessionID == dungeonClearInfoList[i].sBaseInfo.nSessionID ) {
					for( int j=0; j<m_vecStageClearList[k].m_nBoxCount; j++ )
					{
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
						if (bBoxInfo)
							m_vecStageClearList[k].m_sBonusBoxButton[j].SetBoxType( dungeonClearInfoList[i].cRewardItemType[j] );
						else
							m_vecStageClearList[k].m_sBonusBoxButton[j].SetBoxItem( dungeonClearInfoList[i].RewardItem[j] );
#else
						m_vecStageClearList[k].m_sBonusBoxButton[j].SetBoxInfo( dungeonClearInfoList[i].cRewardItemType[j], dungeonClearInfoList[i].RewardItem[j] );
#endif
					}
					break;
				}
			}
		}
	}
}

void CDnDarklairClearRewardDlg::SetAllQuestionBox()
{
	for( int i=0; i<m_nBoxCount; i++ )
	{
		m_sBonusBoxButton[i].m_pBonusBoxButton->SetQuestionBox();
		m_sBonusBoxButton[i].m_pBonusBoxButton->SetChecked( false );
		m_sBonusBoxButton[i].m_pBonusBoxButton->SetUseHandle( true );
		m_sBonusBoxButton[i].m_pBonusBoxButton->Show( true );
	}

	for( int i=0; i<(int)m_vecStageClearList.size(); i++ )
	{
		for( int j=0; j<m_vecStageClearList[i].m_nBoxCount; j++ )
		{
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetQuestionBox();
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetChecked( false );
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->Show( true );
		}
	}
	CheckIdentifyItem();
}

void CDnDarklairClearRewardDlg::SetClosedBonusBox(int idx)
{
	if (idx >= m_nBoxCount)
	{
		return;
	}

	m_sBonusBoxButton[idx].m_pBonusBoxButton->SetBonusBox( m_sBonusBoxButton[idx].m_nBoxType );
	m_sBonusBoxButton[idx].m_pBonusBoxButton->SetUseHandle( false );

	// test
	//m_vecStageClearList[2].m_sBonusBoxButton[idx].m_pBonusBoxButton->SetBonusBox( m_sBonusBoxButton[idx].m_nBoxType );
}

void CDnDarklairClearRewardDlg::SetAllPartyBonusBox()
{
	for( int i=0; i<m_nBoxCount; i++ )
	{
		m_sBonusBoxButton[i].m_pBonusBoxButton->Enable( true );
	}

	for( int i=0; i<(int)m_vecStageClearList.size(); i++ )
	{
		for( int j=0; j<m_vecStageClearList[i].m_nBoxCount; j++ )
		{
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->Enable( true );
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetBonusBox( m_vecStageClearList[i].m_sBonusBoxButton[j].m_nBoxType );
		}
	}
}

void CDnDarklairClearRewardDlg::SetAllBonusBox()
{
	for( int i=0; i<m_nBoxCount; i++ )
	{
		m_sBonusBoxButton[i].m_pBonusBoxButton->SetBonusBox( m_sBonusBoxButton[i].m_nBoxType );
		m_sBonusBoxButton[i].m_pBonusBoxButton->SetUseHandle( false );
		m_sBonusBoxButton[i].m_pBonusBoxButton->Enable( true );
	}

	for( int i=0; i<(int)m_vecStageClearList.size(); i++ )
	{
		for( int j=0; j<m_vecStageClearList[i].m_nBoxCount; j++ )
		{
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->Enable( true );
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetBonusBox( m_vecStageClearList[i].m_sBonusBoxButton[j].m_nBoxType );
		}
	}
}

void CDnDarklairClearRewardDlg::SetOpenBonusBox(int idx)
{
	if (idx >= m_nBoxCount)
	{
		return;
	}

	m_sBonusBoxButton[idx].m_pBonusBoxButton->SetOpenBonusBox( m_sBonusBoxButton[idx].m_nBoxType );
	//m_sBonusBoxButton[i].m_pBonusBoxButton->SetBonusBox( m_sBonusBoxButton[i].m_nBoxType );
	m_sBonusBoxButton[idx].m_pBonusBoxButton->Show( true );
	m_sBonusBoxButton[idx].m_pItemSlotButton->Show( true );

	if( m_sBonusBoxButton[idx].m_pBonusBoxButton->IsChecked() )
	{
		m_sBonusBoxButton[idx].m_pBonusBoxButton->SetItemLight( true );
	}
}

void CDnDarklairClearRewardDlg::SetAllPartyOpenBonusBox()
{
	for( int i=0; i<(int)m_vecStageClearList.size(); i++ )
	{
		for( int j=0; j<m_vecStageClearList[i].m_nBoxCount; j++ )
		{
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetOpenBonusBox( m_vecStageClearList[i].m_sBonusBoxButton[j].m_nBoxType );
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->Enable( true );
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->Show( true );
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pItemSlotButton->Show( true );

			if( m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->IsChecked() )
			{
				m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetItemLight( true );
			}
		}
	}
}

void CDnDarklairClearRewardDlg::SetAllOpenBonusBox()
{
	CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndexArray[SoundType::BOX_OPEN] );

	for( int i=0; i<m_nBoxCount; i++ )
	{
		m_sBonusBoxButton[i].m_pBonusBoxButton->SetOpenBonusBox( m_sBonusBoxButton[i].m_nBoxType );
		//m_sBonusBoxButton[i].m_pBonusBoxButton->SetBonusBox( m_sBonusBoxButton[i].m_nBoxType );
		m_sBonusBoxButton[i].m_pBonusBoxButton->Show( true );
		m_sBonusBoxButton[i].m_pItemSlotButton->Show( true );

		if( m_sBonusBoxButton[i].m_pBonusBoxButton->IsChecked() )
		{
			m_sBonusBoxButton[i].m_pBonusBoxButton->SetItemLight( true );
		}
	}

	for( int i=0; i<(int)m_vecStageClearList.size(); i++ )
	{
		for( int j=0; j<m_vecStageClearList[i].m_nBoxCount; j++ )
		{
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetOpenBonusBox( m_vecStageClearList[i].m_sBonusBoxButton[j].m_nBoxType );
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->Show( true );
			m_vecStageClearList[i].m_sBonusBoxButton[j].m_pItemSlotButton->Show( true );

			if( m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->IsChecked() )
			{
				m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetItemLight( true );
			}
		}
	}
}

void CDnDarklairClearRewardDlg::SelectBonusBox( int nSessionID, char cSelectIndex )
{
	if( nSessionID == CDnBridgeTask::GetInstance().GetSessionID() ) {
		for( int j=0; j<4; j++ ) {
			m_sBonusBoxButton[j].m_pBonusBoxButton->SetChecked( ( cSelectIndex & (char)( 0x01 << j ) ) ? true : false );
		}
	}
	for( int i=0; i<(int)m_vecStageClearList.size(); i++ )
	{
		if( m_vecStageClearList[i].m_nSessionID == nSessionID )
		{
			for( int j=0; j<4; j++ ) {
				m_vecStageClearList[i].m_sBonusBoxButton[j].m_pBonusBoxButton->SetChecked( ( cSelectIndex & (char)( 0x01 << j ) ) ? true : false );
			}
		}
	}
}

void CDnDarklairClearRewardDlg::CheckIdentifyItem()
{
	if( !CDnItemTask::IsActive() ) return;

	int nCount = CDnItemTask::GetInstance().FindItemCountFromItemType( ITEMTYPE_REWARDITEM_IDENTIFY );

	if( !nCount ) {
		m_pIdentifySlot->Show( false );
		m_pIdentifySlotBase->Show( false );
		m_pIdentifyMsg->Show( false );
		for( int i=0; i<2; i++ )
			m_pIdentifyBack[i]->Show( false );
	}
	else {
		m_pIdentifySlot->Show( true );
		m_pIdentifySlotBase->Show( true );
		m_pIdentifyMsg->Show( true );
		for( int i=0; i<2; i++ )
			m_pIdentifyBack[i]->Show( true );

		TItemInfo itemInfo;
		if( CDnItem::MakeItemInfo( REWARDIDENTITY_ITEMID, 1, itemInfo ) == false ) return;

		SAFE_DELETE( m_pIdentifyItem );
		m_pIdentifyItem = GetItemTask().CreateItem( itemInfo );
		m_pIdentifySlot->SetItem( m_pIdentifyItem, nCount );
	}
}

void CDnDarklairClearRewardDlg::OpenBonusBox( char cSlotIndex, char cBoxType )
{
	m_bRequestIdentify = false;
	if( m_pIdentifyItem ) {
		m_pIdentifyItem->PlayInstantUseSound();
	}
	CheckIdentifyItem();
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	m_sBonusBoxButton[cSlotIndex].SetBoxType(cBoxType);
#else
	TItem ItemInfo;
	memset( &ItemInfo, 0, sizeof(TItem) );
	m_sBonusBoxButton[cSlotIndex].SetBoxInfo( cBoxType, ItemInfo );
#endif
	m_sBonusBoxButton[cSlotIndex].m_pBonusBoxButton->SetBonusBox( m_sBonusBoxButton[cSlotIndex].m_nBoxType );
}