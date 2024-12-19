#include "stdafx.h"
#include "DnServiceModuleKOR.h"


#if defined(_KOR)

#include "../../Extern/Nexon/NMClass/NMSerializable.h"
#include "../../Extern/Nexon/NMClass/NMFiles.cpp"
#include "StringUtil.h"
#define	_nmman	CNMManager::GetInstance()
#define	_nmco	CNMCOClientObject::GetInstance()

extern CString g_szOriginalCmdLine;
WiseLog* g_pWiseLog = NULL;
NMGameCode g_uGameCode = 0x0200700D;

BOOL CDnServiceModuleKOR::Initialize()
{
#ifndef _FIRST_PATCH
	CFileFind FileFind;
	if( FileFind.FindFile( _T( "nmcogame.tmp" ) ) )
		ClientDeleteFile( _T( "nmcogame.tmp" ) );

	if( FileFind.FindFile( _T( "nmcogame.new" ) ) )	// 바뀐 nmcogame.dll이 있을 경우 교체
	{
		ClientDeleteFile( _T( "nmcogame.dll" ) );
		MoveFile( _T( "nmcogame.new" ), _T( "nmcogame.dll" ) );
	}

	if( FileFind.FindFile( _T( "nmconew.tmp" ) ) )
		ClientDeleteFile( _T( "nmconew.tmp" ) );

	if( FileFind.FindFile( _T( "nmconew.new" ) ) )	// 바뀐 nmconew.dll이 있을 경우 교체
	{
		ClientDeleteFile( _T( "nmconew.dll" ) );
		MoveFile( _T( "nmconew.new" ), _T( "nmconew.dll" ) );
	}
#endif

	std::vector<stChannelListSet>::iterator iter = DNPATCHINFO.GetChannelList().begin();
	if( iter != DNPATCHINFO.GetChannelList().end() )
	{
		std::vector<stPartitionListSet>::iterator iter2 = (*iter).m_vecPartitionList.begin();
		if( iter2 != (*iter).m_vecPartitionList.end() )
		{
			if( wcscmp( (*iter2).m_strPartitionName, L"Dragonnest" ) == 0 )
				g_uGameCode = 0x0200700D;
			else if( wcscmp( (*iter2).m_strPartitionName, L"Dragonnest_Test" ) == 0 )
				g_uGameCode = 126996;
			else if( wcscmp( (*iter2).m_strPartitionName, L"Dragonnest_OpenTest" ) == 0 )
				g_uGameCode = 0x0001F01F;
		}
	}

	// nmconew.dll에서 테스트서버경로 저장된 .cfg파일을 읽으려면 COM객체를 써야하기 때문에 호출함
	// 윈도우 버젼이 올라가면서 확장된 초기화 함수를 사용 할 수 있다.
#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
#else
	CoInitialize( NULL );
#endif
	if( !InitWiseLog() )
	{
		CoUninitialize();
		return FALSE;
	}

	CoUninitialize();
	if( g_pWiseLog )
		g_pWiseLog->SendWebPost( "gamelauncher.aspx" );

	return TRUE;
}

void CDnServiceModuleKOR::Destroy()
{
	SAFE_DELETE( g_pWiseLog );
}

BOOL CDnServiceModuleKOR::InitWiseLog()
{
	LogWnd::TraceLog( _T("InitWiseLog()") );

	NMLOCALEID	uLocaleId = kLocaleID_KR;
	BOOL bResult = TRUE;
	//	[필수] 로케일 설정
	bResult = CNMCOClientObject::GetInstance().SetLocale(uLocaleId);

	if( !bResult )
	{
		LogWnd::Log( LogLevel::Error, _T("NMService intialize failed. ( SetLocale )") );
		return FALSE;
	}

	//	[필수] 메신저 모듈 초기화
	bResult = CNMCOClientObject::GetInstance().Initialize( g_uGameCode );

	if( !bResult )
	{
		LogWnd::Log( LogLevel::Error, _T("NMService intialize failed. ( nmco.Initialize )") );
		return FALSE;
	}

	// 커맨드라인에서 패스포트 스트링 추출이 성공한 경우, 웹런칭 형식으로 동작합니다.
	if( g_szOriginalCmdLine.GetLength() == 0 )
	{
		LogWnd::Log( LogLevel::Error, _T("NMService intialize failed. ( CmdLine Emtpy )") );
		return FALSE;
	}

	std::wstring pCmdline = g_szOriginalCmdLine;
	std::vector<std::wstring> tokens;
	TokenizeW( pCmdline,  tokens, std::wstring( L" " ) );
	std::wstring szPassPort;
	szPassPort = tokens[0].c_str();

	NMLoginAuthReplyCode resultAuth = CNMCOClientObject::GetInstance().AttachAuth( szPassPort.c_str() );

	if( resultAuth == kLoginAuth_OK )
	{
		LogWnd::TraceLog( _T("kLoginAuth_OK success!"));
		CNMCOClientObject::GetInstance().GetMyInfo();
		UINT32 nMyOid = CNMCOClientObject::GetInstance().GetMyOid();
		// 와이즈 로그 시작
		g_pWiseLog = new WiseLog();
		if( g_pWiseLog )
			g_pWiseLog->Start(nMyOid);
	}
	else
	{
		LogWnd::Log( LogLevel::Error, _T("AttachAuth failed! result %d"), int(resultAuth) );
//		LogWnd::Log( LogLevel::Error, _T("AttachAuth failed! result %d, Passport %s"), int(resultAuth), szPassPort.c_str() );
		return FALSE;
	}

	CNMCOClientObject::GetInstance().LogoutAuth();
	CNMCOClientObject::GetInstance().DetachAuth();

	return TRUE;
}

BOOL CDnServiceModuleKOR::OnForceFullVersionPatch()
{
	BOOL bResult = TRUE;
	NMLOCALEID	uLocaleId = kLocaleID_KR;

	DWORD nErrorCode = 0;
	bResult = CNMCOClientObject::GetInstance().SetLocale( uLocaleId );

	std::wstring pCmdline = g_szOriginalCmdLine;
	std::vector<std::wstring> tokens; 
	TokenizeW( pCmdline, tokens, std::wstring( L" " ) );

	std::wstring szPassPort, szSessionID;

	szPassPort = tokens[1].c_str();

	for( int i=0; i<(int)tokens.size(); i++ )
	{
		if( NULL != StrStrW( tokens[i].c_str() , L"/sid:" ) )
			szSessionID  = tokens[i].c_str() + 5;
	}

	//	[필수] 메신저 모듈 초기화
	if( bResult )
	{
		bResult = CNMCOClientObject::GetInstance().Initialize( g_uGameCode );

		if( bResult )
		{
			bResult = NMCOHelpers::ExecuteNGMInstaller( g_uGameCode, TRUE, TRUE, DNPATCHINFO.GetOptionParameter().GetBuffer(), (WCHAR *)szPassPort.c_str(), (WCHAR*)szSessionID.c_str() );
			return bResult;
		}
		else
			nErrorCode = 7002;
	}
	else
		nErrorCode = 7001;

	if( !bResult )
	{
		CString szMsg;
		szMsg.Format(  _T("드래곤네스트 재설치를 실패하였습니다. 오류코드 : %d"), nErrorCode );
		AfxMessageBox( szMsg );
	}

	return bResult;
}

#endif // _KOR