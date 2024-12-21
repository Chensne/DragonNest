#include "StdAfx.h"
#include "DnStoreTabDlg.h"
#include "DnStoreListDlg.h"
#include "DnStoreSlotDlg.h"
#include "DnSlotButton.h"
#include "DnTradeTask.h"
#include "DnInterfaceDlgID.h"
#include "DnInterface.h"
#include "DnRepairConfirmEquipDlg.h"
#include "DnRepairConfirmAllDlg.h"
#include "DnItemTask.h"
#include "DnPartyTask.h"
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "DnQuestTask.h"
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "TaskManager.h"
#include "DnCommonUtil.h"
#include "DnTableDB.h"
#include "DnGuildWarTask.h"
#include "TradeSendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnStoreTabDlg::CDnStoreTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pTabButtonWeapon(NULL)
	, m_pTabButtonParts(NULL)
	, m_pTabButtonNormal(NULL)
	, m_pTabButtonBox(NULL)
	, m_pStoreWeaponDlg(NULL)		// Note : 무기
	, m_pStorePartsDlg(NULL)		// Note : 방어구
	, m_pStoreNormalDlg(NULL)		// Note : 소비
	, m_pStoreBoxDlg(NULL)			// Note : 상자
	, m_pButtonRepair(NULL)
	, m_pButtonAllRepair(NULL)
	, m_pRepairConfirmEquipDlg(NULL)
	, m_pRepairConfirmAllDlg(NULL)
	, m_pTabButton5(NULL)
	, m_pTabButton6(NULL)
	, m_pStaticMyPoint(NULL)
    , m_pStaticPointValue(NULL)
	, m_pStaticCover_A(NULL)
	, m_pStaticCover_B(NULL) 
	, m_pTabButton7(NULL)
	, m_pStore7Dlg(NULL)
	, m_pStaticRepurchaseBoard(NULL)
	, m_pStaticRepurchase(NULL)
	, m_pStore5Dlg(NULL)
	, m_pStore6Dlg(NULL)
#ifdef PRE_ADD_NEW_MONEY_SEED
	, m_pStaticSeedText( NULL )
	, m_pStaticSeedLine( NULL )
	, m_pStaticSeedBoard( NULL )
	, m_pStaticSeedPoint( NULL )
#endif // PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_ADD_PVPRANK_INFORM
	, m_pButtonPvPRankInfo(NULL)
#endif
{
	m_nStoreType = Shop::Type::Normal;
}

CDnStoreTabDlg::~CDnStoreTabDlg(void)
{
	SAFE_DELETE(m_pRepairConfirmEquipDlg);
	SAFE_DELETE(m_pRepairConfirmAllDlg);
}

void CDnStoreTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StoreTabDlg.ui" ).c_str(), bShow );

	if( CDnTradeTask::IsActive() )
	{
		GetTradeTask().GetTradeItem().SetStoreDialog( this );
	}
	else
	{
		CDebugSet::ToLogFile( "CDnMailDlg::Initialize, 상점 다이얼로그가 만들어 지기 전에 거래 테스크가 생성되어야 합니다." );
	}
}

