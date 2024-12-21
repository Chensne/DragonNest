#include "StdAfx.h"

#include "DnCostumeMixDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnCostumeMixPreviewDlg.h"
#include "DnItemTask.h"
#include "DnCostumeMixDataMgr.h"
#include "DnQuickSlotButton.h"
#include "DnItemSlotButton.h"
#include "DnCommonUtil.h"
#include "DnInterfaceString.h"
#include "ItemSendPacket.h"
#include "DnCostumeMixProgressDlg.h"
#include "DnCostumeMixCompleteDlg.h"
#include "DnPartyTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnCostumeMixDlg::CDnCostumeMixDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
		: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pMixBtn			= NULL;
	m_pCancelBtn		= NULL;
	m_pRandomResult		= NULL;
	m_pPrevResultBtn	= NULL;
	m_pNextResultBtn	= NULL;
	m_pPageStatic		= NULL;
	m_pAbilityComboBox	= NULL;
	m_pPreviewDlg		= NULL;
	m_pProgressDlg		= NULL;
	m_pCompleteDlg		= NULL;
	m_pCloseBtn			= NULL;
	m_CurPageNum		= 1;
	m_CurResultPartsType = CDnParts::AllParts_Amount;
	m_CompleteMixSound	= -1;
	m_bSelectTooltipOn	= false;
}

CDnCostumeMixDlg::~CDnCostumeMixDlg(void)
{
	SAFE_DELETE(m_pPreviewDlg);
	SAFE_DELETE(m_pProgressDlg);
	SAFE_DELETE(m_pCompleteDlg);

	ClearResultSlot();

	if (m_CompleteMixSound != -1)
		CEtSoundEngine::GetInstance().RemoveSound(m_CompleteMixSound);
}

void CDnCostumeMixDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("Com_Base.ui").c_str(), bShow);

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10044 );
	if( strlen( szFileName ) > 0 )
		m_CompleteMixSound = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
}

void CDnCostumeMixDlg::InitialUpdate()
{
	m_pInvenSlotCacheList.resize(MAXCOSMIXSTUFF);

	int i = 0;
	for (; i < MAX_COSTUMEMIX_STUFF_COUNT; ++i)
	{
		std::string btnStr = FormatA("ID_ITEME_GACHA%d", i);
		CDnItemSlotButton* pButton = GetControl<CDnItemSlotButton>(btnStr.c_str());
		if (pButton && i < MAXCOSMIXSTUFF)
		{
			pButton->SetSlotIndex(i);
			pButton->SetSlotType(ST_ITEM_COSTUMEMIX_STUFF);
			m_pStuffSlotList.push_back(pButton);
		}

		if (i >= MAXCOSMIXSTUFF)
		{
			btnStr = FormatA("ID_STATIC_COVER%d", i);
			CEtUIStatic* pStatic = GetControl<CEtUIStatic>(btnStr.c_str());
			if (pStatic)
				pStatic->Show(true);
		}
	}

	for (i = 0; i < MAX_COSTUMEMIX_RESULT_COUNT; ++i)
	{
		std::string btnStr = FormatA("ID_SLOT_SELECT%d", i);
		CEtUIStatic* pStatic = GetControl<CEtUIStatic>(btnStr.c_str());
		if (pStatic)
		{
			pStatic->Show(false);
			m_pResultSlotSelectList.insert(std::make_pair(i, pStatic));
		}
	}

	for (i = 0; i < MAX_COSTUMEMIX_RESULT_COUNT_WITHOUT_RANDOM; ++i)
	{
		std::string btnStr = FormatA("ID_SLOT_GACHA%d", i);
		CDnItemSlotButton* pButton = GetControl<CDnItemSlotButton>(btnStr.c_str());
		if (pButton)
		{
			pButton->SetSlotIndex(i);
			pButton->SetShowTooltip(false);
			pButton->SetSlotType(ST_ITEM_COSTUMEMIX_RESULT);
			m_pResultSlotList.insert(std::make_pair(i, pButton));
		}
	}

	m_pPrevResultBtn	= GetControl<CEtUIButton>("ID_BT_PRIOR");
	m_pNextResultBtn	= GetControl<CEtUIButton>("ID_BT_NEXT");
	m_pPageStatic		= GetControl<CEtUIStatic>("ID_BT_PAGE");

	m_pMixBtn			= GetControl<CEtUIButton>("ID_BUTTON_BUYGACHA");
	m_pCancelBtn		= GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
	m_pCloseBtn			= GetControl<CEtUIButton>("ID_BUTTON_CLOSE");

	m_pRandomResult		= GetControl<CEtUIButton>("ID_SLOT_RANDOM");

	m_pAbilityComboBox	= GetControl<CEtUIComboBox>("ID_COMBOBOX_PARAMETER");
	if (m_pAbilityComboBox)
		SetAbilityComboBox(CDnParts::AllParts_Amount);

	m_pPreviewDlg		= new CDnCostumeMixPreviewDlg(UI_TYPE_CHILD, this);
	m_pPreviewDlg->Initialize(false);

	m_pProgressDlg		= new CDnCostumeMixProgressDlg(UI_TYPE_MODAL);
	m_pProgressDlg->Initialize(false);

	m_pCompleteDlg		= new CDnCostumeMixCompleteDlg(UI_TYPE_MODAL);
	m_pCompleteDlg->Initialize(false);
	m_pCompleteDlg->SetType(CDnCostumeMixCompleteDlg::eCOSMIX);
}

void CDnCostumeMixDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
	if (dataMgr.IsEnableCostumeMix() == false && bShow)
		return;

	CDnLocalPlayerActor::LockInput(bShow);

	if (bShow)
	{
		if (CDnItemTask::IsActive())
		{
			if (CDnActor::s_hLocalActor)
			{
				CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
				dataMgr.SetCurrentClassId(CDnActor::s_hLocalActor->GetClassID());
			}
		}
		else
		{
			_ASSERT(0);
			return;
		}

		ClearStuffSlot();
		m_pAbilityComboBox->SetSelectedByValue(NO_ABILITY_COMBOBOX_SELECTED);
		m_CurPageNum = 1;
	}
	else
	{
		std::vector<CDnItemSlotButton*>::iterator iter = m_pStuffSlotList.begin();
		for (; iter != m_pStuffSlotList.end(); ++iter)
		{
			CDnItemSlotButton* pBtn = *iter;
			if (pBtn)
				ReleaseStuffItemSlotBtn(pBtn);
		}

		ShowResultItemToolTipOff();

		if (m_pProgressDlg && m_pProgressDlg->IsShow())
			m_pProgressDlg->Show(false);

		GetInterface().CloseMessageBox();
	}

	m_pPreviewDlg->Show(bShow);

	drag::Command(UI_DRAG_CMD_CANCEL);
	drag::ReleaseControl();

	CDnCustomDlg::Show( bShow );
}

void CDnCostumeMixDlg::ReleaseInvenSlotCache(const int slotIdx)
{
	std::vector<SSlotCacheUnit>::iterator iter = m_pInvenSlotCacheList.begin();
	for(; iter != m_pInvenSlotCacheList.end(); ++iter)
	{
		SSlotCacheUnit& unit = *iter;
		if (unit.stuffSlotIdx == slotIdx)
		{
			if (unit.pInvenSlotBtn)
			{
				unit.pInvenSlotBtn->SetRegist(false);
				unit.pInvenSlotBtn->DisableSplitMode(true);
				unit.pInvenSlotBtn = NULL;
			}

			m_pInvenSlotCacheList.erase(iter);
			return;
		}
	}
}

bool CDnCostumeMixDlg::ReleaseStuffItemSlotBtn(CDnSlotButton* pStuffSlot)
{
	if (pStuffSlot == NULL)
		return false;

	const int slotIdx = pStuffSlot->GetSlotIndex();
	ReleaseInvenSlotCache(slotIdx);

	pStuffSlot->ResetSlot();

	if (GetStuffCountInSlot() <= 0)
		OnEmptyStuffSlots();

	return true;
}

bool CDnCostumeMixDlg::IsMixableItem(const CDnParts* pItem) const
{
	CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
	return dataMgr.IsEnableCostumeMixStuff(pItem);
}

CDnCostumeMixDlg::SSlotCacheUnit* CDnCostumeMixDlg::GetInvenSlotCache(int invenIndex)
{
	std::vector<SSlotCacheUnit>::iterator iter = m_pInvenSlotCacheList.begin();
	for(; iter != m_pInvenSlotCacheList.end(); ++iter)
	{
		SSlotCacheUnit& unit = (*iter);
		if (unit.pInvenSlotBtn && unit.pInvenSlotBtn->GetSlotIndex() == invenIndex)
			return &unit;
	}

	return NULL;
}

CDnCostumeMixDlg::SSlotCacheUnit* CDnCostumeMixDlg::GetInvenSlotCacheBySlotIndex(int slotIndex)
{
	std::vector<SSlotCacheUnit>::iterator iter = m_pInvenSlotCacheList.begin();
	for(; iter != m_pInvenSlotCacheList.end(); ++iter)
	{
		SSlotCacheUnit& unit = (*iter);
		if (unit.pInvenSlotBtn && unit.stuffSlotIdx == slotIndex)
			return &unit;
	}

	return NULL;
}

