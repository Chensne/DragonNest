#include "StdAfx.h"
#include "DnDarklairDungeonEnterDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "DnPartyTask.h"
#include "DnDungeonExpectDlg.h"
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
#include "TimeSet.h"
#endif	// #if defined(PRE_ADD_CHALLENGE_DARKLAIR)
#include "DnDarklairEnterLevelDlg.h"
#include "PartySendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDarklairDungeonEnterDlg::CDnDarklairDungeonEnterDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pStaticName( NULL )
, m_pTextureDungeonImage( NULL )
, m_pTextureDungeonMap( NULL )
, m_pStaticBottomBack( NULL )
, m_pDarklairEnterLevelDlg( NULL )
, m_pDungeonExpectDlg( NULL )
, m_nIndexFloor( -1 )
{
}

CDnDarklairDungeonEnterDlg::~CDnDarklairDungeonEnterDlg()
{
	m_vecDarklairFloorInfo.clear();
	SAFE_RELEASE_SPTR( m_hIntroImage );
	SAFE_RELEASE_SPTR( m_hIntroMap );

	SAFE_DELETE( m_pDarklairEnterLevelDlg );
	SAFE_DELETE( m_pDungeonExpectDlg );
}

void CDnDarklairDungeonEnterDlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>( "ID_STATIC_DUNGEONNAME" );
	m_pTextureDungeonImage = GetControl<CEtUITextureControl>( "ID_TEXTURE_DUNGEONIMAGE" );
	m_pTextureDungeonImage->GetUICoord( m_uiTextureImageBack );
	m_pTextureDungeonMap = GetControl<CEtUITextureControl>( "ID_TEXTUREL_MAP" );
	m_pStaticBottomBack = GetControl<CEtUIStatic>( "ID_BOTTOM_BLACK" );
	m_pStaticBottomBack->GetUICoord( m_uiBottomBlackBack );

	char szButtonFloor[32]={0};
	stDarklairFloorInfo sFloorInfo;

	for( int i=0; i<MAX_DARKLAIR_FLOOR; i++ )
	{
		sprintf_s( szButtonFloor, 32, "ID_BUTTON_DUN_%02d", i+1 );
		sFloorInfo.m_pButtonFloor = GetControl<CDnStageEnterButton>( szButtonFloor );
		sFloorInfo.m_pButtonFloor->SetDarklairEnterButton();
		sFloorInfo.Show( false );
		m_vecDarklairFloorInfo.push_back( sFloorInfo );
	}

	m_pDarklairEnterLevelDlg = new CDnDarklairEnterLevelDlg( UI_TYPE_CHILD, this, DAKRLAIR_ENTER_DIALOG );
	m_pDarklairEnterLevelDlg->Initialize( false );

	m_pDungeonExpectDlg = new CDnDungeonExpectDlg( UI_TYPE_CHILD, this );
	m_pDungeonExpectDlg->Initialize( false );
}

void CDnDarklairDungeonEnterDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DarkLairEnterDlg.ui" ).c_str(), bShow );
}

void CDnDarklairDungeonEnterDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		CEtUIRadioButton *pRadioButton = (CEtUIRadioButton*)pControl;
		if( !pRadioButton ) return;

		if( strstr( pControl->GetControlName(), "ID_BUTTON_DUN" ) )
		{
			//m_nIndexFloor = pRadioButton->GetTabID();
			//if( m_nIndexFloor >= 0 )
			
			//일단 선택된 스테이지? ID를 얻어서 기존 저장된? 스테이지ID와 비교해서 다른 경우만 값을 변경 해준다.
			int nSelectedFloor = pRadioButton->GetTabID();
			bool bChangeFloor = false;
			if (nSelectedFloor != m_nIndexFloor)
			{
				m_nIndexFloor = nSelectedFloor;
				bChangeFloor = true;
			}

			//스테이지 선택이 변경이 되고, 정상적인 값인 경우 갱신한다.
			if (bChangeFloor && m_nIndexFloor >= 0)
			{
				::SendSelectDungeonInfo( m_vecDarklairFloorInfo[m_nIndexFloor].m_nMapIndex, 0 );
				m_pDarklairEnterLevelDlg->UpdateFloorInfo( m_vecDarklairFloorInfo[m_nIndexFloor].m_nMapIndex, m_vecDarklairFloorInfo[m_nIndexFloor].m_nMinLevel, m_vecDarklairFloorInfo[m_nIndexFloor].m_cPermitFlag );
				stDarklairFloorInfo* pInfo = &m_vecDarklairFloorInfo[m_nIndexFloor];
				if( pInfo )
				{
					if( m_pDungeonExpectDlg )
						m_pDungeonExpectDlg->UpdateDungeonExpectReward( pInfo->m_nMapIndex, 0 );
				}
			}
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnDarklairDungeonEnterDlg::SetSelectDungeonInfo( int nMapIndex, int nDifficult )
{
	if( !m_pDarklairEnterLevelDlg ) return;

	int nMinLevel = 0;
	for( int i=0; i<static_cast<int>( m_vecDarklairFloorInfo.size() ); i++ )
	{
		if( nMapIndex == m_vecDarklairFloorInfo[i].m_nMapIndex )
		{
			nMinLevel = m_vecDarklairFloorInfo[i].m_nMinLevel;
			break;
		}
	}

	m_pDarklairEnterLevelDlg->UpdateFloorInfo( nMapIndex, nMinLevel );

	if( m_pDungeonExpectDlg )
		m_pDungeonExpectDlg->UpdateDungeonExpectReward( nMapIndex, 0 );
}

void CDnDarklairDungeonEnterDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	if( bShow )
	{
		UpdateButtonState();
		UpdateControlPos();
	}
	else
		m_nIndexFloor = -1;

	CEtUIDialog::Show( bShow );
}

void CDnDarklairDungeonEnterDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnDarklairDungeonEnterDlg::OnBlindOpen()
{
	CDnLocalPlayerActor::LockInput(true);
	ShowChildDialog( m_pDarklairEnterLevelDlg, true );
	ShowChildDialog( m_pDungeonExpectDlg, true );

	m_pDarklairEnterLevelDlg->SetCallback( GetCallBack() );
	SetCallback( NULL );
}

void CDnDarklairDungeonEnterDlg::OnBlindOpened()
{
	GetInterface().ShowChatDialog();
	Show( true );
}

void CDnDarklairDungeonEnterDlg::OnBlindClose()
{
	ShowChildDialog( m_pDarklairEnterLevelDlg, false );
	ShowChildDialog( m_pDungeonExpectDlg, false );
	Show( false );
}

void CDnDarklairDungeonEnterDlg::OnBlindClosed()
{
	GetInterface().OpenBaseDialog();
	CDnLocalPlayerActor::LockInput( false );
}

