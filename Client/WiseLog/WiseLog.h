#pragma once

#include <queue>
#include <string>

//  [5/12/2009 nextome]

/*

�ؽ� ������ �α� url

------ �ؽ����� ���ִ� ���� --------------------------------------------------
1. ��	���Ӵٿ�ε�	http://dragonnest.nexon.com/wiselog/clientldownload.aspx
2. �����ӽ�ŸƮ	http://dragonnest.nexon.com/wiselog/gamestart.aspx

------ �巡��׽�Ʈ ���Ŀ��� ���ִ� ���� --------------------------------------
3. Ŭ���̾�Ʈ	��ó����	http://dragonnest.nexon.com/wiselog/gamelauncher.aspx
4. ����üũ����	http://dragonnest.nexon.com/wiselog/versioncheckstart.aspx
5. ����üũ�Ϸ�	http://dragonnest.nexon.com/wiselog/versioncheckend.aspx
6. ��ġ����	http://dragonnest.nexon.com/wiselog/patchstart.aspx
7. ��ġ�Ϸ�	http://dragonnest.nexon.com/wiselog/patchend.aspx
   
��ó ��ŸƮ Ŭ��: http://dragonnest.nexon.com/wiselog/launcherstartclick.aspx
��ó �������� ȣ��: http://dragonnest.nexon.com/wiselog/webloadstart.aspx
��ó �������� ȣ�� ����: http://dragonnest.nexon.com/wiselog/webloadend.aspx
��ó ��ġ ���: http://dragonnest.nexon.com/wiselog/patchcancel.aspx

------ �巡��׽�Ʈ Ŭ���̾�Ʈ ���� ���ִ� ���� -------------------------------
8. �ٽǵ��ʱ�ȭ����	http://dragonnest.nexon.com/wiselog/hackinistart.aspx
9. �ٽǵ��ʱ�ȭ�Ϸ�	http://dragonnest.nexon.com/wiselog/hackiniend.aspx
10. ���ӿ�������	http://dragonnest.nexon.com/wiselog/gameengin.aspx
11. CI���������	http://dragonnest.nexon.com/wiselog/cimov.aspx
12. ��������	http://dragonnest.nexon.com/wiselog/server.aspx
13. ĳ���ͼ���	http://dragonnest.nexon.com/wiselog/character.aspx
14. ä�μ���	http://dragonnest.nexon.com/wiselog/channel.aspx

*/


/*
	Desc :  �ؽ� ������ �ʿ��� wiselog Ŭ����
	������α״� ������ ��� �ܰ迡�� ��Ż �ϴ��� �˾� ���� ���� �ַ������ ���α׷��� ���� ������ ���� �ؽ����� ������
	���������� ȣ�� �������μ� �ܰ躰 ��Ż���� ��� ���� �ִ�.  Start�� ȣ�� �ϰ� �Ǹ� �����尡 ���� �Ǹ� ���ÿ� ������̴ٰ� 
	WriteToWiseLog �Լ��� ���� ť�� �̺�Ʈ�� �߻��ϸ� ���� ������� ������ �α׸� ó���ϰ� �ȴ�.

	exam : g_WiseLog.WriteToWiseLog("hackinistart.aspx"); �̷������� ȣ���ϸ� �ȴ�.
	
*/

#if defined(_KR)

class WiseLog
{
public:
	WiseLog();
	virtual ~WiseLog();

	/*
		desc  : ������ �α׸� �����Ѵ�.
		param : �ؽ������������� �α����� ���� ������ �ϰ� �Ǹ� oid �� �˼� �ִ� �� ���� ���� ���־�� �ؽ��ʿ��� ��踦 ���� �ִ�. 
				�ؽ� ������ ���� �ε����ΰ� ����..
	*/
	bool		Start(UINT uMyOid);
	void		Destroy();

	// �� �Լ��� �̿��ϸ� ������α� �����忡�� SendWebPost �Լ��� ���� ������α׸� �����.
	bool		WriteToWiseLog(const char* szLogFile);
	bool		Terminate( DWORD nExitCode );

	// �� �Լ��� �̿��ϸ� ������ �����忡�� ������α׸� �����. 
	// ���Լ��� �̿��ϸ� ������Ʈ ��Ȳ�� ���� �ణ�� ��ŷ�� �߻��Ҽ� �ִ�. 
	// �׷��� WriteToWiseLog �Լ��� �̿��ϵ��� �Ѵ�.
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