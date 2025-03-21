#include "StdAfx.h"
#include "DnChatTabDlg.h"
#include "DnChatDlg.h"
#include "DnChatOptDlg.h"
#include "VillageSendPacket.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "PartySendPacket.h"
#include "EtUIXML.h"
#include "DebugCmdProc.h"
#include "DnChatHelpDlg.h"
#include "DnInterfaceString.h"
#include "DnLocalPlayerActor.h"
#include "MAChatBalloon.h"
#include "DnInvenTabDlg.h"
#include "DnPartyTask.h"
#include "DnTradeTask.h"
#include "DnMouseCursor.h"
#include "DnIsolate.h"
#include "DnHeadIcon.h"
#include "DNGestureTask.h"
#include "DnWorld.h"
#include "TaskManager.h"
#include "DnVillageTask.h"
#include "DnRestraintTask.h"
#include "DnPvPGameTask.h"
#include "GameOption.h"
#include "DnMissionTask.h"

#include "DnNameLinkMng.h"
#ifdef PRE_MOD_CHATBG
#include "DnChatBoardDlg.h"
#endif

#ifdef PRE_PRIVATECHAT_CHANNEL
#include "DNCountryUnicodeSet.h"
#include "DnChannelChatTask.h"
#include "DnPrivateChannelDlg.h"
#endif // PRE_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
#include "DnLocalDataMgr.h"
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#include "DnSimpleTooltipDlg.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define CHATDLG_EXT_SIZE	0.07f

CDnChatTabDlg::CDnChatTabDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUITabDialog(dialogType, pParentDialog, nID, pCallback)
	, m_pNormalTab(NULL)
#ifdef PRE_ADD_CHAT_RENEWAL
	, m_pWhisperTab(NULL) // 귓속말.
#endif
	, m_pPartyTab(NULL)
	, m_pGuildTab(NULL)
	, m_pSystemTab(NULL)
	, m_pButtonDummy(NULL)
	, m_nSizeMode(0)
	, m_fBasisY(0.0f)
	, m_pChatNormalDlg(NULL)
	, m_pChatPartyDlg(NULL)
	, m_pChatGuildDlg(NULL)
	, m_pChatSystemDlg(NULL)
	, m_pIMEEditBox(NULL)
	, m_pStaticMode(NULL)
	, m_nChatMode(CHAT_NORMAL)
#ifdef PRE_ADD_CHAT_RENEWAL	
	, m_pChatWhisperDlg(NULL) // 귓속말.
	, m_bChangeChatMode(false)
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	, m_pPrivateChannelTab(NULL)
	, m_pPrivateChannelDlg(NULL)
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	, m_pStaticPrivateChannelNoticeText(NULL)
	, m_pStaticPrivateChannelNoticeBalloon(NULL)
	, m_pStaticPrivateChannelTabHighlight(NULL)
	, m_bJoinPrivateChannelNotify(false)
	, m_fJoinPrivateChannelNotify(0.0f)
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
	, m_bShowOptDlg(false)
	, m_ChatDlgSize(SIZE_STEP_SMALL)
	, m_pButtonModeUp(NULL)
	, m_pButtonModeDown(NULL)
	, m_pButtonOption(NULL)
	, m_pButtonHelp(NULL)
	, m_pButtonReport(NULL)
	, m_bMouseIn(false)
	, m_bSetFocusEditBox(false)
	, m_pChatHelpDlg(NULL)
	, m_nRepeat(0)
	, m_IsPVPGameChat(false)
	, m_bPrivateModeChecker(false)
	, m_nPrivateAlarmSoundIndex(-1)
	, m_fPaperingRemainTime(0.0f)
	, m_bPVPLobbyChat(false)
	, m_nChatModeBeforePrivate(CHAT_NO)
	, m_bCustomizeChatMode(false)
	, m_nCustomizeModeType(-1)
	, m_wszCustomizeModeHeader(NULL)
	, m_dwCustomizeModeTextColor(-1)
	, m_bIgnoreShowFunc(false)
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
	, m_bSpammer(false)
#endif
#ifdef PRE_MOD_CHATBG
	, m_pChatBoardDlg(NULL)
#endif
#ifdef PRE_ADD_WORLD_MSG_RED
	, m_pWorldChatRedBoard(NULL)
	, m_pWorldChatRed(NULL)
	, m_fWorldMsgRedCoolTime(0.0f)
	, m_fWorldMsgRed1Sec(0.0f)
	, m_nBlinkCount(0)
#endif // PRE_ADD_WORLD_MSG_RED
{
#if defined(PRE_FIX_57852)
	m_bPvPLobbyChatOtherAreaInside = false;
#endif // PRE_FIX_57852
}

CDnChatTabDlg::~CDnChatTabDlg(void)
{
	SAFE_DELETE(m_pChatHelpDlg);
#ifdef PRE_MOD_CHATBG
	SAFE_DELETE(m_pChatBoardDlg);
#endif

	if (m_nPrivateAlarmSoundIndex == -1)
	{
		CEtSoundEngine::GetInstance().RemoveSound(m_nPrivateAlarmSoundIndex);
	}
}

void CDnChatTabDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);

	// 각각의 요청에 대한 시간 처리등은 모두 여기서 하고, 다이얼로그에선 처리된 내용(시간값 등)을 보여주기만 한다.
	std::list<float>::iterator iter = m_listRecentChatTime.begin();
	while (iter != m_listRecentChatTime.end())
	{
		*iter -= fElapsedTime;
		if (*iter <= 0.0f)
		{
			iter = m_listRecentChatTime.erase(iter);
			continue;
		}
		++iter;
	}

	if (m_fPaperingRemainTime > 0.0f)
		m_fPaperingRemainTime -= fElapsedTime;

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	UpdatePrivateChannelNotify(fElapsedTime);
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_WORLD_MSG_RED
	if (!m_bPVPLobbyChat && m_pWorldChatRed)
	{
		if (m_fWorldMsgRedCoolTime > 0.0f)
		{
			m_fWorldMsgRedCoolTime -= fElapsedTime;
			if (m_fWorldMsgRedCoolTime < 0.0f)
				m_fWorldMsgRedCoolTime = 0.0f;

			m_fWorldMsgRed1Sec -= fElapsedTime;
			if (m_nBlinkCount > 0)
			{
				if (m_fWorldMsgRed1Sec <= 0.0f)
				{
					m_fWorldMsgRed1Sec = 1.0f;
					m_pWorldChatRed->Show(false);
					m_nBlinkCount--;
				}
				else if (m_fWorldMsgRed1Sec < 0.5f)
				{
					m_pWorldChatRed->Show(true);
				}
			}
			else
			{
				if (m_fWorldMsgRed1Sec < 0.5f)
					m_pWorldChatRed->Show(true);
			}
		}
		else
		{
			m_pWorldChatRed->Show(false);
			m_pWorldChatRedBoard->Show(false);
		}
	}
#endif // PRE_ADD_WORLD_MSG_RED
}

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
void CDnChatTabDlg::UpdatePrivateChannelNotify(float fElapsedTime)
{
	if (!IsShow())
		return;

	if (m_bJoinPrivateChannelNotify)
	{
		if (m_fJoinPrivateChannelNotify >= 5.f)
		{
			m_bJoinPrivateChannelNotify = false;
			m_pStaticPrivateChannelNoticeText->Show(false);
			m_pStaticPrivateChannelNoticeBalloon->Show(false);
			m_pStaticPrivateChannelTabHighlight->Show(false);
		}
		else
		{
			static int nTimeBack;
			int nBlinkTime = int(m_fJoinPrivateChannelNotify * 5.0f);

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
				m_pStaticPrivateChannelNoticeText->Show(!m_pStaticPrivateChannelNoticeText->IsShow());
				m_pStaticPrivateChannelNoticeBalloon->Show(!m_pStaticPrivateChannelNoticeBalloon->IsShow());
				m_pStaticPrivateChannelTabHighlight->Show(!m_pStaticPrivateChannelTabHighlight->IsShow());
			}
		}

		m_fJoinPrivateChannelNotify += fElapsedTime;
	}
}

void CDnChatTabDlg::SetPrivateChannelNotify(bool bNotify)
{
	if (m_pStaticPrivateChannelNoticeText == NULL || m_pStaticPrivateChannelNoticeBalloon == NULL || m_pStaticPrivateChannelTabHighlight == NULL)
		return;

	if (bNotify)
	{
		m_bJoinPrivateChannelNotify = true;
		m_fJoinPrivateChannelNotify = 0.0f;
		m_pStaticPrivateChannelNoticeText->Show(true);
		m_pStaticPrivateChannelNoticeBalloon->Show(true);
		m_pStaticPrivateChannelTabHighlight->Show(true);

		if (!IsShow())
			Show(true);

		RequestFocus(m_pButtonOption);
		CDnMouseCursor::GetInstance().ShowCursor(false, true);
	}
	else
	{
		m_bJoinPrivateChannelNotify = false;
		m_fJoinPrivateChannelNotify = 0.0f;
		m_pStaticPrivateChannelNoticeText->Show(false);
		m_pStaticPrivateChannelNoticeBalloon->Show(false);
		m_pStaticPrivateChannelTabHighlight->Show(false);
	}
}

#endif // PRE_ADD_PRIVATECHAT_CHANNEL

void CDnChatTabDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("ChatTabDlg.ui").c_str(), bShow);
	SetFadeMode(CEtUIDialog::AllowRender);

	if (!CDnTableDB::GetInstancePtr())
		return;

	const char *szFileName = CDnTableDB::GetInstance().GetFileName(10015);
	if (strlen(szFileName) > 0)
	{
		m_nPrivateAlarmSoundIndex = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName(szFileName).c_str(), false, false);
	}
}

void CDnChatTabDlg::InitialUpdate()
{
	// Note : 일반 탭과 창
	m_pNormalTab = GetControl<CEtUIRadioButton>("ID_TAB_ALL");
	m_pChatNormalDlg = new CDnChatDlg(UI_TYPE_CHILD, this);
	m_pChatNormalDlg->Initialize(false);
	AddTabDialog(m_pNormalTab, m_pChatNormalDlg);
	m_vecChatDlg.push_back(m_pChatNormalDlg);

	// Note : 귓속말 탭과 창.
#ifdef PRE_ADD_CHAT_RENEWAL
	m_pWhisperTab = GetControl<CEtUIRadioButton>("ID_TAB_SECRET");
	m_pChatWhisperDlg = new CDnChatDlg(UI_TYPE_CHILD, this);
	m_pChatWhisperDlg->Initialize(false);
	AddTabDialog(m_pWhisperTab, m_pChatWhisperDlg);
	m_vecChatDlg.push_back(m_pChatWhisperDlg);
#endif

	// Note : 파티 탭과 창
	m_pPartyTab = GetControl<CEtUIRadioButton>("ID_TAB_PARTY");
	m_pChatPartyDlg = new CDnChatDlg(UI_TYPE_CHILD, this);
	m_pChatPartyDlg->Initialize(false);
	AddTabDialog(m_pPartyTab, m_pChatPartyDlg);
	m_vecChatDlg.push_back(m_pChatPartyDlg);

	// Note : 길드 탭과 창
	m_pGuildTab = GetControl<CEtUIRadioButton>("ID_TAB_GUILD");
#ifdef PRE_ADD_DWC
	if (GetDWCTask().IsDWCChar())
		m_pGuildTab->GetProperty()->nTooltipStringIndex = 121018;
#endif
	m_pChatGuildDlg = new CDnChatDlg(UI_TYPE_CHILD, this);
	m_pChatGuildDlg->Initialize(false);
	AddTabDialog(m_pGuildTab, m_pChatGuildDlg);
	m_vecChatDlg.push_back(m_pChatGuildDlg);


	// Note : 시스템 탭과 창
	m_pSystemTab = GetControl<CEtUIRadioButton>("ID_TAB_SYSTEM");
	m_pChatSystemDlg = new CDnChatDlg(UI_TYPE_CHILD, this);
	m_pChatSystemDlg->Initialize(false);
	AddTabDialog(m_pSystemTab, m_pChatSystemDlg);
	m_vecChatDlg.push_back(m_pChatSystemDlg);

#ifdef PRE_PRIVATECHAT_CHANNEL
	// Note : 사설 채널 탭과 창
	m_pPrivateChannelTab = GetControl<CEtUIRadioButton>("ID_TAB_CHANNEL");
	m_pPrivateChannelDlg = new CDnChatDlg(UI_TYPE_CHILD, this);
	m_pPrivateChannelDlg->Initialize(false);
	AddTabDialog(m_pPrivateChannelTab, m_pPrivateChannelDlg);
	m_vecChatDlg.push_back(m_pPrivateChannelDlg);
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	m_pStaticPrivateChannelNoticeText = GetControl<CEtUIStatic>("ID_TEXT_CHANNEL");
	m_pStaticPrivateChannelNoticeText->Initialize(false);
	m_pStaticPrivateChannelNoticeBalloon = GetControl<CEtUIStatic>("ID_STATIC_CHANNEL");
	m_pStaticPrivateChannelNoticeBalloon->Initialize(false);
	m_pStaticPrivateChannelTabHighlight = GetControl<CEtUIStatic>("ID_STATIC_CHANNELCOVER");
	m_pStaticPrivateChannelTabHighlight->Initialize(false);
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	m_pChatNormalDlg->InitOption(CHAT_NORMAL);
	m_pChatPartyDlg->InitOption(CHAT_PARTY);
	m_pChatGuildDlg->InitOption(CHAT_GUILD);
	m_pChatSystemDlg->InitOption(CHAT_SYSTEM);
#ifdef PRE_ADD_CHAT_RENEWAL
	m_pChatWhisperDlg->InitOption(CHAT_PRIVATE);
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_pPrivateChannelDlg->InitOption(CHAT_PRIVATE_CHANNEL);
#endif // PRE_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	CDnLocalDataMgr::GetInstance().SetChatOption(m_pChatNormalDlg->GetChatOptDlg().GetChatTabType(), m_pChatNormalDlg->GetChatOptDlg().GetOption());
	CDnLocalDataMgr::GetInstance().SetChatOption(m_pChatPartyDlg->GetChatOptDlg().GetChatTabType(), m_pChatPartyDlg->GetChatOptDlg().GetOption());
	CDnLocalDataMgr::GetInstance().SetChatOption(m_pChatGuildDlg->GetChatOptDlg().GetChatTabType(), m_pChatGuildDlg->GetChatOptDlg().GetOption());
	CDnLocalDataMgr::GetInstance().SetChatOption(m_pChatSystemDlg->GetChatOptDlg().GetChatTabType(), m_pChatSystemDlg->GetChatOptDlg().GetOption());
	CDnLocalDataMgr::GetInstance().SetChatOption(m_pChatWhisperDlg->GetChatOptDlg().GetChatTabType(), m_pChatWhisperDlg->GetChatOptDlg().GetOption());
	CDnLocalDataMgr::GetInstance().SetChatOption(m_pPrivateChannelDlg->GetChatOptDlg().GetChatTabType(), m_pPrivateChannelDlg->GetChatOptDlg().GetOption());
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	SetCheckedTab(m_pNormalTab->GetTabID());

	m_pButtonDummy = GetControl<CEtUIButton>("ID_BUTTON_DUMMY");

	m_mapCommandMode.insert(make_pair(std::wstring(L"/s "), CHAT_NORMAL));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/S "), CHAT_NORMAL));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/1 "), CHAT_NORMAL));

	m_mapCommandMode.insert(make_pair(std::wstring(L"/p "), CHAT_PARTY));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/P "), CHAT_PARTY));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/2 "), CHAT_PARTY));

	m_mapCommandMode.insert(make_pair(std::wstring(L"/g "), CHAT_GUILD));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/G "), CHAT_GUILD));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/3 "), CHAT_GUILD));

	m_mapCommandMode.insert(make_pair(std::wstring(L"/w "), CHAT_PRIVATE));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/W "), CHAT_PRIVATE));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/4 "), CHAT_PRIVATE));

