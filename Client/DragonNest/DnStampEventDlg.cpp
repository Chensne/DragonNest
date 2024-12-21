#include "StdAfx.h"

#ifdef PRE_ADD_STAMPSYSTEM

#include "GameOption.h"

#include "DnTableDB.h"
#include "DnItemTask.h"

#include "DnInterface.h"
#include "DnMainMenuDlg.h"

#include "DnStampEventListDlg.h"
#include "DnStampEventDlg.h"


using namespace StampSystem;

CDnStampEventDlg::CDnStampEventDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pListBoxEx = NULL;
	memset( m_pStaticWeekDay, 0, sizeof(m_pStaticWeekDay) );
}

CDnStampEventDlg::~CDnStampEventDlg()
{
	m_pListBoxEx->RemoveAllItems();
}

void CDnStampEventDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Event_DayCheckDlg.ui" ).c_str(), bShow );
}

void CDnStampEventDlg::InitialUpdate()
{
	m_pListBoxEx = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");

	char buf[64] = {0,};
	/*for( int i=0; i<AWEEK; ++i )
	{
		memset( buf, 0, 64 );
		sprintf_s( buf, 64, "ID_TEXT%d", i+1 );
		m_pStaticWeekDay[i] = GetControl<CEtUIStatic>( buf );
	}*/
	int idx = 2;
	for( int i=0; i<AWEEK-1; ++i )
	{
		memset( buf, 0, 64 );
		sprintf_s( buf, 64, "ID_TEXT%d", idx+i );
		m_pStaticWeekDay[i] = GetControl<CEtUIStatic>( buf );
	}
	m_pStaticWeekDay[AWEEK-1] = GetControl<CEtUIStatic>( "ID_TEXT1" );

	CEtUICheckBox * pCheck = GetControl<CEtUICheckBox>("ID_CHECKBOX_SHOW");
	if( pCheck )
		pCheck->SetChecked( CGameOption::GetInstance().m_nWeekDay == -1 ? false : true, false );
}

void CDnStampEventDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		if( CGameOption::GetInstance().m_nShowStampAlram != -1 && CGameOption::GetInstance().m_nShowStampAlram == m_StampData.weekDay )
		{
			CEtUICheckBox * pCheck = GetControl<CEtUICheckBox>("ID_CHECKBOX_SUMSHOW");			
			if( pCheck )
				pCheck->SetChecked( true );
		}
	}
	else
	{
		GetInterface().AddStamp( false, true );
	}

	if( m_pCallback )
	{
		m_pCallback->OnUICallbackProc( GetDialogID(), bShow?EVENT_DIALOG_SHOW:EVENT_DIALOG_HIDE, NULL );
	}
	CEtUIDialog::Show( bShow );
}

void CDnStampEventDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_CANCEL" ) || IsCmdControl("ID_BT_CLOSE") ) {
			Show( false );
		}
	}

	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsShow() )
		{
			// ���� �׸�����.
			if( IsCmdControl("ID_CHECKBOX_SHOW") )
			{
				CEtUICheckBox * pCheck = GetControl<CEtUICheckBox>("ID_CHECKBOX_SHOW");			
				if( pCheck )
				{
					if( pCheck->IsChecked() && ( CGameOption::GetInstance().m_nWeekDay == -1 || m_StampData.weekDay != CGameOption::GetInstance().m_nWeekDay ) )
					{
						CGameOption::GetInstance().m_nWeekDay = m_StampData.weekDay;
						CGameOption::GetInstance().Save();
					}
					else if( pCheck->IsChecked() == false && CGameOption::GetInstance().m_nWeekDay != -1 )
					{
						CGameOption::GetInstance().m_nWeekDay = -1;
						CGameOption::GetInstance().Save();
					}
				}
			}

			// �˸��� �׸�����.
			if( IsCmdControl( "ID_CHECKBOX_SUMSHOW" ) )
			{
				CEtUICheckBox * pCheck = GetControl<CEtUICheckBox>("ID_CHECKBOX_SUMSHOW");			
				if( pCheck )
				{
					if( pCheck->IsChecked() && ( CGameOption::GetInstance().m_nShowStampAlram == -1 || m_StampData.weekDay != CGameOption::GetInstance().m_nShowStampAlram ) )
					{
						CGameOption::GetInstance().m_nShowStampAlram = m_StampData.weekDay;
						CGameOption::GetInstance().Save();
						GetInterface().ShowStampAlarmDlg( false );
					}
					else if( pCheck->IsChecked() == false && CGameOption::GetInstance().m_nShowStampAlram != -1 )
					{
						CGameOption::GetInstance().m_nShowStampAlram = -1;
						CGameOption::GetInstance().Save();
					}
				}
			}
		}
	}
	

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


