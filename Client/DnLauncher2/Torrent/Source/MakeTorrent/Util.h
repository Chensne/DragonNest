#pragma once

void TokenizeW( const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters = L" " );
BOOL MakeTorrent( LPCWSTR strFullFilePath, LPCWSTR strWebSeeds, LPCWSTR strTrackers );