#include "StdAfx.h"
#include "GlobalValue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGlobalValue g_GlobalValue;

CGlobalValue::CGlobalValue(void)
{
	m_pCurView = NULL;
}

CGlobalValue::~CGlobalValue(void)
{
}