#ifdef PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert(make_pair(std::wstring(L"/c "), CHAT_PRIVATE_CHANNEL));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/C "), CHAT_PRIVATE_CHANNEL));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/5 "), CHAT_PRIVATE_CHANNEL));
#else // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert(make_pair(std::wstring(L"/c "), CHAT_CHANNEL));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/C "), CHAT_CHANNEL));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/5 "), CHAT_CHANNEL));

	m_mapCommandMode.insert(make_pair(std::wstring(L"/t "), CHAT_WORLD));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/T "), CHAT_WORLD));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/6 "), CHAT_WORLD));
#endif // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert(make_pair(std::wstring(L"/r "), CHAT_PRIVATE_REPLY));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/R "), CHAT_PRIVATE_REPLY));

#if defined(_KRAZ) || defined(_RDEBUG)
	m_mapCommandMode.insert(make_pair(std::wstring(L"/ㄴ "), CHAT_NORMAL));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/ㅔ "), CHAT_PARTY));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/ㅎ "), CHAT_GUILD));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/ㅈ "), CHAT_PRIVATE));

#ifdef PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert(make_pair(std::wstring(L"/ㅊ "), CHAT_PRIVATE_CHANNEL));
#else // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert(make_pair(std::wstring(L"/ㅊ "), CHAT_CHANNEL));
	m_mapCommandMode.insert(make_pair(std::wstring(L"/ㅅ "), CHAT_WORLD));
#endif // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert(make_pair(std::wstring(L"/ㄱ "), CHAT_PRIVATE_REPLY));
#endif

	m_mapCommandMessage.insert(make_pair(std::wstring(L"/?"), CMD_MSG_HELP));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/invite"), CMD_MSG_INVITE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/makeparty"), CMD_MSG_MAKEPARTY));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/leaveparty"), CMD_MSG_LEAVEPARTY));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/ban"), CMD_MSG_BAN));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/trade"), CMD_MSG_TRADE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/escape"), CMD_MSG_ESCAPE));
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/chcreate"), CMD_MSG_CHANNEL_CREATE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/chjoin"), CMD_MSG_CHANNEL_JOIN));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/chpassword"), CMD_MSG_CHANNEL_PASSWORD));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/chinvite"), CMD_MSG_CHANNEL_INVITE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/chleave"), CMD_MSG_CHANNEL_LEAVE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/chban"), CMD_MSG_CHANNEL_BAN));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/chmaster"), CMD_MSG_CHANNEL_MASTER));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/chhelp"), CMD_MSG_CHANNEL_HELP));
#endif // PRE_PRIVATECHAT_CHANNEL

#if defined(_KRAZ) || defined(_RDEBUG)
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/파티초대"), CMD_MSG_INVITE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/파티생성"), CMD_MSG_MAKEPARTY));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/파티탈퇴"), CMD_MSG_LEAVEPARTY));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/파티추방"), CMD_MSG_BAN));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/거래"), CMD_MSG_TRADE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/탈출"), CMD_MSG_ESCAPE));
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/채널개설"), CMD_MSG_CHANNEL_CREATE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/채널참여"), CMD_MSG_CHANNEL_JOIN));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/채널비밀번호"), CMD_MSG_CHANNEL_PASSWORD));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/채널초대"), CMD_MSG_CHANNEL_INVITE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/나가기"), CMD_MSG_CHANNEL_LEAVE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/채널추방"), CMD_MSG_CHANNEL_BAN));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/방장"), CMD_MSG_CHANNEL_MASTER));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/채널도움말"), CMD_MSG_CHANNEL_HELP));
#endif // PRE_PRIVATECHAT_CHANNEL
#endif

	m_mapCommandMessage.insert(make_pair(std::wstring(L"/y"), CMD_MSG_RAIDNOTICE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/Y"), CMD_MSG_RAIDNOTICE));
	m_mapCommandMessage.insert(make_pair(std::wstring(L"/ㅛ"), CMD_MSG_RAIDNOTICE));

	m_pStaticMode = GetControl<CEtUIStatic>("ID_STATIC_MODE");
	m_pIMEEditBox = GetControl<CEtUIIMEEditBox>("ID_EDITBOX_CHAT");
	m_pButtonModeUp = GetControl<CEtUIButton>("ID_BUTTON_MODE_UP");
	m_pButtonModeDown = GetControl<CEtUIButton>("ID_BUTTON_MODE_DOWN");
	m_pButtonOption = GetControl<CEtUIButton>("ID_BUTTON_OPTION");
	m_pButtonHelp = GetControl<CEtUIButton>("ID_BUTTON_HELP");
	m_pButtonReport = GetControl<CEtUIButton>("ID_BUTTON_REPORT");
#ifdef PRE_MOD_CHATBG
	m_pChatBoardDlg = new CDnChatBoardDlg(UI_TYPE_BOTTOM);
	m_pChatBoardDlg->Initialize(false);
#endif
	m_pButtonReport->Show(false);

	m_pIMEEditBox->ForceRenderCandidateReadingWindowBottomPosition(true);

	SetChatModeText();

	m_pChatHelpDlg = new CDnChatHelpDlg(UI_TYPE_CHILD, this, CHAT_HELP_DIALOG, this);
	m_pChatHelpDlg->Initialize(false);

	m_pChatHelpDlg->AddChatHelpText(L"/s", L"/1", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 501), CHAT_NORMAL, chatcolor::NORMAL);
	m_pChatHelpDlg->AddChatHelpText(L"/p", L"/2", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 632), CHAT_PARTY, chatcolor::PARTY);
#ifdef PRE_ADD_DWC
	int nMid = 5007; // Mid: 길드
	if (GetDWCTask().IsDWCChar()) nMid = 121018; // Mid: 팀
	m_pChatHelpDlg->AddChatHelpText(L"/g", L"/3", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nMid), CHAT_GUILD, chatcolor::GUILD);
#else
	m_pChatHelpDlg->AddChatHelpText(L"/g", L"/3", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5007), CHAT_GUILD, chatcolor::GUILD);
#endif // PRE_ADD_DWC

	m_pChatHelpDlg->AddChatHelpText(L"/w", L"/4", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 648), CHAT_PRIVATE, chatcolor::PRIVATE);
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_pChatHelpDlg->AddChatHelpText(L"/c", L"/5", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1202), CHAT_PRIVATE_CHANNEL, chatcolor::PRIVATECHANNEL);
#endif // PRE_PRIVATECHAT_CHANNEL
	m_bPrivateModeChecker = false;

#ifdef PRE_ADD_WORLD_MSG_RED
	m_pWorldChatRedBoard = GetControl<CEtUIStatic>("ID_STATIC_WORLDCHATBOARD");
	m_pWorldChatRed = GetControl<CEtUITextBox>("ID_TEXTBOX_WORLDCHAT");
	SUIControlProperty* pProperty = m_pWorldChatRed->GetProperty();
	pProperty->TextBoxProperty.bVerticalScrollBar = false;
#endif // PRE_ADD_WORLD_MSG_RED
}

bool CDnChatTabDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;

	if (IsShow())
	{
		if (uMsg == WM_KEYDOWN)
		{
			if (GetKeyState(VK_CONTROL) < 0)
			{
#ifdef PRE_PRIVATECHAT_CHANNEL
				WCHAR checkChar[] = { '1', '2', '3', '4', '5' };
#ifdef PRE_ADD_CHAT_RENEWAL				
				int chatMode[] = { CHAT_NORMAL, CHAT_PARTY, CHAT_GUILD, CHAT_PRIVATE_CHANNEL, CHAT_PRIVATE };
#else // PRE_ADD_CHAT_RENEWAL			
				int chatMode[] = { CHAT_NORMAL, CHAT_PARTY, CHAT_GUILD, CHAT_PRIVATE, CHAT_PRIVATE_CHANNEL };
#endif // PRE_ADD_CHAT_RENEWAL
#else // PRE_PRIVATECHAT_CHANNEL
				WCHAR checkChar[] = { '1', '2', '3', '4', '5', '6' };
#ifdef PRE_ADD_CHAT_RENEWAL				
				int chatMode[] = { CHAT_NORMAL, CHAT_PARTY, CHAT_GUILD, CHAT_CHANNEL, CHAT_PRIVATE, CHAT_WORLD };
#else // PRE_ADD_CHAT_RENEWAL				
				int chatMode[] = { CHAT_NORMAL, CHAT_PARTY, CHAT_GUILD, CHAT_PRIVATE, CHAT_CHANNEL, CHAT_WORLD };
#endif // PRE_ADD_CHAT_RENEWAL
#endif // PRE_PRIVATECHAT_CHANNEL

				_ASSERT(_countof(checkChar) == _countof(chatMode));

				int i = 0;
				for (; i < _countof(checkChar); ++i)
				{
					if (wParam == checkChar[i])
					{
						SetChatMode(chatMode[i]);
						return true;
					}
				}
			}

			switch (wParam)
			{
			case VK_TAB:
			{
				if (CDnMouseCursor::IsActive() && !CDnMouseCursor::GetInstance().IsShowCursor())
					break;

				if (GetChatMode() == CHAT_PRIVATE)
				{
					ChangePrivateName();
				}

				RequestFocus(m_pIMEEditBox);
				return true;
			}
			break;

			case VK_ESCAPE:
			case VK_RETURN:
			{
				if (IsShow() && m_pIMEEditBox->GetTextLength() <= 0)
				{
					ShowEx(false);
					return true;
				}
			}
			break;
			}
		}

		if (!m_bPVPLobbyChat)
		{
			// 채팅탭 세로 사이즈 조절
			POINT MousePoint;
			MousePoint.x = short(LOWORD(lParam));
			MousePoint.y = short(HIWORD(lParam));

			float fMouseX, fMouseY;
			PointToFloat(MousePoint, fMouseX, fMouseY);

			if (uMsg == WM_MOUSEMOVE)
			{
				if (m_nSizeMode == SIZE_BASE)
				{
					if (m_pButtonDummy && m_pButtonDummy->IsInside(fMouseX, fMouseY))
					{
						// 추가로 다른데서 누른 상태로 들어왔는지 검사해야한다.
						if (!CDnMouseCursor::GetInstance().IsPushMouseButton(0))
						{
							m_nSizeMode = SIZE_OVER;
							CDnMouseCursor::GetInstance().SetCursorState(CDnMouseCursor::SIZE);
						}
					}
				}
				else if (m_nSizeMode == SIZE_OVER)
				{
					if (m_pButtonDummy && !m_pButtonDummy->IsInside(fMouseX, fMouseY))
					{
						m_nSizeMode = SIZE_BASE;
						CDnMouseCursor::GetInstance().SetCursorState(CDnMouseCursor::NORMAL);
					}
				}
				else if (m_nSizeMode == SIZE_DRAG)
				{
					// y변경에 따라 채팅다이얼로그 조절.
					float fDelta = m_fBasisY - fMouseY;
					ResizeChatBox(fDelta);

					// 예외처리. 누르고 게임창 밖에서 마우스를 떼고 들어왔을 경우를 체크한다.
					if (m_pButtonDummy && !m_pButtonDummy->IsInside(fMouseX, fMouseY))
					{
						if (!CDnMouseCursor::GetInstance().IsPushMouseButton(0))
						{
							m_nSizeMode = SIZE_BASE;
							CDnMouseCursor::GetInstance().SetCursorState(CDnMouseCursor::NORMAL);
						}
					}
				}
			}
			else if (uMsg == WM_LBUTTONDOWN)
			{
				if (IsMouseInDlg())
					SetRenderPriority(this, true);

				if (m_nSizeMode == SIZE_OVER)
				{
					m_nSizeMode = SIZE_DRAG;
					m_fBasisY = fMouseY;
				}

#ifdef PRE_ADD_WORLD_MSG_RED
				bool isInside4 = false;
				if (m_fWorldMsgRedCoolTime > 0.0f && !m_bPVPLobbyChat && m_pWorldChatRed)
				{
					float fPTMouseX, fPTMouseY;
					PointToFloat(MousePoint, fPTMouseX, fPTMouseY);
					if (m_pWorldChatRed->IsInside(fPTMouseX, fPTMouseY))
						m_pWorldChatRedBoard->Show(true);
				}
#endif // PRE_ADD_WORLD_MSG_RED
			}
			else if (uMsg == WM_LBUTTONUP)
			{
				if (m_nSizeMode == SIZE_DRAG)
				{
					if (m_pButtonDummy && m_pButtonDummy->IsInside(fMouseX, fMouseY))
						m_nSizeMode = SIZE_OVER;
					else
					{
						m_nSizeMode = SIZE_BASE;
						CDnMouseCursor::GetInstance().SetCursorState(CDnMouseCursor::NORMAL);
					}
				}
			}
		}
	}

	if (!IsShow() && !m_bPassMessageToChild)
	{
		return false;
	}