bool CDnCostumeMixDlg::AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot, int itemCount)
{
	if (pFromSlot == NULL || pAttachSlot == NULL)
	{
		_ASSERT(0);
		return false;
	}

	pFromSlot->EnableSplitMode(itemCount);
	MIInventoryItem* pInvenItem = pFromSlot->GetItem();
	if (pInvenItem == NULL || pInvenItem->GetType() != MIInventoryItem::Item)
	{
		_ASSERT(0);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	ITEM_SLOT_TYPE fromSlotType = pFromSlot->GetSlotType();
	if (fromSlotType != ST_ITEM_COSTUMEMIX_STUFF && fromSlotType != ST_INVENTORY_CASH)
	{
		HandleError(eECM_CANT_MIX_STUFF); // 합성할 수 없는 아이템입니다.
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	CDnItem* pItem = static_cast<CDnItem *>(pFromSlot->GetItem());
	if (pItem->IsCashItem() == false)
	{
		HandleError(eECM_GENERAL);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	std::vector<int> nVecJobList;
	if (CDnActor::s_hLocalActor)
	{
		((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if (pItem->IsPermitPlayer(nVecJobList) == false)
		{
			HandleError(eECM_STUFF_DIFFCLASS);
			return false;
		}
	}

	if (pItem->IsEternityItem() == false)
	{
		HandleError(eECM_STUFF_ETERNITY_ONLY);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (pItem->GetItemType() != ITEMTYPE_PARTS)
	{
		HandleError(eECM_CANT_MIX_STUFF);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	CDnParts* pPartsItem = static_cast<CDnParts*>(pItem);
	if (IsMixableItem(pPartsItem) == false)
	{
		HandleError(eECM_CANT_MIX_STUFF);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (pItem->GetLookItemID() != ITEMCLSID_NONE)
	{
		HandleError(eECM_CANT_MIX_STUFF);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (GetStuffCountInSlot() > 1 || pAttachSlot->IsEmptySlot())
	{
		if (m_CurResultPartsType != CDnParts::AllParts_Amount && pPartsItem->GetPartsType() != m_CurResultPartsType)
		{
			HandleError(eECM_DIFF_PARTS_TYPE);
			pFromSlot->DisableSplitMode(true);
			return false;
		}
	}

	SSlotCacheUnit unit(pAttachSlot->GetSlotIndex(), pFromSlot);
	std::vector<SSlotCacheUnit>::const_iterator iter = std::find(m_pInvenSlotCacheList.begin(), m_pInvenSlotCacheList.end(), unit);
	if (iter != m_pInvenSlotCacheList.end())
	{
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (pAttachSlot->GetItem() != NULL)
		ReleaseStuffItemSlotBtn(pAttachSlot);

	if (fromSlotType == ST_ITEM_COSTUMEMIX_STUFF)
	{
		ReleaseStuffItemSlotBtn(pAttachSlot);

		SSlotCacheUnit* pUnit = GetInvenSlotCacheBySlotIndex(pFromSlot->GetSlotIndex());
		if (pUnit)
			pUnit->stuffSlotIdx = pAttachSlot->GetSlotIndex();

		pFromSlot->ResetSlot();
	}
	else
	{
		SSlotCacheUnit unit(pAttachSlot->GetSlotIndex(), pFromSlot);
		pFromSlot->SetRegist(true);
		m_pInvenSlotCacheList.push_back(unit);

		if (pItem->GetItemType() != ITEMTYPE_PARTS)
		{
			_ASSERT(0);
			pFromSlot->DisableSplitMode(true);
			return false;
		}
	}

	pAttachSlot->SetItem(pPartsItem, itemCount);
	CEtSoundEngine::GetInstance().PlaySound( "2D", pPartsItem->GetDragSoundIndex() );

	if (GetStuffCountInSlot() == 1)
	{
		m_CurResultPartsType = pPartsItem->GetPartsType();
		SetResultItemList();
		if (m_ResultSlotSelectCache.pSelectStatic != NULL)
			RevertPartsFromPreview();
	}

	return true;
}

bool CDnCostumeMixDlg::RevertPartsFromPreview()
{
	if (m_pPreviewDlg == NULL || m_pPreviewDlg->IsShow() == false)
		return true;

	if (CDnItemTask::IsActive())
	{
		CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
		int classId = dataMgr.GetCurrentClassId();
		if (CommonUtil::IsValidCharacterClassId(classId))
			m_pPreviewDlg->MakeAvatarToRender(classId);

		return true;
	}

	_ASSERT(0);
	return false;
}

void CDnCostumeMixDlg::AttachPartsToPreview(ITEMCLSID itemId)
{
	m_pPreviewDlg->AttachParts(itemId);
}

void CDnCostumeMixDlg::HandleError(eERR_COSMIX errType)
{
	std::wstring str;
	switch (errType)
	{
	case eECM_GENERAL:				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130310);	break;	// UISTRING : 코스튬 합성 중 문제가 발생했습니다.
	case eECM_NOSELECT_RESULT:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130311);	break;	// UISTRING : 합성 결과 아이템을 선택해주세요.
	case eECM_STUFFCOUNT_SHORT:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130312);	break;	// UISTRING : 합성 재료 개수가 부족합니다
	case eECM_NOSELECT_ABILITY:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4576);	break;	// UISTRING : 능력치를 선택해주세요.
	case eECM_CANT_MIX_STUFF:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130308);	break;	// UISTRING : 합성할 수 없는 아이템입니다.
	case eECM_DIFF_PARTS_TYPE:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130307);	break;	// UISTRING : 동일한 파츠끼리만 합성할 수 있습니다
	case eECM_STUFF_FULL:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130309);	break;	// UISTRING : 더 이상 합성 창에 아이템을 올릴 수 없습니다.
	case eECM_STUFF_ETERNITY_ONLY:	str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130313);	break;	// UISTRING : 기간 제한 아이템은 합성할 수 없습니다.
	case eECM_STUFF_DIFFCLASS:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130315);	break;	// UISTRING : 자신의 클래스 아이템만 합성할 수 있습니다.
	case eECM_STUFFCOUNT_NONE:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130318);	break;	// UISTRING : 합성할 코스튬을 등록해 주세요.
	default:
		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130310);	break;	// UISTRING : 코스튬 합성 중 문제가 발생했습니다.
	}

	GetInterface().MessageBox(str.c_str(), MB_OK);
}

