#include "StdAfx.h"
#include "DnCostumeDesignMixDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnCostumeMixPreviewDlg.h"
#include "DnCostumeMixProgressDlg.h"
#include "DnCostumeMixCompleteDlg.h"
#include "DnItemTask.h"
#include "DnCommonUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnCostumeDesignMixDlg::CDnCostumeDesignMixDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
		: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	SecureZeroMemory(m_pStuffSlots, sizeof(m_pStuffSlots));
	SecureZeroMemory(m_pInvenSlotsCache, sizeof(m_pInvenSlotsCache));
	m_pResultSlot		= NULL;
	m_pPreviewDlg		= NULL;
	m_pProgressDlg		= NULL;
	m_pCompleteDlg		= NULL;
	m_CurPartsType		= CDnParts::AllParts_Amount;
	m_CompleteMixSound	= -1;
}

CDnCostumeDesignMixDlg::~CDnCostumeDesignMixDlg(void)
{
	SAFE_DELETE(m_pPreviewDlg);
	SAFE_DELETE(m_pProgressDlg);
	SAFE_DELETE(m_pCompleteDlg);

	if (m_CompleteMixSound != -1)
		CEtSoundEngine::GetInstance().RemoveSound(m_CompleteMixSound);
}

void CDnCostumeDesignMixDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("Com_OptionBase.ui").c_str(), bShow);

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10044 );
	if (strlen( szFileName ) > 0)
		m_CompleteMixSound = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
}

void CDnCostumeDesignMixDlg::InitialUpdate()
{
	m_pStuffSlots[eDESIGN] = GetControl<CDnItemSlotButton>("ID_ITEME_DESIGN");
	m_pStuffSlots[eDESIGN]->SetSlotIndex(eDESIGN);
	m_pStuffSlots[eDESIGN]->SetSlotType(ST_ITEM_COSTUME_DMIX_STUFF);

	m_pStuffSlots[eABILITY] = GetControl<CDnItemSlotButton>("ID_ITEME_OPTION");
	m_pStuffSlots[eABILITY]->SetSlotIndex(eABILITY);
	m_pStuffSlots[eABILITY]->SetSlotType(ST_ITEM_COSTUME_DMIX_STUFF);

	m_pResultSlot = GetControl<CDnItemSlotButton>("ID_ITEME_COMPLETE");
	m_pResultSlot->SetSlotIndex(eDMIX_RESULT);
	m_pResultSlot->SetSlotType(ST_ITEM_COSTUME_DMIX_RESULT);

	m_pPreviewDlg = new CDnCostumeMixPreviewDlg(UI_TYPE_CHILD, this);
	m_pPreviewDlg->Initialize(false);
	m_pPreviewDlg->SetPreviewType(CDnCostumeMixPreviewDlg::eCOS_DMIX);

	m_pProgressDlg = new CDnCostumeMixProgressDlg(UI_TYPE_MODAL);
	m_pProgressDlg->Initialize(false);

	m_pCompleteDlg = new CDnCostumeMixCompleteDlg(UI_TYPE_MODAL);
	m_pCompleteDlg->Initialize(false);
	m_pCompleteDlg->SetType(CDnCostumeMixCompleteDlg::eDESIGNMIX);
}

void CDnCostumeDesignMixDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if (bShow == false)
	{
		int i = 0;
		for (; i < eDMIX_STUFF_MAX; ++i)
		{
			CDnItemSlotButton* pBtn = m_pStuffSlots[i];
			if (pBtn)
				ReleaseStuffItemSlotBtn(pBtn);
		}

		ReleaseResultSlot();

		if (m_pProgressDlg && m_pProgressDlg->IsShow())
			m_pProgressDlg->Show(false);

		GetInterface().CloseMessageBox();
	}
	else
	{
		m_CurPartsType = CDnParts::AllParts_Amount;
	}

	m_pPreviewDlg->Show(bShow);

	CDnLocalPlayerActor::LockInput(bShow);

	drag::Command(UI_DRAG_CMD_CANCEL);
	drag::ReleaseControl();

	CDnCustomDlg::Show( bShow );
}

int CDnCostumeDesignMixDlg::GetEmptyStuffSlot() const
{
	int i = 0;
	for (; i < eDMIX_STUFF_MAX; ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlots[i];
		if (pButton && pButton->IsEmptySlot())
			return pButton->GetSlotIndex();
	}

	return -1;
}

