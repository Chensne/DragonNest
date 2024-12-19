#include "Stdafx.h"

#ifdef PRE_ADD_GAMEQUIT_REWARD

#include "TaskManager.h"
#include "DnItemTask.h"

#include "DnInterface.h"
#include "DnCharSelectDlg.h"
#include "DnCharSelectListDlg.h"

#include "DnGameQuitNextTimeRewardComboDlg.h"

CDnGameQuitNextTimeRewardComboDlg::CDnGameQuitNextTimeRewardComboDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
,m_pComboCharacter( NULL )
{

}

CDnGameQuitNextTimeRewardComboDlg::~CDnGameQuitNextTimeRewardComboDlg(){

	ReleaseDlg();
	m_vecItemSlot.clear();
	m_vecJob.clear();
}


void CDnGameQuitNextTimeRewardComboDlg::ReleaseDlg()
{
	int size = (int)m_vecItemSlot.size();
	for( int i=0; i<size; ++i )
		m_vecItemSlot[i]->ResetSlot();

	size = (int)m_vecItem.size();
	for( int k=0; k<size; ++k )
		delete m_vecItem[k];
	m_vecItem.clear();


}

void CDnGameQuitNextTimeRewardComboDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("NewbieGiftBoxComboDlg.ui").c_str(), bShow );
}

void CDnGameQuitNextTimeRewardComboDlg::InitialUpdate()
{
	m_pComboCharacter = GetControl<CEtUIComboBox>("ID_COMBOBOX_NAME");	

	m_vecItemSlot.reserve( 21 );

	char str[64] = {0,};
	for( int i=0; i<21; ++i )
	{
		sprintf_s( str, 64, "ID_BT_ITEM%d", i );
		CDnItemSlotButton * pControl = GetControl<CDnItemSlotButton>( str );
		if( pControl )
			m_vecItemSlot.push_back( pControl );
	}

	
	// ID_COMBOBOX_NAME
}

void CDnGameQuitNextTimeRewardComboDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		// 콤보박스에 캐릭터리스트 설정.
		BuildComboBox();
	}
	else
	{

	}
	

	CEtUIDialog::Show( bShow );
}

void CDnGameQuitNextTimeRewardComboDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );


	// Button.
	if( nCommand == EVENT_BUTTON_CLICKED )
	{	
		// 닫기.
		if( IsCmdControl("ID_BT_CLOSE") )
			Show( false );

		// 확인.
		if( IsCmdControl("ID_BT_OK" ) )
		{
			GetInterface().MessageBox( 7956	, MB_OK, 7956, this ); // "선택한 캐릭터가 선물을 받습니다. 접속 하시겠습니까?\n(신규캐릭터는 튜토리얼을 마쳐야 보상을 받을 수 있습니다.)"

			Show( false );
		}
	}

	// ComboBox.
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		if( IsCmdControl("ID_COMBOBOX_NAME") )
		{
			SComboBoxItem* pItem = m_pComboCharacter->GetSelectedItem();
			if( pItem )
			{
				int charIdx = m_pComboCharacter->GetSelectedIndex();

				// 캐릭터선택변경.
				CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
				if( !pLoginTask ) return;
				pLoginTask->SetCharSelect( charIdx );

				// 아이템슬롯설정.
				int nLevel = 0;
				if( m_pComboCharacter->GetItemValue( charIdx, nLevel ) && !m_vecJob.empty() )
					SetNextTimeRewardComboItem( nLevel, m_vecJob[ charIdx ] );
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnGameQuitNextTimeRewardComboDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 7956:
			{
				if( IsCmdControl("ID_OK") )		
				{
					CTask * pTask = CTaskManager::GetInstance().GetTask( "BridgeTask" );
					if( pTask )
						static_cast< CDnBridgeTask * >( pTask )->SetNewbieRewardMessage();

					GetInterface().OpenSecurityCheckDlg( CDnInterface::InterfaceTypeEnum::Login , 0 );
				}
			}
			break;
		}		
	}
}

// 보상대상에 따른 보상아이템 설정.
void CDnGameQuitNextTimeRewardComboDlg::SetNextTimeRewardComboItem( int nLevel, int nClass )
{		
	ReleaseDlg();

	// Table로부터 Item 찾기.	
	CDnItemTask::GetInstance().FindEventItems( 7, nLevel, nClass, m_vecItem );	

	// ItemSlot설정.
	CDnItem * pItem = NULL;
	int size = (int)m_vecItem.size();
	int slotSize = (int)m_vecItemSlot.size();
	if( slotSize >= size )
	{
		for( int i=0; i<size; ++i )
		{
			pItem = m_vecItem[i];
			if( pItem )
				m_vecItemSlot[ i ]->SetItem( pItem, pItem->GetOverlapCount() );
		}
	}

}

// 콤보박스에 캐릭터리스트 설정.
void CDnGameQuitNextTimeRewardComboDlg::BuildComboBox()
{	
	// 콤보박스설정 - 캐릭터명.
	if( m_pComboCharacter && m_pComboCharacter->GetItemCount() < 1 )
	{
		CDnCharSelectDlg * pDlg = GetInterface().GetCharSelectDlg();
		if( pDlg )
		{
			CDnCharSelectListDlg * pDlgList = pDlg->GetCharSelectListDlg();
			if( pDlgList )
			{
				wchar_t buf[256] = {0,};
				const wchar_t * pStr = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4957 ); // "레벨 %d %s"

				std::vector<TCharListData> & vecCharList = pDlgList->GetCharListData();
				int size = (int)vecCharList.size();

				m_vecJob.clear();
				m_vecJob.resize( size );

				for( int i=0; i<size; ++i )
				{
					swprintf_s( buf, 256, pStr, vecCharList[ i ].cLevel, vecCharList[ i ].wszCharacterName );					
					m_pComboCharacter->AddItem( buf, NULL, vecCharList[ i ].cLevel );					

					// 기본 직업군.
					int job = vecCharList[ i ].cJob;
					int temp = 0;
					while( ( temp = CommonUtil::GetParentJob( job ) ) > 0 )
						job = temp;
					m_vecJob[ i ] = job;
				}

			}
		}	
	}

	
}


// 캐릭터선택창에서 선택한 캐릭터Index.
void CDnGameQuitNextTimeRewardComboDlg::SelectedGameQuitRewardCharIndex( int charIndex )
{
	if( !IsShow() )
		return;

	if( charIndex < 0 || charIndex >= (int)m_pComboCharacter->GetItemCount() )
		return;

	m_pComboCharacter->SetSelectedByIndex( charIndex );
}

void CDnGameQuitNextTimeRewardComboDlg::ResetComboBox()
{
	if( m_pComboCharacter && m_pComboCharacter->GetItemCount() > 0 )
		m_pComboCharacter->RemoveAllItems();
}

#endif // PRE_ADD_GAMEQUIT_REWARD