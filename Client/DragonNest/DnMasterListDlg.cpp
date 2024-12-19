#include "Stdafx.h"
#include "DnMasterListDlg.h"
#include "DnCommunityDlg.h"
#include "DnMasterJoinDlg.h"
#include "DnInterfaceString.h"
#include "DnTableDB.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#endif

const float BUTTON_REFRESH_TIME = 2.0f;

CDnMasterListDlg::CDnMasterListDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pMasterJoinDlg( NULL )
, m_pStaticSelect( NULL )
, m_pStaticPage( NULL )
, m_nCurSelect( -1 )
, m_bRefresh( false )
, m_fRefreshDelta( 0.0f )
, m_nCurPage( 0 )
, m_nPrevPage( 0 )
, m_cJob( 0 )
, m_cGender( 0 )
, m_bSearchButton( false )
, m_bRenewButton( false )
, m_bArrowButton( false )
, m_fSearchDelta( 0.0f )
, m_fRenewDelta( 0.0f )
, m_fArrowDelta( 0.0f )
{
	m_vJobID.push_back(0);	// 제한 없음

	DNTableFileFormat* pItemSox = GetDNTable( CDnTableDB::TJOB );
	if( NULL != pItemSox )
	{
		int nJobCount = pItemSox->GetItemCount();
		for( int itr = 0; itr < nJobCount; ++itr )
		{
			int nItemID = pItemSox->GetItemID( itr );

			int nPageType = pItemSox->GetFieldFromLablePtr( nItemID, "_Service" )->GetInteger();
			int nJobNumber = pItemSox->GetFieldFromLablePtr( nItemID, "_JobNumber" )->GetInteger();

			if( 0 == nPageType || eJOB_First == nJobNumber )
				continue;
			
			m_vJobID.push_back( nItemID );
		}
	}
}

CDnMasterListDlg::~CDnMasterListDlg()
{
	SAFE_DELETE( m_pMasterJoinDlg );
}

void CDnMasterListDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MasterListDlg.ui" ).c_str(), bShow );
}

void CDnMasterListDlg::InitialUpdate()
{
	m_pMasterJoinDlg = new CDnMasterJoinDlg(UI_TYPE_CHILD_MODAL, this);
	m_pMasterJoinDlg->Initialize( false );

	m_pStaticSelect = GetControl<CEtUIStatic>("ID_SELECT_BAR");
	m_pStaticPage = GetControl<CEtUIStatic>("ID_PAGE");

	CONTROL( Button, ID_BUTTON_JOINMASTER )->Enable( false );
	CONTROL( Button, ID_BUTTON_INFO )->Enable( true );

	CONTROL( ComboBox, ID_JOB )->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3599), &m_vJobID[0], 0 );	//제한없음
	for( int itr = 1; itr < (int)m_vJobID.size(); ++itr )
		CONTROL( ComboBox, ID_JOB )->AddItem( DN_INTERFACE::STRING::GetJobString( m_vJobID[itr] ), &m_vJobID[itr], 0 );

#if !defined(PRE_MOD_MASTER_GENDER)
	CONTROL( ComboBox, ID_SEX )->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3599), NULL, 0 );
	CONTROL( ComboBox, ID_SEX )->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7203), NULL, 0 );
	CONTROL( ComboBox, ID_SEX )->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7204), NULL, 1 );
#endif	// #if !defined(PRE_MOD_MASTER_GENDER)

	char szControlName[32];
	for( int i = 0; i < MasterSystem::Max::MasterPageRowCount; ++i )
	{
		sprintf_s(szControlName, _countof(szControlName), "ID_TITLE%d", i);
		m_pMasterInfo[i].m_pStaticTitle = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_LEVEL%d", i);
		m_pMasterInfo[i].m_pStaticLevel = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_CLASS%d", i);
		m_pMasterInfo[i].m_pStaticClass = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_SEX%d", i);
		m_pMasterInfo[i].m_pStaticGender = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_NAME%d", i);
		m_pMasterInfo[i].m_pStaticName = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_NAME%d", i);
		m_pMasterInfo[i].m_pStaticName = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_RESPECT%d", i);
		m_pMasterInfo[i].m_pStaticRespect = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_BASE_0%d", i);
		m_pMasterInfo[i].m_pStaticBase = GetControl<CEtUIStatic>(szControlName);

		m_pMasterInfo[i].Clear();
	}

	m_pStaticSelect->Show(false);	
	UpdateSelectPage();
}

void CDnMasterListDlg::Show(bool bShow)
{
	if( m_bShow == bShow )	return;

	m_bRefresh = bShow;
	if( !bShow )
	{
		ShowChildDialog( m_pMasterJoinDlg, false );
		m_nCurSelect = -1;
	}
	else
	{
		if( -1 == m_nCurSelect )
			CONTROL( Button, ID_BUTTON_JOINMASTER )->Enable( false );
		
		m_fRefreshDelta = 0.0f;
	}

	CEtUIDialog::Show( bShow );
}

