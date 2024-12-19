#pragma once
#include "locale.h"

// Rotha 
// Microsoft Excel : CSV 출력 유틸입니다 , 데이터를 뽑아 확인하는 용도로 제작합니다.
// 나중에 필요한것 있으시면 추가하시거나 가져다 쓰시면됩니다.

class CSVWriter
{
public:
	CSVWriter();
	~CSVWriter();

protected:
	FILE *m_stream;
	std::wstring m_strFileName;
	std::wstring m_strColumname;

	bool m_bEnableWrite;
	
public:
	bool Initialize( const wchar_t *FileName , std::wstring strColumname );
	bool CheckExistFile();
	void WriteColumnName();

	void AddColumn(const wchar_t *str);
	void AddField( int nCount = 1);

	void StartWrite();
	void EndWrite();

	FILE* GetStream() { return m_stream; } 
};