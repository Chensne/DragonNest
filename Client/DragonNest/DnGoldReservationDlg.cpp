#include"stdafx.h"
#include "DnGoldReservationDlg.h"
#include "DnMoneyInputDlg.h"
#include "DnSkillTreeDlg.h"
#include "DnSkillTask.h"
#include "DnItemTask.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

CDnReservationGoldDlg::CDnReservationGoldDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallBack )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallBack )
, m_pSPText( NULL )
, m_pGoldText( NULL )
, m_pSilverText( NULL )
, m_pBronzeText( NULL )
, m_pSkillTreeDlg( NULL )
, m_pExculsiveText( NULL )
, m_nPrice( 0 )
, m_nSP( 0 )
{

}

CDnReservationGoldDlg::~CDnReservationGoldDlg()
{

}

void CDnReservationGoldDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillSubLockOffDlg.ui" ).c_str(), bShow );
}

void CDnReservationGoldDlg::InitialUpdate()
{
	m_pSPText = GetControl<CEtUIStatic>( "ID_TEXT_COUNT" );
	m_pGoldText = GetControl<CEtUIStatic>( "ID_GOLD" );
	m_pSilverText = GetControl<CEtUIStatic>( "ID_SILVER" );
	m_pBronzeText = GetControl<CEtUIStatic>( "ID_BRONZE" );
	m_pExculsiveText = GetControl<CEtUIStatic>( "ID_STATIC_TEXT" );
	m_pExculsiveText->Show( false );

	m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_OK") );
}

void CDnReservationGoldDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( true == bShow )
	{
		m_SmartMove.MoveCursor();
		SetInfo();
	}
	else
	{
		m_SmartMove.ReturnCursor();
		m_pSPText->ClearText();
	}

	CEtUIDialog::Show( bShow );
}

void CDnReservationGoldDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			if( m_nSP > GetSkillTask().GetSkillPoint() )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1147) );		// UISTRING : SP가 부족합니다.
				GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1147) );	// UISTRING : SP가 부족합니다.
				return;
			}
			else if( m_nPrice > GetItemTask().GetCoin() )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1744) );		// UISTRING : 소지 금액이 부족합니다.
				GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1744) );	// UISTRING : 소지 금액이 부족합니다.
				return;
			}
			else
			{
				if( m_pSkillTreeDlg )
					m_pSkillTreeDlg->ApplyButtonCommand();
			}
			Show( false );
		}
		else if( IsCmdControl( "ID_CANCEL" ) )
		{
			Show( false );
		}
	}
}

void CDnReservationGoldDlg::SetSkillTreeDlg( CDnSkillTreeDlg * pSkillTreeDlg )
{
	m_pSkillTreeDlg = pSkillTreeDlg;
}

void CDnReservationGoldDlg::SetInfo()
{
	m_nSP = GetSkillTask().GetReservationSkillNeedSP();
	m_nPrice = GetSkillTask().GetReservationSkillNeedMoney();

	m_pSPText->SetIntToText( m_nSP );

	WCHAR wszString[64] = {0,};

	CDnMoneyControl::GetStrMoneyG( m_nPrice, wszString );
	m_pGoldText->SetText( wszString );

	CDnMoneyControl::GetStrMoneyS( m_nPrice, wszString );
	m_pSilverText->SetText( wszString );

	CDnMoneyControl::GetStrMoneyC( m_nPrice, wszString );
	m_pBronzeText->SetText( wszString );
}

#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )