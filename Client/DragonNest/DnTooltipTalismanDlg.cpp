#include "Stdafx.h"
#include "DnTooltipTalismanDlg.h"
#include "DnTableDB.h"

#if defined(PRE_ADD_TALISMAN_SYSTEM)
CDnTooltipTalismanDlg::CDnTooltipTalismanDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
, m_pTextBox(NULL)
{
}

CDnTooltipTalismanDlg::~CDnTooltipTalismanDlg()
{
}

void CDnTooltipTalismanDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "TooltipTalismanDlg.ui" ).c_str(), bShow );
}

void CDnTooltipTalismanDlg::InitialUpdate()
{
	m_pTextBox = GetControl<CEtUITextBox>( "ID_TEXTBOX" );
}

void CDnTooltipTalismanDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	CEtUIDialog::Show(bShow);
}

void CDnTooltipTalismanDlg::SetTalismanSlot( int nSlotIndex, bool bIsCash/*=false*/ )
{
	wchar_t szString[256];
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TTALISMANSLOT );

	int nID = pSox->GetItemID( nSlotIndex-1 );
	int nDescription = pSox->GetFieldFromLablePtr( nID, "_Description" )->GetInteger();
	if( nDescription <= 0)
		return;

	int		nLevel		= pSox->GetFieldFromLablePtr( nID, "_Level")->GetInteger();
	float	fEfficiency = pSox->GetFieldFromLablePtr( nID, "_Efficiency")->GetFloat();
	int		nAmount		= pSox->GetFieldFromLablePtr( nID, "_Amount")->GetInteger();
	int		nNeedItemID = pSox->GetFieldFromLablePtr( nID, "_item")->GetInteger();

	m_pTextBox->ClearText();

	if(bIsCash)
	{
		m_pTextBox->AddText(L"");
		// ex) 100% 효율을 갖는 탈리스만 슬롯.	
		wsprintf( szString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nDescription) );
		m_pTextBox->AddText( szString, textcolor::ORANGE );
		return;
	}

	
	
	// ex) 100% 효율을 갖는 탈리스만 슬롯.	
	wsprintf( szString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nDescription) );
	m_pTextBox->AddText( szString, textcolor::ORANGE );

	// 레벨
	wsprintf( szString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8347) );
	m_pTextBox->AddText( szString, textcolor::TOMATO);
	wsprintf( szString, L"%d", nLevel );
	m_pTextBox->AppendText(szString, textcolor::WHITE);

	// 골드
	int nGold	= int(nAmount/10000);
	int nSilver = int((nAmount%10000)/100);
	int nCopper = int(nAmount%100);
	wsprintf(szString, GetEtUIXML().GetUIString(CEtUIXML::idCategory1,8348));
	m_pTextBox->AddText( szString, textcolor::TOMATO);
	wsprintf(szString, GetEtUIXML().GetUIString(CEtUIXML::idCategory1,8370), nGold, nSilver, nCopper);
	m_pTextBox->AppendText( szString, textcolor::WHITE);

	// 아이템
	if(nNeedItemID > 0)
	{
		wsprintf( szString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8349));
		m_pTextBox->AddText( szString, textcolor::TOMATO);
		wsprintf( szString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nNeedItemID) );
		m_pTextBox->AppendText(szString, textcolor::WHITE);
	}
	else
	{
		wsprintf( szString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8349) );
		m_pTextBox->AddText( szString, textcolor::TOMATO);
		wsprintf( szString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 93) );
		m_pTextBox->AppendText(szString, textcolor::WHITE);
	}	
}

#endif // PRE_ADD_TALISMAN_SYSTEM