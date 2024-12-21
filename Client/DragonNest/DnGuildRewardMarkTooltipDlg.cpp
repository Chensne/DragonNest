#include "StdAfx.h"

#include "DnGuildRewardMarkTooltipDlg.h"
#include "DnTableDB.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnCommonUtil.h"
#include "DnUIString.h"
#include "DnGuildTask.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildRewardMarkTooltipDlg::CDnGuildRewardMarkTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) : CEtUIDialog( dialogType, pParentDialog, nID, pCallback ), 
																																						 m_pContents( NULL )
{
}

CDnGuildRewardMarkTooltipDlg::~CDnGuildRewardMarkTooltipDlg(void)
{
}

void CDnGuildRewardMarkTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SimpleTooltipDlg.ui" ).c_str(), bShow );
}

void CDnGuildRewardMarkTooltipDlg::InitialUpdate()
{
	m_pContents = GetControl<CEtUITextBox>( "ID_TEXTBOX" );
}

bool CDnGuildRewardMarkTooltipDlg::AdjustPosition(float orgX, float orgY)
{
	float fRight = orgX + m_pContents->GetUICoord().fWidth;
	if (fRight > GetScreenWidthRatio())
		orgX -= m_pContents->GetUICoord().fWidth;

	SetPosition(orgX, orgY);

	return true;
}

#ifdef PRE_ADD_BEGINNERGUILD
bool CDnGuildRewardMarkTooltipDlg::SetMarkBeginnerTooltip()
{
	std::wstring result;

	TGuild* pGuild = GetGuildTask().GetGuildInfo();
	if (pGuild == NULL)
		return false;

	int guildGraduateLevel = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::BeginnerGuild_GraduateLevel);
	result = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1593), pGuild->GuildView.wszGuildName, guildGraduateLevel);
	m_pContents->AddColorText(result.c_str());

	return true;
}

bool CDnGuildRewardMarkTooltipDlg::SetMarkNormalTooltip()
{
	std::wstring result;

	TGuild* pGuild = GetGuildTask().GetGuildInfo();
	if (pGuild == NULL)
		return false;

	//1. 길드 이름
	result = FormatW(L"[%s]", pGuild->GuildView.wszGuildName);
	m_pContents->AddColorText(result.c_str(), textcolor::GREENYELLOW);
	//1.1 길드에 소속되어 있으면 아래 혜택이 적용됩니다.
	//m_pContents->AddText(L"길드에 소속되어 있으면 아래 혜택이 적용됩니다.");
	m_pContents->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3374));
	m_pContents->AddText(L"");

	//2.혜택 조회
	result = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3375);
	m_pContents->AddColorText(result.c_str(), textcolor::GREENYELLOW);
	//2.1 커뮤니티 -> 길드 -> 길드 정보
	//m_pContents->AddText(L"커뮤니티 -> 길드 -> 길드 정보");
	m_pContents->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3376));
	m_pContents->AddText(L"");

	//3.길드 혜택
	//result = FormatW(L"[%s]", L"길드 혜택");
	result = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3360);
	m_pContents->AddColorText(result.c_str(), textcolor::GREENYELLOW);

	//3.1 길드 혜택 정보들
	CDnGuildTask::GUILDREWARDINFO_LIST rewardList;
	int nCount = GetGuildTask().CollectGuildRewardInfo(rewardList);
	if (nCount == 0)
	{
		//m_pContents->AddText(L"혜택이 없습니다.");
		m_pContents->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3390));
	}
	else
	{
		CDnGuildTask::GUILDREWARDINFO_LIST::iterator iter = rewardList.begin();
		CDnGuildTask::GUILDREWARDINFO_LIST::iterator endIter = rewardList.end();
		for (; iter != endIter; ++iter)
		{
			GuildReward::GuildRewardInfo &info = (*iter);

			std::wstring msg;
			MakeUIStringUseVariableParam( msg, info._NameID, (char*)info._NameIDParam.c_str() );

			m_pContents->AddText(msg.c_str());

			std::wstring wszDate;
			if (info._Period == 0)
			{
				//m_pContents->AppendText(L" : 무제한", textcolor::RED);
				wszDate = FormatW(L" : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4705));
				m_pContents->AppendText(wszDate.c_str(), textcolor::RED);
			}
			else
			{
				__time64_t remainPeriod = GetGuildTask().GetRemainPeriod(info);
				result = FormatW(L"~ ");
				m_pContents->AppendText(result.c_str(), textcolor::RED);

#ifdef _US
				DN_INTERFACE::STRING::GetDayTextSlash( DN_INTERFACE::STRING::FORMAT_MM_DD_YY, wszDate, remainPeriod );
#else // _US
				DN_INTERFACE::STRING::GetDayText( wszDate, remainPeriod );
#endif // _US
				m_pContents->AppendText(wszDate.c_str(), textcolor::RED);
			}
		}
	}

	return true;
}
#endif // PRE_ADD_BEGINNERGUILD

