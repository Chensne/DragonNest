// NetTest.h : NetTest ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CNetTestApp:
// �� Ŭ������ ������ ���ؼ��� NetTest.cpp�� �����Ͻʽÿ�.
//

class CNetTestApp : public CWinApp
{
public:
	CNetTestApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CNetTestApp theApp;