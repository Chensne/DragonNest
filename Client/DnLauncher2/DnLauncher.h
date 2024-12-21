// DnLauncher.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
class CDnLauncherDlg;

// CDnLauncherApp:
// See DnLauncher.cpp for the implementation of this class
//
class CDnLauncherApp : public CWinApp
{
public:
	CDnLauncherApp();
	~CDnLauncherApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

public:
	BOOL InitLauncher();			// ��ó �ʱ�ȭ
	BOOL InitFail();				// �ʱ�ȭ ���н� �����޼��� �� �α� ���
	BOOL CheckDirectXVersion();		// DirectX ���� üũ
	void DeleteOldLogFile();		// ������ �α����� ����
	void UseLogFile();				// �α����� ��� �� ��� �ʱ�ȭ
	void UseLogWindow();			// �α������� ��� �� ��� �ʱ�ȭ
	BOOL RemoveUsedFile();			// ��ó ���� �� ��� �ߴ� ���� �� ��ó ������Ʈ �� ������ tmp ���� ����
	void ParseCommandLine();		// Ŀ�ǵ� ���� �Ľ�
	
#ifdef _FIRST_PATCH
	BOOL InitFirstPatchThread();	// FirstPatch ������ ����.
	void TerminateFirstPatchThread();	// FirstPatch ������ ����.
#endif
};

extern CDnLauncherApp theApp;

enum emInitErrorCode
{
	INIT_ERROR_CLIENT_ALEADY_RUN = 1,
	INIT_ERROR_DX_VER,
	INIT_ERROR_SERVICE_MODULE,
	INIT_ERROR_REMOVE_FILE,
	INIT_ERROR_PATCHCONFIGLIST_DOWNLOAD,
	INIT_ERROR_PATCHCONFIGLIST_PARSING,
	INIT_ERROR_INTERFACE,
	INIT_ERROR_WEBPAGE,
	INIT_ERROR_SET_PATCHINFO,
	INIT_ERROR_SET_PARAMETER,
	INIT_ERROR_COMMAND_LINE,
#ifdef _USE_BITTORRENT
	INIT_ERROR_BITTORRENT_SESSION,
#endif // _USE_BITTORRENT
};