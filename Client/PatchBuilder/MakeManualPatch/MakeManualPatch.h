// MakeManualPatch.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMakeManualPatchApp:
// �� Ŭ������ ������ ���ؼ��� MakeManualPatch.cpp�� �����Ͻʽÿ�.
//

class CMakeManualPatchApp : public CWinApp
{
public:
	CMakeManualPatchApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMakeManualPatchApp theApp;