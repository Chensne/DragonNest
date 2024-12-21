#include "StdAfx.h"

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL

#include "DnPotentialStatsDlg.h"
#include "DnPotentialJewelDlg.h"
#include "DnPlayerActor.h"
#include "DnPotentialStateItemDlg.h"
#include "DnInterfaceString.h"
#include "DnParts.h"
#include "DnItemTask.h"
#include "TaskManager.h"
#include "DnTooltipDlg.h"
#include "DnActorState.h"

#ifdef _TH
#define INCREASE_MARKER L"+"
#define DECREASE_MARKER L"-"
#else
#define INCREASE_MARKER L"▲"
#define DECREASE_MARKER L"▼"
#endif
using namespace DN_INTERFACE::STRING;


CDnPotentialStatsDlg::CDnPotentialStatsDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true ) 
, m_pListBox(NULL)
, m_pStaticTitle(NULL)
{
}

CDnPotentialStatsDlg::~CDnPotentialStatsDlg()
{
}

void CDnPotentialStatsDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemPowerTooltipDlg.ui" ).c_str(), bShow );
}

void CDnPotentialStatsDlg::InitialUpdate()
{
	m_pListBox = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_TOOLTIP");
	m_pStaticTitle = GetControl<CEtUIStatic>("ID_TEXT_TITLE");
	m_pStaticTitle->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9169));
}

void CDnPotentialStatsDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if(bShow)
		ShowStateDialog();

	CEtUIDialog::Show(bShow);
}

bool CDnPotentialStatsDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		{
			return false;
		}
		break;
	}
	
	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnPotentialStatsDlg::GetNewState(bool bIsTakeAway, int nCloneState, int nItemState, int& nResultState)
{
	if(bIsTakeAway)
		nResultState = nCloneState - nItemState;
	else
		nResultState = nCloneState + nItemState;
}

void CDnPotentialStatsDlg::GetNewState(bool bIsTakeAway, int nCloneState, float fItemState, int& nResultState)
{
	if(bIsTakeAway)
		nResultState = (int)((float)nCloneState - fItemState);
	else
		nResultState = (int)((float)nCloneState + fItemState);
}

void CDnPotentialStatsDlg::CalcStateRatio(bool bIsTakeAway, int nCloneState, float fItemStateRatio, int& nResultState)
{
	float	fAgilityRatio	= fItemStateRatio * 100.0f;
	int		nMyAgilityRatio = nCloneState;

	// 숫자 * (1+퍼센트/100)
	if(bIsTakeAway)
		nMyAgilityRatio = (int)(nMyAgilityRatio * ( 1-(fAgilityRatio / 100) ));
	else
		nMyAgilityRatio += (int)(nMyAgilityRatio * fItemStateRatio);
		//nMyAgilityRatio = (int)(nMyAgilityRatio * ( 1+(fAgilityRatio / 100) ));

	nResultState = nMyAgilityRatio;
}

void CDnPotentialStatsDlg::CalcStateRatio(bool bIsTakeAway, float fCloneState, float fItemStateRatio, float& fResultState)
{
	float	fAgilityRatio	= fItemStateRatio * 100.0f;
	float	fMyAgilityRatio = fCloneState;

	// 숫자 * (1+퍼센트/100)
	if(bIsTakeAway)
		fMyAgilityRatio = fMyAgilityRatio * ( 1-(fAgilityRatio / 100) );
	else
		fMyAgilityRatio += fMyAgilityRatio * fItemStateRatio;
		//fMyAgilityRatio = fMyAgilityRatio * ( 1+(fAgilityRatio / 100) );

	fResultState = fMyAgilityRatio;
}

void CDnPotentialStatsDlg::ShowStateDialog()
{
	// 플레이어 스탯
	if(!CDnActor::s_hLocalActor) return;
	DnPlayerActorHandle hActor = CDnActor::s_hLocalActor;

	CDnItemTask* pItemTask = (CDnItemTask*)CTaskManager::GetInstance().GetTask("ItemTask");
	if(!pItemTask) return;
	
	CDnPotentialJewelDlg* pPotentialJewlDlg = dynamic_cast<CDnPotentialJewelDlg*>(GetParentDialog());
	if(!pPotentialJewlDlg) return;
	
	// 새로운 아이템 스탯
	CDnItem* pNewItem = pPotentialJewlDlg->GetCurrentItem();
	if(!pNewItem) return;
	
	CDnParts* pPartsItem = static_cast<CDnParts*>(pNewItem);
	if(!pPartsItem) return;
	CDnParts::PartsTypeEnum ItemPartsType = pPartsItem->GetPartsType();		

	// 착용중인 아이템 스탯
	CDnItem* pEquippedItem = NULL;
	if(pPartsItem->IsCashItem())
		pEquippedItem = pItemTask->GetCashEquipItem(ItemPartsType);
	else
		pEquippedItem = pItemTask->GetEquipItem(ItemPartsType);

	CDnState* pCurrentState = hActor->GetStateStep(0);
	if(!pCurrentState) return;

	m_pListBox->RemoveAllItems();

	CDnState OriginalState;
	hActor->MakeComparePotentialItemState(OriginalState, pEquippedItem, pNewItem);
	UpdateNewState(&OriginalState, pNewItem->GetPotentialState(), pCurrentState);	// 새로운   아이템에서 "적용"되는 "잠재력 stats" 출력
	UpdateNewState(&OriginalState, pNewItem, pCurrentState);						// 새로운   아이템에서 "적용"되는 "일반	  stats" 출력
}