void CDnStoreTabDlg::InitialUpdate()
{
	m_pButtonRepair = GetControl<CEtUIButton>("ID_BUTTON_REPAIR");
	m_pButtonAllRepair = GetControl<CEtUIButton>("ID_BUTTON_ALL_REPAIR");
	
	m_pTabButtonWeapon = GetControl<CEtUIRadioButton>("ID_TAB_WEAPON");
	m_pStoreWeaponDlg = new CDnStoreListDlg( UI_TYPE_CHILD, this );
	m_pStoreWeaponDlg->Initialize( false );
	AddTabDialog( m_pTabButtonWeapon, m_pStoreWeaponDlg );

	m_pTabButtonParts = GetControl<CEtUIRadioButton>("ID_TAB_PARTS");
	m_pStorePartsDlg = new CDnStoreListDlg( UI_TYPE_CHILD, this );
	m_pStorePartsDlg->Initialize( false );
	AddTabDialog( m_pTabButtonParts, m_pStorePartsDlg );
	
	m_pTabButtonNormal = GetControl<CEtUIRadioButton>("ID_TAB_NORMAL");
	m_pStoreNormalDlg = new CDnStoreListDlg( UI_TYPE_CHILD, this );
	m_pStoreNormalDlg->Initialize( false );
	AddTabDialog( m_pTabButtonNormal, m_pStoreNormalDlg );

	m_pTabButtonBox = GetControl<CEtUIRadioButton>("ID_TAB_BOX");
	m_pStoreBoxDlg = new CDnStoreListDlg( UI_TYPE_CHILD, this );
	m_pStoreBoxDlg->Initialize( false );
	AddTabDialog( m_pTabButtonBox, m_pStoreBoxDlg );

	m_pTabButton5 = GetControl<CEtUIRadioButton>("ID_TAB_5");
	m_pStore5Dlg = new CDnStoreListDlg( UI_TYPE_CHILD, this );
	m_pStore5Dlg->Initialize( false );
	AddTabDialog( m_pTabButton5, m_pStore5Dlg );

	m_pTabButton6 = GetControl<CEtUIRadioButton>("ID_TAB_6");
	m_pStore6Dlg = new CDnStoreListDlg( UI_TYPE_CHILD, this );
	m_pStore6Dlg->Initialize( false );
	AddTabDialog( m_pTabButton6, m_pStore6Dlg );

	m_pTabButton7 = GetControl<CEtUIRadioButton>("ID_TAB_7");
	m_pStore7Dlg = new CDnStoreListDlg( UI_TYPE_CHILD, this );
	m_pStore7Dlg->Initialize( false );
	AddTabDialog( m_pTabButton7, m_pStore7Dlg );

	m_pStaticRepurchaseBoard = GetControl<CEtUIStatic>("ID_STATIC_BOARD1");
	m_pStaticRepurchaseBoard->Show(false);

	m_pStaticRepurchase = GetControl<CEtUIStatic>("ID_TEXT1");
	int feeRate = int(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Shop_Repurchase_Fee) * 100.f) - 100;
	std::wstring txt = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1780), feeRate); // UISTRING : 재구매 시, 수수료 %d%%가 추가됩니다.
	m_pStaticRepurchase->SetText(txt.c_str());
	m_pStaticRepurchase->Show(false);

	m_pStaticMyPoint = GetControl<CEtUIStatic>("ID_TEXT");
	m_pStaticPointValue = GetControl<CEtUIStatic>("ID_TEXT_POINT");
	m_pStaticCover_A = GetControl<CEtUIStatic>("ID_STATIC_BOARD");
	m_pStaticCover_B = GetControl<CEtUIStatic>("ID_STATIC_LINE");

	m_pRepairConfirmEquipDlg = new CDnRepairConfirmEquipDlg( UI_TYPE_MODAL, NULL, REPAIR_CONFIRM_EQUIP_DIALOG, this );
	m_pRepairConfirmEquipDlg->Initialize( false );

	m_pRepairConfirmAllDlg = new CDnRepairConfirmAllDlg( UI_TYPE_MODAL, NULL, REPAIR_CONFIRM_ALL_DIALOG, this );
	m_pRepairConfirmAllDlg->Initialize( false );

#ifdef PRE_ADD_NEW_MONEY_SEED
	m_pStaticSeedText = GetControl<CEtUIStatic>( "ID_TEXT_SEED" );
	m_pStaticSeedText->Show( false );
	m_pStaticSeedLine = GetControl<CEtUIStatic>( "ID_STATIC_SEEDLINE" );
	m_pStaticSeedLine->Show( false );
	m_pStaticSeedBoard = GetControl<CEtUIStatic>( "ID_STATIC_SEEDBOARD" );
	m_pStaticSeedBoard->Show( false );
	m_pStaticSeedPoint = GetControl<CEtUIStatic>( "ID_TEXT_SEEDPOINT" );
	m_pStaticSeedPoint->Show( false );
#endif // PRE_ADD_NEW_MONEY_SEED

