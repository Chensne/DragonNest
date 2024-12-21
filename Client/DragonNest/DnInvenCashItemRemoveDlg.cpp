#include "Stdafx.h"

#include "DnItemTask.h"

#include "DnInterface.h"
#include "DnInvenCashItemRemoveDlg.h"

#ifdef PRE_ADD_CASHREMOVE




CDnInvenCashItemRemoveDlg::CDnInvenCashItemRemoveDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_pSlotButton(NULL)
, m_DlgType( CDnInvenCashItemRemoveDlg::DLGTYPE_REMOVE )
{

}


//CDnInvenCashItemRemoveDlg::


void CDnInvenCashItemRemoveDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("InvenCashDeleteDlg.ui").c_str(), bShow );
}


void CDnInvenCashItemRemoveDlg::InitialUpdate()
{
	m_pSlotButton = GetControl<CDnItemSlotButton>( "ID_ITEM" );		
}


void CDnInvenCashItemRemoveDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		CEtUIStatic * pStatic = NULL;

		// 제거창.
		if( m_DlgType == DLGTYPE::DLGTYPE_REMOVE )
		{
			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_TITLE" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4881 ) ); // "캐시 아이템 삭제"

			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_ASK0" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4882 ) ); // "선택한 캐시아이템을 삭제하시겠습니까?"

			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_ASK1" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4883 ) ); // "동의 시 해당 캐시아이템은 ~ 복구할 수 있습니다."			
		}

		// 복구창.
		else if( m_DlgType == DLGTYPE::DLGTYPE_RESTORE )
		{
			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_TITLE" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4888 ) ); // "캐시 아이템 보구"

			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_ASK0" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4889 ) ); // "선택한 캐시아이템을 복구하시겠습니까?"

			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_ASK1" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4890 ) ); // "동의 시 해당 캐시아이템은 ~ 기준으로 복구됩니다."			
		}
	}
	else
	{
		// 제거창.
		if( m_DlgType == DLGTYPE::DLGTYPE_REMOVE )
			GetInterface().RemoveCashItemCancel();
		//// 복구창.
		//else if( m_DlgType == DLGTYPE::DLGTYPE_RESTORE )
		//	GetInterface().RestoreCashItemCancel;
	}

}


void CDnInvenCashItemRemoveDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			// 제거창.
			if( m_DlgType == DLGTYPE::DLGTYPE_REMOVE )
				GetInterface().RemoveCashItemAccept(); // 제거수락.

			// 복구창.
			else if( m_DlgType == DLGTYPE::DLGTYPE_RESTORE )
				RestoreCashItemAccept(); // 복구수락.
			
			Show( false );			
		}

		else if( IsCmdControl( "ID_CANCEL" ) || IsCmdControl( "ID_CLOSE" ) )
		{
			Show( false );			
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnInvenCashItemRemoveDlg::SetItemSlot( MIInventoryItem * pItem )
{
	// #65680 캐시아이템 삭제확인창에서 아이템에 마우스오버시 툴팁이 뜨지 않음.
	// - 캐시아이템을 삭제하기위해 드래그를 수행하는 경우, 드래그가 끝나지않은 상태에서 툴팁처리는 Hide되게 되어있으므로 강제로 Release 한다.
	if( EtInterface::drag::IsValid() )		
	{		
		drag::Command(UI_DRAG_CMD_CANCEL);
		drag::ReleaseControl();		
	}

	m_pSlotButton->SetItem( pItem, pItem->GetOverlapCount() );
}



// Dlg 종류설정.
void CDnInvenCashItemRemoveDlg::SetDlgType( DLGTYPE type )
{
	m_DlgType = type;
}


// 아이템복구 수락.
void CDnInvenCashItemRemoveDlg::RestoreCashItemAccept()
{
	CDnItem * pItem = static_cast< CDnItem * >( m_pSlotButton->GetItem() );
	if( pItem )
	{			
		GetItemTask().RequestRemoveCashItem( pItem->GetSerialID(), true );
	}	
}

#endif