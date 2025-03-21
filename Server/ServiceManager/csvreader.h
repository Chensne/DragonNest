
#pragma once

#include <tchar.h>

class	CCSVReader
{
public :
	CCSVReader();
	~CCSVReader();

	bool Load(const TCHAR *fname);
	int CountLine() { return m_iLine; }

	int GetData(int col, int row, TCHAR * outptr, int outlen) ;
	int Scan(int col, int row, TCHAR * form, ...);

private :
	TCHAR * m_pBuffer;
	int * m_pOffset;
	int m_iLine;

	int CountLine(const TCHAR *buffer, int * offset);
	int PassToken(const TCHAR *str);
	void Clear();
	TCHAR * ReadFile(const TCHAR *fname);
} ;
