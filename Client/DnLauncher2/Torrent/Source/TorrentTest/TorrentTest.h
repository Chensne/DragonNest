// TorrentTest.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CTorrentTestApp:
// �� Ŭ������ ������ ���ؼ��� TorrentTest.cpp�� �����Ͻʽÿ�.
//

class CTorrentTestApp : public CWinApp
{
public:
	CTorrentTestApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CTorrentTestApp theApp;