void CDnPotentialStatsDlg::AddListBoxText(int nEnumStringID, float fCombineStats, float fPlayerStats, ePotentialType eType)
{
	if(fCombineStats < 0.0f) // 바뀐게 없음
		return;

	if(m_pListBox)
	{
		if(m_pListBox->FindItem<CDnPotentialStateItemDlg>(nEnumStringID) == NULL)
			return;

		//int nListBoxSize = m_pListBox->GetSize();
		//for(int i = 0 ; i < nListBoxSize ; ++i)
		//{
		//	int nValue = 0;
		//	m_pListBox->GetItemValue(i, nValue);
		//	// 중복 메세지 걸러냄.
		//	if(nValue == nEnumStringID)
		//		return;
		//}
	}

	bool bIsUp = true;
	std::wstring wszMaxStats, wszInDecreaseStats, wszStatName;

	if(fCombineStats < 0)
	{
		bIsUp = false; // 음수
		ChangePositiveNumber(fCombineStats);
	}
	else
		bIsUp = true; // 양수

	//----------------------------------
	// ex) 힘		315 (▼20)
	// ex) 힘
	wszStatName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nEnumStringID);

	// ex) 315
	wszMaxStats = FormatW(L"%d", fPlayerStats);

	// ex) (▼20)
	if(eType == ePOTENTIAL_RANGE)		// 실수
		wszInDecreaseStats = FormatW(L"(%s%d%%)", (bIsUp)? INCREASE_MARKER : DECREASE_MARKER , abs(fCombineStats));
	else if( eType == ePOTENTIAL_VALUE) // 정수
		wszInDecreaseStats = FormatW(L"(%s%d)", (bIsUp)? INCREASE_MARKER : DECREASE_MARKER , fCombineStats);
	
	// 임시 리스트 박스 add
	//std::wstring szTempString;
	//szTempString = FormatW(L"%s %s %s", wszStatName.c_str(), wszMaxStats.c_str(), wszInDecreaseStats.c_str());
	//m_pListBox->AddItem(szTempString.c_str(), NULL, nEnumStringID);
	
	CDnPotentialStateItemDlg* pItem = m_pListBox->AddItem<CDnPotentialStateItemDlg>(nEnumStringID);
	if(pItem)
	{
		pItem->SetInfo( wszStatName, wszMaxStats.c_str() ,wszInDecreaseStats.c_str() , bIsUp);
	}
	
}

void CDnPotentialStatsDlg::AddListBoxText(int nEnumStringID, int nCombineStats, int nPlayerStats, ePotentialType eType)
{
	if(nCombineStats == 0) // 바뀐게 없음
		return;

	if(m_pListBox)
	{
		if(m_pListBox->FindItem<CDnPotentialStateItemDlg>(nEnumStringID))
			return;
		
		//int nListBoxSize = m_pListBox->GetSize();
		//for(int i = 0 ; i < nListBoxSize ; ++i)
		//{
		//	int nValue = 0;
		//	m_pListBox->GetItemValue(i, nValue);
		//	// 중복 메세지 걸러냄.
		//	if(nValue == nEnumStringID)
		//		return;
		//}
	}

	bool bIsUp = true;
	std::wstring wszMaxStats, wszInDecreaseStats, wszStatName;

	if(nCombineStats < 0)
	{
		bIsUp = false; // 음수
		ChangePositiveNumber(nCombineStats);
	}
	else
		bIsUp = true; // 양수

	//----------------------------------
	// ex) 힘		315 (▼20)

	// ex) 힘
	wszStatName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nEnumStringID);

	// ex) 315
	wszMaxStats = FormatW(L"%d", nPlayerStats);

	// ex) (▼20)
	if(eType == ePOTENTIAL_RANGE) // 실수
		wszInDecreaseStats = FormatW(L"(%s%d%%)", (bIsUp)? INCREASE_MARKER : DECREASE_MARKER , abs(nCombineStats));
	else if( eType == ePOTENTIAL_VALUE) // 정수
		wszInDecreaseStats = FormatW(L"(%s%d)", (bIsUp)? INCREASE_MARKER : DECREASE_MARKER , nCombineStats);
	
	// 임시 리스트 박스 add
	//std::wstring szTempString;
	//szTempString = FormatW(L"%s %s %s", wszStatName.c_str(), wszMaxStats.c_str(), wszInDecreaseStats.c_str());
	//m_pListBox->AddItem(szTempString.c_str(), NULL, nEnumStringID);

	CDnPotentialStateItemDlg* pItem = m_pListBox->AddItem<CDnPotentialStateItemDlg>(nEnumStringID);
	if(pItem)
	{
		pItem->SetInfo( wszStatName, wszMaxStats.c_str() ,wszInDecreaseStats.c_str() , bIsUp);
	}
}

