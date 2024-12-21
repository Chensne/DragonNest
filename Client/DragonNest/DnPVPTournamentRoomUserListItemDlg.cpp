#include "stdafx.h"
#include "DnPVPTournamentRoomUserListItemDlg.h"
#include "DnBaseRoomDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_PVP_TOURNAMENT

CDnPVPTournamentRoomUserListItemDlg::CDnPVPTournamentRoomUserListItemDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pJobIcon = NULL;
	m_pNameStatic = NULL;
	m_pTextureRank = NULL;
	m_pMaster = NULL;
}

CDnPVPTournamentRoomUserListItemDlg::~CDnPVPTournamentRoomUserListItemDlg(void)
{
}

void CDnPVPTournamentRoomUserListItemDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("PVPTournamentReadyListDlg.ui").c_str(), bShow);
}

void CDnPVPTournamentRoomUserListItemDlg::InitialUpdate()
{
	m_pJobIcon = GetControl<CDnJobIconStatic>("ID_STATIC_CLASS");
	m_pNameStatic = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pMaster = GetControl<CEtUIStatic>("ID_STATIC_MASTER");
	m_pTextureRank = GetControl<CEtUITextureControl>("ID_TEXTURE_RANK");

	m_pMaster->Show(false);
}

void CDnPVPTournamentRoomUserListItemDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show(bShow);
}

void CDnPVPTournamentRoomUserListItemDlg::SetInfo(const SMatchUserInfo& data)
{
	m_pJobIcon->SetIconID(data.nJobID, true);
	m_pNameStatic->SetText(data.playerName.c_str());
	m_pMaster->Show(CDnBaseRoomDlg::IsMaster(data.uiUserState));

	int iIconW,iIconH;
	int iU,iV;
	iIconW = GetInterface().GeticonWidth();
	iIconH = GetInterface().GeticonHeight();
	if (GetInterface().ConvertPVPGradeToUV( data.cPVPLevel, iU, iV ))
	{
		m_pTextureRank->SetTexture(GetInterface().GetPVPIconTex(), iU, iV, iIconW, iIconH);
		m_pTextureRank->Show(true);
	}
}

bool CDnPVPTournamentRoomUserListItemDlg::IsSame(const SMatchUserInfo& info) const
{
	const std::wstring& nameString = m_pNameStatic->GetText();
	return (info.playerName.compare(nameString.c_str()) == 0);
}

#endif // PRE_ADD_PVP_TOURNAMENT