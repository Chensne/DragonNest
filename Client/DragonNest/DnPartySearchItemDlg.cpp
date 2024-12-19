#include "StdAfx.h"
#include "DnPartySearchItemDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPartySearchItemDlg::CDnPartySearchItemDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticName(NULL)
#ifdef PRE_PARTY_DB
#else
, m_pCheckBox(NULL)
#endif
, m_nMapIdForMSG(-1)
, m_pPartyItemDialog(NULL)
{
}

CDnPartySearchItemDlg::~CDnPartySearchItemDlg(void)
{
}

void CDnPartySearchItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PartySearchListDlg.ui" ).c_str(), bShow );
}

void CDnPartySearchItemDlg::InitialUpdate()
{
	m_pStaticName	= GetControl<CEtUIStatic>( "ID_TITLE_NAME" );
#ifdef PRE_PARTY_DB
#else
	m_pCheckBox		= GetControl<CEtUICheckBox>( "ID_CHECKBOX0" );
#endif
}

void CDnPartySearchItemDlg::SetInfo(const std::wstring& name, int nMapId, CEtUIDialog* pParent)
{
	m_pStaticName->SetText(name.c_str());
#ifdef PRE_PARTY_DB
#else
	m_pCheckBox->SetChecked(false);
#endif
	m_nMapIdForMSG = nMapId;
	m_pPartyItemDialog = pParent;
}

void CDnPartySearchItemDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ )
{
	SetCmdControlName( pControl->GetControlName() );

#ifdef PRE_PARTY_DB
#else
	if (nCommand == EVENT_CHECKBOX_CHANGED && bTriggeredByUser)
	{
		if (m_pPartyItemDialog)
			m_pPartyItemDialog->ProcessCommand(nCommand, true, m_pCheckBox, m_nMapIdForMSG);
		return;
	}
#endif

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}