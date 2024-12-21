#include "stdafx.h"

#include <wx/wx.h>
#include <wx/msw/private.h>
#include "TheApp.h"
#include "RTCutSceneMakerFrame.h"
#include "LuaDelegate.h"
#include "ToolData.h"
#include "EtResourceMng.h"
#include "EtUIXML.h"

//#include "ExceptionDump.h"


IMPLEMENT_APP( CTheApp )

// ���ø����̼� ����
CTheApp::CTheApp( void ) : m_pFrame( NULL )
{
	LUA_DELEGATE.RunScript( "script/RTCutSceneTool_Setup.lua" );
	CEtResourceMng::CreateInstance();		// EtUIXML �����ڿ��� ���̹Ƿ� �ݵ�� EtUIXML ���� ���� �����Ǿ�� ��.

	const char* pResourcePath = LUA_DELEGATE.GetString( "resource_path" );
	//const char* pCharPath = LUA_DELEGATE.GetString( "chardata_path" );
	const char* pMapPath = LUA_DELEGATE.GetString( "mapdata_path" );
	const char* pShaderPath = LUA_DELEGATE.GetString( "shader_path" );
	//const char* pSoxPath = LUA_DELEGATE.GetString( "sox_path" );

	// EYE-AD �� ���ҽ� �������� ������� �ʴ� effect ���ϵ��� ��Ƴ��� ���� �����Ƿ� ���� ������ ���̴� �н��� 
	// ���� �˻� �ǵ��� �Ѵ�.
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\SharedEffect" );
	//CEtResourceMng::GetInstance().AddResourcePath( "\\\\192.168.0.11\\ToolData\\SharedEffect" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData\\Resource" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData\\Resource\\Prop", true );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData\\Resource\\Envi" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData\\Resource\\Sound", true );

	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Char", true );

	CEtResourceMng::GetInstance().AddResourcePathByCodePage( "R:\\GameRes\\Resource\\Ext" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Weapon" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Item", true );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Sound", true );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Particle" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\UI", true );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Effect" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\fonts" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Script", true );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Dmv" );
	CEtResourceMng::GetInstance().AddResourcePathByCodePage( "R:\\GameRes\\Resource\\Npc" );
	CEtResourceMng::GetInstance().AddResourcePathByCodePage( "R:\\GameRes\\Resource\\Quest" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\StateEffect" );
	CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\UIString" );

	CEtUIXML::CreateInstance();				// ToolData �����ڿ��� ���̹Ƿ� �ݵ�� ToolData ���� ���� �����Ǿ�� ��.
	CToolData::CreateInstance();

	//ExceptionDump::Enable();
}

CTheApp::~CTheApp( void )
{
	CToolData::DestroyInstance();
	CEtResourceMng::DestroyInstance();
	CEtUIXML::DestroyInstance();
}


bool CTheApp::OnInit( void )
{
	// TODO: ��� ��ũ��Ʈ �ʱ�ȭ
#if defined(_DEBUG) | defined(DEBUG)
	LUA_DELEGATE.StartDebugConsole( wxGetInstance() );
#endif

	m_pFrame = new CRTCutSceneMakerFrame( wxT( "RealTime Cut Scene Maker" ), wxDefaultPosition, wxSize(1024, 768) );
	m_pFrame->Show( true );
	SetTopWindow( m_pFrame );

	return true;
}