// ������� ����.
void CDnStampEventDlg::BuildList( INT64 startTime, int weekDay, bool * bComplete )
{
	DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TSTAMPCHALLENGE );
	if( !pTable )
		return;

	if( weekDay == - 1 )
		weekDay = m_StampData.weekDay;

	m_StampData.Init();	
	m_StampData.time = startTime;
	m_StampData.weekDay = weekDay;	
	if( bComplete )
		m_StampData.Allocate( Common::MaxChallengeCount * Common::MaxWeekDayCount, bComplete );

	DNTableCell * pCell = NULL;
	
	int size = pTable->GetItemCount();
	for( int i=0; i<size; ++i )
	{		
		int id = pTable->GetItemID( i );

		pCell = pTable->GetFieldFromLablePtr( id, "_StampStartDay" );
		if( !pCell || (startTime != _atoi64( pCell->GetString() )) )
			continue;

		STableStampData * pData = new STableStampData;
		pData->stampStartDay = startTime;			
		pData->wDay = weekDay;

		pCell = pTable->GetFieldFromLablePtr( id, "_NameID" );
		if( pCell )
			pData->strID = pCell->GetInteger();

		pCell = pTable->GetFieldFromLablePtr( id, "_DescriptionID" );
		if( pCell )
			pData->strDescriptID = pCell->GetInteger();

		pCell = pTable->GetFieldFromLablePtr( id, "_MailID4" );
		if( pCell )
			pData->items[0] = GetItem( pCell->GetInteger() );

		pCell = pTable->GetFieldFromLablePtr( id, "_MailID7" );
		if( pCell )
			pData->items[1] = GetItem( pCell->GetInteger() );

		pCell = pTable->GetFieldFromLablePtr( id, "_StampType" );
		if( pCell )
			pData->type = pCell->GetInteger();

		pCell = pTable->GetFieldFromLablePtr( id, "_MapID" );
		if( pCell )
			pData->mapID = pCell->GetInteger();
		
		pCell = pTable->GetFieldFromLablePtr( id, "_NpcID" );
		if( pCell )
			pData->npcID = pCell->GetInteger();

		pCell = pTable->GetFieldFromLablePtr( id, "_AssignmentID" );
		if( pCell )
		{	
			if( pData->type == 1 )
			{
				std::vector<std::string> tokens;
				TokenizeA( pCell->GetString(), tokens, ":" );			
				for (UINT i = 0; i < tokens.size(); ++i)
					pData->arrIndex.push_back( atoi( tokens[i].c_str() ) );
			}
			
			else if( pData->type == 2 )
			{
				pData->nMissionGroupID = atoi( pCell->GetString() );
			}
		}


		CDnStampEventListItem * pListItem = m_pListBoxEx->InsertItem<CDnStampEventListItem>( i );
		if( pListItem )
			pListItem->SetListItem( pData );		
	}

	// ����������ֱ�.
	SetAllStamp();

}


// ��ü����������ֱ�.
void CDnStampEventDlg::SetAllStamp()
{
	int idx = 0;
	int size = m_pListBoxEx->GetSize();
	
	for( int i=0; i<size; ++i )
	{
		CDnStampEventListItem * pListItem = m_pListBoxEx->GetItem<CDnStampEventListItem>( i );
		if( pListItem )
		{
			for( int k=0; k<AWEEK; ++k )
			{
				idx = i * AWEEK + k;
				if( m_StampData.bFlag )
					pListItem->SetStamp( k, m_StampData.bFlag[ idx ] );
			}
		}
	}
	
}

// ���������.
void CDnStampEventDlg::SetStamp( int idx, int wDay )
{
	// ���������� 1���ͽ����̴�.
	--idx;
	--wDay;

	if( idx < 0 || wDay < 0 )
		return;

	CDnStampEventListItem * pListItem = m_pListBoxEx->GetItem<CDnStampEventListItem>( idx );
	if( !pListItem )
		return;

	int index = idx * AWEEK + wDay;
	if( index < (Common::MaxWeekDayCount * Common::MaxChallengeCount) )
	{
		m_StampData.bFlag[ index ] = true;
		pListItem->SetStamp( wDay, true );
	}

}


