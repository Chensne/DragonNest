#include "StdAfx.h"
#include "DnPVPLadderSystemDlg.h"
#include "DnInterface.h"
#include "EtUIDialog.h"
#include "SystemSendPacket.h"
#include "DnPVPLobbyVillageTask.h"
#include "TaskManager.h"
#include "PVPSendPacket.h"
#include "DnTableDB.h"
#include "DnChatTabDlg.h"
#include "DnPVPLadderListItemDlg.h"
#include "DnPVPLadderPreviewDlg.h"
#include "DnPVPRoomListDlg.h"
#include "DnPVPLadderTabDlg.h"
#include "DnPartyTask.h"
#if defined(PRE_ADD_DWC)
#include "VillageSendPacket.h"
#include "DnPVPDWCTabDlg.h"
#include "DnDWCTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPLadderSystemDlg::CDnPVPLadderSystemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{    
	m_pLadderListBox = NULL;
	m_pButtonStartSearch = NULL;
	m_pButtonStopSearch = NULL;
	m_pButtonExit = NULL;
	m_pLadderRoomPage = NULL;
	m_pButtonNext = NULL;
	m_pButtonPrev = NULL;
	m_pStaticRoomSelect = NULL;
	m_pButtonObserverJoin = NULL;
	m_pButtonRefresh = NULL;
	m_pLadderLeftTime = NULL;

	m_nLadderRoom_Page = 1;
	m_nLadderRoom_MaxPage = 0;
	m_nRoomRefreshDelay = 0.f;
	m_nSelectedRoomIndex = 0;
	m_nMatchingSoundIndex = -1;
	
	m_vecLadderRoomList.clear();
	m_wszRoomLeaderName.clear();
	m_nLadderState = LadderSystem::RoomState::WaitUser;

	// Child Dlg
	m_pPreviewDlg = NULL;
	m_nSelectedLadderType = LadderSystem::MatchType::None;

	memset(&m_sPlayingRoomListInfo,0,sizeof(m_sPlayingRoomListInfo));
	m_pTeamGameDlg = NULL;
	m_pLadderObserverInfoDlg = NULL;

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	m_bLeavenLadderByExitButton = false;
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
}

CDnPVPLadderSystemDlg::~CDnPVPLadderSystemDlg(void)
{	
	SAFE_DELETE(m_pPreviewDlg);
	SAFE_DELETE(m_pTeamGameDlg);
	SAFE_DELETE(m_pLadderObserverInfoDlg);

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	m_bLeavenLadderByExitButton = false;
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
}


void CDnPVPLadderSystemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpLadderDlg.ui" ).c_str(), bShow );
}

void CDnPVPLadderSystemDlg::InitialUpdate()
{
	m_pPreviewDlg = new CDnPVPLadderPreviewDlg( UI_TYPE_CHILD, this );
	m_pPreviewDlg->Initialize( true );
	m_pTeamGameDlg = new CDnPVPLadderTeamGameDlg(UI_TYPE_CHILD, this );
	m_pTeamGameDlg->Initialize( false );
	m_pLadderObserverInfoDlg = new CDnPVPLadderObserverInfoDlg(UI_TYPE_CHILD_MODAL, this );
	m_pLadderObserverInfoDlg->Initialize( false );

	InitializeCommonInfo();     // 공용     UI
	InitializeLadderRoomInfo(); // 레더룸   UI
	InitializeLadderInfo();     // 레더정보 UI

#ifdef PRE_ADD_DWC
	if(GetDWCTask().IsDWCChar())
	{
		// 게임 끝내고 난후 팀정보 갱신
		SendDWCTeamInfomation(false);
	}
#endif
}

void CDnPVPLadderSystemDlg::ShowLadderListBox(bool bShow , LadderSystem::MatchType::eCode eMatchCode)
{
	if(m_pLadderListBox)
		m_pLadderListBox->Show(bShow);
	m_pTeamGameDlg->Show(!bShow);
	m_pTeamGameDlg->SetRoomState(eMatchCode);
}

void CDnPVPLadderSystemDlg::InitializeCommonInfo()
{
	m_nMatchingSoundIndex = CEtSoundEngine::GetInstance().LoadSound( "UI_PvPMatching.ogg" , false, false );

	m_pLadderListBox = GetControl<CEtUIListBoxEx>("ID_STATUS_LISTBOXEX");
	m_pLadderRoomPage = GetControl<CEtUIStatic>("ID_PAGE");
	m_pLadderRoomPage->SetText(L"1/1");

	m_pLadderLeftTime = GetControl<CEtUIStatic>("ID_STATIC_TIME");
	m_pLadderLeftTime->Show(false);

	// Button
	m_pButtonExit = GetControl<CEtUIButton>("ID_BUTTON_EXIT");
	m_pButtonStartSearch = GetControl<CEtUIButton>("ID_BT_DNGIRL");
	EnableSearchButton(true,false);
	m_pButtonStartSearch->SetDisableTime(1.f);

	m_pButtonStopSearch = GetControl<CEtUIButton>("ID_BT_OUT");
	EnalbeStopSearchButton(false,true);

	m_pButtonNext = GetControl<CEtUIButton>("ID_NEXT");
	m_pButtonPrev = GetControl<CEtUIButton>("ID_PRIOR");
	m_pStaticRoomSelect = GetControl<CEtUIStatic>("ID_SELECT");
	m_pStaticRoomSelect->Show(false);

	m_pButtonObserverJoin = GetControl<CEtUIButton>("ID_BT_OBSERVERJOIN");
	m_pButtonObserverJoin->Enable(false);

	m_pButtonRefresh = GetControl<CEtUIButton>("ID_REFRESH");
	m_pButtonRefresh->SetDisableTime(1.f);
}

