#pragma once

#include <queue>
#include <string>

//  [5/12/2009 nextome]

/*

넥슨 와이즈 로그 url

------ 넥슨에서 해주는 영역 --------------------------------------------------
1. 웹	게임다운로드	http://dragonnest.nexon.com/wiselog/clientldownload.aspx
2. 웹게임스타트	http://dragonnest.nexon.com/wiselog/gamestart.aspx

------ 드래곤네스트 런쳐에서 해주는 영역 --------------------------------------
3. 클라이언트	런처시작	http://dragonnest.nexon.com/wiselog/gamelauncher.aspx
4. 버전체크시작	http://dragonnest.nexon.com/wiselog/versioncheckstart.aspx
5. 버전체크완료	http://dragonnest.nexon.com/wiselog/versioncheckend.aspx
6. 패치시작	http://dragonnest.nexon.com/wiselog/patchstart.aspx
7. 패치완료	http://dragonnest.nexon.com/wiselog/patchend.aspx
   
런처 스타트 클릭: http://dragonnest.nexon.com/wiselog/launcherstartclick.aspx
런처 웹페이지 호출: http://dragonnest.nexon.com/wiselog/webloadstart.aspx
런처 웹페이지 호출 종료: http://dragonnest.nexon.com/wiselog/webloadend.aspx
런처 패치 취소: http://dragonnest.nexon.com/wiselog/patchcancel.aspx

------ 드래곤네스트 클라이언트 에서 해주는 영역 -------------------------------
8. 핵실드초기화시작	http://dragonnest.nexon.com/wiselog/hackinistart.aspx
9. 핵실드초기화완료	http://dragonnest.nexon.com/wiselog/hackiniend.aspx
10. 게임엔진시작	http://dragonnest.nexon.com/wiselog/gameengin.aspx
11. CI동영상시작	http://dragonnest.nexon.com/wiselog/cimov.aspx
12. 서버선택	http://dragonnest.nexon.com/wiselog/server.aspx
13. 캐릭터선택	http://dragonnest.nexon.com/wiselog/character.aspx
14. 채널선택	http://dragonnest.nexon.com/wiselog/channel.aspx

*/


/*
	Desc :  넥슨 연동에 필요한 wiselog 클래스
	와이즈로그는 유저가 어느 단계에서 이탈 하는지 알아 보기 위한 솔루션으로 프로그램의 진행 순서에 따라 넥슨에서 제공한
	웹페이지를 호출 해줌으로서 단계별 이탈율을 통계 낼수 있다.  Start를 호출 하게 되면 쓰레드가 생성 되며 평상시엔 대기중이다가 
	WriteToWiseLog 함수를 통해 큐에 이벤트가 발생하면 들어온 순서대로 와이즈 로그를 처리하게 된다.

	exam : g_WiseLog.WriteToWiseLog("hackinistart.aspx"); 이런식으로 호출하면 된다.
	
*/

#if defined(_KR)

class WiseLog
{
public:
	WiseLog();
	virtual ~WiseLog();

	/*
		desc  : 와이즈 로그를 시작한다.
		param : 넥슨웹페이지에서 로그인을 한후 인증을 하게 되면 oid 를 알수 있다 이 값을 세팅 해주어야 넥슨쪽에서 통계를 낼수 있다. 
				넥슨 계정에 대한 인덱스인가 보다..
	*/
	bool		Start(UINT uMyOid);
	void		Destroy();

	// 이 함수를 이용하면 와이즈로그 쓰레드에서 SendWebPost 함수를 통해 와이즈로그를 남긴다.
	bool		WriteToWiseLog(const char* szLogFile);
	bool		Terminate( DWORD nExitCode );

	// 이 함수를 이용하면 현재의 쓰레드에서 와이즈로그를 남긴다. 
	// 이함수를 이용하면 웹사이트 상황에 따라 약간의 블러킹이 발생할수 있다. 
	// 그래서 WriteToWiseLog 함수를 이용하도록 한다.
	void		SendWebPost(const char* szLogFile);

protected:

	void		Run();

	HANDLE		m_hThreadHandle;
	DWORD		m_nThreadID;
	bool		m_bRunning;
	char		m_szThreadNick[128];

	HANDLE		m_hThreadSignal;
	UINT32		m_uMyOid;
	CRITICAL_SECTION m_Lock;

	std::queue<std::string>	m_WiseLogQueue;

private:

	static	DWORD	WINAPI	_Runner( LPVOID pParam );
	static  void SetThreadName( DWORD dwThreadID, const char* szThreadName);

private:

	// 
	typedef struct tagTHREADNAME_INFO 
	{ 
		DWORD dwType;		// must be 0x1000 
		LPCSTR szName;		// pointer to name (in user addr space) 
		DWORD dwThreadID;	// thread ID (-1=caller thread) 
		DWORD dwFlags;		// reserved for future use, must be zero 
	} THREADNAME_INFO; 

};

#endif	// #if defined(_KR)