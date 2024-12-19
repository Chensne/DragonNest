#include "stdafx.h"
#include <time.h>
#include <DbgHelp.h>
#include <iostream>
#include "BugReporter.h"
#include "StackWalker.h"
#include "shellapi.h"
#include "resource.h"
#include <Richedit.h>
#include "zlib.h"
#include "shlobj.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

LPTOP_LEVEL_EXCEPTION_FILTER BugReporter::m_pExceptionFilter = NULL;
_invalid_parameter_handler BugReporter::m_pInvalidParameterHandler = NULL;
_purecall_handler BugReporter::m_pPureCallHandler = NULL;


// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP) (  HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										  CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										  CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										  CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam  );
using std::cout;
using std::endl;

BugReporter gs_BugReporter;

static void myInvalidParameterHandler(const wchar_t* expression,
							   const wchar_t* function, 
							   const wchar_t* file, 
							   unsigned int line, 
							   uintptr_t pReserved)
{
	RaiseException( EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE , 0, 0);
}

static void myPurecallHandler()
{
	RaiseException( EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE , 0, 0);
}

BugReporter::BugReporter()
{
	LoadLibrary( L"RichEd20.dll" );
	m_pExceptionCallback =0;
	m_szBuildVersion.reserve(1024);
	m_szUserLog.reserve(8192);
	m_szReport.reserve(8192*3);

	// ���� ���� ����� �˾Ƴ���.
	CHAR szCurFileName[MAX_PATH]= {0,};
	::GetModuleFileNameA(NULL, szCurFileName, MAX_PATH);
	CHAR szDrive[MAX_PATH] = { 0, };
	CHAR szDir[MAX_PATH] = { 0, };
	CHAR szFileName[MAX_PATH] = { 0, };
	CHAR szExt[MAX_PATH] = { 0, };
	_splitpath_s(szCurFileName, szDrive, szDir, szFileName, szExt );
	m_szProcessName = szCurFileName;

	m_nDumpLevel = BugReporter::eMiniDumpWithDataSegs;
	m_bEnableDump = true;

	m_HttpClient.SetInternet(("BugReporter Agent"));
	m_HttpClientForSID.SetInternet("SIDReporter Agent");
	strcpy(m_szSID, "0");

	m_bEnableCompress = false;

	m_bForceLogReporting = false;

	// bug time 
	struct tm* now=NULL;
	time_t systemTime;
	time(&systemTime);  
	now=localtime(&systemTime);
	CHAR szTail[MAX_PATH] = {0,};
	sprintf(szTail, "_%04d��%02d��%02d��_%02d��%02d��%02d��",
		1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	m_szStartTime = szTail;
}

BugReporter::~BugReporter()
{

}

void __cdecl BugReporter::DefaultBadAllocFilter()
{
	MessageBoxA( gs_BugReporter.GetReportingServerInfo().hWnd, "Out Of Memory.", "BugReporter : Critical Error!", MB_OK );
}

void BugReporter::Enable( DumpLevel level )
{
	m_bEnableDump = true;
	m_nDumpLevel = level;

	m_pInvalidParameterHandler = _set_invalid_parameter_handler( myInvalidParameterHandler );
	m_pPureCallHandler = _set_purecall_handler( myPurecallHandler );
	m_pExceptionFilter = SetUnhandledExceptionFilter(BugReporter::Exception_Minidump); 
}

void BugReporter::Disable()
{
	SetUnhandledExceptionFilter( m_pExceptionFilter );
	_set_invalid_parameter_handler( m_pInvalidParameterHandler );
	_set_purecall_handler( m_pPureCallHandler );
}

void BugReporter::AddLogA(char* fmt, ... )
{
	ScopeLock<CSyncLock> Lock( m_LogLock );

	CHAR szBuffer[1024] = {0,};
	va_list args;
	va_start( args, fmt );
	_vsnprintf_s(szBuffer, 1024-1, fmt, args);
	va_end( args );
	szBuffer[1024-1] = 0;

	AttachCurrentTimeLog();
	m_szUserLog += szBuffer;
	m_szUserLog += "\n";
}

void BugReporter::AddLogW(WCHAR* fmt, ... )
{
	ScopeLock<CSyncLock> Lock( m_LogLock );

	CHAR szBuffer[2048] = {0,};
	WCHAR wszBuffer[2048] = {0,};
	va_list args;
	va_start( args, fmt );
	_vsnwprintf_s(wszBuffer, sizeof(wszBuffer), fmt, args);
	va_end( args );
	wszBuffer[2048-1] = 0;

	WideCharToMultiByte( CP_ACP, 0, wszBuffer, -1, szBuffer, 2048, NULL, NULL );

	AttachCurrentTimeLog();
	m_szUserLog += szBuffer;
	m_szUserLog += "\n";
}

long BugReporter::MakeBugReportText(_EXCEPTION_POINTERS* pExceptionInfo)
{
	ScopeLock<CSyncLock> Lock( m_LogLock );

	m_szReport = "";

	m_SystemInfo.Initialize();

	m_szReport += "<version>\n";
	m_szReport += m_szBuildVersion;
	m_szReport += "\n";

	m_szReport += "<start time>\n";
	m_szReport += m_szStartTime;
	m_szReport += "\n";

	m_szReport += "<time>\n";
	m_szReport += m_szReportingTime;
	m_szReport += "\n";	

	m_szReport += "<user>\n";
	m_szReport += m_SystemInfo.GetUserInfo();
	m_szReport += "\n";

	m_szReport += "<process>\n";
	m_szReport += m_szProcessName;
	m_szReport += "\n";

	m_szReport += "<os>\n";
	m_szReport += m_SystemInfo.GetOSInfo();
	m_szReport += "\n";

	m_szReport += "<cpu>\n";
	m_szReport += m_SystemInfo.GetCpuString();
	m_szReport += "\n";

	m_szReport += "<memory>\n";
	m_szReport += m_SystemInfo.GetMemInfoString();
	m_szReport += "\n";

	m_szReport += "<video>\n";
	m_szReport += m_SystemInfo.GetVideoCardString();
	m_szReport += "\n";

	m_szReport += "<directx>\n";
	m_szReport += m_SystemInfo.GetDxVersionString();
	m_szReport += "\n";

	m_szReport += "<ip>\n";
	m_szReport += m_SystemInfo.GetLocalIPString();
	m_szReport += "\n";

	// �߰��Ǿ��ִ� ���� �����͵��� ���� ������ش�.
	m_szReport += "<user debug data>\n";
	for ( size_t i = 0 ; i < m_UserDataArray.size() ; i++ )
	{
		std::string* pStr = m_UserDataArray[i];
		if ( pStr )
		{
			m_szReport += *pStr;
			m_szReport += "\n";
		}
	}
	m_szReport += "\n";

	m_szReport += "<user log>\n";
	m_szReport += m_szUserLog;
	m_szReport += "\n";
	
	if ( pExceptionInfo )
	{
		CHAR temp[2048] = {0,};

#pragma warning( disable : 4311 )
		sprintf(temp, "ExceptionCode 0x%08x\nExceptionAddress 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode, (DWORD)pExceptionInfo->ExceptionRecord->ExceptionAddress);
#pragma warning( default : 4311 )

		std::string szFaultReason =	GetFaultReason(pExceptionInfo);
		m_szReport += "<fault reason>\n";
		m_szReport += szFaultReason;
		m_szReport += "\n";
		m_szReport += temp;
		m_szReport += "\n";

		std::string szStackString ;
		NxGetCallStack(szStackString, pExceptionInfo, GSTSO_ALL);

		std::string szRegister = GetRegisterString(pExceptionInfo);

		m_szReport += "<register>\n";
		m_szReport += szRegister;
		//m_szReport += "EAX=00000000  EBX=7FFDD000  ECX=05FE5E60  EDX=01297D2C  ESI=00125FD8\nEDI=00125FC0  EBP=00125FC0  ESP=00125ED8  EIP=0099DB2D  FLG=00210216\nCS=001B   DS=0023  SS=0023  ES=0023   FS=003B  GS=0000";
		m_szReport += "\n";

		m_szReport += "<stack>\n";
		m_szReport += szStackString;
		//m_szReport += "EAX=00000000  EBX=7FFDD000  ECX=05FE5E60  EDX=01297D2C  ESI=00125FD8\nEDI=00125FC0  EBP=00125FC0  ESP=00125ED8  EIP=0099DB2D  FLG=00210216\nCS=001B   DS=0023  SS=0023  ES=0023   FS=003B  GS=0000";
		m_szReport += "\n";
	}

	// ���ð� �ε��� ��� ������ �ؽ�Ʈ ���Ϸ� �����.
	FILE* pFile = fopen(m_szReportFileName.c_str(), "w");
	if( pFile == NULL ) return EXCEPTION_CONTINUE_SEARCH;

	fwrite(m_szReport.c_str(), 1, m_szReport.size(), pFile );
	fclose(pFile);


	return EXCEPTION_CONTINUE_SEARCH;
}

long BugReporter::BugReport(_EXCEPTION_POINTERS* pExceptionInfo)
{
	if ( m_pExceptionCallback )
	{
		m_pExceptionCallback->OnException(pExceptionInfo);
	}

	// bug time 
	struct tm* now=NULL;
	time_t systemTime;

	time(&systemTime);  
	now=localtime(&systemTime);

	CHAR szTail[MAX_PATH] = {0,};
	sprintf(szTail, "_%04d��%02d��%02d��_%02d��%02d��%02d��",
		1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

	m_szReportingTime = szTail;

	sprintf(szTail, "_%04d_%02d_%02d-%02d_%02d_%02d_%02d",
		1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, rand() % 100 );
	m_szFileTime = szTail;

	// ���� ���� ����� �˾Ƴ���.
	CHAR DumpFileName[MAX_PATH] = {0,};
	::GetModuleFileNameA(NULL, DumpFileName, MAX_PATH);

	CHAR drive[_MAX_DRIVE] = {0,};
	CHAR dir[_MAX_DIR] = {0,};
	CHAR fname[_MAX_FNAME] = {0,};
	CHAR ext[_MAX_EXT] = {0,};

	_splitpath( DumpFileName, drive, dir, fname, ext ); 

	int nValue = -1;
	FILE *stream = NULL;
	char szString[256];
	TCHAR pBuffer[MAX_PATH] = { 0, };
	char szPath[_MAX_PATH] = { 0, };
	SHGetSpecialFolderPath( m_ReportHttpInfo.hWnd, pBuffer, CSIDL_PERSONAL, 0 );
	WideCharToMultiByte( CP_ACP, 0, pBuffer, -1, szPath, sizeof(szPath), NULL, NULL );
	char szPartitionFilename[MAX_PATH]={0,};
	sprintf_s( szPartitionFilename, "%s\\DragonNest\\Config\\%s", szPath, "Partition.ini" );
	if( fopen_s( &stream, szPartitionFilename, "r+" ) == 0 )
	{
		fseek( stream, 0L, SEEK_SET );

		int i = 0;
		for (; i < 5; ++i)
		{
			int err = fscanf_s( stream, "%s", szString );
			if (err == EOF)
				break;

			if( strcmp("SelectPartitionId",szString ) == 0 )
			{
				fscanf_s( stream, "%d", &nValue );
				break;
			}
		}
		fclose(stream);

		if (nValue != -1)
			sprintf_s(fname, "DN%d", nValue);
	}

	// Ȯ���ڸ� ������ ��� ��θ� �غ��صд�.
	m_szDumpFileName = drive;
	m_szDumpFileName += dir;
	if( m_bForceLogReporting )
		m_szDumpFileName += "[LogOnly] ";
	m_szDumpFileName += fname;
	m_szDumpFileName += m_szFileTime;
	m_szReportFileName = m_szDumpFileName;

	m_szCompressDumpFileName = m_szDumpFileName;
	m_szCompressDumpFileName += ".gz";
	m_szDumpFileName += ".dmp";
	m_szReportFileName += ".txt";

	// ���� ���� ��ü�� �����ϼ� �ֱ⶧���� ���� ���� ������. 	

	MakeDumpFile(pExceptionInfo);
	MakeBugReportText(pExceptionInfo);
	
	if ( m_ReportHttpInfo.bEnableReporting &&
		 m_nDumpLevel < eMiniDumpWithFullMemory)
	{
		HttpReporting();
	}	

	SIDReporting();

	if ( m_ReportHttpInfo.bSaveLocal == false )
	{
		DeleteFileA(m_szDumpFileName.c_str());
		DeleteFileA(m_szReportFileName.c_str());
		DeleteFileA(m_szCompressDumpFileName.c_str());
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

long BugReporter::ForceReporting()
{
	bool bEnableReportingBackup = m_ReportHttpInfo.bEnableReporting;
	m_ReportHttpInfo.bEnableReporting = true;
	m_bForceLogReporting = true;

	long iResult = BugReport(NULL);

	m_ReportHttpInfo.bEnableReporting = bEnableReportingBackup;
	m_bForceLogReporting = false;

	return iResult;
}

long BugReporter::HttpReporting()
{
	if (m_ReportHttpInfo.szServerURL.empty())
		return 0;

	CHttpResponse *	pResponse = NULL ;
	const DWORD		dwStep = 1024 ; 

	std::string szName1("textfile1");
	std::string szName2("textfile2");
	
	m_HttpClient.AddParam(szName1.c_str(), m_szReportFileName.c_str(), CHttpClient::ParamFile);
	std::string szSendDumpFileName;
	if( m_bEnableCompress ) szSendDumpFileName = m_szCompressDumpFileName;
	else szSendDumpFileName = m_szDumpFileName;

	m_HttpClient.AddParam(szName2.c_str(), szSendDumpFileName.c_str(), CHttpClient::ParamFile);

	m_HttpClient.SetUseUtf8(FALSE);

	m_HttpClient.BeginUpload( m_ReportHttpInfo.szServerURL.c_str() );

	DWORD nCnt = 0;
	while ( pResponse == NULL )
	{
		pResponse = (CHttpResponse*)m_HttpClient.Proceed(dwStep);
		nCnt++;

		if ( nCnt > m_ReportHttpInfo.nMaxReportSize ) {
			break;
		}
	}

	if (pResponse != NULL)
	{
		CHAR szResult[8192] = {0,};
		sprintf(szResult, "%u %s\r\n", pResponse->GetStatus(), pResponse->GetStatusText() );

		if ( pResponse->GetStatus() == 200 )
		{
			if ( m_ReportHttpInfo.bUseAutoView )
				ShellExecuteA(NULL, ("open"), m_ReportHttpInfo.szViewServerURL.c_str(), NULL, NULL, SW_SHOW);
			return 1;
		}
	}

	return 0;
}

long 
BugReporter::SIDReporting()
{
#ifdef _KR
	char szReportAddr[512];
	sprintf(szReportAddr, "%s?SID=%s", m_ReportHttpInfo.szSIDURL.c_str(), m_szSID);
	m_HttpClientForSID.BeginPost( szReportAddr );
	DWORD nCnt = 0;
	CHttpResponse *	pResponse = NULL ;
	while ( pResponse == NULL )
	{
		const DWORD		dwStep = 1024 ; 
		pResponse = (CHttpResponse*)m_HttpClientForSID.Proceed(dwStep);
		nCnt++;

		if ( nCnt > m_ReportHttpInfo.nMaxReportSize )
			break;
	}
#endif
	return 1;
}

void BugReporter::SetSID( char *szSID )
{
	strcpy(m_szSID, szSID);
}

DWORD WINAPI BugReporter::_StackOverflowException( LPVOID pParam )
{
	_EXCEPTION_POINTERS* pExceptionInfo = static_cast<_EXCEPTION_POINTERS*>(pParam);
	gs_BugReporter.GetReportingServerInfo().hWnd = NULL;
	gs_BugReporter.BugReport( pExceptionInfo );
	
	return 0;
}

LONG __stdcall
BugReporter::Exception_Minidump(_EXCEPTION_POINTERS* pExceptionInfo)
{
	if( pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW ) {
		HANDLE hThread = ::CreateThread( 0, 0, _StackOverflowException, ( LPVOID )pExceptionInfo, 0, NULL );
		WaitForSingleObject( hThread, INFINITE );
		return EXCEPTION_EXECUTE_HANDLER;
	}
	LONG lResult = EXCEPTION_CONTINUE_SEARCH;
	lResult = gs_BugReporter.BugReport(pExceptionInfo);
	return lResult;
}

long
BugReporter::MakeDumpFile(_EXCEPTION_POINTERS* pExceptionInfo)
{
	if ( !pExceptionInfo )
		return 0;

	cout << "unhandled excetpion writing dump..." << endl;


	HMODULE hDLL = NULL;
	CHAR szDbgHelpPath[_MAX_PATH] = {0, };

	// ���� ���� ������ �ִ� ���丮���� DBGHELP.DLL�� �ε��� ����.
	// Windows 2000 �� System32 ���丮�� �ִ� DBGHELP.DLL ������ ������ 
	// ������ ���� �� �ֱ� �����̴�. (�ּ� 5.1.2600.0 �̻��̾�� �Ѵ�.)
	if (::GetModuleFileNameA(NULL, szDbgHelpPath, _MAX_PATH))
	{
		if (CHAR* slash = ::strchr(szDbgHelpPath, '\\'))
		{
			::strcpy(slash + 1, "DBGHELP.DLL");
			hDLL = ::LoadLibraryA(szDbgHelpPath);
		}
	}

	// ���� ���丮�� ���ٸ�, �ƹ� �����̳� �ε��Ѵ�.
	if (hDLL == NULL) 
	{
		hDLL = ::LoadLibraryA("dbghelp.dll");
	}

	// DBGHELP.DLL�� ã�� �� ���ٸ� �� �̻� ������ �� ����.
	if (hDLL == NULL)
	{
		AddLogA("dbghelp.dll not found");
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// DLL ���ο��� MiniDumpWriteDump API�� ã�´�.
	MINIDUMPWRITEDUMP pfnMiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDLL, "MiniDumpWriteDump");
	if (pfnMiniDumpWriteDump == NULL)
	{
		AddLogA("dbghelp.dll too old");
		FreeLibrary(hDLL);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	HANDLE hFile = ::CreateFileA( m_szDumpFileName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		AddLogA("Failed to save Dump File.. %s GetLastError:%d", m_szDumpFileName.c_str(), ::GetLastError() );
		FreeLibrary(hDLL);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	MINIDUMP_EXCEPTION_INFORMATION ExParam;
	ExParam.ThreadId = ::GetCurrentThreadId();
	ExParam.ExceptionPointers = pExceptionInfo;
	ExParam.ClientPointers = FALSE;

	MINIDUMP_TYPE dumptype = MiniDumpWithFullMemory;
	switch (m_nDumpLevel)
	{
	case eMiniDumpNormal:			dumptype = MiniDumpNormal;         break;
	case eMiniDumpWithDataSegs:		dumptype = MiniDumpWithDataSegs;   break;
	case eMiniDumpWithFullMemory:   dumptype = MiniDumpWithFullMemory; break;
	}

	// ���� ���� ���� ����� �α� ���Ͽ��� ����Ѵ�.
	pfnMiniDumpWriteDump( ::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, dumptype, &ExParam, NULL, NULL);
	::CloseHandle(hFile);

	if( m_bEnableCompress ) {
		// �����մϴ�.
		FILE *fp;
		if( fopen_s( &fp, m_szDumpFileName.c_str(), "rb" ) == 0 ) {
			fseek( fp, 0L, SEEK_END );
			DWORD dwOrigSize = ftell(fp);
			if( dwOrigSize < 0)
			{
				fclose(fp);
				return EXCEPTION_CONTINUE_SEARCH;
			}
			fseek( fp, 0, SEEK_SET );

			BYTE *pOrigData = new BYTE[dwOrigSize];
			fread( pOrigData, dwOrigSize, 1, fp );
			fclose(fp);

			gzFile file;
			file = gzopen( (char*)m_szCompressDumpFileName.c_str(), "wb" );
			if( file ) {
				gzwrite( file, pOrigData, dwOrigSize );
				m_ReportHttpInfo.nMaxReportSize = gztell( file ) / 1024;
				m_ReportHttpInfo.nMaxReportSize += 1024; // buff size for http proceeding
				gzclose( file );
			}
			else m_bEnableCompress = false;
			delete [] pOrigData;
		}
	}
	else {
		FILE *fp;
		if( fopen_s( &fp, m_szDumpFileName.c_str(), "rb" ) == 0 ) {
			fseek( fp, 0L, SEEK_END );
			m_ReportHttpInfo.nMaxReportSize = ftell(fp) / 1024;
			m_ReportHttpInfo.nMaxReportSize += 1024; // buff size for http proceeding
			fseek( fp, 0, SEEK_SET );
			fclose(fp);
		}
	}

	// dmp ������ �����.
	CHAR temp[2048] = {0,};

	if ( m_ReportHttpInfo.bUseConfirm )
	{
		ConfirmDialog();
	}
	else
	{
		m_ReportHttpInfo.bEnableReporting = true;
	}

	return EXCEPTION_CONTINUE_SEARCH;

}

void __TokenizeA( const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters /* = " " */ )
{
	// �� ù ���ڰ� �������� ��� ����
	std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	// �����ڰ� �ƴ� ù ���ڸ� ã�´�
	std::string::size_type pos = str.find_first_of( delimiters, lastPos );

	while( std::string::npos != pos || std::string::npos != lastPos )
	{
		// token�� ã������ vector�� �߰��Ѵ�
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// �����ڸ� �پ�Ѵ´�.  "not_of"�� �����϶�
		lastPos = str.find_first_not_of( delimiters, pos );
		// ���� �����ڰ� �ƴ� ���ڸ� ã�´�
		pos = str.find_first_of( delimiters, lastPos );
	}
}


BOOL CALLBACK ReportDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage) {
	case WM_INITDIALOG:
		{
			SetWindowText( hDlg, gs_BugReporter.GetReportingServerInfo().szAppName.c_str() );

			HWND hTitle = GetDlgItem(hDlg, IDC_RICHEDIT22);
			HWND hMsg = GetDlgItem(hDlg, IDC_STATICMSG );
			HWND hIcon = GetDlgItem( hDlg, IDC_STATICICON );

			SendMessage( hIcon, STM_SETICON, (WPARAM)gs_BugReporter.GetReportingServerInfo().hAppIcon, (LPARAM)0 );

			SetWindowText( hTitle, gs_BugReporter.GetReportingServerInfo().szNoticeTitle.c_str() );
			SetWindowText( hMsg, gs_BugReporter.GetReportingServerInfo().szNoticeMsg.c_str() );

			SetWindowText( GetDlgItem( hDlg, IDOK ), gs_BugReporter.GetReportingServerInfo().szSendButtonStr.c_str() );
			SetWindowText( GetDlgItem( hDlg, IDCANCEL ), gs_BugReporter.GetReportingServerInfo().szNotSendButtonStr.c_str() );
		}
		return TRUE;
	case WM_COMMAND:
		{
			switch (wParam) 
			{
			case IDOK:
				gs_BugReporter.GetReportingServerInfo().bEnableReporting = true;
				EndDialog(hDlg,0);
				break;
			case IDCANCEL:
				gs_BugReporter.GetReportingServerInfo().bEnableReporting = false;				
				EndDialog(hDlg,0);
				break;
			}
		}
		return TRUE;
	}
	return FALSE;
}

void BugReporter::ConfirmDialog()
{
	DialogBox( m_ReportHttpInfo.hInst, MAKEINTRESOURCE(IDD_DIALOG_REPORT), m_ReportHttpInfo.hWnd, ReportDlgProc );
}

void BugReporter::AttachCurrentTimeLog()
{
	// ���� �ð� �߰�.
	struct tm* now=NULL;
	time_t systemTime;
	time(&systemTime);  
	now=localtime(&systemTime);
	char caTime[MAX_PATH] = {0,};
	sprintf_s( caTime, "[%02d:%02d:%02d] ", now->tm_hour, now->tm_min, now->tm_sec );
	m_szUserLog += caTime;
}
