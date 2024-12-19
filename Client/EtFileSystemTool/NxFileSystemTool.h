#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"		// 주 기호


// CNxFileSystemToolApp:
// 이 클래스의 구현에 대해서는 NxFileSystemTool.cpp을 참조하십시오.
//

class CNxFileSystemToolApp : public CWinApp
{
public:
	CNxFileSystemToolApp();

// 재정의
	public:
	virtual BOOL InitInstance();

	// 용량별 패킹
	BOOL	_CapaPacking(std::vector<std::string>& tokens);
	// 폴더로 패킹
	BOOL	_FolderPacking(std::vector<std::string>& tokens);

// 구현

	DECLARE_MESSAGE_MAP()
	
};

extern CNxFileSystemToolApp theApp;


		