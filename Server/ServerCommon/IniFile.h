#pragma once

class CIniFile
{
public:
	CIniFile(void);
	virtual ~CIniFile(void);

private:
	TCHAR	m_wszFileName[MAX_PATH];

public:
	BOOL	Open(LPCTSTR fileName);
	BOOL	Close(void);

	BOOL	SetValue(LPCTSTR keyName, LPCTSTR valueName, LPCTSTR value);
	BOOL	SetValue(LPCTSTR keyName, LPCTSTR valueName, int value);

	BOOL	GetValue(LPCTSTR keyName, LPCTSTR valueName, LPTSTR value, int nlength = 128);
	BOOL	GetValue(LPCTSTR keyName, LPCTSTR valueName, LPINT value);
	BOOL	GetValue(LPCTSTR keyName, LPCTSTR valueName, LPINT value, int defaultValue);
};


extern CIniFile g_IniFile;