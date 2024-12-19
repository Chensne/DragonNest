#include "StdAfx.h"

#include "DnItemDisjointDlg.h"
#include "DnItemDisjointInfoDlg.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnLocalPlayerActor.h"
#include "DnNPCActor.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#endif
#include "EtUIListBoxEx.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnItemDisjointDlg::CDnItemDisjointDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
	, m_pStaticGold(NULL)
	, m_pStaticSilver(NULL)
	, m_pStaticBronze(NULL)
	, m_pButtonDisjoint(NULL)
	, m_pButtonCancel(NULL)
	, m_dwColorGold(0)
	, m_dwColorSilver(0)
	, m_dwColorBronze(0)
	, m_pResultListBox(NULL)
{
}

CDnItemDisjointDlg::~CDnItemDisjointDlg(void)
{
	SAFE_DELETE_PVEC(m_vecItem);
}

void CDnItemDisjointDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("ItemDisjointDlg.ui").c_str(), bShow);
}

void CDnItemDisjointDlg::InitialUpdate()
{
	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD");
	m_pStaticSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pStaticBronze = GetControl<CEtUIStatic>("ID_BRONZE");
	m_dwColorGold = m_pStaticGold->GetTextColor();
	m_dwColorSilver = m_pStaticSilver->GetTextColor();
	m_dwColorBronze = m_pStaticBronze->GetTextColor();

	m_pButtonDisjoint = GetControl<CEtUIButton>("ID_BUTTON_DISJOINT");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");

	m_pResultListBox = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");

	// StoreSlotDlg와 PrivateMarketDlg를 보면 같은 CDnItemSlotButton인데도 다르게 초기화한다.
	// 전자는 컨트롤 생성 후 개별적으로 InitCustomControl함수안에서 vec에 넣는 구조고,
	// 후자는 컨트롤 생성 후 InitCustomControl함수에선 아무일 안하다가 Dialog의 InitialUpdate에서 한번에 vec에 넣는 구조다.
	// 여기서는 후자를 따르기로 한다.
	//
	char szTemp[32] = { 0 };
	for (int i = 0; i<ITEM_DISJOINT_ITEMSLOT_MAX; i++)
	{
		sprintf_s(szTemp, 32, "ID_ITEM%d", i);
		CDnItemSlotButton *pItemSlotButton = GetControl<CDnItemSlotButton>(szTemp);
		pItemSlotButton->SetSlotIndex(i);
		pItemSlotButton->SetSlotType(ST_ITEM_DISJOINT);
		m_vecSlotButton.push_back(pItemSlotButton);
	}

	m_vecItem.resize(ITEM_DISJOINT_ITEMSLOT_MAX);
	m_vecQuickSlotButton.resize(ITEM_DISJOINT_ITEMSLOT_MAX);
}

void CDnItemDisjointDlg::Clear(bool bAll)
{
	if (bAll)
	{
		m_pResultListBox->RemoveAllItems();
		//m_pItemCacheList.clear();
	}

}

void CDnItemDisjointDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	if (bShow)
	{
		m_pStaticGold->SetIntToText(0);
		m_pStaticSilver->SetIntToText(0);
		m_pStaticBronze->SetIntToText(0);
		m_pStaticGold->SetTextColor(m_dwColorGold);
		m_pStaticSilver->SetTextColor(m_dwColorSilver);
		m_pStaticBronze->SetTextColor(m_dwColorBronze);

		m_pButtonDisjoint->Enable(false);
		m_pButtonCancel->Enable(true);

		for (int i = 0; i<ITEM_DISJOINT_ITEMSLOT_MAX; i++)
		{
			SAFE_DELETE(m_vecItem[i]);
			m_vecSlotButton[i]->ResetSlot();
		}

		// 분해 창이 보일 때 Activate 액션 실행
		DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
		DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);
		if (hNpc)
		{
			hNpc->SetActionQueue("Activate");
		}


		this->Clear(true);
		this->Clear(false);
	}
	else
	{
		// 창이 종료될때 퀵슬롯 색상처리한거 되돌려둔다.
		for (int i = 0; i<ITEM_DISJOINT_ITEMSLOT_MAX; i++)
		{
			if (m_vecQuickSlotButton[i])
			{
				m_vecQuickSlotButton[i]->SetRegist(false);
				m_vecQuickSlotButton[i]->DisableSplitMode(true);
				m_vecQuickSlotButton[i] = NULL;
			}
		}

		// 분해 창이 닫힐 때 Disjoint_01 액션 실행
		DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
		DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);
		if (hNpc)
		{
			hNpc->SetActionQueue("Disjoint_01");
		}


		GetInterface().CloseBlind();
	}

	CEtUIDialog::Show(bShow);
}

