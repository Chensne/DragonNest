#pragma once
#include "DnTextBoxDlg.h"
#include "DnCommDlgMng.h"

class CDnTextBoxDlgMng : public CDnCommDlgMng<CDnTextBoxDlg>
{
public:
	CDnTextBoxDlgMng(void);
	virtual ~CDnTextBoxDlgMng(void);

public:
	DWORD ShowTextWindow( LPCWSTR wszStr, float fX, float fY, float fWidth, float fDelayTime, DWORD dwSetID = 0, CEtUICallback *pCall = NULL );
	DWORD ShowTextWindow( LPCWSTR wszStr, float fX, float fY, int nPos, int nFormat, float fDelayTime, DWORD dwSetID = 0, CEtUICallback *pCall = NULL );
	DWORD ShowCountDownWindow( int nCountDown, float fX, float fY, int nPos, int nFormat, float fDelayTime, DWORD dwSetID, CEtUICallback *pCall = NULL );
};