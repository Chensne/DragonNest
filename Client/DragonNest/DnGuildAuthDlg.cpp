#include "StdAfx.h"
#include "DnGuildAuthDlg.h"
#include "DnGuildTask.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildAuthDlg::CDnGuildAuthDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
, m_pButtonOK(NULL)
{
	m_pComboGrade = NULL;
	for( int i = 0; i < GUILDAUTH_TYPE_CNT; ++i )
		m_pCheckOption[i] = NULL;
	m_pComboTakeItem = NULL;
	m_pComboWithDraw = NULL;
	m_dwPrevComboGradeIndex = 0;
	m_bShowChangeMsg = true;
}

CDnGuildAuthDlg::~CDnGuildAuthDlg(void)
{
}

void CDnGuildAuthDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildGradeOptionDlg.ui" ).c_str(), bShow );
}

void CDnGuildAuthDlg::InitialUpdate()
{
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");

	m_pComboGrade = GetControl<CEtUIComboBox>("ID_COMBOBOX0");
	char szName[32] = {0,};
	for( int i = 0; i < GUILDAUTH_TYPE_CNT; ++i )
	{
		sprintf_s( szName, _countof(szName), "ID_CHECKBOX_MASTER%d", i );
		m_pCheckOption[i] = GetControl<CEtUICheckBox>(szName);
	}
	m_pComboTakeItem = GetControl<CEtUIComboBox>("ID_COMBOBOX_ITEM");
	m_pComboWithDraw = GetControl<CEtUIComboBox>("ID_COMBOBOX_MONEY");

	InitControl();
}

void CDnGuildAuthDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_dwPrevComboGradeIndex = m_pComboGrade->GetSelectedIndex();
		RefreshAuth();
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildAuthDlg::InitControl()
{

#ifdef PRE_ADD_CHANGEGUILDROLE
	// 길드장, 길드부장은 모든 권한을 갖는다.
#else
	// 길드장, 길드부장 모두 보이는건 같다. 길드부장부터 신입길드원의 권한 설정 가능.
	m_pComboGrade->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3734 ), NULL, GUILDROLE_TYPE_SUBMASTER );
#endif
	m_pComboGrade->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3735 ), NULL, GUILDROLE_TYPE_SENIOR );
	m_pComboGrade->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3736 ), NULL, GUILDROLE_TYPE_REGULAR );
	m_pComboGrade->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3737 ), NULL, GUILDROLE_TYPE_JUNIOR );

	// 콤보박스에 테이블 값들 쭉 넣어두고.(인덱스 순차적으로 들어있을거라 가정)
	WCHAR wszTemp[64] = {0,};
	for( int i = 0; i < GetGuildTask().GetMaxTakeItemIndexCount(); ++i )
	{
		int nData = GetGuildTask().GetMaxTakeItemData( i );
		if( nData == 0 )
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3816 ) );
		else
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4123 ), nData );
		m_pComboTakeItem->AddItem( wszTemp, NULL, nData );
	}
	for( int i = 0; i < GetGuildTask().GetMaxWithDrawIndexCount(); ++i )
	{
		int nData = GetGuildTask().GetMaxWithDrawData( i );
		if( nData == 0 )
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3816 ) );
		else
			swprintf_s( wszTemp, _countof(wszTemp), L"%d%s", nData / 10000, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 507 ) );
		m_pComboWithDraw->AddItem( wszTemp, NULL, nData );
	}
}

void CDnGuildAuthDlg::RefreshAuth()
{
	if( !CDnGuildTask::IsActive() ) return;
	TGuild *pGuild = GetGuildTask().GetGuildInfo();
	if( !pGuild->IsSet() ) return;

	int nCurRole = -1;
	m_pComboGrade->GetSelectedValue( nCurRole );
	if( nCurRole == -1 ) return;

	for( int i = 0 ; i < GUILDAUTH_TYPE_CNT; ++i ) {
		m_pCheckOption[i]->SetChecked( GetGuildTask().GetAuth( (eGuildRoleType)nCurRole, static_cast<eGuildAuthType>(i) ), false );
		m_pCheckOption[i]->Enable( true );
	}
	// 콤보박스 내용은 길드태스크로부터 아이템
	m_pComboTakeItem->SetSelectedByValue( GetGuildTask().GetMaxTakeItemByRole( (eGuildRoleType)nCurRole ) );
	m_pComboWithDraw->SetSelectedByValue( GetGuildTask().GetMaxWithDrawByRole( (eGuildRoleType)nCurRole ) );
	m_pComboTakeItem->Enable( true );
	m_pComboWithDraw->Enable( true );

	switch( nCurRole )
	{
	case GUILDROLE_TYPE_SENIOR:
		{
			m_pCheckOption[0]->Enable( false );
		}
		break;
	case GUILDROLE_TYPE_REGULAR:
		{
			m_pCheckOption[0]->Enable( false );
			m_pCheckOption[1]->Enable( false );
			m_pCheckOption[2]->Enable( false );
			m_pCheckOption[3]->Enable( false );
		}
		break;
	case GUILDROLE_TYPE_JUNIOR:
		{
			for( int i = 0 ; i < GUILDAUTH_TYPE_CNT; ++i )
				m_pCheckOption[i]->Enable( false );
			m_pComboTakeItem->Enable( false );
			m_pComboWithDraw->Enable( false );
		}
		break;
	}
}

void CDnGuildAuthDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			Apply();

			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100060) );

			return;
		}
		else if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED && bTriggeredByUser )
	{
		if( IsCmdControl("ID_COMBOBOX0") )
		{
			if( m_dwPrevComboGradeIndex != m_pComboGrade->GetSelectedIndex() )
			{
				if( m_bShowChangeMsg )
				{
					if( IsChanged() )
					{
						m_bShowChangeMsg = false;
						m_pComboGrade->SetSelectedByIndex( m_dwPrevComboGradeIndex );
						GetInterface().MessageBox( 3170 );
						return;
					}
				}
				else
				{
					m_bShowChangeMsg = true;
				}
				m_dwPrevComboGradeIndex = m_pComboGrade->GetSelectedIndex();
				RefreshAuth();
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildAuthDlg::Apply()
{
	if( !CDnGuildTask::IsActive() ) return;
	TGuild *pGuild = GetGuildTask().GetGuildInfo();
	if( !pGuild->IsSet() ) return;

	// 창이 열렸을때의 값과 같다면 아무 요청하지 않고, 달라졌다면 서버에 변경요청을 보낸다.
	int nCurRole = -1;
	m_pComboGrade->GetSelectedValue( nCurRole );
	if( nCurRole == -1 ) return;

	bool bChanged = false;
	int iInt2 = 0;	// 권한 비트플래그 저장.
	int iInt3 = 0;  // 꺼내기 횟수
	int iInt4 = 0;  // 출금액수 

	// 권한 설정 변화체크
	for( int i = 0; i < GUILDAUTH_TYPE_CNT; ++i ) 
	{
		if( GetGuildTask().GetAuth( (eGuildRoleType)nCurRole, static_cast<eGuildAuthType>(i) ) != m_pCheckOption[i]->IsChecked() ) 
		{
			bChanged = true;
			break;
		}
	}

	// 꺼내기 횟수 및 출금액수 설정 체크 (처리부탁 드릴게요)
	if( !bChanged )
	{
		int nSelectedValue = 0;
		m_pComboTakeItem->GetSelectedValue( nSelectedValue );
		if( GetGuildTask().GetMaxTakeItemByRole( (eGuildRoleType)nCurRole ) != nSelectedValue )
			bChanged = true;

		m_pComboWithDraw->GetSelectedValue( nSelectedValue );
		if( GetGuildTask().GetMaxWithDrawByRole( (eGuildRoleType)nCurRole ) != nSelectedValue )
			bChanged = true;
	}

	// 권한체크가 바뀌었든, 출금, 꺼내기 횟수가 바뀌었든 하나라도 바뀌었으면 Auth변경패킷을 보낸다.
	if( bChanged )
	{
		for( int i = 0; i < GUILDAUTH_TYPE_CNT; ++i ) 
		{
			if( m_pCheckOption[i]->IsChecked() )
				iInt2 |= (static_cast<int>(1) << i);
		}

		// 꺼내기 횟수 및 출금액수 설정
		m_pComboTakeItem->GetSelectedValue( iInt3 );
		m_pComboWithDraw->GetSelectedValue( iInt4 );

		GetGuildTask().RequestChangeGuildInfoEx( GUILDUPDATE_TYPE_ROLEAUTH, nCurRole, iInt2, iInt3, iInt4 );		
	}
}

bool CDnGuildAuthDlg::IsChanged()
{
	bool bChanged = false;
	if( !CDnGuildTask::IsActive() ) return false;
	TGuild *pGuild = GetGuildTask().GetGuildInfo();
	if( !pGuild->IsSet() ) return false;

	// 콤보박스 아이템이 변경되었을때 호출되는 것이므로 예전에 선택해둔 것으로 CurRole을 구해야한다.
	int nCurRole = -1;
	m_pComboGrade->GetItemValue( m_dwPrevComboGradeIndex, nCurRole );
	if( nCurRole == -1 ) return false;

	for( int i = 0; i < GUILDAUTH_TYPE_CNT; ++i ) {
		if( GetGuildTask().GetAuth( (eGuildRoleType)nCurRole, static_cast<eGuildAuthType>(i) ) != m_pCheckOption[i]->IsChecked() ) {
			bChanged = true;
			break;
		}
	}

	int nSelectedValue = 0;
	m_pComboTakeItem->GetSelectedValue( nSelectedValue );
	if( GetGuildTask().GetMaxTakeItemByRole( (eGuildRoleType)nCurRole ) != nSelectedValue )
		bChanged = true;
	m_pComboWithDraw->GetSelectedValue( nSelectedValue );
	if( GetGuildTask().GetMaxWithDrawByRole( (eGuildRoleType)nCurRole ) != nSelectedValue )
		bChanged = true;

	return bChanged;
}
