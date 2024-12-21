#include "StdAfx.h"

#ifdef PRE_ADD_SERVER_WAREHOUSE

#include "DnAccountStorageBaseDlg.h"
#include "DnItemTask.h"
#include "DnStoreConfirmExDlg.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DnCommonUtil.h"
#include "DnItemSlotButton.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnWorldServerStorageBaseDlg::CDnWorldServerStorageBaseDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
, m_pSplitConfirmExDlg(NULL)
{
	m_pCountStatic = NULL;
	m_pNoticeStatic = NULL;
	m_pNoticeOnNoChargeStatic = NULL;
}

CDnWorldServerStorageBaseDlg::~CDnWorldServerStorageBaseDlg(void)
{
}

void CDnWorldServerStorageBaseDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "StorageServerDlg.ui" ).c_str(), bShow );
}

void CDnWorldServerStorageBaseDlg::InitialUpdate()
{
	m_pCountStatic = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
	m_pCountStatic->Show(false);
	m_pNoticeStatic = GetControl<CEtUIStatic>("ID_TEXT_NOTICE");
	m_pNoticeStatic->Show(false);
	m_pChargeItemSlot = GetControl<CDnItemSlotButton>("ID_BT_ITEM");
	m_pChargeItemSlot->Show(false);
	m_pChargeItemSlotStatic = GetControl<CEtUIStatic>("ID_STATIC_SLOT");
	m_pChargeItemSlotStatic->Show(false);
	m_pNoticeOnNoChargeStatic = GetControl<CEtUIStatic>("ID_TEXT_NORMAL");
	m_pNoticeOnNoChargeStatic->Show(false);
}

void CDnWorldServerStorageBaseDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	CEtUIDialog::Show(bShow);
}

void CDnWorldServerStorageBaseDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{

}

void CDnWorldServerStorageBaseDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);
}

void CDnWorldServerStorageBaseDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{

}

void CDnWorldServerStorageBaseDlg::SetItem(MIInventoryItem* pItem)
{

}

void CDnWorldServerStorageBaseDlg::ResetSlot(MIInventoryItem* pItem)
{

}

void CDnWorldServerStorageBaseDlg::SetUseItemCnt(DWORD dwItemCnt)
{

}

bool CDnWorldServerStorageBaseDlg::IsEmptySlot() const
{
	return true;
}

int CDnWorldServerStorageBaseDlg::GetEmptySlot() const
{
	return 0;
}

void CDnWorldServerStorageBaseDlg::MoveItemToStorage(const CDnQuickSlotButton* pFromSlot, bool bItemSplit)
{

}

void CDnWorldServerStorageBaseDlg::RequestMoveItem(eItemMoveType moveType, const CDnItem* pItem, int itemCount)
{

}

#endif // PRE_ADD_ACCOUNT_STORAGE