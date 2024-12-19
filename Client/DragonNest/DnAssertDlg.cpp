#include "StdAfx.h"
#include "DnAssertDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

bool g_bRenderFrame = false;

CDnAssertDlg::CDnAssertDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pStaticMsg(NULL)
	, m_emRetValue(valueNone)
	, m_emInterfaceType( CDnInterface::InterfaceTypeEnum::Common )
{
}

CDnAssertDlg::~CDnAssertDlg(void)
{
}

void CDnAssertDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AssertDlg.ui" ).c_str(), bShow );
}

void CDnAssertDlg::InitialUpdate()
{
	m_pStaticMsg = GetControl<CEtUIStatic>("ID_STATIC2");
}

void CDnAssertDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_YES" ) )
		{
			m_emRetValue = valueBreak;
		}
		else if( IsCmdControl("ID_NO") )
		{
			m_emRetValue = valueContinue;
		}

		Show(false);
	}
}

void CDnAssertDlg::SetInfo( CDnInterface::InterfaceTypeEnum emType, const wchar_t *szMsg, CEtUICallback *pCallback )
{
	m_emInterfaceType = emType;
	SetCallback( pCallback );
	m_pStaticMsg->SetText( szMsg );
}

void CDnAssertDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_emRetValue = valueNone;
	}
	
	CEtUIDialog::Show( bShow );

	/*
	if( (m_emInterfaceType==CDnInterface::Village) || (m_emInterfaceType==CDnInterface::Game) )
	{
		CDnMouseCursor::GetInstance().ShowCursor( bShow );
	}
	*/
}

void _gassert( const wchar_t *szExp, const wchar_t *szFile, unsigned nLine )
{
#ifndef _FINAL_BUILD
	if( g_bRenderFrame )
	{
		assert( szExp );
		return;
	}

	wchar_t szTemp[256]={0};
	swprintf_s( szTemp, 256, L"Assertion Failed!\n\n"
							 L"File : %s\n"
							 L"Line : %d\n\n"
							 L"Expression : %s", szFile, nLine, szExp );

	if( !CDnInterface::IsActive() )
	{
		CDebugSet::ToLogFile( "ASSERT:Error occured! File : %s, Line : %d, Expression : %s is false.", szFile, nLine, szExp );
		return;
	}

	GetInterface().OpenAssert( szTemp, NULL );

	MSG msg;

	while(1)
	{
		BOOL bResult = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
		if( msg.message == WM_QUIT ) break;
		if( msg.message == WM_PAINT ) bResult = false;

		if( bResult ) 
		{
			if( TranslateAccelerator( msg.hwnd, NULL, &msg ) == 0 ) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// Network Process
			//CClientSessionManager::GetInstance().Process();

			// Process Task
			//if( m_pTaskMng->Excute() == false ) break;

			CDnAssertDlg::emReturnValue retValue = CDnAssertDlg::GetInstance().GetReturnValue();

			switch( retValue )
			{
			case CDnAssertDlg::valueBreak:
				_asm { int 3 }
				//abort();
			case CDnAssertDlg::valueContinue:
			case CDnAssertDlg::valueIgnore:
				return;
			}

			g_bRenderFrame = true;
			{
				EternityEngine::ClearScreen( 0xFFFFFFFF, 1.0f, 0, false );
				EternityEngine::RenderFrame( 0.0f );
				EtInterface::Render( 0.0f );
				EternityEngine::ShowFrame( NULL );
			}
			g_bRenderFrame = false;
		}
	}
#else
	CDebugSet::ToLogFile( "ASSERT:Error occured! File : %s, Line : %d, Expression : %s is false.", szFile, nLine, szExp );
#endif
}