bool CDnItemDisjointDlg::IsDisjoitableItem(CDnSlotButton* pDragButton, CDnItem* pItem) const
{
	if (pDragButton->GetSlotType() != ST_INVENTORY)
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3610), MB_OK);
		return false;
	}

	// 올린거 또 올리는거면 패스
	if (CheckItemDuplication(pItem))
		return false;

	// 아이템 종류 검사
	if (pItem->GetItemType() == ITEMTYPE_QUEST)
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3609), MB_OK);
		return false;
	}

	// 분해가 안되는 아이템인지 검사.
	if (!pItem->CanDisjoint())
	{
		GetInterface().MessageBox(1740, MB_OK);
		return false;
	}

	int nDisjointLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Disjoint_ItemLimitLevel);
	int nDisjointUserLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Disjoint_LimitUserLevel);

	if (pItem->GetLevelLimit() >= nDisjointLevelLimit && nDisjointUserLevelLimit > CDnActor::s_hLocalActor->GetLevel())
	{
		GetInterface().MessageBox(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8145), pItem->GetLevelLimit(), pItem->GetLevelLimit()).c_str(), MB_OK);
		return false;
	}

	// 귀속아이템이면서 봉인상태의 아이템인지 확인
	if (pItem->GetReversion() > CDnItem::NoReversion && !pItem->IsSoulbBound())
	{
		GetInterface().MessageBox(1750, MB_OK);
		return false;
	}

	// 분해 타입 체크
	DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
	DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);
	if (hNpc) {
		// 갯수에 따라 NPC 연출 다르게 한 후
		CDnNPCActor *pNPC = dynamic_cast<CDnNPCActor*>(hNpc.GetPointer());

		if (!pItem->CanDisjointType(pNPC->GetNpcData().nParam[0])) {
			GetInterface().MessageBox(8100, MB_OK);
			return false;
		}
	}

	return true;
}

void CDnItemDisjointDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_DISJOINT"))
		{
			// 등록되어있는 갯수를 세서
			//
			int nCount = 0;
			CDnItemSlotButton *pButton(NULL);
			DWORD dwVecSize = (DWORD)m_vecSlotButton.size();
			for (DWORD i = 0; i<dwVecSize; i++)
			{
				pButton = m_vecSlotButton[i];
				if (!pButton) continue;
				if (!pButton->IsEmptySlot())
					++nCount;
			}

			// NPC 얻어온 후
			DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
			DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);
			if (hNpc)
			{
				// 갯수에 따라 NPC 연출 다르게 한 후
				CDnNPCActor *pNPC = dynamic_cast<CDnNPCActor*>(hNpc.GetPointer());
				char szActionName[16];
				sprintf_s(szActionName, sizeof(szActionName), "disjoint_0%d", nCount);
				pNPC->SetActionQueue(szActionName);

				// 플레이타임을 구해놓는다.
				//float fFPS = hNpc->CDnActionBase::GetFPS();
				//CEtActionBase::ActionElementStruct* pActionInfo = hNpc->GetElement( szActionName );
				//float fActionTimeLength = (float)pActionInfo->dwLength / fFPS;
			}

			// 인벤토리 닫는다.
			ProcessCommand(EVENT_BUTTON_CLICKED, false, GetControl("ID_CLOSE_DIALOG"), 0);