// ����ID�� �����۹�ȯ.
CDnItem * CDnStampEventDlg::GetItem( int mailID )
{
	CDnItem * pItem = NULL;
	
	DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TMAIL );
	if( !pTable )
		return NULL;

	DNTableCell * pCell = NULL;

	int itemID = -1;
	pCell = pTable->GetFieldFromLablePtr( mailID, "_MailPresentItem1" );
	if( !pCell )
		return NULL;
	itemID = pCell->GetInteger();

	int itemCnt = 1;
	pCell = pTable->GetFieldFromLablePtr( mailID, "_Count1" );
	if( pCell )
		itemCnt = pCell->GetInteger();


	bool bCash = false;
	pCell = pTable->GetFieldFromLablePtr( mailID, "_IsCash" );
	if( pCell )
		bCash = pCell->GetBool();

	// ĳ����.
	if( bCash )
	{
		DNTableFileFormat * pCashTable = GetDNTable( CDnTableDB::TCASHCOMMODITY );
		DNTableCell * pCashCell = NULL;
		if( pCashTable )
		{
			pCashCell = pCashTable->GetFieldFromLablePtr( itemID, "_ItemID01" );
			if( pCashCell )
				itemID = pCashCell->GetInteger();
		}
	}

	TItemInfo itemInfo;
	if( CDnItem::MakeItemInfo( itemID, itemCnt, itemInfo ) )
		pItem = GetItemTask().CreateItem( itemInfo );

	return pItem;
}


// ���Ϻ���.
void CDnStampEventDlg::ChangeWeekDay( int wDay )
{
	if( wDay < 1 || wDay > AWEEK )
		return;

	m_StampData.weekDay = wDay;

	for( int i=0; i<AWEEK; ++i )
		m_pStaticWeekDay[i]->SetTextColor( textcolor::WHITE );	
	m_pStaticWeekDay[wDay-1]->SetTextColor( D3DCOLOR_ARGB( 255, 70, 55, 45 ) );


	// ListItem.
	int size = m_pListBoxEx->GetSize();
	for( int i=0; i<size; ++i )
	{
		CDnStampEventListItem * pItem = m_pListBoxEx->GetItem<CDnStampEventListItem>( i );
		if( pItem )
			pItem->ChangeWeekDay( wDay );
	}


	// �Ϸ絿�ȿ�������.
	if( IsShow() && CGameOption::GetInstance().m_nWeekDay == wDay )
	{
		//this->Show( false );
	
		CEtUIDialog * pEventDlg = GetInterface().GetMainMenuDialog()->GetMainDialog( CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG );
		if( pEventDlg && pEventDlg->IsShow() )
			pEventDlg->Show( false );

	}
	
	// �˸��� �׸����� - �����̹ٲ�� üũ����.
	CEtUICheckBox * pCheck = GetControl<CEtUICheckBox>("ID_CHECKBOX_SUMSHOW");			
	if( pCheck )
		pCheck->SetChecked( false );

}


// ������������.
void CDnStampEventDlg::EndStamp()
{
	for( int i=0; i<AWEEK; ++i )
		m_pStaticWeekDay[i]->SetTextColor( textcolor::WHITE );	

	m_pListBoxEx->RemoveAllItems();

//	m_StampData.Init();
}

// �ش����� �������� �ϷῩ��.
// - 0(�Ϸ��ѰԾ���),  1(�Ѱ��̻�Ϸ�),  2(��οϷ�)
int CDnStampEventDlg::IsComplete( int wDay )
{	
	wDay = wDay == 0 ? m_StampData.weekDay : wDay;

	// ������ 1���ͽ����̹Ƿ� -1.
	if( wDay > 0 )
		--wDay;

	int completeSize = 0;
	int size = m_pListBoxEx->GetSize();
	for( int i=0; i<size; ++i )
	{
		if( m_StampData.bFlag[ i * Common::MaxWeekDayCount + wDay ] )
			++completeSize;
	}

	int res = 0;
	if( completeSize == size )
		res = 2;
	else if( completeSize > 0 )
		res = 1;

	return res;
}


// �������� ���翩��.
bool CDnStampEventDlg::IsExistStamp()
{
	return ( m_pListBoxEx->GetSize() > 0 );
}


int CDnStampEventDlg::GetToday()
{
	return m_StampData.weekDay;
}

#endif // PRE_ADD_STAMPSYSTEM