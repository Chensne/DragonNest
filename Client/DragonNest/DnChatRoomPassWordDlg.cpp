#include "StdAfx.h"
#include "DnChatRoomPassWordDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnChatRoomPassWordDlg::CDnChatRoomPassWordDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnPassWordInputDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_bAutoCursor = true;
}

CDnChatRoomPassWordDlg::~CDnChatRoomPassWordDlg(void)
{
}