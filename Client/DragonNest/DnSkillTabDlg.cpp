#include "StdAfx.h"
#include "DnSkillTabDlg.h"
#include "DnSkillDlg.h"
#include "DnMainMenuDlg.h"
#include "DnItemTask.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"
#include "DnSkillStoreTabDlg.h"
#include "DnSkillRadioMsgDlg.h"
#include "DnSkillGestureDlg.h"
#include "DnPlayerActor.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnSkillTabDlg::CDnSkillTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pActiveSkillTabButton(NULL)
	, m_pPassiveSkillTabButton(NULL)
	, m_pRadioMsgSkillTabButton(NULL)
	, m_pGestureSkillTabButton(NULL)
	, m_pActiveSkillDlg(NULL)
	, m_pPassiveSkillDlg(NULL)
	, m_pRadioMsgSkillDlg(NULL)
	, m_pGestureSkillDlg(NULL)
	, m_pStaticMoney(NULL)
	, m_pStaticGold(NULL)
	, m_pStaticSilver(NULL)
	, m_pStaticBronze(NULL)
{
}

CDnSkillTabDlg::~CDnSkillTabDlg(void)
{
}

void CDnSkillTabDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillTabDlg.ui" ).c_str(), bShow );
}

void CDnSkillTabDlg::InitialUpdate()
{
	m_pActiveSkillTabButton = GetControl<CEtUIRadioButton>("ID_TAB_ACTIVE");
	m_pActiveSkillDlg = new CDnSkillDlg( UI_TYPE_CHILD, this );
	m_pActiveSkillDlg->Initialize( false );
	AddTabDialog( m_pActiveSkillTabButton, m_pActiveSkillDlg );

	m_pPassiveSkillTabButton = GetControl<CEtUIRadioButton>("ID_TAB_PASSIVE");
	m_pPassiveSkillDlg = new CDnSkillDlg( UI_TYPE_CHILD, this );
	m_pPassiveSkillDlg->Initialize( false );
	AddTabDialog( m_pPassiveSkillTabButton, m_pPassiveSkillDlg );

	m_pGestureSkillTabButton = GetControl<CEtUIRadioButton>("ID_TAB_GESTURE");
	m_pGestureSkillDlg= new CDnSkillGestureDlg( UI_TYPE_CHILD, this );
	m_pGestureSkillDlg->Initialize( false );
	AddTabDialog( m_pGestureSkillTabButton, m_pGestureSkillDlg );

	m_pRadioMsgSkillTabButton = GetControl<CEtUIRadioButton>("ID_TAB_RADIOMSG");
	m_pRadioMsgSkillTabButton->Show( false );

	SetCheckedTab( m_pActiveSkillTabButton->GetTabID() );

	m_pStaticMoney = GetControl<CEtUIStatic>("ID_STATIC_MONEY");
	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD");
	m_pStaticSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pStaticBronze = GetControl<CEtUIStatic>("ID_BRONZE");

	m_pStaticMoney->Show( false );
	m_pStaticGold->Show( false );
	m_pStaticSilver->Show( false );
	m_pStaticBronze->Show( false );
}

void CDnSkillTabDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CDnSkillStoreTabDlg *pSkillStoreDlg = (CDnSkillStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_STORE_DIALOG );
		if( pSkillStoreDlg && pSkillStoreDlg->IsShow() )
		{
			m_pStaticMoney->Show( true );
			m_pStaticGold->Show( true );
			m_pStaticSilver->Show( true );
			m_pStaticBronze->Show( true );
		}
	}
	else
	{
		if( drag::IsValid() )
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			if( pDragButton->GetSlotType() == ST_SKILL )
			{
				drag::ReleaseControl();
			}
		}

		if( m_pStaticMoney ) m_pStaticMoney->Show( false );
		if( m_pStaticGold ) m_pStaticGold->Show( false );
		if( m_pStaticSilver ) m_pStaticSilver->Show( false );
		if( m_pStaticBronze ) m_pStaticBronze->Show( false );
	}

	CEtUITabDialog::Show( bShow );
//	CDnMouseCursor::GetInstance().ShowCursor( bShow );

	if( m_pCallback )
	{
		m_pCallback->OnUICallbackProc( GetDialogID(), bShow?EVENT_DIALOG_SHOW:EVENT_DIALOG_HIDE, NULL );
	}
}

void CDnSkillTabDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		INT64 nCoin = GetItemTask().GetCoin();
		INT64 nGold = nCoin/10000;
		INT64 nSilver = (nCoin%10000)/100;
		INT64 nBronze = nCoin%100;
		m_pStaticGold->SetInt64ToText( nGold );
		m_pStaticSilver->SetInt64ToText( nSilver );
		m_pStaticBronze->SetInt64ToText( nBronze );

		// 전투모드에 따른 탭 활성화 처리.
		CDnSkillStoreTabDlg *pSkillStoreDlg = (CDnSkillStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_STORE_DIALOG );
		if( pSkillStoreDlg && pSkillStoreDlg->IsShow() )
		{
		}
		else
		{
			// 스킬창만 열려있을땐 현재 배틀모드에 따라 탭을 바꾼다.
			if( CDnActor::s_hLocalActor )
			{
				CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				bool bBattleMode = pActor->IsBattleMode();

				if( !bBattleMode )
				{
					// 평화모드에서 제스처가 아니면 전환
					if( GetCurrentTabID() != m_pGestureSkillTabButton->GetTabID() )
					{
						SetCheckedTab( m_pGestureSkillTabButton->GetTabID() );
						//m_dwPrevQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
						//m_QuickSlgDlgGroup.ShowDialog( LIFESKILL_DLG, true );
						//m_pStaticSlotNumber->SetText(L"G");
					}
				}
				else if( GetCurrentTabID() == m_pGestureSkillTabButton->GetTabID() )
				{
					// 전투모드에서 제스처가 보이면 전환
					SetCheckedTab( m_pActiveSkillTabButton->GetTabID() );
				}
			}
		}
	}
}

void CDnSkillTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_RADIOBUTTON_CHANGED && bTriggeredByUser )
	{
		CDnSkillStoreTabDlg *pSkillStoreDlg = (CDnSkillStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_STORE_DIALOG );
		if( pSkillStoreDlg && pSkillStoreDlg->IsShow() ) {
		}
		else {
			if( IsCmdControl("ID_TAB_ACTIVE") || IsCmdControl("ID_TAB_PASSIVE") ) {
				CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor*>((CDnActor*)CDnActor::s_hLocalActor);
				if( pPlayer && !pPlayer->IsDie() && ( pPlayer->IsStay() || pPlayer->IsMove() ) )
				{
					if( pPlayer->IsCanBattleMode() )
					{
						if( pPlayer->IsMove() ) pPlayer->CmdStop( "Stand" );
						pPlayer->CmdToggleBattle( true );
					}
				}
			}
			else if( IsCmdControl("ID_TAB_GESTURE") ) {
				CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor*>((CDnActor*)CDnActor::s_hLocalActor);
				if( pPlayer && !pPlayer->IsDie() && ( pPlayer->IsStay() || pPlayer->IsMove() ) )
				{
					if( pPlayer->IsMove() ) pPlayer->CmdStop( "Stand" );
					pPlayer->CmdToggleBattle(false);
				}
			}
		}
	}

	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSkillTabDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnSkillTabDlg::SetItem, pItem is NULL!");
	
	//CDnSkillDlg *pSkillDlg(NULL);
	//CDnSkill *pSkill = static_cast<CDnSkill*>(pItem);
	//
	//switch( pSkill->GetSkillType() )
	//{
	//case CDnSkill::Active:
	//	pSkillDlg = static_cast<CDnSkillDlg*>(m_groupTabDialog.GetDialog(m_pActiveSkillTabButton->GetTabID()));
	//	break;
	//case CDnSkill::Passive:
	//	pSkillDlg = static_cast<CDnSkillDlg*>(m_groupTabDialog.GetDialog(m_pPassiveSkillTabButton->GetTabID()));
	//	break;
	//default:
	//	ASSERT(0&&"CDnSkillTabDlg::SetItem");
	//	break;
	//}

	//if( pSkillDlg )
	//{
	//	pSkillDlg->SetItem( pItem );
	//}
}

void CDnSkillTabDlg::OnSkillLevelUp( bool bSuccessed )
{
	//if( GetCurrentTabID() == m_pActiveSkillTabButton->GetTabID() || GetCurrentTabID() == m_pPassiveSkillTabButton->GetTabID() )
	//{
	//	CDnSkillDlg *pSkillDlg = static_cast<CDnSkillDlg*>(m_groupTabDialog.GetShowDialog());
	//	if( pSkillDlg ) pSkillDlg->OnSkillLevelUp( bSuccessed );
	//}
}

void CDnSkillTabDlg::UpdatePage()
{
	//if( GetCurrentTabID() == m_pActiveSkillTabButton->GetTabID() || GetCurrentTabID() == m_pPassiveSkillTabButton->GetTabID() )
	//{
	//	CDnSkillDlg *pSkillDlg = static_cast<CDnSkillDlg*>(m_groupTabDialog.GetShowDialog());
	//	if( pSkillDlg ) pSkillDlg->UpdatePage();
	//}
}

void CDnSkillTabDlg::ResetAllItem()
{
	//m_pActiveSkillDlg->ResetAllItem();
	//m_pPassiveSkillDlg->ResetAllItem();
}