void CDnDarklairDungeonEnterDlg::Refresh( CDnWorld::DungeonGateStruct* pGateInfo )
{
	ASSERT( pGateInfo && "CDnDungeonEnterDlg::SetGateInfo, pGateInfo is NULL!" );

	// 던전 이름 셋팅
	m_pStaticName->SetText( pGateInfo->szMapName.c_str() );

	for( int i=0; i<(int)m_vecDarklairFloorInfo.size(); i++ )
	{
		m_vecDarklairFloorInfo[i].Show( false );
	}

	SetDungeonImage( pGateInfo->szGateEnterImage );

	CDnWorld::DungeonGateStruct *pGateStruct( NULL );

	if( static_cast<int>(pGateInfo->pVecMapList.size()) == 0 )
	{
		m_vecDarklairFloorInfo[0].SetInfo( pGateInfo->szMapName.c_str(), pGateInfo->szDesc.c_str(), pGateInfo->nMinPartyCount, pGateInfo->nMaxPartyCount, 
											pGateInfo->nMapIndex, pGateInfo->Environment, pGateInfo->nMinLevel, pGateInfo->nMaxLevel, true, pGateInfo->PermitFlag );
		m_vecDarklairFloorInfo[0].SetMinLevel( pGateInfo->nMinLevel );
		m_vecDarklairFloorInfo[0].Show( true );
		m_vecDarklairFloorInfo[0].Enable( true );
	}
	else
	{
		DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TDLMAP );
		if( pSox == NULL )
			return;

		DNTableFileFormat*  pMapSOX = GetDNTable( CDnTableDB::TMAP );
		if( pMapSOX == NULL )
			return;

		DNTableFileFormat*  pDungeonEnterSOX = GetDNTable( CDnTableDB::TDUNGEONENTER );
		if( pDungeonEnterSOX == NULL )
			return;

		DNTableFileFormat* pGateSox = GetDNTable( CDnTableDB::TMAPGATE );
		if (pGateSox == NULL)
			return;

		int nBeforeTotalRound = 0;
		for( int i=0; i<(int)pGateInfo->pVecMapList.size(); i++ )
		{
			pGateStruct = (CDnWorld::DungeonGateStruct*)pGateInfo->pVecMapList[i];
			if( !pGateStruct ) continue;
			if( !pSox->IsExistItem( pGateStruct->nMapIndex ) ) continue;

			wchar_t wszStr[128] = {0};
#if defined( PRE_ADD_CHALLENGE_DARKLAIR )
			bool bChallengeType = pSox->GetFieldFromLablePtr( pGateStruct->nMapIndex, "_ChallengeType" )->GetInteger() == 0 ? false : true;
			if( true == bChallengeType )
			{
				int nStringID = pSox->GetFieldFromLablePtr( pGateStruct->nMapIndex, "_GameModeName" )->GetInteger();
				swprintf_s( wszStr, _countof(wszStr), L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringID ) );
			}
			else
#endif	// #if defined( PRE_ADD_CHALLENGE_DARKLAIR )
			{
				int nStageCount = pSox->GetFieldFromLablePtr( pGateStruct->nMapIndex, "_StageCount" )->GetInteger();
				swprintf_s( wszStr, _countof(wszStr), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 828 ), nBeforeTotalRound + 1, nBeforeTotalRound + nStageCount );
				nBeforeTotalRound += nStageCount;
			}

			bool bCheckAchieve = false;
			int nDungeonEnterTableID = pMapSOX->GetFieldFromLablePtr( pGateStruct->nMapIndex, "_EnterConditionTableID" )->GetInteger();
			if( nDungeonEnterTableID > 0 ) 
			{
				if( pDungeonEnterSOX->IsExistItem( nDungeonEnterTableID ) )
				{
					int nNeedMission = pDungeonEnterSOX->GetFieldFromLablePtr( nDungeonEnterTableID, "_NeedMission" )->GetInteger();
					if( nNeedMission != 0 )
					{
						DWORD dwCount = GetMissionTask().GetMissionCount();
						for( DWORD i=0; i<dwCount; i++ )
						{
							CDnMissionTask::MissionInfoStruct* pInfo = GetMissionTask().GetMissionInfo( i );

							if( !pInfo ) continue;
							if( pInfo->nNotifierID == nNeedMission )
							{
								if( pInfo->bAchieve )
									bCheckAchieve = true;

								break;
							}
						}
					}
					else
						bCheckAchieve = true;
				}
			}

			bool bCheckLevel = false;
			if( pGateStruct->nMinLevel == -1 && pGateStruct->nMaxLevel == -1 )
			{
				bCheckLevel = true;
			}
			else if( pGateStruct->nMinLevel != -1 && pGateStruct->nMaxLevel == -1 )
			{
				if( CDnActor::s_hLocalActor->GetLevel() >= pGateInfo->nMinLevel )
					bCheckLevel = true;
			}
			else if( pGateStruct->nMinLevel == -1 && pGateStruct->nMaxLevel != -1 )
			{
				if( CDnActor::s_hLocalActor->GetLevel() <= pGateStruct->nMaxLevel )
					bCheckLevel = true;
			}
			else
			{
				if( CDnActor::s_hLocalActor->GetLevel() >= pGateStruct->nMinLevel && CDnActor::s_hLocalActor->GetLevel() <= pGateStruct->nMaxLevel )
					bCheckLevel = true;
			}

			bool bEnable = false;
			if( bCheckAchieve && bCheckLevel )
				bEnable = true;

#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
			if( true == bEnable && true == IsCloseGateByTime( pGateStruct->nMapIndex ) )
				bEnable = false;
