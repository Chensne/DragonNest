#include "StdAfx.h"
#include "DnDwcInviteReqDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDwcInviteReqDlg::CDnDwcInviteReqDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pProgressBarTime(NULL)
, m_pTitleName(NULL)
, m_pCharacterName(NULL)
, m_pButtonOK(NULL)
, m_pButtonCancel(NULL)
, m_fTotalTime(0.0f)
, m_fElapsedTime(0.0f)
{
}

CDnDwcInviteReqDlg::~CDnDwcInviteReqDlg(void)
{
}

void CDnDwcInviteReqDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AcceptProcessBarRequestDlg.ui" ).c_str(), bShow );
}

void CDnDwcInviteReqDlg::InitialUpdate()
{
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
	m_pTitleName = GetControl<CEtUIStatic>("ID_TEXT_TITLE_NAME");
	m_pCharacterName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");

	CEtUIStatic* pStatic = NULL;
	pStatic = GetControl<CEtUIStatic>("ID_TITLE");
	if(pStatic) pStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120226)); // mid: �����ʴ�

	pStatic = GetControl<CEtUIStatic>("ID_TEXT1");
	if(pStatic) pStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120230)); // mid: ���� �����Ͻðڽ��ϱ�? 
}

void CDnDwcInviteReqDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	if( m_fElapsedTime <= 0.0f )
	{
		CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonCancel, 0 );
		Show(false);
	}
	else
	{
		// AcceptRequest���̾�α׿� �޸� ��øó������ ������ ���⶧���� ������ �Ϸ����尻���� �Ѵ�.
		m_fElapsedTime -= fElapsedTime;

		if( m_fTotalTime != 0.0f)
		{
			m_pProgressBarTime->SetProgress( m_fElapsedTime / m_fTotalTime * 100.0f );
		}
		else
		{
			m_pProgressBarTime->SetProgress( 0.0f );
		}
	}
}

void CDnDwcInviteReqDlg::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if( !bShow )
	{
		SetDialogID(-1);
		SetCallback(NULL);
		m_fTotalTime = 0.0f;
		m_fElapsedTime = 0.0f;
	}
	else
	{
		//m_pProgressBarTime->SetProgress(100.0f);
	}

	CEtUIDialog::Show( bShow );
}

void CDnDwcInviteReqDlg::SetInfo( LPCWSTR pwszGuildName, LPCWSTR wszCharacterName, float fTotalTime, int nID, CEtUICallback *pCall )
{
	m_nDialogID = nID;
	m_pCallback = pCall;
	m_fTotalTime = fTotalTime;
	m_fElapsedTime = fTotalTime;

	m_pCharacterName->SetText( wszCharacterName );

	m_pTitleName->SetText( pwszGuildName );
	m_pButtonOK->Show( true );
	m_pButtonCancel->Show( true );
}

void CDnDwcInviteReqDlg::SetElapsedTime( float fElapsedTime )
{
	fElapsedTime = min(fElapsedTime, m_fTotalTime);
	fElapsedTime = max(fElapsedTime, 0.0f);
	m_fElapsedTime = fElapsedTime;
}

void CDnDwcInviteReqDlg::SendReject()
{
	if( m_pButtonCancel )
		CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonCancel, 0 );
}