#ifdef PRE_FIX_57852
	bool bExeMsgProc = false;
	bool bNeedFocusEditBox = false;
	if (IsShow())
	{
		bExeMsgProc = true;
	}
	else
	{
#if defined(PRE_ADD_68286)
		bool isAlarmIconClick = false;
#endif // PRE_ADD_68286

		if (uMsg == WM_LBUTTONDOWN)
		{
			POINT MousePoint;
			MousePoint.x = short(LOWORD(lParam));
			MousePoint.y = short(HIWORD(lParam));

			float fMouseX, fMouseY;
			//PointToFloat( MousePoint, fMouseX, fMouseY );

			fMouseX = MousePoint.x / GetScreenWidth();
			fMouseY = MousePoint.y / GetScreenHeight();

			//////////////////////////////////////////////////////////////////////////
			bool isInside2 = false;
			bool isInside1 = false;
			bool isInside3 = false;

			SUICoord normalDlgCoord;
			SUICoord optDlgCoord;

			CDnChatOptDlg &chatOptDlg = m_pChatNormalDlg->GetChatOptDlg();

			m_pChatNormalDlg->GetDlgCoord(normalDlgCoord);
			normalDlgCoord.fX = m_pChatNormalDlg->GetXCoord();
			normalDlgCoord.fY = m_pChatNormalDlg->GetYCoord();
			isInside1 = normalDlgCoord.IsInside(fMouseX, fMouseY);

			chatOptDlg.GetDlgCoord(optDlgCoord);
			optDlgCoord.fX = chatOptDlg.GetXCoord();
			optDlgCoord.fY = chatOptDlg.GetYCoord();
			if (chatOptDlg.IsShow() && optDlgCoord.IsInside(fMouseX, fMouseY))
				isInside1 = true;
			//////////////////////////////////////////////////////////////////////////

#if defined(PRE_MOD_CHATBG)
			SUICoord chatBoardCoord;
			if (m_pChatBoardDlg)
			{
				m_pChatBoardDlg->GetDlgCoord(chatBoardCoord);
				chatBoardCoord.fX = m_pChatBoardDlg->GetXCoord();
				chatBoardCoord.fY = m_pChatBoardDlg->GetYCoord();
				isInside2 = chatBoardCoord.IsInside(fMouseX, fMouseY);
			}

#endif // PRE_MOD_CHATBG
			if (m_pIMEEditBox)
				isInside3 = m_pIMEEditBox->IsInside(fMouseX, fMouseY);

#if defined(PRE_ADD_68286)
			if (GetInterface().IsAlarmIconClick(MousePoint) == true)
			{
				isInside1 = isInside2 = isInside3 = false;
				isAlarmIconClick = true;
			}
#endif // PRE_ADD_68286

#ifdef PRE_ADD_WORLD_MSG_RED
			bool isInside4 = false;
			if (m_fWorldMsgRedCoolTime > 0.0f && !m_bPVPLobbyChat && m_pWorldChatRed)
			{
				float fPTMouseX, fPTMouseY;
				PointToFloat(MousePoint, fPTMouseX, fPTMouseY);
				isInside4 = m_pWorldChatRed->IsInside(fPTMouseX, fPTMouseY);
			}

			if (isInside1 || isInside2 || isInside3 || isInside4)
#else // PRE_ADD_WORLD_MSG_RED
			if (isInside1 || isInside2 || isInside3)
#endif // PRE_ADD_WORLD_MSG_RED
			{
				ShowEx(true);
				bNeedFocusEditBox = true;
				bExeMsgProc = true;
			}
		}

		//PvP 채팅창에서 오른쪽 하단에서의 마우스 이벤트 처리를 위해서
		if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONUP || uMsg == WM_LBUTTONDOWN)
		{
			//PvP로비 채팅에서 오른쪽 영역
			if (m_bPVPLobbyChat && m_bPvPLobbyChatOtherAreaInside)
			{
				bNeedFocusEditBox = false;
				bExeMsgProc = true;
			}
		}

#if defined(PRE_ADD_68286)
		if (isAlarmIconClick == true)
		{
			bNeedFocusEditBox = false;
			bExeMsgProc = false;
		}
#endif // PRE_ADD_68286
	}

	if (bExeMsgProc)
	{
		bool bMsgRet = CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
		if (bNeedFocusEditBox)
			RequestFocus(m_pIMEEditBox);

		return bMsgRet;
	}
	else
		return false;
#else
	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
#endif
}

void CDnChatTabDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	CDnChatDlg *pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());

	if (nCommand == EVENT_RADIOBUTTON_CHANGED)
	{
		if (pChatDlg)
		{
			pChatDlg->ShowOptDlg(false);
		}
	}

	CEtUITabDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);

	pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
	if (!pChatDlg)
		return;

	if (nCommand == EVENT_EDITBOX_KEYUP)
	{
		SetNextChatHistory();
		return;
	}

	if (nCommand == EVENT_EDITBOX_KEYDOWN)
	{
		SetPrevChatHistory();
		return;
	}

	if (nCommand == EVENT_EDITBOX_SHIFT_KEYUP)
	{
		SetChatNextMode();
		return;
	}

	if (nCommand == EVENT_EDITBOX_SHIFT_KEYDOWN)
	{
		SetChatPrevMode();
		return;
	}

	if (nCommand == EVENT_EDITBOX_LCTRL_KEYUP)
	{
		// 탭 교체
		int nNextTab = GetCurrentTabID() + 1;
		if (nNextTab > 4) nNextTab = 1;
		SetCheckedTab(nNextTab);
		return;
	}

	if (nCommand == EVENT_EDITBOX_LCTRL_KEYDOWN)
	{
		// 탭 교체
		int nNextTab = GetCurrentTabID() - 1;
		if (nNextTab < 1) nNextTab = 4;
		SetCheckedTab(nNextTab);
		return;
	}

	if (nCommand == EVENT_EDITBOX_STRING)
	{
		if (!m_bSetFocusEditBox)
		{
			if (AddEditBoxString())
			{
				ShowEx(false);
			}
		}
		else
		{
			m_bSetFocusEditBox = false;
		}

		return;
	}

	if (nCommand == EVENT_EDITBOX_ESCAPE)
	{
		ShowEx(false);
		return;
	}

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_MODE_UP"))
		{
			SetChatNextMode();
			return;
		}

		if (IsCmdControl("ID_BUTTON_MODE_DOWN"))
		{
			SetChatPrevMode();
			return;
		}

		if (IsCmdControl("ID_BUTTON_OPTION"))
		{
			bool bShow = pChatDlg->IsShowOptDlg();
			pChatDlg->ShowOptDlg(!bShow);
			m_bShowOptDlg = !bShow;
			return;
		}

		if (IsCmdControl("ID_BUTTON_HELP"))
		{
			bool bShow = m_pChatHelpDlg->IsShow();
			ShowChildDialog(m_pChatHelpDlg, !bShow);
			return;
		}

		if (IsCmdControl("ID_BUTTON_REPORT"))
		{
			return;
		}
	}

	if (nCommand == EVENT_RADIOBUTTON_CHANGED)
	{
		if (pChatDlg->GetChatType() != CHAT_SYSTEM)
		{
#ifdef PRE_ADD_CHAT_RENEWAL
			m_bChangeChatMode = true;
#endif
			SetChatMode(pChatDlg->GetChatType());
		}

		if (m_bShowOptDlg)
		{
			pChatDlg->ShowOptDlg(true);
		}

#ifdef PRE_ADD_CHAT_RENEWAL
		// #65714 파티채팅자동전환 선택후 파티가입시, 채팅창의 스크롤이 생기는 현상.
		if (IsShow())
#endif 
			pChatDlg->ShowScrollBar(true);

		RequestFocus(m_pIMEEditBox);
		return;
	}
#ifdef PRE_ADD_WORLD_MSG_RED
	if (!m_bPVPLobbyChat && nCommand == EVENT_TEXTBOX_SELECTION)
	{
		if (IsCmdControl("ID_TEXTBOX_WORLDCHAT"))
		{
			CWord selectedWord = m_pWorldChatRed->GetSelectedWordData();
			if (selectedWord.m_strWordWithTag.empty() == false)
				EtInterface::GetNameLinkMgr().TranslateText(std::wstring(), selectedWord.m_strWordWithTag.c_str(), this);
		}
	}
#endif // PRE_ADD_WORLD_MSG_RED
}

#ifdef PRE_ADD_WORLD_MSG_RED
bool CDnChatTabDlg::OnParseTextItemInfo(const std::wstring& argString)
{
	CDnNameLinkMng* pMgr = GetInterface().GetNameLinkMng();
	if (pMgr)
	{
		CDnItem* pLinkItem = pMgr->MakeItem(argString);
		if (pLinkItem)
		{
			CDnTooltipDlg* pToolTipDlg = GetInterface().GetNameLinkToolTipDlg();
			if (pToolTipDlg)
			{
				ITEM_SLOT_TYPE slotType = ST_ITEM_NONE;
				if (pLinkItem->GetType() == MIInventoryItem::Item)
					slotType = ST_INVENTORY;
				else if (pLinkItem->GetType() == MIInventoryItem::Skill)
					slotType = ST_SKILL;
				pToolTipDlg->ShowTooltip(pLinkItem, pLinkItem->GetType(), slotType, GetScreenWidthRatio() * 0.5f, GetScreenHeightRatio() * 0.83f, true);
			}
		}
	}

	return true;
}
#endif // PRE_ADD_WORLD_MSG_RED

void CDnChatTabDlg::SetChatGameMode(bool isPVP)
{
	m_IsPVPGameChat = isPVP;

	if (m_IsPVPGameChat)
	{
		if (m_pPartyTab)
		{
#ifdef PRE_FIX_COLOCHAT_ICON
#else
			m_pPartyTab->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 121018));
#endif
			SetChatMode(CHAT_PARTY);
			//SetChatModeText();
			SetCheckedTab(m_pPartyTab->GetTabID());
			ShowEx(true);
			ShowEx(false);
		}
	}
#ifdef PRE_FIX_COLOCHAT_ICON
#else
	else
	{
		if (m_pPartyTab)
			m_pPartyTab->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 632));

	}
#endif
}

void CDnChatTabDlg::Render(float fElapsedTime)
{
	if (IsShow())
		CheckCommandMode();

	CEtUIDialog::Render(fElapsedTime);
}

void CDnChatTabDlg::ResizeChatBox(float fHeightDelta)
{
	// 우선 크기는 임의대로 설정하겠다. 차후 변경 필요.
	if (m_DlgInfo.DlgCoord.fHeight + fHeightDelta < 0.24f || m_DlgInfo.DlgCoord.fHeight + fHeightDelta > 0.6f)
		return;

	UpdateDlgCoord(0, 0, 0, fHeightDelta);
	SetPosChatDlg(fHeightDelta);
	SetPosBottomCtl(fHeightDelta);
#ifdef PRE_MOD_CHATBG
	if (m_pChatBoardDlg)
		m_pChatBoardDlg->UpdateDlgCoord(0, 0, 0, fHeightDelta);
#endif
}

void CDnChatTabDlg::ResizeIMECCtl(LPCWSTR wszHeader)
{
	DWORD dwTextColor(chatcolor::NORMAL);

	switch (GetChatMode())
	{
	case CHAT_NORMAL:	dwTextColor = chatcolor::NORMAL;	break;
	case CHAT_PRIVATE:	dwTextColor = chatcolor::PRIVATE;	break;
	case CHAT_PARTY:	dwTextColor = chatcolor::PARTY;		break;
	case CHAT_GUILD:	dwTextColor = chatcolor::GUILD;		break;
	case CHAT_CHANNEL:	dwTextColor = chatcolor::CHANNEL;	break;
	case CHAT_WORLD:	dwTextColor = chatcolor::WORLD;		break;
#ifdef PRE_PRIVATECHAT_CHANNEL
	case CHAT_PRIVATE_CHANNEL:	dwTextColor = chatcolor::PRIVATECHANNEL;	break;
#endif // PRE_PRIVATECHAT_CHANNEL
	default:
		ASSERT(0 && "CDnChatTabDlg::ResizeIMECCtl");
		break;
	}

	wchar_t wszMsg[256] = { 0 };

	if (m_bCustomizeChatMode && m_nCustomizeModeType == GetChatMode())
	{
		swprintf_s(wszMsg, 256, L"[%s]:", m_wszCustomizeModeHeader);

		m_pIMEEditBox->SetTextColor(m_dwCustomizeModeTextColor);
		m_pIMEEditBox->SetCompTextColor(m_dwCustomizeModeTextColor);
		m_pStaticMode->SetTextColor(m_dwCustomizeModeTextColor);

		m_pStaticMode->SetText(wszMsg);
	}
	else
	{
		m_pIMEEditBox->SetTextColor(dwTextColor);
		m_pIMEEditBox->SetCompTextColor(dwTextColor);
		m_pStaticMode->SetTextColor(dwTextColor);

		m_pStaticMode->SetText(wszHeader);
	}

	SUICoord sTextCoord;
	CalcTextRect(wszHeader, m_pStaticMode->GetElement(0), sTextCoord);
	sTextCoord.fWidth += 0.001f;

	SUICoord sCtlCoord;
	m_pStaticMode->GetUICoord(sCtlCoord);
	float fTemp = sTextCoord.fWidth - sCtlCoord.fWidth;
	sCtlCoord.fWidth = sTextCoord.fWidth;
	m_pStaticMode->SetUICoord(sCtlCoord);

	m_pIMEEditBox->GetUICoord(sCtlCoord);
	sCtlCoord.fX += fTemp;
	sCtlCoord.fWidth -= fTemp;
	m_pIMEEditBox->SetUICoord(sCtlCoord);
}

void CDnChatTabDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();
	SetChatModeText();
}

void CDnChatTabDlg::SetPosChatDlg(float fResizeRatio)
{
	int nVecSize = (int)m_vecChatDlg.size();
	for (int i = 0; i<nVecSize; i++)
	{
		if (m_vecChatDlg[i])
		{
			m_vecChatDlg[i]->UpdateDlgCoord(0, 0, 0, fResizeRatio);
		}
	}

	m_pChatHelpDlg->UpdateDlgCoord(0, -fResizeRatio, 0, 0);
}

void CDnChatTabDlg::SetPosBottomCtl(float fResizeRatio)
{
	m_pButtonModeUp->UpdateUICoord(0, fResizeRatio, 0, 0);
	m_pButtonModeDown->UpdateUICoord(0, fResizeRatio, 0, 0);
	m_pStaticMode->UpdateUICoord(0, fResizeRatio, 0, 0);
	m_pIMEEditBox->UpdateUICoord(0, fResizeRatio, 0, 0);
	m_pButtonReport->UpdateUICoord(0, fResizeRatio, 0, 0);
}