#endif	// #if defined(PRE_ADD_CHALLENGE_DARKLAIR)	

			if( bCheckLevel )
			{
				m_vecDarklairFloorInfo[i].SetInfo( pGateStruct->szMapName.c_str(), wszStr, pGateStruct->nMinPartyCount, pGateStruct->nMaxPartyCount, 
					pGateStruct->nMapIndex, pGateStruct->Environment, -1, -1, bCheckAchieve, pGateStruct->PermitFlag );
			}
			else
			{
				m_vecDarklairFloorInfo[i].SetInfo( pGateStruct->szMapName.c_str(), wszStr, pGateStruct->nMinPartyCount, pGateStruct->nMaxPartyCount, 
					pGateStruct->nMapIndex, pGateStruct->Environment, pGateStruct->nMinLevel, pGateStruct->nMaxLevel, bCheckAchieve, pGateStruct->PermitFlag );
			}
			m_vecDarklairFloorInfo[i].SetMinLevel( pGateStruct->nMinLevel );
			m_vecDarklairFloorInfo[i].Show( true );
			m_vecDarklairFloorInfo[i].Enable( bEnable );
		}
	}

	CDnPartyTask::PartyRoleEnum partyState = GetPartyTask().GetPartyRole();
	if( partyState == CDnPartyTask::MEMBER )
	{
		for( int i=0; i<static_cast<int>( m_vecDarklairFloorInfo.size() ); i++ )
		{
			m_vecDarklairFloorInfo[i].Enable( false );
		}
	}
}

void CDnDarklairDungeonEnterDlg::SetDungeonImage( const std::string &DungeonImageName )
{
	SAFE_RELEASE_SPTR( m_hIntroImage );
	SAFE_RELEASE_SPTR( m_hIntroMap );

	if( DungeonImageName.empty() )
	{
		m_hIntroImage = LoadResource( CEtResourceMng::GetInstance().GetFullName( "DungeonImage01.dds" ).c_str(), RT_TEXTURE );
	}
	else
	{
		m_hIntroImage = LoadResource( CEtResourceMng::GetInstance().GetFullName( DungeonImageName.c_str() ).c_str(), RT_TEXTURE );
	}

	if( m_hIntroImage )
		m_pTextureDungeonImage->SetTexture( m_hIntroImage, 0, 0, m_hIntroImage->Width(), m_hIntroImage->Height() );

	m_hIntroMap = LoadResource( CEtResourceMng::GetInstance().GetFullName( "DungeonRoadmap02.dds" ).c_str(), RT_TEXTURE );
	if( m_hIntroMap )
		m_pTextureDungeonMap->SetTexture( m_hIntroMap, 0, 0, m_hIntroMap->Width(), m_hIntroMap->Height() );
}

void CDnDarklairDungeonEnterDlg::UpdateControlPos()
{
	SUICoord textureCoord, bottomCoord;

	textureCoord.fWidth = m_uiTextureImageBack.fWidth * GetScreenWidthRatio();
	textureCoord.fHeight = m_uiTextureImageBack.fHeight * GetScreenWidthRatio();

	textureCoord.fX = m_uiTextureImageBack.fX - ((textureCoord.fWidth - m_uiTextureImageBack.fWidth)/2.0f);
	textureCoord.fY = m_uiTextureImageBack.fY - ((textureCoord.fHeight - m_uiTextureImageBack.fHeight)/2.0f);

	m_pTextureDungeonImage->SetUICoord( textureCoord );

	bottomCoord.fWidth = m_uiBottomBlackBack.fWidth * GetScreenWidthRatio();
	bottomCoord.fHeight = m_uiBottomBlackBack.fHeight * GetScreenWidthRatio();

	bottomCoord.fX = m_uiBottomBlackBack.fX - ((bottomCoord.fWidth - m_uiBottomBlackBack.fWidth)/2.0f);
	bottomCoord.fY = m_uiBottomBlackBack.fY;

	m_pStaticBottomBack->SetUICoord( bottomCoord );
}

void CDnDarklairDungeonEnterDlg::UpdateButtonState()
{
	CDnPartyTask::PartyRoleEnum partyState = GetPartyTask().GetPartyRole();
	if( partyState == CDnPartyTask::MEMBER )
		return;

	int nPartyTargetMapIndex = 0;
	if( GetPartyTask().GetPartyRole() == CDnPartyTask::LEADER )
		nPartyTargetMapIndex = GetPartyTask().GetPartyTargetStageIdx();

	int nMinLevel(-1);
	int nMaxIndex(0);

	for( int i=0; i<static_cast<int>( m_vecDarklairFloorInfo.size() ); i++ )
	{
		if( !m_vecDarklairFloorInfo[i].IsShow() )
			continue;

		if( !m_vecDarklairFloorInfo[i].IsEnable() )
			continue;

		// 만약 맵인덱스가 타겟으로 정한 것과 같다면, 바로 선택.
		if( m_vecDarklairFloorInfo[i].m_nMapIndex == nPartyTargetMapIndex )
		{
			nMaxIndex = i;
			break;
		}

		if( m_vecDarklairFloorInfo[i].m_nMinLevel > nMinLevel )
		{
			nMinLevel = m_vecDarklairFloorInfo[i].m_nMinLevel;
			nMaxIndex = i;
		}
	}

	m_vecDarklairFloorInfo[nMaxIndex].m_pButtonFloor->SetChecked( true );
}