#ifdef _ADD_NEWDISJOINT
			// 해당 아이템들의 분해를 요청
			CSItemDisjointReqNew Req;
			ZeroMemory(&Req, sizeof(CSItemDisjointReqNew));

			int nSuccessCount = 0;
			for (DWORD i = 0; i<dwVecSize; i++)
			{
				pButton = m_vecSlotButton[i];
				if (!pButton) continue;

				if (!pButton->IsEmptySlot())
				{
					CDnItem *pItem = static_cast<CDnItem *>(pButton->GetItem());
					if (NULL == pItem)
						continue;

					Req.cSlotIndex[i] = pItem->GetSlotIndex();
					Req.biItemSerial[i] = pItem->GetSerialID();
					Req.nCount[i] = pItem->GetOverlapCount();
					Req.nNpcUniqueID = nUID;
					nSuccessCount++;
				}
			}
			if (nSuccessCount > 0)
				GetItemTask().RequestItemDisjointNew(Req);
#else
			// 해당 아이템들의 분해를 요청
			for (DWORD i = 0; i<dwVecSize; i++)
			{
				pButton = m_vecSlotButton[i];
				if (!pButton) continue;

				if (!pButton->IsEmptySlot())
				{
					CDnItem *pItem = static_cast<CDnItem *>(pButton->GetItem());
					GetItemTask().RequestItemDisjoint(pItem, nUID);
				}
			}
#endif

			// 위 절차는 아직 기획이 정립되지 않아서 그냥 임시적으로 처리하는 것이다.
			// 제대로 하려면 요청과 동시에 서버에서 템 삭제 및 수수료 처리까지 완료하고나서,
			// NPC애니메이션이 끝난 후 템을 뱉어내야하는데,
			// 이렇게 할 경우 파티원끼리의 NPC동기화 등 많은 부분에 대한 예외처리가 더 필요해서
			// 그냥 어렵게 처리하지 말고,
			// 분해 눌렀을때 바로 자신한테만 연출 보임과 동시에 아이템이 나오도록 하기로 했다.
			//
			// 나중에 바뀌게되면 다 뜯어야할거다.

			return;
		}
		else if (IsCmdControl("ID_BUTTON_CANCEL"))
		{
			ProcessCommand(EVENT_BUTTON_CLICKED, false, GetControl("ID_CLOSE_DIALOG"), 0);
			return;
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if (IsCmdControl("ID_BT_SMALLHELP"))
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if (pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_ITEM_DISJOINT);
		}
