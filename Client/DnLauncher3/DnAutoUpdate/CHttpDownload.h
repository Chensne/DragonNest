//////////////////////////////////////////////////////////////////////////
// Http를 사용한 이어받기 클래스
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <afxsock.h>
#include <afxinet.h>

class CHttpDownload
{
public:
	CHttpDownload();
	virtual ~CHttpDownload();

	enum { HTTPDOWNLOAD_SUCCESS = 0, HTTPDOWNLOAD_ERROR, HTTPDOWNLOAD_ALREADY_DOWNLOAD, HTTPDOWNLOAD_TERMINATE };
public:
	int DownloadFile( HINTERNET hInternetSession, CString strHttpPath, CString strLocalPath );
	void SetExit( BOOL bExit ) { m_bExit = bExit; }
	BOOL IsExit() { return m_bExit; }

	DWORD GetTotalFileSize() { return m_dwTotalFileLength; }
	DWORD GetTotalDownloadBytes() { return m_dwTotalBytes; }
	float GetDownloadRate() { return m_fRate; }
	float GetDownloadPercent() { return m_fPercent; }

	void Reset();

private:
	DWORD GetDownloadFileSize( HINTERNET hInternetSession, CString strHttpPath );
	BOOL GetMD5Checksum();

private:
	HINTERNET m_hHttpConnection;	// Http Connection Handle

	DWORD m_dwTempFileLength;		// 다운 받은 파일의 양
	DWORD m_dwDownloadFileLength;	// 다운 받아야할 파일의 양
	DWORD m_dwTotalFileLength;		// 총 파일의 양
	DWORD m_dwTotalBytes;			// 총 받은 파일의 양

	CString m_strHttpPath;			// Http 파일 주소
	CString m_strLocalPath;			// 로컬의 파일 위치

	float m_fRate;					// Download Rate
	float m_fPercent;				// Download Percent

	BOOL m_bExit;					// Exit Flag
};