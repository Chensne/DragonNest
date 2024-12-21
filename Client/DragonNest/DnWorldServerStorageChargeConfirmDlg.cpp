#include "StdAfx.h"
#include "DnWorldServerStorageChargeConfirmDlg.h"
#include "DnItem.h"
#include "DnInterfaceString.h"
#include "DnItemTask.h"
#include "DnCommonTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#define MAX_WSTORAGE_FEE_CHAR_COUNT 16

CDnWorldServerStorageChargeConfirmDlg::CDnWorldServerStorageChargeConfirmDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
, m_pItemSlotButton(NULL)
, m_pItemName(NULL)
, m_pOKButton(NULL)
, m_pCancelButton(NULL)
{
	m_bRenderCountMinusOne = false;
}

CDnWorldServerStorageChargeConfirmDlg::~CDnWorldServerStorageChargeConfirmDlg(void)
{
}

void CDnWorldServerStorageChargeConfirmDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "StorageCashMessageBoxDlg.ui" ).c_str(), bShow);
}

void CDnWorldServerStorageChargeConfirmDlg::InitialUpdate()
{
	m_pItemName = GetControl<CEtUIStatic>("ID_TEXT_NAME"); 

	m_pOKButton = GetControl<CEtUIButton>("ID_OK");
	m_pCancelButton = GetControl<CEtUIButton>("ID_CANCEL");

	CEtUIStatic* pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_ASK2");
	pTempStatic->Show(false);

	m_SmartMove.SetControl(m_pCancelButton);
}

void CDnWorldServerStorageChargeConfirmDlg::InitCustomControl(CEtUIControl *pControl)
{
	m_pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);
}

void CDnWorldServerStorageChargeConfirmDlg::Show(bool bShow)
{ 
	if (bShow == m_bShow)
		return;

	if (bShow)
	{
		m_SmartMove.MoveCursor();
	}
	else
	{
		Reset();
	}

	CDnCustomDlg::Show(bShow);
}

CDnItem* CDnWorldServerStorageChargeConfirmDlg::GetItem()
{
	return static_cast<CDnItem*>(m_pItemSlotButton->GetItem());
}

bool CDnWorldServerStorageChargeConfirmDlg::SetItem(CDnItem* pItem, int slotCount)
{
	if (!pItem)
		return false;

	m_pItemSlotButton->SetItem(pItem, slotCount);
	m_pItemName->SetTextColor(DN_INTERFACE::STRING::ITEM::RANK_2_COLOR(pItem->GetItemRank()));
	m_pItemName->SetText(pItem->GetName());

	return true;
}

void CDnWorldServerStorageChargeConfirmDlg::SetItemWithSlotButton(CDnSlotButton* pFromSlot, bool bRenderCountMinusOne)
{
	if (pFromSlot == NULL)
		return;

	m_bRenderCountMinusOne = bRenderCountMinusOne;

	MIInventoryItem* pInvenItem = pFromSlot->GetItem();
	if (pInvenItem == NULL || pInvenItem->GetType() != MIInventoryItem::Item)
	{
		_ASSERT("WSS");
		return;
	}

	CDnItem* pItem = static_cast<CDnItem*>(pInvenItem);
	int slotCount = pFromSlot->GetRenderCount();
	if (slotCount > 0)
		slotCount = bRenderCountMinusOne ? (slotCount - 1) : slotCount;
	SetItem(pItem, slotCount);

	m_pFromSlotBtnCache = pFromSlot;
}

void CDnWorldServerStorageChargeConfirmDlg::EnableButtons(bool bEnable)
{
	if (m_pOKButton == NULL || m_pCancelButton == NULL)
		return;
	m_pOKButton->Enable(bEnable);
	m_pCancelButton->Enable(bEnable);
}

void CDnWorldServerStorageChargeConfirmDlg::Reset()
{
	m_SmartMove.ReturnCursor();

	m_pItemSlotButton->ResetSlot();
	m_pFromSlotBtnCache = NULL;
}

int CDnWorldServerStorageChargeConfirmDlg::GetSlotItemCount() const
{
	if (m_pItemSlotButton)
		return m_pItemSlotButton->GetRenderCount();

	return -1;
}

