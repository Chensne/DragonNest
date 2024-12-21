#include "stdafx.h"

#include "DnInspectGuildInfoDlg.h"
#include "DnTableDB.h"
#include "DnActor.h"
#include "DnPlayerActor.h"
#include "DnUIString.h"
#include "DnInterfaceString.h"
#include "DnInspectPlayerDlg.h"

#include "DnGuildTask.h"

//////////////////////////////////////////////////////////////////////////
// CDnInspectGuildInfoDlg

CDnInspectGuildInfoDlg::CDnInspectGuildInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pPlayerName = NULL;
	m_pGuildName = NULL;
	m_pGuildMasterName = NULL;
	m_pGuildLevel = NULL;
	m_pGuildMembers = NULL;
	m_pGuildRewards = NULL;
	m_pTextureGuildMark = NULL;
}

CDnInspectGuildInfoDlg::~CDnInspectGuildInfoDlg()
{
	
}

void CDnInspectGuildInfoDlg::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "PlayerGuildInfoDlg.ui" ).c_str(), bShow );
}

void CDnInspectGuildInfoDlg::InitialUpdate()
{
	m_pPlayerName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pGuildName = GetControl<CEtUIStatic>("ID_TEXT_GUILDNAME");
	m_pGuildMasterName = GetControl<CEtUIStatic>("ID_TEXT_GUILDMASTER");
	m_pGuildLevel = GetControl<CEtUIStatic>("ID_TEXT_GUILDLEVEL");
	m_pGuildMembers = GetControl<CEtUIStatic>("ID_TEXT_GUILDMEMBER");
	m_pGuildRewards = GetControl<CEtUITextBox>("ID_TEXTBOX_GUILDREWARD");

	m_pTextureGuildMark = GetControl<CEtUITextureControl>("ID_TEXTUREL_GUILDMARK");
}

void CDnInspectGuildInfoDlg::InitCustomControl( CEtUIControl *pControl )
{
	
}

void CDnInspectGuildInfoDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		RefreshGuildInfo();

	CEtUIDialog::Show( bShow );
}

#ifdef PRE_ADD_BEGINNERGUILD
bool CDnInspectGuildInfoDlg::SetBeginnerGuildInfo(const SCPlayerGuildInfo& playerGuildInfo)
{
	std::wstring msg;

	//길드 정보 설정.
	m_pGuildName->SetText(playerGuildInfo.wszGuildName);
	m_pGuildMasterName->SetText(L"");

	msg = FormatW(L"%d / %d", playerGuildInfo.nMemberSize, playerGuildInfo.nGuildSize);
	m_pGuildMembers->SetText(msg.c_str());	//길드인원 : %d / %d

	msg = FormatW(L"%d", playerGuildInfo.nGuildLevel);
	m_pGuildLevel->SetText(msg.c_str());	//길드레벨 : %d

	//길드 혜택
	m_pGuildRewards->AddColorText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1595));

	return true;
}

bool CDnInspectGuildInfoDlg::SetNormalGuildInfo(SCPlayerGuildInfo& playerGuildInfo)
{
	std::wstring msg;

	//길드 정보 설정.
	m_pGuildName->SetText(playerGuildInfo.wszGuildName);
	//msg = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3358 ), playerGuildInfo.wszMasterName);
	msg = playerGuildInfo.wszMasterName;
	m_pGuildMasterName->SetText(msg.c_str());	//길드장 : %s

	msg = FormatW(L"%d / %d", playerGuildInfo.nMemberSize, playerGuildInfo.nGuildSize);
	m_pGuildMembers->SetText(msg.c_str());	//길드인원 : %d / %d

	msg = FormatW(L"%d", playerGuildInfo.nGuildLevel);
	m_pGuildLevel->SetText(msg.c_str());	//길드레벨 : %d
	//길드 혜택

	CDnGuildTask::GUILDREWARDINFO_LIST rewardList;
	int nCount = GetGuildTask().CollectGuildRewardInfo(rewardList, playerGuildInfo.GuildRewardItem);
	if (nCount == 0)
	{
		//m_pContents->AddText(L"혜택이 없습니다.");
		m_pGuildRewards->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3390));
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

			m_pGuildRewards->AddText(msg.c_str());
			//기간 표시는 없앤다.. UI크기 문제..
		}
	}

	return true;
}
#endif

void CDnInspectGuildInfoDlg::RefreshGuildInfo()
{
	CDnInspectPlayerDlg* pParentInspectPlayerDlg = dynamic_cast<CDnInspectPlayerDlg*>( GetParentDialog() );
	if( !pParentInspectPlayerDlg ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pParentInspectPlayerDlg->GetSessionID() );
	if( !hActor ) return;

	CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor*>( hActor.GetPointer() );
	if( !pActor ) return;

	m_pPlayerName->SetText(pActor->GetName());

	TGuildSelfView& guildSelfView = pActor->GetGuildSelfView();
	SCPlayerGuildInfo& playerGuildInfo = pActor->GetPlayerGuildInfo();
	
	m_pGuildRewards->ClearText();

#ifdef PRE_ADD_BEGINNERGUILD
#else
	std::wstring msg;
#endif
	if (guildSelfView.GuildUID.IsSet() == true)
	{
#ifdef PRE_ADD_BEGINNERGUILD
		if (CDnGuildTask::IsActive())
		{
			if (GetGuildTask().IsBeginnerGuild())
				SetBeginnerGuildInfo(playerGuildInfo);
			else
				SetNormalGuildInfo(playerGuildInfo);
		}
#else
		//길드 정보 설정.
		m_pGuildName->SetText(playerGuildInfo.wszGuildName);
		//msg = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3358 ), playerGuildInfo.wszMasterName);
		msg = playerGuildInfo.wszMasterName;
		m_pGuildMasterName->SetText(msg.c_str());	//길드장 : %s

		msg = FormatW(L"%d / %d", playerGuildInfo.nMemberSize, playerGuildInfo.nGuildSize);
		m_pGuildMembers->SetText(msg.c_str());	//길드인원 : %d / %d

		msg = FormatW(L"%d", playerGuildInfo.nGuildLevel);
		m_pGuildLevel->SetText(msg.c_str());	//길드레벨 : %d
		//길드 혜택

		CDnGuildTask::GUILDREWARDINFO_LIST rewardList;
		int nCount = GetGuildTask().CollectGuildRewardInfo(rewardList, playerGuildInfo.GuildRewardItem);
		if (nCount == 0)
		{
			//m_pContents->AddText(L"혜택이 없습니다.");
			m_pGuildRewards->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3390));
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

				m_pGuildRewards->AddText(msg.c_str());
				//기간 표시는 없앤다.. UI크기 문제..
			}
		}
#endif
	}
	else
	{
		//길드 정보 없음..
		//m_pGuildName->SetText(L"없음");
		m_pGuildName->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 93));

		m_pGuildMasterName->SetText(L"");	//길드장 : %s
		m_pGuildMembers->SetText(L"");	//길드인원 : %d / %d
		m_pGuildLevel->SetText(L"");	//길드레벨 : %d
		m_pGuildRewards->AddText(L"");
	}

	if( GetGuildTask().IsShowGuildMark( guildSelfView ) )
	{
		EtTextureHandle hGuildMark = GetGuildTask().GetGuildMarkTexture( guildSelfView );
		m_pTextureGuildMark->SetTexture( hGuildMark );
		m_pTextureGuildMark->Show( true );
	}
	else
		m_pTextureGuildMark->Show( false );
}


void CDnInspectGuildInfoDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	
}