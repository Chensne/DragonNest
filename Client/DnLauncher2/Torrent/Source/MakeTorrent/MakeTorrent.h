// MakeTorrent.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMakeTorrentApp:
// �� Ŭ������ ������ ���ؼ��� MakeTorrent.cpp�� �����Ͻʽÿ�.
//

class CMakeTorrentApp : public CWinApp
{
public:
	CMakeTorrentApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()

protected:
	void LoadConfig();

protected:
	std::wstring m_strMakeFileName;
	std::wstring m_strWebSeeds;
	std::wstring m_strTrackers;
};

extern CMakeTorrentApp theApp;