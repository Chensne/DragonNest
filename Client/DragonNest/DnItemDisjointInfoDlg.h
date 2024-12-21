#pragma once
#include "DnCustomDlg.h"

class CDnItem;
class CDnItemDisjointInfoDlg : public CDnCustomDlg
{
public:
	CDnItemDisjointInfoDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnItemDisjointInfoDlg(void);

	virtual void	Initialize(bool bShow);
	void			SetInfo(int nItemID);
	void			SetInfo(int nItemID,int nCount);

protected:
	virtual void	InitialUpdate();
	virtual void	InitCustomControl(CEtUIControl *pControl);
	void			ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);


private:
	CEtUIStatic*		m_pBase;
	CDnItemSlotButton*	m_pSlotBtn;
	CEtUIStatic*		m_pNameStatic;
	CEtUIStatic*		m_pItemSlotBG;

	CEtUIStatic*		m_pCountStatic;


	CDnItem*			m_pItem;
	int					m_Index;
}; 