void CDnPVPLadderSystemDlg::InitializeLadderRoomInfo()
{
	for(int i=0; i< LadderSystem::Common::RoomListPerPage ; i++ )
	{
		sLadderRoomList Temp;

		char szUIName[256] = "";

#ifdef PRE_FIX_PVP_LOBBY_RENEW
		sprintf(szUIName, "ID_STATIC_CLASS0_%d", i);
		Temp.pJobIconTeam_A = GetControl<CDnJobIconStatic>(szUIName);
		Temp.pJobIconTeam_A->Show(false);

		sprintf(szUIName, "ID_STATIC_CLASS1_%d", i);
		Temp.pJobIconTeam_B = GetControl<CDnJobIconStatic>(szUIName);
		Temp.pJobIconTeam_B->Show(false);
#endif

		sprintf(szUIName,"ID_TEXT_NANE0_%d",i);
		Temp.pStaticTeam_A = GetControl<CEtUIStatic>(szUIName);
		Temp.pStaticTeam_A->Show(false);

		sprintf(szUIName,"ID_TEXT_NANE1_%d",i);
		Temp.pStaticTeam_B = GetControl<CEtUIStatic>(szUIName);
		Temp.pStaticTeam_B->Show(false);

		sprintf(szUIName,"ID_TEXT_VS%d",i);
		Temp.pStaticVS = GetControl<CEtUIStatic>(szUIName);
		Temp.pStaticVS->Show(false);

		sprintf(szUIName,"ID_ROBBY_SELECT%d",i);
		Temp.pStaticDummy = GetControl<CEtUIStatic>( szUIName );

		m_vecLadderRoomList.push_back(Temp);
	}
};

void CDnPVPLadderSystemDlg::InitializeLadderInfo()
{
	m_sLadderInfo.pStatic_Score =              GetControl<CEtUIStatic>("ID_TEXT_SCORE");
	m_sLadderInfo.pStatic_ExpectPoint =        GetControl<CEtUIStatic>("ID_TEXT_EXPECT");
	m_sLadderInfo.pStatic_WeekCount =          GetControl<CEtUIStatic>("ID_TEXT_WEEKCOUNT");
	m_sLadderInfo.pStatic_TotalWinningRate =   GetControl<CEtUIStatic>("ID_TEXT_ALLWINCOUNT");
	m_sLadderInfo.pStatic_TotalWinningStreak = GetControl<CEtUIStatic>("ID_TEXT_ALLWIN");
	m_sLadderInfo.pStatic_TotalLosingStreak =  GetControl<CEtUIStatic>("ID_TEXT_ALLDEFEAT");
	m_sLadderInfo.pStatic_TotalHighClass =     GetControl<CEtUIStatic>("ID_TEXT_HIGHCLASS");
	m_sLadderInfo.pStatic_TotalLowClass =      GetControl<CEtUIStatic>("ID_TEXT_LOWCLASS");
	m_sLadderInfo.pStatic_TodayWin =           GetControl<CEtUIStatic>("ID_TEXT_TODAY_WIN");
	m_sLadderInfo.pStatic_TodayDefeat =        GetControl<CEtUIStatic>("ID_TEXT_TODAY_DEFEAT");
	m_sLadderInfo.pStatic_TodayDraw =          GetControl<CEtUIStatic>("ID_TEXT_TODAY_TIE");
	m_sLadderInfo.pStatic_TodayWinningRate =   GetControl<CEtUIStatic>("ID_TEXT_TODAY_WINCOUNT");
	m_sLadderInfo.pStatic_TotalWin =           GetControl<CEtUIStatic>("ID_TEXT_ALL_WIN");
	m_sLadderInfo.pStatic_TotalDefeat =        GetControl<CEtUIStatic>("ID_TEXT_ALL_DEFEAT");
	m_sLadderInfo.pStatic_TotalDraw =          GetControl<CEtUIStatic>("ID_TEXT_ALL_TIE");
	m_sLadderInfo.pStatic_TotalWinningRate_S = GetControl<CEtUIStatic>("ID_TEXT_ALL_WINCOUNT"); // 아래쪽 [전체-승률]
}

