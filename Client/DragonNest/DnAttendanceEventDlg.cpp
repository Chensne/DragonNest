#include "StdAfx.h"
#include "DnAttendanceEventDlg.h"
#include "DnTableDB.h"
#include "DnItem.h"
#include "DnMissionRewardCoinTooltipDlg.h"
#include "DnItemSlotButton.h"
#include "DnItemTask.h"
#include "DnCashShopTask.h"
#include "DnCommonTask.h"
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//////////////////////////////////////////////////////////////////////////
// CDnAttendanceEventDlg
//////////////////////////////////////////////////////////////////////////

CDnAttendanceEventDlg::CDnAttendanceEventDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pAttendanceEventRewardListBox( NULL )
, m_pStaticCheckAttendanceDay( NULL )
, m_pStaticEventDate( NULL )
{
}

CDnAttendanceEventDlg::~CDnAttendanceEventDlg()
{
}

void CDnAttendanceEventDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "Event_CheckDlg.ui" ).c_str(), bShow );
}

void CDnAttendanceEventDlg::InitialUpdate()
{
	m_pAttendanceEventRewardListBox = GetControl<CEtUIListBoxEx>( "ID_LISTBOXEX_LIST" );
	m_pStaticCheckAttendanceDay = GetControl<CEtUIStatic>( "ID_TEXT_DAY" );
	m_pStaticEventDate = GetControl<CEtUIStatic>( "ID_TEXT_TIME" );
}

void CDnAttendanceEventDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		SetAttendanceEventReward();
	else
	{
		if( m_pAttendanceEventRewardListBox )
		{
			for( int i=0; i<m_pAttendanceEventRewardListBox->GetSize(); i++ )
			{
				CDnAttendanceEventItemDlg* pItem = m_pAttendanceEventRewardListBox->GetItem<CDnAttendanceEventItemDlg>( i );
				if( pItem )
					pItem->Show( false );
			}
			m_pAttendanceEventRewardListBox->DeselectItem();
		}
	}

	CDnCustomDlg::Show( bShow );
}

void CDnAttendanceEventDlg::SetAttendanceEventReward()
{
	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TATTENDANCEEVENT );
	if( pSox == NULL ) 
		return;

	CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>( CTaskManager::GetInstance().GetTask( "CommonTask" ) );
	if( pCommonTask == NULL ) 
		return;

	std::map<CommonVariable::Type::eCode, CommonVariable::Data>::iterator iter = pCommonTask->GetVariableData().find( CommonVariable::Type::AttendanceEvent );
	if( iter == pCommonTask->GetVariableData().end() ) 
	{
		m_pStaticCheckAttendanceDay->SetText( L"" );
		m_pStaticEventDate->SetText( L"" );

		return;	
	}

	int nOngoingDay = static_cast<int>( iter->second.biValue >> 32 );
	int nPassDay = static_cast<int>( iter->second.biValue & 0xffffffff );
	WORD wDoneDay = LOWORD(nPassDay);	

	nPassDay = wDoneDay;

	std::wstring strText;
	strText = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7731 ), nOngoingDay );
	m_pStaticCheckAttendanceDay->SetText( strText.c_str() );

	m_pAttendanceEventRewardListBox->RemoveAllItems();

	for( int i=0; i<pSox->GetItemCount(); i++ )
	{
		int nTableID = pSox->GetItemID( i );

		bool bEnable = pSox->GetFieldFromLablePtr( nTableID, "_ON" )->GetInteger() == 1 ? true : false;
		if( !bEnable )
			continue;

		int nStartDate = atoi( pSox->GetFieldFromLablePtr( nTableID, "_Start_Date" )->GetString() );
		int nEndDate = atoi( pSox->GetFieldFromLablePtr( nTableID, "_End_Date" )->GetString() );

		strText = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7733 ), 
							nStartDate / 10000, ( nStartDate % 10000 ) / 100, ( nStartDate % 10000 ) % 100,
							nEndDate / 10000, ( nEndDate % 10000 ) / 100, ( nEndDate % 10000 ) % 100 );
		m_pStaticEventDate->SetText( strText.c_str() );

		int nAttendanceDate = pSox->GetFieldFromLablePtr( nTableID, "_Date_Count" )->GetInteger();
		bool bRewardHide = pSox->GetFieldFromLablePtr( nTableID, "_IconHide" )->GetInteger() == 1 ? true : false;
		int nMailRewardID = pSox->GetFieldFromLablePtr( nTableID, "_MailID" )->GetInteger();
		int nCashRewardID = pSox->GetFieldFromLablePtr( nTableID, "_CashMailID" )->GetInteger();

		bool bCheck = false;
		if( nAttendanceDate == nOngoingDay )	// 연속 출석일 당일 출석일 경우 체크
			bCheck = true;

		if( nAttendanceDate >= nOngoingDay && nAttendanceDate >= nPassDay )	// 현재 연속 출석일 보다 크고 이미 달성한 날짜 보다 크면 출력
		{
			CDnAttendanceEventItemDlg* pItem = m_pAttendanceEventRewardListBox->AddItem<CDnAttendanceEventItemDlg>();
			if( pItem )
				pItem->SetCheckAttendanceReward( nAttendanceDate, bCheck, bRewardHide, nMailRewardID, nCashRewardID );
		}
	}
}

void CDnAttendanceEventDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_CANCEL" ) || IsCmdControl( "ID_BT_CLOSE" ) || IsCmdControl( "ID_BUTTON0" ) )
			Show( false );
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

//////////////////////////////////////////////////////////////////////////
// CDnAttendanceEventItemDlg
//////////////////////////////////////////////////////////////////////////

CDnAttendanceEventItemDlg::CDnAttendanceEventItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_nRewardCoin( 0 )
, m_pRewardCoinTooltipDlg( NULL )
, m_bShowQuestion( false )
{
	memset( m_pItem, 0, sizeof(m_pItem) );
}

CDnAttendanceEventItemDlg::~CDnAttendanceEventItemDlg()
{
	SAFE_DELETE( m_pRewardCoinTooltipDlg );
	for( int i=0; i<MAX_ATTENDANCEEVENT_REWARD_ITEM; i++ )
		SAFE_DELETE( m_pItem[i] );
}

void CDnAttendanceEventItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Event_CheckListDlg.ui" ).c_str(), bShow );
}

void CDnAttendanceEventItemDlg::InitialUpdate()
{
	m_pRewardCoinTooltipDlg = new CDnMissionRewardCoinTooltipDlg;
	m_pRewardCoinTooltipDlg->Initialize( false );

	char szControl[32] = {0,};
	for( int i=0; i<MAX_ATTENDANCEEVENT_REWARD_ITEM; i++ )
	{
		sprintf_s( szControl, 32, "ID_LIST_SLOT%d", i );
		CDnItemSlotButton* pRewardItem = GetControl<CDnItemSlotButton>( szControl );
		if( pRewardItem )
			pRewardItem->SetSlotType( ST_ITEM_NONE );
	}
}

void CDnAttendanceEventItemDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		CEtUIStatic* pStaticTitle = GetControl<CEtUIStatic>( "ID_TEXT_TITLE" );
		if( pStaticTitle )
			pStaticTitle->SetText( L"" );

		char szControl[32] = {0,};
		for( int i=0; i<MAX_ATTENDANCEEVENT_REWARD_ITEM; i++ )
		{
			SAFE_DELETE( m_pItem[i] );
			sprintf_s( szControl, 32, "ID_LIST_SLOT%d", i );
			CDnItemSlotButton* pRewardItem = GetControl<CDnItemSlotButton>( szControl );
			if( pRewardItem )
			{
				pRewardItem->ResetSlot();
				pRewardItem->Show( false );
			}
		}
		m_pRewardCoinTooltipDlg->Show( false );
	}

	CDnCustomDlg::Show( bShow );
}

void CDnAttendanceEventItemDlg::SetCheckAttendanceReward( int nAttendanceDate, bool bCheck, bool bRewardHide, int nMailRewardID, int nCashRewardID )
{
	CEtUIStatic* pStaticTitle = GetControl<CEtUIStatic>( "ID_TEXT_TITLE" );
	if( pStaticTitle )
	{
		std::wstring strTitle;
		strTitle = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7732 ), nAttendanceDate );
		pStaticTitle->SetText( strTitle.c_str() );
	}

	CEtUIStatic* pStatiCheck = GetControl<CEtUIStatic>( "ID_STATIC_CHECK" );
	if( pStatiCheck )
	{
		if( bCheck )
			pStatiCheck->Show( true );
		else
			pStatiCheck->Show( false );
	}

	char szControl[32] = {0,};
	if( !bCheck && bRewardHide )
		m_bShowQuestion = true;

	for( int i=0; i<MAX_ATTENDANCEEVENT_REWARD_ITEM; i++ )
	{
		sprintf_s( szControl, 32, "ID_STATIC_QUESTION%d", i );
		CEtUIStatic* pQuestionStatic = GetControl<CEtUIStatic>( szControl );
		if( pQuestionStatic )
			pQuestionStatic->Show( false );

		sprintf_s( szControl, 32, "ID_LIST_SLOT%d", i );
		CDnItemSlotButton* pRewardItem = GetControl<CDnItemSlotButton>( szControl );
		if( pRewardItem )
			pRewardItem->Show( true );
	}

	CEtUIStatic* pQuestionStatic = GetControl<CEtUIStatic>( "ID_STATIC_QUESTION4" );
	if( pQuestionStatic )
		pQuestionStatic->Show( false );

	m_nRewardCoin = CDnTableDB::GetInstance().GetMailInfo( nMailRewardID, CDnTableDB::MailInfoRewardCoin );
	CEtUIStatic* pStaticRewardCoin = GetControl<CEtUIStatic>( "ID_STATIC_COIN" );
	if( pStaticRewardCoin )
	{
		if( m_nRewardCoin > 0 )
		{
			if( m_bShowQuestion )
			{
				CEtUIStatic* pQuestionStatic = GetControl<CEtUIStatic>( "ID_STATIC_QUESTION0" );
				if( pQuestionStatic )
					pQuestionStatic->Show( true );

				pStaticRewardCoin->Show( false );
			}
			else
				pStaticRewardCoin->Show( true );
		}
		else
			pStaticRewardCoin->Show( false );
	}

	int nStartRewardSlotIndex = 0;
	if( m_nRewardCoin > 0 )
		nStartRewardSlotIndex = 1;
	int nStartIndex = 0;

	nStartIndex = SetRewardItem( nMailRewardID, nStartIndex, nStartRewardSlotIndex, false );
	SetRewardItem( nCashRewardID, nStartIndex, nStartRewardSlotIndex, true );
}