void CDnChatTabDlg::CheckCommandMode()
{
	std::wstring strCmd;
	COMMANDMODE_MAP_ITER iter;
	m_bPrivateModeChecker = false;

	strCmd = m_pIMEEditBox->GetText();

	std::wstring::size_type idx = strCmd.find_first_of(L" ");
	if (idx != std::wstring::npos)
	{
		std::wstring checkStr = strCmd.substr(0, idx + 1);

		iter = m_mapCommandMode.find(checkStr);
		if (iter != m_mapCommandMode.end())
		{
			CDnChatDlg *pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
			if (pChatDlg)
			{
				int chatType = iter->second;
				if (chatType & CHAT_PRIVATE_REPLY)
				{
					std::wstring strLastName;
					if (GetLastPrivateName(strLastName))
					{
						SetPrivateName(strLastName);
					}
					return;
				}

				if (chatType & CHAT_PARTY)
				{
					CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
					if (pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm)
						return;
				}

				CDnChatOptDlg &chatOptDlg = pChatDlg->GetChatOptDlg();
				int nOption = chatOptDlg.GetOption();

				// 모드 강제변경.
				// 파티탭 상태에서 일반에 체크가 안되어있고, 이때 /s 하면 일반탭으로 전환된다. 필요한가..
				if (!(nOption & chatType))
				{
					switch (chatType)
					{
					case CHAT_NORMAL:	SetCheckedTab(m_pNormalTab->GetTabID());	break;
					case CHAT_PARTY:	SetCheckedTab(m_pPartyTab->GetTabID());	break;
					case CHAT_GUILD:	SetCheckedTab(m_pGuildTab->GetTabID());	break;
#ifdef PRE_ADD_CHAT_RENEWAL
					case CHAT_PRIVATE:	SetCheckedTab(m_pWhisperTab->GetTabID());	break;
#else
					case CHAT_PRIVATE:	SetCheckedTab(m_pNormalTab->GetTabID());	break;
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
					case CHAT_PRIVATE_CHANNEL:	SetCheckedTab(m_pPrivateChannelTab->GetTabID()); break;
#endif // PRE_PRIVATECHAT_CHANNEL
					case CHAT_CHANNEL:	SetCheckedTab(m_pNormalTab->GetTabID());	break;
					case CHAT_WORLD:	SetCheckedTab(m_pNormalTab->GetTabID());	break;
					}
				}

				if (chatType == CHAT_PRIVATE)
				{
					m_bPrivateModeChecker = true;
				}
				else
				{
					SetChatMode(chatType);
					m_pIMEEditBox->ClearText();
					RequestFocus(m_pIMEEditBox);
				}
			}
		}

		if (m_bPrivateModeChecker)
		{
			// Note : 상대이름이 입력될때 까지 모드를 바꾸지 않는다.
			//
			std::wstring::size_type begIdx, endIdx;

			begIdx = strCmd.find_first_of(L" ");
			begIdx = (begIdx != std::wstring::npos) ? ++begIdx : std::wstring::npos;
			endIdx = strCmd.find_first_of(L" ", begIdx);

			if (endIdx != std::wstring::npos)
			{
				std::wstring strPrivateUserName = strCmd.substr(begIdx, endIdx - begIdx);
				EtInterface::GetNameLinkMgr().TranslateText(m_strPrivateUserName, strPrivateUserName.c_str());
				if ((int)m_strPrivateUserName.size() > CGlobalInfo::GetInstance().m_nClientCharNameLenMax)
					m_strPrivateUserName.resize(CGlobalInfo::GetInstance().m_nClientCharNameLenMax);

				SetChatMode(CHAT_PRIVATE);
				m_pIMEEditBox->ClearText();
				RequestFocus(m_pIMEEditBox);

				m_bPrivateModeChecker = false;
			}
		}
	}
}

bool CDnChatTabDlg::CheckCommandMode(LPCWSTR wszMsg)
{
	std::wstring strCmd(wszMsg);
	if (strCmd.size() < 2) return false;
	if ((strCmd.size() > 2) && (strCmd[2] != L' ')) return false;

	strCmd = strCmd.substr(0, 2);
	strCmd += L" ";
	COMMANDMODE_MAP_ITER iter = m_mapCommandMode.find(strCmd);

	if (iter != m_mapCommandMode.end())
	{
		if (iter->second&CHAT_PRIVATE_REPLY)
		{
			std::wstring strLastName;
			if (GetLastPrivateName(strLastName))
			{
				SetPrivateName(strLastName);
			}
			return true;
		}

		if (iter->second & CHAT_PARTY)
		{
			CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
			if (pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm)
				return false;
		}

		CDnChatDlg *pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
		if (pChatDlg)
		{
			CDnChatOptDlg &chatOptDlg = pChatDlg->GetChatOptDlg();
			int nOption = chatOptDlg.GetOption();

			if (!(nOption&iter->second))
			{
				switch (iter->second)
				{
				case CHAT_NORMAL:	SetCheckedTab(m_pNormalTab->GetTabID());	break;
#ifdef PRE_ADD_CHAT_RENEWAL
				case CHAT_PRIVATE:	SetCheckedTab(m_pWhisperTab->GetTabID());	break;
#else
				case CHAT_PRIVATE:	SetCheckedTab(m_pNormalTab->GetTabID());	break;
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
				case CHAT_PRIVATE_CHANNEL:	SetCheckedTab(m_pPrivateChannelTab->GetTabID()); break;
#endif // PRE_PRIVATECHAT_CHANNEL
				case CHAT_PARTY:	SetCheckedTab(m_pPartyTab->GetTabID());	break;
				case CHAT_GUILD:	SetCheckedTab(m_pGuildTab->GetTabID());	break;
				case CHAT_CHANNEL:	SetCheckedTab(m_pNormalTab->GetTabID());	break;
				case CHAT_WORLD:	SetCheckedTab(m_pNormalTab->GetTabID());	break;
				}
			}

			if (iter->second == CHAT_PRIVATE)
			{
				strCmd = wszMsg;
				std::wstring::size_type begIdx = strCmd.find_first_not_of(L" ", 2);
				if (begIdx != std::wstring::npos)
				{
					std::wstring strPrivateUserName = strCmd.substr(begIdx);
					EtInterface::GetNameLinkMgr().TranslateText(m_strPrivateUserName, strPrivateUserName.c_str());
					if ((int)m_strPrivateUserName.size() > CGlobalInfo::GetInstance().m_nClientCharNameLenMax)
					{
						m_strPrivateUserName.resize(CGlobalInfo::GetInstance().m_nClientCharNameLenMax);
					}
				}
			}

			SetChatMode(iter->second);
			m_pIMEEditBox->ClearText();
			RequestFocus(m_pIMEEditBox);
		}

		return true;
	}

	return false;
}

void CDnChatTabDlg::SetChatPrevMode()
{
	// 전과 달리 옵션에 따라 영향받지 않으므로,
	int nCurChatMode = GetChatMode();
	while (true)
	{
		nCurChatMode >>= 1;

		if (nCurChatMode == 0)
			nCurChatMode = CHAT_WORLD;

		if (SetChatMode(nCurChatMode))
		{
			RequestFocus(m_pIMEEditBox);
			break;
		}
	}
}

void CDnChatTabDlg::SetChatNextMode()
{
	// 전과 달리 옵션에 따라 영향받지 않으므로,
	int nCurChatMode = GetChatMode();
	while (true)
	{
		nCurChatMode <<= 1;

		if (nCurChatMode >= CHAT_MODE_MAX)
			nCurChatMode = CHAT_NORMAL;

		if (SetChatMode(nCurChatMode))
		{
			RequestFocus(m_pIMEEditBox);
			break;
		}
	}
}

void CDnChatTabDlg::SetChatModeText()
{
	if (!m_pStaticMode)
		return;

	wchar_t wszMsg[256] = { 0 };

	switch (m_nChatMode)
	{
	case CHAT_NORMAL:
		swprintf_s(wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 501));
		break;
	case CHAT_PARTY:
	{
		if (m_IsPVPGameChat)
			swprintf_s(wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 121018));
		else
			swprintf_s(wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 632));

	}

	break;
	case CHAT_GUILD:
#ifdef PRE_ADD_DWC
		if (GetDWCTask().IsDWCChar())
			swprintf_s(wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 121018));  // MID: 팀
		else
			swprintf_s(wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5007));	// MID: 길드		
#else
		swprintf_s(wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5007));
#endif
		break;
	case CHAT_PRIVATE:
	{
		swprintf_s(wszMsg, 256, L"[%s][%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 648), m_strPrivateUserName.c_str());

		if (!m_strPrivateUserName.empty())
		{
			AddPrivateName(m_strPrivateUserName);
		}
	}
	break;
	case CHAT_CHANNEL:
		swprintf_s(wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 605));
		break;
	case CHAT_WORLD:
		swprintf_s(wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 606));
		break;
#ifdef PRE_PRIVATECHAT_CHANNEL
	case CHAT_PRIVATE_CHANNEL:
		swprintf_s(wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1202));
		break;
#endif // PRE_PRIVATECHAT_CHANNEL
	}

	if (IsCustomizeChatMode() && GetChatMode() == m_nCustomizeModeType)
	{
		swprintf_s(wszMsg, 256, L"[%s]:", m_wszCustomizeModeHeader);
		ResizeIMECCtl(wszMsg);
	}
	else
	{
		ResizeIMECCtl(wszMsg);
	}
}

void CDnChatTabDlg::ResetChatMode()
{
	m_nChatMode = CHAT_NORMAL;

	SetChatModeText();

	SetCheckedTab(m_pNormalTab->GetTabID());
	ShowEx(true);
	ShowEx(false);
}

bool CDnChatTabDlg::SetChatMode(int nChatMode, bool bParty)
{
	if (nChatMode == CHAT_PARTY)
	{
		CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
		if (pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm)
			return false;

		// 파티에 가입되지 않았다면 (PVP에서는 팀챗으로 쓰여서 바꿔 줘야 함)
		if (GetPartyTask().GetPartyRole() == CDnPartyTask::SINGLE && !m_IsPVPGameChat)
			return false;
	}
	else if (nChatMode == CHAT_GUILD)
	{
		// 길드에 가입안되어있다면,
		if (CDnActor::s_hLocalActor) {
			CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

			if (!pPlayer->IsJoinGuild()
#ifdef PRE_ADD_DWC
				&&  GetDWCTask().IsDWCChar() == false
#endif // PRE_ADD_DWC
				)
			{
				return false;
			}
		}
	}
	else if (nChatMode == CHAT_PRIVATE)
	{
#ifdef PRE_ADD_CHAT_RENEWAL
		// 귓속말 대상이 없으면
		if (GetPrivateName() == L"")
		{
			std::wstring strLastName;
			if (GetLastPrivateName(strLastName))
				m_strPrivateUserName = strLastName;
			else
				return false;
		}
#else
		// 귓속말 대상이 없으면
		if (GetPrivateName() == L"")
			return false;
#endif // PRE_ADD_CHAT_RENEWAL
	}
	else if (nChatMode == CHAT_CHANNEL)
	{
		// 클베에서 우선 기능 비활성화.
		return false;
	}
	else if (nChatMode == CHAT_WORLD)
	{
		return false;
	}
#ifdef PRE_PRIVATECHAT_CHANNEL
	else if (nChatMode == CHAT_PRIVATE_CHANNEL)
	{
		if (GetChannelChatTask().IsActive() && !GetChannelChatTask().IsJoinChannel())
		{
			return false;
		}
	}
#endif // PRE_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_CHAT_RENEWAL	
	// 자동전환 and 파티상태.
	if (CGameOption::IsActive() && CGameOption::GetInstance().cPartyChatAcceptable == 1 && CDnPartyTask::IsActive() &&
		(CDnPartyTask::PartyRoleEnum::LEADER == GetPartyTask().GetPartyRole() ||
			CDnPartyTask::PartyRoleEnum::MEMBER == GetPartyTask().GetPartyRole())
		)
	{
		// 파티생성 or 파티가입 - 파티가된경우에 호출되었을때 탭을 일반으로 변경, 채팅모드는 파티상태.
		if (bParty)
		{
			nChatMode = CHAT_PARTY;
			SetCheckedTab(m_pNormalTab->GetTabID());
			CDnChatDlg * pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
			if (pChatDlg && pChatDlg->IsShowScrollBar())
				pChatDlg->ShowScrollBar(false);
		}

		// 탭클릭으로의 전환 - 일반탭인경우 채팅모드를 파티상태.
		else if (m_bChangeChatMode && nChatMode == CHAT_NORMAL)
		{
			m_bChangeChatMode = false;
			nChatMode = CHAT_PARTY;
		}
	}
#endif

	m_nChatMode = nChatMode;
	SetChatModeText();
	return true;
}

bool CDnChatTabDlg::AddEditBoxString()
{
	std::wstring strChat = m_pIMEEditBox->GetText();
	strChat.reserve(CHATLENMAX);

	// 아무것도 입력이 안되었을때 엔터쳐야 채팅창 비활성화가 되어야한다면,
	// 아래 두줄 주석 풀고, 맨아래 return true를 return false로 바꾸면 됩니다.
	//if( strChat.empty() )
	//	return true;

	std::wstring chatTemp;
	chatTemp = boost::algorithm::trim_copy(strChat);
	if (chatTemp.empty())
		return true;

	if (CheckCommandMode(strChat.c_str()))
		return false;

	// 명령어 처리하기전에 기억해놔야 히스토리 이용해서 명령어 재입력을 할 수 있다.
	AddChatHistotry(strChat.c_str());

	// 적절한 명령어로 처리가 되면 입력창이 포커스를 잃도록 true를 리턴.
	if (CommandMessage(strChat.c_str()))
		return true;

	// 여기서 채팅 제재 검사
	if (GetRestraintTask().CheckRestraint(_RESTRAINTTYPE_CHAT, true))
		return true;

	// 제스처에도 명령어 있다.
	// 제스처는 다른 명령어들과 달리 포커스 잃게 하지 않는다. 그냥 채팅중이므로.
	GetGestureTask().UseGestureByChat(strChat.c_str());

	// 운영자 명령어때문에 /로 시작하는건 Send하지 않는다.
	// set_user_level, makeitem 같은 경우엔 DebugCmdProc에서 처리하지 않기때문에, 서버로 그냥 보내는 수밖에 없다.
	// 그래서 Final_build 에서만 체크하겠다.
#ifdef _FINAL_BUILD
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if (pLocalPlayer)
	{
		if (strChat[0] == L'/' && CDnActor::s_hLocalActor && pLocalPlayer->IsDeveloperAccountLevel() == false)
		{
			_AddChatMessage(m_nChatMode, CDnActor::s_hLocalActor->GetName(), strChat.c_str());

			// 적절한 명령어였다면 위에서 처리되었을거고, 아닌 경우니 return false로 채팅포커스를 잃지 않도록 하겠다.
			return false;
		}
	}
#endif

#ifdef PRE_ADD_ANTI_CHAT_SPAM
	// #47642 일반(전체) 모드 채팅일 경우에만 제제.
	if (CHAT_NORMAL == m_nChatMode)
	{
		// 미국 스팸 방지 제재.	
		bool bSpammer = GetRestraintTask().CheckUSAChatSpam(strChat);
		if (bSpammer)
		{
			_AddChatMessage(m_nChatMode, CDnActor::s_hLocalActor->GetName(), strChat.c_str());
			if (!m_bSpammer)
			{
				//일단 서버쪽으로 보냅니다.
				SendChatSpammer(true);
				m_bSpammer = true;
			}
			//m_pIMEEditBox->ClearText();
			//return false;
		}
		else if (m_bSpammer)
		{
			SendChatSpammer(false);
			m_bSpammer = false;
		}
	}
#endif // #ifdef PRE_ADD_ANTI_CHAT_SPAM

	SendChat(m_nChatMode, strChat, m_strPrivateUserName);

	m_pIMEEditBox->ClearText();

	return true;
}

