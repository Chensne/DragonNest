#include "StdAfx.h"
#include "DnGameMacroMsgOptDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameMacroMsgOptDlg::CDnGameMacroMsgOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: BaseClass( dialogType, pParentDialog, nID, pCallback )
{
	m_wszTemp[0] = '\0';
	memset( m_pEditBox, 0, sizeof( m_pEditBox ) );
	memset( m_nDefaultMacroUIStringID, 0, sizeof( m_nDefaultMacroUIStringID ) );
}

CDnGameMacroMsgOptDlg::~CDnGameMacroMsgOptDlg()
{
}

void CDnGameMacroMsgOptDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameMacroMsgOptDlg.ui" ).c_str(), bShow );
	GetInterface().SetGameMacroMsgOptDlg( this );
}

void CDnGameMacroMsgOptDlg::InitialUpdate()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TRADIOMSG );

	for( int i = 0; i < CGameOption::NUM_COMM_MACROS; i++ ) {
		char szIDString[255]={0,};
		sprintf(szIDString, "ID_OP_MACRO_BAR%d", i);
		m_pEditBox[ i ] = GetControl<CEtUIIMEEditBox>( szIDString );

		int nID = pSox->GetItemIDFromField( "_MacroNum", i+1 );
		if( nID > 0 ) m_nDefaultMacroUIStringID[i] = pSox->GetFieldFromLablePtr( nID, "_MacroDef" )->GetInteger();
	}
}

bool CDnGameMacroMsgOptDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			ImportSetting();			
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if( pMainMenuDlg ) {
				pMainMenuDlg->ShowSystemDialog( true );
			}
			return true;
		}
		else if (wParam == VK_RETURN)
		{
			ExportSetting();
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if( pMainMenuDlg ) {
				pMainMenuDlg->ShowSystemDialog( true );
			}
			return true;
		}
	}

	return BaseClass::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnGameMacroMsgOptDlg::ExportSetting()
{
	for( int i = 0; i < CGameOption::NUM_COMM_MACROS; i++)
	{
		if( wcscmp(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_nDefaultMacroUIStringID[i] ), m_pEditBox[i]->GetText()) == 0 )
		{
			CGameOption::GetInstance().m_bDefaultMacro[i] = true;
			swprintf_s( CGameOption::GetInstance().m_wszMacro[i], L"" );
		}
		else
		{
			CGameOption::GetInstance().m_bDefaultMacro[i] = false;
			_wcscpy( CGameOption::GetInstance().m_wszMacro[i], _countof(CGameOption::GetInstance().m_wszMacro[i]), m_pEditBox[i]->GetText(), (int)wcslen(m_pEditBox[i]->GetText()) );
		}
	}
}

void CDnGameMacroMsgOptDlg::ImportSetting()
{
	for( int i = 0; i < CGameOption::NUM_COMM_MACROS; i++)
	{
		if( CGameOption::GetInstance().m_bDefaultMacro[i] )
		{
			// 디폴트 매크로 번호 얻어와
			m_pEditBox[i]->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_nDefaultMacroUIStringID[i] ) );
		}
		else
		{
			WCHAR wszTemp[MAX_PATH] = {0,};
			_wcscpy( wszTemp, _countof(wszTemp), CGameOption::GetInstance().m_wszMacro[i], (int)wcslen(CGameOption::GetInstance().m_wszMacro[i]) );
			m_pEditBox[i]->SetText( wszTemp );
		}
	}
}

bool CDnGameMacroMsgOptDlg::IsChanged()
{
	for( int i = 0; i < CGameOption::NUM_COMM_MACROS; i++) {
		if( CGameOption::GetInstance().m_bDefaultMacro[i] == true ) {
			if( wcscmp(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_nDefaultMacroUIStringID[i] ), m_pEditBox[i]->GetText()) != 0 )
				return true;
		}
		else {
			if( wcscmp(CGameOption::GetInstance().m_wszMacro[i], m_pEditBox[i]->GetText()) != 0 )
				return true;
		}
	}

	return false;
}

LPCWSTR CDnGameMacroMsgOptDlg::GetMacroString( int nIndex )
{
	if( nIndex < 0 || nIndex >= CGameOption::NUM_COMM_MACROS )
		return NULL;

	if( CGameOption::GetInstance().m_bDefaultMacro[nIndex] )
	{
		return GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_nDefaultMacroUIStringID[nIndex] );
	}
	else
	{
		_wcscpy( m_wszTemp, _countof(m_wszTemp), CGameOption::GetInstance().m_wszMacro[nIndex], (int)wcslen(CGameOption::GetInstance().m_wszMacro[nIndex]));
		return m_wszTemp;
	}
}

void CDnGameMacroMsgOptDlg::ResetDefault()
{
	GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3200 ), MB_YESNO, 0, this );
}

void CDnGameMacroMsgOptDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) )
		{
			for( int i = 0; i < CGameOption::NUM_COMM_MACROS; i++ )
			{
				m_pEditBox[i]->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_nDefaultMacroUIStringID[i] ) );
			}
		}
	}
}