int CDnWorldServerStorageChargeConfirmDlg::GetRealItemCount() const
{
	int slotItemCount = GetSlotItemCount();
	if (IsRenderCountMinusOne())
		return slotItemCount + 1;

	return slotItemCount;
}

bool CDnWorldServerStorageChargeConfirmDlg::IsFromCashInventory() const
{
	if (m_pFromSlotBtnCache == NULL)
	{
		_ASSERT("WSS");
		return false;
	}

	ITEM_SLOT_TYPE slotType = m_pFromSlotBtnCache->GetSlotType();
	return (slotType == ST_INVENTORY_CASH);
}

bool CDnWorldServerStorageChargeConfirmDlg::IsFromCashStorage() const
{
	if (m_pFromSlotBtnCache == NULL)
	{
		_ASSERT("WSS");
		return false;
	}

	ITEM_SLOT_TYPE slotType = m_pFromSlotBtnCache->GetSlotType();
	return (slotType == ST_STORAGE_WORLDSERVER_CASH);
}

void CDnWorldServerStorageChargeConfirmDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BT_CLOSE"))
			Show(false);
	}
	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

//////////////////////////////////////////////////////////////////////////

CDnWorldServerStorageChargeConfirmExDlg::CDnWorldServerStorageChargeConfirmExDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnWorldServerStorageChargeConfirmDlg(dialogType, pParentDialog, nID, pCallback)
, m_pItemUp(NULL)
, m_pItemDown(NULL)
, m_pButtonMin(NULL)
, m_pButtonMax(NULL)
, m_pEditBoxItemCount(NULL)
{
}

void CDnWorldServerStorageChargeConfirmExDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "StorageCashMessageBoxEXDlg.ui" ).c_str(), bShow);
}

void CDnWorldServerStorageChargeConfirmExDlg::InitialUpdate()
{
	CDnWorldServerStorageChargeConfirmDlg::InitialUpdate();

	m_pEditBoxItemCount = GetControl<CEtUIEditBox>("ID_EDITBOX_COUNT");
	m_pItemUp = GetControl<CEtUIButton>("ID_ITEM_UP");
	m_pItemDown = GetControl<CEtUIButton>("ID_ITEM_DOWN");

	m_pButtonMin = GetControl<CEtUIButton>("ID_BT_MINI");
	m_pButtonMax = GetControl<CEtUIButton>("ID_BT_MAX");

	CEtUIStatic* pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_ASK2");
	pTempStatic->Show(false);
}

void CDnWorldServerStorageChargeConfirmExDlg::UpdateAmount()
{
	CDnItem *pItem = static_cast<CDnItem*>(m_pItemSlotButton->GetItem());
	if (!pItem)
		return;

	int nCount = m_pEditBoxItemCount->GetTextToInt();
	int nOverCount(0);
	int nAmount(0);

	bool bPresentCost = true;
	nOverCount = pItem->GetOverlapCount();
	nAmount = pItem->GetItemAmount();

	if (nCount > nOverCount)
	{
		m_pEditBoxItemCount->SetIntToText(nOverCount);
	}
	else if (nCount >= 0 && nCount < 1)
	{
		m_pEditBoxItemCount->SetIntToText(1);
	}
}

