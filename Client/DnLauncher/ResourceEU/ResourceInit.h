#pragma once

#include <afxwin.h>         // MFC core and standard components

void InitCtrlRect();
void GetCtrlRect( CRect* pRect, int nMaxNum );
int GetLanguageResourceIndexOffset( CString strLanguageParam );
void ResetInitCtrlRect();
