#include "StdAfx.h"
#include "DnUnlockSkillDlg.h"
#include "DnSkill.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnSkillUnlockDlg::CDnSkillUnlockDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallBack )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallBack )
	, m_pSkillSlotButton(NULL)
	, m_pGold( NULL )
	, m_pSilver( NULL )
	, m_pBronze( NULL )
	, m_pSkillName( NULL )
	, m_pButtonCancel( NULL )
	, m_pButtonOK( NULL )
	, m_pNeedSP( NULL )
	, m_pStaticExclusive( NULL )
{
}

CDnSkillUnlockDlg::~CDnSkillUnlockDlg(void)
{
}

void CDnSkillUnlockDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillLockOffDlg.ui" ).c_str(), bShow );
}

void CDnSkillUnlockDlg::InitialUpdate()
{
	m_pSkillName = GetControl<CEtUIStatic>( "ID_SKILL_NAME" );
	m_pNeedSP = GetControl<CEtUIStatic>( "ID_SKILL_SP" );

	m_pGold = GetControl<CEtUIStatic>("ID_GOLD"); 
	m_pSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pBronze = GetControl<CEtUIStatic>("ID_BRONZE");

	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");

	m_pStaticExclusive = GetControl<CEtUIStatic>( "ID_STATIC_TEXT" );
	m_pStaticExclusive->Show( false );

	m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_OK") );
}

void CDnSkillUnlockDlg::InitCustomControl( CEtUIControl *pControl )
{
	m_pSkillSlotButton = static_cast<CDnSkillSlotButton*>(pControl);
}

void CDnSkillUnlockDlg::SetSkill( MIInventoryItem *pSkill )
{
	ASSERT( pSkill&&"CDnSkillUnlockDlg::SetSkill, pSkill is NULL!" );
	m_pSkillSlotButton->SetItem(pSkill, CDnSlotButton::NO_COUNTABLE_RENDER);

	m_pSkillName->SetText( ((CDnSkill*)pSkill)->GetName() );

	wchar_t szTemp[ MAX_PATH ] = { 0 };
	swprintf_s( szTemp, MAX_PATH, L"%s: %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 727 ), 
				static_cast<CDnSkill*>(pSkill)->GetNeedAcquireSkillPoint() );
	m_pNeedSP->SetText( szTemp );

	CDnSkill* pDnSkill = static_cast<CDnSkill*>( pSkill );
	if( 0 < pDnSkill->GetExclusiveID() )
		m_pStaticExclusive->Show( true );
	else
		m_pStaticExclusive->Show( false );
}

void CDnSkillUnlockDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_SmartMove.MoveCursor();
	}
	else
	{
		m_SmartMove.ReturnCursor();

		m_pSkillSlotButton->ResetSlot();
		m_pSkillName->ClearText();
	}

	CEtUIDialog::Show( bShow );
}


void CDnSkillUnlockDlg::SetPrice( int iPrice )
{
	int nGold = iPrice/10000;
	int nSilver = (iPrice%10000)/100;
	int nBronze = iPrice%100;

	m_pGold->SetIntToText( nGold );
	m_pSilver->SetIntToText( nSilver );
	m_pBronze->SetIntToText( nBronze );
}