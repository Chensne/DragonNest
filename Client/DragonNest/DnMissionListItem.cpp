#include "StdAfx.h"
#include "DnMissionListItem.h"
#include "DnTableDB.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnMissionAppellationTooltipDlg.h"
#include "DnMissionDailyQuestDlg.h"
#include "DnMissionRewardCoinTooltipDlg.h"
#include "DnCashShopTask.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMissionListItem::CDnMissionListItem( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pName(NULL)
, m_pDescription(NULL)
, m_pRewardPoint(NULL)
, m_pRewardAppellation(NULL)
, m_pMissionIcon(NULL)
, m_pAppellationTooltip(NULL)
, m_pRewardCoinTooltipDlg(NULL)
, m_pInfo(NULL)
, m_pCompleteMark(NULL)
, m_pCompleteText(NULL)
, m_pDailyProgress(NULL)
, m_pDailyCount(NULL)
, m_pDailyCountBack(NULL)
#ifdef PRE_MOD_MISSION_HELPER
, m_pParentListBoxEx(NULL)
#else
, m_pParentDailyDialog(NULL)
#endif
, m_pCheckBox(NULL)
#ifdef PRE_MOD_MISSION_HELPER
, m_pStaticFlag(NULL)
#endif
{
	for( int i = 0; i < CDnMissionTask::RewardItem_Amount; ++i )
	{
		m_pItem[i] = NULL;
		m_pRewardItem[i] = NULL;
		m_pRewardMoney[i] = NULL;
	}
}

CDnMissionListItem::~CDnMissionListItem(void)
{
	SAFE_DELETE( m_pAppellationTooltip );
	SAFE_DELETE( m_pRewardCoinTooltipDlg );
	for( int i = 0; i < CDnMissionTask::RewardItem_Amount; ++i )
		SAFE_DELETE( m_pItem[i] );
}

void CDnMissionListItem::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MissionListDlg.ui" ).c_str(), bShow );

	m_pAppellationTooltip = new CDnMissionAppellationTooltipDlg;
	m_pAppellationTooltip->Initialize( false );
	m_pRewardCoinTooltipDlg = new CDnMissionRewardCoinTooltipDlg;
	m_pRewardCoinTooltipDlg->Initialize( false );
}

void CDnMissionListItem::InitialUpdate()
{
	m_pName = GetControl<CEtUIStatic>( "ID_STATIC_TITLE" );
	m_pDescription = GetControl<CEtUIStatic>( "ID_STATIC_SUBTITLE" );
	m_pRewardPoint = GetControl<CEtUIStatic>( "ID_POINT" );
	m_pRewardAppellation = GetControl<CEtUIStatic>( "ID_STATIC1" );
	m_pMissionIcon = GetControl<CDnMissionIconStatic>( "ID_MISSION_ICON" );
	m_pCheckBox = GetControl<CEtUICheckBox>( "ID_CHECKBOX0" );
	m_pCompleteMark = GetControl<CEtUIStatic>( "ID_MISSION_COMPLETION" );
	m_pCompleteText = GetControl<CEtUIStatic>( "ID_TEXT_COMPLETION" );
	m_pDailyProgress = GetControl<CEtUIProgressBar>( "ID_MISSION_GAUGE" );
	m_pDailyCount = GetControl<CEtUIStatic>( "ID_MISSION_COUNT" );
	m_pDailyCountBack = GetControl<CEtUIStatic>( "ID_STATIC0" );
#ifdef PRE_MOD_MISSION_HELPER
	m_pStaticFlag = GetControl<CEtUIStatic>( "ID_STATIC_FLAG" );
#endif

	char szLabel[32];
	for( int i = 0; i < CDnMissionTask::RewardItem_Amount; ++i )
	{
		sprintf_s(szLabel, 32, "ID_ITEM%d", i+1);
		m_pRewardItem[i] = GetControl<CDnItemSlotButton>( szLabel );
		m_pRewardItem[i]->SetSlotType( ST_MISSIONREWARD );

		sprintf_s(szLabel, 32, "ID_STATIC_COIN%d", i);
		m_pRewardMoney[i] = GetControl<CEtUIStatic>( szLabel );
	}
}

void CDnMissionListItem::SetInfo( CDnMissionTask::MissionInfoStruct *pInfo )
{
	// 33138 이슈
	bool bAchieve = !pInfo->bAchieve;

	m_pInfo = pInfo;
	m_pMissionIcon->SetIconID( pInfo->nIcon );
	m_pName->SetText( pInfo->szTitle );
	m_pDescription->SetText( pInfo->szSubTitle );

	WCHAR wszTemp[64];
	wsprintf( wszTemp, L"%d", pInfo->nRewardPoint );
	m_pRewardPoint->SetText( wszTemp );

#ifndef _FINAL_BUILD
	if( GetAsyncKeyState( VK_LSHIFT )&0x8000 ) {
		swprintf_s( wszTemp, _countof(wszTemp), L"MissionID : %d", GetItemID() );
		m_pName->SetText( wszTemp );
	}
#endif

	tstring wszAppellation;
	if( pInfo->nRewardAppellationID > 0 ) {
		m_pRewardAppellation->Show( true );
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );
		if( pSox->IsExistItem( pInfo->nRewardAppellationID ) ) {
			wszAppellation = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( pInfo->nRewardAppellationID, "_NameID" )->GetInteger() );
		}
	}
	else m_pRewardAppellation->Show( false );
	m_pRewardAppellation->SetText( wszAppellation );

	// nArrayIndex == -1 이면 Daily/WeeklyMission 입니다.
	if( pInfo->nArrayIndex == -1 &&
		( eDailyMissionType::DAILYMISSION_DAY == pInfo->nType 
		|| eDailyMissionType::DAILYMISSION_WEEK == pInfo->nType
		|| eDailyMissionType::DAILYMISSION_WEEKENDEVENT == pInfo->nType
#ifdef PRE_ADD_MONTHLY_MISSION
		|| eDailyMissionType::DAILYMISSION_MONTH == pInfo->nType
#endif 
		) ) 
	{
		m_pCheckBox->Show( true );
		m_pDailyProgress->Show( true );
		m_pDailyCount->Show( true );
		m_pDailyCountBack->Show( true );

		wsprintf( wszTemp, L"%d / %d", pInfo->nCounter, pInfo->nRewardCounter );
		m_pDailyCount->SetText( wszTemp );
		float fProgress = 0.f;
		if( pInfo->nRewardCounter > 0 ) fProgress = 100.f / pInfo->nRewardCounter * pInfo->nCounter;
		m_pDailyProgress->SetProgress( fProgress );
	}
	else {
#ifdef PRE_MOD_MISSION_HELPER
		m_pCheckBox->Show( true );
#else
		m_pCheckBox->Show( false );
#endif
		m_pDailyProgress->Show( false );
		m_pDailyCount->Show( false );
		m_pDailyCountBack->Show( false );
	}

	// 데이터 확인.
	// 미션보상에서 캐시템보상이 설정되어있을때 일반템보상이나 돈이 들어있으면 안된다고 한다.
	bool bRewardCashItem = false;
	for( int i=0; i<CDnMissionTask::RewardItem_Amount; i++ ) {
		if( pInfo->nRewardCashItemSN[i] > 0 ) {
			bRewardCashItem = true;
			break;
		}
	}

#ifndef _FINAL_BUILD
	bool bRewardItem = false;
	for( int i=0; i<CDnMissionTask::RewardItem_Amount; i++ ) {
		if( pInfo->nRewardItemID[i] > 0 ) {
			bRewardItem = true;
			break;
		}
	}
	if( (bRewardCashItem && bRewardItem) || (bRewardCashItem && pInfo->nRewardCoin) )
	{
		_ASSERT(0&&"미션보상에 캐시템과 일반 아이템, 돈이 같이 셋팅되어있습니다. 데이터를 확인하세요.");
	}