bool CDnCostumeMixDlg::CheckMix()
{
	if (CDnItemTask::IsActive() && CDnItemTask::GetInstance().IsLockCosMixSendPacket())
	{
		//HandleError(eECM_GENERAL);
		return false;
	}

	if (GetStuffCountInSlot() <= 0)
	{
		HandleError(eECM_STUFFCOUNT_NONE);
		return false;
	}

	if (GetStuffCountInSlot() != MAXCOSMIXSTUFF)
	{
		HandleError(eECM_STUFFCOUNT_SHORT);
		return false;
	}

	if (m_ResultSlotSelectCache.IsEmpty())
	{
		HandleError(eECM_NOSELECT_RESULT);
		return false;
	}

	//	STUFF ITEM PROCESS
	int i = 0;
	for (; i < (int)m_pStuffSlotList.size(); ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlotList[i];
		if (pButton == NULL || pButton->GetItem() == NULL)
		{
			HandleError(eECM_STUFFCOUNT_SHORT);
			return false;
		}
		else
		{
			const CDnItem* pStuffItem = static_cast<const CDnItem*>(pButton->GetItem());
			if (pStuffItem == NULL)
			{
				HandleError(eECM_STUFFCOUNT_SHORT);
				return false;
			}
		}
	}

	//	OPTION PROCESS
	int selAbility = -1;
	bool bAbilRet = m_pAbilityComboBox->GetSelectedValue(selAbility);

	if (bAbilRet == false || selAbility < 0)
	{
		HandleError(eECM_NOSELECT_ABILITY);
		return false;
	}

	return true;
}

void CDnCostumeMixDlg::DoMix()
{
	if (CheckMix() == false)
		return;

	//	RESULT ITEM PROCESS
	ITEMCLSID resultItemID = -1;
	if (m_ResultSlotSelectCache.resultSlotIdx > 0)
	{
		std::map<int, CDnItemSlotButton*>::const_iterator iter = m_pResultSlotList.find(m_ResultSlotSelectCache.resultSlotIdx - 1);
		if (iter != m_pResultSlotList.end())
		{
			const CDnItemSlotButton* pBtn = (*iter).second;
			if (pBtn)
				resultItemID = pBtn->GetItemID();
		}
	}
	else if (m_ResultSlotSelectCache.resultSlotIdx <= 0)
	{
		resultItemID = ITEMCLSID_NONE;
	}

	//	STUFF ITEM PROCESS
	INT64 stuffSerials[MAXCOSMIXSTUFF];
	int i = 0;
	for (; i < (int)m_pStuffSlotList.size(); ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlotList[i];
		const CDnItem* pStuffItem = static_cast<const CDnItem*>(pButton->GetItem());
		if (pStuffItem)
			stuffSerials[i] = pStuffItem->GetSerialID();
	}

	//	OPTION PROCESS
	int selAbility = -1;
	m_pAbilityComboBox->GetSelectedValue(selAbility);

	if (CDnItemTask::IsActive())
	{
		ClearStuffSlot();

		CDnItemTask::GetInstance().RequestItemCostumeMix(stuffSerials, MAXCOSMIXSTUFF, resultItemID, (char)selAbility);
	}
}

