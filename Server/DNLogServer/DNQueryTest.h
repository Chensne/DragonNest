
#pragma once

#include "Singleton.h"

class CDNQueryTest:public CSingleton<CDNQueryTest>
{
public:

	CDNQueryTest();
	~CDNQueryTest();

	void ProcessQueryTest( const char* pszCmd );

private:

	void P_AddCommonLog( std::vector<std::string>& vSplit );
};