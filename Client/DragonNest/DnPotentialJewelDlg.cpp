#include "StdAfx.h"
#include "DnPotentialJewelDlg.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "ItemSendPacket.h"
#include "DnPotentialJewel.h"
#include "DnTableDB.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL

#define RETURN_SELECT_OK_BTN   2030214
#define MESSAGEBOX_CHECK_MONEY 2030215

#include "DnTooltipDlg.h"
#include "DnInterfaceString.h"
#include "DnPotentialStatsDlg.h"
#include "DnPotentialCheckCancelDlg.h"
#include "DnMainDlg.h"
#include "DnCommonTask.h"
#include "TaskManager.h"
using namespace DN_INTERFACE::STRING;

CDnPotentialJewelDlg::CDnPotentialJewelDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pItem = NULL;
	m_pItemSlot = NULL;
	m_pButtonApply = NULL;
	m_pButtonQuickSlot = NULL;
	m_pButtonCharStats = NULL;
	m_pButtonPotentialApply = NULL;
	m_pCurrentDescription = NULL;
	m_pAtferDescription = NULL;
	m_pNotifyText = NULL;
	m_pPotentialItem = NULL;

	m_nSoundIdx = -1;
	m_cPrevPotentialIndex = 0;
	m_fElapsedTime = 1.0f;
	m_bIsPotentialApply = false;
	m_nCodeSerialNumber = 0;
	m_nPotentialID = 0;
	m_nPotentialCharge = 0;

	m_pDetailStatsDlg = NULL;
	m_pCheckCancleDlg = NULL;
	m_bEnoughCoin = false;

	memset( m_pGrayIcon, NULL, sizeof(m_pGrayIcon) );
}

CDnPotentialJewelDlg::~CDnPotentialJewelDlg()
{
	SAFE_DELETE( m_pCheckCancleDlg );
	SAFE_DELETE( m_pDetailStatsDlg );
	SAFE_DELETE( m_pItem );
	CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIdx );
}

void CDnPotentialJewelDlg::SetPotentialItemID( CDnItem* pItem )
{
	if(!pItem)
		return;

	if(pItem->IsCashItem())
		m_nPotentialID = pItem->GetSerialID();
	else
		m_nPotentialID = pItem->GetSlotIndex();
}

void CDnPotentialJewelDlg::Reset()
{
	m_pCurrentDescription->ClearText();
	m_pAtferDescription->ClearText();
	m_pButtonPotentialApply->Enable(true);
	m_pButtonCharStats->Enable(false);
	m_pNotifyText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9187));
	m_pButtonPotentialApply->Enable(true);
	
	m_fElapsedTime = 1.0f;
	m_bIsPotentialApply = false;
	m_nCodeSerialNumber = 0;
	m_bEnoughCoin = false;
	m_nPotentialCharge = 0;

	for(int i = 0 ; i < 3 ; ++i)
		m_pGrayIcon[i]->Show(false);

	if(m_pDetailStatsDlg)
		m_pDetailStatsDlg->Show(false);
	if(m_pCheckCancleDlg)
		m_pCheckCancleDlg->Show(false);
}

void CDnPotentialJewelDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemPowerDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10041 );
	if( strlen( szFileName ) > 0 )
		m_nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );

	m_pDetailStatsDlg = new CDnPotentialStatsDlg(UI_TYPE_CHILD, this);
	m_pDetailStatsDlg->Initialize(false);

	m_pCheckCancleDlg = new CDnPotentialCheckCancelDlg(UI_TYPE_CHILD_MODAL, this, 437, this);
	m_pCheckCancleDlg->Initialize(false);
}

void CDnPotentialJewelDlg::InitialUpdate()
{
	m_pItemSlot		= GetControl<CDnItemSlotButton>("ID_ITEM");
	m_pButtonApply	= GetControl<CEtUIButton>("ID_OK");
	m_pButtonApply->Enable(false);

	m_pButtonCharStats		 = GetControl<CEtUIButton>("ID_BT_STATUS");
	m_pButtonCharStats->Enable(false);
	m_pButtonPotentialApply	 = GetControl<CEtUIButton>("ID_BT_STE");
	m_pButtonPotentialApply->Enable(false);

	m_pCurrentDescription	 = GetControl<CEtUITextBox>("ID_TEXTBOX_OPTION0");
	m_pAtferDescription		 = GetControl<CEtUITextBox>("ID_TEXTBOX_OPTION1");

	m_pNotifyText			 = GetControl<CEtUIStatic>("ID_TEXT2");
	m_pNotifyText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9187)); // 잠재력을 부여할 아이템을 선택해 주세요

	m_pGrayIcon[0] = GetControl<CEtUIStatic>("ID_STATIC_WEAPON");
	m_pGrayIcon[1] = GetControl<CEtUIStatic>("ID_STATIC_BODY");
	m_pGrayIcon[2] = GetControl<CEtUIStatic>("ID_STATIC_RING");

	GetControl<CEtUIStatic>("ID_STATIC_EARRING")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_NECKLACE")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_BOOTS")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_LEG")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_GLOVE")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_HEAD")->Show(false);
}

void CDnPotentialJewelDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	// 서버에 None상태로 변환해줌.	
	CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
	if( pTask )			
		pTask->SendWindowState( bShow == true ? WINDOW_BLIND : WINDOW_NONE );

	CDnCustomDlg::Show( bShow );

	if( bShow )
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() )
			pInvenDlg->ShowTab( ST_INVENTORY );

		// 초기화
		Reset();

		// 카테고리에 맞는 이미지 선택
		if( m_pPotentialItem->GetCategoryType() == ITEM_CATEGORY_WEAPON )
			m_pGrayIcon[0]->Show(true);
		else if( m_pPotentialItem->GetCategoryType() == ITEM_CATEGORY_ARMOR )
			m_pGrayIcon[1]->Show(true);
		else if( m_pPotentialItem->GetCategoryType() == ITEM_CATEGORY_ACCESSORY )
			m_pGrayIcon[2]->Show(true);

		SetRenderPriority( this, true );
	}
	else
	{
		m_pItemSlot->ResetSlot();
		m_pButtonApply->Enable( false );

		if( m_pButtonQuickSlot )
		{
			m_pButtonQuickSlot->SetRegist( false );
			m_pButtonQuickSlot = NULL;
		}
	}
}

void CDnPotentialJewelDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	//focus::SetFocus();
	ShowChildDialog( m_pCheckCancleDlg, false );

	switch(nID)
	{
	case RETURN_SELECT_OK_BTN:
		{
			Show(false);
		}
		break;

	case MESSAGEBOX_CHECK_MONEY:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if(IsCmdControl("ID_YES"))
				{
					// 코드 아이템 사용.
					if(m_pButtonQuickSlot && m_pItem)
						SendPotentialJewel(m_pButtonQuickSlot->GetSlotIndex(), m_pItem->GetSerialID(), ( m_pPotentialItem->IsCashItem() == true ) ? m_pPotentialItem->GetSerialID() : m_pPotentialItem->GetSlotIndex());
				}
				else if(IsCmdControl("ID_NO"))
				{
				}
			}
		}
		break;
	}
}