void CDnCostumeMixDlg::Process(float fElapsedTime)
{
	if (m_bShow)
	{
		if (CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsLocalActorEnterGateReady())
		{
			if (CDnItemTask::IsActive())
				CDnItemTask::GetInstance().RequestItemCostumeMixClose();

			return;
		}
	}

	CDnCustomDlg::Process(fElapsedTime);
}

void CDnCostumeMixDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_BUYGACHA"))
		{
			if (CheckMix())
			{
				m_pProgressDlg->SetMessageBox(PROGRESS_BAR_TIME, PROGRESS_BAR_TIME, ePROGRESSDLG, this);
				m_pProgressDlg->Show(true);
			}
			return;
		}
		else if (IsCmdControl("ID_BUTTON_CANCEL") || IsCmdControl("ID_BUTTON_CLOSE"))
		{
			//GetInterface().ShowCostumeMixDlg(false, true);
			if (CDnItemTask::IsActive())
				CDnItemTask::GetInstance().RequestItemCostumeMixClose();
			return;
		}
		else if (IsCmdControl("ID_BT_PRIOR"))
		{
			--m_CurPageNum;
			SetResultItemList();
		}
		else if (IsCmdControl("ID_BT_NEXT"))
		{
			++m_CurPageNum;
			SetResultItemList();
		}
		else if (IsCmdControl("ID_SLOT_RANDOM"))
		{
			if (GetStuffCountInSlot() <= 0)
			{
				m_pRandomResult->Enable(false);
			}
			else
			{
				std::map<int, CEtUIStatic*>::iterator slotSelectIter = m_pResultSlotSelectList.find(0);
				CEtUIStatic* pSel = (*slotSelectIter).second;
				if (pSel && m_ResultSlotSelectCache.IsRandomSelected() == false)
				{
					pSel->Show(true);

					if (m_ResultSlotSelectCache.pSelectStatic)
						ShowOffResultSelectSlot();
					m_ResultSlotSelectCache.pSelectStatic = pSel;
					m_ResultSlotSelectCache.resultSlotIdx = 0;

					RevertPartsFromPreview();
				}
				m_pRandomResult->SetPressed(true);
			}
		}

		if (strstr(pControl->GetControlName(), "ID_ITEME_GACHA"))
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			CDnItemSlotButton *pPressedButton = static_cast<CDnItemSlotButton*>(pControl);

			if (uMsg == WM_RBUTTONUP)
			{
				if (ReleaseStuffItemSlotBtn(pPressedButton) == false)
				{
					HandleError(eECM_GENERAL);
					return;
				}
			}

			if (pDragButton)
			{
				if (pDragButton != pPressedButton)
				{
					if (AddAttachItem(pDragButton, pPressedButton, CDnSlotButton::ITEM_ORIGINAL_COUNT) == false)
						drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
					return;
				}
				else
				{
					drag::ReleaseControl();
					return;
				}
			}

			if (pPressedButton != NULL && uMsg == WM_LBUTTONDOWN)
			{
				CDnItem* pPressedBtnItem = static_cast<CDnItem*>(pPressedButton->GetItem());
				if (pPressedBtnItem)
				{
					pPressedButton->EnableSplitMode(pPressedBtnItem->GetOverlapCount());
					drag::SetControl(pPressedButton);

					CEtSoundEngine::GetInstance().PlaySound("2D", pPressedBtnItem->GetDragSoundIndex());
				}
			}
		}

		if (strstr(pControl->GetControlName(), "ID_SLOT_GACHA"))
		{
			CDnItemSlotButton *pPressedButton = static_cast<CDnItemSlotButton*>(pControl);

			if (pPressedButton && pPressedButton->IsEmptySlot() == false)
			{
				int idx = pPressedButton->GetSlotIndex() + 1;
				if (idx >= 0 && idx < (int)m_pResultSlotSelectList.size())
				{
					std::map<int, CEtUIStatic*>::iterator slotSelectIter = m_pResultSlotSelectList.find(idx);
					if (slotSelectIter != m_pResultSlotSelectList.end())
					{
						CEtUIStatic* pStatic = (*slotSelectIter).second;
						if (pStatic)
						{
							if (m_ResultSlotSelectCache.pSelectStatic != pStatic)
							{
								ShowOffResultSelectSlot();

								pStatic->Show(true);
								m_ResultSlotSelectCache.pSelectStatic = pStatic;
								m_ResultSlotSelectCache.resultSlotIdx = idx;
								AttachPartsToPreview(pPressedButton->GetItemID());
								if (m_pRandomResult->IsPressed())
									m_pRandomResult->SetPressed(false);
							}
							else
							{
								ShowOffResultSelectSlot();
								RevertPartsFromPreview();
							}
						}
						return;
					}
				}
			}
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCostumeMixDlg::ShowResultItemToolTipOn(CDnItemSlotButton* pBtn, float fX, float fY)
{
	if (pBtn && pBtn->GetItem() != NULL)
	{
		CDnItem* pItem = static_cast<CDnItem *>(pBtn->GetItem());
		CDnTooltipDlg* pDlg = GetInterface().GetTooltipDialog();
		if (pDlg == NULL || pItem == NULL)
			return;

		pDlg->SetMode(CDnTooltipDlg::TOOLTIP_MANUALCONTROL);
		fY = fY + ((Height() - pDlg->Height()) * 0.5f);
		pDlg->ShowTooltip(pItem, pItem->GetType(), pBtn->GetSlotType(), fX, fY, false);
		m_bSelectTooltipOn = true;
	}
}

void CDnCostumeMixDlg::ShowResultItemToolTipOff()
{
	if (m_bSelectTooltipOn)
	{
		CDnTooltipDlg* pDlg = GetInterface().GetTooltipDialog();
		if (pDlg)
			pDlg->HideTooltip();

		pDlg->SetMode(CDnTooltipDlg::TOOLTIP_NORMAL);
		m_bSelectTooltipOn = false;
	}
}

bool CDnCostumeMixDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );

			float fMouseX, fMouseY;
			PointToFloat( MousePoint, fMouseX, fMouseY );

			bool bToolTipShowing = false;
			std::map<int, CDnItemSlotButton*>::iterator resSlotIter = m_pResultSlotList.begin();
			for (; resSlotIter != m_pResultSlotList.end(); ++resSlotIter)
			{
				CDnItemSlotButton* pBtn = (*resSlotIter).second;
				if (pBtn && pBtn->GetItem() != NULL && pBtn->IsInside(fMouseX, fMouseY))
				{
					float fX = fMouseX, fY = fMouseY;
					GetPosition(fX, fY);
					ShowResultItemToolTipOn(pBtn, fX + Width(), fY);
					bToolTipShowing = true;
					break;
				}
			}

			if (bToolTipShowing == false && m_ResultSlotSelectCache.IsEmpty() == false)
			{
				if (m_ResultSlotSelectCache.pSelectStatic->IsInside(fMouseX, fMouseY))
				{
					std::map<int, CDnItemSlotButton*>::iterator iter = m_pResultSlotList.find(m_ResultSlotSelectCache.resultSlotIdx - 1);
					if (iter != m_pResultSlotList.end())
					{
						CDnItemSlotButton* pBtn = (*iter).second;
						if (pBtn)
						{
							CDnItem* pItem = static_cast<CDnItem *>(pBtn->GetItem());
							CDnTooltipDlg* pDlg = GetInterface().GetTooltipDialog();
							if (pDlg == NULL || pItem == NULL)
								return true;

							pDlg->SetMode(CDnTooltipDlg::TOOLTIP_MANUALCONTROL);
							pDlg->ShowTooltip(pItem, pItem->GetType(), pBtn->GetSlotType(), fMouseX, fMouseY, false);
							m_bSelectTooltipOn = bToolTipShowing = true;
							return true;
						}
					}
				}
			}

			if (bToolTipShowing == false)
				ShowResultItemToolTipOff();
		}
		break;
	}

	return CDnCustomDlg::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnCostumeMixDlg::AddAttachItem(CDnSlotButton *pFromButton)
{
	int index = GetEmptyStuffSlot();
	if (index < 0)
	{
		HandleError(eECM_STUFF_FULL);
		return;
	}

	if (index >= 0 && index < (int)m_pStuffSlotList.size())
	{
		CDnItemSlotButton* pEmptyBtn = m_pStuffSlotList[index];
		if (pEmptyBtn)
		{
			if (AddAttachItem(pFromButton, pEmptyBtn, CDnSlotButton::ITEM_ORIGINAL_COUNT) == false)
				drag::Command(UI_DRAG_CMD_CANCEL);
			drag::ReleaseControl();
		}
	}
}

