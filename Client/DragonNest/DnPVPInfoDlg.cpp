#include "StdAfx.h"
#include "DnPVPInfoDlg.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnPartyTask.h"
#include "DnPlayerActor.h"
#include "DnPVPLobbyVillageTask.h"
#include "TaskManager.h"
#include "DnCharInventory.h"
#include "DnStorageInventory.h"
#include "DnItemTask.h"
#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

CDnPVPInfoDlg::CDnPVPInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
,
 pUIJobIcon(NULL),   // 직업이름 
 pUIGuildName(NULL), // 길드이름
 pUILevel(NULL),     // 레벨
 pUIName(NULL),      // 아이디
 pUIRankIcon(NULL),  // 계급 아이콘
 pUIRankName(NULL),  // 계급 이름
 pUIXP(NULL),        // 현재 콜로세움 XP
 pUINeedXP(NULL)    // 다음 레벨에 필요로하는 XP
 ,pUIScore(NULL)    // 승패
,pUIMedal(NULL)     // 소지메달 갯수
#ifdef PRE_ADD_DWC
, m_bIsDWCUser(false)
#endif
{
}

CDnPVPInfoDlg::~CDnPVPInfoDlg(void)
{
}

void CDnPVPInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpInfoDlg.ui" ).c_str(), bShow );
}

void CDnPVPInfoDlg::InitialUpdate()
{
	pUIJobIcon =  GetControl<CDnJobIconStatic>("ID_STATIC_JOB");
	pUIGuildName = GetControl<CEtUIStatic>("ID_TEXT_GUILDNAME");
	pUILevel =  GetControl<CEtUIStatic>("ID_TEXT_LEVEL");
	pUIName =  GetControl<CEtUIStatic>("ID_TEXT_NAME");
	pUIRankIcon =  GetControl<CEtUITextureControl>("ID_STATIC_RANK");
	pUIRankName =  GetControl<CEtUIStatic>("ID_TEXT_RANKNAME");
	pUIXP =  GetControl<CEtUIStatic>("ID_TEXT_PVPXP");
	pUINeedXP =  GetControl<CEtUIStatic>("ID_TEXT_NEXTXP");
	pUIScore = GetControl<CEtUIStatic>("ID_TEXT_SCORE");
	pUIMedal = GetControl<CEtUIStatic>("ID_TEXT_MEDAL");

#ifdef PRE_ADD_DWC
	m_bIsDWCUser = GetDWCTask().IsDWCChar();

	SetPVPInfo();
	if(m_bIsDWCUser) 
		SetDWCInfo();
#else
	SetPVPInfo();
#endif
}

void CDnPVPInfoDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPVPInfoDlg::Process( float fElapsedTime )
{
	if(!IsShow())
		return;

	CEtUIDialog::Process(fElapsedTime);
}

void CDnPVPInfoDlg::SetPVPInfo()
{
	CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	if(!pPVPLobbyTask)
		return;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
	
	int iUIString = 0;

	if ( pSox && pSox->IsExistItem( pPVPLobbyTask->GetUserInfo().sPvPInfo.cLevel ) )
	{
		iUIString = pSox->GetFieldFromLablePtr(  pPVPLobbyTask->GetUserInfo().sPvPInfo.cLevel , "PvPRankUIString" )->GetInteger();
	}
	if(pPVPLobbyTask->GetUserInfo().GuildSelfView.IsSet()) // 길드에 가입해있는가?
	{
		pUIGuildName->SetText(pPVPLobbyTask->GetUserInfo().GuildSelfView.wszGuildName);
	}

	vector<int> jobHistory;
	for( int i=0; i<JOBMAX; i++ ) {
		if( pPVPLobbyTask->GetUserInfo().cJobArray[i] != 0 )
			jobHistory.push_back( pPVPLobbyTask->GetUserInfo().cJobArray[i] );

	}

	if( !jobHistory.empty() )
	{
		pUIJobIcon->SetIconID(jobHistory[jobHistory.size() -1],true);
	}
	
	pUILevel->SetIntToText(pPVPLobbyTask->GetUserInfo().cLevel);
	pUIName->SetText( pPVPLobbyTask->GetUserInfo().wszCharacterName );
	
	//아이콘
	int iIconW,iIconH;
	int iU,iV;
	iIconW = GetInterface().GeticonWidth();
	iIconH = GetInterface().GeticonHeight();
	if( GetInterface().ConvertPVPGradeToUV( pPVPLobbyTask->GetUserInfo().sPvPInfo.cLevel , iU, iV ))
	{
		pUIRankIcon->SetTexture(GetInterface().GetPVPIconTex(), iU, iV, iIconW, iIconH);
		pUIRankIcon->Show(true);
	}

	pUIRankName->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( pPVPLobbyTask->GetUserInfo().sPvPInfo.cLevel, "PvPRankUIString" )->GetInteger()));
	pUIXP->SetIntToText( pPVPLobbyTask->GetUserInfo().sPvPInfo.uiXP );

	if ( pSox )
	{
		int nNextXP = pSox->GetFieldFromLablePtr( pPVPLobbyTask->GetUserInfo().sPvPInfo.cLevel , "PvPRankEXP" )->GetInteger();
		pUINeedXP->SetIntToText(nNextXP);
	}

	WCHAR wszStr[128];
	wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, PVPWinUIString ),pPVPLobbyTask->GetUserInfo().sPvPInfo.uiWin , pPVPLobbyTask->GetUserInfo().sPvPInfo.uiLose , pPVPLobbyTask->GetUserInfo().sPvPInfo.uiDraw );

	pUIScore->SetText(wszStr);
	pUIMedal->SetIntToText(GetItemTask().GetCharInventory().GetItemCount( PVPMedalTableID ) + GetItemTask().GetStorageInventory().GetItemCount(PVPMedalTableID));
}

