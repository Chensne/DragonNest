#pragma once

extern std::vector< std::string > g_vecResourceDir;

EtResourceHandle LoadResource( const char *pFileName, ResourceType Type, bool bLoadImmediate = false );
