#include "StdAfx.h"
#include "DnSkillAcquireDlg.h"
#include "DnSkill.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnSkillAcquireDlg::CDnSkillAcquireDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallBack )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallBack )
	, m_pSkillSlotButton(NULL)
	, m_pStaticSkillName(NULL)
	, m_pStaticSkillSP(NULL)
{
}

CDnSkillAcquireDlg::~CDnSkillAcquireDlg(void)
{
}

void CDnSkillAcquireDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillAcquireDlg.ui" ).c_str(), bShow );
}

void CDnSkillAcquireDlg::InitialUpdate()
{
	m_pStaticSkillName = GetControl<CEtUIStatic>("ID_SKILL_NAME");
	m_pStaticSkillSP = GetControl<CEtUIStatic>("ID_STATIC_SP");

	m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_OK") );
}

void CDnSkillAcquireDlg::InitCustomControl( CEtUIControl *pControl )
{
	m_pSkillSlotButton = static_cast<CDnSkillSlotButton*>(pControl);
}

void CDnSkillAcquireDlg::SetSkill( MIInventoryItem *pSkill )
{
	ASSERT( pSkill&&"CDnSkillAcquireDlg::SetSkill, pSkill is NULL!" );
	m_pSkillSlotButton->SetItem(pSkill, CDnSlotButton::NO_COUNTABLE_RENDER);

	m_pStaticSkillName->SetText( ((CDnSkill*)pSkill)->GetName() );

	wchar_t szTemp[64] = {0};
	//swprintf_s( szTemp, _countof(szTemp), L"SP %d %s", ((CDnSkill*)pSkill)->GetNeedAcquireSkillPoint(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1812 ) );	
	CDnSkill* pDnSkill = static_cast<CDnSkill*>(pSkill);
	int iNeedSkillPoint = 0;
	if( pDnSkill->IsAcquired() )
		iNeedSkillPoint = pDnSkill->GetNextLevelSkillPoint();
	else
		iNeedSkillPoint = pDnSkill->GetNeedAcquireSkillPoint();

	swprintf_s( szTemp, _countof(szTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 731 ), iNeedSkillPoint );
	m_pStaticSkillSP->SetText( szTemp );
}

void CDnSkillAcquireDlg::Show( bool bShow ) 
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
		m_pStaticSkillName->ClearText();
		m_pStaticSkillSP->ClearText();
	}

	CEtUIDialog::Show( bShow );
}