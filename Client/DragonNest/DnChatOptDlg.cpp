#include "StdAfx.h"
#include "DnChatOptDlg.h"
#include "EtUICheckBox.h"
#ifdef PRE_MOD_CHATBG
#include "GameOption.h"
#endif
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
#include "DnLocalDataMgr.h"
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChatOptDlg::CDnChatOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pCheckBoxNormal(NULL)
	, m_pCheckBoxParty(NULL)
	, m_pCheckBoxGuild(NULL)
	, m_pCheckBoxPrivate(NULL)
	, m_pCheckBoxSystem(NULL)
#ifdef PRE_MOD_CHATBG
	, m_pCheckBoxBackground(NULL)
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	, m_pCheckBoxPrivateChannel( NULL )
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	, m_nChatTabType( NULL )
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
	//, m_pCheckBoxChannel(NULL)
	//, m_pCheckBoxFriendLog(NULL)
	//, m_pCheckBoxGuildLog(NULL)
	//, m_pCheckBoxQuest(NULL)
	//, m_pCheckBoxDamage(NULL)
	//, m_pCheckBoxMonster(NULL)
	//, m_pCheckBoxLevel(NULL)
	//, m_pCheckBoxBuff(NULL)
	//, m_pCheckBoxSkill(NULL)
	//, m_pCheckBoxPartyDeath(NULL)
	//, m_pCheckBoxItem(NULL)
{
}

CDnChatOptDlg::~CDnChatOptDlg(void)
{
}

void CDnChatOptDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ChatOptDlg.ui" ).c_str(), bShow );
}

void CDnChatOptDlg::InitialUpdate()
{
	m_pCheckBoxNormal = GetControl<CEtUICheckBox>("ID_CHECKBOX_NORMAL");
	m_pCheckBoxParty = GetControl<CEtUICheckBox>("ID_CHECKBOX_PARTY");
	m_pCheckBoxGuild = GetControl<CEtUICheckBox>("ID_CHECKBOX_GUILD");
#ifdef PRE_ADD_DWC
	if(GetDWCTask().IsDWCChar())
		m_pCheckBoxGuild->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121018)); // Mid: 팀
#endif
	m_pCheckBoxPrivate = GetControl<CEtUICheckBox>("ID_CHECKBOX_PRIVATE");
	m_pCheckBoxSystem = GetControl<CEtUICheckBox>("ID_CHECKBOX_SYSTEM");
#ifdef PRE_MOD_CHATBG
	m_pCheckBoxBackground = GetControl<CEtUICheckBox>("ID_CHECKBOX_BG");
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_pCheckBoxPrivateChannel = GetControl<CEtUICheckBox>("ID_CHECKBOX_CHANNEL");
#endif // PRE_PRIVATECHAT_CHANNEL

	//m_pCheckBoxChannel = GetControl<CEtUICheckBox>("ID_CHECKBOX_CHANNEL");
	//m_pCheckBoxFriendLog = GetControl<CEtUICheckBox>("ID_CHECKBOX_FRIENDLOG");
	//m_pCheckBoxGuildLog = GetControl<CEtUICheckBox>("ID_CHECKBOX_GUILDLOG");

	//m_pCheckBoxQuest = GetControl<CEtUICheckBox>("ID_CHECKBOX_QUEST");
	//m_pCheckBoxDamage = GetControl<CEtUICheckBox>("ID_CHECKBOX_DAMAGE");
	//m_pCheckBoxMonster = GetControl<CEtUICheckBox>("ID_CHECKBOX_MONSTER");
	//m_pCheckBoxLevel = GetControl<CEtUICheckBox>("ID_CHECKBOX_LEVEL");
	//m_pCheckBoxBuff = GetControl<CEtUICheckBox>("ID_CHECKBOX_BUFF");
	//m_pCheckBoxSkill = GetControl<CEtUICheckBox>("ID_CHECKBOX_SKILL");
	//m_pCheckBoxPartyDeath = GetControl<CEtUICheckBox>("ID_CHECKBOX_PARTYDEATH");
	//m_pCheckBoxItem = GetControl<CEtUICheckBox>("ID_CHECKBOX_ITEM");
}

void CDnChatOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl("ID_CHECKBOX_NORMAL" ) )
		{
			SetNomal(m_pCheckBoxNormal->IsChecked());
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
			CDnLocalDataMgr::GetInstance().SetChatOptionValue( m_nChatTabType, CHAT_NORMAL, m_pCheckBoxNormal->IsChecked() );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
		}
		else if( IsCmdControl("ID_CHECKBOX_PARTY" ) )
		{
			SetParty(m_pCheckBoxParty->IsChecked());
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
			CDnLocalDataMgr::GetInstance().SetChatOptionValue( m_nChatTabType, CHAT_PARTY, m_pCheckBoxParty->IsChecked() );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
		}
		else if( IsCmdControl("ID_CHECKBOX_GUILD" ) )
		{
			SetGuild(m_pCheckBoxGuild->IsChecked());
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
			CDnLocalDataMgr::GetInstance().SetChatOptionValue( m_nChatTabType, CHAT_GUILD, m_pCheckBoxGuild->IsChecked() );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
		}
		else if( IsCmdControl("ID_CHECKBOX_PRIVATE" ) )	
		{
			SetPrivate(m_pCheckBoxPrivate->IsChecked());
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
			CDnLocalDataMgr::GetInstance().SetChatOptionValue( m_nChatTabType, CHAT_PRIVATE, m_pCheckBoxPrivate->IsChecked() );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
		}
		else if( IsCmdControl("ID_CHECKBOX_SYSTEM" ) )	
		{
			SetSystem(m_pCheckBoxSystem->IsChecked());
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
			CDnLocalDataMgr::GetInstance().SetChatOptionValue( m_nChatTabType, CHAT_SYSTEM, m_pCheckBoxSystem->IsChecked() );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
		}
#ifdef PRE_MOD_CHATBG
		else if (IsCmdControl("ID_CHECKBOX_BG"))
		{
			SetCheckBackground(m_pCheckBoxBackground->IsChecked(), true);
		}
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
		else if (IsCmdControl("ID_CHECKBOX_CHANNEL")) 
		{
			SetPrivateChannel( m_pCheckBoxPrivateChannel->IsChecked() );
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
			CDnLocalDataMgr::GetInstance().SetChatOptionValue( m_nChatTabType, CHAT_PRIVATE_CHANNEL, m_pCheckBoxPrivateChannel->IsChecked() );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
		}
