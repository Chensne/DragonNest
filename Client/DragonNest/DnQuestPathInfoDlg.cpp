#include "StdAfx.h"
#include "DnQuestPathInfoDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnQuestPathInfoDlg::CDnQuestPathInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnQuestPathInfoDlg::~CDnQuestPathInfoDlg(void)
{
}

void CDnQuestPathInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "QuestPathInfoDlg.ui" ).c_str(), bShow );
}

void CDnQuestPathInfoDlg::InitialUpdate()
{
}