void CDnCostumeMixDlg::SetAbilityComboBox(CDnParts::PartsTypeEnum type)
{
	m_pAbilityComboBox->RemoveAllItems();

	m_pAbilityComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130314), NULL, NO_ABILITY_COMBOBOX_SELECTED);	// UISTRING : 능력치 선택
	if (type == CDnParts::AllParts_Amount || CDnItemTask::IsActive() == false)
	{
		m_pAbilityComboBox->Enable(false);
		return;
	}

	CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
	const std::vector<CDnCostumeMixDataMgr::SAddAbility>* pAbilityList = dataMgr.GetMixAbilityList(type);
	if (pAbilityList == NULL || pAbilityList->size() <= 0)
	{
		m_pAbilityComboBox->Enable(false);
		return;
	}

	std::vector<CDnCostumeMixDataMgr::SAddAbility>::const_iterator iter = pAbilityList->begin();
	for (; iter != pAbilityList->end(); ++iter)
	{
		const CDnCostumeMixDataMgr::SAddAbility& ability = *iter;
		const WCHAR* pAbilityName = DN_INTERFACE::STRING::ITEM::GET_STATIC(CDnTooltipDlg::GetToolTipStaticByStateType(ability.abilityNum));
		if (pAbilityName && pAbilityName[0] != '\0')
		{
			std::wstring temp;
			temp = FormatW(L"%s %s%.1f%%", pAbilityName, (ability.abilityValue >= 0) ? L"+" : L"-", abs(ability.abilityValue) * 100.f);
			m_pAbilityComboBox->AddItem(temp.c_str(), NULL, ability.potenOffset);
		}
	}

	m_pAbilityComboBox->Enable(true);
}

