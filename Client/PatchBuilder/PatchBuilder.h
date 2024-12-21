#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include "RegProfile.h"


class CPatchBuilderApp : public CWinApp
{
public:
	CPatchBuilderApp();
	~CPatchBuilderApp();

protected:
	CRegProfile m_RegProfile;

public:
	virtual BOOL InitInstance();
	void CheckPatchFolder();

	DECLARE_MESSAGE_MAP()

private:
	void UseLogFile();
};

extern CPatchBuilderApp theApp;