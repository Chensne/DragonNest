#pragma once

class CFileChangeNotification
{
public:
	enum{
		eThread_State_Enabled = 0,		// �����尡 Ȱ��ȭ ���� �� ����
		eThread_State_Executing,		// �����尡 ���� �ϴ� ����
		eThread_State_Request_Destroy,	// ������ ���� ��û ����
		eThread_State_Enable_Destroy,	// �����尡 ���� �� �� �ִ� ����
	};

public:
	CFileChangeNotification();
	virtual ~CFileChangeNotification();

protected:
	static HANDLE	s_hThreadHandle;
	static int		s_nThreadStatus;
	UINT			m_dwThreadIndex;

	static std::wstring m_wszPath;
	static bool			m_bSubTree;
	static DWORD		m_FilterFlag;

	static UINT __stdcall ChangeNotification( void *pParam );

public:
	void StartFileChangeNotification( WCHAR * wszPath, bool bSubTree = false, DWORD FilterFlag = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS );
	void EndFileChangeNotification();
};