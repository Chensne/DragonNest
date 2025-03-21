#include "StdAfx.h"
#include "DnItemDisjointInfoDlg.h"
#include "DnItemDisjointResultDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnItemDisjointResultDlg::CDnItemDisjointResultDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
	, m_pResultListBox(NULL)
{
}

CDnItemDisjointResultDlg::~CDnItemDisjointResultDlg(void)
{
}

void CDnItemDisjointResultDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("ItemDisjointResultDlg.ui").c_str(), bShow);
}

void CDnItemDisjointResultDlg::InitialUpdate()
{
	//ID_LISTBOXEX_LIST
	//ID_BT_OK
	//ID_CLOSE_DIALOG
	m_pResultListBox = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");
}
void CDnItemDisjointResultDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (strcmp(pControl->GetControlName(), "ID_BT_OK") == 0)
		{
			this->Show(false);
		}

		if (strcmp(pControl->GetControlName(), "ID_CLOSE_DIALOG") == 0)
		{
			this->Show(false);
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}


void CDnItemDisjointResultDlg::OnRecvDropList(SCItemDisjointResNew *pPacket)
{
	this->Clear();

	for(int i=0;i<20;i++)
	{
		if(pPacket->nItemID[i] == 0 || pPacket->nItemID[i] < 0)
			continue;

		CDnItemDisjointInfoDlg* pItemInfoDlg = m_pResultListBox->AddItem<CDnItemDisjointInfoDlg>();
		if (pItemInfoDlg)
		{ 
			pItemInfoDlg->SetInfo(pPacket->nItemID[i],pPacket->nCount[i]);
		}
	}
}

void CDnItemDisjointResultDlg::Clear()
{
	m_pResultListBox->RemoveAllItems();
}