void CDnCostumeDesignMixDlg::AddAttachItem(CDnSlotButton *pFromButton)
{
	int index = GetEmptyStuffSlot();
	if (index < 0)
		return;

	if (index >= 0 && index < eDMIX_STUFF_MAX)
	{
		CDnItemSlotButton* pEmptyBtn = m_pStuffSlots[index];
		if (pEmptyBtn)
		{
			if (AddAttachItem(pFromButton, pEmptyBtn, CDnSlotButton::ITEM_ORIGINAL_COUNT) == false)
				drag::Command(UI_DRAG_CMD_CANCEL);
			drag::ReleaseControl();
		}
	}
}

void CDnCostumeDesignMixDlg::MakeResultSlot()
{
	if (CheckStuffSlotType() != eDMIX_STUFF_MAX)
		return;

	ReleaseResultSlot();

	// 임시 item 생성
	CDnItem* pAbilityItem = static_cast<CDnItem*>(m_pStuffSlots[eABILITY]->GetItem());
	if (pAbilityItem == NULL)
	{
		_ASSERT(0);
		return;
	}

	CDnItem* pDesignItem = static_cast<CDnItem*>(m_pStuffSlots[eDESIGN]->GetItem());
	if (pDesignItem == NULL)
	{
		_ASSERT(0);
		return;
	}

	TItemInfo abilityItemInfo;
	pAbilityItem->GetTItemInfo(abilityItemInfo);
	abilityItemInfo.Item.nLookItemID = (pDesignItem->GetLookItemID() != ITEMCLSID_NONE) ? pDesignItem->GetLookItemID() : pDesignItem->GetClassID();

	CDnItem* pItem = CDnItemTask::GetInstance().CreateItem(abilityItemInfo.Item);
	if (pItem)
		m_pResultSlot->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
}

void CDnCostumeDesignMixDlg::ReleaseResultSlot()
{
	if (m_pResultSlot == NULL || m_pResultSlot->IsEmptySlot())
		return;

	MIInventoryItem* pItem = m_pResultSlot->GetItem();
	if (pItem)
		SAFE_DELETE(pItem);
}