#ifdef PRE_ADD_PVPRANK_INFORM
	m_pButtonPvPRankInfo = GetControl<CEtUIButton>("ID_BT_RANKINFO");
	m_pButtonPvPRankInfo->Show(false);
#endif

	SetCheckedTab( m_pTabButtonWeapon->GetTabID() );
}

void CDnStoreTabDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if(m_pStaticPointValue && m_pStaticPointValue->IsShow())
	{
		int nPointValue = 0;

		switch(m_nStoreType)
		{
		case Shop::Type::Normal:
		case Shop::Type::Combined:
			break;
		case Shop::Type::LadderPoint:
		case Shop::Type::CombinedLadderPoint:
			{
				if(CDnPartyTask::IsActive())
				{
					nPointValue = CDnPartyTask::GetInstance().GetPVPLadderInfo()->Data.iPvPLadderPoint;
				}
			}
			break;
		case Shop::Type::GuildPoint:
		case Shop::Type::CombinedGuildPoint:
			// int 넘을일이 있으려나..
			if( CDnGuildWarTask::IsActive() )
				nPointValue = (int)CDnGuildWarTask::GetInstance().GetGuildWarFestivalPoint();
			break; // 길드전할때 사용해주세용~ 길드포인트 받아서 출력부분.
		case Shop::Type::ReputePoint:
		case Shop::Type::CombinedReputePoint:
			{
				const CEtUIRadioButton* pCurBtn = GetCurrentTabControl();
				if (pCurBtn)
				{
					const std::wstring& btnText = pCurBtn->GetText();
					DNTableFileFormat*  pUnionTable = GetDNTable(CDnTableDB::TUNION);
					if (pUnionTable == NULL)
						return;
					
					int count = pUnionTable->GetItemCount();
					int i = 0;
					for (; i < count; ++i)
					{
						int idx = pUnionTable->GetItemID(i);
						int uiStringNum = pUnionTable->GetFieldFromLablePtr(idx, "_TabNameID")->GetInteger();
						const wchar_t* uiString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, uiStringNum);
						if (btnText.compare(uiString) == 0)
						{
							CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
							if( !pQuestTask ) 
								return;

							CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
							if (pReputationRepos == NULL)
								return;

							nPointValue = (int)pReputationRepos->GetUnionPoint(CommonUtil::ConvertUnionTableIDToUnionType(idx));
							break;
						}
					}
				}
			}
			break;
		}
		if(m_pStaticPointValue && m_pStaticPointValue->IsShow())
			m_pStaticPointValue->SetIntToText(nPointValue);
	}

#ifdef PRE_ADD_NEW_MONEY_SEED
	if( CDnItemTask::IsActive() && m_pStaticSeedPoint && m_pStaticSeedPoint->IsShow() )
	{
		m_pStaticSeedPoint->SetInt64ToText( GetItemTask().GetSeed() );
	}
#endif // PRE_ADD_NEW_MONEY_SEED
}

void CDnStoreTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_REPAIR") )
		{
			// Note : 아이템 수리
			//
			// 장착한 아이템들의 수리비를 계산해서 다이얼로그로 물어본다.
			int nPrice = GetItemTask().CalcRepairEquipPrice();
			if( nPrice == 0 )
			{
				GetInterface().MessageBox( 1736, MB_OK );
			}
			else
			{
				// 호감도에 따른 수리비 할인 혜택이 있는가.
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
				bool bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::RepairFeeDiscount, nPrice );
				
				// 아이템 구매창에 호감도로 값이 할인된다는 표시 보여줌.
				m_pRepairConfirmEquipDlg->ShowReputationBenefit( bAvailBenefit );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

				m_pRepairConfirmEquipDlg->SetPrice( nPrice );
				m_pRepairConfirmEquipDlg->Show( true );
			}
			return;
		}

		if( IsCmdControl("ID_BUTTON_ALL_REPAIR") )
		{
			// Note : 아이템 전체 수리
			//
			int nPrice = GetItemTask().CalcRepairEquipPrice() + GetItemTask().CalcRepairCharInvenPrice();
			if( nPrice == 0 )
			{
				GetInterface().MessageBox( 1736, MB_OK );
			}
			else
			{
				// 호감도에 따른 수리비 할인 혜택이 있는가.
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
				bool bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::RepairFeeDiscount, nPrice );
				// 아이템 구매창에 호감도로 값이 할인된다는 표시 보여줌.
				m_pRepairConfirmAllDlg->ShowReputationBenefit( bAvailBenefit );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

				m_pRepairConfirmAllDlg->SetPrice( nPrice );
				m_pRepairConfirmAllDlg->Show( true );
			}
			return;
		}

