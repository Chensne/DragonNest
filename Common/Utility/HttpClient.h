/*//===============================================================

	<< HTTP CLIENT >>

	FILE : HTTPCLIENT.H, HTTPCLIENT.CPP
	DESC : WININET 라이브러리를 이용한 HTTP CLIENT 클래스
	INIT BUILT DATE : 2006. 02. 20
	LAST BUILT DATE : 2006. 02. 21

	P.S.>
		- One-Request,One-Response / Keep-Alive 방식의 연결,송수신 모드 지원
		- One-Request,One-Response : 특정 URL의 HTTP 자원을 한번에 요청하여 받는 형식 (Open() → RecvResponse() → Close())
		- Keep-Alive : 특정 HTTP서버에 일단 접속한 후 매번 다른 자원을 요청하여 받는 형식 (Open() → SendRequest() → RecvResponse() → Close())

	Copyrightⓒ 2002~2007 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include <TCHAR.H>
#include <WINDOWS.H>
#include <WININET.H>

#pragma comment(lib, "WININET.LIB")


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define	DF_DEFAULT_HTTP_PORTNUMBER	(80)

///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CHttpClient
//*---------------------------------------------------------------
// DESC : Http Client 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CHttpClient
{
public:
	enum EF_HTTP_CONNECTION_TYPE {
		EV_HTTP_CONNECTION_NONE			= 0,	// 초기값
		EV_HTTP_CONNECTION_ONEREQONERSP	= 1,	// HTTP 연결방식 : One-Req,One-Rsp
		EV_HTTP_CONNECTION_KEEPALIVE	= 2,	// HTTP 연결방식 : Keep-Alive
	};

public:
	CHttpClient();
	~CHttpClient();

	// One-Req,One-Rsp / Keep-Alive 공용 메서드
	BOOL Open(
		LPCTSTR lpszHttpServerAddressUrl,	// One-Req,One-Rsp : URL 자원경로만 사용 / Keep-Alive : 서버주소 (URL, DOT-DECIMAL)
		EF_HTTP_CONNECTION_TYPE eHttpConnectionMode = EV_HTTP_CONNECTION_ONEREQONERSP,
		INTERNET_PORT wHttpServerPortNumber = DF_DEFAULT_HTTP_PORTNUMBER,	// Keep-Alive 일 경우만 사용
		LPCTSTR lpszUserName = NULL,		// Keep-Alive 일 경우만 사용
		LPCTSTR lpszPassword = NULL			// Keep-Alive 일 경우만 사용
		);	
	VOID Close();
	BOOL IsOpen();
	INT RecvResponse(LPBYTE lpbtBuffer, INT nBufferSize);
	INT RecvDownFile(LPCTSTR lpszFileName, LPBINDSTATUSCALLBACK pCallBack);
	DWORD SetOption(DWORD dwMaxConnCount, DWORD dwTimout);

	// One-Req,One-Rsp 전용 메서드

	// Keep-Alive 전용 메서드
	BOOL ReOpen();
	INT SendRequest(LPCTSTR lpszUrl, BOOL bCheckResultDataLength = FALSE, BOOL bAsyncRequest = FALSE);	
	INT SendRequsetPostA(LPCTSTR lpszUrl, LPCSTR szPostData); //Ansi용 Post
	INT SendRequestGet(LPCTSTR lpszUrl);	

	LONG m_RefCount;
	LONG IncRefCount() { return(InterlockedIncrement(&m_RefCount)); }
	LONG DecRefCount() { return(InterlockedDecrement(&m_RefCount)); }

private:
	VOID Initialize();
	VOID Finalize();

private:
	// One-Req,One-Rsp / Keep-Alive 공용 멤버변수
	HINTERNET m_hInternet;				// InternetOpen() 의 핸들을 가짐
	HINTERNET m_hInternetConnection;	// One-Req,One-Rsp : InternetOpenUrl() / Keep-Alive : InternetConnect() 의 핸들을 가짐
	HINTERNET m_hHttpRequest;			// Keep-Alive 방식에서만 사용됨

	EF_HTTP_CONNECTION_TYPE m_eHttpConnectionMode;

	// One-Req,One-Rsp 전용 멤버변수

	// Keep-Alive 전용 멤버변수
	TCHAR m_szHttpServerAddress[MAX_PATH];
	INTERNET_PORT m_wHttpServerPortNumber;
	TCHAR m_szUserName[MAX_PATH];
	TCHAR m_szPassword[MAX_PATH];	
};

