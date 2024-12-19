#include "stdafx.h"
#include "CSVWriter.h"
#include "StringUtil.h"


// 초반 네이밍 칼럼 설정
// 칼럼 추가는 AddColoum 으로 설정
// 필드 추가는 AddField로 설정


CSVWriter::CSVWriter()
{
	m_stream = NULL;
	m_bEnableWrite = false;
}

CSVWriter::~CSVWriter()
{
}

bool CSVWriter::Initialize( const wchar_t *FileName , std::wstring strColumname )
{
#ifndef _FINAL_BUILD
	_wsetlocale(LC_ALL,L"kor"); // 로케일 지정을 해주어야 국가별 언어가 정상적으로 표현됩니다.
#endif

	m_strFileName = FileName;
	m_strColumname = strColumname;

	return CheckExistFile();
}

bool CSVWriter::CheckExistFile()
{
	if( _wfopen_s( &m_stream, m_strFileName.c_str(), L"r" ) != 0 )
	{
		if( _wfopen_s( &m_stream, m_strFileName.c_str(), L"w" ) != 0 )
		{
			return false;
		}
		else
		{
			fclose(m_stream);
			WriteColumnName();
		}
	}
	else
	{
		fclose(m_stream);
	}

	return true;
}

void CSVWriter::WriteColumnName()
{
	StartWrite();
	std::vector<std::wstring> tokens;
	TokenizeW( m_strColumname , tokens, L";" );
	for ( int j = 0 ; j < (int)tokens.size() ; j++ )
		AddColumn(tokens[j].c_str());
	AddField();
	EndWrite();
}

void CSVWriter::StartWrite()
{
	if( _wfopen_s( &m_stream, m_strFileName.c_str(), L"a" ) != 0 )
		return;

	m_bEnableWrite = true;
}
void CSVWriter::EndWrite()
{
	if(!m_bEnableWrite)
		return;

	m_bEnableWrite = false;
	fclose(m_stream);
}


void CSVWriter::AddColumn(const wchar_t *str)
{
	if(m_bEnableWrite)
		fwprintf_s( m_stream, L"%s,", str );
}



void CSVWriter::AddField( int nCount )
{
	if(m_bEnableWrite)
		for( int i = 0; i < nCount; i ++ )
			fwprintf_s( m_stream, L"\n" );
}