#ifdef PRE_ADD_PVPRANK_INFORM
		if (IsCmdControl("ID_BT_RANKINFO"))
		{
			GetInterface().ShowPVPRankInformDlg(true);
			return;
		}
#endif
	}
	else if (nCommand == EVENT_RADIOBUTTON_CHANGED)
	{
		bool bOnRepurchaseOpen = IsRepurchaseTabBtnName(m_strCmdControlName);
		OnOpenOrCloseRepurchaseTab(bOnRepurchaseOpen);
	}

	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnStoreTabDlg::IsRepurchaseTabBtnName(const std::string& controlName) const
{
	int rpsTabID = GetRepurchasableTabID();
	if (rpsTabID == CDnStoreRepurchaseSystem::eINVALID_REPURCHASE_ID)
		return false;

	CEtUIRadioButton* pBtn = GetStoreTabButtonByTabID(rpsTabID);
	const char* pBtnName = pBtn->GetControlName();
	return (pBtn && pBtnName && controlName.compare(pBtnName) == 0);
}

void CDnStoreTabDlg::OnOpenOrCloseRepurchaseTab(bool bOpenRepurchase)
{
	if (m_pButtonRepair)
		m_pButtonRepair->Show(!bOpenRepurchase);
	if (m_pButtonAllRepair)
		m_pButtonAllRepair->Show(!bOpenRepurchase);

	if (m_pStaticRepurchaseBoard)
		m_pStaticRepurchaseBoard->Show(bOpenRepurchase);
	if (m_pStaticRepurchase)
		m_pStaticRepurchase->Show(bOpenRepurchase);

	if (bOpenRepurchase)
	{
		m_pStaticMyPoint->Show(!bOpenRepurchase);
		m_pStaticPointValue->Show(!bOpenRepurchase);
		m_pStaticCover_A->Show(!bOpenRepurchase);
		m_pStaticCover_B->Show(!bOpenRepurchase);

#ifdef PRE_ADD_NEW_MONEY_SEED
		m_pStaticSeedText->Show( !bOpenRepurchase );
		m_pStaticSeedLine->Show( !bOpenRepurchase );
		m_pStaticSeedBoard->Show( !bOpenRepurchase );
		m_pStaticSeedPoint->Show( !bOpenRepurchase );
#endif // PRE_ADD_NEW_MONEY_SEED

		int rpsTabID = GetRepurchasableTabID();
		if (rpsTabID != CDnStoreRepurchaseSystem::eINVALID_REPURCHASE_ID)
		{
			CDnStoreListDlg* pDlg = GetStoreListDlgByTabID(rpsTabID);
			if (pDlg)
				pDlg->ResetAllSlot();
			SendShopGetRepurchaseList();
		}
		else
		{
			_ASSERT(0);
		}
	}
	else
	{
		SetStoreType(m_nStoreType);
	}
}

void CDnStoreTabDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CEtUITabDialog::Show( bShow );
//	CDnMouseCursor::GetInstance().ShowCursor( bShow );

	if( bShow )
	{
		int nLastTabID = 0;
		if( !m_pStoreWeaponDlg->IsEmptyListDialog() )
		{
			m_pTabButtonWeapon->Show( true );
			nLastTabID++;
		}

		if( !m_pStorePartsDlg->IsEmptyListDialog() )
		{
			m_pTabButtonParts->Show( true );
			nLastTabID++;
		}

		if( !m_pStoreNormalDlg->IsEmptyListDialog() )
		{
			m_pTabButtonNormal->Show( true );
			nLastTabID++;
		}

		if( !m_pStoreBoxDlg->IsEmptyListDialog() )
		{
			m_pTabButtonBox->Show( true );
			nLastTabID++;
		}

		if( !m_pStore5Dlg->IsEmptyListDialog() )
		{
			m_pTabButton5->Show( true );
			nLastTabID++;
		}

		if( !m_pStore6Dlg->IsEmptyListDialog() )
		{
			m_pTabButton6->Show( true );
			nLastTabID++;
		}

		m_pTabButton7->Show(false);

		SetRepurchaseTab(nLastTabID);

		CEtUIRadioButton *pButton(m_pTabButtonWeapon);

		if( m_pTabButtonWeapon->IsShow() )
		{
			pButton = m_pTabButtonWeapon;
		}
		else if( m_pTabButtonParts->IsShow() )
		{
			pButton = m_pTabButtonParts;
		}
		else if( m_pTabButtonNormal->IsShow() )
		{
			pButton = m_pTabButtonNormal;
		}
		else if( m_pTabButtonBox->IsShow() )
		{
			pButton = m_pTabButtonBox;
		}
		else if( m_pTabButton5->IsShow() )
		{
			pButton = m_pTabButton5;
		}
		else if( m_pTabButton6->IsShow() )
		{
			pButton = m_pTabButton6;
		}

		if( pButton )
		{
			SetCheckedTab( pButton->GetTabID() );
		}
	}
	else
	{
		if( drag::IsValid() )
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			if( pDragButton->GetSlotType() == ST_STORE )
			{
				drag::ReleaseControl();
			}
		}

		if( CDnTradeTask::IsActive() )
		{
			GetTradeTask().GetTradeItem().DeleteAllStore();
		}

		GetInterface().CloseBlind();

		m_pTabButtonWeapon->Show( false );
		m_pTabButtonParts->Show( false );
		m_pTabButtonNormal->Show( false );
		m_pTabButtonBox->Show( false );
		m_pTabButton5->Show( false );
		m_pTabButton6->Show( false );
	}
}

void CDnStoreTabDlg::ResetRepurchaseTab()
{
	m_pStoreWeaponDlg->SetRepurchaseDlg(false);
	m_pStorePartsDlg->SetRepurchaseDlg(false);
	m_pStoreNormalDlg->SetRepurchaseDlg(false);
	m_pStoreBoxDlg->SetRepurchaseDlg(false);
	m_pStore5Dlg->SetRepurchaseDlg(false);
	m_pStore6Dlg->SetRepurchaseDlg(false);
	m_pStore7Dlg->SetRepurchaseDlg(false);
}

void CDnStoreTabDlg::SetRepurchaseTab(int nLastTabID)
{
	ResetRepurchaseTab();

	int nRepurchaseTabID = 0;

	if (nLastTabID < 1 || nLastTabID > SHOPTABMAX + 1)
	{
		_ASSERT(0);
		return;
	}

	nRepurchaseTabID = nLastTabID + 1;
	SetTabUIStringID(nRepurchaseTabID, 1779); // UISTRING : 재구매
	CEtUIRadioButton* pRepurchaseTabButtton = GetStoreTabButtonByTabID(nRepurchaseTabID);
	CDnStoreListDlg* pRepurchaseListDlg = GetStoreListDlgByTabID(nRepurchaseTabID);
	if (pRepurchaseTabButtton && pRepurchaseListDlg)
	{
		pRepurchaseTabButtton->Show(true);
		pRepurchaseListDlg->SetRepurchaseDlg(true);
	}
	else
	{
		_ASSERT(0);
		return;
	}
}

CEtUIRadioButton* CDnStoreTabDlg::GetStoreTabButtonByTabID(int tabID) const
{
	switch(tabID)
	{
	case 1: return m_pTabButtonWeapon;
	case 2: return m_pTabButtonParts;
	case 3: return m_pTabButtonNormal;
	case 4: return m_pTabButtonBox;
	case 5: return m_pTabButton5;
	case 6: return m_pTabButton6;
	case 7: return m_pTabButton7;
	default:
		return NULL;
	}
}

