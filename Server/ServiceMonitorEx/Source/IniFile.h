

// P.S.> 파일 이름에 세부 경로가 없으면 윈도우 디렉토리 (ex> C:\Windows) 에 파일이 생성되니 주의할 것 !!!


#pragma once


#include "SYSBASEBASE.H"


class CIniFile
{
public:
	CIniFile();

	DWORD Open(LPCTSTR pFileName);
	VOID Close();
	BOOL IsOpen() const { return(m_IsOpen); }

	BOOL Flush();

	DWORD SetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPCTSTR pValue);
	DWORD SetValue(LPCTSTR pKeyName, LPCTSTR pValueName, INT pValue);
	DWORD SetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPVOID pValue, INT pSize);

	DWORD GetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPTSTR pValue);
	DWORD GetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPINT pValue);
	DWORD GetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPINT pValue, INT pDefaultValue);
	DWORD GetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPVOID pValue, INT pSize);

private:
	TCHAR m_FileName[MAX_PATH];
	BOOL m_IsOpen;
};
