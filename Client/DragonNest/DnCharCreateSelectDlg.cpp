#include "StdAfx.h"
#include "DnCharCreateSelectDlg.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnTableDB.h"
#include "EtUIRadioButton.h"
#include "EtUIStatic.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#ifdef PRE_MOD_SELECT_CHAR

// ��ĳ �߰� �� ���� ����!

CDnCharCreateSelectDlg::CDnCharCreateSelectDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_nFocusCharIndex( -1 )
{
	memset( m_pStaticJobName, NULL, sizeof(CEtUIStatic*) * CLASSKINDMAX );
#ifdef _ADD_NEWLOGINUI
	memset( m_pRadioButton, NULL, sizeof(CEtUIRadioButton*) * 20 );
#else
	memset( m_pRadioButton, NULL, sizeof(CEtUIRadioButton*) * CLASSKINDMAX );
#endif

}

CDnCharCreateSelectDlg::~CDnCharCreateSelectDlg(void)
{
	m_mapTabJobIndex.clear();
}

void CDnCharCreateSelectDlg::Initialize( bool bShow )
{
#ifdef _ADD_NEWLOGINUI
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("charcreateclassdlg.ui").c_str(), bShow);
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreate_CharSelectDlg.ui" ).c_str(), bShow );
#endif
}

void CDnCharCreateSelectDlg::Show( bool bShow )
{
	if( bShow == m_bShow )
		return;

	CEtUIDialog::Show(bShow);
}

void CDnCharCreateSelectDlg::InitialUpdate()
{
#ifdef _ADD_NEWLOGINUI
	std::map<int,int> vecEnabledClass;//enabled + new banner
	bool bLoadSettings = true;
	char szControlName[32];
	for( int i=0; i<CLASSKINDMAX; ++i )
	{
		sprintf_s( szControlName, _countof(szControlName), "ID_STATIC_CLASS%d", i );
		m_pStaticJobName[i] = GetControl<CEtUIStatic>( szControlName );
	}
	
	int i;
	//new login
	for (i = 0; i < 10; i++)
	{
		GetControl<CEtUIStatic>(FormatA("ID_STATIC_DUMMT%d", i).c_str())->Show(false);
		GetControl<CEtUIStatic>(FormatA("ID_STATIC_NEW%d", i).c_str())->Show(false);
	}

	//get data from table
	DNTableFileFormat *pSox = GetDNTable(CDnTableDB::TCHARCREATE);
	if (!pSox) { 
		bLoadSettings = false; 
		OutputDebug("[%s] pSox == NULL", __FUNCTION__); 
	}

	int dwNewBannerCount = 0;
	float LastPos = 0.0f;
	//CEtUIRadioButton*	m_pClassButton;
	CEtUIStatic* m_pNewBanner;
	
	for (i = 0; i < pSox->GetItemCount(); i++)
	{
		if (pSox->GetFieldFromLablePtr(i, "_CanCreate") == NULL)
			continue;

		int dwCanCreate = pSox->GetFieldFromLablePtr(i, "_CanCreate")->GetInteger();
		int dwNewBanner = pSox->GetFieldFromLablePtr(i, "_NewBanner")->GetInteger();

		if (dwCanCreate == 1)
		{
			m_pRadioButton[i] = GetControl<CEtUIRadioButton>(FormatA("ID_RBT_%d", i - 1).c_str());
			//SUICoord CurrentCoord;
			//CurrentCoord = m_pClassButton->GetUICoord();

			m_pRadioButton[i]->SetPosition(0.015f, LastPos);

			if (dwNewBanner)
			{
				m_pNewBanner = GetControl<CEtUIStatic>(FormatA("ID_STATIC_NEW%d", dwNewBannerCount).c_str());
				m_pNewBanner->SetPosition(0.f, LastPos + 0.015f);//+0.015 pe axa Y?
				m_pNewBanner->Show(true);
				dwNewBannerCount += 1;
			}

			LastPos += 0.07f;//+0.07 == 50px

			m_pRadioButton[i]->Show(true);
		}
	}


	//hide for later !
	GetControl<CEtUIStatic>("ID_STATIC0")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC1")->Show(false);
	GetControl<CEtUIButton>("ID_BT_NEXT")->Show(false);
	GetControl<CEtUIButton>("ID_BT_BACK")->Show(false);
	for ( i = 0; i < 5; i++)
	{
		GetControl<CEtUIStatic>(FormatA("ID_BUTTON_PAGE%d",i).c_str())->Show(false);	
	}

#else
    //TODO(Cussrro): �޸�������ɫ����
    char szControlName[32];
    for (int i = 0; i < CLASSKINDMAX; ++i)
    {
        sprintf_s(szControlName, _countof(szControlName), "ID_STATIC_CLASS%d", i);
        m_pStaticJobName[i] = GetControl<CEtUIStatic>(szControlName);
    }
#endif
	//
	/*float LastPos = 0.0f;
	CEtUIRadioButton*	m_pClassButton;
	for (std::vector<int>::iterator it = vecEnabledClass.begin(); it != vecEnabledClass.end(); ++it)
	{
		m_pClassButton = GetControl<CEtUIRadioButton>(FormatA("ID_RBT_%d", *it-1).c_str());
		SUICoord SUICoordtest;
		SUICoordtest = m_pClassButton->GetUICoord();

		m_pClassButton->SetPosition(SUICoordtest.fX, LastPos);
		LastPos += 0.07f;//+0.07 ?
		
		m_pClassButton->Show(true);

	
		OutputDebug("[%s] Valid Class :: %d  x=%.5f ; y=%.5f", __FUNCTION__, *it,SUICoordtest.fX,SUICoordtest.fY);
	//	m_pClassButton->SetPosition(10.f, LastPos + 50.f);
	}
	*/


	

	//rlkt_newlogin
	/*for (i = 0; i < 12; i++)
	{
		CEtUIRadioButton*	m_pClassButton;
		m_pClassButton = GetControl<CEtUIRadioButton>(FormatA("ID_RBT_%d", i).c_str());
		m_pClassButton->
		
	} */


	//rlkt_newlogin
#ifndef _ADD_NEWLOGINUI
	m_pRadioButton[0] = GetControl<CEtUIRadioButton>( "ID_RBT_WARRIOR" );
	m_pRadioButton[1] = GetControl<CEtUIRadioButton>( "ID_RBT_ARCHER" );
	m_pRadioButton[2] = GetControl<CEtUIRadioButton>( "ID_RBT_SOCERESS" );
	m_pRadioButton[3] = GetControl<CEtUIRadioButton>( "ID_RBT_CLERIC" );
	m_pRadioButton[4] = GetControl<CEtUIRadioButton>( "ID_RBT_ACADEMIC" );
	m_pRadioButton[5] = GetControl<CEtUIRadioButton>( "ID_RBT_KALI" );
#if defined( PRE_ADD_ASSASSIN )
	m_pRadioButton[6] = GetControl<CEtUIRadioButton>( "ID_RBT_ASSASSIN" );
#endif
#if defined( PRE_ADD_LENCEA )
	m_pRadioButton[7] = GetControl<CEtUIRadioButton>( "ID_RBT_ASTA" );
#endif
#if defined( PRE_ADD_MACHINA )
	m_pRadioButton[8] = GetControl<CEtUIRadioButton>( "ID_RBT_MA" );
#endif
	//#if defined( PRE_ADD_ASSASSIN )

	m_mapTabJobIndex.insert( make_pair( 5, 0 ) );
	m_mapTabJobIndex.insert( make_pair( 2, 1 ) );
	m_mapTabJobIndex.insert( make_pair( 0, 2 ) );
	m_mapTabJobIndex.insert( make_pair( 1, 3 ) );
	m_mapTabJobIndex.insert( make_pair( 3, 4 ) );
	m_mapTabJobIndex.insert( make_pair( 4, 5 ) );
#if defined( PRE_ADD_ASSASSIN )
	m_mapTabJobIndex.insert( make_pair( 6, 6 ) );
#endif	// #if defined( PRE_ADD_ASSASSIN )
#if defined( PRE_ADD_LENCEA )
	m_mapTabJobIndex.insert( make_pair( 7, 7 ) );
#endif	
#if defined( PRE_ADD_MACHINA )
	m_mapTabJobIndex.insert( make_pair( 8, 8 ) );
#endif
#pragma message("  ----> CHECK DARK AVENGER")
#endif
}

void CDnCharCreateSelectDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pLoginTask ) return;

	if( EVENT_RADIOBUTTON_CHANGED == nCommand )
	{
		if( strstr( pControl->GetControlName(), "ID_RBT" ) )
		{
			int nSelectIndex = static_cast<CEtUIRadioButton*>( pControl )->GetTabID();
#ifndef _ADD_NEWLOGINUI
			if (nSelectIndex == 8) nSelectIndex = 9; //rlkt_dark!
#endif
			pLoginTask->SelectMoveCharacter( nSelectIndex );
		}
	}
}

void CDnCharCreateSelectDlg::SetCharCreateSelect( int nClassIndex )
{
	if( nClassIndex < 0 || nClassIndex >= CLASSKINDMAX )
		return;

	if( m_pRadioButton[nClassIndex] )
		m_pRadioButton[nClassIndex]->SetChecked( true );
}

void CDnCharCreateSelectDlg::ResetButton()
{
	for( int i=0; i<CLASSKINDMAX; ++i )
	{
		if (m_pRadioButton[i])
			m_pRadioButton[i]->SetChecked( false );
	}
}

bool CDnCharCreateSelectDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;
#ifndef _ADD_NEWLOGINUI
	if( uMsg == WM_MOUSEMOVE )
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		PointToFloat( MousePoint, fMouseX, fMouseY );

		int nFocusIndex = -1;
		for( int i=0; i<CLASSKINDMAX; ++i )
		{
			if( m_pRadioButton[i]->IsChecked() )
			{
				nFocusIndex = i;
			}
			if( m_pRadioButton[i]->IsInside( fMouseX, fMouseY ) )
			{
				nFocusIndex = i;
				break;
			}
		}

		if( m_nFocusCharIndex > -1 )
			nFocusIndex = m_nFocusCharIndex;

		for( int i=0; i<CLASSKINDMAX; ++i )
		{
			m_pStaticJobName[i]->Show( false );
		}

		if( nFocusIndex > -1 )
		{
			std::map<int, int>::iterator iter = m_mapTabJobIndex.find( nFocusIndex );
			if( iter != m_mapTabJobIndex.end() )
			{
				int nSelectIndex = iter->second;

				if (nSelectIndex == 8 || nSelectIndex == 9)//rlkt_dark!
					m_pStaticJobName[8]->Show(true);

				if( nSelectIndex >= 0 && nSelectIndex < CLASSKINDMAX )
					m_pStaticJobName[nSelectIndex]->Show( true );
			}
		}
	}
#endif

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}


#endif // PRE_MOD_SELECT_CHAR