#endif

		if (strstr(pControl->GetControlName(), "ID_ITEM"))
		{
			CDnSlotButton *pDragButton = (CDnSlotButton *)drag::GetControl();
			CDnSlotButton *pPressedButton = (CDnItemSlotButton *)pControl;

			// 우클릭으로 빼기
			if (uMsg == WM_RBUTTONUP)
			{
				SAFE_DELETE(m_vecItem[pPressedButton->GetSlotIndex()]);
				pPressedButton->ResetSlot();
				if (m_vecQuickSlotButton[pPressedButton->GetSlotIndex()])
				{
					m_vecQuickSlotButton[pPressedButton->GetSlotIndex()]->SetRegist(false);
					m_vecQuickSlotButton[pPressedButton->GetSlotIndex()]->DisableSplitMode(true);
					m_vecQuickSlotButton[pPressedButton->GetSlotIndex()] = NULL;
				}
				CheckItemList();
				//rlkt_test
				this->Clear(true);
				for (int i = 0; i<4; i++)
				{
					if (m_vecItem[i])
						if (m_vecItem[i]->GetClassID() > 0)
							SetItemInfo(m_vecItem[i]->GetDisjointItemDrop(), (int)m_vecItem[i]->GetEnchantLevel());
				}
				return;
			}

			if (pDragButton)
			{
				drag::ReleaseControl();

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if (pItem == NULL || IsDisjoitableItem(pDragButton, pItem) == false)
				{
					pDragButton->DisableSplitMode(true);
					return;
				}

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());

				// 만약 이미 올려진 자리에 다른 아이템을 올리는 거라면,
				if (pPressedItem)
				{
					// 해당 아이템을 창에서 제거한다.
					SAFE_DELETE(m_vecItem[pPressedButton->GetSlotIndex()]);
					pPressedButton->ResetSlot();
					m_vecQuickSlotButton[pPressedButton->GetSlotIndex()]->SetRegist(false);
					m_vecQuickSlotButton[pPressedButton->GetSlotIndex()]->DisableSplitMode(true);
					m_vecQuickSlotButton[pPressedButton->GetSlotIndex()] = NULL;
				}

				// 분해할 아이템에 등록할때 전처럼 이렇게 인벤토리 아이템의 포인터를 직접 넘기지 않고,
				//pPressedButton->SetItem( pItem );
				// 이렇게 임시템을 만들어서 넣어준다.
				TItemInfo itemInfo;
				pItem->GetTItemInfo(itemInfo);
				CDnItem *pNewItem = GetItemTask().CreateItem(itemInfo);
				m_vecItem[pPressedButton->GetSlotIndex()] = pNewItem;

				pPressedButton->SetItem(pNewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

				CheckItemList();
				CEtSoundEngine::GetInstance().PlaySound("2D", pItem->GetDragSoundIndex());

				m_vecQuickSlotButton[pPressedButton->GetSlotIndex()] = (CDnQuickSlotButton *)pDragButton;
				m_vecQuickSlotButton[pPressedButton->GetSlotIndex()]->SetRegist(true);
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

bool CDnItemDisjointDlg::CheckItemDuplication(CDnItem *pItem) const
{
	for (int i = 0; i<ITEM_DISJOINT_ITEMSLOT_MAX; i++)
	{
		if (m_vecQuickSlotButton[i] && m_vecQuickSlotButton[i]->GetItem() == pItem)
			return true;
	}
	return false;
}

void CDnItemDisjointDlg::AddItemToList(CDnQuickSlotButton *pPressedButton)
{
	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());

	// 교환창에 공간이 있는지 검사
	if (!IsEmptySlot())
	{
		GetInterface().MessageBox(3621, MB_OK);
		return;
	}

	// 올린거 또 올리는거면 패스
	if (CheckItemDuplication(pItem))
		return;

	// 아이템 종류 검사
	if (pItem->GetItemType() == ITEMTYPE_QUEST)
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3609), MB_OK);
		return;
	}

	// 분해가 안되는 아이템인지 검사.
	if (!pItem->CanDisjoint())
	{
		GetInterface().MessageBox(1740, MB_OK);
		return;
	}

	int nDisjointLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Disjoint_ItemLimitLevel);
	int nDisjointUserLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Disjoint_LimitUserLevel);

	if (pItem->GetLevelLimit() >= nDisjointLevelLimit && nDisjointUserLevelLimit > CDnActor::s_hLocalActor->GetLevel())
	{
		GetInterface().MessageBox(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8145), pItem->GetLevelLimit(), pItem->GetLevelLimit()).c_str(), MB_OK);
		return;
	}

	// 귀속아이템이면서 봉인상태의 아이템인지 확인
	if (pItem->GetReversion() > CDnItem::NoReversion && !pItem->IsSoulbBound())
	{
		GetInterface().MessageBox(1750, MB_OK);
		return;
	}

	// 분해 타입 체크
	DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
	DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);
	if (hNpc) {
		// 갯수에 따라 NPC 연출 다르게 한 후
		CDnNPCActor *pNPC = dynamic_cast<CDnNPCActor*>(hNpc.GetPointer());

		if (!pItem->CanDisjointType(pNPC->GetNpcData().nParam[0])) {
			GetInterface().MessageBox(8100, MB_OK);
			return;
		}
	}

	// 비어있는 슬롯을 얻어
	CDnSlotButton *pEmptyButton = m_vecSlotButton[GetEmptySlot()];

	// 분해할 아이템에 등록할때 전처럼 이렇게 인벤토리 아이템의 포인터를 직접 넘기지 않고,
	//pEmptyButton->SetItem( pItem );
	// 이렇게 임시템을 만들어서 넣어준다.
	TItemInfo itemInfo;
	pItem->GetTItemInfo(itemInfo);
	CDnItem *pNewItem = GetItemTask().CreateItem(itemInfo);
	m_vecItem[GetEmptySlot()] = pNewItem;

	pEmptyButton->SetItem(pNewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

	CheckItemList();
	CEtSoundEngine::GetInstance().PlaySound("2D", pItem->GetDragSoundIndex());

	m_vecQuickSlotButton[pEmptyButton->GetSlotIndex()] = pPressedButton;
	m_vecQuickSlotButton[pEmptyButton->GetSlotIndex()]->SetRegist(true);

	//rlkt_test
	this->Clear(true);
	for (int i = 0; i<4; i++)
	{
		if (m_vecItem[i])
			if (m_vecItem[i]->GetClassID() > 0)
				SetItemInfo(m_vecItem[i]->GetDisjointItemDrop(), (int)m_vecItem[i]->GetEnchantLevel());
	}
}

