#include "stdafx.h"
#if defined(PRE_UNZIP_CHANGE)
#include "ZipArchive.h"
#include "LauncherSession.h"
#include "UnZipProcess.h"

extern void TextOut(const TCHAR * format, ...);

unsigned int WINAPI _RunThread(void* pData)
{
	CUnZipProcess* pZip = static_cast<CUnZipProcess*>(pData);
	pZip->WorkerThread(pZip->GetThreadID());
	return 0;
}

CUnZipProcess::CUnZipProcess(CLauncherSession* pSession, LPCTSTR szFilePath, LPCTSTR szFileName)
{
	m_pLauncherSession = pSession;
	_tcscpy_s(m_szOutputFolder, szFilePath);
	_tcscpy_s(m_szFileName, szFileName);

	m_nThreadCount = 0;
	m_nMaxZipFileCount = 0;
	m_nUnzipCount = 0;
	//압축 작업용 쓰레드 갯수(CPU쓰레드 갯수 /2 + 1(최대 MAX_ZIP_THREAD, 최소 2)
	_SYSTEM_INFO _Info;
	GetSystemInfo(&_Info);
	m_nMaxThreadCount = max(_Info.dwNumberOfProcessors/2 + 1, 2);
	//너무 많으면 조절해준다
	m_nMaxThreadCount = min(m_nMaxThreadCount, MAX_ZIP_THREAD);

	for(int i = 0; i < m_nMaxThreadCount; i++)
		m_hZipEndEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CUnZipProcess::~CUnZipProcess()
{
	for(int i = 0; i < m_nMaxThreadCount; i++)
		CloseHandle(m_hZipEndEvent[i]);
	
	m_pLauncherSession = NULL;
	m_nMaxZipFileCount = 0;
	m_nMaxThreadCount = 0;
	m_nThreadCount = 0;

	if(!m_ZipLength.empty())
	{
		for(int i = 0; i < (int)m_ZipLength.size();i++)
			SAFE_DELETE(m_ZipLength[i]);
		m_ZipLength.clear();
	}

	m_Zip.Close();
}

bool CUnZipProcess::OpenZip()
{
	m_Zip.Open(m_szFileName, CZipArchive::zipOpenReadOnly);

	return true;
}

bool CUnZipProcess::UnZip()
{
	if (m_Zip.GetCount() == 0)
	{
		TextOut(_T("Unzip File Counting Failed"));
		return false;
	}

	m_nMaxZipFileCount = m_Zip.GetCount();

	//미리 쓰레드가 처리할 값들을 만들어준다
	sZipLength* sZip;
	int nCur = 0;
	int nStart = 0;
	int nEnd = 0;

	InitializeCriticalSection(&m_CriticalSection);
	//한 쓰레드에 할당할 파일의 갯수 계산
	int nTerm = MAX_UNZIP_COUNT_IN_THREAD;
	if(m_nMaxZipFileCount < (m_nMaxThreadCount * MAX_UNZIP_COUNT_IN_THREAD))
		nTerm = m_nMaxZipFileCount / m_nMaxThreadCount;
	
	while(1)
	{
		for(int k = 0; k < m_nMaxThreadCount;k++)
		{
			sZip = (sZipLength*)malloc(sizeof(sZipLength));
			nStart = (nCur+k) * nTerm;
			nEnd = nStart + nTerm;

			if(nStart >= m_nMaxZipFileCount)
				break;
			if(nEnd > m_nMaxZipFileCount)
				nEnd = m_nMaxZipFileCount;

			sZip->nStart = nStart;
			sZip->nEnd = nEnd;
			
			m_ZipLength.push_back(sZip);
		}

		if(nStart >= m_nMaxZipFileCount)
			break;
		nCur = nCur + m_nMaxThreadCount;
	}

	UINT tId;
	for(int i = 0; i < m_nMaxThreadCount; i++)
	{
		_beginthreadex(NULL, NULL, _RunThread, this, NULL, &tId);
	}

	WaitForMultipleObjects(m_nMaxThreadCount, m_hZipEndEvent, TRUE, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);

	//파일 갯수, 크기 체크
	CZipFileHeader fh;
	TCHAR _FilePath[MAX_PATH];
	bool IsOk = true;
	std::vector<int> _vecRetryZip;
	TextOut(_T("UnZip Success, Check Files"));
	for(int i = 0;i < m_nMaxZipFileCount; i++)
	{
		m_Zip.GetFileInfo(fh, i);
		//파일 타입이 디렉토리인 경우 스킵
		if(fh.GetOriginalAttributes() & 0x10)
			continue;

		//실제 폴더에서 파일을 찾아본다
		_sntprintf_s(_FilePath, MAX_PATH, _T("%s\\%s"), m_szOutputFolder, (LPCTSTR)fh.GetFileName());
		HANDLE hFile = CreateFile(_FilePath,    // file to open
			GENERIC_READ,          // open for reading
			FILE_SHARE_READ,       // share for reading
			NULL,                  // default security
			OPEN_EXISTING,         // existing file only
			FILE_ATTRIBUTE_NORMAL, // normal file
			NULL);                 // no attr. template
		
		if (hFile == INVALID_HANDLE_VALUE) 
		{ 
			IsOk = false;
			_vecRetryZip.push_back(i);
			continue;
		}

		DWORD dwSize = GetFileSize(hFile, NULL) ; 
		// 크기 확인
		if (dwSize == 0xFFFFFFFF || dwSize != fh.m_uUncomprSize) {
			_vecRetryZip.push_back(i);
			IsOk = false;
		}
		CloseHandle(hFile);
	}
	// 실패한 경우 재시도 해준다
	if(!IsOk)
	{
		TextOut(_T("Retry Unzip(Count : %d)"), _vecRetryZip.size());
		for(int i = 0; i< (int)_vecRetryZip.size(); i++)
		{
			try
			{
				IsOk = m_Zip.ExtractFile(_vecRetryZip[i], m_szOutputFolder, true);
			}
			catch(...)
			{
				IsOk = false;
			}

			if(!IsOk)
			{
				m_Zip.GetFileInfo(fh, i);
				TextOut(_T("%s UnZip Failed"), (LPCTSTR)fh.GetFileName());
				break;
			}
		}
	}
	return IsOk;
}

void CUnZipProcess::WorkerThread(int nThreadID)
{
	bool IsSuccess;
	int nRetry = 0;

	CZipArchive zipInThread;
	zipInThread.OpenFrom(m_Zip);
	sZipLength* sZip;

	while(1)
	{
		// 쓰레드들이 작업할 범위를 매번 가져오는 형식
		// 처리할 범위를 미리 지정할 경우, 나중에 먼저 끝나고 종료되는 쓰레드가 존재하기 때문
		sZip = GetZipLength();
		if(sZip == NULL)
			break;

		for(ZIP_INDEX_TYPE i = sZip->nStart; i < sZip->nEnd;i++)
		{
			nRetry = 0;
			while(1)
			{
				try
				{
					IsSuccess = zipInThread.ExtractFile(i, m_szOutputFolder, true);
				}
				catch(...)
				{
					IsSuccess = false;
				}

				if(IsSuccess)
				{ 
					if(m_pLauncherSession)
					{
						CZipFileHeader fh;
						zipInThread.GetFileInfo(fh, i);
						InterlockedIncrement(&m_nUnzipCount);
#if defined(UNICODE)
						m_pLauncherSession->OnUnzip(fh.GetFileName(), m_nUnzipCount, m_nMaxZipFileCount);
#else
						WCHAR filename[MAX_PATH];
						MultiByteToWideChar(CP_ACP, 0, fh.GetFileName(), -1, filename, MAX_PATH);
						m_pLauncherSession->OnUnzip(filename, m_nUnzipCount, m_nMaxZipFileCount);
#endif
					}
					break;
				}
				nRetry++;
				//압축 풀기에 실패한 경우 20회까지 재시도함
				if(nRetry > 20)
					break;
				else
					Sleep(50);
				
			}	
		}
		SAFE_DELETE(sZip);
	}

	zipInThread.Close();

	SetEvent(m_hZipEndEvent[nThreadID]);
	_endthreadex(0);
}
#endif	// #if defined(PRE_UNZIP_CHANGE)