CDnStoreListDlg* CDnStoreTabDlg::GetStoreListDlgByTabID(int tabID) const
{
	switch(tabID)
	{
	case 1:	return m_pStoreWeaponDlg;
	case 2: return m_pStorePartsDlg;
	case 3: return m_pStoreNormalDlg;
	case 4: return m_pStoreBoxDlg;
	case 5: return m_pStore5Dlg;
	case 6: return m_pStore6Dlg;
	case 7: return m_pStore7Dlg;
	default:
		return NULL;
	}
}

bool CDnStoreTabDlg::IsRepurchasableTab(int nTabID) const
{
	CDnStoreListDlg* pCurList = GetStoreListDlgByTabID(nTabID);
	if (pCurList == NULL)
		return false;

	return pCurList->IsRepurchaseDlg();
}

int CDnStoreTabDlg::GetRepurchasableTabID() const
{
	int i = 1;
	for (; i <= SHOPTABMAX + 1; ++i)
	{
		CDnStoreListDlg* pDlg = GetStoreListDlgByTabID(i);
		if (pDlg)
		{
			if (pDlg->IsRepurchaseDlg())
				return i;
		}
	}

	return INVALID_TABID;
}

bool CDnStoreTabDlg::IsShowRepurchasableTab() const
{
	int tabId = GetRepurchasableTabID();
	if (tabId == INVALID_TABID)
		return false;

	CDnStoreListDlg* pDlg = GetStoreListDlgByTabID(tabId);
	if (pDlg && pDlg->IsRepurchaseDlg() && pDlg->IsShow())
		return true;

	return false;
}

void CDnStoreTabDlg::SetStoreType(int nType)
{
	m_nStoreType = nType;

#ifdef PRE_ADD_NEW_MONEY_SEED
	bool bShowSeedShop = false;
#endif // PRE_ADD_NEW_MONEY_SEED
	bool bEnablePointView = false;

	m_pStaticCover_A->Show(false);
	m_pStaticCover_B->Show(false);

	switch(nType)
	{
	case Shop::Type::Normal:
	case Shop::Type::Combined:
		break;
	case Shop::Type::LadderPoint:
	case Shop::Type::CombinedLadderPoint:
		m_pStaticMyPoint->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126176 ));
		bEnablePointView = true;
		break;
	case Shop::Type::GuildPoint:
	case Shop::Type::CombinedGuildPoint:
		m_pStaticMyPoint->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126084 ));
		bEnablePointView = true;
		break;
	case Shop::Type::ReputePoint:
	case Shop::Type::CombinedReputePoint:
		m_pStaticMyPoint->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3273 )); // UISTRING : 연합 포인트
		bEnablePointView = true;
		break;
#ifdef PRE_ADD_NEW_MONEY_SEED
	case Shop::Type::CombinedSeedPoint:
		bShowSeedShop = true;
		break;
#endif // PRE_ADD_NEW_MONEY_SEED
	}

	m_pStaticMyPoint->Show(bEnablePointView);
	m_pStaticPointValue->Show(bEnablePointView);
	m_pStaticCover_A->Show(bEnablePointView);
	m_pStaticCover_B->Show(bEnablePointView);

#ifdef PRE_ADD_NEW_MONEY_SEED
	m_pStaticSeedText->Show( bShowSeedShop );
	m_pStaticSeedLine->Show( bShowSeedShop );
	m_pStaticSeedBoard->Show( bShowSeedShop );
	m_pStaticSeedPoint->Show( bShowSeedShop );
#endif // PRE_ADD_NEW_MONEY_SEED
}

#ifdef PRE_ADD_PVPRANK_INFORM
void CDnStoreTabDlg::CheckPvPRankButton(int nShopID)
{
	if (nShopID > 0)
	{
		bool bShowPvPRankInfoButton = (nShopID >= PvPRankInfoUIDef::eMIN_PVPRANK_SHOPID && nShopID <= PvPRankInfoUIDef::eMAX_PVPRANK_SHOPID);
		m_pButtonPvPRankInfo->Show(bShowPvPRankInfoButton);
	}
}
#endif

