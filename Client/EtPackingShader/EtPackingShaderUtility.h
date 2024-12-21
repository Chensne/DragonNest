#pragma once

std::vector<std::string> GetShaderFileNameList( CString szPath );
std::vector< D3DXMACRO > GetStringToMacro(CString str );
LONG GetRegistryString(HKEY hKey, char *pSubKey, char *pValueName, char *pValue, int nMaxLength );
LONG SetRegistryString(HKEY hKey, char *pSubKey, char *pValueName, char *pValue );
