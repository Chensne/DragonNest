#include "StdAfx.h"
#include "DnDungeonSelectDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDungeonSelectDlg::CDnDungeonSelectDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnDungeonSelectDlg::~CDnDungeonSelectDlg(void)
{
}

void CDnDungeonSelectDlg::Initialize( bool bShow )
{
	//CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DungeonClearDlg.ui" ).c_str(), bShow );
}

void CDnDungeonSelectDlg::InitialUpdate()
{
}