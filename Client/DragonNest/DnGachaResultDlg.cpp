#include "StdAfx.h"
#include "DnGachaResultDlg.h"
#include "DnItem.h"
#include "DnInterfaceString.h"
#ifdef PRE_MOD_GACHA_SYSTEM
#include "DnCommonUtil.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_GACHA_JAPAN

using namespace DN_INTERFACE;
using namespace STRING;

CDnGachaResultDlg::CDnGachaResultDlg( UI_DIALOG_TYPE dialogType /* = UI_TYPE_FOCUS */, 
										  CEtUIDialog *pParentDialog /* = NULL */, 
										  int nID /* = -1 */, CEtUICallback *pCallback /* = NULL */ ) : CDnCustomDlg( dialogType, pParentDialog, nID, pCallback ),
										  m_pResultItemSlot( NULL ),
										  m_pResultItemTextBox( NULL )
{
	
}

CDnGachaResultDlg::~CDnGachaResultDlg(void)
{

}


void CDnGachaResultDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Jp_Result.ui" ).c_str(), bShow );
}


void CDnGachaResultDlg::InitialUpdate( void )
{
	//m_pStaticResult = GetControl<CEtUIStatic>( "ID_STATIC0" );
	m_pResultItemTextBox = GetControl<CEtUITextBox>( "ID_TEXTBOX_ITEM" );
	
	m_SmartMove.SetControl( GetControl<CEtUIButton>( "ID_OK" ) );
}


void CDnGachaResultDlg::InitCustomControl( CEtUIControl *pControl )
{
	m_pResultItemSlot = static_cast<CDnItemSlotButton*>( pControl );
}

void CDnGachaResultDlg::SetItem( CDnItem *pItem )
{
	m_pResultItemSlot->SetItem( pItem, 1 );
	m_pResultItemTextBox->ClearText();
	m_pResultItemTextBox->SetText( pItem->GetName(), ITEM::RANK_2_COLOR( pItem->GetItemRank() ) );
#ifdef PRE_MOD_GACHA_SYSTEM
	std::wstring addStatString;
	CommonUtil::GetItemStatString_ExceptBasic(addStatString, pItem);
	m_pResultItemTextBox->AddText(addStatString.c_str());
#endif
}

void CDnGachaResultDlg::Show( bool bShow )
{
	CEtUIDialog::Show( bShow );
	if( bShow )
		m_SmartMove.MoveCursor();
	else
		m_SmartMove.ReturnCursor();
}



#endif //PRE_ADD_GACHA_JAPAN