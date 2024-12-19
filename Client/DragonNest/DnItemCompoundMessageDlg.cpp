#include "Stdafx.h"
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
#include "DnItemCompoundMessageDlg.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnItemCompoundMessageBox::CDnItemCompoundMessageBox( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
	m_pStaticTitle = NULL;
	m_pItemSlotButton = NULL;
	m_ButtonOK = NULL;
	m_ButtonCancel = NULL;
	m_pItem = NULL;
	//m_pProgressDlg = NULL;
}

CDnItemCompoundMessageBox::~CDnItemCompoundMessageBox()
{
	Reset();
	
	//SAFE_DELETE( m_pProgressDlg );// 아이템 세이프 딜리트 
}

void CDnItemCompoundMessageBox::InitialUpdate()
{
	m_pStaticTitle = GetControl<CEtUIStatic>("ID_TEXT_TITLE");
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEM");
	m_ButtonOK = GetControl<CEtUIButton>("ID_BT_OK");
	m_ButtonCancel = GetControl<CEtUIButton>("ID_BT_CANCEL");
	m_pItemSlotButton->SetSlotType( ST_ITEM_COMPOUND );
	
	m_SmartMove.SetControl( m_ButtonOK );
	m_pStaticTitle->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2106) );

	//m_pProgressDlg = new CDnItemCompoundProgressDlg( UI_TYPE_MODAL, this );
	//m_pProgressDlg->Initialize( false );
}
void CDnItemCompoundMessageBox::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemCompoundMessageDlg.ui" ).c_str(), bShow );
}
void CDnItemCompoundMessageBox::Show( bool bShow )	
{
	if( m_bShow == bShow )
		return;
	
	if( bShow )
	{
		m_SmartMove.MoveCursor();
	}
	else
	{
		m_SmartMove.ReturnCursor();
		Reset();
	}
	CEtUIDialog::Show( bShow );
}
void CDnItemCompoundMessageBox::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{	
		if (IsCmdControl("ID_BT_OK"))
		{
			// 프로그레스바 시작  !
			//float fTime = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::AllGiveCharmItemOpenTime);
			//m_pProgressDlg->ShowEx( true , (float)fTime * 0.001, m_pItem );
		
			//CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
			//GetItemTask().GetCompoundInfo( m_ItemPackage.nCompoundID, &CompoundInfo );
			//GetItemTask().RequestItemCompoundOpen( m_ItemPackage.nCompoundID, m_ItemPackage.vNeedItemInfo, m_pItem->GetSerialID(), m_ItemPackage.nCost );
			Show( false );
		}
		else if(IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE") )
		{
			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnItemCompoundMessageBox::Reset()
{
	SAFE_DELETE( m_pItem );
	m_pItemSlotButton->ResetSlot();
}

void CDnItemCompoundMessageBox::SetItemSlot( CDnItem* pItem )
{
	if( ! pItem )
		return;

	SAFE_DELETE( m_pItem );
	m_pItemSlotButton->ResetSlot();
	
	TItemInfo itemInfo;
	pItem->GetTItemInfo( itemInfo );

	m_pItem = GetItemTask().CreateItem( itemInfo );

	if( m_pItemSlotButton && m_pItem )
	{
		m_pItemSlotButton->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	}
}
#endif 