void CDnMasterListDlg::Process(float fElapsedTime)
{
	if( m_bRefresh )
	{
		m_fRefreshDelta -= fElapsedTime;

		if( m_fRefreshDelta <= 0.f ) 
		{
			GetMasterTask().RequestMasterList( m_nCurPage, m_cJob, m_cGender );
			m_fRefreshDelta = MasterSystem::Common::RefreshCacheTimeSec;
		}
	}

	if( m_bSearchButton )
	{
		m_fSearchDelta -= fElapsedTime;

		if( m_fSearchDelta < 0.0f )
		{
			CONTROL( Button, ID_BUTTON_SEARCH)->Enable( true );
			m_bSearchButton = false;
		}
	}
	if( m_bRenewButton )
	{
		m_fRenewDelta -= fElapsedTime;

		if( m_fRenewDelta < 0.0f )
		{
			CONTROL( Button, ID_BUTTON_REFRESH)->Enable( true );
			m_bRenewButton = false;
		}
	}
	if( m_bArrowButton )
	{
		m_fArrowDelta -= fElapsedTime;

		if( m_fArrowDelta < 0.0f )
		{
			CONTROL( Button, ID_PRIOR)->Enable( true );
			CONTROL( Button, ID_NEXT)->Enable( true );
			m_bArrowButton = false;
		}
	}

	CEtUIDialog::Process( fElapsedTime );
}

void CDnMasterListDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_INFO") )
		{
			Show( false );
			((CDnCommunityDlg *)m_pParentDialog)->ChangeMasterDialog( CDnCommunityDlg::typeMasterInfo );
		}
		else if( IsCmdControl("ID_BUTTON_SEARCH") )
		{
			m_bSearchButton = true;
			m_fRefreshDelta = 0.0f;
			m_fSearchDelta = BUTTON_REFRESH_TIME;
			m_nCurPage = 0;
			m_nPrevPage = 0;

			CONTROL( Button, ID_BUTTON_SEARCH)->Enable( false );
		}
		else if( IsCmdControl("ID_BUTTON_REFRESH") )
		{
			m_bRenewButton = true;
			m_fRefreshDelta = 0.0f;
			m_fRenewDelta = BUTTON_REFRESH_TIME;
			m_nCurPage = 0;
			m_nPrevPage = 0;

			CONTROL( Button, ID_BUTTON_REFRESH)->Enable( false );
		}
		else if( IsCmdControl("ID_BUTTON_JOINMASTER") )
		{
			if( -1 != m_nCurSelect )
			{
				const CDnMasterTask::SMasterList & MasterList = GetMasterTask().GetMasterList();

				GetMasterTask().RequestMasterApplication( MasterList.MasterInfoList[m_nCurSelect].biCharacterDBID );
			}
		}
		else if( IsCmdControl("ID_PRIOR") )
		{
			if( m_nCurPage < 1 )
				return;

			CONTROL( Button, ID_PRIOR)->Enable( false );
			CONTROL( Button, ID_NEXT)->Enable( false );
			m_bArrowButton = true;
			m_fRefreshDelta = 0.0f;
			m_fArrowDelta = BUTTON_REFRESH_TIME;

			m_nPrevPage = m_nCurPage--;
			m_fRefreshDelta = 0.0f;
		}
		else if( IsCmdControl("ID_NEXT") )
		{
			CONTROL( Button, ID_PRIOR)->Enable( false );
			CONTROL( Button, ID_NEXT)->Enable( false );
			m_bArrowButton = true;
			m_fRefreshDelta = 0.0f;
			m_fArrowDelta = BUTTON_REFRESH_TIME;

			m_nPrevPage = m_nCurPage++;
			m_fRefreshDelta = 0.0f;
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_STATIC_HELPBASE") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_MASTER);
		}
#endif
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		if( IsCmdControl("ID_JOB") )
		{
			m_cJob = *((BYTE *)CONTROL( ComboBox, ID_JOB )->GetSelectedData());
		}
#if !defined(PRE_MOD_MASTER_GENDER)
		else if( IsCmdControl("ID_SEX") )
		{
			m_cGender = (BYTE)CONTROL( ComboBox, ID_SEX )->GetSelectedIndex();
		}
#endif	// #if !defined(PRE_MOD_MASTER_GENDER)
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