void CDnPotentialStatsDlg::SetCloneState(CDnState* pCloneMyState, CDnState* pNewItemState, bool bIsTakeAway /*=false*/ )
{
	if(!pCloneMyState || !pNewItemState) return;

	int nResultState = 0;

	// 공격력/방어력
	if( pNewItemState->GetAttackPMin() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetAttackPMin(), pNewItemState->GetAttackPMin(), nResultState);
		pCloneMyState->SetAttackPMin(nResultState);
	}
	if( pNewItemState->GetAttackPMax() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetAttackPMax(), pNewItemState->GetAttackPMax(), nResultState);
		pCloneMyState->SetAttackPMax(nResultState);
	}
	if( pNewItemState->GetAttackMMin() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetAttackMMin(), pNewItemState->GetAttackMMin(), nResultState);
		pCloneMyState->SetAttackMMin(nResultState);
	}
	if( pNewItemState->GetAttackMMax() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetAttackMMax(), pNewItemState->GetAttackMMax(), nResultState);
		pCloneMyState->SetAttackMMax(nResultState);
	}


	if( pNewItemState->GetDefenseP() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetDefenseP(), pNewItemState->GetDefenseP(), nResultState);
		pCloneMyState->SetDefenseP(nResultState);
	}
	if( pNewItemState->GetDefenseM() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetDefenseM(), pNewItemState->GetDefenseM(), nResultState);
		pCloneMyState->SetDefenseM(nResultState);
	}

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pNewItemState->GetStrength() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetStrength(), pNewItemState->GetStrength(), nResultState);
		pCloneMyState->SetStrength(nResultState);
	}
	if( pNewItemState->GetAgility() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetAgility(), pNewItemState->GetAgility(), nResultState);
		pCloneMyState->SetAgility(nResultState);
	}
	if( pNewItemState->GetIntelligence() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetIntelligence(), pNewItemState->GetIntelligence(), nResultState);
		pCloneMyState->SetIntelligence(nResultState);
	}
	if( pNewItemState->GetStamina() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetStamina(), pNewItemState->GetStamina(), nResultState);
		pCloneMyState->SetStamina(nResultState);
	}
	if( pNewItemState->GetSpirit() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetSpirit(), pNewItemState->GetSpirit(), nResultState);
		pCloneMyState->SetSpirit(nResultState);
	}

	// 내부 스탯 증가
	if( pNewItemState->GetStiff() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetStiff(), pNewItemState->GetStiff(), nResultState);
		pCloneMyState->SetStiff(nResultState);
	}

	if( pNewItemState->GetStiffResistance() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetStiffResistance(), pNewItemState->GetStiffResistance(), nResultState);
		pCloneMyState->SetStiffResistance(nResultState);
	}

	if( pNewItemState->GetStun() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetStun(), pNewItemState->GetStun(), nResultState);
		pCloneMyState->SetStun(nResultState);
	}

	if( pNewItemState->GetStunResistance() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetStunResistance(), pNewItemState->GetStunResistance(), nResultState);
		pCloneMyState->SetStunResistance(nResultState);
	}

	if( pNewItemState->GetCritical() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetCritical(), pNewItemState->GetCritical(), nResultState);
		pCloneMyState->SetCritical(nResultState);
	}

	if( pNewItemState->GetCriticalResistance() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetCriticalResistance(), pNewItemState->GetCriticalResistance(), nResultState);
		pCloneMyState->SetCriticalResistance(nResultState);
	}

	if( pNewItemState->GetMoveSpeed() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetMoveSpeed(), pNewItemState->GetMoveSpeed(), nResultState);
		pCloneMyState->SetMoveSpeed(nResultState);
	}

	if( pNewItemState->GetSafeZoneMoveSpeed() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetSafeZoneMoveSpeed(), pNewItemState->GetSafeZoneMoveSpeed(), nResultState);
		pCloneMyState->SetSafeZoneMoveSpeed(nResultState);
	}

	if( pNewItemState->GetMaxHP() > 0 )
	{
		GetNewState(bIsTakeAway, (int)pCloneMyState->GetMaxHP(), (int)pNewItemState->GetMaxHP(), nResultState);
		pCloneMyState->SetMaxHP(nResultState);
	}

	if( pNewItemState->GetMaxSP() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetMaxSP(), pNewItemState->GetMaxSP(), nResultState);
		pCloneMyState->SetMaxSP(nResultState);
	}

	if( pNewItemState->GetRecoverySP() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetRecoverySP(), pNewItemState->GetRecoverySP(), nResultState);
		pCloneMyState->SetRecoverySP(nResultState);
	}

	// 슈퍼아머, 파이널데미지
	if( pNewItemState->GetSuperAmmor() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetSuperAmmor(), pNewItemState->GetSuperAmmor(), nResultState);
		pCloneMyState->SetSuperAmmor(nResultState);
	}

	if( pNewItemState->GetFinalDamage() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetFinalDamage(), pNewItemState->GetFinalDamage(), nResultState);
		pCloneMyState->SetFinalDamage(nResultState);
	}


	//--------------------------------------------------------
	// 아이템 능력치 표시 두번째. Ratio관련.
	// 공격력/방어력
	if( pNewItemState->GetAttackPMinRatio() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetAttackPMin(), pNewItemState->GetAttackPMinRatio(), nResultState);
		pCloneMyState->SetAttackPMin(nResultState);
	}

	if( pNewItemState->GetAttackPMaxRatio() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetAttackPMax(), pNewItemState->GetAttackPMaxRatio(), nResultState);
		pCloneMyState->SetAttackPMax(nResultState);
	}

	if( pNewItemState->GetAttackMMinRatio() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetAttackMMin(), pNewItemState->GetAttackMMinRatio(), nResultState);
		pCloneMyState->SetAttackMMin(nResultState);
	}

	if( pNewItemState->GetAttackMMaxRatio() > 0 )
	{
		GetNewState(bIsTakeAway, pCloneMyState->GetAttackMMax(), pNewItemState->GetAttackMMaxRatio(), nResultState);
		pCloneMyState->SetAttackMMax(nResultState);
	}

	if( pNewItemState->GetDefensePRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetDefenseP(), pNewItemState->GetDefensePRatio(), nResultState);
		pCloneMyState->SetDefenseP(nResultState);
	}

	if( pNewItemState->GetDefenseMRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetDefenseM(), pNewItemState->GetDefenseMRatio(), nResultState);
		pCloneMyState->SetDefenseM(nResultState);
	}

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pNewItemState->GetStrengthRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetStrength(), pNewItemState->GetStrengthRatio(), nResultState);
		pCloneMyState->SetStrength(nResultState);
	}

	if( pNewItemState->GetAgilityRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetAgility(), pNewItemState->GetAgilityRatio(), nResultState);
		pCloneMyState->SetAgility(nResultState);
	}

	if( pNewItemState->GetIntelligenceRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetIntelligence(), pNewItemState->GetIntelligenceRatio(), nResultState);
		pCloneMyState->SetIntelligence(nResultState);
	}

	if( pNewItemState->GetStaminaRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetStamina(), pNewItemState->GetStaminaRatio(), nResultState);
		pCloneMyState->SetStamina(nResultState);
	}

	if( pNewItemState->GetSpiritRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetSpirit(), pNewItemState->GetSpiritRatio(), nResultState);
		pCloneMyState->SetSpirit(nResultState);
	}

	// 내부 스탯 증가
	if( pNewItemState->GetStiffRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetStiff(), pNewItemState->GetStiffRatio(), nResultState);
		pCloneMyState->SetStiff(nResultState);
	}

	if( pNewItemState->GetStiffResistanceRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetStiffResistance(), pNewItemState->GetStiffResistanceRatio(), nResultState);
		pCloneMyState->SetStiffResistance(nResultState);
	}

	if( pNewItemState->GetStunRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetStun(), pNewItemState->GetStunRatio(), nResultState);
		pCloneMyState->SetStun(nResultState);
	}

	if( pNewItemState->GetStunResistanceRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetStunResistance(), pNewItemState->GetStunResistanceRatio(), nResultState);
		pCloneMyState->SetStunResistance(nResultState);
	}

	if( pNewItemState->GetCriticalRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetCritical(), pNewItemState->GetCriticalRatio(), nResultState);
		pCloneMyState->SetCritical(nResultState);
	}

	if( pNewItemState->GetCriticalResistanceRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetCriticalResistance(), pNewItemState->GetCriticalResistanceRatio(), nResultState);
		pCloneMyState->SetCriticalResistance(nResultState);
	}

	if( pNewItemState->GetMoveSpeedRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetMoveSpeed(), pNewItemState->GetMoveSpeedRatio(), nResultState);
		pCloneMyState->SetMoveSpeed(nResultState);
	}

	if( pNewItemState->GetSafeZoneMoveSpeedRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetSafeZoneMoveSpeed(), pNewItemState->GetSafeZoneMoveSpeedRatio(), nResultState);
		pCloneMyState->SetSafeZoneMoveSpeed(nResultState);
	}

	if( pNewItemState->GetMaxHPRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, (int)pCloneMyState->GetMaxHP(), pNewItemState->GetMaxHPRatio(), nResultState);
		pCloneMyState->SetMaxHP(nResultState);
	}

	if( pNewItemState->GetMaxSPRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetMaxSP(), pNewItemState->GetMaxSPRatio(), nResultState);
		pCloneMyState->SetMaxSP(nResultState);
	}

	if( pNewItemState->GetRecoverySPRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetRecoverySP(), pNewItemState->GetRecoverySPRatio(), nResultState);
		pCloneMyState->SetRecoverySP(nResultState);
	}

	// 슈퍼아머, 파이널데미지
	if( pNewItemState->GetSuperAmmorRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetSuperAmmor(), pNewItemState->GetSuperAmmorRatio(), nResultState);
		pCloneMyState->SetSuperAmmor(nResultState);
	}

	if( pNewItemState->GetFinalDamageRatio() > 0 )
	{
		CalcStateRatio(bIsTakeAway, pCloneMyState->GetFinalDamage(), pNewItemState->GetFinalDamageRatio(), nResultState);
		pCloneMyState->SetFinalDamage(nResultState);
	}

	// 속성 공격력/저항 만 별도의 Ratio가 없으므로 아래 출력한다.
	float fResultState = .0f;
	if( (pNewItemState->GetElementAttack( CDnState::Light ) * 100.f) > 0 )
	{
		CalcStateRatio(bIsTakeAway, (pCloneMyState->GetElementAttack( CDnState::Light ) * 100.f), (pNewItemState->GetElementAttack( CDnState::Light ) * 100.f), fResultState);
		pCloneMyState->SetElementAttack(CDnState::Light, fResultState);
	}

	if( (pNewItemState->GetElementAttack( CDnState::Dark ) * 100.f) > 0 )
	{
		CalcStateRatio(bIsTakeAway, (pCloneMyState->GetElementAttack( CDnState::Dark ) * 100.f), (pNewItemState->GetElementAttack( CDnState::Dark ) * 100.f), fResultState);
		pCloneMyState->SetElementAttack(CDnState::Dark, fResultState);
	}

	if( ( pNewItemState->GetElementAttack( CDnState::Fire ) * 100.f ) > 0 )
	{
		CalcStateRatio(bIsTakeAway, (pCloneMyState->GetElementAttack( CDnState::Fire ) * 100.f), (pNewItemState->GetElementAttack( CDnState::Fire ) * 100.f), fResultState);
		pCloneMyState->SetElementAttack(CDnState::Fire, fResultState);
	}

	if( ( pNewItemState->GetElementAttack( CDnState::Ice ) * 100.f ) > 0 )
	{
		CalcStateRatio(bIsTakeAway, (pCloneMyState->GetElementAttack( CDnState::Ice ) * 100.f), (pNewItemState->GetElementAttack( CDnState::Ice ) * 100.f), fResultState);
		pCloneMyState->SetElementAttack(CDnState::Ice, fResultState);
	}

	if( (pNewItemState->GetElementDefense( CDnState::Light ) * 100.f) > 0 )
	{
		CalcStateRatio(bIsTakeAway, (pCloneMyState->GetElementDefense( CDnState::Light ) * 100.f), (pNewItemState->GetElementDefense( CDnState::Light ) * 100.f), fResultState);
		pCloneMyState->SetElementDefense(CDnState::Light, fResultState);
	}

	if( (pNewItemState->GetElementDefense( CDnState::Dark ) * 100.f) > 0 )
	{
		CalcStateRatio(bIsTakeAway, (pCloneMyState->GetElementDefense( CDnState::Dark ) * 100.f), (pNewItemState->GetElementDefense( CDnState::Dark ) * 100.f), fResultState);
		pCloneMyState->SetElementDefense(CDnState::Dark, fResultState);
	}

	if( (pNewItemState->GetElementDefense( CDnState::Fire ) * 100.f) > 0 )
	{
		CalcStateRatio(bIsTakeAway, (pCloneMyState->GetElementDefense( CDnState::Fire ) * 100.f), (pNewItemState->GetElementDefense( CDnState::Fire ) * 100.f), fResultState);
		pCloneMyState->SetElementDefense(CDnState::Fire, fResultState);
	}

	if( (pNewItemState->GetElementDefense( CDnState::Ice ) * 100.f) > 0 )
	{
		CalcStateRatio(bIsTakeAway, (pCloneMyState->GetElementDefense( CDnState::Ice ) * 100.f), (pNewItemState->GetElementDefense( CDnState::Ice ) * 100.f), fResultState);
		pCloneMyState->SetElementDefense(CDnState::Ice, fResultState);
	}

	if( (pNewItemState->GetAddExp() * 100.0f) > 0 )
	{
		float fResultState = .0f;
		if(bIsTakeAway)
			fResultState = (pCloneMyState->GetAddExp()*100.0f) - (pNewItemState->GetAddExp()*100.0f);
		else
			fResultState = (pCloneMyState->GetAddExp()*100.0f) + (pNewItemState->GetAddExp()*100.0f);
		
		pCloneMyState->SetAddExp(fResultState);
	}
}