bool CDnPotentialJewelDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			if( wParam == VK_RETURN )
			{
				//ProcessCommand(EVENT_BUTTON_CLICKED, true, GetControl("ID_OK") , 0);
				return false;
			}
			else if (wParam == VK_ESCAPE)
			{
				//ProcessCommand(EVENT_BUTTON_CLICKED, true, GetControl("ID_CANCEL") , 0);
				return false;
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

bool CDnPotentialJewelDlg::IsEnoughMoney()
{
	if(!m_pPotentialItem) return false;

	bool  bResult = false;
	INT64 nMyCoin = CDnItemTask::GetInstance().GetCoin();
	m_nPotentialCharge = ((CDnPotentialJewel*)m_pPotentialItem)->GetChargeCoin();

	if(nMyCoin < m_nPotentialCharge)
		bResult = true;
	
	m_bEnoughCoin = bResult;

	return bResult;
}

void CDnPotentialJewelDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" )) {
			m_pButtonApply->Enable( false );

			// 잠재력이 부여됬습니다. ( 메세지 박스 )
			GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9181), MB_OK, RETURN_SELECT_OK_BTN, this );
			
			// 잠재력이 부여됬습니다. ( 채팅창 )
			if(m_pItem)
			{
				int nUIStringID = 8054;
				//if( m_pItem->GetPotentialIndex() <= 0 ) nUIStringID = 8055;
				std::wstring str;
				str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nUIStringID), m_pItem->GetName());
				GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", str.c_str(), false);
			}
			Show(false);
		}
		else if(IsCmdControl("ID_CANCEL") || IsCmdControl("ID_BT_CLOSE")) 
		{
			if(m_bIsPotentialApply)
			{
				if(m_pPotentialItem)
				{
					m_pCheckCancleDlg->SetMoney((int)m_nPotentialCharge);
					ShowChildDialog( m_pCheckCancleDlg, true );
					//m_pCheckCancleDlg->Show(true);
				}
			}
			else
			{
				Show(false);
			}
		}
		else if( IsCmdControl("ID_BT_STATUS"))
		{
			if(m_pDetailStatsDlg)
				m_pDetailStatsDlg->Show( !m_pDetailStatsDlg->IsShow() );
		}
		else if( IsCmdControl("ID_BT_STE"))
		{
			//----------------------------------------------------------------------------------
			// 1. 여기서, 돈이 충분한지 체크를 하고 넘어간다.
			if(IsEnoughMoney())
			{
				// 돈이 부족해서, 잠재력을 보호할수 없습니다. 그래도 진행하시겠습니까? yes / no
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9188) , MB_YESNO, MESSAGEBOX_CHECK_MONEY, this);
				return;
			}

			//----------------------------------------------------------------------------------
			// 2. 소지금이 충분하므로 정상적으로 진행.
			// 2.0. 기존의 잠재력이 있다면, 인덱스를 저장해둔다.
			CDnItem *pCurrentItem = static_cast<CDnItem *>(m_pItemSlot->GetItem());
			if(pCurrentItem) m_cPrevPotentialIndex = pCurrentItem->GetPotentialIndex(); // 임시로 잠재력 아이템의 Index 저장.

			// 2.2. 서버에 보옥코드 사용했다고 패킷 날리기.			
			if(m_pButtonQuickSlot && m_pItem)
			{
				SetPotentialItemID(m_pPotentialItem);
				SendPotentialJewel(m_pButtonQuickSlot->GetSlotIndex(), m_pItem->GetSerialID(), ( m_pPotentialItem->IsCashItem() == true ) ? m_pPotentialItem->GetSerialID() : m_pPotentialItem->GetSlotIndex());
				m_pButtonPotentialApply->Enable(false);
			}
		}
		else if( IsCmdControl("ID_ITEM") )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			CDnSlotButton *pPressedButton = ( CDnItemSlotButton * )pControl;

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				// 잠재력이 새로 적용한 상태에는 우클릭해제를 막는다.
				if(m_bIsPotentialApply)
					return;

				Reset();
				SAFE_DELETE(m_pItem);
				pPressedButton->ResetSlot();
				if( m_pButtonQuickSlot )
				{
					m_pButtonQuickSlot->SetRegist( false );
					m_pButtonQuickSlot = NULL;
				}

				// 카테고리에 맞는 이미지 선택
				if(m_pPotentialItem)
				{
					if( m_pPotentialItem->GetCategoryType() == ITEM_CATEGORY_WEAPON )
						m_pGrayIcon[0]->Show(true);
					else if( m_pPotentialItem->GetCategoryType() == ITEM_CATEGORY_ARMOR )
						m_pGrayIcon[1]->Show(true);
					else if( m_pPotentialItem->GetCategoryType() == ITEM_CATEGORY_ACCESSORY )
						m_pGrayIcon[2]->Show(true);
				}
				return;
			}

			if( pDragButton )
			{
				drag::ReleaseControl();

				// 잠재력이 새로 적용한 상태에는 막는다.
				if(m_bIsPotentialApply)
					return;

				if( pDragButton->GetItemType() != MIInventoryItem::Item )
					return;

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if( pItem == NULL || IsPotentialableItem( pDragButton ) == false )
				{
					pDragButton->DisableSplitMode( true );
					return;
				}

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if( pPressedItem )
				{
					SAFE_DELETE( m_pItem );
					pPressedButton->ResetSlot();
					m_pButtonQuickSlot->SetRegist( false );
					m_pButtonQuickSlot = NULL;
				}

				TItemInfo itemInfo;
				pItem->GetTItemInfo( itemInfo );
				m_pItem = GetItemTask().CreateItem( itemInfo );
				pPressedButton->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

				CheckPotentialItem();
				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

				m_pButtonQuickSlot = (CDnQuickSlotButton *)pDragButton;
				m_pButtonQuickSlot->SetRegist( true );

				// 현재 잠재력 표시
				SetPotentialDescription(); 

				// 잠재력 부여 버튼 활성화
				m_pButtonPotentialApply->Enable(true);
				
				// 잠재력을 부여 하시겠습니까?
				m_pNotifyText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9183));
			}
			return;
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_POTENTIAL);
		}
#endif
	}
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPotentialJewelDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnPotentialJewelDlg::SetPotentialDescription()
{
	if(!m_pItem) return;
	
	CDnState* pItemState = m_pItem->GetPotentialState();
	if(pItemState)
	{
		CheckCurrentPotentialState(pItemState, m_pCurrentDescription);
	}
	else
		m_pCurrentDescription->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9196));
}

