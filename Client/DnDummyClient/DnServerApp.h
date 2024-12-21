#pragma once

#include "NxServerApp.h"

class DnServerApp : public NxServerApp
{
public:
	DnServerApp();
	virtual ~DnServerApp();

	virtual bool CreateSessionManager();
};

extern DnServerApp	g_ServerApp;
