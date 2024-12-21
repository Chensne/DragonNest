#include "StdAfx.h"
#include "DnPartyPassDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_PARTY_RENEW_THIRD

CDnPartyPassDlg::CDnPartyPassDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pEditBoxPassWord(NULL)
{
}

CDnPartyPassDlg::~CDnPartyPassDlg(void)
{
}

void CDnPartyPassDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PartyPassDlg.ui" ).c_str(), bShow );
}

void CDnPartyPassDlg::InitialUpdate()
{
	m_pStaticPartyName = GetControl<CEtUIStatic>("ID_STATIC_PARTYNAME");
	m_pStaticTargetStage = GetControl<CEtUIStatic>("ID_STATIC_TARGETSTAGE");

	int i = 0;
	for (; i < PARTYCOUNTMAX; ++i)
	{
		SPartyPassUnit unit;
		std::string str;

		str = FormatA("ID_STATIC2_MASTER%d", i);
		unit.pMasterMark = GetControl<CEtUIStatic>(str.c_str());

		str = FormatA("ID_STATIC_NAME%d", i);
		unit.pName = GetControl<CEtUIStatic>(str.c_str());

		str = FormatA("ID_STATIC_LV%d", i);
		unit.pLevel = GetControl<CEtUIStatic>(str.c_str());

		str = FormatA("ID_STATIC_JOB%d", i);
		unit.pJob = GetControl<CEtUIStatic>(str.c_str());

		m_pPartyMemberInfoList.push_back(unit);
	}

	m_pEditBoxPassWord = GetControl<CEtUIEditBox>("ID_EDITBOX_PASS");

	m_pConfirmBtn = GetControl<CEtUIButton>("ID_BUTTON_OK");
	m_pCancelBtn = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
}

void CDnPartyPassDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_pEditBoxPassWord->ClearText();
		RequestFocus(m_pEditBoxPassWord);
	}
}

LPCWSTR CDnPartyPassDlg::GetPassword()
{
	return m_pEditBoxPassWord->GetText();
}

#endif // PRE_PARTY_RENEW_THIRD