#ifdef PRE_CHAT_FIX_DICE_PAPERING
bool CDnChatTabDlg::HandleChatPapering(const std::wstring& chatMsg)
{
	// 파티나 길드 채팅모드에선 도배검사 하지 않는다.
	if (m_nChatMode != CHAT_PARTY && m_nChatMode != CHAT_GUILD)
	{
		// 메세지가 /로 시작한다면, 도배방지 기능을 적용하지 않는다.
		bool bCheckPapering = true;
		if (chatMsg[0] == L'/')
		{
			std::wstring::size_type firstBlank = chatMsg.find_first_of(L" ");
			std::wstring subStr = chatMsg.substr(1, firstBlank - 1);
			bCheckPapering = (subStr.compare(L"dice") == 0
#if defined(_KR) || defined(_RDEBUG)
				|| subStr.compare(L"주사위") == 0
#endif
				);
		}
		if (bCheckPapering && CheckPapering())
		{
			wchar_t wszMsg[256] = { 0 };
			int nTime = (int)m_fPaperingRemainTime;
			swprintf_s(wszMsg, 256, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 672), nTime);

			// 옵션체크도 안되고, 색상도 완전히 별도라 이렇게 처리한다.
			CDnChatDlg *pDlg = NULL;
			if (m_pNormalTab->GetTabID() == GetCurrentTabID())			pDlg = m_pChatNormalDlg;
#ifdef PRE_ADD_CHAT_RENEWAL
			else if (m_pWhisperTab->GetTabID() == GetCurrentTabID())   pDlg = m_pChatWhisperDlg;
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
			else if (m_pPrivateChannelTab->GetTabID() == GetCurrentTabID())	pDlg = m_pPrivateChannelDlg;
#endif // PRE_PRIVATECHAT_CHANNEL
			else if (m_pPartyTab->GetTabID() == GetCurrentTabID())		pDlg = m_pChatPartyDlg;
			else if (m_pGuildTab->GetTabID() == GetCurrentTabID())		pDlg = m_pChatGuildDlg;
			else if (m_pSystemTab->GetTabID() == GetCurrentTabID())	pDlg = m_pChatSystemDlg;
			if (pDlg)
			{
				pDlg->AddChat(NULL, L"", 0, UITEXT_NONE, false, false, -1, 0, true);
				pDlg->AddChat(NULL, wszMsg, 0, UITEXT_NONE, true, true, textcolor::RED);
			}
			m_pIMEEditBox->ClearText();
			return true;
		}
	}

	return false;
}
#endif // PRE_CHAT_FIX_DICE_PAPERING

void CDnChatTabDlg::SendChat(int chatMode, std::wstring& chatMsg, const std::wstring& userName)
{
	// 5회 반복 체크는 이제 사용하지 않는다고 한다.
	// 나중에 정말 필요없어질때 완전히 삭제하겠다.
	//if( CheckRepeat() )
	//{
	//	wchar_t wszMsg[256]={0};
	//	swprintf_s( wszMsg, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 670) );
	//	AddChatDlg( wszMsg, CHAT_SYSTEM );
	//	m_pIMEEditBox->ClearText();
	//	return;
	//}

#ifdef PRE_CHAT_FIX_DICE_PAPERING
	if (HandleChatPapering(chatMsg))
		return;
#else
	// 파티나 길드 채팅모드에선 도배검사 하지 않는다.
#ifdef PRE_PRIVATECHAT_CHANNEL
	if (m_nChatMode != CHAT_PARTY && m_nChatMode != CHAT_GUILD && m_nChatMode != CHAT_PRIVATE_CHANNEL)
#else // PRE_PRIVATECHAT_CHANNEL
	if (m_nChatMode != CHAT_PARTY && m_nChatMode != CHAT_GUILD)
#endif // PRE_PRIVATECHAT_CHANNEL
	{
		// 메세지가 /로 시작한다면, 도배방지 기능을 적용하지 않는다.
		bool bCheckPapering = true;
		if (chatMsg[0] == L'/') bCheckPapering = false;
		if (bCheckPapering && CheckPapering())
		{
			wchar_t wszMsg[256] = { 0 };
			int nTime = (int)m_fPaperingRemainTime;
			swprintf_s(wszMsg, 256, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 672), nTime);

			// 옵션체크도 안되고, 색상도 완전히 별도라 이렇게 처리한다.
			CDnChatDlg *pDlg = NULL;
			if (m_pNormalTab->GetTabID() == GetCurrentTabID())			pDlg = m_pChatNormalDlg;
#ifdef PRE_ADD_CHAT_RENEWAL
			else if (m_pWhisperTab->GetTabID() == GetCurrentTabID())   pDlg = m_pChatWhisperDlg;
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
			else if (m_pPrivateChannelTab->GetTabID() == GetCurrentTabID())	pDlg = m_pPrivateChannelDlg;
#endif // PRE_PRIVATECHAT_CHANNEL
			else if (m_pPartyTab->GetTabID() == GetCurrentTabID())		pDlg = m_pChatPartyDlg;
			else if (m_pGuildTab->GetTabID() == GetCurrentTabID())		pDlg = m_pChatGuildDlg;
			else if (m_pSystemTab->GetTabID() == GetCurrentTabID())	pDlg = m_pChatSystemDlg;
			if (pDlg)
			{
				pDlg->AddChat(NULL, L"", 0, UITEXT_NONE, false, false, -1, 0, true);
				pDlg->AddChat(NULL, wszMsg, 0, UITEXT_NONE, true, true, textcolor::RED);
			}
			m_pIMEEditBox->ClearText();
			return;
		}
	}
#endif	// PRE_CHAT_FIX_DICE_PAPERING

	switch (chatMode)
	{
	case CHAT_NORMAL:
	{
		bool bAdd = true;
		if (CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->IsPlayerActor())
		{
			CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if (pPlayer && pPlayer->IsObserver())
			{
				bAdd = false; // 관전자 모드는 일반챗을 추가하지 않습니다.
			}
		}
		if (bAdd)
		{
			SendChatMsg(CHATTYPE_NORMAL, chatMsg.c_str());

#if defined( PRE_ADD_CHAT_MISSION )
			GetMissionTask().RequestChatMission(chatMsg);
#endif	// #if defined( PRE_ADD_CHAT_MISSION )
		}
	}
	break;
	case CHAT_PRIVATE:
	{
		if (userName.empty())
			break;

		_AddChatMessage(CHAT_PRIVATE, userName.c_str(), chatMsg.c_str());
		SendChatPrivateMsg(userName.c_str(), chatMsg.c_str());
	}
	break;
	case CHAT_PARTY:
	{
		if (m_IsPVPGameChat)
			SendChatMsg(CHATTYPE_TEAMCHAT, chatMsg.c_str());
		else
			SendChatMsg(CHATTYPE_PARTY, chatMsg.c_str());
	}
	break;
	case CHAT_GUILD:
	{
#ifdef PRE_ADD_DWC
		eChatType ChatType;
		(GetDWCTask().IsDWCChar()) ? ChatType = CHATTYPE_DWC_TEAM : ChatType = CHATTYPE_GUILD;
		SendChatMsg(ChatType, chatMsg.c_str());
#else
		SendChatMsg(CHATTYPE_GUILD, chatMsg.c_str());
#endif
	}
	break;
	case CHAT_CHANNEL:
	{
		SendChatMsg(CHATTYPE_CHANNEL, chatMsg.c_str());
	}
	break;
	case CHAT_WORLD:
	{
		SendChatMsg(CHATTYPE_WORLD, chatMsg.c_str());
	}
	break;
#ifdef PRE_PRIVATECHAT_CHANNEL
	case CHAT_PRIVATE_CHANNEL:
	{
		SendChatMsg(CHATTYPE_PRIVATECHANNEL, chatMsg.c_str());
	}
	break;
#endif // PRE_PRIVATECHAT_CHANNEL
	}
}

bool CDnChatTabDlg::CommandMessage(LPCWSTR wszMsg)
{
	std::wstring strText(wszMsg);
	std::wstring strCommand;
	std::wstring strArg1;
	if (strText[0] == L'/')
	{
		bool bProcessed = false;

		// 공백단위로 인자가 몇개인지 미리 세어둔다.
		// 현재 모든 명령어가 명령어 자체를 포함해 2개까지만을 처리하기때문에,
		// 1개, 2개까지만 유효하도록 처리한다.
		int nNumArg = 0;
		std::wstring::size_type endIdx, endIdx2;
		endIdx = strText.find_first_of(L" ");
		if (endIdx != std::wstring::npos)
		{
			strCommand = strText.substr(0, endIdx);
			endIdx2 = strText.find_first_of(L" ", endIdx + 1);
			if (endIdx2 != std::wstring::npos
				&& strCommand.compare(L"/y") != 0
				&& strCommand.compare(L"/Y") != 0
				&& strCommand.compare(L"/ㅛ") != 0
				)
				nNumArg = 2;
			else
			{
				nNumArg = 1;
				strArg1 = strText.substr(endIdx + 1, endIdx2);
			}
		}
		else
		{
			strCommand = strText;
		}

		if (nNumArg == 0 || nNumArg == 1)
		{
			COMMANDMESSAGE_MAP_ITER iter;

			iter = m_mapCommandMessage.find(strCommand);
			if (iter != m_mapCommandMessage.end())
			{
				// 블라인드 상태라면, 헬프 명령어 외 처리하지 않는다.
				if (GetInterface().IsOpenBlind()
					|| GetInterface().IsShowCostumeMixDlg()
					)
				{
					if (iter->second != CMD_MSG_HELP)
					{
						AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100301), CHAT_SYSTEM);
						m_pIMEEditBox->ClearText();
						return true;
					}
				}

				// 마을 아닌 곳에서 마을에서만 사용하려는 기능 사용시
				// 지금은 사용할 수 없는 기능입니다. 메세지 출력.
				if (CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage)
				{
					if (iter->second == CMD_MSG_MAKEPARTY)
					{
						AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100301), CHAT_SYSTEM);
						m_pIMEEditBox->ClearText();
						return true;
					}
				}

				if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon)
				{
					if (iter->second == CMD_MSG_INVITE)
					{
						AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100301), CHAT_SYSTEM);
						m_pIMEEditBox->ClearText();
						return true;
					}
				}

				// 콜로세움, pvp로비, pvp게임중이라면 아래 명령어를 수행하지 않는다.
				CDnVillageTask *pVillage = (CDnVillageTask *)CTaskManager::GetInstance().GetTask("VillageTask");
				if ((pVillage && pVillage->GetVillageType() == CDnVillageTask::PvPVillage) ||
					(pVillage && pVillage->GetVillageType() == CDnVillageTask::FarmVillage) ||
					m_bPVPLobbyChat ||
					m_IsPVPGameChat)
				{
					if (iter->second == CMD_MSG_INVITE || iter->second == CMD_MSG_MAKEPARTY || iter->second == CMD_MSG_LEAVEPARTY || iter->second == CMD_MSG_BAN ||
						iter->second == CMD_MSG_TRADE)
					{
						AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100301), CHAT_SYSTEM);
						m_pIMEEditBox->ClearText();
						return true;
					}
				}

#ifdef PRE_PRIVATECHAT_CHANNEL
				if (iter->second != CMD_MSG_CHANNEL_CREATE && iter->second != CMD_MSG_CHANNEL_JOIN && iter->second != CMD_MSG_CHANNEL_PASSWORD && iter->second != CMD_MSG_CHANNEL_INVITE &&
					iter->second != CMD_MSG_CHANNEL_LEAVE && iter->second != CMD_MSG_CHANNEL_BAN && iter->second != CMD_MSG_CHANNEL_MASTER && iter->second != CMD_MSG_CHANNEL_HELP)
				{
					// 로컬액터를 구할 수 없는 상태라면 일반적인 상황이 아닌 것이다.
					if (!CDnActor::s_hLocalActor)
						return true;

					// 유령 상태에서는 명령어를 사용할 수 없게 한다.
					if (CDnActor::s_hLocalActor->IsDie())
					{
						wchar_t wszMsg[256] = { 0 };
						swprintf_s(wszMsg, 256, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 671));
						AddChatDlg(wszMsg, CHAT_SYSTEM);
						return true;
					}
				}
#else // PRE_PRIVATECHAT_CHANNEL
				// 로컬액터를 구할 수 없는 상태라면 일반적인 상황이 아닌 것이다.
				if (!CDnActor::s_hLocalActor)
					return true;

				// 유령 상태에서는 명령어를 사용할 수 없게 한다.
				if (CDnActor::s_hLocalActor->IsDie())
				{
					wchar_t wszMsg[256] = { 0 };
					swprintf_s(wszMsg, 256, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 671));
					AddChatDlg(wszMsg, CHAT_SYSTEM);
					return true;
				}
#endif // PRE_PRIVATECHAT_CHANNEL

				switch (iter->second)
				{
				case CMD_MSG_HELP:
				{
					// 도움말 출력
					if (nNumArg == 0)
					{
						const UINT nStartIndex = 112001;
						const UINT nEndIndex = 112999;
						WCHAR wszMessage[128];
						for (UINT i = nStartIndex; i < nEndIndex; ++i)
						{
							swprintf_s(wszMessage, 128, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, i));
							if (wszMessage[0] == '/')
							{
								AddChatDlg(wszMessage, CHAT_SYSTEM);
							}
						}
						bProcessed = true;
					}
				}
				break;
				case CMD_MSG_INVITE:
				{
					// 자신초대 - 파티생성, 리더된 후 이미 대상이 파티에 속해있어...
					// 비접속 유저 - 초대할 유저를 찾을 수 없습니다.
					// 다른 채널 유저 - 초대에 실패하였습니다.
					if (nNumArg == 1)
					{
						GetPartyTask().ReqInviteParty(strArg1.c_str());
						bProcessed = true;
					}
				}
				break;
				case CMD_MSG_MAKEPARTY:
				{
					// 기본설정으로 파티를 생성한다.
					if (nNumArg > 0 && CommonUtil::IsValidPartyNameLen((int)strArg1.length()) == false)
					{
						AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3534), CHAT_SYSTEM);
						m_pIMEEditBox->ClearText();
						bProcessed = true;
						break;
					}

#ifdef PRE_PARTY_DB
					SPartyCreateParam param;
					if (nNumArg > 0)
						param.wszPartyName = strArg1;
					else
						param.wszPartyName = FormatW(L"%s%s", CDnActor::s_hLocalActor->GetName(), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3535));
					GetPartyTask().ReqCreateParty(param);
#else
					std::wstring partyName;
					if (nNumArg > 0)
						partyName = strArg1;
					else
						partyName = FormatW(L"%s%s", CDnActor::s_hLocalActor->GetName(), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3535));

					GetPartyTask().ReqCreateParty(0, 4, 1, 100, partyName.c_str(), NULL, 0, 0, 0);