void CDnPotentialStatsDlg::UpdateNewState(CDnState* pCloneMyState, CDnState* pNewItemState, CDnState* pOriginalMyState)
{
	if(!pCloneMyState || !pNewItemState || !pOriginalMyState) return;

	// 공격력/방어력
	if( pNewItemState->GetAttackPMin() > 0 )
	{
		AddListBoxText(	5018, 
			pCloneMyState->GetAttackPMin() - pOriginalMyState->GetAttackPMin(), 
			pCloneMyState->GetAttackPMin(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetAttackPMax() > 0 )
	{
		AddListBoxText(	5018, 
			pCloneMyState->GetAttackPMax()-pOriginalMyState->GetAttackPMax(),
			pCloneMyState->GetAttackPMax(), 
			ePOTENTIAL_VALUE );
	}
	if( pNewItemState->GetAttackMMin() > 0 )
	{
		AddListBoxText(	5019, 
			pCloneMyState->GetAttackMMin()-pOriginalMyState->GetAttackMMin(),
			pCloneMyState->GetAttackMMin(), 
			ePOTENTIAL_VALUE );
	}
	if( pNewItemState->GetAttackMMax() > 0 )
	{
		AddListBoxText(	5019, 
			pCloneMyState->GetAttackMMax()-pOriginalMyState->GetAttackMMax(),
			pCloneMyState->GetAttackMMax(), 
			ePOTENTIAL_VALUE );
	}
	

	if( pNewItemState->GetDefenseP() > 0 )
	{
		AddListBoxText(	5020, 
			pCloneMyState->GetDefenseP()-pOriginalMyState->GetDefenseP(),
			pCloneMyState->GetDefenseP(), 
			ePOTENTIAL_VALUE );
	}
	if( pNewItemState->GetDefenseM() > 0 )
	{
		AddListBoxText(	5062, 
			pCloneMyState->GetDefenseM()-pOriginalMyState->GetDefenseM(),
			pCloneMyState->GetDefenseM(), 
			ePOTENTIAL_VALUE );
	}

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pNewItemState->GetStrength() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STRENGTH ), 
			pCloneMyState->GetStrength()-pOriginalMyState->GetStrength(),
			pCloneMyState->GetStrength(), 
			ePOTENTIAL_VALUE );
	}
	if( pNewItemState->GetAgility() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_AGILITY ), 
			pCloneMyState->GetAgility()-pOriginalMyState->GetAgility(),
			pCloneMyState->GetAgility(), 
			ePOTENTIAL_VALUE );
	}
	if( pNewItemState->GetIntelligence() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_INTELLIGENCE ), 
			pCloneMyState->GetIntelligence()-pOriginalMyState->GetIntelligence(),
			pCloneMyState->GetIntelligence(), 
			ePOTENTIAL_VALUE );
	}
	if( pNewItemState->GetStamina() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STAMINA ), 
			pCloneMyState->GetStamina()-pOriginalMyState->GetStamina(),
			pCloneMyState->GetStamina(), 
			ePOTENTIAL_VALUE );
	}
	if( pNewItemState->GetSpirit() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_SPIRIT ), 
			pCloneMyState->GetSpirit()-pOriginalMyState->GetSpirit(),
			pCloneMyState->GetSpirit(), 
			ePOTENTIAL_VALUE );
	}

	// 내부 스탯 증가
	if( pNewItemState->GetStiff() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ), 
			pCloneMyState->GetStiff()-pOriginalMyState->GetStiff(),
			pCloneMyState->GetStiff(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetStiffResistance() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ), 
			pCloneMyState->GetStiffResistance()-pOriginalMyState->GetStiffResistance(),
			pCloneMyState->GetStiffResistance(), 
			ePOTENTIAL_VALUE );
		
	}

	if( pNewItemState->GetStun() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STUN_P ), 
			pCloneMyState->GetStun()-pOriginalMyState->GetStun(),
			pCloneMyState->GetStun(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetStunResistance() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STUN_R ), 
			pCloneMyState->GetStunResistance()-pOriginalMyState->GetStunResistance(),
			pCloneMyState->GetStunResistance(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetCritical() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_CRITICAL_P ), 
			pCloneMyState->GetCritical()-pOriginalMyState->GetCritical(),
			pCloneMyState->GetCritical(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetCriticalResistance() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_CRITICAL_R ), 
			pCloneMyState->GetCriticalResistance()-pOriginalMyState->GetCriticalResistance(),
			pCloneMyState->GetCriticalResistance(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetMoveSpeed() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_MOVE ), 
			pCloneMyState->GetMoveSpeed()-pOriginalMyState->GetMoveSpeed(),
			pCloneMyState->GetMoveSpeed(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetSafeZoneMoveSpeed() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_SAFEZONEMOVE ), 
			pCloneMyState->GetSafeZoneMoveSpeed()-pOriginalMyState->GetSafeZoneMoveSpeed(),
			pCloneMyState->GetSafeZoneMoveSpeed(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetMaxHP() > 0 )
	{
		AddListBoxText(	2254, 
			(int)(pCloneMyState->GetMaxHP()-pOriginalMyState->GetMaxHP()),
			(int)pCloneMyState->GetMaxHP(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetMaxSP() > 0 )
	{
		AddListBoxText(	2255, 
			pCloneMyState->GetMaxSP()-pOriginalMyState->GetMaxSP(),
			pCloneMyState->GetMaxSP(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetRecoverySP() > 0 )
	{
		AddListBoxText(	2256, 
			pCloneMyState->GetRecoverySP()-pOriginalMyState->GetRecoverySP(),
			pCloneMyState->GetRecoverySP(), 
			ePOTENTIAL_VALUE );
	}

	// 슈퍼아머, 파이널데미지
	if( pNewItemState->GetSuperAmmor() > 0 )
	{
		AddListBoxText(	2279, 
			pCloneMyState->GetSuperAmmor()-pOriginalMyState->GetSuperAmmor(),
			pCloneMyState->GetSuperAmmor(), 
			ePOTENTIAL_VALUE );
	}

	if( pNewItemState->GetFinalDamage() > 0 )
	{
		AddListBoxText(	2280, 
			pCloneMyState->GetFinalDamage()-pOriginalMyState->GetFinalDamage(),
			pCloneMyState->GetFinalDamage(), 
			ePOTENTIAL_VALUE );
	}

	// 아이템 능력치 표시 두번째. Ratio관련.
	// 공격력/방어력	
	if( pNewItemState->GetAttackPMinRatio() > 0 || pNewItemState->GetAttackPMaxRatio() > 0 )
	{
		AddListBoxText(	5018, 
			pCloneMyState->GetAttackPMin()-pOriginalMyState->GetAttackPMin(),
			pCloneMyState->GetAttackPMin(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetAttackMMinRatio() > 0 || pNewItemState->GetAttackMMaxRatio() > 0 )
	{
		AddListBoxText(	5019, 
			pCloneMyState->GetAttackMMin()-pOriginalMyState->GetAttackMMin(),
			pCloneMyState->GetAttackMMin(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetDefensePRatio() > 0 )
	{
		AddListBoxText(	5020, 
			pCloneMyState->GetDefenseP()-pOriginalMyState->GetDefenseP(),
			pCloneMyState->GetDefenseP(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetDefenseMRatio() > 0 )
	{
		AddListBoxText(	5062, 
			pCloneMyState->GetDefenseM()-pOriginalMyState->GetDefenseM(),
			pCloneMyState->GetDefenseM(), 
			ePOTENTIAL_VALUE);
	}

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pNewItemState->GetStrengthRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STRENGTH ),
			pCloneMyState->GetStrength()-pOriginalMyState->GetStrength(),
			pCloneMyState->GetStrength(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetAgilityRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_AGILITY ),
			pCloneMyState->GetAgility()-pOriginalMyState->GetAgility(),
			pCloneMyState->GetAgility(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetIntelligenceRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_INTELLIGENCE ),
			pCloneMyState->GetIntelligence()-pOriginalMyState->GetIntelligence(),
			pCloneMyState->GetIntelligence(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetStaminaRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STAMINA ),
			pCloneMyState->GetStamina()-pOriginalMyState->GetStamina(),
			pCloneMyState->GetStamina(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetSpiritRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_SPIRIT ),
			pCloneMyState->GetSpirit()-pOriginalMyState->GetSpirit(),
			pCloneMyState->GetSpirit(), 
			ePOTENTIAL_VALUE);
	}

	// 내부 스탯 증가
	if( pNewItemState->GetStiffRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ),
			pCloneMyState->GetStiff()-pOriginalMyState->GetStiff(),
			pCloneMyState->GetStiff(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetStiffResistanceRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ),
			pCloneMyState->GetStiffResistance()-pOriginalMyState->GetStiffResistance(),
			pCloneMyState->GetStiffResistance(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetStunRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STUN_P ),
			pCloneMyState->GetStun()-pOriginalMyState->GetStun(),
			pCloneMyState->GetStun(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetStunResistanceRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_STUN_R ),
			pCloneMyState->GetStunResistance()-pOriginalMyState->GetStunResistance(),
			pCloneMyState->GetStunResistance(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetCriticalRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_CRITICAL_P ),
			pCloneMyState->GetCritical()-pOriginalMyState->GetCritical(),
			pCloneMyState->GetCritical(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetCriticalResistanceRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_CRITICAL_R ),
			pCloneMyState->GetCriticalResistance()-pOriginalMyState->GetCriticalResistance(),
			pCloneMyState->GetCriticalResistance(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetMoveSpeedRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_MOVE ),
			pCloneMyState->GetMoveSpeed()-pOriginalMyState->GetMoveSpeed(),
			pCloneMyState->GetMoveSpeed(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetSafeZoneMoveSpeedRatio() > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_SAFEZONEMOVE ),
			pCloneMyState->GetSafeZoneMoveSpeed()-pOriginalMyState->GetSafeZoneMoveSpeed(),
			pCloneMyState->GetSafeZoneMoveSpeed(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetMaxHPRatio() > 0 )
	{
		AddListBoxText(	2254,
			(int)(pCloneMyState->GetMaxHP()-pOriginalMyState->GetMaxHP()),
			(int)pCloneMyState->GetMaxHP(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetMaxSPRatio() > 0 )
	{
		AddListBoxText(	2255,
			pCloneMyState->GetMaxSP()-pOriginalMyState->GetMaxSP(),
			pCloneMyState->GetMaxSP(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetRecoverySPRatio() > 0 )
	{
		AddListBoxText(	2256,
			pCloneMyState->GetRecoverySP()-pOriginalMyState->GetRecoverySP(),
			pCloneMyState->GetRecoverySP(), 
			ePOTENTIAL_VALUE);
	}

	// 슈퍼아머, 파이널데미지
	if( pNewItemState->GetSuperAmmorRatio() > 0 )
	{
		AddListBoxText(	2279,
			pCloneMyState->GetSuperAmmor()-pOriginalMyState->GetSuperAmmor(),
			pCloneMyState->GetSuperAmmor(), 
			ePOTENTIAL_VALUE);
	}

	if( pNewItemState->GetFinalDamageRatio() > 0 )
	{
		AddListBoxText(	2280,
			pCloneMyState->GetFinalDamage()-pOriginalMyState->GetFinalDamage(),
			pCloneMyState->GetFinalDamage(), 
			ePOTENTIAL_VALUE);
	}

	// 속성 공격력/저항 만 별도의 Ratio가 없으므로 아래 출력한다.
	if( (pNewItemState->GetElementAttack( CDnState::Light ) * 100.f) > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_LIGHT_A ),
			(pCloneMyState->GetElementAttack( CDnState::Light ) * 100.f) - (pOriginalMyState->GetElementAttack( CDnState::Light ) * 100.f),
			(pCloneMyState->GetElementAttack( CDnState::Light ) * 100.f),
			ePOTENTIAL_VALUE);
	}

	if( (pNewItemState->GetElementAttack( CDnState::Dark ) * 100.f) > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_DARK_A ),
			(pCloneMyState->GetElementAttack( CDnState::Dark ) * 100.f) - (pOriginalMyState->GetElementAttack( CDnState::Dark ) * 100.f),
			(pCloneMyState->GetElementAttack( CDnState::Dark ) * 100.f),
			ePOTENTIAL_VALUE);
	}

	if( ( pNewItemState->GetElementAttack( CDnState::Fire ) * 100.f ) > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_FIRE_A ),
			(pCloneMyState->GetElementAttack( CDnState::Fire ) * 100.f) - (pOriginalMyState->GetElementAttack( CDnState::Fire ) * 100.f),
			(pCloneMyState->GetElementAttack( CDnState::Fire ) * 100.f),
			ePOTENTIAL_VALUE);
	}

	if( ( pNewItemState->GetElementAttack( CDnState::Ice ) * 100.f ) > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_ICE_A ),
			(pCloneMyState->GetElementAttack( CDnState::Ice ) * 100.f) - (pOriginalMyState->GetElementAttack( CDnState::Ice ) * 100.f),
			(pCloneMyState->GetElementAttack( CDnState::Ice ) * 100.f),
			ePOTENTIAL_VALUE);
	}

	if( (pNewItemState->GetElementDefense( CDnState::Light ) * 100.f) > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_LIGHT_R ),
			(pCloneMyState->GetElementAttack( CDnState::Light ) * 100.f) - (pOriginalMyState->GetElementAttack( CDnState::Light ) * 100.f),
			(pCloneMyState->GetElementAttack( CDnState::Light ) * 100.f),
			ePOTENTIAL_VALUE);
	}

	if( (pNewItemState->GetElementDefense( CDnState::Dark ) * 100.f) > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_DARK_R ),
			(pCloneMyState->GetElementAttack( CDnState::Dark ) * 100.f) - (pOriginalMyState->GetElementAttack( CDnState::Dark ) * 100.f),
			(pCloneMyState->GetElementAttack( CDnState::Dark ) * 100.f),
			ePOTENTIAL_VALUE);
	}

	if( (pNewItemState->GetElementDefense( CDnState::Fire ) * 100.f) > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_FIRE_R ),
			(pCloneMyState->GetElementAttack( CDnState::Fire ) * 100.f) - (pOriginalMyState->GetElementAttack( CDnState::Fire ) * 100.f),
			(pCloneMyState->GetElementAttack( CDnState::Fire ) * 100.f),
			ePOTENTIAL_VALUE);
	}

	if( (pNewItemState->GetElementDefense( CDnState::Ice ) * 100.f) > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_ICE_R ),
			(pCloneMyState->GetElementAttack( CDnState::Ice ) * 100.f) - (pOriginalMyState->GetElementAttack( CDnState::Ice ) * 100.f),
			(pCloneMyState->GetElementAttack( CDnState::Ice ) * 100.f),
			ePOTENTIAL_VALUE);
	}

	if( (pNewItemState->GetAddExp() * 100.0f) > 0 )
	{
		AddListBoxText(	ITEM::STATIC_2_INDEX( emTOOLTIP_STATIC::ITEM_ADDEXP ),
			(pCloneMyState->GetAddExp() * 100.0f) - (pOriginalMyState->GetAddExp() * 100.0f),
			(pCloneMyState->GetAddExp() * 100.f),
			ePOTENTIAL_VALUE);
	}
}

#endif //PRE_MOD_POTENTIAL_JEWEL_RENEWAL