void CDnPotentialJewelDlg::CheckCurrentPotentialState(CDnState* pStats, CEtUITextBox* pTextBox)
{
	if(!pStats) return;
	wchar_t szTemp[256]={0};

	// 공격력/방어력
	if( pStats->GetAttackPMin() > 0 || pStats->GetAttackPMax() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d%s%d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5018 ), pStats->GetAttackPMin(), TILDE, pStats->GetAttackPMax() );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetAttackMMin() > 0 || pStats->GetAttackMMax() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d%s%d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5019 ), pStats->GetAttackMMin(), TILDE, pStats->GetAttackMMax() );
		pTextBox->AddText(szTemp);
	}

	if( pStats->GetDefenseP() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5020 ), pStats->GetDefenseP() );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetDefenseM() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5062 ), pStats->GetDefenseM() );
		pTextBox->AddText(szTemp);
	}

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pStats->GetStrength() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STRENGTH ), pStats->GetStrength() );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetAgility() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_AGILITY ), pStats->GetAgility() );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetIntelligence() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_INTELLIGENCE ), pStats->GetIntelligence() );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetStamina() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STAMINA ), pStats->GetStamina() );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetSpirit() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SPIRIT ), pStats->GetSpirit() );
		pTextBox->AddText(szTemp);
	}

	int nTemp(0);
	INT64 nTempHP = 0;
	// 내부 스탯 증가
	nTemp = pStats->GetStiff();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ), nTemp );
		pTextBox->AddText(szTemp);
	}

	nTemp = pStats->GetStiffResistance();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ), nTemp );
		pTextBox->AddText(szTemp);
	}

	nTemp = pStats->GetStun();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_P ), nTemp );
		pTextBox->AddText(szTemp);
	}

	nTemp = pStats->GetStunResistance();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_R ), nTemp );
		pTextBox->AddText(szTemp);
	}

	nTemp = pStats->GetCritical();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_P ), nTemp );
		pTextBox->AddText(szTemp);
	}

	nTemp = pStats->GetCriticalResistance();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_R ), nTemp );
		pTextBox->AddText(szTemp);
	}


	nTemp = pStats->GetMoveSpeed();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_MOVE ), nTemp );
		pTextBox->AddText(szTemp);
	}

	nTemp = pStats->GetSafeZoneMoveSpeed();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SAFEZONEMOVE ), nTemp );
		pTextBox->AddText(szTemp);
	}

	nTempHP = pStats->GetMaxHP();
	if( nTempHP > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %I64d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2254 ), nTempHP );
		pTextBox->AddText(szTemp);
	}

	nTemp = pStats->GetMaxSP();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2255 ), nTemp );
		pTextBox->AddText(szTemp);
	}

	nTemp = pStats->GetRecoverySP();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2256 ), nTemp );
		pTextBox->AddText(szTemp);
	}

	// 슈퍼아머, 파이널데미지
	nTemp = pStats->GetSuperAmmor();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2279 ), nTemp );
		pTextBox->AddText(szTemp);
	}
	nTemp = pStats->GetFinalDamage();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2280 ), nTemp );
		pTextBox->AddText(szTemp);
	}

	// 아이템 능력치 표시 두번째. Ratio관련.
	// 공격력/방어력
	if( pStats->GetAttackPMinRatio() > 0 || pStats->GetAttackPMaxRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%%s%.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5018 ), pStats->GetAttackPMinRatio() * 100.0f, TILDE, pStats->GetAttackPMaxRatio() * 100.0f );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetAttackMMinRatio() > 0 || pStats->GetAttackMMaxRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%%s%.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5019 ), pStats->GetAttackMMinRatio() * 100.0f, TILDE, pStats->GetAttackMMaxRatio() * 100.0f );
		pTextBox->AddText(szTemp);
	}

	if( pStats->GetDefensePRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5020 ), pStats->GetDefensePRatio() * 100.0f );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetDefenseMRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5062 ), pStats->GetDefenseMRatio() * 100.0f );
		pTextBox->AddText(szTemp);
	}

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pStats->GetStrengthRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STRENGTH ), pStats->GetStrengthRatio() * 100.0f );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetAgilityRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_AGILITY ), pStats->GetAgilityRatio() * 100.0f );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetIntelligenceRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_INTELLIGENCE ), pStats->GetIntelligenceRatio() * 100.0f );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetStaminaRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STAMINA ), pStats->GetStaminaRatio() * 100.0f );
		pTextBox->AddText(szTemp);
	}
	if( pStats->GetSpiritRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SPIRIT ), pStats->GetSpiritRatio() * 100.0f );
		pTextBox->AddText(szTemp);
	}

	float fTemp(0);

	// 내부 스탯 증가
	fTemp = pStats->GetStiffRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetStiffResistanceRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetStunRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_P ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetStunResistanceRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_R ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetCriticalRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_P ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetCriticalResistanceRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_R ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetMoveSpeedRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_MOVE ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetSafeZoneMoveSpeedRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SAFEZONEMOVE ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetMaxHPRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2254 ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetMaxSPRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2255 ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetRecoverySPRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2256 ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	// 슈퍼아머, 파이널데미지
	fTemp = pStats->GetSuperAmmorRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2279 ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	fTemp = pStats->GetFinalDamageRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2280 ), fTemp * 100.0f );
		pTextBox->AddText(szTemp);
	}

	// 속성 공격력/저항 만 별도의 Ratio가 없으므로 아래 출력한다.
	//
	fTemp = ( pStats->GetElementAttack( CDnState::Light ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_LIGHT_A ), fTemp );
		pTextBox->AddText(szTemp);
	}

	fTemp = ( pStats->GetElementAttack( CDnState::Dark ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DARK_A ), fTemp );
		pTextBox->AddText(szTemp);
	}

	fTemp = ( pStats->GetElementAttack( CDnState::Fire ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_FIRE_A ), fTemp );
		pTextBox->AddText(szTemp);
	}

	fTemp = ( pStats->GetElementAttack( CDnState::Ice ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ICE_A ), fTemp );
		pTextBox->AddText(szTemp);
	}


	fTemp = ( pStats->GetElementDefense( CDnState::Light ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_LIGHT_R ), fTemp );
		pTextBox->AddText(szTemp);
	}

	fTemp = ( pStats->GetElementDefense( CDnState::Dark ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DARK_R ), fTemp );
		pTextBox->AddText(szTemp);
	}

	fTemp = ( pStats->GetElementDefense( CDnState::Fire ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_FIRE_R ), fTemp );
		pTextBox->AddText(szTemp);
	}

	fTemp = ( pStats->GetElementDefense( CDnState::Ice ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ICE_R ), fTemp );
		pTextBox->AddText(szTemp);
	}

	fTemp = (pStats->GetAddExp() * 100.0f);
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ADDEXP ), fTemp );	// UIString 필요.
		pTextBox->AddText(szTemp);
	}
}

