#include "StdAfx.h"

#ifdef PRE_ADD_COSRANDMIX

#include "DnCostumeRandomMixDlg.h"
#include "DnInterface.h"
#include "DnCostumeMixPreviewDlg.h"
#include "DnCostumeMixCompleteDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnItemTask.h"
#include "DnRandomProgressMessageBox.h"
#include "DnDummyModalDlg.h"
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
#include "DnMoneyInputDlg.h"
#endif
#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "DnInvenCashDlg.h"
#include "DnInvenSlotDlg.h"
#include "DnQuickSlotButton.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
CDnCostumeRandomMixDlg::SFeeForNpcMixUI::SFeeForNpcMixUI()
{
	pBackground = NULL;
	pFeeTitle = NULL;
	pMoneyBase = NULL;
	pGold = NULL;
	pSilver = NULL;
	pCopper = NULL;

	dwColorGold = dwColorSilver = dwColorCopper = textcolor::WHITE;
	bInit = false;
}

void CDnCostumeRandomMixDlg::SFeeForNpcMixUI::Show(bool bShow)
{
	if (bInit == false)
		return;

	pBackground->Show(bShow);
	pFeeTitle->Show(bShow);
	pMoneyBase->Show(bShow);
	pGold->Show(bShow);
	pSilver->Show(bShow);
	pCopper->Show(bShow);
}

bool CDnCostumeRandomMixDlg::SFeeForNpcMixUI::IsShow() const
{
	return (bInit && pBackground && pBackground->IsShow());
}

void CDnCostumeRandomMixDlg::SFeeForNpcMixUI::SetMoney(INT64 nMoney)
{
	if (IsShow() == false)
		return;

	if (nMoney == 0)
	{
		pGold->SetTextColor(dwColorGold);
		pSilver->SetTextColor(dwColorSilver);
		pCopper->SetTextColor(dwColorCopper);

		pGold->SetInt64ToText(0);
		pSilver->SetInt64ToText(0);
		pCopper->SetInt64ToText(0);
		return;
	}

	wchar_t wszMoney[MAX_FEE_CHAR_COUNT] = {0, };
	INT64 nMoneyUnit = 0;

	if (CDnItemTask::IsActive())
	{
		if (CDnItemTask::GetInstance().GetCoin() < nMoney)
		{
			pGold->SetTextColor(textcolor::RED);
			pSilver->SetTextColor(textcolor::RED);
			pCopper->SetTextColor(textcolor::RED);
		}
		else
		{
			pGold->SetTextColor(dwColorGold);
			pSilver->SetTextColor(dwColorSilver);
			pCopper->SetTextColor(dwColorCopper);
		}
	}

	CDnMoneyControl::GetStrMoneyG(nMoney, wszMoney, &nMoneyUnit);
	pGold->SetInt64ToText(nMoneyUnit);
	CDnMoneyControl::GetStrMoneyS(nMoney, wszMoney, &nMoneyUnit);
	pSilver->SetInt64ToText(nMoneyUnit);
	CDnMoneyControl::GetStrMoneyC(nMoney, wszMoney, &nMoneyUnit);
	pCopper->SetInt64ToText(nMoneyUnit);
}

//////////////////////////////////////////////////////////////////////////
#endif //  PRE_ADD_COSRANDMIX_ACCESSORY

CDnCostumeRandomMixDlg::CDnCostumeRandomMixDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
		: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true ),
		m_fPreviewChangePartsTermSec(2.f)
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
		, m_fPreviewChangeAccessoryTermSec(2.f)
#endif
{
	SecureZeroMemory(m_pStuffSlots, sizeof(m_pStuffSlots));
	SecureZeroMemory(m_pInvenSlotsCache, sizeof(m_pInvenSlotsCache));

	m_pResultSlot = NULL;
	m_pPreviewDlg = NULL;
	m_pCompleteDlg = NULL;
	m_pMixBtn = NULL;
	m_pCancelBtn = NULL;
	m_pCloseBtn = NULL;
	m_CompleteMixSound	= -1;
	m_CurPartsType = CDnParts::AllParts_Amount;
	m_pExpectItemName = NULL;
	m_pProgressBarText = NULL;
	m_pProgressBarTime = NULL;

	m_fMaxTimerSec = 0.f;
	m_fTimerOffset = 0.f;
	m_fMaxTimerOffsetSec = 0.5f;
	m_fTimer = 0.f;
	m_pPreviewItem = NULL;
	m_CurrentResultItemId = ITEMCLSID_NONE;
	m_pDummyModalDlg = NULL;

	m_bPreviewDemo = false;
	m_fPreviewChangePartsTimer = 0.f;

	m_bInDrawProgress = false;
	m_fCompleteTimer = 0.0f;
}

CDnCostumeRandomMixDlg::~CDnCostumeRandomMixDlg(void)
{
	SAFE_DELETE(m_pPreviewDlg);
	SAFE_DELETE(m_pCompleteDlg);
	SAFE_DELETE(m_pDummyModalDlg);

	ClearItemCache();

	if (m_CompleteMixSound != -1)
		CEtSoundEngine::GetInstance().RemoveSound(m_CompleteMixSound);
}

