// DnResourceEditer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CDnResourceEditerApp:
// �� Ŭ������ ������ ���ؼ��� DnResourceEditer.cpp�� �����Ͻʽÿ�.
//

class CDnResourceEditerApp : public CWinApp
{
public:
	CDnResourceEditerApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CDnResourceEditerApp theApp;