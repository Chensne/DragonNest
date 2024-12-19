#pragma once
#include "EtUIDialog.h"
#include "DnCashShopDefine.h"

class CDnInvenSlotDlg;
class CDnCashShopCartDlg : public CDnCustomDlg
{
public:
	struct SCartDlgUnit
	{
		int			id;
		CDnItem*	pItem;

		SCartDlgUnit()
		{
			id = -1;
			pItem = NULL;
		}
	};

	CDnCashShopCartDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopCartDlg(void);

	virtual void	Initialize(bool bShow);
	virtual void	Show(bool bShow);
	virtual void	InitCustomControl( CEtUIControl *pControl );

	void			UpdateItem(eRetCartAction action, const SCartActionParam& param);
	void			MakeItemList();

	void			RefreshItems();
	void			AddItem(CASHITEM_SN sn, int cartItemId);
	void			RemoveItem(int cartItemId);

protected:
	virtual void	Process(float fElapsedTime);
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void	InitialUpdate();

private:
	void			Update();
	void			PrevPage();
	void			NextPage();

	void			Clear();
	void			EmptySlots();
	void			SetCurrentPageToMaxPage();

	std::vector<CDnQuickSlotButton*>	m_pSlotBtns;

	CEtUIButton*		m_pBuyBtn;
	CEtUIButton*		m_pPresentBtn;

	CEtUIButton*		m_pPrevBtn;
	CEtUIButton*		m_pNextBtn;
	CEtUIStatic*		m_pPageNum;
	CEtUIButton*		m_pEmptyCart;

	int					m_MaxPage;
	int					m_CurrentPage;

	std::list<SCartDlgUnit> m_pItems;	// sync to cartlist
};