#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
bool CDnDarklairDungeonEnterDlg::IsCloseGateByTime( const int nMapIndex )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAPGATE );

	std::vector<int> nVecDeungeonItemList;
	pSox->GetItemIDListFromField( "_MapIndex", nMapIndex, nVecDeungeonItemList );

	for( DWORD i=0; i<nVecDeungeonItemList.size(); i++ ) 
	{
		int nItemID = nVecDeungeonItemList[i];
		if (nItemID <= 0)
			continue;

		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAPGATE );
		char* pszDateTime = pSox->GetFieldFromLablePtr( nItemID, "_DateTime" )->GetString();
		if(!pszDateTime )
			continue;

		std::vector<string> vGateDateTime;
		TokenizeA(pszDateTime , vGateDateTime, ":");

		int nType = 0, nAttr1 = 0, nAttr2 = 0, nAttr3 = 0;
		if (vGateDateTime.size() > 0)
		{
			CTimeSet LocalSet;
			nType = atoi(vGateDateTime[0].c_str());
			if (nType == MapGateCondition::oDailyHours && vGateDateTime.size() == 3)	// 시간
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());
				nAttr2 = atoi(vGateDateTime[2].c_str());

				if (nAttr1 <= LocalSet.GetHour() && LocalSet.GetHour() < nAttr2)
					continue;
				else
					return true;
			}
			else if (nType == MapGateCondition::oDayOfWeek && vGateDateTime.size() == 2) // 요일
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());		
				if (LocalSet.GetDayOfWeek() != nAttr1)
					return true;
			}
			else if (nType == MapGateCondition::oDayOfWeekAndHours && vGateDateTime.size() == 4) // 요일 & 시간
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());
				nAttr2 = atoi(vGateDateTime[2].c_str());
				nAttr3 = atoi(vGateDateTime[3].c_str());

				if (LocalSet.GetDayOfWeek() != nAttr1)
					return true;

				if (nAttr2 <= LocalSet.GetHour() && LocalSet.GetHour() < nAttr3)
					continue;
				else
					return true;
			}
			else if (nType == MapGateCondition::oDayOfWeekDuringFatigueTime && vGateDateTime.size() == 2) // 요일 피로도 시간
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());
				if( LocalSet.GetDayOfWeek() != nAttr1 && LocalSet.GetDayOfWeek() != (nAttr1+1)%7 ) 
					return true;

				DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TSCHEDULE );
				if( !pSox )
					return true;

				int nItemID = 2;	//피로도 리셋
				if( !pSox->IsExistItem( nItemID ) ) return true;
				int nHour = (char)pSox->GetFieldFromLablePtr( nItemID, "_Hour" )->GetInteger();
				int nMinute = (char)pSox->GetFieldFromLablePtr( nItemID, "_Minute" )->GetInteger();

				if( LocalSet.GetDayOfWeek() == nAttr1 )	//초기화 시간 이후 인지 쳌
				{
					if( LocalSet.GetHour() >= nHour && LocalSet.GetMinute() >= nMinute )
						continue;
					else
						return true;
				}
				else if( LocalSet.GetDayOfWeek() == (nAttr1+1)%7 )	//초기화 시간 전 인지 쳌
				{
					if( LocalSet.GetHour() < nHour || (LocalSet.GetMinute() < nMinute && LocalSet.GetHour() <= nHour) )
						continue;
					else
						return true;
				}
			}
			else if (nType == MapGateCondition::cMonthlyHours && vGateDateTime.size() == 4) // 매월 해당일 & 시간만 *클로즈*
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());
				nAttr2 = atoi(vGateDateTime[2].c_str());
				nAttr3 = atoi(vGateDateTime[3].c_str());

				if (LocalSet.GetDay() != nAttr1)
					continue;

				if (LocalSet.GetHour() >= nAttr2 && LocalSet.GetHour() < nAttr3)
					return true;
				else
					continue;
			}
		}
	}

	return false;
}
#endif	// #if defined(PRE_ADD_CHALLENGE_DARKLAIR)