#if defined(PRE_ADD_DWC)
void CDnPVPLadderSystemDlg::SetDWCScoreInfo(TDWCTeam pData)
{
	WCHAR wszWinningRate[20] = { 0, };

	int TotalCount = (pData.nTotalWin+pData.nTotalLose+pData.nTotalDraw) ? (pData.nTotalWin+pData.nTotalLose+pData.nTotalDraw) : 1;
	wsprintf(wszWinningRate,L"%d%%", (int)((float)pData.nTotalWin/(float)TotalCount * 100) );
	m_sLadderInfo.pStatic_TotalWinningRate->SetText(wszWinningRate);				// 위쪽		[전체 승률]
	m_sLadderInfo.pStatic_TotalWinningStreak->SetIntToText(pData.wConsecutiveWin);	// 연승
	m_sLadderInfo.pStatic_TotalLosingStreak->SetIntToText(pData.wConsecutiveLose);  // 연패

	m_sLadderInfo.pStatic_TotalWinningRate_S->SetText(wszWinningRate);				// 아래쪽	[전체-승률]

	TotalCount = (pData.wTodayWin+pData.wTodayLose+pData.wTodayDraw) ? (pData.wTodayWin+pData.wTodayLose+pData.wTodayDraw) : 1;
	wsprintf(wszWinningRate,L"%d%%", int((float)pData.wTodayWin/(float)TotalCount* 100) );
	m_sLadderInfo.pStatic_TodayWinningRate->SetText(wszWinningRate);			// 오늘 승률

	m_sLadderInfo.pStatic_Score->SetIntToText(pData.nDWCPoint);					// 평점
	//m_sLadderInfo.pStatic_WeekCount->SetIntToText(pData.nWeeklyPlayCount);		// 이번주 경기수
	m_sLadderInfo.pStatic_ExpectPoint->SetIntToText(pData.nWeeklyPlayCount);		// 이번주 경기수

	m_sLadderInfo.pStatic_TodayWin->SetIntToText(pData.wTodayWin);
	m_sLadderInfo.pStatic_TodayDefeat->SetIntToText(pData.wTodayLose);
	m_sLadderInfo.pStatic_TodayDraw->SetIntToText(pData.wTodayDraw);

	m_sLadderInfo.pStatic_TotalWin->SetIntToText(pData.nTotalWin);
	m_sLadderInfo.pStatic_TotalDefeat->SetIntToText(pData.nTotalLose);
	m_sLadderInfo.pStatic_TotalDraw->SetIntToText(pData.nTotalDraw);
}
#endif