int CDnCostumeMixDlg::GetResultSlotUISize()
{
	return int(m_pResultSlotList.size());
}

void CDnCostumeMixDlg::SetResultItemList()
{
	if (CDnItemTask::IsActive() == false)
	{
		_ASSERT(0);
		return;
	}

	CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
	int classId = dataMgr.GetCurrentClassId();
	const CDnCostumeMixDataMgr::MIX_RESULT_LIST* pItemList = dataMgr.GetMixResultItemList(classId, m_CurResultPartsType);
	if (pItemList == NULL || pItemList->size() <= 0)
	{
		HandleError(eECM_CANT_MIX_STUFF);
		ClearStuffSlot();
		return;
	}

	CDnCostumeMixDataMgr::MIX_RESULT_LIST::const_iterator iter = pItemList->begin();

	int resultSlotSize = GetResultSlotUISize();
	int itemListSize = (int)pItemList->size();

	int maxPageNum = itemListSize / resultSlotSize;
	maxPageNum += ((itemListSize % resultSlotSize) > 0) ? 1 : 0;

	m_pNextResultBtn->Enable(true);
	m_pPrevResultBtn->Enable(true);

	if (m_CurPageNum <= 1)
	{
		m_CurPageNum = 1;
		m_pPrevResultBtn->Enable(false);
	}

	if (m_CurPageNum >= maxPageNum)
	{
		m_CurPageNum = maxPageNum;
		m_pNextResultBtn->Enable(false);
	}

	int offset = resultSlotSize * (m_CurPageNum - 1);

	std::wstring str = FormatW(L"%d/%d", m_CurPageNum, maxPageNum);
	m_pPageStatic->SetText(str.c_str());

	if (m_pPrevResultBtn && m_pPrevResultBtn->IsShow() == false)
		m_pPrevResultBtn->Show(true);
	if (m_pNextResultBtn && m_pNextResultBtn->IsShow() == false)
		m_pNextResultBtn->Show(true);
	if (m_pPageStatic && m_pPageStatic->IsShow() == false)
		m_pPageStatic->Show(true);

	if (offset > 0)
		std::advance(iter, offset);
	int i = 0;
	std::map<int, CDnItemSlotButton*>::iterator resSlotIter = m_pResultSlotList.begin();
	for (; resSlotIter != m_pResultSlotList.end(); ++resSlotIter)
	{
		CDnItemSlotButton* pBtn = (*resSlotIter).second;
		if (pBtn)
		{
			if (iter == pItemList->end())
			{
				MIInventoryItem* pItem = pBtn->GetItem();
				if (pBtn && pItem)
					SAFE_DELETE(pItem);
			}
			else
			{
				const CDnCostumeMixDataMgr::SResultItemInfo& info = *iter;
				const ITEMCLSID& curItem = info.itemId;
				TItem itemInfo;
				CDnItem::MakeItemInfo(curItem, 1, itemInfo);

				CDnItem* pItem = CDnItemTask::GetInstance().CreateItem(itemInfo);
				if (pItem)
				{
					MIInventoryItem* pSlotItem = pBtn->GetItem();
					if (pSlotItem)
						SAFE_DELETE(pSlotItem);
					pBtn->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
				}
				++iter;
			}
		}
	}

	if (m_pRandomResult->IsEnable() == false)
		m_pRandomResult->Enable(true);

	if (m_ResultSlotSelectCache.IsRandomSelected() == false)
	{
		if (m_ResultSlotSelectCache.pSelectStatic != NULL)
			RevertPartsFromPreview();
		ShowOffResultSelectSlot();
	}

	SetAbilityComboBox(m_CurResultPartsType);
}

