#pragma once
#include "EtUITabDialog.h"

class MIInventoryItem;

class CDnStoreListDlg : public CEtUITabDialog
{
public:
	enum eStoreListDlgType
	{
		eNormal,
		eRepurchase,
	};

	CDnStoreListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnStoreListDlg(void);

protected:
	std::vector<CEtUIRadioButton*> m_vecTabButton;
	eStoreListDlgType m_StoreListDlgType;

public:
	void SetItem( MIInventoryItem *pItem );
	void ResetAllSlot();

	bool IsEmptyListDialog();
	void SetRepurchaseDlg(bool bSet) { m_StoreListDlgType = bSet ? eRepurchase : eNormal; }
	bool IsRepurchaseDlg() const { return (m_StoreListDlgType == eRepurchase); }
	void ResetSlot(BYTE cSlotIndex);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};
