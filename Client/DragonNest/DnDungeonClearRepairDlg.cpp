#include "StdAfx.h"
#include "DnDungeonClearRepairDlg.h"
#include "GameSendPacket.h"
#include "DnInterface.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "DnItemTask.h"
#include "TradeSendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnDungeonClearRepairDlg::CDnDungeonClearRepairDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
{
	for (int i = 0; i < 2; i++)
	{
		m_pGold[i] = NULL;
		m_pSilver[i] = NULL;
		m_pBronze[i] = NULL;
	}
}

CDnDungeonClearRepairDlg::~CDnDungeonClearRepairDlg(void)
{

}

void CDnDungeonClearRepairDlg::InitialUpdate()
{
	for (int i = 0; i < 2; i++)
	{
		m_pGold[i] = GetControl<CEtUIStatic>(FormatA("ID_GOLD_%d", i).c_str());
		m_pSilver[i] = GetControl<CEtUIStatic>(FormatA("ID_SILVER_%d", i).c_str());
		m_pBronze[i] = GetControl<CEtUIStatic>(FormatA("ID_BRONZE_%d", i).c_str());
	}
}

void CDnDungeonClearRepairDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("DungeonRepairConfirmDlg.ui").c_str(), bShow);
}

void CDnDungeonClearRepairDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	CEtUIDialog::Show(bShow);
}


bool CDnDungeonClearRepairDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!this->IsShow())
		return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);;


	if (uMsg == WM_KEYDOWN)
	{
		if (GetKeyState(VK_F1) & 0x80)
		{
			if(this->CalculateRepair() != 0)
				SendRepairEquip();
			//GetInterface().AddChatMessage(eChatType::CHATTYPE_WORLD, L"Server", L"Press F1");
			return true;
		}
		if (GetKeyState(VK_F2) & 0x80)
		{
			if (this->CalculateRepair(true) != 0)
				SendRepairAll();
			//GetInterface().AddChatMessage(eChatType::CHATTYPE_WORLD, L"Server", L"Press F2");
			return true;
		}
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnDungeonClearRepairDlg::Calculate()
{
	this->CalculateRepair(false); //repair all false
	this->CalculateRepair(true); //repair all true
}

int CDnDungeonClearRepairDlg::CalculateRepair(bool bRepairAll)
{
	int nPrice = 0;

	if (!bRepairAll)
	{
		nPrice = GetItemTask().CalcRepairEquipPrice();
	} else {
		nPrice = GetItemTask().CalcRepairEquipPrice() + GetItemTask().CalcRepairCharInvenPrice();
	}

	this->SetPrice(nPrice, bRepairAll);

	return nPrice;
}

void CDnDungeonClearRepairDlg::SetPrice(int nPrice,bool bRepairAll)
{
	int nGold = nPrice / 10000;
	int nSilver = (nPrice % 10000) / 100;
	int nBronze = nPrice % 100;

	if (!bRepairAll) {
		m_pGold[0]->SetText(FormatW(L"%d", nGold).c_str());
		m_pSilver[0]->SetText(FormatW(L"%d", nSilver).c_str());
		m_pBronze[0]->SetText(FormatW(L"%d", nBronze).c_str());
	} else {
		m_pGold[1]->SetText(FormatW(L"%d", nGold).c_str());
		m_pSilver[1]->SetText(FormatW(L"%d", nSilver).c_str());
		m_pBronze[1]->SetText(FormatW(L"%d", nBronze).c_str());
	}
}