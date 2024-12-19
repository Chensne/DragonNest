#include "StdAfx.h"
#include "DnAniTestDlg.h"

#ifdef PRE_TEST_ANIMATION_UI

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnAniTestDlg::CDnAniTestDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pTestAni(NULL)
{
}

CDnAniTestDlg::~CDnAniTestDlg(void)
{
}

void CDnAniTestDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "AniTestDlg.ui" ).c_str(), bShow);
}

void CDnAniTestDlg::InitialUpdate()
{
	m_pTestAni = GetControl<CEtUIAnimation>("ID_ANIMATION0");
	m_pTestAni->SetLoop(true);
}

void CDnAniTestDlg::Show( bool bShow )
{
	CEtUIDialog::Show(bShow);

	if (bShow)
	{
		if (m_pTestAni)
			m_pTestAni->Play();
	}
}

#endif