void CDnStoreTabDlg::SetStoreItem( int nTabID, MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnStoreTabDlg::SetItem, pItem is NULL!");

	CDnItem *pSellItem = (CDnItem*)pItem;
	if( !pSellItem ) return;

	switch( nTabID ) {
		case 1:	m_pStoreWeaponDlg->SetItem( pItem ); break;
		case 2:	m_pStorePartsDlg->SetItem( pItem ); break;
		case 3:	m_pStoreNormalDlg->SetItem( pItem ); break;
		case 4:	m_pStoreBoxDlg->SetItem( pItem ); break;
		case 5:	m_pStore5Dlg->SetItem( pItem ); break;
		case 6:	m_pStore6Dlg->SetItem( pItem ); break;
		case 7:	m_pStore7Dlg->SetItem( pItem ); break;
		default:
			CDebugSet::ToLogFile( "CDnStoreTabDlg::SetStoreItem, default case(%d)", nTabID );
			break;
	}
}


void CDnStoreTabDlg::ResetAllListSlot()
{
	m_pStoreWeaponDlg->ResetAllSlot();
	m_pStorePartsDlg->ResetAllSlot();
	m_pStoreNormalDlg->ResetAllSlot();
	m_pStoreBoxDlg->ResetAllSlot();
	m_pStore5Dlg->ResetAllSlot();
	m_pStore6Dlg->ResetAllSlot();
	m_pStore7Dlg->ResetAllSlot();
}

void CDnStoreTabDlg::ResetSlot(BYTE cSlotIndex)
{
	if (IsShowRepurchasableTab())
	{
		int rpsID = GetRepurchasableTabID();
		CDnStoreListDlg* pDlg = GetStoreListDlgByTabID(rpsID);
		if (pDlg != NULL)
			pDlg->ResetSlot(cSlotIndex);
		else
			_ASSERT(0);
	}
}

void CDnStoreTabDlg::OnStoreSell()
{
	if (IsShowRepurchasableTab() && m_pTabButtonWeapon)
		SetCheckedTab( m_pTabButtonWeapon->GetTabID() );
}

void CDnStoreTabDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch( nID ) 
		{
		case REPAIR_CONFIRM_EQUIP_DIALOG:
			{
				if( IsCmdControl("ID_OK") )
				{
					GetItemTask().RequestRepairEquip();
					m_pRepairConfirmEquipDlg->Show( false );
				}
				else if( IsCmdControl("ID_CANCEL") )
				{
					m_pRepairConfirmEquipDlg->Show( false );
				}
			}
			break;
		case REPAIR_CONFIRM_ALL_DIALOG:
			{
				if( IsCmdControl("ID_OK") )
				{
					GetItemTask().RequestRepairAll();
					m_pRepairConfirmAllDlg->Show( false );
				}
				else if( IsCmdControl("ID_CANCEL") )
				{
					m_pRepairConfirmAllDlg->Show( false );
				}
			}
			break;
		}
	}
}

void CDnStoreTabDlg::SetTabUIStringID( int nTabID, int nUIStringID )
{
	if( nUIStringID <= 0 ) return;

	LPCWSTR wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringID );
	switch( nTabID ) {
		case 1:	m_pTabButtonWeapon->SetText( wszName ); break;
		case 2:	m_pTabButtonParts->SetText( wszName ); break;
		case 3:	m_pTabButtonNormal->SetText( wszName ); break;
		case 4:	m_pTabButtonBox->SetText( wszName ); break;
		case 5:	m_pTabButton5->SetText( wszName ); break;
		case 6:	m_pTabButton6->SetText( wszName ); break;
		case 7: m_pTabButton7->SetText( wszName ); break;
		default:
			CDebugSet::ToLogFile( "CDnStoreTabDlg::SetTabUIStringID, default case(%d)", nTabID );
			break;
	}
}