void CDnPotentialJewelDlg::SetPotentialItem( CDnItem *pItem )
{
	m_pPotentialItem = pItem;
	if( m_pPotentialItem && m_pPotentialItem->GetItemType() != ITEMTYPE_POTENTIAL_JEWEL ) {
		m_pPotentialItem = NULL;
		return;
	}
}

void CDnPotentialJewelDlg::SetPotentialItem( CDnQuickSlotButton *pPressedButton )
{// 아이템 올릴때

	// 새로운 잠재력이 등록된 상태에, 새로운 아이템을 올리지 못한다.
	if(m_bIsPotentialApply)
		return;

	if( pPressedButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8051 ), MB_OK );
		return;
	}

	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pButtonQuickSlot && (pItem == m_pButtonQuickSlot->GetItem()) )
	{
		return;
	}

	// 아이템 종류 검사
	switch( pItem->GetItemType() ) {
		case ITEMTYPE_WEAPON:
		case ITEMTYPE_PARTS:
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
			return;
	}
	if( pItem->GetTypeParam(1) == 0 ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
	}

	int nApplyApplicableValue = ((CDnPotentialJewel*)m_pPotentialItem)->GetApplyApplicableValue();
	int nCurApplicableValue = pItem->GetApplicableValue();
	if( ( nCurApplicableValue & nApplyApplicableValue ) != nCurApplicableValue ) {
		GetInterface().MessageBox( 8051, MB_OK ); // 적용할수없는 아이템입니다.
		return;
	}
	bool bErase = ((CDnPotentialJewel*)m_pPotentialItem)->IsErasable();
	//if( !bErase && pItem->GetPotentialIndex() > 0 ) {
	//	GetInterface().MessageBox( 8052, MB_OK ); // 잠재력이 적용 된 아이템입니다.
	////	return;
	//}
	if( bErase && pItem->GetPotentialIndex() <= 0 ) {
		GetInterface().MessageBox( 8051, MB_OK );// 적용할수없는 아이템입니다.
		return;
	}
	if( ((CDnPotentialJewel*)m_pPotentialItem)->IsCanApplySealedItem() == false )
	{
		if( pItem->IsSoulbBound() == false )
		{
			GetInterface().MessageBox( 8051, MB_OK );// 적용할수없는 아이템입니다.
			return;
		}
	}

	Reset();
	
	m_pNotifyText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9183)); // 잠재력을 부여 하시겠습니까?

	CDnItem *pOriginItem = static_cast<CDnItem *>(m_pItemSlot->GetItem());
	if( pOriginItem )
	{
		SAFE_DELETE( m_pItem );
		m_pItemSlot->ResetSlot();
		m_pButtonQuickSlot->SetRegist( false );
		m_pButtonQuickSlot = NULL;
	}

	TItemInfo itemInfo;
	pItem->GetTItemInfo( itemInfo );
	m_pItem = GetItemTask().CreateItem( itemInfo );
	m_pItemSlot->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

	//CheckPotentialItem();
	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_pButtonQuickSlot = (CDnQuickSlotButton *)pPressedButton;
	m_pButtonQuickSlot->SetRegist( true );

	SetPotentialDescription(); // 현재 잠재력 표시
	m_pButtonPotentialApply->Enable(true); // 잠재력 부여버튼 활성화
}


