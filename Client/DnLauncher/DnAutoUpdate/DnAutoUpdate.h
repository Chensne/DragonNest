// DnAutoUpdate.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CDnAutoUpdateApp:
// �� Ŭ������ ������ ���ؼ��� DnAutoUpdate.cpp�� �����Ͻʽÿ�.
//

class CDnAutoUpdateApp : public CWinApp
{
public:
	CDnAutoUpdateApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CDnAutoUpdateApp theApp;