#include "StdAfx.h"
#include "DnPVPTournamentModeHud.h"
#ifdef PRE_PVP_GAMBLEROOM
#include "DnBridgeTask.h"
#endif // PRE_PVP_GAMBLEROOM


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_PVP_TOURNAMENT

CDnPVPTournamentModeHUD::CDnPVPTournamentModeHUD(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnRoundModeHUD(dialogType, pParentDialog, nID, pCallback)
{
}

CDnPVPTournamentModeHUD::~CDnPVPTournamentModeHUD(void)
{
}

void CDnPVPTournamentModeHUD::Initialize(bool bShow)
{
	CDnRoundModeHUD::Initialize( bShow );

	m_pObjectiveCount = GetControl<CEtUIStatic>("ID_PVP_OBJECT");

#ifdef PRE_PVP_GAMBLEROOM
	if( m_pObjectiveCount )
	{
		bool bGambleMode = false;
		if( CDnBridgeTask::IsActive() )
		{
			const CDnPVPTournamentDataMgr* pDataMgr = &(CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr());
			if( pDataMgr )
			{
				const SBasicRoomInfo& roomInfo = pDataMgr->GetRoomInfo();
				if( roomInfo.cGambleType == PvPGambleRoom::Petal || roomInfo.cGambleType == PvPGambleRoom::Gold )
					bGambleMode = true;
			}
		}

		if( bGambleMode )
			m_pObjectiveCount->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8404 ) ); // UISTRING : 도박모드
		else
			m_pObjectiveCount->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120175 ) ); // UISTRING : 토너먼트
	}
#else // PRE_PVP_GAMBLEROOM
	if (m_pObjectiveCount)
		m_pObjectiveCount->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120175)); // UISTRING : 토너먼트
#endif // PRE_PVP_GAMBLEROOM

	ForceShowOff_ContinuousDualInfo(true);
}

void CDnPVPTournamentModeHUD::SetVersusUserName(const std::wstring& leftUser, const std::wstring& rightUser)
{
	if (m_pStaticFriendly)
		m_pStaticFriendly->SetText(leftUser.c_str());
	if (m_pStaticEnemy)
		m_pStaticEnemy->SetText(rightUser.c_str());
}

#endif