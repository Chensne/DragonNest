// EtActionTool.h : EtActionTool ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CEtActionToolApp:
// �� Ŭ������ ������ ���ؼ��� EtActionTool.cpp�� �����Ͻʽÿ�.
//

class CEtActionToolApp : public CWinApp
{
public:
	CEtActionToolApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CEtActionToolApp theApp;