bool CDnPotentialJewelDlg::IsPotentialableItem( CDnSlotButton *pDragButton )
{
	if( pDragButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8051 ), MB_OK );
		return false;
	}

	CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pButtonQuickSlot && (pItem == m_pButtonQuickSlot->GetItem()) )
	{
		return false;
	}

	// 아이템 종류 검사
	switch( pItem->GetItemType() ) {
		case ITEMTYPE_WEAPON:
		case ITEMTYPE_PARTS:
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
			return false;
	}
	if( pItem->GetTypeParam(1) == 0 ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}

	int nApplyApplicableValue = ((CDnPotentialJewel*)m_pPotentialItem)->GetApplyApplicableValue();
	int nCurApplicableValue = pItem->GetApplicableValue();
	if( ( nCurApplicableValue & nApplyApplicableValue ) != nCurApplicableValue ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}
	bool bErase = ((CDnPotentialJewel*)m_pPotentialItem)->IsErasable();
	//if( !bErase && pItem->GetPotentialIndex() > 0 ) {
	//	GetInterface().MessageBox( 8052, MB_OK ); // 잠재력이 적용된 아이템입니다.
	//	return false;
	//}
	if( bErase && pItem->GetPotentialIndex() <= 0 ) {
		GetInterface().MessageBox( 8051, MB_OK ); // 적용할수 없는 아이템입니다.
		return false;
	}
	if( ((CDnPotentialJewel*)m_pPotentialItem)->IsCanApplySealedItem() == false )
	{
		if( pItem->IsSoulbBound() == false ) // 귀속
		{
			GetInterface().MessageBox( 8051, MB_OK ); // 적용할수 없는 아이템입니다.
			return false;
		}
	}

	return true;
}

void CDnPotentialJewelDlg::CheckPotentialItem()
{
	if( m_pItemSlot && !m_pItemSlot->IsEmptySlot() && m_pItem )
	{
		m_pButtonApply->Enable( true );
	}
	else
	{
		m_pButtonApply->Enable( false );
	}
}


void CDnPotentialJewelDlg::OnRecvPotentialItemRollBack( int nResult, int nSlotIndex )
{
	if( nResult == ERROR_NONE )
	{
		// 잠재력이 보호되었습니다.
		//GetInterface().MessageBox( 9181, MB_OK );
		GetInterface().MessageBox( 9189, MB_OK );
	}
	else
	{
		// 제작에 실패하였습니다.
		GetInterface().MessageBox( 7011, MB_OK );
	}

	Show(false);
}

void CDnPotentialJewelDlg::OnRecvPotentialItem( int nResult, int nSlotIndex )
{	
	// 잠재력 부여후 리턴함수
	// 1초 딜레이....
	if( nResult == ERROR_NONE )
	{
		if(m_bEnoughCoin)
		{
			// 돈이 부족한 유저. 바로 적용시킨다.
			ProcessCommand(EVENT_BUTTON_CLICKED, true, GetControl<CEtUIButton>("ID_OK"), 0);
		}
		else
		{
			// 1. after잠재력 텍스트박스에 표시.
			CDnItem *pItem = GetItemTask().GetCharInventory().GetItem( nSlotIndex );
			if( !pItem || !m_pItem || pItem->GetClassID() != m_pItem->GetClassID() )
				return;

			CDnState* pItemState = pItem->GetPotentialState();
			if(pItemState)
				CheckCurrentPotentialState(pItemState, m_pAtferDescription);

			TItemInfo itemInfo;
			pItem->GetTItemInfo( itemInfo );
			m_pItem = GetItemTask().CreateItem( itemInfo );
			m_pItemSlot->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

			// 4. 적용버튼 & 능력치 보기 버튼 활성.
			m_pButtonApply->Enable(true);
			m_pButtonCharStats->Enable(true); // 캐릭터 능력치 버튼 활성화
			m_pButtonPotentialApply->Enable(false);
			m_bIsPotentialApply = true;

			// 5. 사운드 플레이
			if (m_nSoundIdx >= 0)
				CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIdx, false );

			m_pNotifyText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9180)); // 새로운 [잠재능력]을 적용 시키겠습니까?
		}
	}	
}


