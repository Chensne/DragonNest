#pragma once

#if defined(PRE_UNZIP_CHANGE)
#define MAX_ZIP_THREAD 8 //UnZip 작업을 할 쓰레드 최대 갯수(일정 갯수 넘어가면 속도 차이가 없음)
#define MAX_UNZIP_COUNT_IN_THREAD 150	//쓰레드당 처리할 갯수

class CLauncherSession;
class CZipArchive;

class CUnZipProcess  
{
public:
	CUnZipProcess(CLauncherSession* pSession, LPCTSTR szFilePath, LPCTSTR szFileName);
	~CUnZipProcess();
	
	int GetThreadID(){ return m_nThreadCount++;}
	bool OpenZip();
	bool UnZip();
	void WorkerThread(int nThreadID);

private:
	struct sZipLength
	{
		int nStart;
		int nEnd;
	};
	std::vector<sZipLength*> m_ZipLength;
	CLauncherSession* m_pLauncherSession;

	CZipArchive m_Zip;
	CRITICAL_SECTION m_CriticalSection;

	TCHAR m_szOutputFolder[MAX_PATH];
	TCHAR m_szFileName[MAX_PATH];

	int m_nThreadCount;
	long m_nUnzipCount;

	int m_nMaxThreadCount;
	int m_nMaxZipFileCount;
	
	HANDLE m_hZipEndEvent[MAX_ZIP_THREAD];

	sZipLength* GetZipLength()
	{
		EnterCriticalSection(&m_CriticalSection);
		if(m_ZipLength.empty())
		{
			LeaveCriticalSection(&m_CriticalSection);
			return NULL;
		}
		sZipLength* sLength = m_ZipLength.back();
		m_ZipLength.pop_back();
		LeaveCriticalSection(&m_CriticalSection);
		return sLength;
	}
};

#endif	// #if defined(PRE_UNZIP_CHANGE)