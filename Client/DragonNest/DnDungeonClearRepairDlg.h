#pragma once
#include "EtUIDialog.h"

class CDnDungeonClearRepairDlg : public CEtUIDialog, public CEtUICallback
{
	enum
	{
		MESSAGEBOX_STAGECLEAR_GO_TOWN,
		MESSAGEBOX_STAGECLEAR_GO_STAGE,
	};

private:
	CEtUIStatic* m_pGold[2];
	CEtUIStatic* m_pSilver[2];
	CEtUIStatic* m_pBronze[2];

public:
	CDnDungeonClearRepairDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnDungeonClearRepairDlg(void);

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show(bool bShow);
	void Calculate();
	int CalculateRepair(bool bRepairAll = false);
	void SetPrice(int nPrice, bool bRepairAll = false);

protected:
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