void CDnPVPLadderSystemDlg::SetLadderScoreInfo(LadderSystem::SC_SCOREINFO *pData) // 으아 코드 지저분 ㅠㅠ
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );	
	if(!pSox)
		return;

	std::wstring m_wszJobName;
	WCHAR wszWinningRate[20];

	int LadderType =         pData->Data.cLadderTypeCount;
	int LadderPoint =        pData->Data.iPvPLadderPoint;
	int HighClass =          pData->Data.cMaxKillJobCode;
	int LowClass =           pData->Data.cMaxDeathJobCode;

	TPvPLadderMatchTypeScore LadderScoreInfo[LadderSystem::MatchType::MaxCount];
	memset(&LadderScoreInfo,0,sizeof(LadderScoreInfo));
	for(int i=0;i<LadderSystem::MatchType::MaxCount;i++)
	{
		LadderScoreInfo[i].iPvPLadderGradePoint = LadderSystem::Stats::InitGradePoint; // 레더 초기값 . 정보가 없다면 초기값으로 설정.
	}

	int LadderModeType = LadderSystem::MatchType::_1vs1 - 1; // 디폴트 

	if(m_nSelectedLadderType >= LadderSystem::MatchType::_1vs1 && m_nSelectedLadderType <= LadderSystem::MatchType::MaxCount)
		LadderModeType = m_nSelectedLadderType - 1; // 설정된 레더타입으로 인지.
	
	if(pData->Data.cLadderTypeCount >= LadderSystem::MatchType::_1vs1 && pData->Data.cLadderTypeCount <= LadderSystem::MatchType::MaxCount ) // 레더정보가 존재하고
	{
		for(int i=0; i<pData->Data.cLadderTypeCount; i++)
		{
			if(pData->Data.LadderScore[i].cPvPLadderCode == m_nSelectedLadderType)
			{
				LadderScoreInfo[LadderModeType] = pData->Data.LadderScore[i];
				break;
			}
		}
	}

	int LadderGrade =       LadderScoreInfo[LadderModeType].iPvPLadderGradePoint;
	int LadderHiddenGrade = LadderScoreInfo[LadderModeType].iHiddenPvPLadderGradePoint;
	int LadderWin =         LadderScoreInfo[LadderModeType].iWin;
	int LadderLose =        LadderScoreInfo[LadderModeType].iLose;
	int LadderDraw =        LadderScoreInfo[LadderModeType].iDraw;
	int WinningStreak =     LadderScoreInfo[LadderModeType].nConsecutiveWin;
	int LosingStreak =      LadderScoreInfo[LadderModeType].nConsecutiveLose;
	int TodayWin =          LadderScoreInfo[LadderModeType].nTodayWin;
	int TodayLose =         LadderScoreInfo[LadderModeType].nTodayLose;
	int TodayDraw =         LadderScoreInfo[LadderModeType].nTodayDraw;
	int WeekCount =         LadderScoreInfo[LadderModeType].nWeeklyCount;
	__time64_t Time =       LadderScoreInfo[LadderModeType].tLasePvPLadderScoreDate;

	int TotalCount = (LadderWin+LadderLose+LadderDraw) ? (LadderWin+LadderLose+LadderDraw) : 1;
	wsprintf(wszWinningRate,L"%d%%", (int)((float)LadderWin/(float)TotalCount * 100) );
	m_sLadderInfo.pStatic_TotalWinningRate->SetText(wszWinningRate);         // 승률
	m_sLadderInfo.pStatic_TotalWinningRate_S->SetText(wszWinningRate);       // 승률

	TotalCount = (TodayWin+TodayLose+TodayDraw) ? (TodayWin+TodayLose+TodayDraw) : 1;
	wsprintf(wszWinningRate,L"%d%%", int((float)TodayWin/(float)TotalCount* 100) );
	m_sLadderInfo.pStatic_TodayWinningRate->SetText(wszWinningRate);         // 오늘 승률

	m_sLadderInfo.pStatic_Score->SetIntToText(LadderGrade);               // 레더 포인트
	m_sLadderInfo.pStatic_WeekCount->SetIntToText(WeekCount);            // 이번주 참여횟수

	if(WeekCount < LadderSystem::Exchange::MinGamePlay)
	{
		m_sLadderInfo.pStatic_ExpectPoint->SetIntToText(0); // 예상획득점수 카운트 미만일시 0 
		m_sLadderInfo.pStatic_WeekCount->SetTextColor(textcolor::RED);
	}
	else
	{
		m_sLadderInfo.pStatic_ExpectPoint->SetIntToText(LadderScoreInfo[LadderModeType].GetExchangeLadderPoint()); // 예상획득점수
		m_sLadderInfo.pStatic_WeekCount->SetTextColor(textcolor::GREENYELLOW);
	}

	m_sLadderInfo.pStatic_TotalWinningStreak->SetIntToText(WinningStreak);// 연승
	m_sLadderInfo.pStatic_TotalLosingStreak->SetIntToText(LosingStreak);  // 연패

	m_sLadderInfo.pStatic_TotalHighClass->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4308 )); // 없을때는 정보없음
	m_sLadderInfo.pStatic_TotalLowClass->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4308 )); // 없을때는 정보없음

	if( pSox->IsExistItem( HighClass ) ) 
	{		
		m_wszJobName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( HighClass, "_JobName" )->GetInteger() );	
		m_sLadderInfo.pStatic_TotalHighClass->SetText(m_wszJobName.c_str());// 가장많이 이긴 클래스
	}

	if( pSox->IsExistItem( LowClass ) ) 
	{		
		m_wszJobName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( LowClass, "_JobName" )->GetInteger() );	
		m_sLadderInfo.pStatic_TotalLowClass->SetText(m_wszJobName.c_str()); // 가장많이 진 클래스
	}
		
	m_sLadderInfo.pStatic_TodayWin->SetIntToText(TodayWin);
	m_sLadderInfo.pStatic_TodayDefeat->SetIntToText(TodayLose);
	m_sLadderInfo.pStatic_TodayDraw->SetIntToText(TodayDraw);

	m_sLadderInfo.pStatic_TotalWin->SetIntToText(LadderWin);
	m_sLadderInfo.pStatic_TotalDefeat->SetIntToText(LadderLose);
	m_sLadderInfo.pStatic_TotalDraw->SetIntToText(LadderDraw);
}

void CDnPVPLadderSystemDlg::SetLadderJobScoreInfo(LadderSystem::SC_SCOREINFO_BYJOB *pData)
{
	m_pLadderListBox->RemoveAllItems();

	for(int i=0; i<pData->Data.cJobCount; i++)
	{
		CDnPVPLadderListItemDlg *pItem = m_pLadderListBox->AddItem<CDnPVPLadderListItemDlg>();
		pItem->SetClassInfo(&pData->Data.LadderScoreByJob[i]);
	}
}

void CDnPVPLadderSystemDlg::SetLadderLeftTime(LadderSystem::SC_MATCHING_AVGSEC *pData)
{
	m_pLadderLeftTime->Show(true);

	if(pData->iSec < 0)
	{
		m_pLadderLeftTime->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126016 ));
	}
	else
	{
		int Min,Sec = 0;
		Min = pData->iSec / 60;
		Sec = pData->iSec % 60;
		WCHAR wszMessage[256];
		wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126017 ),Min,Sec);
		m_pLadderLeftTime->SetText(wszMessage);
	}
}


void CDnPVPLadderSystemDlg::RefreshRoomList(UINT nPage)
{
	SendRefreshLadderRoomList(nPage);
}