#endif

	if( bRewardCashItem )
	{
		// 캐시템 보상
		for( int i = 0; i < CDnMissionTask::RewardItem_Amount; ++i )
		{
			SAFE_DELETE( m_pItem[i] );
			m_pRewardItem[i]->Show( false );
			m_pRewardMoney[i]->Show( false );
			if( pInfo->nRewardCashItemSN[i] > 0 ) {
				const SCashShopItemInfo* pCashInfo = GetCashShopTask().GetItemInfo( pInfo->nRewardCashItemSN[i] );
				if( pCashInfo ) {
					TItemInfo Info;
					if( CDnItem::MakeItemInfo( pCashInfo->presentItemId, pCashInfo->count, Info ) ) {
						m_pItem[i] = CDnItemTask::GetInstance().CreateItem( Info );

						if (m_pItem[i]) {
							m_pItem[i]->SetCashItemSN( pInfo->nRewardCashItemSN[i] );
							m_pRewardItem[i]->SetItem( m_pItem[i],  pInfo->nRewardCashItemCount[i] );
							m_pRewardItem[i]->Show( true );
							m_pRewardItem[i]->SetRegist( bAchieve );
						}
					}
				}
			}
		}
	}
	else
	{
		// 일반템 + 돈 보상
		int nLastRewardItemIndex = -1;
		for( int i = 0; i < CDnMissionTask::RewardItem_Amount; ++i )
		{
			SAFE_DELETE( m_pItem[i] );
			m_pRewardItem[i]->Show( false );
			m_pRewardMoney[i]->Show( false );
			if( pInfo->nRewardItemID[i] > 0 ) {
				TItemInfo Info;
				if( CDnItem::MakeItemInfo( pInfo->nRewardItemID[i], 1, Info ) ) {
					m_pItem[i] = CDnItemTask::GetInstance().CreateItem( Info );

					if (m_pItem[i]) {
						m_pRewardItem[i]->SetItem( m_pItem[i], pInfo->nRewardItemCount[i] );
						m_pRewardItem[i]->Show( true );
						m_pRewardItem[i]->SetRegist( bAchieve );
						nLastRewardItemIndex = i;
					}
				}
			}
		}

		if( pInfo->nRewardCoin ) {
#ifndef _FINAL_BUILD
			if( nLastRewardItemIndex + 1 >= CDnMissionTask::RewardItem_Amount ) {
				_ASSERT(0&&"미션의 보상아이템이 3개 다 들어있는데 보상으로 돈을 또 줍니다. 데이터를 확인하세요.");
			}
#endif
			if( 0 <= nLastRewardItemIndex + 1 && nLastRewardItemIndex + 1 < CDnMissionTask::RewardItem_Amount ) {
				m_pRewardMoney[nLastRewardItemIndex + 1]->Show( true );
				m_pRewardMoney[nLastRewardItemIndex + 1]->Enable( !bAchieve );
			}
		}
	}

	m_pName->Enable( !bAchieve );
	m_pDescription->Enable( !bAchieve );
	m_pRewardPoint->Enable( !bAchieve );
	m_pRewardAppellation->Enable( true ); // 이건 그냥 Enable상태라야 잘 보인다고 한다.
	m_pMissionIcon->Enable( !bAchieve );

#if defined(PRE_ADD_68196)
	m_pCompleteText->Show( false );
	m_pCompleteMark->Show( false );
#else
	m_pCompleteText->Show( !bAchieve );
	m_pCompleteMark->Show( !bAchieve );
#endif	// #if defined(PRE_ADD_68196)

