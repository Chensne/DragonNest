#pragma once

#include <string>

class NxNetInit
{
public:
	NxNetInit();
	~NxNetInit();

	bool StartupWinSock();
	void CleanupWinSock();

};

class NxGetLastError
{
public:

 static	int				GetCode();
 static	tstring			GetString();
 static	void			Trace();

};

 