void CDnPVPLadderSystemDlg::ResetRoomPage()
{
	m_nLadderRoom_Page = 1;
	m_nLadderRoom_MaxPage = 0;

	m_pButtonNext->Enable(false);
	m_pButtonPrev->Enable(false);

	WCHAR wszMessage[20];
	wsprintf(wszMessage,L"%d/%d", m_nLadderRoom_Page , m_nLadderRoom_MaxPage <= 0 ? 1 : m_nLadderRoom_MaxPage); 
	m_pLadderRoomPage->SetText(wszMessage); // 페이지 사이즈만큼 설정.
}

void CDnPVPLadderSystemDlg::ResetRoomList(bool bShow)
{
	for(int i=0; i< LadderSystem::Common::RoomListPerPage ; i++ )
	{
#ifdef PRE_FIX_PVP_LOBBY_RENEW
		m_vecLadderRoomList[i].pJobIconTeam_A->Show(bShow);
		m_vecLadderRoomList[i].pJobIconTeam_A->Show(bShow);

		m_vecLadderRoomList[i].pJobIconTeam_B->Show(bShow);
		m_vecLadderRoomList[i].pJobIconTeam_B->Show(bShow);
#endif
		m_vecLadderRoomList[i].pStaticTeam_A->Show(bShow);
		m_vecLadderRoomList[i].pStaticTeam_B->Show(bShow);
		m_vecLadderRoomList[i].pStaticVS->Show(bShow);
	}
}

void CDnPVPLadderSystemDlg::SetRoomList(LadderSystem::SC_PLAYING_ROOMLIST *pData) // 요기에 패킷에 포함된 방제목과 기타정보를 설정/저장하면됩니다.
{
	ResetRoomList();

	int LadderNameCount = LadderSystem::MatchType::_1vs1 * 2; // 디폴트

#ifdef PRE_ADD_DWC
	if(GetDWCTask().IsDWCChar())
	{
		if( m_nSelectedLadderType == LadderSystem::MatchType::_3vs3_DWC || m_nSelectedLadderType == LadderSystem::MatchType::_3vs3_DWC_PRACTICE)
			m_nSelectedLadderType = LadderSystem::GetNeedTeamCount(static_cast<LadderSystem::MatchType::eCode>(m_nSelectedLadderType));
	}
#endif

	if(m_nSelectedLadderType >= LadderSystem::MatchType::_1vs1 && m_nSelectedLadderType <= LadderSystem::MatchType::MaxCount)
		LadderNameCount = m_nSelectedLadderType * 2; //      2 / 4/ 6 / 8

	int nRoomSize = pData->unNameCount/LadderNameCount; // 모드별로 나눠야할듯 <2vs2 , 3vs3 > 등등

	if(nRoomSize > LadderSystem::Common::RoomListPerPage)
		nRoomSize = LadderSystem::Common::RoomListPerPage;


	for(int i=0; i< nRoomSize ; i++ ) // 패킷에 룸 사이즈만 큼만 맥스치 검사해야함 // 하기전에 리프래쉬 한번 해줌 < show(False) >
	{
		int nFirstIndex = i * LadderNameCount; // 1vs1 , 2vs2 , 3vs3  등은 배율이 틀려야합니다.

#ifdef PRE_FIX_PVP_LOBBY_RENEW
		m_vecLadderRoomList[i].pJobIconTeam_A->Show(true);
		m_vecLadderRoomList[i].pJobIconTeam_A->SetIconID(pData->cJob[nFirstIndex]);

		m_vecLadderRoomList[i].pJobIconTeam_B->Show(true);
		m_vecLadderRoomList[i].pJobIconTeam_B->SetIconID(pData->cJob[nFirstIndex + LadderNameCount/2]);
#endif
		m_vecLadderRoomList[i].pStaticTeam_A->Show(true);
		m_vecLadderRoomList[i].pStaticTeam_A->SetText(pData->wszCharName[nFirstIndex]);
		m_vecLadderRoomList[i].pStaticTeam_B->Show(true);
		m_vecLadderRoomList[i].pStaticTeam_B->SetText(pData->wszCharName[nFirstIndex + LadderNameCount/2]);
		m_vecLadderRoomList[i].pStaticVS->Show(true);
	}

	m_pButtonNext->Enable(false);
	m_pButtonPrev->Enable(false);

	m_nLadderRoom_MaxPage = pData->unMaxPage;
	
	if(m_nLadderRoom_Page > m_nLadderRoom_MaxPage)
		m_nLadderRoom_Page = m_nLadderRoom_MaxPage;

	if(m_nLadderRoom_Page < 1)
		m_nLadderRoom_Page = 1;
	
	if(m_nLadderRoom_MaxPage > 0)
	{
		m_pButtonNext->Enable(true);
		m_pButtonPrev->Enable(true);

		if(m_nLadderRoom_Page >= m_nLadderRoom_MaxPage)
			m_pButtonNext->Enable(false);
		
		if(m_nLadderRoom_Page <= 1)
			m_pButtonPrev->Enable(false);
	}

	WCHAR wszMessage[20];
	wsprintf(wszMessage,L"%d/%d", m_nLadderRoom_Page , m_nLadderRoom_MaxPage <= 0 ? 1 : m_nLadderRoom_MaxPage); 
	// 맥스 페이지가 존재한다면 패킷에 맥스페이지가 1이넘어오고 방이 하나도없으면 0 인데 이럴때는 페이지를 1로 표시

	m_pLadderRoomPage->SetText(wszMessage); // 페이지 사이즈만큼 설정.
	m_nRoomRefreshDelay = 0.f;

	memset(&m_sPlayingRoomListInfo,0,sizeof(m_sPlayingRoomListInfo));
	m_sPlayingRoomListInfo = *pData; // 정보를 들고있자 상세정보에서 이 해당정보를 사용한다.
}