bool CDnItemDisjointDlg::IsEmptySlot()
{
	if (GetEmptySlot() == -1)
	{
		return false;
	}

	return true;
}

int CDnItemDisjointDlg::GetEmptySlot()
{
	CDnItemSlotButton *pButton(NULL);

	DWORD dwVecSize = (DWORD)m_vecSlotButton.size();
	for (DWORD i = 0; i<dwVecSize; i++)
	{
		pButton = m_vecSlotButton[i];
		if (!pButton) continue;

		if (pButton->IsEmptySlot())
			return pButton->GetSlotIndex();
	}

	return -1;
}

void CDnItemDisjointDlg::CheckItemList() const
{
	int nPrice = 0;
	bool bListEmpty = true;

	// 먼저 슬롯에 아무것도 등록이 안되있는지 확인
	CDnItemSlotButton *pButton(NULL);
	DWORD dwVecSize = (DWORD)m_vecSlotButton.size();
	for (DWORD i = 0; i<dwVecSize; i++)
	{
		pButton = m_vecSlotButton[i];
		if (!pButton) continue;

		if (!pButton->IsEmptySlot())
		{
			CDnItem *pItem = static_cast<CDnItem *>(pButton->GetItem());
			nPrice += pItem->GetDisjointCost() * pItem->GetOverlapCount();
			bListEmpty = false;
		}
	}

	if (bListEmpty == true)
	{
		// 합산 가격으로 판단하지 않는다.
		m_pButtonDisjoint->Enable(false);
	}
	else
	{
		// 가격이 모자라도 비활성화다.
		if (nPrice > GetItemTask().GetCoin())
			m_pButtonDisjoint->Enable(false);
		else
			m_pButtonDisjoint->Enable(true);
	}

	int nGold = nPrice / 10000;
	int nSilver = (nPrice % 10000) / 100;
	int nBronze = nPrice % 100;

	m_pStaticGold->SetIntToText(nGold);
	m_pStaticSilver->SetIntToText(nSilver);
	m_pStaticBronze->SetIntToText(nBronze);
	if (nPrice > GetItemTask().GetCoin())
	{
		if (nPrice >= 10000) m_pStaticGold->SetTextColor(0xFFFF0000, true);
		if (nPrice >= 100) m_pStaticSilver->SetTextColor(0xFFFF0000, true);
		m_pStaticBronze->SetTextColor(0xFFFF0000, true);
	}
	else
	{
		m_pStaticGold->SetTextColor(m_dwColorGold);
		m_pStaticSilver->SetTextColor(m_dwColorSilver);
		m_pStaticBronze->SetTextColor(m_dwColorBronze);
	}
}
void CDnItemDisjointDlg::AddDropResultToList(int nItemID, int nItemLevel, bool bRefreshList)
{
	if (bRefreshList)
		this->Clear(true);

	if (m_pResultListBox->GetElementCount() == 0)
		return;
}


void CDnItemDisjointDlg::SetItemInfo(int nItemID, int nItemLevel)
{
	//this->Clear(true);

	std::vector<int> vecDisjointItems = CDisjointInfoTable::GetInstance().GetValue(nItemID, nItemLevel);

	if (vecDisjointItems.size() == 0)
		return;

	for (int i = 0; i<vecDisjointItems.size(); i++)
	{
		if (vecDisjointItems[i] == 0 || vecDisjointItems[i] < 0)
			continue;

		//	for (int j = 0; j < m_pItemCacheList.size(); j++)
		//		if (m_pItemCacheList[j] == vecDisjointItems[i])
		//			goto end;

		m_pItemCacheList.push_back(vecDisjointItems[i]);
		CDnItemDisjointInfoDlg* pItemInfoDlg = m_pResultListBox->AddItem<CDnItemDisjointInfoDlg>();
		if (pItemInfoDlg)
			pItemInfoDlg->SetInfo(vecDisjointItems[i]);
		//end:;
	}
}

void CDnItemDisjointDlg::RemoveItemFromCache(int nItemID, int nItemLevel)
{

}