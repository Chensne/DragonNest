#pragma once

#ifndef PTSDK_DLL
#	define PTSDK_DLL
#endif
#include "ServerInterface.h"
#include "ConfigInterface.h"
#include "InfoCast.h"

class PTSDK_STUFF CAccountServer :
	public CServerInterface, private CInfoNode, private CInfoViewer
{
private:
	CAccountServer(void);
	~CAccountServer(void);
public:
	static CAccountServer * NewInstance();
	static void DeleteInstance(CAccountServer * p_as);
	CInfoNode * GetInfoNode();
	void Init(const char * iniFileName);
	void Init(CConfigInterface & config);

	(operator CInfoNode &)();
	(operator CServerInterface&)();
};

class PTSDK_STUFF CAccountServerPtr
{
public:
	CAccountServerPtr();
	~CAccountServerPtr();

	CAccountServer * GetPoint();
	CAccountServer& operator*(void);
	CAccountServer * operator->(void);
	CInfoNode	   * GetInfoNode();

private:
	CAccountServer * m_pAS;
};