void CDnPVPLadderSystemDlg::SetRoomLeaderName(std::wstring wszName)
{
	m_wszRoomLeaderName = wszName;

	CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	if( !pPVPLobbyTask )
		return;

	std::wstring wszCharName = pPVPLobbyTask->GetUserInfo().wszCharacterName;
	if( __wcsicmp_l( wszCharName.c_str(), m_wszRoomLeaderName.c_str() ) == 0 )
	{
		EnableSearchButton(true,true);
	}
	else
	{
		EnableSearchButton(false,false);
	}
}

void CDnPVPLadderSystemDlg::SetPVPLadderState(int nState)
{
	m_nLadderState = nState;
	m_pButtonRefresh->Enable(false);

	switch(nState)
	{
	case LadderSystem::RoomState::WaitUser:
		{
			if(GetInterface().GetPVPLadderTabDlg())
			{
				GetInterface().GetPVPLadderTabDlg()->EnableTabButton(LadderSystem::MatchType::eCode::None,true,true);
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				GetInterface().GetPVPLadderTabDlg()->SetPVPLadderState( nState );
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
			}
			m_pButtonExit->Enable(true);
			EnableSearchButton(true,true);
			EnalbeStopSearchButton(false);
			m_pButtonRefresh->Enable(true);
			m_pLadderLeftTime->Show(false);
			break;
		}
	case LadderSystem::RoomState::WaitMatching:
		{
			if(GetInterface().GetPVPLadderTabDlg())
			{
				GetInterface().GetPVPLadderTabDlg()->EnableTabButton(LadderSystem::MatchType::eCode::None,false,true);
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				GetInterface().GetPVPLadderTabDlg()->SetPVPLadderState( nState );
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
			}
			EnableSearchButton(false);
			EnalbeStopSearchButton(true,true);
			m_pButtonExit->Enable(false);
			break;
		}
	case LadderSystem::RoomState::Matched:
		{
			if(GetInterface().GetPVPLadderTabDlg())
			{
				GetInterface().GetPVPLadderTabDlg()->EnableTabButton(LadderSystem::MatchType::eCode::None,false,true);
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				GetInterface().GetPVPLadderTabDlg()->SetPVPLadderState( nState );
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
			}
			EnableSearchButton(false);
			EnalbeStopSearchButton(true,false);
			m_pButtonExit->Enable(false);
			if(m_pPreviewDlg)
				m_pPreviewDlg->SetJobAction();
			if( m_nMatchingSoundIndex != -1 )
				CEtSoundEngine::GetInstance().PlaySound( "2D", m_nMatchingSoundIndex, false );

			break;
		}
	case LadderSystem::RoomState::Starting:
		{
			if(GetInterface().GetPVPLadderTabDlg())
			{
				GetInterface().GetPVPLadderTabDlg()->EnableTabButton(LadderSystem::MatchType::eCode::None,false,true);
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				GetInterface().GetPVPLadderTabDlg()->SetPVPLadderState( nState );
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
			}
			EnableSearchButton(false);
			EnalbeStopSearchButton(true,false);
			m_pButtonExit->Enable(false);
			break;
		}
	case LadderSystem::RoomState::Syncing:
		{
			if(GetInterface().GetPVPLadderTabDlg())
			{
				GetInterface().GetPVPLadderTabDlg()->EnableTabButton(LadderSystem::MatchType::eCode::None,false,true);
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				GetInterface().GetPVPLadderTabDlg()->SetPVPLadderState( nState );
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
			}
			EnableSearchButton(false);
			EnalbeStopSearchButton(true,false);
			m_pButtonExit->Enable(false);

			break;
		}
	default:
		break;
	}
}

void CDnPVPLadderSystemDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_EXIT"))	
		{
			if(GetInterface().GetPVPLadderTabDlg())
			{
				SendPvPLeaveChannel();

#if defined(PRE_ADD_DWC)
				if(GetDWCTask().IsDWCChar())
				{
					// 일반탭으로 이동
					if( GetInterface().GetPVPDWCTablDlg() )
						GetInterface().GetPVPDWCTablDlg()->SetDWCRadioButton(CDnPVPDWCTabDlg::eDWCChannel::eChannel_Normal);
				}
				else
				{
		#ifdef PRE_ADD_COLOSSEUM_BEGINNER
					CDnPVPLadderTabDlg* pPVPLadderTabDlg = GetInterface().GetPVPLadderTabDlg();
					if( pPVPLadderTabDlg )
					{
						pPVPLadderTabDlg->UpdateAndSelectTabProperly();
						m_bLeavenLadderByExitButton = true;
					}
		#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
				}
#else // #else PRE_ADD_DWC
		#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				CDnPVPLadderTabDlg* pPVPLadderTabDlg = GetInterface().GetPVPLadderTabDlg();
				if( pPVPLadderTabDlg )
				{
					pPVPLadderTabDlg->UpdateAndSelectTabProperly();
					m_bLeavenLadderByExitButton = true;
				}
		#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
#endif // PRE_ADD_DWC


			}
		}

		if( IsCmdControl("ID_BT_DNGIRL"))
		{
			if(m_nLadderState == LadderSystem::RoomState::WaitUser)
				SendPvPSearchLadder(false);
		}
		if(IsCmdControl("ID_BT_OUT"))
		{
			if(m_nLadderState == LadderSystem::RoomState::WaitMatching || m_nLadderState == LadderSystem::RoomState::Matched )
				SendPvPSearchLadder(true);
		}
		if(IsCmdControl("ID_REFRESH"))
		{
			RefreshRoomList(m_nLadderRoom_Page);
		}
		if(IsCmdControl("ID_NEXT"))
		{
			if(m_nLadderRoom_Page < m_nLadderRoom_MaxPage)
			{
				m_nLadderRoom_Page++;
				RefreshRoomList(m_nLadderRoom_Page);
			}
		}
		if(IsCmdControl("ID_PRIOR"))
		{
			if(m_nLadderRoom_Page > 1)
			{
				m_nLadderRoom_Page--;
				RefreshRoomList(m_nLadderRoom_Page);
			}
		}
		if(IsCmdControl("ID_BT_OBSERVERJOIN"))
		{
			if(m_nSelectedRoomIndex >= 0 && m_nSelectedRoomIndex <= (int)m_vecLadderRoomList.size())
			{
				if(m_vecLadderRoomList[m_nSelectedRoomIndex].pStaticTeam_A && m_vecLadderRoomList[m_nSelectedRoomIndex].pStaticTeam_A->IsShow())
				{
					std::wstring wszName;
					wszName = m_vecLadderRoomList[m_nSelectedRoomIndex].pStaticTeam_A->GetText();
					SendLadderObserverJoin(wszName);

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
					// 서버중심이 아니라 클라이언트에서 패킷도보내고 이렇게 브릿지를 통해서 임시로 정보를 저장하는 형태는 좀 안좋은것 같습니다.
					CDnBridgeTask *pBridgeTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
					CDnPVPLadderTabDlg* pPVPLadderTabDlg = GetInterface().GetPVPLadderTabDlg();
					if( pBridgeTask && pPVPLadderTabDlg )
					{	
						pBridgeTask->SetSelectedPVPGradeChannelOrLadder( pPVPLadderTabDlg->GetSelectedGradeChannel(), pPVPLadderTabDlg->GetSelectedChannel() );
					}
#endif

				}
				else
				{
					m_pStaticRoomSelect->Show(false); 
					m_pButtonObserverJoin->Enable(false); // 선택된 방이 끝나서 사라진경우 버튼을 눌르면 비활성화 시킨다.
				}
			}

		}
	}
}

bool CDnPVPLadderSystemDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	if( !IsShow() )
	{
		return false;
	}	

	switch( uMsg )
	{

	case WM_LBUTTONDOWN:
		{
			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			for( int i = 0 ; i < (int)m_vecLadderRoomList.size() ; ++i ) 
			{
				if( !m_vecLadderRoomList[i].pStaticDummy ) continue;
				SUICoord uiCoord;
				m_vecLadderRoomList[i].pStaticDummy->GetUICoord( uiCoord );

				if( uiCoord.IsInside( fMouseX, fMouseY ) ) 
				{
					m_pStaticRoomSelect->Show(false);
					m_pButtonObserverJoin->Enable(false);

					if(m_vecLadderRoomList[i].pStaticTeam_A->IsShow())
					{
						SUICoord ObserverCoord;
						m_pLadderObserverInfoDlg->GetDlgCoord(ObserverCoord);

						if(!m_pTeamGameDlg->GetLadderInviteUserListDlg()->IsShow() && m_nSelectedLadderType != LadderSystem::MatchType::_1vs1)
						{
							m_pLadderObserverInfoDlg->SetLadderUserInfo(m_nSelectedLadderType,i,&m_sPlayingRoomListInfo); // 레더타입 / 선택된슬롯 / 받은 레더 네임리스트.
							m_pLadderObserverInfoDlg->Show(true);
							m_pLadderObserverInfoDlg->SetPosition((uiCoord.fX - ObserverCoord.fWidth + GetXCoord()) ,( uiCoord.fY + uiCoord.fHeight/2 + GetYCoord()));
						}

						m_nSelectedRoomIndex = i;
						m_pStaticRoomSelect->Show(true);
						m_pStaticRoomSelect->SetUICoord(m_vecLadderRoomList[i].pStaticDummy->GetUICoord());
		
						if(m_nLadderState ==  LadderSystem::RoomState::WaitUser) // 대기 상태에서만 가능하며 , 2vs2 ,3vs3 일대는 추가 예외상황 처리해야함
							m_pButtonObserverJoin->Enable(true);
					}
					break;
				}
				else
				{
					m_pLadderObserverInfoDlg->Show(false);
				}
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnPVPLadderSystemDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
	if(!IsShow())
		return;

	m_nRoomRefreshDelay += fElapsedTime;

	if(m_nRoomRefreshDelay > RoomListRefreshTime::Default)
	{
		RefreshRoomList(m_nLadderRoom_Page);
		m_nRoomRefreshDelay = 0.f;
	}
}


void CDnPVPLadderSystemDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

	if(!bShow) 	m_pLadderObserverInfoDlg->Show(false);

	if(bShow && m_pPreviewDlg)
	{
		m_pPreviewDlg->RefreshPreview();
		RefreshRoomList();

#ifdef PRE_ADD_DWC
		if(GetDWCTask().IsDWCChar())
		{
			std::vector<TDWCTeam> vDwcTeamList = GetDWCTask().GetDwcTeamInfoList();
			if(!vDwcTeamList.empty()) SetDWCScoreInfo(vDwcTeamList[0]);
		}
		else
		{
			if(CDnPartyTask::IsActive())
				SetLadderScoreInfo(CDnPartyTask::GetInstance().GetPVPLadderInfo());
		}

		SetDWCModeControls();
#else	// #else PRE_ADD_DWC
		if(CDnPartyTask::IsActive())
			SetLadderScoreInfo(CDnPartyTask::GetInstance().GetPVPLadderInfo());
#endif	// #endif PRE_ADD_DWC
	}

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	if( bShow )
	{
		m_bLeavenLadderByExitButton = false;
	}
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
}

void CDnPVPLadderSystemDlg::EnableSearchButton(bool bShow,bool bEnable)
{
	if(m_pTeamGameDlg->IsShow() && !m_pTeamGameDlg->IsMasterUser())
	{
		bShow = false;
		bEnable = false;
	}
	else if(m_pTeamGameDlg->IsShow() && m_pTeamGameDlg->IsMasterUser() && !m_pTeamGameDlg->IsReady())
	{
		bEnable = false;
	}

	if(m_pButtonStartSearch )
	{
		m_pButtonStartSearch->Show(bShow);
		m_pButtonStartSearch->Enable(bEnable);

		switch(m_nSelectedLadderType)
		{
		case LadderSystem::MatchType::_1vs1:
			m_pButtonStartSearch->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126015 ));
			break;
		case LadderSystem::MatchType::_2vs2:
			m_pButtonStartSearch->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126030 ));
			break;
		case LadderSystem::MatchType::_3vs3:
			m_pButtonStartSearch->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126031 ));
			break;
		case LadderSystem::MatchType::_4vs4:
			m_pButtonStartSearch->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126032 ));
			break;
#ifdef PRE_ADD_DWC
		case LadderSystem::MatchType::_3vs3_DWC:
		case LadderSystem::MatchType::_3vs3_DWC_PRACTICE:
			m_pButtonStartSearch->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126031 ));
			break;
#endif
		default:
			m_pButtonStartSearch->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126015 ));
			break;
		}
	}

}
void CDnPVPLadderSystemDlg::EnalbeStopSearchButton(bool bShow, bool bEnable)
{
	if(m_pTeamGameDlg->IsShow() && !m_pTeamGameDlg->IsMasterUser())
	{
		bShow = false;
		bEnable = false;
	}
	if(m_pButtonStartSearch &&m_pButtonStartSearch)
	{
		m_pButtonStopSearch->Show(bShow);
		m_pButtonStopSearch->Enable(bEnable);
	}
}

#ifdef PRE_ADD_DWC
void CDnPVPLadderSystemDlg::SetDWCModeControls()
{
	bool bIsDWCUser = GetDWCTask().IsDWCChar();
	CEtUIStatic* pTempStatic = NULL;

	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT1"); // 예상 포인트 -> 이번주 경기수
	if(pTempStatic) {
		//pTempStatic->Show(!bIsDWCUser);
		pTempStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 126185)); // Mid: 이번주 경기수
	}
	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_EXPECT"); // 예상 포인트 카운트
	if(pTempStatic) {
		//pTempStatic->Show(!bIsDWCUser);
	}

	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT6");
	if(pTempStatic) pTempStatic->Show(!bIsDWCUser);
	
	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT7");
	if(pTempStatic) pTempStatic->Show(!bIsDWCUser);

	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_HIGHCLASS");
	if(pTempStatic) pTempStatic->Show(!bIsDWCUser);

	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_LOWCLASS");
	if(pTempStatic) pTempStatic->Show(!bIsDWCUser);

	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT2"); // 이번주 경기수
	if(pTempStatic) pTempStatic->Show(!bIsDWCUser);
	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_WEEKCOUNT"); // 이번주 경기수 카운트
	if(pTempStatic) pTempStatic->Show(!bIsDWCUser);
}
#endif // PRE_ADD_DWC