#else // PRE_MOD_POTENTIAL_JEWEL_RENEWAL



CDnPotentialJewelDlg::CDnPotentialJewelDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pItem = NULL;
	m_pItemSlot = NULL;
	m_pStaticText = NULL;
	m_pButtonApply = NULL;
	m_pButtonQuickSlot = NULL;

	m_pPotentialItem = NULL;
	m_nSoundIdx = -1;
}

CDnPotentialJewelDlg::~CDnPotentialJewelDlg()
{
	SAFE_DELETE( m_pItem );
	CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIdx );
}

void CDnPotentialJewelDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemPowerDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10041 );
	if( strlen( szFileName ) > 0 )
		m_nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
}

void CDnPotentialJewelDlg::InitialUpdate()
{
	m_pItemSlot = GetControl<CDnItemSlotButton>("ID_ITEM");
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT_ASK");
	m_pButtonApply = GetControl<CEtUIButton>("ID_OK");
}

void CDnPotentialJewelDlg::Show( bool bShow )
{
	CDnCustomDlg::Show( bShow );

	if( bShow )
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() )
			pInvenDlg->ShowTab( ST_INVENTORY );

		WCHAR wszText[256] = {0,};
		swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8053), m_pPotentialItem->GetName() );
		m_pStaticText->SetText( wszText );

		SetRenderPriority( this, true );
	}
	else
	{
		m_pItemSlot->ResetSlot();
		m_pStaticText->SetText( L"" );

		m_pButtonApply->Enable( false );

		if( m_pButtonQuickSlot )
		{
			m_pButtonQuickSlot->SetRegist( false );
			m_pButtonQuickSlot = NULL;
		}
	}
}

void CDnPotentialJewelDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) ) {
			if( !m_pButtonQuickSlot || !m_pPotentialItem ) return;
			SendPotentialJewel( m_pButtonQuickSlot->GetSlotIndex(), m_pItem->GetSerialID(), ( m_pPotentialItem->IsCashItem() == true ) ? m_pPotentialItem->GetSerialID() : m_pPotentialItem->GetSlotIndex() );
			m_pButtonApply->Enable( false );
		}
		else if( IsCmdControl( "ID_CANCEL" ) ) {
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_CLOSE"))
		{
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_ITEM") )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			CDnSlotButton *pPressedButton = ( CDnItemSlotButton * )pControl;

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				SAFE_DELETE(m_pItem);
				pPressedButton->ResetSlot();
				if( m_pButtonQuickSlot )
				{
					m_pButtonQuickSlot->SetRegist( false );
					m_pButtonQuickSlot = NULL;
				}
				CheckPotentialItem();
				return;
			}

			if( pDragButton )
			{
				drag::ReleaseControl();

				if( pDragButton->GetItemType() != MIInventoryItem::Item )
					return;

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if( pItem == NULL || IsPotentialableItem( pDragButton ) == false )
				{
					pDragButton->DisableSplitMode( true );
					return;
				}

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if( pPressedItem )
				{
					SAFE_DELETE( m_pItem );
					pPressedButton->ResetSlot();
					m_pButtonQuickSlot->SetRegist( false );
					m_pButtonQuickSlot = NULL;
				}

				TItemInfo itemInfo;
				pItem->GetTItemInfo( itemInfo );
				m_pItem = GetItemTask().CreateItem( itemInfo );
				pPressedButton->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

				CheckPotentialItem();
				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

				m_pButtonQuickSlot = (CDnQuickSlotButton *)pDragButton;
				m_pButtonQuickSlot->SetRegist( true );
			}
			return;
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_POTENTIAL);
		}
#endif
	}
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

}

