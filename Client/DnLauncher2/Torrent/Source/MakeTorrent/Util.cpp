#include "stdafx.h"
#include "Util.h"
#include "WLibTorrent.h"

void TokenizeW( const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters /* = L" "*/ )
{
	// 맨 첫 글자가 구분자인 경우 무시
	std::wstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	// 구분자가 아닌 첫 글자를 찾는다
	std::wstring::size_type pos = str.find_first_of( delimiters, lastPos );

	while( std::wstring::npos != pos || std::wstring::npos != lastPos )
	{
		// token을 찾았으니 vector에 추가한다
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str.find_first_not_of( delimiters, pos );
		// 다음 구분자가 아닌 글자를 찾는다
		pos = str.find_first_of( delimiters, lastPos );
	}
}

BOOL MakeTorrent( LPCWSTR strFullFilePath, LPCWSTR strWebSeeds, LPCWSTR strTrackers )
{
	std::wstring strFullFileName = strFullFilePath;
	std::wstring strFileName = strFullFileName;
	int nSlashIndex = strFileName.rfind( L"\\" );
	strFileName.erase( 0, nSlashIndex + 1 );
	std::wstring strMakeFileName;
	strMakeFileName = strFullFileName + L".torrent";

	std::vector<std::wstring> vecWstrWebSeeds;
	std::vector<std::wstring> vecWstrTrackers;

	std::vector<std::wstring> tokens;
	TokenizeW( strWebSeeds, tokens, L";" );
	for( int i=0; i<static_cast<int>( tokens.size() ); i++ )
	{
		std::wstring strWebSeed;
		strWebSeed = tokens[i];
		nSlashIndex = strWebSeed.rfind( L"/" );
		if( nSlashIndex != strWebSeed.length() - 1 )
			strWebSeed += L"/";
		strWebSeed += strFileName;
		vecWstrWebSeeds.push_back( strWebSeed );
	}
	tokens.clear();

	TokenizeW( strTrackers, tokens, L";" );
	for( int i=0; i<static_cast<int>( tokens.size() ); i++ )
	{
		vecWstrTrackers.push_back( tokens[i] );
	}
	tokens.clear();

	BOOL bMake = WLibTorrent::GetInstance()->MakeTorrent( strFullFileName.c_str(), vecWstrWebSeeds, vecWstrTrackers, strMakeFileName.c_str(), L"DN" );

	vecWstrWebSeeds.clear();
	vecWstrTrackers.clear();

	WLibTorrent::DestroyInstance();

	return bMake;
}