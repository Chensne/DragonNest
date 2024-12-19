#include "stdafx.h"

#if defined(PRE_ADD_DWC)
#include "DnDwcMemberListDlg.h"
#include "DnInterfaceString.h"

CDnDwcMemberListDlg::CDnDwcMemberListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticTitleName(NULL)
, m_pStaticName(NULL)
, m_pStaticClass(NULL)
, m_pStaticCurrentPlace(NULL)
, m_pStaticLevel(NULL)
{
}

CDnDwcMemberListDlg::~CDnDwcMemberListDlg()
{
}

void CDnDwcMemberListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PVPListDlg.ui" ).c_str(), bShow );
}

void CDnDwcMemberListDlg::InitialUpdate()
{
	m_pStaticTitleName	= GetControl<CEtUIStatic>("ID_TEXT_TITLE");
	m_pStaticName		= GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pStaticClass		= GetControl<CEtUIStatic>("ID_TEXT_CLASS");
	m_pStaticCurrentPlace = GetControl<CEtUIStatic>("ID_TEXT_VILLAGE");
	m_pStaticLevel		= GetControl<CEtUIStatic>("ID_TEXT_LV");
}

void CDnDwcMemberListDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	CEtUIDialog::Show(bShow);
}

void CDnDwcMemberListDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnDwcMemberListDlg::SetData(TDWCTeamMember data)
{
	int nMidNumber = 120225;				 // ÆÀ¿ø
	if(data.bTeamLeader) nMidNumber = 120224;// ÆÀÀå

	if(data.bTeamLeader) m_pStaticTitleName->SetTextColor(textcolor::GUILDMEMBERNAME);	
	m_pStaticTitleName->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nMidNumber));
	m_pStaticName->SetText(data.wszCharacterName);
	m_pStaticClass->SetText(DN_INTERFACE::STRING::GetJobString(data.cJobCode));
	
	std::wstring wszLocation;
	DN_INTERFACE::STRING::GetLocationText(wszLocation, data.Location);

	if(eLocationState::_LOCATION_NONE == data.Location.cServerLocation)
		m_pStaticCurrentPlace->SetTextColor(textcolor::RED);

	m_pStaticCurrentPlace->SetText(wszLocation);
}

#endif // PRE_ADD_DWC