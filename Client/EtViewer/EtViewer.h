// EtViewer.h : EtViewer ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.

class CNXNIntegrationService;

// CEtViewerApp:
// �� Ŭ������ ������ ���ؼ��� EtViewer.cpp�� �����Ͻʽÿ�.
//

class CEtViewerApp : public CWinApp
{
public:
	CEtViewerApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

protected:

public:
// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileOpenAB();

public:
	virtual BOOL OnIdle(LONG lCount);
};

extern CEtViewerApp theApp;
