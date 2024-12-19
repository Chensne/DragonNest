#include "stdafx.h"
#include "DebugSet.h"
#include <atltime.h>
#include <strsafe.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

namespace CDebugSet
{
	#define MAX_STRLENGTH	(256)

	char m_szFullDir[MAX_PATH] = {0};
	bool m_bLogFileFinalOpen = false;
	bool m_bLogWrite(false);
	CTime m_LogTime;
	std::string m_strLogFile;
	std::string m_strLogName;
	std::string m_strSTARTTIME;

	void ToFile ( const char *szFileName, const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		//	Note	:	절대 경로 붙이기
		//
		char szFullPathFileName[256] = {0};

		StringCchCopy( szFullPathFileName, 256, m_szFullDir );
		StringCchCat( szFullPathFileName, 256, szFileName );

		FILE *fp;
		fp = fopen ( szFullPathFileName, "at" );
		if ( fp )
		{
			fprintf ( fp, "%s\n", szBuffer );
			fclose(fp);
		}
	}

	std::string GetCurTime ()
	{
		CTime Time = CTime::GetCurrentTime();
		char szTemp[MAX_STRLENGTH]={0};
		sprintf_s( szTemp, MAX_STRLENGTH, "%d-%02d-%02dT%02d:%02d:%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay(),
																		Time.GetHour(), Time.GetMinute(), Time.GetSecond() );

		return szTemp;
	}

	void ToFileWithTime ( const char* szFileName, const char* szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		//	Note	:	절대 경로 붙이기
		//
		char szFullPathFileName[256] = {0};

		StringCchCopy( szFullPathFileName, 256, m_szFullDir );
		StringCchCat( szFullPathFileName, 256, szFileName );

		FILE *fp;
		fp = fopen ( szFullPathFileName, "at" );
		if ( fp )
		{
			std::string strTIME = GetCurTime ();
			fprintf ( fp, "[%s]%s\n", strTIME.c_str(), szBuffer );
			fclose(fp);
		}
	}

	void ToLogFile ( const char *szFormat, ... )
	{//RLKT!!
//#if defined(_DEBUG)
		ASSERT(!m_strLogFile.empty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = {0};

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		FILE *fp;
		fp = fopen ( m_strLogFile.c_str(), "at" );
		if ( fp )
		{
			if ( !m_bLogWrite )
			{
				fprintf ( fp, "--------------------------------------------------------------\n" );
				fprintf ( fp, "\tDragon Nest Log Report\n" );
				fprintf ( fp, "--------------------------------------------------------------\n" );
				fprintf ( fp, "\n" );
				fprintf ( fp, "Execution time : %s\n\n", m_strSTARTTIME.c_str() );

				m_bLogWrite = true;
			}

			std::string strTIME = GetCurTime();
			fprintf ( fp, "[%s] %s\n", strTIME.c_str(), szBuffer );
			fclose(fp);
		}
//#endif
	}

	void ClearFile ( const char *szFileName )
	{
		char szFullPathFileName[256] = {0};
		StringCchCopy( szFullPathFileName, 256, m_szFullDir );
		StringCchCat( szFullPathFileName, 256, szFileName );
		DeleteFile ( szFullPathFileName );
	}

	const char* GetPath ()	
	{ 
		return m_szFullDir;
	}

	bool Initialize ( const char *szAppPath, bool bLogFileFinalOpen )
	{
		Finalize();

		TCHAR szFullPath[MAX_PATH] = {0};
		StringCchCopy( szFullPath, MAX_PATH, szAppPath );
		CreateDirectory ( szFullPath, NULL );

		StringCchCopy( m_szFullDir, MAX_PATH, szAppPath );
		CreateDirectory ( m_szFullDir, NULL );

		m_bLogFileFinalOpen = bLogFileFinalOpen;
		m_strSTARTTIME = GetCurTime();

		CTime cCurTime = CTime::GetCurrentTime();
		char strFileName[MAX_STRLENGTH] = {0};
		//sprintf_s( strFileName, MAX_STRLENGTH,  "report_%d%d%d%d%d%d.txt", cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(),
		//																cCurTime.GetHour(), cCurTime.GetMinute(), cCurTime.GetSecond() );
		sprintf_s( strFileName, MAX_STRLENGTH,  "report_dragonnest.txt" );

		m_strLogName = strFileName;
		m_strLogFile = m_szFullDir;
		m_strLogFile += "\\" + m_strLogName;

		m_bLogWrite = false;
		return true;
	}

	bool Finalize()
	{
		//if ( m_bLogWrite && m_bLogFileFinalOpen )
		//{
		//	char strPROCESS[MAX_STRLENGTH] = {0};
		//	sprintf_s( strPROCESS, MAX_STRLENGTH, "notepad %s", m_strLogName.c_str() );

		//	STARTUPINFO si;
		//	memset ( &si, 0, sizeof ( STARTUPINFO ) );
		//	si.cb = sizeof ( STARTUPINFO );
		//	si.dwFlags = 0;

		//	PROCESS_INFORMATION pi;
		//	pi.hProcess = NULL;
		//	pi.hThread = NULL;
		//	CreateProcess ( NULL, strPROCESS, NULL, NULL, FALSE, 0, NULL, m_szFullDir, &si, &pi );
		//	CloseHandle( pi.hProcess );
		//	CloseHandle( pi.hThread );
		//}

		return true;
	}
};