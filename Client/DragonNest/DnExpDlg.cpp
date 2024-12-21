#include "StdAfx.h"
#include "DnExpDlg.h"
#include "DnActor.h"
#include "DnTableDB.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnExpDlg::CDnExpDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pTextBoxExp(NULL)
{
}

CDnExpDlg::~CDnExpDlg(void)
{
}

void CDnExpDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SimpleTooltipDlg.ui" ).c_str(), bShow );
}

void CDnExpDlg::InitialUpdate()
{
	m_pTextBoxExp = GetControl<CEtUITextBox>("ID_TEXTBOX");
}

void CDnExpDlg::SetExp( int nCurExp, int nMaxExp )
{
	int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
	if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetLevel() == nLimitLevel ) {
		CDnLocalPlayerActor *pActor = (CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
		if( pActor->GetAccountLevel() < AccountLevel_Master ) {
			m_pTextBoxExp->ClearText();
			m_pTextBoxExp->AddText( L"Max", textcolor::FONT_ORANGE );
			return;
		}
	}

	wchar_t szTemp[256]={0};
	swprintf_s( szTemp, 256, L"%d/%d", nCurExp, nMaxExp );
	m_pTextBoxExp->ClearText();
	m_pTextBoxExp->AddText( szTemp, textcolor::FONT_ORANGE );
}