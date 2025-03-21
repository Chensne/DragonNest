#include "StdAfx.h"

#ifdef PRE_MOD_CHATBG

#include "DnChatBoardDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChatBoardDlg::CDnChatBoardDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
					: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
					, m_pBackBoard(NULL)
{
}

void CDnChatBoardDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ChatBoardDlg.ui" ).c_str(), bShow );
}

void CDnChatBoardDlg::InitialUpdate()
{
	m_pBackBoard = GetControl<CEtUIStatic>("ID_STATIC0");
}

void CDnChatBoardDlg::UpdateDlgCoord(float fX, float fY, float fWidth, float fHeight)
{
	m_pBackBoard->UpdateUICoord(fX, fY,  fWidth, fHeight);

	CEtUIDialog::UpdateDlgCoord(fX, fY, fWidth, fHeight);
}

#endif // PRE_MOD_CHATBG