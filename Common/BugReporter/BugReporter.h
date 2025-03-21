#pragma once

#include "SystemInfo.h"
#include "RyeolHttpClient.h"
#include "../Utility/AssertX.h"
#include "../Utility/CriticalSection.h"
#include <vector>

using namespace Ryeol;

class ExceptionCallback
{
public:
	virtual void OnException(_EXCEPTION_POINTERS* pExceptionInfo) { }
};
/*
	뻑 나면 덤프파일과 버그리포팅 파일을 남기고 웹으로 전송해주는 클래스 
*/
class BugReporter
{
public:
	enum DumpLevel
	{
		eMiniDumpNormal = 0,
		eMiniDumpWithDataSegs = 1,
		eMiniDumpWithFullMemory = 2,
	};

	struct ReportingServerInfo
	{
		ReportingServerInfo()
		{
			// kb 단위
			nMaxReportSize = 20000; // 기본값 20메가 ㅋㅋ
#if defined(_CH)
			szViewServerURL = "";//"http://116.211.21.173/ReportList.aspx";
			szServerURL = "http://116.211.21.173/BugReport/BugProcess.aspx";
			szSIDURL = "http://116.211.21.173/BugReport/dumpprocess.aspx";
// #elif defined(_JP)
// 			szViewServerURL = "";
// 			szServerURL = "bugreport.hangame.co.jp/BugProcess.aspx";
// 			szSIDURL = "bugreport.hangame.co.jp/dumpprocess.aspx";
// #elif defined(_TW)
// 			szViewServerURL = "";
// 			szServerURL =	"http://112.121.85.5/BugTrace.aspx";
// 			szSIDURL =	"http://112.121.85.5/dumpprocess.aspx";
// #elif defined(_US)
// 			szViewServerURL = "";
// 			szServerURL =	"http://208.85.111.192/BugProcess.aspx";
// 			szSIDURL =	"http://208.85.111.192/dumpprocess.aspx";
// #elif defined(_SG)
// 			szViewServerURL = "";
// 			szServerURL =	"http://bugrpt.dn.cherrycredits.com/BugProcess.aspx";
// 			szSIDURL =	"http://bugrpt.dn.cherrycredits.com/dumpprocess.aspx";
#elif defined(_KRAZ)
			szViewServerURL = "";
			szServerURL = "http://61.251.164.138/BugProcess.aspx";				//요넘이 실재로 보내지는것 
			szSIDURL = "http://61.251.164.138/BugReport/dumpprocess.aspx";		//덤프 발생시 보내지 않음을 선택할때 발생횟수만 체크하는것
#else
			szViewServerURL = "";
			szServerURL = "";
			szSIDURL = "";
#endif
			
			szNoticeMsg = L"";
			bUseAutoView = false;
			bSaveLocal = false;		// 유저의 하드에 덤프와 로그를 남길것인가.
			bUseConfirm = false;
			hWnd = NULL;
			hAppIcon = 0;
			hInst = 0;

		}
		std::string szServerURL;
		std::string szViewServerURL;
		std::string szSIDURL;
		std::wstring szAppName;
		std::wstring szNoticeTitle;
		std::wstring szNoticeMsg;
		std::wstring szSendButtonStr;
		std::wstring szNotSendButtonStr;
		HICON hAppIcon;
		
		DWORD		nMaxReportSize;
		bool		bUseAutoView; // 리포팅 리스트를 볼것인지
		bool		bSaveLocal;	// 리포팅후 하드에 저장할것인지
		bool		bUseConfirm; // 물어보고 등록할것인지 옵션
		HWND		hWnd;
		HINSTANCE	hInst;
		bool		bEnableReporting;	// 리포팅 여부
	};

	BugReporter();
	virtual ~BugReporter();

	// 버그리포팅할 url
	void SetReportingServerInfo(ReportingServerInfo& info)
	{ 
		m_ReportHttpInfo = info;
	}

