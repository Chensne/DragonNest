#pragma once

class CDNUserSession;
class CGameListener
{
public:
	CGameListener();
	virtual ~CGameListener();

protected:

public:
	virtual int OnDispatchMessage(CDNUserSession * pSession, int iMainCmd, int iSubCmd, char * pData, int iLen) = 0;
};