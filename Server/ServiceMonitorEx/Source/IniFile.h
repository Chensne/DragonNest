

// P.S.> ���� �̸��� ���� ��ΰ� ������ ������ ���丮 (ex> C:\Windows) �� ������ �����Ǵ� ������ �� !!!


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