#endif
					bProcessed = true;
				}
				break;
				case CMD_MSG_LEAVEPARTY:
				{
					// 파티없이 하면 아무일 일어나지 않는다.
					if (nNumArg == 0)
					{
						GetPartyTask().ReqOutParty();
						bProcessed = true;
					}
				}
				break;
				case CMD_MSG_BAN:
				{
					// 추방의 경우 마을인지 아닌지만 판단하는게 아니라, 던전안인지도 판단해야한다.
					// 지금은 사용할 수 없는 기능입니다. 메세지 출력.
					if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon)
					{
						AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100301), CHAT_SYSTEM);
						return true;
					}

					if (nNumArg == 1)
					{
						GetPartyTask().ReqPartyMemberKick(strArg1.c_str(), 0);
						bProcessed = true;
					}
				}
				break;
				case CMD_MSG_TRADE:
				{
					if (nNumArg == 1)
					{
						wchar_t wszMsg[256] = { 0 };

						// 자신이 누군가에게 거래를 요청중이라면, 혹은 누군가 자신에게 거래를 요청중이라면,
						if (GetTradeTask().GetTradePrivateMarket().IsRequesting() || GetTradeTask().GetTradePrivateMarket().IsAccepting())
						{
							swprintf_s(wszMsg, 256, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1724));
							AddChatDlg(wszMsg, CHAT_SYSTEM);
							break;
						}

						// 대상을 근처에서 찾을 수 없는 경우,
						DnActorHandle hActor = CDnActor::FindActorFromName((TCHAR *)strArg1.c_str());
						if (!hActor)
						{
							swprintf_s(wszMsg, 256, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 673));
							AddChatDlg(wszMsg, CHAT_SYSTEM);
							break;
						}
						DWORD nSessionID = hActor->GetUniqueID();

						// 자신에게 거래를 시도했다면
						if (CDnActor::s_hLocalActor->GetUniqueID() == nSessionID)
							break;

						GetTradeTask().GetTradePrivateMarket().RequestPrivateMarket(nSessionID);
						bProcessed = true;
					}
				}
				break;
				case CMD_MSG_ESCAPE:
				{
#ifdef PRE_FIX_ESCAPE
					GetInterface().ShowEscapeDlg(true);
#else
					if (!CDnActor::s_hLocalActor) break;
					CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
					if (!pPlayer) break;
					pPlayer->CmdEscape();
#endif

					bProcessed = true;
					return true;
				}
				break;
				case CMD_MSG_RAIDNOTICE:
				{
					if (!CDnActor::s_hLocalActor) break;
					CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
					if (pPlayer && IsGuildWarMaster(pPlayer->GetName()))
					{
						std::wstring::size_type startidx, endidx;
						startidx = strText.find_first_of(L" ");
						endidx = strText.length();

						std::wstring wstrTemp;
						wstrTemp = strText.substr(startidx + 1, endidx);

						SendChatMsg(CHATTYPE_RAIDNOTICE, wstrTemp.c_str());
						return true;
					}
					bool bCanNotUse = false;
					if (CDnPartyTask::IsActive() == false)
						bCanNotUse = true;

					if (CDnPartyTask::GetInstance().IsPartyType(_RAID_PARTY_8) == false ||
						CDnPartyTask::GetInstance().IsLocalActorMaster() == false)
						bCanNotUse = true;

					if (bCanNotUse)
					{
						AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3840), CHAT_SYSTEM);	// UISTRING : 외침 기능은 공대장만 사용 가능합니다.
						return true;
					}

					SendChatMsg(CHATTYPE_RAIDNOTICE, strText.c_str());
					return true;
				}
				break;
#ifdef PRE_PRIVATECHAT_CHANNEL
				case CMD_MSG_CHANNEL_CREATE:
				{
					m_pIMEEditBox->ClearText();
					if (nNumArg == 1)
					{
						if (GetChannelChatTask().IsActive() && !GetChannelChatTask().IsJoinChannel())
						{
							if (strArg1.size() < CHANNEL_NAME_MIN || strArg1.size() > CHANNEL_NAME_MAX)
							{
								GetInterface().AddChatMessage(CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7812), false);	// UISTRING : 2~10자 이내의 채널 제목을 입력해주세요.
								return true;
							}

							DWORD dwCheckType = ALLOW_STRING_DEFAULT;
							if (g_CountryUnicodeSet.Check(strArg1.c_str(), dwCheckType) == false)
							{
								GetInterface().AddChatMessage(CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7811), false);	// UISTRING : 잘못된 형식의 채널이름 입니다.
								return true;
							}
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
							GetChannelChatTask().SendChannelAdd(strArg1.c_str(), -1);
#else // PRE_ADD_PRIVATECHAT_CHANNEL
							GetChannelChatTask().SendChannelAdd(strArg1.c_str());
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
						}
						else
						{
							AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7813), CHAT_SYSTEM);	// UISTRING : 1개 이상의 채널에 참여할 수 없습니다.
						}
					}
					return true;
				}
				break;
				case CMD_MSG_CHANNEL_JOIN:
				{
					m_pIMEEditBox->ClearText();
					if (nNumArg == 1)
					{
						if (GetChannelChatTask().IsActive() && !GetChannelChatTask().IsJoinChannel())
						{
							if (strArg1.size() < CHANNEL_NAME_MIN || strArg1.size() > CHANNEL_NAME_MAX)
							{
								GetInterface().AddChatMessage(CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7812), false);	// UISTRING : 2~10자 이내의 채널 제목을 입력해주세요.
								return true;
							}

							DWORD dwCheckType = ALLOW_STRING_DEFAULT;
							if (g_CountryUnicodeSet.Check(strArg1.c_str(), dwCheckType) == false)
							{
								GetInterface().AddChatMessage(CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7811), false);	// UISTRING : 잘못된 형식의 채널이름 입니다.
								return true;
							}

							GetChannelChatTask().SendChannelJoin(strArg1.c_str(), -1);
						}
						else
						{
							AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7813), CHAT_SYSTEM);	// UISTRING : 1개 이상의 채널에 참여할 수 없습니다.
						}
					}
					return true;
				}
				break;
				case CMD_MSG_CHANNEL_PASSWORD:
				{
					m_pIMEEditBox->ClearText();
					if (GetChannelChatTask().IsActive() && GetChannelChatTask().IsJoinChannel())
					{
						if (GetChannelChatTask().IsChannelMaster())
						{
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
							CDnPrivateChannelPasswordChangeDlg* pDnPrivateChannelPasswordChangeDlg = GetInterface().GetPrivateChannelPasswordChangeDlg();
							if (pDnPrivateChannelPasswordChangeDlg)
							{
								pDnPrivateChannelPasswordChangeDlg->Show(true);
							}
#else // PRE_ADD_PRIVATECHAT_CHANNEL
							CDnPrivateChannelPasswordDlg* pDnPrivateChannelPasswordDlg = GetInterface().GetPrivateChannelPasswordDlg();
							if (pDnPrivateChannelPasswordDlg)
							{
								pDnPrivateChannelPasswordDlg->SetChannelPasswordType(CDnPrivateChannelPasswordDlg::TYPE_PASSWORD_CHANGE);
								pDnPrivateChannelPasswordDlg->Show(true);
							}
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
						}
						else
						{
							AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7817), CHAT_SYSTEM);	// UISTRING : 권한이 없습니다.
						}
					}
					else
					{
						AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7829), CHAT_SYSTEM);	// UISTRING : 참여하고 있는 채널이 없습니다.
					}
					return true;
				}
				break;
				case CMD_MSG_CHANNEL_INVITE:
				{
					m_pIMEEditBox->ClearText();
					if (nNumArg == 1)
					{
						if (GetChannelChatTask().IsActive() && GetChannelChatTask().IsJoinChannel())
						{
							if (GetChannelChatTask().IsChannelMaster())
							{
								GetChannelChatTask().SendChannelInvite(strArg1.c_str());
							}
							else
							{
								AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7817), CHAT_SYSTEM);	// UISTRING : 권한이 없습니다.
							}
						}
						else
						{
							AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7829), CHAT_SYSTEM);	// UISTRING : 참여하고 있는 채널이 없습니다.
						}
					}
					return true;
				}
				break;
				case CMD_MSG_CHANNEL_LEAVE:
				{
					m_pIMEEditBox->ClearText();
					if (GetChannelChatTask().IsActive() && GetChannelChatTask().IsJoinChannel())
					{
						GetChannelChatTask().SendChannelOut();
					}
					else
					{
						AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7829), CHAT_SYSTEM);	// UISTRING : 참여하고 있는 채널이 없습니다.
					}
					return true;
				}
				break;
				case CMD_MSG_CHANNEL_BAN:
				{
					m_pIMEEditBox->ClearText();
					if (nNumArg == 1)
					{
						if (GetChannelChatTask().IsActive() && GetChannelChatTask().IsJoinChannel())
						{
							if (GetChannelChatTask().IsChannelMaster())
							{
								GetChannelChatTask().SendChannelKick(strArg1.c_str());
							}
							else
							{
								AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7817), CHAT_SYSTEM);	// UISTRING : 권한이 없습니다.
							}
						}
						else
						{
							AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7829), CHAT_SYSTEM);	// UISTRING : 참여하고 있는 채널이 없습니다.
						}
					}
					return true;
				}
				break;
				case CMD_MSG_CHANNEL_MASTER:
				{
					m_pIMEEditBox->ClearText();
					if (nNumArg == 1)
					{
						if (GetChannelChatTask().IsActive() && GetChannelChatTask().IsJoinChannel())
						{
							if (GetChannelChatTask().IsChannelMaster())
							{
								PrivateChatChannel::TMemberInfo* pChannelMemberInfo = GetChannelChatTask().GetChannelMemberInfoByName(strArg1.c_str());
								if (pChannelMemberInfo)
								{
									GetChannelChatTask().SendChannelMod(PrivateChatChannel::Common::ChangeMaster, 0, pChannelMemberInfo->biCharacterDBID);
								}
								else
								{
									AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7825), CHAT_SYSTEM);	// UISTRING : 방장으로 위임할 유저가 존재하지 않습니다.
								}
							}
							else
							{
								AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7817), CHAT_SYSTEM);	// UISTRING : 권한이 없습니다.
							}
						}
						else
						{
							AddChatDlg(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7829), CHAT_SYSTEM);	// UISTRING : 참여하고 있는 채널이 없습니다.
						}
					}
					return true;
				}
				break;
				case CMD_MSG_CHANNEL_HELP:
				{
					m_pIMEEditBox->ClearText();
					std::vector<std::wstring> vecHelps;
					TokenizeW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7836), vecHelps, L"\\n");

					for (int i = 0; i<static_cast<int>(vecHelps.size()); i++)
					{
						AddChatDlg(vecHelps[i].c_str(), CHAT_NORMAL);
					}
					return true;
				}
#endif // PRE_PRIVATECHAT_CHANNEL
				}
			}
		}

		if (DebugCmdProc(strText.c_str())) bProcessed = true;

		m_pIMEEditBox->ClearText();
		RequestFocus(m_pIMEEditBox);

		// 적절한 명령어로 처리가 되면 입력창이 포커스를 잃도록 true를 리턴.
		if (bProcessed)
			return true;
	}

	return false;
}

void CDnChatTabDlg::_AddChatMessage(int nChatType, LPCWSTR wszFromCharName, LPCWSTR wszMessage)
{
	std::wstring wszTempMsg = wszMessage;
	if (CDnActor::s_hLocalActor && ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->IsDeveloperAccountLevel() == false)
		DN_INTERFACE::UTIL::CheckChat(wszTempMsg, '*');
	wszMessage = wszTempMsg.c_str();

	wchar_t wszMsg[1024] = { 0 };

	int nVecSize = (int)m_vecChatDlg.size();
	for (int i = 0; i<nVecSize; i++)
	{
		if (m_vecChatDlg[i])
		{
			CDnChatOptDlg &chatOptDlg = m_vecChatDlg[i]->GetChatOptDlg();

			// 아래 타입 중 귓속말 제외하고, 실행되는건 /로 시작되는 명령어를 로컬 채팅창에 등록할때밖에 없다.
			// 그래서 wszFromCharName가 NULL인지 검사하는 코드가 없다.
			switch (nChatType)
			{
			case CHAT_NORMAL:
			{
				if (chatOptDlg.IsNomal())
				{
					swprintf_s(wszMsg, 1024, L"[%s] : %s", wszFromCharName, wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_NORMAL);
				}
			}
			break;
			case CHAT_PARTY:
			{
				if (chatOptDlg.IsParty())
				{
					swprintf_s(wszMsg, 1024, L"[%s] : %s", wszFromCharName, wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_PARTY);
				}
			}
			break;
			case CHAT_GUILD:
			{
				if (chatOptDlg.IsGuild())
				{
					swprintf_s(wszMsg, 1024, L"[%s] : %s", wszFromCharName, wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_GUILD);
				}
			}
			break;
			case CHAT_PRIVATE:
			{
				CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::INVENTORY_DIALOG);
				if (pInvenDlg)
				{
					// wszFromCharName과 거래중인 상대방 이름이 같을 경우에만 내 말풍선에 등록.
					if (pInvenDlg->IsYourName(wszFromCharName))
						pInvenDlg->AddPrivateMarketChatMe(wszMessage);
				}

				if (chatOptDlg.IsPrivate())
				{
					swprintf_s(wszMsg, 1024, L"[%s]<<< : %s", wszFromCharName, wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_PRIVATE);
				}
			}
			break;
			case CHAT_CHANNEL:
			{
				if (chatOptDlg.IsChannel())
				{
					swprintf_s(wszMsg, 1024, L"[%s] : %s", wszFromCharName, wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_CHANNEL);
				}
			}
			break;
			case CHAT_WORLD:
			{
				if (chatOptDlg.IsWorld())
				{
					swprintf_s(wszMsg, 1024, L"[%s] : %s", wszFromCharName, wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_WORLD);
				}
			}
			break;
#ifdef PRE_PRIVATECHAT_CHANNEL
			case CHAT_PRIVATE_CHANNEL:
			{
				if (chatOptDlg.IsPrivateChannel())
				{
					swprintf_s(wszMsg, 1024, L"[%s] : %s", wszFromCharName, wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_PRIVATE_CHANNEL);
				}
			}
			break;
#endif // PRE_PRIVATECHAT_CHANNEL
			}
		}
	}

}