void CDnCostumeDesignMixDlg::AttachResultToPreview()
{
	if (m_pResultSlot == NULL)
	{
		RevertPartsFromPreview();
		return;
	}

	CDnItem* pItem = static_cast<CDnItem*>(m_pResultSlot->GetItem());
	if (CheckStuffSlotType() != eDMIX_STUFF_MAX || pItem == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_pPreviewDlg->AttachParts(pItem->GetLookItemID());
}

bool CDnCostumeDesignMixDlg::RevertPartsFromPreview()
{
	if (m_pPreviewDlg == NULL || m_pPreviewDlg->IsShow() == false)
		return true;

	if (CDnItemTask::IsActive())
	{
		int classId = m_pPreviewDlg->GetCurrentClassId();
		if (CommonUtil::IsValidCharacterClassId(classId))
			m_pPreviewDlg->MakeAvatarToRender(classId);

		return true;
	}

	_ASSERT(0);
	return false;
}

bool CDnCostumeDesignMixDlg::IsDesignMixableItem(CDnParts* pPartsItem) const
{
	if (pPartsItem == NULL)
		return false;

	CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
	return dataMgr.IsEnableCostumeDesignMixStuff(pPartsItem);
}

bool CDnCostumeDesignMixDlg::ReleaseStuffItemSlotBtn(CDnSlotButton* pStuffSlot, int slotIdxIfSlotChange /*= eDMIX_STUFF_EMPTY*/, bool bNoReleaseResultSlot /*= false*/)
{
	if (pStuffSlot == NULL)
	{
		HandleError(eECDMIX_NORELEASE_BUFFSLOT);
		return false;
	}

	pStuffSlot->ResetSlot();

	int slotIdx = pStuffSlot->GetSlotIndex();
	if (slotIdx >= eDMIX_STUFF_MIN && slotIdx < eDMIX_STUFF_MAX)
	{
		if (m_pInvenSlotsCache[slotIdx])
		{
			m_pInvenSlotsCache[slotIdx]->SetRegist(false);
			m_pInvenSlotsCache[slotIdx]->DisableSplitMode(true);
			m_pInvenSlotsCache[slotIdx] = NULL;

			if (bNoReleaseResultSlot == false)
			{
				ReleaseResultSlot();
				RevertPartsFromPreview();
			}
		}

		if (slotIdxIfSlotChange != eDMIX_STUFF_EMPTY)
		{
			m_pInvenSlotsCache[slotIdx] = m_pInvenSlotsCache[slotIdxIfSlotChange];
			m_pInvenSlotsCache[slotIdxIfSlotChange] = NULL;

			MakeResultSlot();
			AttachResultToPreview();
		}
	}

	return true;
}

bool CDnCostumeDesignMixDlg::AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot, int itemCount)
{
	if (pFromSlot == NULL || pAttachSlot == NULL)
	{
		_ASSERT(0);
		return false;
	}

	eSTUFF_TYPE attachSlotType = (eSTUFF_TYPE)pAttachSlot->GetSlotIndex();

	int i = 0;
	for (; i < eDMIX_STUFF_MAX; ++i)
	{
		if (m_pInvenSlotsCache[i] != NULL && m_pInvenSlotsCache[i] == pFromSlot)
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
	if (fromSlotType != ST_ITEM_COSTUME_DMIX_STUFF && fromSlotType != ST_INVENTORY_CASH)
	{
		HandleError(eECDMIX_ILLEGAL_DMIX_FROM_SLOT); // 합성할 수 없는 아이템입니다.
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	CDnItem* pItem = static_cast<CDnItem *>(pFromSlot->GetItem());
	if (pItem->IsCashItem() == false)
	{
		HandleError(eECDMIX_NO_CASH_STUFF);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (pItem->GetLookItemID() != ITEMCLSID_NONE)
	{
		HandleError(eECDMIX_CANT_DMIX_ALREADY_DMIX);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	std::vector<int> nVecJobList;
	if (CDnActor::s_hLocalActor)
	{
		((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if (pItem->IsPermitPlayer(nVecJobList) == false)
		{
			HandleError(eECDMIX_STUFF_DIFFCLASS);
			return false;
		}
	}

	if (pItem->IsEternityItem() == false)
	{
		HandleError(eECDMIX_STUFF_ETERNITY_ONLY);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (pItem->GetItemType() != ITEMTYPE_PARTS)
	{
		HandleError(eECDMIX_STUFF_NOPARTS);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	CDnParts* pPartsItem = static_cast<CDnParts*>(pItem);
	if (IsDesignMixableItem(pPartsItem) == false)
	{
		HandleError(eECDMIX_CANT_DMIX_STUFF);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (CheckStuffSlotType() != eDMIX_STUFF_EMPTY)
	{
		CDnParts::PartsTypeEnum curPartsType = pPartsItem->GetPartsType();
		if (curPartsType != m_CurPartsType)
		{
			HandleError(eECDMIX_DIFF_PARTS_TYPE);
			pFromSlot->DisableSplitMode(true);
			return false;
		}
	}
	else
	{
		m_CurPartsType = CDnParts::AllParts_Amount;
	}

	if (pAttachSlot->GetItem() != NULL)
		ReleaseStuffItemSlotBtn(pAttachSlot);

	if (fromSlotType == ST_ITEM_COSTUME_DMIX_STUFF)
	{
		ReleaseStuffItemSlotBtn(pAttachSlot, pFromSlot->GetSlotIndex());

		pFromSlot->ResetSlot();
	}
	else
	{
		pFromSlot->SetRegist(true);

		int slotIdx = pAttachSlot->GetSlotIndex();
		if (slotIdx >= eDMIX_STUFF_MIN && slotIdx < eDMIX_STUFF_MAX)
			m_pInvenSlotsCache[slotIdx] = pFromSlot;

		if (pItem->GetItemType() != ITEMTYPE_PARTS)
		{
			_ASSERT(0);
			pFromSlot->DisableSplitMode(true);
			return false;
		}
	}

	pAttachSlot->SetItem(pPartsItem, itemCount);
	m_CurPartsType = pPartsItem->GetPartsType();
	CEtSoundEngine::GetInstance().PlaySound( "2D", pPartsItem->GetDragSoundIndex() );

	eSTUFF_TYPE curStuffSlot = CheckStuffSlotType();
	if (curStuffSlot == eDMIX_STUFF_MAX)
	{
		MakeResultSlot();
		AttachResultToPreview();
	}

	return true;
}

void CDnCostumeDesignMixDlg::HandleError(CDnCostumeDesignMixDlg::eERR_COSDMIX code)
{
	std::wstring str;
	switch (code)
	{
	case eECDMIX_GENERAL:					str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130310);	break;	// UISTRING : 코스튬 합성 중 문제가 발생했습니다.
	case eECDMIX_ILLEGAL_DMIX_FROM_SLOT:	str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130308);	break;	// UISTRING : 합성할 수 없는 아이템입니다.
	case eECDMIX_CANT_DMIX_STUFF:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130308);	break;	// UISTRING : 합성할 수 없는 아이템입니다.
	case eECDMIX_NO_CASH_STUFF:				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130327);	break;	// UISTRING : 캐시 아이템이 아닙니다.
	case eECDMIX_STUFF_DIFFCLASS:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130328);	break;	// UISTRING : 자신과 같은 클래스의 아이템만 합성할 수 있습니다.
	case eECDMIX_STUFF_ETERNITY_ONLY:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130329);	break;	// UISTRING : 영구 아이템만 합성할 수 있습니다.
	case eECDMIX_STUFF_NOPARTS:				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130330);	break;	// UISTRING : 파츠 아이템만 합성할 수 있습니다.
	case eECDMIX_NORELEASE_BUFFSLOT:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130310);	break;	// UISTRING : 코스튬 합성 중 문제가 발생했습니다.
	case eECDMIX_DIFF_PARTS_TYPE:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130331);	break;	// UISTRING : 같은 파츠 타입끼리만 합성할 수 있습니다.
	case eECDMIX_STUFFCOUNT_NONE:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130332);	break;	// UISTRING : 합성할 아이템을 올려주세요.
	case eECDMIX_CANT_DMIX_ALREADY_DMIX:	str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130335);	break;	// UISTRING : 합성된 아이템은 다시 합성 할 수 없습니다.
	default:
		str = FormatW(L"%s (CODE:%d)", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130310), code);	break;	// UISTRING : 코스튬 합성 중 문제가 발생했습니다.
	}

	GetInterface().MessageBox(str.c_str(), MB_OK);
}

