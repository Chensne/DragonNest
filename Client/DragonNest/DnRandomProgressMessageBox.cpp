#include "stdafx.h"
#include "DnRandomProgressMessageBox.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnItemTask.h"

#ifdef PRE_ADD_COSRANDMIX

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnRandomProgressMessageBox::CDnRandomProgressMessageBox(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
		: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pCancelButton = NULL;
	m_pProgressBarTime = NULL;
	m_pItemSlot = NULL;
	m_pItemNameText = NULL;
	m_pShowItem = NULL;

	m_fMaxTimerSec = 0.f;
	m_fTimerOffset = 0.f;
	m_fMaxTimerOffsetSec = 0.5f;
	m_fTimer = 0.f;
	m_nSoundIndex = -1;
	m_bOnlyProgressBar = false;
}

CDnRandomProgressMessageBox::~CDnRandomProgressMessageBox()
{
	ClearItemCache();
}

void CDnRandomProgressMessageBox::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "CharmItemOpen.ui" ).c_str(), bShow);
}

void CDnRandomProgressMessageBox::InitialUpdate()
{
	m_pCancelButton = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
	m_pCancelButton->Show(false);
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");

	m_pItemSlot = GetControl<CDnItemSlotButton>("ID_BT_ITEM");
	m_pItemNameText = GetControl<CEtUIStatic>("ID_TEXT_ITEMNAME");

	if (m_pItemSlot)
		m_pItemSlot->SetShowTooltip(false);

	m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( "UI_CharmItemOpen_Loop.ogg", false, false );
}

