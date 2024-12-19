
#include "StdAfx.h"
#include "INIFILE.H"
#include "FILESET.H"


// IniFile

CIniFile::CIniFile()
{
	::memset(m_FileName, 0, sizeof(m_FileName));
	m_IsOpen = FALSE;
}

DWORD CIniFile::Open(LPCTSTR pFileName)
{
	if (!pFileName) 
		return (HASERROR+0);

	_tcsncpy_s(m_FileName, pFileName, MAX_PATH);

	m_IsOpen = TRUE;

	return NOERROR;
}

VOID CIniFile::Close()
{
	m_FileName[0] = _T('\0');
	m_IsOpen = FALSE;
}

BOOL CIniFile::Flush()
{
	return(::WritePrivateProfileString(NULL, NULL, NULL, m_FileName));
}

DWORD CIniFile::SetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPCTSTR pValue)
{
	if (!pKeyName || !pValueName || !pValue)
		return (HASERROR+0);
	

	BOOL aRetVal = ::WritePrivateProfileString(pKeyName, pValueName, pValue, m_FileName);
	if (!aRetVal) {
		return(::GetLastError());
	}

	return NOERROR;
}

DWORD CIniFile::SetValue(LPCTSTR pKeyName, LPCTSTR pValueName, INT pValue)
{
	if (!pKeyName || !pValueName) 
		return (HASERROR+0);

	TCHAR aValue[16] = { 0, };
	_itot(pValue, aValue, 10);

	BOOL aRetVal = ::WritePrivateProfileString(pKeyName, pValueName, aValue, m_FileName);
	if (!aRetVal)
		return(::GetLastError());

	return NOERROR;
}

DWORD CIniFile::SetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPVOID pValue, INT pSize)
{
	if (!pKeyName || !pValueName || !pValue || 0 >= pSize)
		return (HASERROR+0);

	BOOL aRetVal = ::WritePrivateProfileStruct(pKeyName, pValueName, pValue, pSize, m_FileName);
	if (!aRetVal)
		return(::GetLastError());
	return NOERROR;
}

DWORD CIniFile::GetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPTSTR pValue)
{
	if (!pKeyName || !pValueName || !pValue)
		return (HASERROR+0);
	
	::GetPrivateProfileString(pKeyName, pValueName, _T(""), pValue, 128, m_FileName);

	return NOERROR;
}

DWORD CIniFile::GetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPINT pValue)
{
	if (!pKeyName || !pValueName || !pValue)
		return (HASERROR+0);
	

	(*pValue) = ::GetPrivateProfileInt(pKeyName, pValueName, 0, m_FileName);

	return NOERROR;
}

DWORD CIniFile::GetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPINT pValue, INT pDefaultValue)
{

	if (!pKeyName || !pValueName || !pValue) {
		BASE_RETURN(HASERROR+0);
	}

	(*pValue) = ::GetPrivateProfileInt(pKeyName, pValueName, pDefaultValue, m_FileName);

	return NOERROR;
}

DWORD CIniFile::GetValue(LPCTSTR pKeyName, LPCTSTR pValueName, LPVOID pValue, INT pSize)
{
	if (!pKeyName || !pValueName || !pValue || 0 >= pSize)
		return (HASERROR+0);
	

	BOOL aRetVal = ::GetPrivateProfileStruct(pKeyName, pValueName, pValue, pSize, m_FileName);
	if (!aRetVal)
		return(HASERROR+1);			// ����� ������ ���а� ��ȯ��

	return NOERROR;
}