void CDnCostumeRandomMixDlg::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("Com_RandomOptionBase.ui").c_str(), bShow);

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10044 );
	if( strlen( szFileName ) > 0 )
		m_CompleteMixSound = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );

	m_fPreviewChangePartsTermSec = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CostumeRandomMixPreviewChangePartsTime) * 0.001f;
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	m_fPreviewChangeAccessoryTermSec = 2.f;
#endif

	m_pDummyModalDlg = new CDnDummyModalDlg;
	m_pDummyModalDlg->Initialize(false);
}

void CDnCostumeRandomMixDlg::InitialUpdate()
{
	int i = 0;
	std::string ctrlName;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		ctrlName = FormatA("ID_ITEME_RANDOMOPTION%d", i);
		m_pStuffSlots[i] = GetControl<CDnItemSlotButton>(ctrlName.c_str());
		m_pStuffSlots[i]->SetSlotIndex(i);
		m_pStuffSlots[i]->SetSlotType(ST_ITEM_COSTUME_RANDOMMIX_STUFF);
	}

	m_pResultSlot = GetControl<CDnItemSlotButton>("ID_ITEME_RANDOMCOMPLETE");
	m_pResultSlot->SetSlotIndex(RANDOMMIX_RESULT_SLOT_INDEX);
	m_pResultSlot->SetSlotType(ST_ITEM_COSTUME_RANDOMMIX_RESULT);
	m_pResultSlot->SetShowTooltip(false);

	m_pPreviewDlg = new CDnCostumeMixPreviewDlg(UI_TYPE_CHILD, this);
	m_pPreviewDlg->Initialize(false);
	m_pPreviewDlg->SetPreviewType(CDnCostumeMixPreviewDlg::eCOS_DMIX);

	m_pCompleteDlg = new CDnCostumeMixCompleteDlg(UI_TYPE_MODAL);
	m_pCompleteDlg->Initialize(false);
	m_pCompleteDlg->SetType(CDnCostumeMixCompleteDlg::eRANDOMMIX);

	m_pMixBtn = GetControl<CEtUIButton>("ID_BUTTON_BUYGACHA");
	m_pCancelBtn = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
	m_pCloseBtn = GetControl<CEtUIButton>("ID_BUTTON_CLOSE");

	m_pExpectItemName = GetControl<CEtUIStatic>("ID_TEXT_ITEMNAME");
	m_pProgressBarText = GetControl<CEtUIStatic>("ID_TEXT_TIME");
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	m_FeeUIs.pBackground = GetControl<CEtUIStatic>("ID_STATIC_COUNTBOARD");
	m_FeeUIs.pFeeTitle = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
	m_FeeUIs.pMoneyBase = GetControl<CEtUIStatic>("ID_STATIC_COUNT");
	m_FeeUIs.pGold = GetControl<CEtUIStatic>("ID_GOLD");
	m_FeeUIs.pSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_FeeUIs.pCopper = GetControl<CEtUIStatic>("ID_BRONZE");

	m_FeeUIs.dwColorGold = m_FeeUIs.pGold->GetTextColor();
	m_FeeUIs.dwColorSilver = m_FeeUIs.pSilver->GetTextColor();
	m_FeeUIs.dwColorCopper = m_FeeUIs.pCopper->GetTextColor();

	m_FeeUIs.SetInit(true);
	m_FeeUIs.Show(false);
#endif
}

void CDnCostumeRandomMixDlg::ClearAndInitializeMix()
{
	int i = 0;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		CDnItemSlotButton* pBtn = m_pStuffSlots[i];
		if (pBtn)
			ReleaseStuffItemSlotBtn(pBtn);
	}

	OnMixStartOrEnd(false);
	GetInterface().CloseMessageBox();

	ClearItemCache();
	m_pProgressBarTime->SetProgress(0.f);
	m_pProgressBarText->Show(false);
	m_pExpectItemName->Show(false);

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	if (CDnItemTask::IsActive())
	{
		const CDnCostumeRandomMixDataMgr& mgr = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr();
		bool bShowMoneyCtrl = (mgr.GetCurrentOpenType() == CostumeMix::RandomMix::OpenByNpc);
		m_FeeUIs.Show(bShowMoneyCtrl);
		if (bShowMoneyCtrl)
			m_FeeUIs.SetMoney(0);
	}
#endif

	m_CurPartsType = CDnParts::AllParts_Amount;
	m_fCompleteTimer = 0.f;
	m_CurrentResultItemId = ITEMCLSID_NONE;
}

void CDnCostumeRandomMixDlg::ClearItemCache()
{
	SAFE_DELETE(m_pPreviewItem);
}

void CDnCostumeRandomMixDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

#ifdef PRE_MOD_SYSTEM_STATE
	if (bShow == true)
		GetInterface().GetSystemStateMgr().StartState(eSTATE_COSTUMEMIX_RANDOM);
	else
		GetInterface().GetSystemStateMgr().EndState(eSTATE_COSTUMEMIX_RANDOM);
#endif

#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
	if( bShow == false )
		EnableSameItemFromCashInven( m_pStuffSlots[0] != NULL ? m_pStuffSlots[0] : NULL );