bool CDnMasterListDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	bool bDoubleClick = false;

	switch( uMsg )
	{
	case WM_LBUTTONDBLCLK:
		if( !IsMouseInDlg() ) break;
		bDoubleClick = true;
	case WM_LBUTTONDOWN:
		{
			if (CONTROL( ComboBox, ID_JOB )->IsOpenedDropDownBox() 
#if !defined(PRE_MOD_MASTER_GENDER)
				|| CONTROL( ComboBox, ID_SEX )->IsOpenedDropDownBox()
#endif	// #if !defined(PRE_MOD_MASTER_GENDER)
				)
				break;

			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			int nFindSelect = FindInsideItem( fMouseX, fMouseY );
			if( nFindSelect != -1 )
			{
				if( m_pMasterInfo[nFindSelect].m_pStaticBase->IsShow() )
				{
					m_nCurSelect = nFindSelect;
					UpdateSelectBar();
					CONTROL( Button, ID_BUTTON_JOINMASTER )->Enable( true );

					if( bDoubleClick )
					{
						const CDnMasterTask::SMasterList & MasterList = GetMasterTask().GetMasterList();

						GetMasterTask().RequestMasterApplication( MasterList.MasterInfoList[m_nCurSelect].biCharacterDBID );
					}

					return true;
				}
				else
				{
					CONTROL( Button, ID_BUTTON_JOINMASTER )->Enable( false );
					m_pStaticSelect->Show(false);
					m_nCurSelect = -1;
				}
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMasterListDlg::RefreshList()
{
	const CDnMasterTask::SMasterList & MasterList = GetMasterTask().GetMasterList();

	if( 0 == MasterList.cCount && m_bArrowButton)
	{
		m_nCurPage = m_nPrevPage;
		return;
	}

	ResetList();

	WCHAR szString[256];
	for( BYTE itr = 0; itr < MasterList.cCount; ++itr )
	{
		m_pMasterInfo[itr].m_pStaticTitle->SetText( MasterList.MasterInfoList[itr].wszSelfIntroduction );
		
		m_pMasterInfo[itr].m_pStaticName->SetText( MasterList.MasterInfoList[itr].wszCharName );

		m_pMasterInfo[itr].m_pStaticClass->SetText( DN_INTERFACE::STRING::GetJobString( MasterList.MasterInfoList[itr].cJobCode ) );

		wsprintf( szString, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), (int)MasterList.MasterInfoList[itr].cLevel );
		m_pMasterInfo[itr].m_pStaticLevel->SetText( szString );

		wsprintf( szString, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7205 ), MasterList.MasterInfoList[itr].iRespectPoint );
		m_pMasterInfo[itr].m_pStaticRespect->SetText( szString );

#if !defined(PRE_MOD_MASTER_GENDER)
		if( 1 == MasterList.MasterInfoList[itr].cGenderCode )
		{
			m_pMasterInfo[itr].m_pStaticGender->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7203 ) );
			m_pMasterInfo[itr].m_pStaticGender->Show( true );
		}
		else if( 2 == MasterList.MasterInfoList[itr].cGenderCode )
		{
			m_pMasterInfo[itr].m_pStaticGender->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7204 ) );
			m_pMasterInfo[itr].m_pStaticGender->Show( true );
		}
		else
			m_pMasterInfo[itr].m_pStaticGender->Show( false );
#endif	// #if !defined(PRE_MOD_MASTER_GENDER)

		m_pMasterInfo[itr].m_pStaticBase->Show( true );
	}

	UpdateSelectPage();
}

void CDnMasterListDlg::UpdateSelectBar()
{
	SUICoord uiCoordBase, uiCoordSelect;
	m_pMasterInfo[m_nCurSelect].m_pStaticBase->GetUICoord(uiCoordBase);
	m_pStaticSelect->GetUICoord(uiCoordSelect);
	uiCoordSelect.fX = uiCoordBase.fX - 0.0039f;
	uiCoordSelect.fY = uiCoordBase.fY;
	m_pStaticSelect->SetUICoord(uiCoordSelect);
	m_pStaticSelect->Show(true);
}

int CDnMasterListDlg::FindInsideItem( float fX, float fY )
{
	for( int i = 0; i < MasterSystem::Max::MasterPageRowCount; i++ )
	{
		if( m_pMasterInfo[i].IsInsideItem( fX, fY ) )
		{
			return i;
		}
	}

	return -1;
}

void CDnMasterListDlg::ResetList()
{
	for( int i = 0; i < MasterSystem::Max::MasterPageRowCount; ++i )
		m_pMasterInfo[i].Clear();

	m_pStaticSelect->Show(false);
}

void CDnMasterListDlg::UpdateSelectPage()
{
	WCHAR szString[256];

	wsprintf( szString, L"%d", m_nCurPage + 1 );
	m_pStaticPage->SetText( szString );
}

void CDnMasterListDlg::RefreshSimple()
{
}

void CDnMasterListDlg::RefreshMaster()
{
	m_pMasterJoinDlg->RefreshInfo( m_nCurSelect );

	ShowChildDialog( m_pMasterJoinDlg, true );
}
