#include "stdafx.h"

#ifdef PRE_ADD_DRAGON_GEM
#include "DnDragonGemEquipDlg.h"
#include "DnItemTask.h"

CDnDragonGemEquipDlg::CDnDragonGemEquipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_pDescription = NULL;
	m_pMessage = NULL;

	m_pItem = NULL;
	m_pItemSlot = NULL;
	m_pButtonQuickSlot = NULL;

}

CDnDragonGemEquipDlg::~CDnDragonGemEquipDlg()
{
}

void CDnDragonGemEquipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("InvenItemJewelDlg.ui").c_str(), bShow);
}

void CDnDragonGemEquipDlg::InitialUpdate()
{

		m_pMessage = GetControl<CEtUIStatic>("ID_TEXT_ASK");
		m_pDescription = GetControl<CEtUITextBox>("ID_TEXTBOX_OPTION");
	
}

void CDnDragonGemEquipDlg::InitCustomControl(CEtUIControl *pControl)
{
	if (pControl == NULL)
		return;

	if (!strstr(pControl->GetControlName(), "ID_ITEM"))
		return;

	CDnItemSlotButton* pSlotBtn = NULL;
	pSlotBtn = static_cast<CDnItemSlotButton*>(pControl);
	pSlotBtn->SetSlotType(ST_INVENTORY);
	pSlotBtn->SetSlotIndex(0);

	if(pSlotBtn)
		m_pItemSlot = pSlotBtn;
}

void CDnDragonGemEquipDlg::Show(bool bShow)
{
	if (bShow == m_bShow)
		return;

	CEtUIDialog::Show(bShow);
}

bool CDnDragonGemEquipDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsShow())
		return false;

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnDragonGemEquipDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process(fElapsedTime);
}

void CDnDragonGemEquipDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName(pControl->GetControlName());
	if (nCommand == EVENT_BUTTON_CLICKED) {
		if (strcmp(pControl->GetControlName(), "ID_OK") == 0)
		{
			this->Show(false);
		}
		else if (strcmp(pControl->GetControlName(), "ID_CANCEL") == 0 || strcmp(pControl->GetControlName(), "ID_CLOSE") == 0)
		{
			this->Show(false);
		}
		else if (IsCmdControl("ID_ITEM"))
		{
			CDnSlotButton *pDragButton = (CDnSlotButton *)drag::GetControl();
			CDnSlotButton *pPressedButton = (CDnItemSlotButton *)pControl;

			// 우클릭으로 빼기
			if (uMsg == WM_RBUTTONUP)
			{
				SAFE_DELETE(m_pItem);
				pPressedButton->ResetSlot();
				if (m_pButtonQuickSlot)
				{
					m_pButtonQuickSlot->SetRegist(false);
					m_pButtonQuickSlot = NULL;
				}
				//CheckEnchantJewelItem();
				return;
			}

			if (pDragButton)
			{
				drag::ReleaseControl();

				if (pDragButton->GetItemType() != MIInventoryItem::Item)
					return;

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if (pItem == NULL )//|| CanApplyEnchantJewel(pDragButton) == false)
				{
					pDragButton->DisableSplitMode(true);
					return;
				}

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if (pPressedItem)
				{
					SAFE_DELETE(m_pItem);
					pPressedButton->ResetSlot();
					m_pButtonQuickSlot->SetRegist(false);
					m_pButtonQuickSlot = NULL;
				}

				TItemInfo itemInfo;
				pItem->GetTItemInfo(itemInfo);
				m_pItem = GetItemTask().CreateItem(itemInfo);
				pPressedButton->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

				//CheckEnchantJewelItem();
				CEtSoundEngine::GetInstance().PlaySound("2D", pItem->GetDragSoundIndex());

				m_pButtonQuickSlot = (CDnQuickSlotButton *)pDragButton;
				m_pButtonQuickSlot->SetRegist(true);
			}
			return;
		}
	}
	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}


void CDnDragonGemEquipDlg::OnRecvDragonGemEquip(int nResult, int nSlotIndex)
{
	if (nResult == ERROR_NONE)
	{
		CDnItem *pItem = GetItemTask().GetCharInventory().GetItem(nSlotIndex);
		if (!pItem || !m_pItem || pItem->GetClassID() != m_pItem->GetClassID())
			return;

		/*if (m_nSoundIdx >= 0)
			CEtSoundEngine::GetInstance().PlaySound("2D", m_nSoundIdx, false);*/
	}

	SAFE_DELETE(m_pItem);
	m_pItemSlot->ResetSlot();

	if (m_pButtonQuickSlot)
	{
		m_pButtonQuickSlot->SetRegist(false);
		m_pButtonQuickSlot = NULL;
	}

	// 창을 닫아버리는게 더 편한 듯
	Show(false);
}

#endif // PRE_ADD_DRAGON_GEM