#ifdef PRE_ADD_DWC
static bool CompareLeader( TDWCTeamMember s1, TDWCTeamMember s2 ) // 정렬용
{
	bool bResult = false;
	if(s1.bTeamLeader) 
		bResult = true;

	return bResult;
}

void CDnPVPInfoDlg::SetDWCInfo()
{
	// UI On / Off
	CEtUIStatic* pTempStatic = NULL;
	pTempStatic = GetControl<CEtUIStatic>("ID_STATIC4"); // Mid: 팀명
	if(pTempStatic) pTempStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120257)); 

	pTempStatic = GetControl<CEtUIStatic>("ID_STATIC7"); // Mid: 콜로세움 등급
	if(pTempStatic) {
		std::wstring wszLeaderName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120224);
		RemoveStringW(wszLeaderName, L":");
		pTempStatic->SetTextColor(textcolor::RED);
		pTempStatic->SetText(wszLeaderName); // Mid: 팀장
	}

	pTempStatic = GetControl<CEtUIStatic>("ID_STATIC5"); // Mid: 콜로세움 XP
	if(pTempStatic) pTempStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120265)); // Mid: 팀원

	pTempStatic = GetControl<CEtUIStatic>("ID_STATIC6"); // Mid: 목표 XP
	if(pTempStatic) pTempStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120265)); // Mid: 팀원

	pTempStatic = GetControl<CEtUIStatic>("ID_STATIC9"); // Mid: 메달 개수
	if(pTempStatic) pTempStatic->Show(false);

	std::vector<TDWCTeamMember> vMemberList = GetDWCTask().GetDwcTeamMemberList();		
	if(vMemberList.empty() == false)
	{
		std::sort(vMemberList.begin(), vMemberList.end(), CompareLeader); // 리더가 제일 위로

		int nSize = static_cast<int>(vMemberList.size());
		// 팀장
		if(nSize > 0)
		{
			// ※ 콜로세움 등급 아이콘떄문에 아이콘 폭만큼 왼쪽으로 Text를 이동시킵니다
			pUIRankIcon->Show(false);
			SUICoord Coord = pUIRankName->GetUICoord();
			Coord.fX = pUIXP->GetUICoord().fX;
			pUIRankName->SetPosition(Coord.fX, Coord.fY);
			pUIRankName->SetText(vMemberList[0].wszCharacterName);
			pUIRankName->Show(true);
		}

		// 팀원
		if(nSize > 1)
		{
			pUIXP->Show(true);
			pUIXP->SetText(vMemberList[1].wszCharacterName);
		}
		else
			pUIXP->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7106)); // Mid: <없음>

		if(nSize > 2)
		{
			pUINeedXP->Show(true);
			pUINeedXP->SetText(vMemberList[2].wszCharacterName);
		}
		else
			pUINeedXP->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7106)); // Mid: <없음>
	}
	else
	{
		pUIRankIcon->Show(false);
		pUIRankName->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7106)); // Mid: <없음>
		pUIXP->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7106)); // Mid: <없음>
		pUINeedXP->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7106)); // Mid: <없음>
	}


	// 메달은 끈다
	pUIMedal->Show(false);

	std::vector<TDWCTeam> vTeamData = GetDWCTask().GetDwcTeamInfoList();
	if( vTeamData.empty() == false )
	{
		// Team이 있음
		TDWCTeam Data = vTeamData[0];
		WCHAR wszStr[128] = { 0 , };
		wsprintf(wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, PVPWinUIString ), Data.nTotalWin, Data.nTotalLose, Data.nTotalDraw);
		pUIScore->SetText(wszStr);
		pUIGuildName->SetText(Data.wszTeamName);
	}
	else
	{
		WCHAR wszStr[128] = { 0 , };
		wsprintf(wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, PVPWinUIString ), 0,0,0);
		pUIScore->SetText(wszStr);
		pUIGuildName->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1349)); // Mid: 없음
	}

}
#endif // PRE_ADD_DWC