#endif 

	ClearAndInitializeMix();

	m_pPreviewDlg->Show(bShow);
	m_pProgressBarText->Show(false);

	CDnLocalPlayerActor::LockInput(bShow);

	drag::Command(UI_DRAG_CMD_CANCEL);
	drag::ReleaseControl();

	if (bShow == false)
		SetDummyModalDlg(false);

	CDnCustomDlg::Show( bShow );
}

void CDnCostumeRandomMixDlg::AddAttachItem(CDnSlotButton *pFromButton)
{
	int index = GetEmptyStuffSlotIndex();
	if (index < 0)
		return;

	if (index >= 0 && index < MAX_COSTUME_RANDOMMIX_STUFF)
	{
		CDnItemSlotButton* pEmptyBtn = m_pStuffSlots[index];
		if (pEmptyBtn)
		{
			if (AddAttachItem(pFromButton, pEmptyBtn, CDnSlotButton::ITEM_ORIGINAL_COUNT) == false)
				drag::Command(UI_DRAG_CMD_CANCEL);
			drag::ReleaseControl();
		}

#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
		EnableSameItemFromCashInven( m_pStuffSlots[index] );
#endif 
	}
}
#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
void CDnCostumeRandomMixDlg::ResetSameItemFromCashInven() 
{
	int nCurSlotIndex = 0;
	CDnItem* pItem = static_cast<CDnItem*>( m_pStuffSlots[0]->GetItem() );
	if( pItem )
		nCurSlotIndex = pItem->GetSlotIndex();

	CDnInvenTabDlg* pInvenTabDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
	if( ! pInvenTabDlg ) 
		return;

	CDnInvenCashDlg* pCashDlg = pInvenTabDlg->GetCashInvenDlg();		
	if( pCashDlg )
	{
		CDnItem* pTempItem = NULL; CDnParts* pPartsItem = NULL;
		CDnQuickSlotButton * pSlotBtn = NULL; CDnInvenSlotDlg* pInvenSlot = NULL;
		std::vector< CDnInvenSlotDlg* > vCashInvenList;

		vCashInvenList = pCashDlg->GetInvenSlotDlgList();
		for( int i = 0; i < (int)vCashInvenList.size(); ++i )
		{
			pInvenSlot = vCashInvenList[i];
			if( pInvenSlot == NULL )
				continue;

			const std::vector< CDnQuickSlotButton* > & vSlotButtons = pInvenSlot->GetSlotButtons();
			size_t size = vSlotButtons.size();
			for( size_t j=0; j<size; ++j )
			{
				pSlotBtn = vSlotButtons.at( j );
				if( !pSlotBtn || !pSlotBtn->GetItem() )
					continue;

				if( nCurSlotIndex != pSlotBtn->GetItem()->GetSlotIndex() )
					pSlotBtn->SetRegist( false );
			}
		}
	};
}

void CDnCostumeRandomMixDlg::EnableSameItemFromCashInven( CDnSlotButton* pSlotButton )
{
	CDnParts* pItem;
	pItem = static_cast<CDnParts*>( pSlotButton->GetItem() );

	if( ! pItem || pSlotButton->GetSlotIndex() != 0 ) 
		return;		

	CDnInvenTabDlg* pInvenTabDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
	if( ! pInvenTabDlg || ! pInvenTabDlg->IsShow()) 
		return;

	int nCurPartsType = 0, OtherPartsType = 0;
	CDnInvenCashDlg* pCashDlg = pInvenTabDlg->GetCashInvenDlg();		
	if( pCashDlg )
	{
		CDnItem* pTempItem = NULL; 
		CDnQuickSlotButton * pSlotBtn = NULL; CDnInvenSlotDlg* pInvenSlot = NULL;
		std::vector< CDnInvenSlotDlg* > vCashInvenList;
		
		vCashInvenList = pCashDlg->GetInvenSlotDlgList();
		for( int i = 0; i < (int)vCashInvenList.size(); ++i )
		{
			pInvenSlot = vCashInvenList[i];
			if( pInvenSlot == NULL )
				continue;

			const std::vector< CDnQuickSlotButton* > & vSlotButtons = pInvenSlot->GetSlotButtons();
			size_t size = vSlotButtons.size();
			for( size_t j=0; j<size; ++j )
			{
				pSlotBtn = vSlotButtons.at( j );
				if( !pSlotBtn || !pSlotBtn->GetItem() )
					continue;

				pTempItem = static_cast< CDnItem* >( pSlotBtn->GetItem() );
	
				if( pTempItem->GetItemType() != pItem->GetItemType() ) 
					continue;
				
				nCurPartsType = (int)CDnParts::GetPartsType( pItem->GetClassID() );
				OtherPartsType = (int)CDnParts::GetPartsType(pTempItem->GetClassID());

				if( nCurPartsType == OtherPartsType )
				{
					pSlotBtn->SetRegist( pItem->GetSlotIndex() == pTempItem->GetSlotIndex() );
				}
				else 
				{
					if( pSlotBtn->GetItemCondition() != MIInventoryItem::Unusable )
						pSlotBtn->SetRegist( true );
				}
			}
		}
	}


}
#endif 