bool CDnGuildRewardMarkTooltipDlg::SetMarkTooltip()
{
	m_pContents->ClearText();

	if (CDnGuildTask::IsActive() == false)
		return false;

#ifdef PRE_ADD_BEGINNERGUILD
	if (GetGuildTask().IsBeginnerGuild())
		SetMarkBeginnerTooltip();
	else
		SetMarkNormalTooltip();
#else
	std::wstring result;

	TGuild* pGuild = GetGuildTask().GetGuildInfo();
	if (pGuild == NULL)
		return false;

	//1. 길드 이름
	result = FormatW(L"[%s]", pGuild->GuildView.wszGuildName);
	m_pContents->AddColorText(result.c_str(), textcolor::GREENYELLOW);
	//1.1 길드에 소속되어 있으면 아래 혜택이 적용됩니다.
	//m_pContents->AddText(L"길드에 소속되어 있으면 아래 혜택이 적용됩니다.");
	m_pContents->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3374));
	m_pContents->AddText(L"");

	//2.혜택 조회
	result = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3375);
	m_pContents->AddColorText(result.c_str(), textcolor::GREENYELLOW);
	//2.1 커뮤니티 -> 길드 -> 길드 정보
	//m_pContents->AddText(L"커뮤니티 -> 길드 -> 길드 정보");
	m_pContents->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3376));
	m_pContents->AddText(L"");

	//3.길드 혜택
	//result = FormatW(L"[%s]", L"길드 혜택");
	result = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3360);
	m_pContents->AddColorText(result.c_str(), textcolor::GREENYELLOW);

	//3.1 길드 혜택 정보들
	CDnGuildTask::GUILDREWARDINFO_LIST rewardList;
	int nCount = GetGuildTask().CollectGuildRewardInfo(rewardList);
	if (nCount == 0)
	{
		//m_pContents->AddText(L"혜택이 없습니다.");
		m_pContents->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3390));
	}
	else
	{
		CDnGuildTask::GUILDREWARDINFO_LIST::iterator iter = rewardList.begin();
		CDnGuildTask::GUILDREWARDINFO_LIST::iterator endIter = rewardList.end();
		for (; iter != endIter; ++iter)
		{
			GuildReward::GuildRewardInfo &info = (*iter);

			std::wstring msg;
			MakeUIStringUseVariableParam( msg, info._NameID, (char*)info._NameIDParam.c_str() );
			
			m_pContents->AddText(msg.c_str());

			std::wstring wszDate;
			if (info._Period == 0)
			{
				//m_pContents->AppendText(L" : 무제한", textcolor::RED);
				wszDate = FormatW(L" : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4705));
				m_pContents->AppendText(wszDate.c_str(), textcolor::RED);
			}
			else
			{
				__time64_t remainPeriod = GetGuildTask().GetRemainPeriod(info);
				result = FormatW(L"~ ");
				m_pContents->AppendText(result.c_str(), textcolor::RED);

#ifdef _US
				DN_INTERFACE::STRING::GetDayTextSlash( DN_INTERFACE::STRING::FORMAT_MM_DD_YY, wszDate, remainPeriod );
#elif _RU
				DN_INTERFACE::STRING::GetDayTextSlash( DN_INTERFACE::STRING::FORMAT_DD_MM_YY, wszDate, remainPeriod, '.' );
#else // _US
				DN_INTERFACE::STRING::GetDayText( wszDate, remainPeriod );
#endif // _US
				m_pContents->AppendText(wszDate.c_str(), textcolor::RED);
			}
		}
	}
#endif // PRE_ADD_BEGINNERGUILD

	return true;
}

bool CDnGuildRewardMarkTooltipDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_bShow == false)
		return false;

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			float fMouseX = 0;
			float fMouseY = 0;
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			if (m_pContents && m_pContents->IsInside(fMouseX, fMouseY))
				Show(false);
		}
	}

	return bRet;
}