// EtPackingShader.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CEtPackingShaderApp:
// �� Ŭ������ ������ ���ؼ��� EtPackingShader.cpp�� �����Ͻʽÿ�.
//

class CEtPackingShaderApp : public CWinApp
{
public:
	CEtPackingShaderApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CEtPackingShaderApp theApp;