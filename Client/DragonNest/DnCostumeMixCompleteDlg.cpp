#include "StdAfx.h"
#include "DnCostumeMixCompleteDlg.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#ifdef _TEST_CODE_KAL
#include "DnCostumeRandomMixDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCostumeMixCompleteDlg::CDnCostumeMixCompleteDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pResultSlotBtn = NULL;
	m_pResultStatic = NULL;
	m_pOkBtn		= NULL;
	m_pCloseBtn		= NULL;
	m_Type			= eCOSMIX;
}

CDnCostumeMixCompleteDlg::~CDnCostumeMixCompleteDlg(void)
{
	MIInventoryItem* pSlotItem = m_pResultSlotBtn->GetItem();
	if (pSlotItem)
		SAFE_DELETE(pSlotItem);
}

void CDnCostumeMixCompleteDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Com_Result.ui" ).c_str(), bShow );
}

void CDnCostumeMixCompleteDlg::InitialUpdate()
{
	m_pResultSlotBtn	= GetControl<CDnItemSlotButton>("ID_BUTTON0");
	m_pResultStatic		= GetControl<CEtUIStatic>("ID_STATIC0");
	m_pOkBtn			= GetControl<CEtUIButton>("ID_OK");
	m_pCloseBtn			= GetControl<CEtUIButton>("ID_BUTTON_CLOSE");

	m_SmartMove.SetControl(m_pOkBtn);
}

void CDnCostumeMixCompleteDlg::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if (bShow)
		m_SmartMove.MoveCursor();
	else
		m_SmartMove.ReturnCursor();

	CDnCustomDlg::Show( bShow );
}

void CDnCostumeMixCompleteDlg::SetInfo(ITEMCLSID resultItemID, char cOption)
{
	if (resultItemID == ITEMCLSID_NONE)
		return;

	m_pResultStatic->ClearText();

	TItem itemInfo;
	CDnItem::MakeItemInfo(resultItemID, 1, itemInfo);
	itemInfo.cOption = cOption;

	CDnItem* pItem = CDnItemTask::GetInstance().CreateItem(itemInfo);
	if (pItem && m_pResultSlotBtn)
	{
		MIInventoryItem* pSlotItem = m_pResultSlotBtn->GetItem();
		if (pSlotItem)
			SAFE_DELETE(pSlotItem);
		m_pResultSlotBtn->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

		std::wstring str;
		str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130306), pItem->GetItemName(resultItemID).c_str());
		m_pResultStatic->SetText(str.c_str());
	}
}

void CDnCostumeMixCompleteDlg::SetInfo(CDnItem* pCompleteItem)
{
	if (pCompleteItem == NULL)
		return;

	m_pResultStatic->ClearText();

	TItemInfo itemInfo;
	pCompleteItem->GetTItemInfo(itemInfo);

	CDnItem* pItem = CDnItemTask::GetInstance().CreateItem(itemInfo.Item);
	if (pItem && m_pResultSlotBtn)
	{
		MIInventoryItem* pSlotItem = m_pResultSlotBtn->GetItem();
		if (pSlotItem)
			SAFE_DELETE(pSlotItem);
		m_pResultSlotBtn->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

		std::wstring str;
		str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130306), pItem->GetName());
		m_pResultStatic->SetText(str.c_str());
	}
}

void CDnCostumeMixCompleteDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_OK") || IsCmdControl("ID_BUTTON_CLOSE"))
		{
			if (CDnItemTask::IsActive())
			{
				if (m_Type == eCOSMIX)
				{
					if (CDnItemTask::GetInstance().RequestItemCostumeMixCloseComplete())
						Show(false);
				}
				else if (m_Type == eDESIGNMIX)
				{
					if (CDnItemTask::GetInstance().RequestItemCostumeDesignMixCloseComplete())
						Show(false);
				}
#ifdef PRE_ADD_COSRANDMIX
				else if (m_Type == eRANDOMMIX)
				{
	#ifdef PRE_MOD_COSRANDMIX_NPC_CLOSE
					const CDnCostumeRandomMixDataMgr& mgr = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr();
					if (mgr.GetCurrentOpenType() == CostumeMix::RandomMix::OpenByNpc)
					{
						GetInterface().ResetCostumeRandomMixDlg();
						Show(false);
					}
					else
					{
						if (CDnItemTask::GetInstance().RequestItemCostumeRandomMixCloseComplete())
							Show(false);
					}
	#else
					if (CDnItemTask::GetInstance().RequestItemCostumeRandomMixCloseComplete())
						Show(false);
	#endif
				}
#endif
			}
			return;
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}