#ifdef PRE_ADD_DOORS
void CDnChatTabDlg::AddChatMessage(int nChatType, LPCWSTR wszFromCharName, LPCWSTR wszMessage, bool isAppend, bool hasColor, DWORD colorValue, DWORD bgColorValue, bool bIsDoorsMobile)
#else
void CDnChatTabDlg::AddChatMessage(int nChatType, LPCWSTR wszFromCharName, LPCWSTR wszMessage, bool isAppend, bool hasColor, DWORD colorValue, DWORD bgColorValue)
#endif
{
	if (!m_bPVPLobbyChat) {
		if (nChatType == CHAT_NORMAL || nChatType == CHAT_PARTY) {
			if (wcslen(wszFromCharName) > 0) {
				DnActorHandle hActor = CDnActor::FindActorFromName(const_cast<TCHAR*>(wszFromCharName));
				bool bBlackList = (CDnIsolateTask::IsActive() && GetIsolateTask().IsBlackList(wszFromCharName));

				if (hActor && false == bBlackList) {

#ifdef PRE_ADD_CASH_AMULET
					if (hActor->GetChatBalloon().IsEnableCustomChatBallon())
					{
						hActor->GetChatBalloon().StopChatBalloonCustom();
						hActor->GetChatBalloon().ClearCustomChatBalloonText();
					}

					if (CGameOption::GetInstance().m_bShowCashChatBalloon && hActor->IsPlayerActor())
					{
						CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(hActor.GetPointer());
						DnPartsHandle hCashAmulet = pPlayer->GetCashParts(CDnParts::CashNecklace);

						if (hCashAmulet)
						{
							int nItemID = hCashAmulet->GetClassID();
							DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TCASHCHATBALLOONTABLE);

							if (pSox && pSox->IsExistItem(nItemID))
							{
								const DWORD NORMAL_CHAT_COLOR = D3DCOLOR_ARGB(255, 90, 82, 76);
								const DWORD PARTY_CHAT_COLOR = D3DCOLOR_ARGB(255, 0, 0, 128);
								// chatroomcolor::NORMAL

								if (nChatType == CHAT_PARTY)
									hActor->GetChatBalloon().SetChatBalloonCustomText(pSox->GetFieldFromLablePtr(nItemID, "_UIResourceName2")->GetString(), wszMessage, PARTY_CHAT_COLOR, GetTickCount(), 1);
								else
									hActor->GetChatBalloon().SetChatBalloonCustomText(pSox->GetFieldFromLablePtr(nItemID, "_UIResourceName1")->GetString(), wszMessage, NORMAL_CHAT_COLOR, GetTickCount(), 1);
							}
							else
							{
								hActor->SetChatBalloonText(wszMessage, GetTickCount(), nChatType);
							}
						}
						else
							hActor->SetChatBalloonText(wszMessage, GetTickCount(), nChatType);

					}
					else
						hActor->SetChatBalloonText(wszMessage, GetTickCount(), nChatType);

#else
					hActor->SetChatBalloonText(wszMessage, GetTickCount(), nChatType);
#endif
				}
			}
		}
	}

	// 귓말 왔을때 비활성이라면 소리출력.
	if (nChatType == CHAT_PRIVATE)
	{
		if (!IsShow())
		{
			if (m_nPrivateAlarmSoundIndex != -1)
				CEtSoundEngine::GetInstance().PlaySound("2D", m_nPrivateAlarmSoundIndex);
		}
	}

	wchar_t wszMsg[1024] = { 0 };

	std::wstring fromCharNameString;
	if (wcslen(wszFromCharName) != 0)
	{
		CDnNameLinkMng* pMgr = GetInterface().GetNameLinkMng();
		if (pMgr)
			pMgr->MakeNameLinkString_UserName(fromCharNameString, wszFromCharName);
	}

	int nGuildMid = 5007; // Mid: 길드
#ifdef PRE_ADD_DWC
	if (GetDWCTask().IsDWCChar()) nGuildMid = 121018;
#endif

#ifndef _FINAL_BUILD
	bool bCheckDebugMessage = false;
	if (nChatType == CHATTYPE_NORMAL && __wcsicmp_l(wszFromCharName, L"Debug") == 0) bCheckDebugMessage = true;
