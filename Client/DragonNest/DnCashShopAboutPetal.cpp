#include "stdafx.h"
#include "DnCashShopAboutPetal.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCashShopAboutPetal::CDnCashShopAboutPetal(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
			: CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pCloseBtn = NULL;
}

void CDnCashShopAboutPetal::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "CSAboutPetal.ui" ).c_str(), bShow);
}

void CDnCashShopAboutPetal::InitialUpdate()
{
	m_pCloseBtn = GetControl<CEtUIButton>("ID_BUTTON_CLOSE");
	m_SmartMove.SetControl( m_pCloseBtn );
}

void CDnCashShopAboutPetal::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_CLOSE"))
			Show(false);
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopAboutPetal::Show( bool bShow ) 
{
	if (bShow == m_bShow)
		return;

	if (bShow)
	{
		m_SmartMove.MoveCursor();
	}
	else
	{
		m_SmartMove.ReturnCursor();
	}

	CEtUIDialog::Show(bShow);
}

//////////////////////////////////////////////////////////////////////////
#ifdef PRE_ADD_NEW_MONEY_SEED

CDnCashShopAboutSeed::CDnCashShopAboutSeed(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pCloseBtn = NULL;
}

void CDnCashShopAboutSeed::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "CSAboutSeed.ui" ).c_str(), bShow);
}

void CDnCashShopAboutSeed::InitialUpdate()
{
	m_pCloseBtn = GetControl<CEtUIButton>("ID_BUTTON_CLOSE");
	m_SmartMove.SetControl( m_pCloseBtn );
}

void CDnCashShopAboutSeed::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_CLOSE"))
			Show(false);
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopAboutSeed::Show( bool bShow ) 
{
	if (bShow == m_bShow)
		return;

	if (bShow)
	{
		m_SmartMove.MoveCursor();
	}
	else
	{
		m_SmartMove.ReturnCursor();
	}

	CEtUIDialog::Show(bShow);
}

#endif // PRE_ADD_NEW_MONEY_SEED