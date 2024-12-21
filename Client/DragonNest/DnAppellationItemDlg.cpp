#include "StdAfx.h"
#include "DnAppellationItemDlg.h"
#include "DnAppellationDlg.h"
#include "DnAppellationTask.h"
#include "SyncTimer.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnAppellationItemDlg::CDnAppellationItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pCheckBox(NULL)
, m_pParent(NULL)
, m_pInfo(NULL)
, m_pStaticNew(NULL)
, m_pCoverCheckBox(NULL)
{
	m_pStaticName = NULL;
}

CDnAppellationItemDlg::~CDnAppellationItemDlg(void)
{
}

void CDnAppellationItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Titlename_Bar_Renew.ui" ).c_str(), bShow );
}

void CDnAppellationItemDlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>( "ID_TITLE_NAME" );

	m_pCoverCheckBox = GetControl<CEtUICheckBox>( "ID_CHECKBOX0" );
	m_pCheckBox = GetControl<CEtUICheckBox>( "ID_CHECKBOX1" );
	m_pStaticNew =	GetControl<CEtUIStatic>( "ID_STATIC_NEW" );
	m_pStaticNew->Show(false);

	m_pStaticName->Show(false);
}

void CDnAppellationItemDlg::SetInfo( CDnAppellationTask::AppellationStruct *pInfo, CDnAppellationDlg *pDlg )
{
	m_pParent = pDlg;
	m_pInfo = pInfo;

	m_pStaticName->Show( true );	
	m_pStaticName->SetText( pInfo->szName );
	m_pStaticName->SetTextColor(pInfo->ColorValue);
	
	DWORD dwShadowColor = 0xFF000000;
	if (pInfo->ColorValue != 0xFFFFFFFF)
		dwShadowColor = 0xFF1E0038;
	m_pStaticName->SetShadowColor(dwShadowColor);

	m_pCheckBox->SetChecked( false, false );
	m_pCoverCheckBox->SetChecked( false, false );

	if( pInfo->bNew )
	{
		m_pStaticNew->Show(true);
		m_pStaticName->GetFontCoord()->fX += 0.044f;
	}
}

void CDnAppellationItemDlg::SetChecked( bool bCheck )
{
	m_pCheckBox->SetChecked( bCheck, false );
}

void CDnAppellationItemDlg::SetCoverChecked( bool bCheck )
{
	m_pCoverCheckBox->SetChecked( bCheck, false );
}

void CDnAppellationItemDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl( "ID_CHECKBOX0" ) )			// Ä¿¹öÄªÈ£
		{
			if( m_pCoverCheckBox->IsChecked() )
				m_pParent->SetSelectAppellationName( CDnAppellationDlg::eAppellationType::Appellation_Cover, m_pInfo->nArrayIndex );
			else
				m_pParent->SetSelectAppellationName( CDnAppellationDlg::eAppellationType::Appellation_Cover, -1 );
		}
		else if( IsCmdControl( "ID_CHECKBOX1" ) )		// Àû¿ëÄªÈ£
		{
			if( m_pCheckBox->IsChecked() )
				m_pParent->SetSelectAppellationName( CDnAppellationDlg::eAppellationType::Appellation_Apply, m_pInfo->nArrayIndex );
			else
				m_pParent->SetSelectAppellationName( CDnAppellationDlg::eAppellationType::Appellation_Apply, -1 );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnAppellationItemDlg::IsOneDayExpire()
{
	__time64_t pNowTime = CSyncTimer::GetInstance().GetCurTime();
	__time64_t deltaTime = m_pInfo->tExpireDate - pNowTime;

	if( deltaTime < EXPIRE_TIME_COLOR )
		return true;

	return false;
}