int CDnCostumeRandomMixDlg::GetEmptyStuffSlotIndex() const
{
	int i = 0;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlots[i];
		if (pButton && pButton->IsEmptySlot())
			return pButton->GetSlotIndex();
	}

	return -1;
}

bool CDnCostumeRandomMixDlg::AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot, int itemCount)
{
	if (pFromSlot == NULL || pAttachSlot == NULL)
	{
		_ASSERT(0);
		return false;
	}

	int i = 0;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
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
	if (fromSlotType != ST_ITEM_COSTUME_RANDOMMIX_STUFF && fromSlotType != ST_INVENTORY_CASH)
	{
		HandleError(eECRMIX_ILLEGAL_DMIX_FROM_SLOT); // 합성할 수 없는 아이템입니다.
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	CDnItem* pItem = static_cast<CDnItem *>(pFromSlot->GetItem());
	if (pItem->IsCashItem() == false)
	{
		HandleError(eECRMIX_NO_CASH_STUFF);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (pItem->GetLookItemID() != ITEMCLSID_NONE)
	{
		HandleError(eECRMIX_CANT_MIX_ALREADY_DMIX);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	std::vector<int> nVecJobList;
	if (CDnActor::s_hLocalActor)
	{
		((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if (pItem->IsPermitPlayer(nVecJobList) == false)
		{
			HandleError(eECRMIX_STUFF_DIFFCLASS);
			return false;
		}
	}

	if (pItem->IsEternityItem() == false)
	{
		HandleError(eECRMIX_STUFF_ETERNITY_ONLY);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (pItem->GetItemType() != ITEMTYPE_PARTS)
	{
		HandleError(eECRMIX_STUFF_NOPARTS);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	CDnParts* pPartsItem = static_cast<CDnParts*>(pItem);
	if (IsRandomMixableItem(pPartsItem) == false)
	{
		HandleError(eECRMIX_CANT_DMIX_STUFF);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	int stuffCount = GetStuffCountInSlot();
	if (stuffCount > 0)
	{
		bool bNeedCheck = true;
		if (stuffCount == 1)
		{
			int i = 0;
			for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
			{
				const CDnItemSlotButton* pButton = m_pStuffSlots[i];
				if (pButton && pButton->IsEmptySlot() == false)
				{
					if (pButton == pAttachSlot)
					{
						bNeedCheck = false;
						break;
					}
				}
			}
		}

		if (bNeedCheck)
		{
			CDnParts::PartsTypeEnum curPartsType = pPartsItem->GetPartsType();
			if (curPartsType != m_CurPartsType)
			{
				HandleError(eECRMIX_DIFF_PARTS_TYPE);
				pFromSlot->DisableSplitMode(true);
				return false;
			}
		}
	}

	if (pPartsItem->IsExistSubParts())
	{
		HandleError(eECRMIX_CANT_MIX_SETITEM);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (fromSlotType == ST_ITEM_COSTUME_RANDOMMIX_STUFF)
	{
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (pAttachSlot->GetItem() != NULL)
		ReleaseStuffItemSlotBtn(pAttachSlot);

	pFromSlot->SetRegist(true);

	int slotIdx = pAttachSlot->GetSlotIndex();
	if (slotIdx >= 0 && slotIdx < MAX_COSTUME_RANDOMMIX_STUFF)
		m_pInvenSlotsCache[slotIdx] = pFromSlot;

	if (pItem->GetItemType() != ITEMTYPE_PARTS)
	{
		_ASSERT(0);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	pAttachSlot->SetItem(pPartsItem, itemCount);
	m_CurPartsType = pPartsItem->GetPartsType();
	CEtSoundEngine::GetInstance().PlaySound__( "2D", pPartsItem->GetDragSoundIndex() );

	if (GetEmptyStuffSlotIndex() < 0)
	{
		StartPreviewDemo();
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
		if (CDnItemTask::IsActive() && m_FeeUIs.IsShow())
		{
			const CDnCostumeRandomMixDataMgr& mgr = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr();
			int i = 0;
			std::vector<int> stuffItemIDs;
			for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
			{
				if (m_pStuffSlots[i] == NULL)
					return false;

				const MIInventoryItem* pItem = m_pStuffSlots[i]->GetItem();
				if (pItem == NULL || pItem->GetType() != MIInventoryItem::Item)
					return false;

				stuffItemIDs.push_back(pItem->GetClassID());
			}

			INT64 nFee = mgr.GetRandomMixFee(stuffItemIDs);
			if (nFee < 0)
				return false;

			m_FeeUIs.SetMoney(nFee);
		}
#endif
	}

	return true;
}

void CDnCostumeRandomMixDlg::StartPreviewDemo()
{
	if (MakePreviewItemList() == false)
		HandleErrorOnMakePreviewItemList();

	if (m_PreviewItemList.empty() == false)
	{
		m_Random.srand(timeGetTime());
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
		m_fPreviewChangePartsTimer = CDnParts::IsAccessoryItem(m_CurPartsType, true) ? m_fPreviewChangeAccessoryTermSec : m_fPreviewChangePartsTermSec;
#else
		m_fPreviewChangePartsTimer = m_fPreviewChangePartsTermSec;
#endif
	}

	m_bPreviewDemo = !m_PreviewItemList.empty();
}

bool CDnCostumeRandomMixDlg::MakePreviewItemList()
{
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	const CDnCostumeRandomMixDataMgr& mgr = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr();
	return mgr.MakePreviewItemList(m_PreviewItemList, m_CurPartsType);
#else
	int i = 0;
	std::vector<ITEMCLSID> stuffItemIDs;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		if (m_pStuffSlots[i] == NULL)
			return false;

		const MIInventoryItem* pItem = m_pStuffSlots[i]->GetItem();
		if (pItem == NULL || pItem->GetType() != MIInventoryItem::Item)
			return false;

		stuffItemIDs.push_back(pItem->GetClassID());
	}

	const CDnCostumeRandomMixDataMgr& mgr = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr();
	return mgr.MakePreviewItemList(m_PreviewItemList, m_CurPartsType, stuffItemIDs);
#endif
}

void CDnCostumeRandomMixDlg::StopPreviewDemo()
{
	if (m_pPreviewDlg == NULL || m_pPreviewDlg->IsShow() == false)
		return;

	m_bPreviewDemo = false;
	m_pPreviewDlg->ClearParts(m_pPreviewDlg->GetCurrentClassId());
}

void CDnCostumeRandomMixDlg::HandleError(eERR_COSRANDOMMIX code) const
{
	std::wstring str;
	switch (code)
	{
	case eECRMIX_GENERAL:					str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130310);	break;	// UISTRING : 코스튬 합성 중 문제가 발생했습니다.
	case eECRMIX_ILLEGAL_DMIX_FROM_SLOT:	str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130308);	break;	// UISTRING : 합성할 수 없는 아이템입니다.
	case eECRMIX_CANT_DMIX_STUFF:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130308);	break;	// UISTRING : 합성할 수 없는 아이템입니다.
	case eECRMIX_NO_CASH_STUFF:				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130327);	break;	// UISTRING : 캐시 아이템이 아닙니다.
	case eECRMIX_STUFF_DIFFCLASS:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130328);	break;	// UISTRING : 자신과 같은 클래스의 아이템만 합성할 수 있습니다.
	case eECRMIX_STUFF_ETERNITY_ONLY:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130329);	break;	// UISTRING : 영구 아이템만 합성할 수 있습니다.
	case eECRMIX_STUFF_NOPARTS:				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130330);	break;	// UISTRING : 파츠 아이템만 합성할 수 있습니다.
	case eECRMIX_NORELEASE_BUFFSLOT:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130310);	break;	// UISTRING : 코스튬 합성 중 문제가 발생했습니다.
	case eECRMIX_DIFF_PARTS_TYPE:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130331);	break;	// UISTRING : 같은 파츠 타입끼리만 합성할 수 있습니다.
	case eECRMIX_STUFFCOUNT_NONE:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130332);	break;	// UISTRING : 합성할 아이템을 올려주세요.
	case eECRMIX_STUFFCOUNT_SHORT:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130312);	break;	// UISTRING : 합성 재료 개수가 부족합니다
	case eECRMIX_CANT_MIX_ALREADY_DMIX:		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130335);	break;	// UISTRING : 합성된 아이템은 다시 합성 할 수 없습니다.
	case eECRMIX_CANT_MIX_SETITEM:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130340);	break;	// UISTRING : 한벌 옷은 합성할 수 없습니다.
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	case eECRMIX_FEE_INSUFFICIENT:			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4036);	break;	// UISTRING : 수수료가 부족합니다.
#endif
	default:
		str = FormatW(L"%s (CODE:%d)", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130310), code);	break;	// UISTRING : 코스튬 합성 중 문제가 발생했습니다.
	}

	GetInterface().MessageBox(str.c_str(), MB_OK);
}