#ifdef PRE_MOD_MISSION_HELPER
	m_pCheckBox->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7874 ) );
	int nArrayIndex = GetItemID();
	if( nArrayIndex >= 0 && CDnMissionTask::GetInstance().IsRegisterNotifier( CDnMissionTask::GetInstance().GetNotifierType( pInfo ), nArrayIndex ) )
	{
		SetCheckBox( true );
		m_pName->Enable( true );
	}
	DWORD dwTextColor = 0xFFFFFFFF;
	if( IsChecked() ) dwTextColor = 0xFF78D746;
	m_pName->SetTextColor( dwTextColor );
	m_pStaticFlag->Show( IsChecked() );
#endif
}

void CDnMissionListItem::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_CHECKBOX_CHANGED ) {
		if( IsCmdControl( "ID_CHECKBOX0" ) ) {
			if( ((CEtUICheckBox*)pControl)->IsChecked() ) {
#ifdef PRE_MOD_MISSION_HELPER
				if( CDnMissionTask::IsActive() ) {
					ASSERT( GetItemID() >= 0 );
					CDnMissionTask::GetInstance().RegisterNotifier( CDnMissionTask::GetInstance().GetNotifierType( m_pInfo ), m_pInfo->nNotifierID );

					CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7875 ), textcolor::YELLOW, 4.0f );

					m_pName->Enable( true );
					m_pName->SetTextColor( 0xFF78D746 );
					m_pStaticFlag->Show( true );

					if( CDnMissionTask::GetInstance().IsMissionNotifierFull() ) {
						if( m_pParentListBoxEx ) {
							int nSize = m_pParentListBoxEx->GetSize();
							for( int i = 0; i < nSize; i++ ) {
								CDnMissionListItem *pListItem = m_pParentListBoxEx->GetItem<CDnMissionListItem>( i );
								if( pListItem && pListItem->IsChecked() == false )
									pListItem->EnableCheckBox( false );
							}
						}
					}
				}
#else
				if( m_pParentDailyDialog ) {
					m_pParentDailyDialog->InvertCheckOthers( this );
				}
#endif
			}
			else {
#ifdef PRE_MOD_MISSION_HELPER
				if( CDnMissionTask::IsActive() ) {
					CDnMissionTask::GetInstance().RemoveNotifier( CDnMissionTask::GetInstance().GetNotifierType( m_pInfo ), m_pInfo->nNotifierID );

					m_pName->Enable( m_pInfo->bAchieve );
					m_pName->SetTextColor( 0xFFFFFFFF );
					m_pStaticFlag->Show( false );

					if( CDnMissionTask::GetInstance().IsMissionNotifierFull() == false ) {
						if( m_pParentListBoxEx ) {
							int nSize = m_pParentListBoxEx->GetSize();
							for( int i = 0; i < nSize; i++ ) {
								CDnMissionListItem *pListItem = m_pParentListBoxEx->GetItem<CDnMissionListItem>( i );
								if( pListItem ) pListItem->EnableCheckBox( true );
							}
						}
					}
				}
#else
				CDnMissionTask::GetInstance().RemoveNotifier( m_pInfo->nNotifierID );
#endif
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMissionListItem::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );

	float fMouseX, fMouseY;
	// ListBoxEx ItemDlg는 자체적으로 MsgProc를 돌지 않기때문에 이 함수로 구할 수 없다.
	//GetMouseMovePoints( fMouseX, fMouseY );
	GetScreenMouseMovePoints( fMouseX, fMouseY );
	fMouseX -= GetXCoord();
	fMouseY -= GetYCoord();
	bool bMouseEnter(false);
	SUICoord uiCoord;
	m_pRewardAppellation->GetUICoord(uiCoord);
	if( uiCoord.IsInside(fMouseX, fMouseY) )
		bMouseEnter = true;

	if( m_pRewardAppellation->IsShow() && bMouseEnter && m_pInfo && m_pInfo->nRewardAppellationID > 0 ) {
		m_pAppellationTooltip->SetAppellationInfo( m_pInfo->nRewardAppellationID );
		GetScreenMouseMovePoints( fMouseX, fMouseY );
		m_pAppellationTooltip->SetPosition( fMouseX, fMouseY );
		SUICoord DlgCoord;
		m_pAppellationTooltip->GetDlgCoord(DlgCoord);
		DlgCoord.fX -= DlgCoord.fWidth / 2;
		DlgCoord.fY += CDnMouseCursor::GetInstance().GetCursorHeight() / 2;
		static float fXRGap(4.0f/DEFAULT_UI_SCREEN_WIDTH);
		if( (DlgCoord.Right()+fXRGap) > GetScreenWidthRatio() )
			DlgCoord.fX -= (DlgCoord.Right()+fXRGap - GetScreenWidthRatio());
		m_pAppellationTooltip->SetDlgCoord(DlgCoord);
		m_pAppellationTooltip->Show( true );
	}
	else m_pAppellationTooltip->Show( false );


	// 보상Coin 컨트롤 위치 체크
	bMouseEnter = false;
	for( int i = 0; i < CDnMissionTask::RewardItem_Amount; ++i )
	{
		if( !m_pRewardMoney[i]->IsShow() ) continue;
		m_pRewardMoney[i]->GetUICoord(uiCoord);
		if( uiCoord.IsInside(fMouseX, fMouseY) )
			bMouseEnter = true;
	}

	if( bMouseEnter ) {
		m_pRewardCoinTooltipDlg->SetRewardCoin( m_pInfo->nRewardCoin );
		GetScreenMouseMovePoints( fMouseX, fMouseY );
		m_pRewardCoinTooltipDlg->SetPosition( fMouseX, fMouseY );
		SUICoord DlgCoord;
		m_pRewardCoinTooltipDlg->GetDlgCoord(DlgCoord);
		static float fXRGap(4.0f/DEFAULT_UI_SCREEN_WIDTH);
		if( (DlgCoord.Right()+fXRGap) > GetScreenWidthRatio() )
			DlgCoord.fX -= (DlgCoord.Right()+fXRGap - GetScreenWidthRatio());
		m_pRewardCoinTooltipDlg->SetDlgCoord( DlgCoord );
		m_pRewardCoinTooltipDlg->Show( true );
	}
	else m_pRewardCoinTooltipDlg->Show( false );
}

