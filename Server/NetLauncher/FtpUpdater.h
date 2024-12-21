
#pragma once

#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <vector>
#include <string>
#include "stdafx.h"

class	CFtpUpdater
{
public :
	CFtpUpdater(bool bIsPassive);
	~CFtpUpdater();

	bool  Connect(const TCHAR *ip, int port, const TCHAR *userid, const TCHAR *password);
	bool  SetDirectory(const TCHAR * szPath);
	
	bool  DownloadFile(const TCHAR * szFilename, const TCHAR *szStoreFile);
	bool  DownloadAll(const TCHAR * szDest);

private :
	void Disconnect();
	void EnumFileList(const TCHAR *path, std::vector <std_str> &list);

	volatile bool m_bIsPassiveMode;
	HINTERNET m_hSession;
	HINTERNET m_hConnection;
} ;