int CDnCostumeMixDlg::GetEmptyStuffSlot() const
{
	int i = 0;
	for (; i < (int)m_pStuffSlotList.size(); ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlotList[i];
		if (pButton && pButton->IsEmptySlot())
			return pButton->GetSlotIndex();
	}

	return -1;
}

int CDnCostumeMixDlg::GetEmptyStuffSlotCount() const
{
	int i = 0, count = 0;
	for (; i < (int)m_pStuffSlotList.size(); ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlotList[i];
		if (pButton && pButton->IsEmptySlot())
			count++;
	}

	return count;
}

int CDnCostumeMixDlg::GetStuffCountInSlot() const
{
	int i = 0, count = 0;
	for (; i < (int)m_pStuffSlotList.size(); ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlotList[i];
		if (pButton && pButton->IsEmptySlot() == false)
			count++;
	}

	return count;
}

void CDnCostumeMixDlg::ClearInvenSlotCache()
{
	std::vector<SSlotCacheUnit>::iterator iter = m_pInvenSlotCacheList.begin();
	for(; iter != m_pInvenSlotCacheList.end(); ++iter)
	{
		SSlotCacheUnit& unit = *iter;
		if (unit.pInvenSlotBtn)
		{
			unit.pInvenSlotBtn->SetRegist(false);
			unit.pInvenSlotBtn->DisableSplitMode(true);
			unit.pInvenSlotBtn = NULL;
		}
	}

	m_pInvenSlotCacheList.clear();
}

void CDnCostumeMixDlg::ClearStuffSlot()
{
	std::vector<CDnItemSlotButton*>::iterator iter = m_pStuffSlotList.begin();
	for (; iter != m_pStuffSlotList.end(); ++iter)
	{
		CDnItemSlotButton* pBtn = *iter;
		if (pBtn)
			pBtn->ResetSlot();
	}

	ClearInvenSlotCache();

	OnEmptyStuffSlots();
}

void CDnCostumeMixDlg::ClearResultSlot()
{
	std::map<int, CDnItemSlotButton*>::iterator iter = m_pResultSlotList.begin();
	for (; iter != m_pResultSlotList.end(); ++iter)
	{
		CDnItemSlotButton* pBtn = (*iter).second;
		if (pBtn)
		{
			pBtn->ResetSlot();
			MIInventoryItem* pItem = pBtn->GetItem();
			if (pItem != NULL)
				SAFE_DELETE(pItem);
		}
	}

	m_pRandomResult->Enable(false);
	ShowOffResultSelectSlot();
	RevertPartsFromPreview();
}

void CDnCostumeMixDlg::ShowOffResultSelectSlot()
{
	if (m_ResultSlotSelectCache.pSelectStatic)
	{
		m_ResultSlotSelectCache.pSelectStatic->Show(false);
		m_ResultSlotSelectCache.pSelectStatic = NULL;
		m_ResultSlotSelectCache.resultSlotIdx = -1;
	}
}

void CDnCostumeMixDlg::OnEmptyStuffSlots()
{
	ClearResultSlot();

	m_pPrevResultBtn->Show(false);
	m_pNextResultBtn->Show(false);
	m_pPageStatic->Show(false);

	m_CurResultPartsType = CDnParts::AllParts_Amount;

	SetAbilityComboBox(m_CurResultPartsType);
}

void CDnCostumeMixDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if (pControl)
		SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case ePROGRESSDLG:
			{
				DoMix();
			}
			break;
		}
	}
}

void CDnCostumeMixDlg::CompleteMix(ITEMCLSID resultItem, char cOption)
{
	m_pProgressDlg->Show(false);

	if (resultItem == ITEMCLSID_NONE)
	{
		HandleError(eECM_GENERAL);
		return;
	}

	CEtSoundEngine::GetInstance().PlaySound( "2D", m_CompleteMixSound );

	m_pCompleteDlg->SetInfo(resultItem, cOption);
	m_pCompleteDlg->Show(true);
}

void CDnCostumeMixDlg::DisableAllDlgs(bool bEnable, const std::wstring& text)
{
	if (bEnable)
	{
		GetInterface().CloseMessageBox();
		GetInterface().MessageBox(text.c_str(), MB_OK, eDISABLEDLG, this, true, false, true, false);
	}
	else
	{
		GetInterface().CloseMessageBox();
	}
}