void CDnRandomProgressMessageBox::Process(float fElapsedTime)
{
	CDnCustomDlg::Process(fElapsedTime);

	if (IsShow() && (m_fTimer < m_fMaxTimerSec))
	{
		m_fTimer += fElapsedTime;
		if (m_fTimer >= m_fMaxTimerSec)
		{
			OnEndProcess();
		}
		else
		{
			m_pProgressBarTime->SetProgress( 100.f - ( 100.f / m_fMaxTimerSec * m_fTimer ) );

			if (m_fTimerOffset == 0 || m_fTimerOffset >= m_fMaxTimerOffsetSec)
			{
				m_fMaxTimerOffsetSec += 0.001f;
				m_fTimerOffset = 0.f;

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
				int nShowItemSize = (int)m_ShowItemList.size();
				int nNumberDrew = m_Random.rand(0, nShowItemSize - 1);
				const int& nItemIdDrew = m_ShowItemList[nNumberDrew].progressItem;
				if (m_pShowItem == NULL)
				{
					TItemInfo itemInfo;
					if (CDnItem::MakeItemInfo( nItemIdDrew, 1, itemInfo))
						m_pShowItem = GetItemTask().CreateItem(itemInfo);
					if (m_pItemSlot && m_pShowItem)
						m_pItemSlot->SetItem(m_pShowItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
				}
				else
				{
					DNTableFileFormat* pTable = GetDNTable( CDnTableDB::TITEM );
					int nIconIndex = -1;
					eItemRank itemRank = ITEMRANK_D;
					eItemTypeEnum itemType = ITEMTYPE_NORMAL;

					const DNTableCell* pIconImageField = pTable->GetFieldFromLablePtr( nItemIdDrew, "_IconImageIndex" );
					if (pIconImageField)
						nIconIndex = pIconImageField->GetInteger();

					const DNTableCell* pRankField = pTable->GetFieldFromLablePtr( nItemIdDrew, "_Rank" );
					if (pRankField)
						itemRank = (eItemRank)pRankField->GetInteger();

					const DNTableCell* pItmeTypeField = pTable->GetFieldFromLablePtr( nItemIdDrew, "_Type" );
					if (pItmeTypeField)
						itemType = (eItemTypeEnum)pItmeTypeField->GetInteger();

					m_pShowItem->ChangeIconIndex(nIconIndex);
					m_pShowItem->ChangeRank(itemRank);
					m_pShowItem->ChangeItemType(itemType);

					if (m_pItemSlot)
						m_pItemSlot->SetItem(m_pShowItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
				}

				if (m_pItemNameText)
				{
					tstring szName = CDnItem::GetItemFullName(nItemIdDrew);
					m_pItemNameText->SetText(szName.c_str());
				}
#else
							float fNumberDrew = m_Random.rand(0.f, 100.f);
							float fAccumulator = 0.f;
							std::vector<SRandomProgressData>::const_iterator iter = m_ShowItemList.begin();
							for (; iter != m_ShowItemList.end(); ++iter)
							{
								const SRandomProgressData& currentData = (*iter);
								fAccumulator += currentData.fShowRatio;
								if (fNumberDrew <= fAccumulator)
								{
									int nIndexDrew = m_Random.rand(0, int(currentData.progressItemList.size()) - 1);
									const int& nItemIdDrew = currentData.progressItemList[nIndexDrew];

									if (m_pShowItem == NULL)
									{
										TItemInfo itemInfo;
										if (CDnItem::MakeItemInfo( nItemIdDrew, 1, itemInfo))
											m_pShowItem = GetItemTask().CreateItem(itemInfo);
										if (m_pItemSlot && m_pShowItem)
											m_pItemSlot->SetItem(m_pShowItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
									}
									else
									{
										DNTableFileFormat* pTable = GetDNTable( CDnTableDB::TITEM );
										int nIconIndex = -1;
										eItemRank itemRank = ITEMRANK_D;
										eItemTypeEnum itemType = ITEMTYPE_NORMAL;

										const DNTableCell* pIconImageField = pTable->GetFieldFromLablePtr( nItemIdDrew, "_IconImageIndex" );
										if (pIconImageField)
											nIconIndex = pIconImageField->GetInteger();

										const DNTableCell* pRankField = pTable->GetFieldFromLablePtr( nItemIdDrew, "_Rank" );
										if (pRankField)
											itemRank = (eItemRank)pRankField->GetInteger();

										const DNTableCell* pItmeTypeField = pTable->GetFieldFromLablePtr( nItemIdDrew, "_Type" );
										if (pItmeTypeField)
											itemType = (eItemTypeEnum)pItmeTypeField->GetInteger();

										m_pShowItem->ChangeIconIndex(nIconIndex);
										m_pShowItem->ChangeRank(itemRank);
										m_pShowItem->ChangeItemType(itemType);

										if (m_pItemSlot)
											m_pItemSlot->SetItem(m_pShowItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
									}

									if (m_pItemNameText)
									{
										tstring szName = CDnItem::GetItemFullName(nItemIdDrew);
										m_pItemNameText->SetText(szName.c_str());
									}

									break;
								}
							}
#endif // PRE_ADD_COSRANDMIX_ACCESSORY
			}

			m_fTimerOffset += fElapsedTime;
		}
	}
}

void CDnRandomProgressMessageBox::OnEndProcess()
{
	if (m_pCallback)
		m_pCallback->OnUICallbackProc(GetDialogID(), EVENT_DIALOG_HIDE, NULL, RANDOM_MSGBOX_EVENT_END_PROGRESS);

	m_pProgressBarTime->Enable(false);

	if (m_hSound)
		m_hSound->GetChannel()->stop();
}

void CDnRandomProgressMessageBox::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnRandomProgressMessageBox::ClearItemCache()
{
	SAFE_DELETE(m_pShowItem);
}

void CDnRandomProgressMessageBox::Show(bool bShow)
{
	ClearItemCache();

	if (bShow)
	{
		if (m_nSoundIndex != -1)
			m_hSound = CEtSoundEngine::GetInstance().PlaySound__( "2D", m_nSoundIndex, true );
	}
	else
	{
		if (m_hSound)
			m_hSound->GetChannel()->stop();
	}
	CDnCustomDlg::Show(bShow);
}

bool CDnRandomProgressMessageBox::IsValidShowItemList() const
{
	if (m_ShowItemList.empty())
		return false;

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
#else
	float fSum = 0.f;
	std::vector<SRandomProgressData>::const_iterator iter = m_ShowItemList.begin();
	for (; iter != m_ShowItemList.end(); ++iter)
	{
		const SRandomProgressData& data = (*iter);
		if (data.progressItemList.empty())
			return false;

		fSum += data.fShowRatio;
	}

	if (fSum < 99.9f || fSum > 100.1f)
		return false;
#endif

	if (m_fMaxTimerSec <= 0.f)
		return false;

	return true;
}

void CDnRandomProgressMessageBox::MakeRandomMsgBoxType(CDnRandomProgressMessageBox::eRandomMsgBoxType type)
{
	if (type == RANDOM_MSGBOX_NORMAL)
	{
		m_fMaxTimerSec = 10.f;
		m_fMaxTimerOffsetSec = 0.02f;
		m_fTimer = 0.f;
		m_fTimerOffset = 0.f;
	}
	else
	{
		m_fMaxTimerSec = 0.f;
		m_fMaxTimerOffsetSec = 0.f;
		m_fTimer = 0.f;
		m_fTimerOffset = 0.f;
	}
}

void CDnRandomProgressMessageBox::HandleError(const std::wstring& msg)
{
	Show(false);
	GetInterface().MessageBox(msg.c_str(), MB_OK);
}
#endif // PRE_ADD_COSRANDMIX