	ReportingServerInfo& GetReportingServerInfo() { return m_ReportHttpInfo; }

	// 버전을 구분하기위한 문자열 
	void SetBuildVersion(std::string& szBuildVersion) { m_szBuildVersion = szBuildVersion; }
	
	/*
		커스텀 디버깅 정보를 리포트에 추가하기 위해서 사용한다.
		ex ) 
		std::string g_szUserNickname;
		BugReporter.AddDebugData(&g_szUserNickname); // 포인터를 등록한다.

		g_szUserNickname 은 로그인 완료 서버 패킷을 받으면 유저닉네임을 등록해둔다.  UserNick = [던힐] UserGuild = [프로스트길드] 

		오류 발생후 리포팅된 파일에 UserDebugData 항목에 보면 해당 정보를 확인할수 있다.
	*/
	void AddDebugData(std::string* pStr) { m_UserDataArray.push_back(pStr); }

	// 인증이라던지 기타 추가적으로 확인할 로그를 넣어둔다. 너무 많은 로그를 쓰면 리포팅 파일이 너무 커지므로 꼭 필요하고 중요한 내용만 로그로 추가한다.
	void AddLogA(char* fmt, ... );

	void AttachCurrentTimeLog();

	void AddLogW(WCHAR* fmt, ... );

	void Enable(DumpLevel level = eMiniDumpWithDataSegs );
	void Disable();

	static DWORD WINAPI _StackOverflowException( LPVOID pParam );
	static  LONG	__stdcall Exception_Minidump(_EXCEPTION_POINTERS* pExceptionInfo);
	static  void	__cdecl	  DefaultBadAllocFilter();

	long BugReport(_EXCEPTION_POINTERS* pExceptionInfo);
	long MakeBugReportText(_EXCEPTION_POINTERS* pExceptionInfo);
	long MakeDumpFile(_EXCEPTION_POINTERS* pExceptionInfo);
	long HttpReporting();
	long SIDReporting();
	void SetSID( char *szSID );
	void EnableCompressDump( bool bEnable ) { m_bEnableCompress = bEnable; }

	void ConfirmDialog();
	// 익셉션이 난 상황은 아니지만 그냥 강제로 로그를 받아보기 위해서 리포팅 하는 기능이다.
	long ForceReporting();

	void SetExceptionCallBack(ExceptionCallback* pCallback) { m_pExceptionCallback = pCallback; }
private:


private:
	int				m_nDumpLevel;
	bool			m_bEnableDump;
	bool			m_bEnableCompress;

	// 로그를 남겨서 보내는 기능을 추가하면서,, 여러 스레드에서 로그를 남길 수 있으므로 동기화 처리를 함.
	// 실제로 리포팅하는 부분도(BugReport()) 추후에 여러 스레드에서 할 수 있게 된다면 동기화 처리를 해야한다.
	CSyncLock		m_LogLock;
	bool m_bForceLogReporting;

	static  LPTOP_LEVEL_EXCEPTION_FILTER m_pExceptionFilter;
	static  _invalid_parameter_handler m_pInvalidParameterHandler;
	static  _purecall_handler m_pPureCallHandler;

	SystemInfo m_SystemInfo;

	std::string m_szProcessName;
	std::string m_szBuildVersion;
	std::string m_szUserLog;
	std::string m_szReport;
	std::string m_szStartTime;	
	std::string m_szReportingTime;	
	std::string m_szFileTime;	
	std::string m_szDumpFileName;
	std::string m_szCompressDumpFileName;
	std::string m_szReportFileName;
	std::vector<std::string*>	m_UserDataArray;

	ReportingServerInfo m_ReportHttpInfo;
	CHttpClientA m_HttpClient;
	CHttpClientA m_HttpClientForSID;

	ExceptionCallback*	m_pExceptionCallback;
	char			m_szSID[256];

};

extern BugReporter gs_BugReporter;