void CDnPotentialJewelDlg::SetPotentialItem( CDnQuickSlotButton *pPressedButton )
{
	if( pPressedButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8051 ), MB_OK );
		return;
	}

	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pButtonQuickSlot && (pItem == m_pButtonQuickSlot->GetItem()) )
	{
		return;
	}

	// 아이템 종류 검사
	switch( pItem->GetItemType() ) {
		case ITEMTYPE_WEAPON:
		case ITEMTYPE_PARTS:
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
			return;
	}
	if( pItem->GetTypeParam(1) == 0 ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
	}

	int nApplyApplicableValue = ((CDnPotentialJewel*)m_pPotentialItem)->GetApplyApplicableValue();
	int nCurApplicableValue = pItem->GetApplicableValue();
	if( ( nCurApplicableValue & nApplyApplicableValue ) != nCurApplicableValue ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
	}
	bool bErase = ((CDnPotentialJewel*)m_pPotentialItem)->IsErasable();
	if( !bErase && pItem->GetPotentialIndex() > 0 ) {
		GetInterface().MessageBox( 8052, MB_OK );
		return;
	}
	if( bErase && pItem->GetPotentialIndex() <= 0 ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
	}
	if( ((CDnPotentialJewel*)m_pPotentialItem)->IsCanApplySealedItem() == false )
	{
		if( pItem->IsSoulbBound() == false )
		{
			GetInterface().MessageBox( 8051, MB_OK );
			return;
		}
	}

	CDnItem *pOriginItem = static_cast<CDnItem *>(m_pItemSlot->GetItem());
	if( pOriginItem )
	{
		SAFE_DELETE( m_pItem );
		m_pItemSlot->ResetSlot();
		m_pButtonQuickSlot->SetRegist( false );
		m_pButtonQuickSlot = NULL;
	}

	TItemInfo itemInfo;
	pItem->GetTItemInfo( itemInfo );
	m_pItem = GetItemTask().CreateItem( itemInfo );
	m_pItemSlot->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

	CheckPotentialItem();
	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_pButtonQuickSlot = (CDnQuickSlotButton *)pPressedButton;
	m_pButtonQuickSlot->SetRegist( true );
}


bool CDnPotentialJewelDlg::IsPotentialableItem( CDnSlotButton *pDragButton )
{
	if( pDragButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8051 ), MB_OK );
		return false;
	}

	CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pButtonQuickSlot && (pItem == m_pButtonQuickSlot->GetItem()) )
	{
		return false;
	}

	// 아이템 종류 검사
	switch( pItem->GetItemType() ) {
		case ITEMTYPE_WEAPON:
		case ITEMTYPE_PARTS:
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
			return false;
	}
	if( pItem->GetTypeParam(1) == 0 ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}

	int nApplyApplicableValue = ((CDnPotentialJewel*)m_pPotentialItem)->GetApplyApplicableValue();
	int nCurApplicableValue = pItem->GetApplicableValue();
	if( ( nCurApplicableValue & nApplyApplicableValue ) != nCurApplicableValue ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}
	bool bErase = ((CDnPotentialJewel*)m_pPotentialItem)->IsErasable();
	if( !bErase && pItem->GetPotentialIndex() > 0 ) {
		GetInterface().MessageBox( 8052, MB_OK );
		return false;
	}
	if( bErase && pItem->GetPotentialIndex() <= 0 ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}
	if( ((CDnPotentialJewel*)m_pPotentialItem)->IsCanApplySealedItem() == false )
	{
		if( pItem->IsSoulbBound() == false )
		{
			GetInterface().MessageBox( 8051, MB_OK );
			return false;
		}
	}

	return true;
}

void CDnPotentialJewelDlg::CheckPotentialItem()
{
	if( m_pItemSlot && !m_pItemSlot->IsEmptySlot() && m_pItem )
	{
		m_pButtonApply->Enable( true );
	}
	else
	{
		m_pButtonApply->Enable( false );
	}
}

void CDnPotentialJewelDlg::SetPotentialItem( CDnItem *pItem )
{
	m_pPotentialItem = pItem;
	if( m_pPotentialItem && m_pPotentialItem->GetItemType() != ITEMTYPE_POTENTIAL_JEWEL ) {
		m_pPotentialItem = NULL;
		return;
	}
}


void CDnPotentialJewelDlg::OnRecvPotentialItem( int nResult, int nSlotIndex )
{
	if( nResult == ERROR_NONE )
	{
		CDnItem *pItem = GetItemTask().GetCharInventory().GetItem( nSlotIndex );
		if( !pItem || !m_pItem || pItem->GetClassID() != m_pItem->GetClassID() )
			return;

		if (m_nSoundIdx >= 0)
			CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIdx, false );
	}

	SAFE_DELETE(m_pItem);
	m_pItemSlot->ResetSlot();

	if( m_pButtonQuickSlot )
	{
		m_pButtonQuickSlot->SetRegist( false );
		m_pButtonQuickSlot = NULL;
	}

	// 창을 닫아버리는게 더 편한 듯
	Show( false );
}
#endif // // PRE_MOD_POTENTIAL_JEWEL_RENEWAL