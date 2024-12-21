
#include "stdafx.h"
#include "ftpupdater.h"
#include <stdio.h>
#include <direct.h>


CFtpUpdater::CFtpUpdater(bool bIsPassive)
{
	m_hSession = NULL;
	m_hConnection = NULL;
	m_bIsPassiveMode = bIsPassive;
}


CFtpUpdater::~CFtpUpdater()
{
	Disconnect();
}


bool CFtpUpdater::Connect(const TCHAR *ip, int port, const TCHAR *userid, const TCHAR *password)
{
	Disconnect();

	m_hSession = InternetOpen(_T("Microsoft Internet Explorer"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	if (m_bIsPassiveMode)
		m_hConnection = InternetConnect(m_hSession, ip, port, userid, password, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	else
		m_hConnection = InternetConnect(m_hSession, ip, port, userid, password, INTERNET_SERVICE_FTP, 0, 0);
	return m_hConnection != NULL ? true : false;
}


bool CFtpUpdater::SetDirectory(const TCHAR * szPath)
{
	bool check = false;
	if (m_hConnection)
		check = FtpSetCurrentDirectory(m_hConnection, szPath) == TRUE ? true : false;
	return check;
}


void CFtpUpdater::Disconnect()
{
	if (m_hConnection)
		InternetCloseHandle(m_hConnection);
	if (m_hSession)
		InternetCloseHandle(m_hSession);

	m_hConnection = 0;
	m_hSession = 0;
}


bool CFtpUpdater::DownloadAll(const TCHAR * szDest)
{
	_tmkdir(_T("./PatchFiles"));
	TCHAR szCreateFolder[256];
	_stprintf_s(szCreateFolder, _T("./PatchFiles%s"), szDest);

	for (int j = 0; (j < sizeof(szCreateFolder)/sizeof(*szCreateFolder)) && szCreateFolder[j] != NULL; j++)
		if (szCreateFolder[j] == '\\')
			szCreateFolder[j] = '/';

	_tcscpy(_tcsrchr(szCreateFolder, '/'), _T(""));
	_tmkdir(szCreateFolder);

	std::vector <std_str> list;
	unsigned int i;
	EnumFileList(_T("."), list);
	for(i=0; i<list.size(); i++)
	{
		bool bMakeFolder = false;
		const TCHAR * fname = list[i].c_str();
		TCHAR szFolder[256], szFileName[256], szPath[256], szTemp[256];
		memset(szTemp, 0, sizeof(szTemp));

		_tcscpy_s(szFolder, fname);
		_tcscpy_s(szFileName, _tcsrchr(szFolder, '/'));
		_tcscpy(_tcsrchr(szFolder, '/'), _T(""));
		
		if (_tcscmp(szFolder, _T(".")))
			bMakeFolder = true;

		for (int h = 0, nCnt = 0; h < (sizeof(szTemp)/sizeof(*szTemp)) && szFolder[h] != NULL; h++)
		{
			if (szFolder[h] != '.')
			{
				szTemp[nCnt] = szFolder[h];
				nCnt++;
			}
		}
		
		_stprintf(szPath, _T("%s%s"), szCreateFolder, szTemp);
		if (bMakeFolder)
		{
			_tmkdir(szPath);
			_stprintf(szPath, _T("%s%s"), szPath, szFileName);
		}

		DeleteFile(szPath);	// 파일속성이 Read Only 경우에는 지워지지 않음
		FtpGetFile(m_hConnection, fname, szPath, FALSE, 0, 0, 0);	
	}
	return true;
}


void CFtpUpdater::EnumFileList(const TCHAR *path, std::vector <std_str> &list)
{
	HINTERNET hRemoteFile;
	WIN32_FIND_DATA fd;
	std::vector <std_str> folderlist;
	unsigned int i;
	TCHAR szFilter[256];
	TCHAR szPath[128];

	_stprintf_s(szFilter, _T("%s/*.*"), path);

	hRemoteFile = FtpFindFirstFile(m_hConnection, szFilter, &fd, 0, 0);
	if (hRemoteFile != NULL)
	{
		do {
			_stprintf_s(szPath, _T("%s/%s"), path, fd.cFileName);
			if (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (m_bIsPassiveMode)
				{
					if (_tcsstr(szPath, _T("/.")))
						continue;
				}

				folderlist.push_back(szPath);
			}	else
			{
				list.push_back(szPath);
			}
		}	while(InternetFindNextFile(hRemoteFile, &fd) == TRUE);

		InternetCloseHandle(hRemoteFile);
	}

	for(i=0; i<folderlist.size(); i++)
	{
		EnumFileList(folderlist[i].c_str(), list);
	}
}


bool CFtpUpdater::DownloadFile(const TCHAR * szFilename, const TCHAR *szStoreFile)
{
	HINTERNET hRemoteFile;
	DWORD readbytes, offset=0;
	BOOL hr;
	FILE *fp;

	hRemoteFile = FtpOpenFile(m_hConnection, szFilename, GENERIC_READ,
							  FTP_TRANSFER_TYPE_BINARY|INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_RESYNCHRONIZE, NULL);

	if (hRemoteFile == NULL)
		return false;

	if (_tfopen_s((&(fp)), szStoreFile, _T("wb")) == 0)
		return false;

	if (fp == NULL)
	{
		InternetCloseHandle(hRemoteFile);
		return false;
	}

	do {
		char buffer[16*1024];
		hr = InternetReadFile(hRemoteFile, buffer, sizeof(buffer), &readbytes);

		if (hr == TRUE)
		{
			if (readbytes > 0)
				fwrite(buffer, readbytes, 1, fp);
			else
				break;
		}
	}	while(1);

	fclose(fp);

	InternetCloseHandle(hRemoteFile); // 경고 : 이 함수에서 리턴이 안되는 경우가 있다.
	return true;
}