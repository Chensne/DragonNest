#pragma once

#include "MultiCommon.h"
extern std::vector< std::string > g_vecResourceDir;

EtResourceHandle LoadResource( CMultiRoom *pRoom, const char *pFileName, ResourceType Type );