#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
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