bool CDnCostumeRandomMixDlg::IsRandomMixableItem(CDnParts* pPartsItem) const
{
	const CDnCostumeRandomMixDataMgr& mgr = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr();
	return mgr.IsEnableCostumeRandomMixStuff(pPartsItem);
}

bool CDnCostumeRandomMixDlg::ReleaseStuffItemSlotBtn(CDnSlotButton* pStuffSlot)
{
	if (pStuffSlot == NULL)
		return false;

	const int slotIdx = pStuffSlot->GetSlotIndex();
	ReleaseInvenSlotCache(slotIdx);

	pStuffSlot->ResetSlot();
	StopPreviewDemo();

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	if (m_FeeUIs.IsShow())
		m_FeeUIs.SetMoney(0);
#endif

	if (GetStuffCountInSlot() <= 0)
		m_CurPartsType = CDnParts::AllParts_Amount;
	
#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
	CDnItemSlotButton* pItemSlot = static_cast<CDnItemSlotButton*>(pStuffSlot);
	if( pItemSlot->GetSlotIndex() == 0 ) ResetSameItemFromCashInven();
#endif

	return true;
}

void CDnCostumeRandomMixDlg::CompleteMix(int nResultItemId)
{
	if (nResultItemId == ITEMCLSID_NONE)
	{
		HandleError(eECRMIX_GENERAL);
		return;
	}

	SetResultFromServer(nResultItemId);

	m_fCompleteTimer = 0.3f;
}