#endif
	int nVecSize = (int)m_vecChatDlg.size();
	for (int i = 0; i<nVecSize; i++)
	{
		if (m_vecChatDlg[i])
		{
			CDnChatOptDlg &chatOptDlg = m_vecChatDlg[i]->GetChatOptDlg();

			switch (nChatType)
			{
			case CHAT_NORMAL:
			{
				if (CDnIsolateTask::IsActive() && GetIsolateTask().IsBlackList(wszFromCharName))
					return;

				if (chatOptDlg.IsNomal())
				{
					if (fromCharNameString.empty())
						swprintf_s(wszMsg, 1024, L"%s", wszMessage);
					else
						swprintf_s(wszMsg, 1024, L"[%s] : %s", fromCharNameString.c_str(), wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_NORMAL, 0, isAppend, hasColor, colorValue, bgColorValue);
				}
#ifndef _FINAL_BUILD
				if (bCheckDebugMessage) break;
#endif
			}
			break;
			case CHAT_PARTY:
			{
				if (chatOptDlg.IsParty())
				{
					if (isAppend)
					{
						swprintf_s(wszMsg, 1024, L"%s", wszMessage);
					}
					else
					{
						if (m_IsPVPGameChat)
						{
							if (IsGuildWarMaster(wszFromCharName))
								nChatType = CHAT_GUILDWAR;
							swprintf_s(wszMsg, 1024, L"[%s][%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 121018), fromCharNameString.c_str(), wszMessage);
						}
						else
						{
							if (CDnPartyTask::IsActive() == false)
								return;

							bool bRaidMaster = false;
							if (CDnPartyTask::GetInstance().IsPartyType(_RAID_PARTY_8))
							{
								CDnPartyTask::PartyStruct* pData = CDnPartyTask::GetInstance().GetPartyDataFromMemberName(wszFromCharName);
								if (pData == NULL)
									return;

								if (pData->bPartyMaster)
								{
									swprintf_s(wszMsg, 1024, L"[%s]%s : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3838), fromCharNameString.c_str(), wszMessage); // UISTRING : 공대장
									bRaidMaster = true;
								}
								else
								{
									swprintf_s(wszMsg, 1024, L"[%s]%s : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3839), fromCharNameString.c_str(), wszMessage); // UISTRING : 공격대원
								}
							}
							else
							{
								if (fromCharNameString.empty())
									swprintf_s(wszMsg, 1024, L"[%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 632), wszMessage);
								else
									swprintf_s(wszMsg, 1024, L"[%s][%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 632), fromCharNameString.c_str(), wszMessage);
							}
						}
					}

					if (!m_bCustomizeChatMode)
					{
						m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, nChatType, 0, isAppend, hasColor, colorValue, bgColorValue);
					}
					else
					{
						if (m_nCustomizeModeType == GetChatMode() && nChatType == m_nCustomizeModeType)
						{
							swprintf_s(wszMsg, 1024, L"[%s][%s] : %s", m_wszCustomizeModeHeader, fromCharNameString.c_str(), wszMessage);
							m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, nChatType, 0, isAppend, hasColor, colorValue, bgColorValue);
						}
						else
						{
							m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, nChatType, 0, isAppend, hasColor, colorValue, bgColorValue);
						}
					}

					nChatType = CHAT_PARTY;
				}
			}
			break;
			case CHAT_RAIDNOTICE:
			{
				std::wstring chatContents = wszMessage;
				if (wszMessage[0] == L'/')
				{
					int nNumArg = 0;
					std::wstring::size_type endIdx;
					endIdx = chatContents.find_first_of(L" ");
					if (endIdx != std::wstring::npos)
						chatContents = chatContents.substr(endIdx, std::wstring::npos);
				}
				std::wstring chatBoxStr;
				std::wstring captionStr;
				if (CDnBridgeTask::GetInstance().IsPvPGameMode(PvPCommon::GameMode::PvP_GuildWar))
				{
					if (!IsGuildWarMaster(wszFromCharName))
						return;

					int nPvPUserState = GetGuildWarUserState(wszFromCharName);

					if (PvPCommon::UserState::GuildWarCaptain & nPvPUserState)
					{
						chatBoxStr = FormatW(L"[%s]%s:%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2373), fromCharNameString.c_str(), chatContents.c_str()); // UISTRING : 마스터
						captionStr = FormatW(L"[%s]:%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2373), chatContents.c_str());
					}
					else if (PvPCommon::UserState::GuildWarSedcondCaptain & nPvPUserState)
					{
						chatBoxStr = FormatW(L"[%s]%s:%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 126290), fromCharNameString.c_str(), chatContents.c_str()); // UISTRING : 부마스터
						captionStr = FormatW(L"[%s]:%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 126290), chatContents.c_str());
					}
				}
				else
				{
					chatBoxStr = FormatW(L"[%s]%s:%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3838), fromCharNameString.c_str(), chatContents.c_str()); // UISTRING : 공대장
					captionStr = FormatW(L"[%s]:%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3838), chatContents.c_str());
				}
				m_vecChatDlg[i]->AddChat(wszFromCharName, chatBoxStr.c_str(), CHAT_RAIDNOTICE, 0, isAppend, hasColor, colorValue, bgColorValue);
				GetInterface().ShowCaptionDialog(CDnInterface::typeCaption4, captionStr.c_str(), chatcolor::RAIDNOTICE);
			}
			break;
			case CHAT_GUILD:
			{
				if (chatOptDlg.IsGuild())
				{
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
					if (bIsDoorsMobile)
						swprintf_s(wszMsg, 1024, L"[%s][%s][m] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nGuildMid), fromCharNameString.c_str(), wszMessage);
					else
						swprintf_s(wszMsg, 1024, L"[%s][%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nGuildMid), fromCharNameString.c_str(), wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_GUILD, 0, isAppend, hasColor, colorValue, bgColorValue);
#else
					swprintf_s(wszMsg, 1024, L"[%s][%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nGuildMid), fromCharNameString.c_str(), wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_GUILD, 0, isAppend, hasColor, colorValue, bgColorValue);
#endif
				}
			}
			break;
			case CHAT_PRIVATE:
			{
				CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::INVENTORY_DIALOG);
				if (pInvenDlg)
				{
					// wszFromCharName과 거래중인 상대방 이름이 같을 경우에만 내 말풍선에 등록.
					if (pInvenDlg->IsYourName(wszFromCharName))
						pInvenDlg->AddPrivateMarketChatYou(wszMessage);
				}

				// IsPrivate옵션 여부와 상관없이 이건 저장해둔다.
				// 이렇게 해야 Shift+r로 응답이 가능해진다.
				SetLastPrivateName(wszFromCharName);

				if (chatOptDlg.IsPrivate())
				{
#ifdef PRE_ADD_DOORS
					if (bIsDoorsMobile)
						swprintf_s(wszMsg, 1024, L"[%s][m]>>> : %s", fromCharNameString.c_str(), wszMessage);
					else
						swprintf_s(wszMsg, 1024, L"[%s]>>> : %s", fromCharNameString.c_str(), wszMessage);
#else
					swprintf_s(wszMsg, 1024, L"[%s]>>> : %s", fromCharNameString.c_str(), wszMessage);
#endif
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_PRIVATE, 0, isAppend, hasColor, colorValue, bgColorValue);
					AddPrivateName(wszFromCharName);
				}
			}
			break;
			case CHAT_CHANNEL:
			{
				if (chatOptDlg.IsChannel())
				{
					swprintf_s(wszMsg, 1024, L"[%s][%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 605), fromCharNameString.c_str(), wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_CHANNEL, 0, isAppend, hasColor, colorValue, bgColorValue);
				}
			}
			break;
			case CHAT_WORLD:
			{
				if (chatOptDlg.IsWorld())
				{
					swprintf_s(wszMsg, 1024, L"[%s][%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 606), fromCharNameString.c_str(), wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_WORLD, 0, isAppend, hasColor, colorValue, bgColorValue);
				}
			}
			break;
#ifdef PRE_PRIVATECHAT_CHANNEL
			case CHAT_PRIVATE_CHANNEL:
			{
				if (chatOptDlg.IsPrivateChannel())
				{
					if (fromCharNameString.length() > 0)
						swprintf_s(wszMsg, 1024, L"[%s][%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1202), fromCharNameString.c_str(), wszMessage);
					else
						swprintf_s(wszMsg, 1024, L"[%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1202), wszMessage);

					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_PRIVATE_CHANNEL, 0, isAppend, hasColor, colorValue, bgColorValue);
				}
			}
			break;
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_WORLD_MSG_RED
			case CHAT_WORLD_RED:
			{
				if (chatOptDlg.IsWorld())
				{
					swprintf_s(wszMsg, 1024, L"[%s][%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 606), fromCharNameString.c_str(), wszMessage);
					m_vecChatDlg[i]->AddChat(wszFromCharName, wszMsg, CHAT_WORLD_RED, 0, isAppend, hasColor, colorValue, bgColorValue);
				}
			}
			break;
#endif // PRE_ADD_WORLD_MSG_RED
			}
		}
	}

#ifdef PRE_ADD_WORLD_MSG_RED
	if (!m_bPVPLobbyChat && nChatType == CHAT_WORLD_RED)
	{
		if (m_pWorldChatRed)
		{
			m_pWorldChatRed->ClearText();
			swprintf_s(wszMsg, 1024, L"[%s][%s]", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 606), fromCharNameString.c_str());
			m_pWorldChatRed->AddText(wszMsg, textcolor::WHITE);
			swprintf_s(wszMsg, 1024, L"%s", wszMessage);
			m_pWorldChatRed->AddText(wszMsg, chatcolor::WORLD_RED);
			int nLineCount = 4 - m_pWorldChatRed->GetLineSize();
			for (int i = 0; i<nLineCount; i++)
				m_pWorldChatRed->AddText(L"\n");
			m_fWorldMsgRedCoolTime = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::WorldMessageRedCoolTime) / 1000.0f;
			m_nBlinkCount = 1;//static_cast<int>( CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::WorldMessageRedCoolTime ) / 1000.0f ) % 3;
			m_fWorldMsgRed1Sec = 0.3f;
		}
	}
#endif // PRE_ADD_WORLD_MSG_RED
}

void CDnChatTabDlg::AppendChatEditBox(LPCWSTR wszString, bool bFocus)
{
	if (m_pIMEEditBox->GetFullTextLength() + lstrlenW(wszString) >= CHATLENMAX)
		return;

	if (IsShow() == false)
	{
		Show(true);
		if (bFocus == false)
			focus::ReleaseControl();
	}
	m_pIMEEditBox->AddText(wszString, bFocus);
}

void CDnChatTabDlg::AddSystemMessage(LPCWSTR wszFromCharName, LPCWSTR wszMessage, bool bShowCaption)
{
	wchar_t wszMsg[1024] = { 0 };

	if (wszFromCharName == NULL || wszFromCharName[0] == '\0')
	{
		swprintf_s(wszMsg, 1024, L"%s", wszMessage);
	}
	else
	{
		// 이렇게 직접 하나에만 넣는거 바꿔야할듯.
		// 그런데 시스템 메세지에도 이름 넣어서 들어오는게 있긴 한가?
		std::wstring strFromCharName;
		CDnNameLinkMng* pMgr = GetInterface().GetNameLinkMng();
		if (pMgr)
			pMgr->MakeNameLinkString_UserName(strFromCharName, wszFromCharName);

		swprintf_s(wszMsg, 1024, L"[%s] : %s", strFromCharName.c_str(), wszMessage);
	}
	AddChatDlg(wszMsg, CHAT_SYSTEM);

	if (bShowCaption)
		GetInterface().ShowCaptionDialog(CDnInterface::typeCaption3, wszMessage, textcolor::YELLOW, 5.0f);
}

void CDnChatTabDlg::AddNoticeMessage(LPCWSTR wszFromCharName, LPCWSTR wszMessage)
{
	wchar_t wszMsg[1024] = { 0 };

	if (wszFromCharName == NULL || wszFromCharName[0] == '\0')
	{
		// 서버에서 '공지'단어를 안보내는 것으로 바뀌어서 이렇게 직접 처리한다.
		swprintf_s(wszMsg, 1024, L"[%s] : %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 96), wszMessage);
	}
	else
	{
		std::wstring strFromCharName;
		CDnNameLinkMng* pMgr = GetInterface().GetNameLinkMng();
		if (pMgr)
			pMgr->MakeNameLinkString_UserName(strFromCharName, wszFromCharName);

		swprintf_s(wszMsg, 1024, L"[%s] : %s", strFromCharName.c_str(), wszMessage);
	}

	// 기존 방법이 이상하게 되있어서, 아래처럼 직접 넣는 것으로 우선 커버하겠다.
	m_pChatNormalDlg->AddChat(wszFromCharName, wszMsg, CHAT_NOTICE);
#ifdef PRE_ADD_CHAT_RENEWAL
	m_pChatWhisperDlg->AddChat(wszFromCharName, wszMsg, CHAT_NOTICE); // 귓속말.
#endif
	m_pChatPartyDlg->AddChat(wszFromCharName, wszMsg, CHAT_NOTICE);
	m_pChatGuildDlg->AddChat(wszFromCharName, wszMsg, CHAT_NOTICE);
	m_pChatSystemDlg->AddChat(wszFromCharName, wszMsg, CHAT_NOTICE);

	GetInterface().ShowCaptionDialog(CDnInterface::typeCaption1, wszMessage, textcolor::HOTPINK);
}

bool CDnChatTabDlg::CheckRepeat()
{
	std::wstring wszString = m_pIMEEditBox->GetText();

	return CheckRepeat(wszString);
}

bool CDnChatTabDlg::CheckRepeat(const std::wstring& chatMsg)
{
	const UINT nMaxCount = 5;

	// 예외처리
	if (chatMsg == L"")
		return false;

	if (m_wszPrevString != chatMsg)
	{
		m_wszPrevString = chatMsg;
		m_nRepeat = 1;
	}
	else
	{
		if (m_nRepeat >= nMaxCount)
			return true;

		++m_nRepeat;
	}
	return false;
}

bool CDnChatTabDlg::CheckPapering()
{
	// 채팅도배로 금지시간 중이라면,
	if (m_fPaperingRemainTime > 0.0f)
		return true;

	// 추가했을때 제한 횟수에 걸리면,
	if ((int)m_listRecentChatTime.size() + 1 >= CHAT_PAPERING_CHECKCOUNT)
		m_fPaperingRemainTime = (float)CHAT_PAPERING_RESTRICTIONTIME;

	m_listRecentChatTime.push_back((float)CHAT_PAPERING_CHECKTIME);
	return false;
}

void CDnChatTabDlg::AddChatQuestMsg(LPCWSTR wszMsg)
{
	AddChatDlg(wszMsg, CHAT_SYSTEM);
	GetInterface().ShowCaptionDialog(CDnInterface::typeCaption2, wszMsg, textcolor::YELLOW);
}

void CDnChatTabDlg::AddChatQuestReceipt(LPCWSTR wszQuestName)
{
	wchar_t wszMsg[256] = { 0 };
#if defined(PRE_ADD_ENGLISH_STRING)
	swprintf_s(wszMsg, 256, L"\"%s\" %s", wszQuestName, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 635));
#else
	swprintf_s(wszMsg, 256, L"%s, %s %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 647), wszQuestName, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 635));
#endif
	AddChatDlg(wszMsg, CHAT_SYSTEM);

	GetInterface().ShowCaptionDialog(CDnInterface::typeCaption2, wszMsg, textcolor::YELLOW);
}

void CDnChatTabDlg::AddChatQuestCompletion(LPCWSTR wszQuestName)
{
	wchar_t wszMsg[256] = { 0 };
#if defined(PRE_ADD_ENGLISH_STRING)
	swprintf_s(wszMsg, 256, L"\"%s\" %s", wszQuestName, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 636));
#else
	swprintf_s(wszMsg, 256, L"%s, %s %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 647), wszQuestName, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 636));
#endif
	AddChatDlg(wszMsg, CHAT_SYSTEM);

	GetInterface().ShowCaptionDialog(CDnInterface::typeCaption2, wszMsg, textcolor::YELLOW);
}

void CDnChatTabDlg::AddChatDlg(LPCWSTR wszMsg, int nChatType)
{
	int nVecSize = (int)m_vecChatDlg.size();
	for (int i = 0; i<nVecSize; i++)
	{
		if (m_vecChatDlg[i])
		{
			CDnChatOptDlg &chatOptDlg = m_vecChatDlg[i]->GetChatOptDlg();

			if (chatOptDlg.GetOption() & nChatType)
			{
				m_vecChatDlg[i]->AddChat(NULL, wszMsg, nChatType);
			}
		}
	}
}

CDnChatDlg *CDnChatTabDlg::GetCurrentChatDlg()
{
	CDnChatDlg *pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
	return pChatDlg;
}

void CDnChatTabDlg::ShowChatDlg()
{
	m_groupTabDialog.ShowDialog(GetCurrentTabID(), true);

	// 이 ShowChatDlg() 함수는 CloseAllDialog 후 다시 채팅창을 보이게 할때 호출되는데,
	// 이때 아래의 스크롤바 처리를 하지 않으면 채팅탭이 Show(활성화)되어있지 않음에도 불구하고 스크롤바가 보여지게 된다.
	// 그래서 강제로 스크롤바를 보이지 않게 처리하도록 하겠다.

#ifdef PRE_MOD_CHATBG
	if (m_pChatBoardDlg)
		m_pChatBoardDlg->Show(IsBackGroundMode() ? true : false);
#endif

	if (!IsShow())
	{
		CDnChatDlg *pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
		if (!pChatDlg) return;
		pChatDlg->ShowScrollBar(false);
	}
}

void CDnChatTabDlg::Show(bool bShow)
{
	if (m_bIgnoreShowFunc)
	{
		m_bIgnoreShowFunc = false;
		return;
	}

	if (!m_bEndInitialize)
	{
		ShowEx(bShow);
		return;
	}

	CEtUIDialog::Show(bShow);
	ShowControl(bShow);

	m_groupTabDialog.ShowDialog(GetCurrentTabID(), bShow);

	if (bShow)
	{
		RequestFocus(m_pIMEEditBox);
#ifdef PRE_ADD_DWC
		if (GetDWCTask().IsDWCChar())
		{
			if (m_pGuildTab)
				m_pGuildTab->GetProperty()->nTooltipStringIndex = 121018;
		}
#endif
	}
	else
	{
		m_pIMEEditBox->ClearText();
		focus::ReleaseControl();
		m_bShowOptDlg = false;
	}

	CDnMouseCursor::GetInstance().ShowCursor(bShow);
}

void CDnChatTabDlg::ShowEx(bool bShow)
{
	if (bShow && GetInterface().IsShowCashShopDlg())
		return;

	SetRenderPriority(this, bShow);

	if (m_bShow == bShow)
	{
		if (bShow)
		{
			RequestFocus(m_pIMEEditBox);
			m_pIMEEditBox->CheckIMEToggleState();

			CDnMouseCursor::GetInstance().ShowCursor(true, true);
		}
		return;
	}

	CDnChatDlg *pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
	if (!pChatDlg) return;

	CEtUIDialog::Show(bShow);
	ShowControl(bShow);

#ifdef PRE_ADD_WORLD_MSG_RED
	if (!m_bPVPLobbyChat && m_pWorldChatRedBoard)
	{
		if (m_fWorldMsgRedCoolTime > 0)
			m_pWorldChatRedBoard->Show(bShow);
	}
#endif // PRE_ADD_WORLD_MSG_RED

	pChatDlg->ShowScrollBar(bShow);

	if (bShow)
	{
		RequestFocus(m_pIMEEditBox);
		m_pIMEEditBox->CheckIMEToggleState();
		CDnMouseCursor::GetInstance().ShowCursor(true);
	}
	else
	{
		ShowChildDialog(m_pChatHelpDlg, false);
		pChatDlg->ShowOptDlg(false);

		m_pIMEEditBox->ClearText();
		if (m_pIMEEditBox->IsFocus())
			focus::ReleaseControl();

		m_bShowOptDlg = false;

		if (!drag::IsValid())
		{
			CDnMouseCursor::GetInstance().ShowCursor(false);
		}
		else
		{
			CDnMouseCursor::GetInstance().ReleaseShowRefCnt();
		}

		// 예외처리. 대고 있거나 드래그 중 Esc
		if (m_nSizeMode == SIZE_OVER || m_nSizeMode == SIZE_DRAG)
		{
			m_nSizeMode = SIZE_BASE;
			CDnMouseCursor::GetInstance().SetCursorState(CDnMouseCursor::NORMAL);
		}
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
		SetPrivateChannelNotify(false);
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
	}

#ifdef PRE_MOD_CHATBG
	if (m_pChatBoardDlg)
	{
		m_pChatBoardDlg->Show(IsBackGroundMode() ? true : bShow);
	}
#endif
}

#ifdef PRE_MOD_CHATBG
bool CDnChatTabDlg::IsBackGroundMode() const
{
	const CDnChatDlg *pChatDlg = static_cast<const CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
	if (pChatDlg)
	{
		const CDnChatOptDlg &chatOptDlg = pChatDlg->GetChatOptDlg();
		return chatOptDlg.IsCheckBackground();
	}

	return false;
}
#endif

void CDnChatTabDlg::ShowControl(bool bShow)
{
	m_pNormalTab->Show(bShow);
#ifdef PRE_ADD_CHAT_RENEWAL
	m_pWhisperTab->Show(bShow);
#endif
	m_pPartyTab->Show(bShow);
	m_pGuildTab->Show(bShow);
	m_pSystemTab->Show(bShow);
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_pPrivateChannelTab->Show(bShow);
#endif // PRE_PRIVATECHAT_CHANNEL

	m_pButtonModeUp->Show(bShow);
	m_pButtonModeDown->Show(bShow);
	m_pButtonOption->Show(bShow);
	m_pButtonHelp->Show(bShow);
	m_pButtonReport->Show(false);

	m_pStaticMode->Show(bShow);
	m_pIMEEditBox->Show(bShow);
}

void CDnChatTabDlg::ChangePrivateName()
{
	if (!m_listPrivateName.empty())
	{
		m_strPrivateUserName = m_listPrivateName.front();
		m_listPrivateName.pop_front();
		m_listPrivateName.push_back(m_strPrivateUserName);

		wchar_t wszMsg[256] = { 0 };
		swprintf_s(wszMsg, 256, L"[%s][%s]:", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 648), m_strPrivateUserName.c_str());
		ResizeIMECCtl(wszMsg);
	}
}

void CDnChatTabDlg::AddPrivateName(const std::wstring strPrivateName)
{
	PRIVATENAME_LIST_ITER iter = m_listPrivateName.begin();
	for (; iter != m_listPrivateName.end(); )
	{
		if ((*iter) == strPrivateName)
		{
			iter = m_listPrivateName.erase(iter);
		}
		else ++iter;
	}

	m_listPrivateName.push_back(strPrivateName);
}

void CDnChatTabDlg::DelPrivateName(const std::wstring &strPrivateName)
{
	return;

	PRIVATENAME_LIST_ITER iter = m_listPrivateName.begin();
	for (; iter != m_listPrivateName.end(); )
	{
		if ((*iter) == strPrivateName)
		{
			iter = m_listPrivateName.erase(iter);
		}
		else ++iter;
	}

	if (!m_listPrivateName.empty())
	{
		ChangePrivateName();
	}
	else
	{
		SetChatMode(CHAT_NORMAL);
		m_strPrivateUserName.clear();
	}
}

void CDnChatTabDlg::SetPrivateName(const std::wstring &strPrivateName)
{
	m_strPrivateUserName = strPrivateName;

	SetChatMode(CHAT_PRIVATE);
	m_pIMEEditBox->ClearText();
	RequestFocus(m_pIMEEditBox);
}

bool CDnChatTabDlg::GetLastPrivateName(std::wstring &strLastPrivateName)
{
	if (!m_strLastPrivateUserName.empty())
	{
		strLastPrivateName = m_strLastPrivateUserName;
		return true;
	}

	return false;
}

void CDnChatTabDlg::SetLastPrivateName(LPCWSTR wszLastPrivateName)
{
	ASSERT(wszLastPrivateName&&"CDnChatTabDlg::SetLastPrivateName");
	m_strLastPrivateUserName = wszLastPrivateName;
}

void CDnChatTabDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	switch (nID)
	{
	case CHAT_HELP_DIALOG:
		if (nCommand == EVENT_TEXTBOX_SELECTION)
		{
			CEtUITextBox *pTextBox = (CEtUITextBox *)pControl;
			if (!pTextBox) return;

			SLineData sLineData;
			if (pTextBox->GetSelectedLineData(sLineData))
			{
				SetChatMode(sLineData.m_nData);
			}

			ShowChildDialog(m_pChatHelpDlg, false);
			RequestFocus(m_pIMEEditBox);
		}
		break;
	}
}

void CDnChatTabDlg::AddChatHistotry(LPCWSTR wszChat)
{
	if (m_listChatHistory.size() > 20)
	{
		m_listChatHistory.pop_back();
		m_listChatHistory.push_front(wszChat);
	}
	else
	{
		m_listChatHistory.push_front(wszChat);
	}
}

void CDnChatTabDlg::SetPrevChatHistory()
{
	if (!m_listChatHistory.empty())
	{
		std::wstring strChat = m_listChatHistory.back();
		m_listChatHistory.pop_back();
		m_listChatHistory.push_front(strChat);

		m_pIMEEditBox->SetText(strChat.c_str());
	}
}

void CDnChatTabDlg::SetNextChatHistory()
{
	if (!m_listChatHistory.empty())
	{
		std::wstring strChat = m_listChatHistory.front();
		m_listChatHistory.pop_front();
		m_listChatHistory.push_back(strChat);

		m_pIMEEditBox->SetText(strChat.c_str());
	}

	return;
}

void CDnChatTabDlg::CustomizeChatMode(bool bTrue, int nModeType, LPCWSTR wszHeader, DWORD dwTextColor)
{
	m_bCustomizeChatMode = bTrue;
	m_nCustomizeModeType = nModeType;
	m_wszCustomizeModeHeader = wszHeader;
	m_dwCustomizeModeTextColor = dwTextColor;

	SetChatModeText();

};

bool CDnChatTabDlg::IsGuildWarMaster(LPCWSTR wszFromCharName)
{
	if (CDnBridgeTask::GetInstance().IsPvPGameMode(PvPCommon::GameMode::PvP_GuildWar))
	{
		int nPvPUserState = GetGuildWarUserState(wszFromCharName);

		if (PvPCommon::UserState::GuildWarCaptain & nPvPUserState
			|| PvPCommon::UserState::GuildWarSedcondCaptain & nPvPUserState)
			return true;
	}

	return false;
}

UINT CDnChatTabDlg::GetGuildWarUserState(LPCWSTR wszFromCharName)
{
	CDnPartyTask::PartyStruct * pData = GetPartyTask().GetPartyDataFromMemberName(wszFromCharName);

	if (pData)
	{
		if (!CDnLocalPlayerActor::s_hLocalActor) return 0;
		CDnLocalPlayerActor *pLocalActor = dynamic_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
		if (!pLocalActor) return 0;
		if (pLocalActor->GetTeam() != pData->usTeam)
			return 0;

		return pData->uiPvPUserState;
	}

	return 0;
}

void CDnChatTabDlg::ClearText()
{
	m_pIMEEditBox->ClearText();
}