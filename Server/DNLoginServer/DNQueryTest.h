
#pragma once

#include "Singleton.h"

class CDNQueryTest:public CSingleton<CDNQueryTest>
{
public:

	void ProcessQueryTest( const char* pszCmd );

private:

};