#endif // PRE_PRIVATECHAT_CHANNEL

		//else if( IsCmdControl("ID_CHECKBOX_CHANNEL" ) )		SetChannel(m_pCheckBoxChannel->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_FRIENDLOG" ) )	SetFriendLog(m_pCheckBoxFriendLog->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_GUILDLOG" ) )	SetGuildLog(m_pCheckBoxGuildLog->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_QUEST" ) )		SetQuest(m_pCheckBoxQuest->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_DAMAGE" ) )		SetDamage(m_pCheckBoxDamage->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_MONSTER" ) )		SetMonster(m_pCheckBoxMonster->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_LEVEL" ) )		SetLevel(m_pCheckBoxLevel->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_BUFF" ) )		SetBuff(m_pCheckBoxBuff->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_SKILL" ) )		SetSkill(m_pCheckBoxSkill->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_PARTYDEATH" ) )	SetPartyDeath(m_pCheckBoxPartyDeath->IsChecked());
		//else if( IsCmdControl("ID_CHECKBOX_ITEM" ) )		SetItem(m_pCheckBoxItem->IsChecked());
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnChatOptDlg::InitOption( int nChatType )
{
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	switch( nChatType )
	{
	case CHAT_NORMAL:
		{
			m_nChatTabType = 0;
			m_pCheckBoxNormal->Enable( false );
		}
		break;
	case CHAT_PARTY:
		{
			m_nChatTabType = 1;
			m_pCheckBoxParty->Enable( false );
		}
		break;
	case CHAT_GUILD:
		{
			m_nChatTabType = 2;
			m_pCheckBoxGuild->Enable( false );
		}
		break;
	case CHAT_SYSTEM:
		{
			m_nChatTabType = 3;
			m_pCheckBoxSystem->Enable( false );
		}
		break;
	case CHAT_PRIVATE:
		{
			m_nChatTabType = 4;
			m_pCheckBoxPrivate->Enable( false );
		}
		break;
	case CHAT_PRIVATE_CHANNEL:
		{
			m_nChatTabType = 5;
			m_pCheckBoxPrivateChannel->Enable( false );
		}
		break;
	}

	if( CDnLocalDataMgr::GetInstance().IsExistChatOption( m_nChatTabType ) )	// 저장된 옵션이 있으면 옵션으로 재 설정
	{
		SetCheckNomal( CDnLocalDataMgr::GetInstance().GetChatOptionValue( m_nChatTabType, CHAT_NORMAL ) );
		SetCheckParty( CDnLocalDataMgr::GetInstance().GetChatOptionValue( m_nChatTabType, CHAT_PARTY ) );
		SetCheckGuild( CDnLocalDataMgr::GetInstance().GetChatOptionValue( m_nChatTabType, CHAT_GUILD ) );
		SetCheckPrivate( CDnLocalDataMgr::GetInstance().GetChatOptionValue( m_nChatTabType, CHAT_PRIVATE ) );
		SetCheckPrivateChannel( CDnLocalDataMgr::GetInstance().GetChatOptionValue( m_nChatTabType, CHAT_PRIVATE_CHANNEL ) );
		SetCheckSystem( CDnLocalDataMgr::GetInstance().GetChatOptionValue( m_nChatTabType, CHAT_SYSTEM ) );
#ifdef PRE_MOD_CHATBG
		SetCheckBackground( CGameOption::GetInstance().m_bChatBackground, false );
#endif
		// 채널말과 월드말 등의 무조건 들어야하는 캐쉬말은 별도의 설정없이 항상 켜둔다.
		SetChannel( true );
		SetWorld( true );

		return;
	}
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	switch( nChatType )
	{
	case CHAT_NORMAL:
		{
			// 전체탭은 기본적으로 전부 다 보이게 하자.
			SetCheckNomal( true );
			SetCheckParty( true );
			SetCheckGuild( true );
			SetCheckPrivate( true );
			SetCheckSystem( true );
			//SetCheckChannel( true );
			//SetCheckFriendLog( true );	
			//SetCheckGuildLog( true );
			//SetCheckQuest( true );	
			//SetCheckPartyDeath( true );
			//SetCheckItem( true );
			m_pCheckBoxNormal->Enable( false );
		}
		break;
	case CHAT_PARTY:
		{
			SetCheckParty( true );
			SetCheckGuild( true );
			SetCheckPrivate( true );
			SetCheckSystem( true );
			//SetCheckPartyDeath( true );
			//SetCheckItem( true );
			m_pCheckBoxParty->Enable( false );
		}
		break;
	case CHAT_GUILD:
		{
			SetCheckGuild( true );
			SetCheckPrivate( true );
			SetCheckSystem( true );
			//SetCheckGuildLog( true );
			m_pCheckBoxGuild->Enable( false );
			//m_pCheckBoxGuildLog->Enable( false );
		}
		break;
	case CHAT_SYSTEM:
		{
			SetCheckPrivate( true );
			SetCheckSystem( true );
			m_pCheckBoxSystem->Enable( false );
			//SetCheckFriendLog( true );
			//SetCheckQuest( true );
			//SetCheckDamage( true );
			//SetCheckMonster( true );
			//SetCheckLevel( true );
			//SetCheckBuff( true );
			//SetCheckSkill( true );
			//SetCheckPartyDeath( true );
			//SetCheckItem( true );
		}
		break;
#ifdef PRE_ADD_CHAT_RENEWAL
	case CHAT_PRIVATE:
		{
			SetCheckPrivate( true );
			m_pCheckBoxPrivate->Enable( false );
		}
		break;
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	case CHAT_PRIVATE_CHANNEL:
		{
			SetCheckPrivate( true );
			SetCheckSystem( true );
			SetCheckPrivateChannel( true );
			m_pCheckBoxPrivateChannel->Enable( false );
		}
		break;
#endif // PRE_PRIVATECHAT_CHANNEL
	}

	// 채널말과 월드말 등의 무조건 들어야하는 캐쉬말은 별도의 설정없이 항상 켜둔다.
	SetChannel( true );
	SetWorld( true );
#ifdef PRE_MOD_CHATBG
	SetCheckBackground(CGameOption::GetInstance().m_bChatBackground, false);
#endif
}