int CDnAttendanceEventItemDlg::SetRewardItem( int nMailRewardID, int nStartIndex, int nStartRewardSlotIndex, bool bCash )
{
	if( nMailRewardID <= 0 )
		return 0;

	char szControl[32] = {0,};
	for( int k=0; k<MAX_ATTENDANCEEVENT_REWARD_ITEM; k++, nStartIndex++ )
	{
		int nItemID = CDnTableDB::GetInstance().GetMailInfo( nMailRewardID, CDnTableDB::MailInfoRewardItemID, bCash, k );
		
		if( nItemID > 0 )
		{
			sprintf_s( szControl, 32, "ID_LIST_SLOT%d", nStartIndex + nStartRewardSlotIndex );
			CDnItemSlotButton* pRewardItem = GetControl<CDnItemSlotButton>( szControl );
			if( m_bShowQuestion )
			{
				sprintf_s( szControl, 32, "ID_STATIC_QUESTION%d", nStartIndex + nStartRewardSlotIndex );
				CEtUIStatic* pQuestionStatic = GetControl<CEtUIStatic>( szControl );
				if( pQuestionStatic )
					pQuestionStatic->Show( true );
				continue;
			}

			bool bMakeItem = false;
			TItemInfo Info;
			if( bCash )
			{
				const SCashShopItemInfo* pCashInfo = GetCashShopTask().GetItemInfo( nItemID );
				if( pCashInfo ) 
				{
					if( CDnItem::MakeItemInfo( pCashInfo->presentItemId, pCashInfo->count, Info ) )
						bMakeItem = true;
				}
			}
			else
			{
				if( CDnItem::MakeItemInfo( nItemID, 1, Info ) ) 
					bMakeItem = true;
			}

			if( bMakeItem )
			{
				if( MAX_ATTENDANCEEVENT_REWARD_ITEM <= nStartIndex )
					break;

				m_pItem[nStartIndex] = CDnItemTask::GetInstance().CreateItem( Info );
				if( m_pItem[nStartIndex] )
				{
					int nItemCount = CDnTableDB::GetInstance().GetMailInfo( nMailRewardID, CDnTableDB::MailInfoRewardItemCount, bCash, k );
					pRewardItem->SetItem( m_pItem[nStartIndex], nItemCount );
				}
			}
		}
		else
			break;
	}

	return nStartIndex;
}

void CDnAttendanceEventItemDlg::Process( float fElapsedTime )
{
	if( !IsShow() )
		return;

	CDnCustomDlg::Process( fElapsedTime );

	if( m_nRewardCoin == 0 ) return;
	if( m_bShowQuestion ) return;

	float fMouseX, fMouseY;
	GetScreenMouseMovePoints( fMouseX, fMouseY );
	fMouseX -= GetXCoord();
	fMouseY -= GetYCoord();

	CEtUIStatic* pStaticRewardCoin = GetControl<CEtUIStatic>( "ID_STATIC_COIN" );
	if( pStaticRewardCoin && pStaticRewardCoin->IsEnable() )
	{
		SUICoord uiCoord;
		pStaticRewardCoin->GetUICoord( uiCoord );
		if( uiCoord.IsInside( fMouseX, fMouseY ) )
		{
			m_pRewardCoinTooltipDlg->SetRewardCoin( m_nRewardCoin );
			GetScreenMouseMovePoints( fMouseX, fMouseY );
			m_pRewardCoinTooltipDlg->SetPosition( fMouseX, fMouseY );
			SUICoord DlgCoord;
			m_pRewardCoinTooltipDlg->GetDlgCoord( DlgCoord );
			static float fXRGap(4.0f/DEFAULT_UI_SCREEN_WIDTH);
			if( (DlgCoord.Right()+fXRGap) > GetScreenWidthRatio() )
				DlgCoord.fX -= (DlgCoord.Right()+fXRGap - GetScreenWidthRatio());
			m_pRewardCoinTooltipDlg->SetDlgCoord( DlgCoord );
			m_pRewardCoinTooltipDlg->Show( true );
		}
		else
			m_pRewardCoinTooltipDlg->Show( false );
	}
}