bool CDnCostumeRandomMixDlg::ReleaseInvenSlotCache(int slotIdx)
{
	int i = 0;

	if (slotIdx < 0 || slotIdx >= MAX_COSTUME_RANDOMMIX_STUFF)
		return false;

	if (m_pInvenSlotsCache[slotIdx])
	{
		m_pInvenSlotsCache[slotIdx]->SetRegist(false);
		m_pInvenSlotsCache[slotIdx]->DisableSplitMode(true);
		m_pInvenSlotsCache[slotIdx] = NULL;
	}

	return true;
}

int CDnCostumeRandomMixDlg::GetStuffCountInSlot() const
{
	int i = 0, count = 0;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlots[i];
		if (pButton && pButton->IsEmptySlot() == false)
			count++;
	}

	return count;
}

void CDnCostumeRandomMixDlg::Process(float fElapsedTime)
{
	if (m_bShow)
	{
		if (CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsLocalActorEnterGateReady())
		{
			if (CDnItemTask::IsActive())
				CDnItemTask::GetInstance().RequestItemCostumeRandomMixClose();

			return;
		}

		if (m_bPreviewDemo)
		{
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
			const float& fChangeTerm = CDnParts::IsAccessoryItem(m_CurPartsType, true) ? m_fPreviewChangeAccessoryTermSec : m_fPreviewChangePartsTermSec;
			m_fPreviewChangePartsTimer += fElapsedTime;
			if (m_fPreviewChangePartsTimer >= fChangeTerm)
			{
				ITEMCLSID nAttachItemId = DrawRandomItemIdFromPreviewItemList();
				if (nAttachItemId != ITEMCLSID_NONE)
					m_pPreviewDlg->AttachParts(nAttachItemId);
				m_fPreviewChangePartsTimer = 0.f;
			}
#else
			m_fPreviewChangePartsTimer += fElapsedTime;
			if (m_fPreviewChangePartsTimer >= m_fPreviewChangePartsTermSec)
			{
				ITEMCLSID nAttachItemId = DrawRandomItemIdFromPreviewItemList();
				if (nAttachItemId != ITEMCLSID_NONE)
					m_pPreviewDlg->AttachParts(nAttachItemId);
				m_fPreviewChangePartsTimer = 0.f;
			}
#endif
		}

		if (m_bInDrawProgress)
		{
			if (OnDrawProgress(fElapsedTime) == false)
				EndDrawProgress();
		}

		if (m_fCompleteTimer > 0.f)
		{
			m_fCompleteTimer -= fElapsedTime;
			if (m_fCompleteTimer <= 0.f)
			{
				CEtSoundEngine::GetInstance().PlaySound__( "2D", m_CompleteMixSound );

				if (m_pCompleteDlg)
				{
					m_pCompleteDlg->SetInfo(m_CurrentResultItemId, 0);
					m_pCompleteDlg->Show(true);

					StopPreviewDemo();

					if (m_pPreviewDlg)
						m_pPreviewDlg->AttachParts(m_CurrentResultItemId);
				}
				else
				{
					HandleError(eECRMIX_GENERAL);
				}
				m_fCompleteTimer = 0.f;
			}
		}
	}

	CDnCustomDlg::Process(fElapsedTime);
}

bool CDnCostumeRandomMixDlg::OnDrawProgress(float fElapsedTime)
{
	if (m_fTimer < m_fMaxTimerSec)
	{
		m_fTimer += fElapsedTime;
		if (m_fTimer >= m_fMaxTimerSec)
		{
			return false;
		}
		else
		{
			float fCurrentProgress = (100.f / ((m_fMaxTimerSec == 0) ? 1.f : m_fMaxTimerSec)) * m_fTimer;
			CommonUtil::ClipNumber(fCurrentProgress, 0.f, 100.f);
			m_pProgressBarTime->SetProgress(fCurrentProgress);

			if (m_fTimerOffset == 0 || m_fTimerOffset >= m_fMaxTimerOffsetSec)
			{
				m_fTimerOffset = 0.f;

				ITEMCLSID nItemIdDrew = DrawRandomItemIdFromPreviewItemList();
				if (nItemIdDrew != ITEMCLSID_NONE)
					SetResultSlot(nItemIdDrew);
			}

			m_fTimerOffset += fElapsedTime;
		}
	}

	return true;
}

ITEMCLSID CDnCostumeRandomMixDlg::DrawRandomItemIdFromPreviewItemList()
{
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	int nMaxSize = (int)m_PreviewItemList.size();
	int nNumberDrew = m_Random.rand(0, nMaxSize - 1);

	return m_PreviewItemList[nNumberDrew].progressItem;
#else
	float fNumberDrew = m_Random.rand(0.f, 100.f);
	float fAccumulator = 0.f;
	std::vector<SRandomProgressData>::const_iterator iter = m_PreviewItemList.begin();
	for (; iter != m_PreviewItemList.end(); ++iter)
	{
		const SRandomProgressData& currentData = (*iter);
		fAccumulator += currentData.fShowRatio;
		if (fNumberDrew <= fAccumulator)
		{
			int nIndexDrew = m_Random.rand(0, int(currentData.progressItemList.size()) - 1);
			const int& nItemIdDrew = currentData.progressItemList[nIndexDrew];
			return nItemIdDrew;
		}
	}
#endif

	return ITEMCLSID_NONE;
}