void CDnChatOptDlg::SetCheckNomal(bool bCheck)
{
	SetNomal(bCheck);
	m_pCheckBoxNormal->SetChecked(bCheck);
}

void CDnChatOptDlg::SetCheckParty(bool bCheck)
{
	SetParty(bCheck);
	m_pCheckBoxParty->SetChecked(bCheck);
}

void CDnChatOptDlg::SetCheckGuild(bool bCheck)
{
	SetGuild(bCheck);
	m_pCheckBoxGuild->SetChecked(bCheck);
}

void CDnChatOptDlg::SetCheckPrivate(bool bCheck)
{
	SetPrivate(bCheck);
	m_pCheckBoxPrivate->SetChecked(bCheck);
}

void CDnChatOptDlg::SetCheckSystem(bool bCheck)
{
	SetSystem(bCheck);
	m_pCheckBoxSystem->SetChecked(bCheck);
}

#ifdef PRE_MOD_CHATBG
void CDnChatOptDlg::SetCheckBackground(bool bCheck, bool bOnlyCheckFlag)
{
	CGameOption::GetInstance().m_bChatBackground = bCheck;
	if (bOnlyCheckFlag == false)
		m_pCheckBoxBackground->SetChecked(bCheck);
}
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
void CDnChatOptDlg::SetCheckPrivateChannel( bool bCheck )
{
	SetPrivateChannel( bCheck );
	m_pCheckBoxPrivateChannel->SetChecked( bCheck );
}
#endif // PRE_PRIVATECHAT_CHANNEL

//void CDnChatOptDlg::SetCheckChannel( bool bCheck )
//{
//	SetChannel(bCheck);
//	m_pCheckBoxChannel->SetChecked(bCheck);
//}
//
//void CDnChatOptDlg::SetCheckFriendLog( bool bCheck )
//{
//	SetFriendLog(bCheck);
//	m_pCheckBoxFriendLog->SetChecked(bCheck);
//}
//
//void CDnChatOptDlg::SetCheckGuildLog( bool bCheck )
//{
//	SetGuildLog(bCheck);
//	m_pCheckBoxGuildLog->SetChecked(bCheck);
//}

//void CDnChatOptDlg::SetCheckQuest( bool bCheck )
//{
//	SetQuest(bCheck);
//	m_pCheckBoxQuest->SetChecked(bCheck);
//}
//
//void CDnChatOptDlg::SetCheckDamage( bool bCheck )
//{
//	SetDamage(bCheck);
//	m_pCheckBoxDamage->SetChecked(bCheck);
//}
//
//void CDnChatOptDlg::SetCheckMonster( bool bCheck )
//{
//	SetMonster(bCheck);
//	m_pCheckBoxMonster->SetChecked(bCheck);
//}
//
//void CDnChatOptDlg::SetCheckLevel( bool bCheck )
//{
//	SetLevel(bCheck);
//	m_pCheckBoxLevel->SetChecked(bCheck);
//}
//
//void CDnChatOptDlg::SetCheckBuff( bool bCheck )
//{
//	SetBuff(bCheck);
//	m_pCheckBoxBuff->SetChecked(bCheck);
//}
//
//void CDnChatOptDlg::SetCheckSkill( bool bCheck )
//{
//	SetSkill(bCheck);
//	m_pCheckBoxSkill->SetChecked(bCheck);
//}
//
//void CDnChatOptDlg::SetCheckPartyDeath( bool bCheck )
//{
//	SetPartyDeath(bCheck);
//	m_pCheckBoxPartyDeath->SetChecked(bCheck);
//}
//
//void CDnChatOptDlg::SetCheckItem( bool bCheck )
//{
//	SetItem(bCheck);
//	m_pCheckBoxItem->SetChecked(bCheck);
//}