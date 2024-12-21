
#include "stdafx.h"
#include "DNQueryTest.h"
#include "DNSQLMembershipManager.h"
#include "DNSQLWorldManager.h"
#include <iostream>

void CDNQueryTest::ProcessQueryTest( const char* pszCmd )
{
	std::string					strString(pszCmd);
	std::vector<std::string>	vSplit;
	boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(", ") );

	if( !vSplit.empty() )
	{
	}
}