void CDnCostumeRandomMixDlg::SetResultSlot(const ITEMCLSID& resultItemId)
{
	if (m_pPreviewItem == NULL)
	{
		TItemInfo itemInfo;
		if (CDnItem::MakeItemInfo( resultItemId, 1, itemInfo))
			m_pPreviewItem = GetItemTask().CreateItem(itemInfo);
		if (m_pResultSlot && m_pPreviewItem)
			m_pResultSlot->SetItem(m_pPreviewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	}
	else
	{
		DNTableFileFormat* pTable = GetDNTable( CDnTableDB::TITEM );
		int nIconIndex = -1;
		eItemRank itemRank = ITEMRANK_D;
		eItemTypeEnum itemType = ITEMTYPE_NORMAL;

		const DNTableCell* pIconImageField = pTable->GetFieldFromLablePtr( resultItemId, "_IconImageIndex" );
		if (pIconImageField)
			nIconIndex = pIconImageField->GetInteger();

		const DNTableCell* pRankField = pTable->GetFieldFromLablePtr( resultItemId, "_Rank" );
		if (pRankField)
			itemRank = (eItemRank)pRankField->GetInteger();

		const DNTableCell* pItmeTypeField = pTable->GetFieldFromLablePtr( resultItemId, "_Type" );
		if (pItmeTypeField)
			itemType = (eItemTypeEnum)pItmeTypeField->GetInteger();

		m_pPreviewItem->ChangeIconIndex(nIconIndex);
		m_pPreviewItem->ChangeRank(itemRank);
		m_pPreviewItem->ChangeItemType(itemType);

		if (m_pResultSlot)
			m_pResultSlot->SetItem(m_pPreviewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	}

	m_CurrentResultItemId = resultItemId;

	if (m_pExpectItemName)
	{
		tstring szName = CDnItem::GetItemFullName(resultItemId);
		m_pExpectItemName->SetText(szName.c_str());
		m_pExpectItemName->Show(true);
	}
}

void CDnCostumeRandomMixDlg::EndDrawProgress()
{
	m_pProgressBarTime->Enable(false);
	m_pProgressBarText->Show(false);

	DoMix();

	m_bInDrawProgress = false;
}

void CDnCostumeRandomMixDlg::OnMixStartOrEnd(bool bStart)
{
	int i = 0;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		m_pStuffSlots[i]->Enable(!bStart);
	}

	m_pMixBtn->Enable(!bStart);
	m_pCancelBtn->Enable(!bStart);
	m_pCloseBtn->Enable(!bStart);

	if (IsShow())
		SetDummyModalDlg(bStart);
}

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
void CDnCostumeRandomMixDlg::StartMixProgress()
{
	OnMixStartOrEnd(true);

	bool bRet = StartDrawProgress();
	if (bRet == false)
		OnMixStartOrEnd(false);
}

void CDnCostumeRandomMixDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case eCONFIRMDLG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if (IsCmdControl("ID_YES"))
					StartMixProgress();
			}
		}
		break;
	}
}
#endif

void CDnCostumeRandomMixDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_BUYGACHA"))
		{
			if (CheckMix())
			{
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
				bool bStartConfirm = true;
				if (CDnItemTask::IsActive())
				{
					const CDnCostumeRandomMixDataMgr& mgr = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr();
					CostumeMix::RandomMix::eOpenType openType = mgr.GetCurrentOpenType();
					if (openType == CostumeMix::RandomMix::OpenByNpc)
					{
						GetInterface().MessageBox(130341, MB_YESNO, eCONFIRMDLG, this); // UISTRING : 합성 중에는 취소할 수 없으며, 수수료는 환불되지 않습니다. 정말 합성 하시겠습니까?
						bStartConfirm = false;
					}
				}

				if (bStartConfirm)
					StartMixProgress();
#else
				OnMixStartOrEnd(true);

				bool bRet = StartDrawProgress();
				if (bRet == false)
					OnMixStartOrEnd(false);
#endif
			}
			return;
		}
		else if (IsCmdControl("ID_BUTTON_CANCEL") || IsCmdControl("ID_BUTTON_CLOSE"))
		{
			if (CDnItemTask::IsActive())
				CDnItemTask::GetInstance().RequestItemCostumeRandomMixClose();
			return;
		}
	}

	if (strstr(pControl->GetControlName(), "ID_ITEME_RANDOMOPTION"))
	{
		CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
		CDnItemSlotButton *pPressedButton = static_cast<CDnItemSlotButton*>(pControl);

		if (uMsg == WM_RBUTTONUP)
		{
			if (ReleaseStuffItemSlotBtn(pPressedButton) == false)
			{
				HandleError(eECRMIX_GENERAL);
				return;
			}
		}

		if (pDragButton)
		{
			if (pDragButton != pPressedButton)
			{
				if (AddAttachItem(pDragButton, pPressedButton, CDnSlotButton::ITEM_ORIGINAL_COUNT) == false)
					drag::Command(UI_DRAG_CMD_CANCEL);
#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
				EnableSameItemFromCashInven( pPressedButton );
#endif 
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

				CEtSoundEngine::GetInstance().PlaySound__("2D", pPressedBtnItem->GetDragSoundIndex());
			}
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

bool CDnCostumeRandomMixDlg::StartDrawProgress()
{
	m_Random.srand(timeGetTime());
	m_PreviewItemList.clear();

	if (m_PreviewItemList.empty())
	{
		if (MakePreviewItemList() == false)
			HandleErrorOnMakePreviewItemList();

		if (m_PreviewItemList.empty())
		{
			HandleError(eECRMIX_GENERAL);
			m_bInDrawProgress = false;
			return false;
		}
	}

	m_pProgressBarText->Show(true);

	SetDrawTimer();

	m_bInDrawProgress = true;

	return true;
}

bool CDnCostumeRandomMixDlg::CheckMix()
{
	if (CDnItemTask::IsActive() && CDnItemTask::GetInstance().IsLockCosRandMixSendPacket())
	{
		return false;
	}

	if (GetStuffCountInSlot() <= 0)
	{
		HandleError(eECRMIX_STUFFCOUNT_NONE);
		return false;
	}

	if (GetStuffCountInSlot() != MAX_COSTUME_RANDOMMIX_STUFF)
	{
		HandleError(eECRMIX_STUFFCOUNT_SHORT);
		return false;
	}

	//	STUFF ITEM PROCESS
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	std::vector<int> stuffItemIds;
#endif

	int i = 0;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlots[i];
		if (pButton == NULL || pButton->GetItem() == NULL)
		{
			HandleError(eECRMIX_STUFFCOUNT_SHORT);
			return false;
		}
		else
		{
			const CDnItem* pStuffItem = static_cast<const CDnItem*>(pButton->GetItem());
			if (pStuffItem == NULL)
			{
				HandleError(eECRMIX_STUFFCOUNT_SHORT);
				return false;
			}

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
			stuffItemIds.push_back(pStuffItem->GetClassID());
#endif
		}
	}

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	const CDnCostumeRandomMixDataMgr& mgr = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr();	
	if (mgr.GetCurrentOpenType() == CostumeMix::RandomMix::OpenByNpc)
	{
		if (stuffItemIds.size() != MAX_COSTUME_RANDOMMIX_STUFF)
		{
			HandleError(eECRMIX_CANT_DMIX_STUFF);
			return false;
		}

		INT64 fee = mgr.GetRandomMixFee(stuffItemIds);
		if (CDnItemTask::GetInstance().GetCoin() < fee)
		{
			HandleError(eECRMIX_FEE_INSUFFICIENT);
			return false;
		}
	}
#endif
	return true;
}

void CDnCostumeRandomMixDlg::DoMix()
{
	if (CheckMix() == false)
		return;

	//	STUFF ITEM PROCESS
	INT64 stuffSerials[MAXCOSMIXSTUFF];
	int i = 0;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		const CDnItemSlotButton* pButton = m_pStuffSlots[i];
		const CDnItem* pStuffItem = static_cast<const CDnItem*>(pButton->GetItem());
		if (pStuffItem)
			stuffSerials[i] = pStuffItem->GetSerialID();
	}

	if (CDnItemTask::IsActive())
	{
		ClearStuffSlot();

		CDnItemTask::GetInstance().RequestItemCostumeRandomMix(stuffSerials);
	}
}

void CDnCostumeRandomMixDlg::SetResultFromServer(const ITEMCLSID& resultItemId)
{
	SetResultSlot(resultItemId);
}

bool CDnCostumeRandomMixDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CDnCustomDlg::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnCostumeRandomMixDlg::DisableAllDlgs(bool bEnable, const std::wstring& text)
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

void CDnCostumeRandomMixDlg::ClearStuffSlot()
{
	int i = 0;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		CDnItemSlotButton* pBtn = m_pStuffSlots[i];
		if (pBtn)
			pBtn->ResetSlot();
	}

	ClearInvenSlotCache();
}

void CDnCostumeRandomMixDlg::ClearInvenSlotCache()
{
	int i = 0;
	for(; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		CDnItemSlotButton* pBtn = m_pStuffSlots[i];
		if (pBtn)
		{
			pBtn->SetRegist(false);
			pBtn->DisableSplitMode(true);
			pBtn = NULL;
		}
	}
}

void CDnCostumeRandomMixDlg::SetDrawTimer()
{
	m_fMaxTimerSec = 10.f;
	m_fMaxTimerOffsetSec = 0.03f;
	m_fTimer = 0.f;
	m_fTimerOffset = 0.f;
}

void CDnCostumeRandomMixDlg::HandleErrorOnMakePreviewItemList() const
{
	HandleError(eECRMIX_NO_ITEMLIST);
}

bool CDnCostumeRandomMixDlg::IsEnableClose() const
{
	return (m_pCancelBtn->IsEnable());
}

bool CDnCostumeRandomMixDlg::SetDummyModalDlg(bool bSet)
{
	if (m_pDummyModalDlg)
		m_pDummyModalDlg->Show(bSet);

	return true;
}
#endif // PRE_ADD_COSRANDMIX