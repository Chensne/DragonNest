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

		// ����â.
		if( m_DlgType == DLGTYPE::DLGTYPE_REMOVE )
		{
			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_TITLE" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4881 ) ); // "ĳ�� ������ ����"

			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_ASK0" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4882 ) ); // "������ ĳ�þ������� �����Ͻðڽ��ϱ�?"

			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_ASK1" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4883 ) ); // "���� �� �ش� ĳ�þ������� ~ ������ �� �ֽ��ϴ�."			
		}

		// ����â.
		else if( m_DlgType == DLGTYPE::DLGTYPE_RESTORE )
		{
			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_TITLE" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4888 ) ); // "ĳ�� ������ ����"

			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_ASK0" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4889 ) ); // "������ ĳ�þ������� �����Ͻðڽ��ϱ�?"

			pStatic = GetControl<CEtUIStatic>( "ID_TEXT_ASK1" );
			pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4890 ) ); // "���� �� �ش� ĳ�þ������� ~ �������� �����˴ϴ�."			
		}
	}
	else
	{
		// ����â.
		if( m_DlgType == DLGTYPE::DLGTYPE_REMOVE )
			GetInterface().RemoveCashItemCancel();
		//// ����â.
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
			// ����â.
			if( m_DlgType == DLGTYPE::DLGTYPE_REMOVE )
				GetInterface().RemoveCashItemAccept(); // ���ż���.

			// ����â.
			else if( m_DlgType == DLGTYPE::DLGTYPE_RESTORE )
				RestoreCashItemAccept(); // ��������.
			
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
	// #65680 ĳ�þ����� ����Ȯ��â���� �����ۿ� ���콺������ ������ ���� ����.
	// - ĳ�þ������� �����ϱ����� �巡�׸� �����ϴ� ���, �巡�װ� ���������� ���¿��� ����ó���� Hide�ǰ� �Ǿ������Ƿ� ������ Release �Ѵ�.
	if( EtInterface::drag::IsValid() )		
	{		
		drag::Command(UI_DRAG_CMD_CANCEL);
		drag::ReleaseControl();		
	}

	m_pSlotButton->SetItem( pItem, pItem->GetOverlapCount() );
}



// Dlg ��������.
void CDnInvenCashItemRemoveDlg::SetDlgType( DLGTYPE type )
{
	m_DlgType = type;
}


// �����ۺ��� ����.
void CDnInvenCashItemRemoveDlg::RestoreCashItemAccept()
{
	CDnItem * pItem = static_cast< CDnItem * >( m_pSlotButton->GetItem() );
	if( pItem )
	{			
		GetItemTask().RequestRemoveCashItem( pItem->GetSerialID(), true );
	}	
}

#endif