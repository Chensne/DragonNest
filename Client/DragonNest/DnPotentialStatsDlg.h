#pragma once
#include "DnCustomDlg.h"

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL

class CDnPotentialStatsDlg : public CEtUIDialog, public CEtUICallback
{
protected:
	CEtUIListBoxEx* m_pListBox;
	CEtUIStatic*	m_pStaticTitle;
	enum ePotentialType
	{
		ePOTENTIAL_RANGE = 0,
		ePOTENTIAL_VALUE,

		ePOTENTIAL_MAX
	};
public:
	CDnPotentialStatsDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPotentialStatsDlg();

protected:
	void ShowStateDialog();
	void UpdateNewState(CDnState* pCloneMyState, CDnState* pNewItemState, CDnState* pOriginalMyState);
	void SetCloneState(CDnState* pCloneMyState, CDnState* pNewItemState, bool bIsTakeAway = false);
	void AddListBoxText(int nEnumStringID, int nCombineStats, int nPlayerStats, ePotentialType eType);
	void AddListBoxText(int nEnumStringID, float fCombineStats, float fPlayerStats, ePotentialType eType);
	void GetNewState(bool bIsTakeAway, int nCloneState, int nItemState, int& nResultState);
	void GetNewState(bool bIsTakeAway, int nCloneState, float fItemState, int& nResultState);
	void CalcStateRatio(bool bIsTakeAway, int nCloneState, float fItemStateRatio, int& nResultState);
	void CalcStateRatio(bool bIsTakeAway, float fCloneState, float fItemStateRatio, float& fResultState);

	void ChangePositiveNumber(int &nNumber)		{ nNumber *= -1; }
	void ChangePositiveNumber(float &fNumber)	{ fNumber *= -1.0f; }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif //PRE_MOD_POTENTIAL_JEWEL_RENEWAL