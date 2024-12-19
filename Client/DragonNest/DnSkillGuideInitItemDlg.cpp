#include "stdafx.h"
#include "DnSkillGuideInitItemDlg.h"
#include "DnTableDB.h"

CDnSkillGuideInitItemDlg::CDnSkillGuideInitItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallBack )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallBack )
, m_pCommentStatic( NULL )
{

}

CDnSkillGuideInitItemDlg::~CDnSkillGuideInitItemDlg(void)
{

}

void CDnSkillGuideInitItemDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillInititemGuide.ui" ).c_str(), bShow );

	SetElementDialog(true);
}

void CDnSkillGuideInitItemDlg::InitialUpdate()
{
	m_pCommentStatic = GetControl<CEtUIStatic>( "ID_STATIC_SKILLINITGUIDE" );

	SetResetGuideComment();
}

void CDnSkillGuideInitItemDlg::SetResetGuideComment()
{
	WCHAR wszComment[512] = {0,};
	const int nResetComment_Level = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Skill_ResetComment_Level );
	swprintf_s( wszComment, _countof(wszComment), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1820 ), nResetComment_Level );	// UISTRING : %d레벨이 되면 스킬초기화 아이템이 지급됩니다. 마음껏 스킬을 올리고 사용해 보세요.

	m_pCommentStatic->SetText( wszComment );
}

void CDnSkillGuideInitItemDlg::SetItemSkillComment()
{
	m_pCommentStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1833 ) );	// UISTRING : 스킬레벨업 장비 해제 후 스킬레벨을 최대치로 올려야만 착용 시 최대레벨을 초과할 수 있습니다.
}