void CDnMissionListItem::SetElementDialogShowState( bool bShow )
{
	CEtUIDialog::SetElementDialogShowState( bShow );
	if( !bShow && m_pAppellationTooltip && m_pAppellationTooltip->IsShow() )
		m_pAppellationTooltip->Show( false );
	if( !bShow && m_pRewardCoinTooltipDlg && m_pRewardCoinTooltipDlg->IsShow() )
		m_pRewardCoinTooltipDlg->Show( false );
}

void CDnMissionListItem::SetCheckBox( bool bCheck )
{
	if( m_pCheckBox ) {
		m_pCheckBox->SetChecked( bCheck, false );
	}
}

#ifdef PRE_MOD_MISSION_HELPER
bool CDnMissionListItem::IsChecked()
{
	if( m_pCheckBox ) {
		return m_pCheckBox->IsChecked();
	}
	return false;
}

void CDnMissionListItem::EnableCheckBox( bool bEnable )
{
	if( m_pCheckBox ) {
		m_pCheckBox->Enable( bEnable );
		m_pCheckBox->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, bEnable ? 7874 : 7876 ) );
	}
}
#endif

void CDnMissionListItem::ShowCheckBox( bool bShow )
{
	if( m_pCheckBox ) {
		m_pCheckBox->Show( bShow );
	}
}

int CDnMissionListItem::GetItemID()
{
	return m_pInfo ? m_pInfo->nNotifierID : -1;		// Daily/Weekly nArrayIndex는 전부 -1 이므로, 테이블과 매칭되는 nItemID 로 사용하자.
}