void CDnCostumeDesignMixDlg::Process(float fElapsedTime)
{
	if (m_bShow)
	{
		if (CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsLocalActorEnterGateReady())
		{
			if (CDnItemTask::IsActive())
				CDnItemTask::GetInstance().RequestItemCostumeDesignMixClose();

			return;
		}
	}

	CDnCustomDlg::Process(fElapsedTime);
}

void CDnCostumeDesignMixDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (strstr(pControl->GetControlName(), "ID_ITEME_") && IsCmdControl("ID_ITEME_COMPLETE") == false)
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			CDnItemSlotButton *pPressedButton = static_cast<CDnItemSlotButton*>(pControl);

			if (uMsg == WM_RBUTTONUP)
			{
				if (ReleaseStuffItemSlotBtn(pPressedButton) == false)
					return;
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
		else if (IsCmdControl("ID_BUTTON_CANCEL") || IsCmdControl("ID_BUTTON_CLOSE"))
		{
			if (CDnItemTask::IsActive())
				CDnItemTask::GetInstance().RequestItemCostumeDesignMixClose();
			return;
		}
		if (IsCmdControl("ID_BUTTON_BUYGACHA"))
		{
			eRET_CHECKMIX ret = CheckMix();
			if (ret == eRET_ABLE)
			{
				m_pProgressDlg->SetMessageBox(COSTUMEDESIGNMIX_PROGRESS_BAR_TIME, COSTUMEDESIGNMIX_PROGRESS_BAR_TIME, ePROGRESSDLG, this);
				m_pProgressDlg->Show(true);
			}
			else if (ret == eRET_WARNING_DIFF_RANK)
			{
				GetInterface().CloseMessageBox();
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130326), MB_YESNO, eWARNINGDIFFDLG, this, true, true); // UISTRING: 능력을 결정할 아이템보다 디자인을 결정하는 아이템의 등급이 더 높습니다. 정말 합성 하시겠습니까?
			}
			return;
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

CDnCostumeDesignMixDlg::eSTUFF_TYPE CDnCostumeDesignMixDlg::CheckStuffSlotType() const
{
	eSTUFF_TYPE stuffType = eDMIX_STUFF_EMPTY;
	if (m_pStuffSlots[eABILITY] != NULL && m_pStuffSlots[eABILITY]->GetItem() != NULL) 
		stuffType = eABILITY;
	if (m_pStuffSlots[eDESIGN] != NULL && m_pStuffSlots[eDESIGN]->GetItem() != NULL)
		stuffType = (stuffType == eABILITY) ? eDMIX_STUFF_MAX : eDESIGN;

	return stuffType;
}

void CDnCostumeDesignMixDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
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

		case eWARNINGDIFFDLG:
			{
				if (strcmp( pControl->GetControlName(), "ID_YES" ) == 0) 
				{
					m_pProgressDlg->SetMessageBox(COSTUMEDESIGNMIX_PROGRESS_BAR_TIME, COSTUMEDESIGNMIX_PROGRESS_BAR_TIME, ePROGRESSDLG, this);
					m_pProgressDlg->Show(true);
				}
			}
			break;
		}
	}
}

void CDnCostumeDesignMixDlg::DisableAllDlgs(bool bEnable, const std::wstring& text)
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

CDnCostumeDesignMixDlg::eRET_CHECKMIX CDnCostumeDesignMixDlg::CheckMix()
{
	if (CDnItemTask::IsActive() && CDnItemTask::GetInstance().IsLockCosDesignMixSendPacket())
		return eRET_UNABLE;

	if (CheckStuffSlotType() != eDMIX_STUFF_MAX)
	{
		HandleError(eECDMIX_STUFFCOUNT_NONE);
		return eRET_UNABLE;
	}

	const CDnItem* pAbility = static_cast<CDnItem*>(m_pStuffSlots[eABILITY]->GetItem());
	if (pAbility == NULL)
	{
		_ASSERT(0);
		return eRET_UNABLE;
	}

	const CDnItem* pDesign = static_cast<CDnItem*>(m_pStuffSlots[eDESIGN]->GetItem());
	if (pDesign == NULL)
	{
		_ASSERT(0);
		return eRET_UNABLE;
	}

	if (pDesign->GetItemRank() > pAbility->GetItemRank())
		return eRET_WARNING_DIFF_RANK;

	return eRET_ABLE;
}

void CDnCostumeDesignMixDlg::CompleteMix(CDnItem* pCompleteInvenItem)
{
	m_pProgressDlg->Show(false);

	CEtSoundEngine::GetInstance().PlaySound( "2D", m_CompleteMixSound );

	if (pCompleteInvenItem == NULL)
	{
		_ASSERT(0);
		return;
	}

	int i = 0;
	for (; i < eDMIX_STUFF_MAX; ++i)
	{
		CDnItem* pCurStuff = static_cast<CDnItem*>(m_pInvenSlotsCache[i]->GetItem());
		if (pCurStuff && pCurStuff->GetSerialID() == pCompleteInvenItem->GetSerialID())
		{
			if (m_pInvenSlotsCache[i]->GetSlotType() == ST_INVENTORY_CASH)
			{
				CDnQuickSlotButton* pQBtn = static_cast<CDnQuickSlotButton*>(m_pInvenSlotsCache[i]);
				pQBtn->RefreshIcon(*pCompleteInvenItem);
				pQBtn->SetQuickItem(pCompleteInvenItem);
			}
		}
	}

	ClearStuffSlot(true);

	CDnItem* pResult = static_cast<CDnItem*>(m_pResultSlot->GetItem());
	if (pResult)
	{
		m_pCompleteDlg->SetInfo(pResult);
		m_pCompleteDlg->Show(true);

		ReleaseResultSlot();
	}
	else
	{
		//	using messagebox
	}
}

void CDnCostumeDesignMixDlg::ClearStuffSlot(bool bNoReleaseResultSlot)
{
	int i = 0;
	for(; i < eDMIX_STUFF_MAX; ++i)
	{
		CDnItemSlotButton* pBtn = m_pStuffSlots[i];
		if (pBtn)
			ReleaseStuffItemSlotBtn(pBtn, eDMIX_STUFF_EMPTY, bNoReleaseResultSlot);
	}
}

void CDnCostumeDesignMixDlg::DoMix()
{
	if (CheckMix() == eRET_UNABLE)
		return;

	INT64 stuffSerials[eDMIX_STUFF_MAX];
	ZeroMemory(stuffSerials, sizeof(stuffSerials));

	int i = 0;
	bool bNoStuff = false;
	for (; i < eDMIX_STUFF_MAX; ++i)
	{
		CDnItemSlotButton* pBtn = m_pStuffSlots[i];
		if (pBtn)
		{
			CDnItem* pItem = static_cast<CDnItem*>(pBtn->GetItem());
			if (pItem)
				stuffSerials[i] = pItem->GetSerialID();
			else
				bNoStuff = true;
		}
		else
		{
			bNoStuff = true;
		}
	}

	if (bNoStuff)
		return;

	if (CDnItemTask::IsActive())
	{
		CDnItemTask::GetInstance().RequestItemCostumeDesignMix(stuffSerials);
	}
}