bool CDnWorldServerStorageChargeConfirmExDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (!IsShow())
		return false;

	if (uMsg == WM_MOUSEWHEEL)
	{
		POINT MousePoint;
		MousePoint.x = short(LOWORD(lParam));
		MousePoint.y = short(HIWORD(lParam));

		float fMouseX, fMouseY;
		PointToFloat(MousePoint, fMouseX, fMouseY);

		if (IsMouseInDlg())
		{
			UINT uLines;
			SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uLines, 0);
			int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
			if (nScrollAmount > 0)
			{
				ProcessCommand(EVENT_BUTTON_CLICKED, false, m_pItemUp, 0);
				return true;
			}
			else if (nScrollAmount < 0)
			{
				ProcessCommand(EVENT_BUTTON_CLICKED, false, m_pItemDown, 0);
				return true;
			}
		}
	}

	// 에디트박스(수량입력)에 포커스가 가있더라도 위아래키가 적용되게 하려면, UI툴의 핫키로는 안된다.
	// 그래서 직접 MsgProc에서 처리하겠다.
	if( uMsg == WM_KEYDOWN )
	{
		if (wParam == VK_UP)
		{
			ProcessCommand(EVENT_BUTTON_CLICKED, false, m_pItemUp, 0);
			return true;
		}
		else if (wParam == VK_DOWN)
		{
			ProcessCommand(EVENT_BUTTON_CLICKED, false, m_pItemDown, 0);
			return true;
		}
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnWorldServerStorageChargeConfirmExDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_EDITBOX_CHANGE)
	{
		if (IsCmdControl("ID_EDITBOX_COUNT"))
		{
			UpdateAmount();
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
			return;
		}
	}

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_ITEM_UP"))
		{
			m_pEditBoxItemCount->SetIntToText(m_pEditBoxItemCount->GetTextToInt() + 1);
			UpdateAmount();
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
			return;
		}

		if (IsCmdControl("ID_ITEM_DOWN"))
		{
			int nCount = m_pEditBoxItemCount->GetTextToInt() - 1;
			nCount = max(nCount, 1);
			m_pEditBoxItemCount->SetIntToText(nCount);
			UpdateAmount();
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
			return;
		}

		if (IsCmdControl("ID_BT_MINI"))
		{
			m_pEditBoxItemCount->SetIntToText(1);
			UpdateAmount();
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
			return;
		}

		if (IsCmdControl("ID_BT_MAX"))
		{
			m_pEditBoxItemCount->SetIntToText(99999);
			UpdateAmount();
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
			return;
		}
	}

	// 0개 넣고 엔터치면 포커스가 에디트박스에 있는 상태로 0개 구입 못한다는 메세지박스가 뜨는데,
	// 이때 포커스가 에디트박스에 남아있기때문에, 메세지박스 다이얼로그의 핫키들이 안먹게 된다.
	// 그래서 우선 이렇게 해서 포커스를 잃도록 하겠다.
	if( nCommand == EVENT_EDITBOX_STRING )
	{
		RequestFocus( GetControl<CEtUIButton>("ID_OK") );
	}

	CDnWorldServerStorageChargeConfirmDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnWorldServerStorageChargeConfirmExDlg::SetItem(CDnItem* pItem, int slotCount)
{
	if (!pItem)
		return false;

	m_pItemSlotButton->SetItem(pItem, slotCount);
	m_pItemName->SetTextColor(DN_INTERFACE::STRING::ITEM::RANK_2_COLOR(pItem->GetItemRank()));
	m_pItemName->SetText(pItem->GetName());

	m_pEditBoxItemCount->SetIntToText(1);
	return true;
}

int CDnWorldServerStorageChargeConfirmExDlg::GetSlotItemCount() const
{
	return m_pEditBoxItemCount->GetTextToInt();
}

void CDnWorldServerStorageChargeConfirmExDlg::EnableButtons(bool bEnable)
{
	if (m_pButtonMin == NULL || m_pButtonMax == NULL)
		return;
	m_pOKButton->Enable(bEnable);
	m_pCancelButton->Enable(bEnable);
	m_pButtonMin->Enable(bEnable);
	m_pButtonMax->Enable(bEnable);
}

void CDnWorldServerStorageChargeConfirmExDlg::Reset()
{
	CDnWorldServerStorageChargeConfirmDlg::Reset();

	m_pEditBoxItemCount->ClearText();
}

void CDnWorldServerStorageChargeConfirmExDlg::Show(bool bShow)
{ 
	if (bShow == m_bShow)
		return;

	if (bShow)
	{
		m_SmartMove.MoveCursor();
	}
	else
	{
		Reset();
	}

	CDnCustomDlg::Show(bShow);
}

void CDnWorldServerStorageChargeConfirmExDlg::SetItemCountForce(int count)
{
	m_pEditBoxItemCount->SetIntToText(count);
}
