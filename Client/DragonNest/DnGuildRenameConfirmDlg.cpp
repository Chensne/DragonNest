#include "StdAfx.h"
#include "DnGuildRenameConfirmDlg.h"
#include "DnCommonUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildRenameConfirmDlg::CDnGuildRenameConfirmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallBack )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallBack )
,m_pStaticRecheck(NULL)
{

}

void CDnGuildRenameConfirmDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CSGuildConfirm.ui").c_str(), bShow);
}

void CDnGuildRenameConfirmDlg::InitialUpdate()
{	
	m_pStaticRecheck = GetControl<CEtUIStatic>("ID_TEXT_RECHECK");
}

void CDnGuildRenameConfirmDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;	

	CEtUIDialog::Show(bShow);
}

void CDnGuildRenameConfirmDlg::SetGuildName(const WCHAR* wszGuildName)
{
	if( wszGuildName )
		m_pStaticRecheck->SetText( FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3969 ), wszGuildName)); // UISTRING : %s로 길드 이름을 변경하시겠습니까?
}

