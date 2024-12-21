
#include "stdafx.h"
#include "DNQueryTest.h"
#include "DNSQLConnection.h"
#include "DNSQLConnectionManager.h"

CDNQueryTest::CDNQueryTest()
{

}

CDNQueryTest::~CDNQueryTest()
{

}

void CDNQueryTest::ProcessQueryTest( const char* pszCmd )
{
	std::string					strString(pszCmd);
	std::vector<std::string>	vSplit;
	boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(", ") );

	if( !vSplit.empty() )
	{
		// P_AddCommonLog
		if( stricmp( vSplit[0].c_str(), "P_AddCommonLog" ) == 0 )
		{
			P_AddCommonLog( vSplit );
		}

	}
}

void CDNQueryTest::P_AddCommonLog( std::vector<std::string>& vSplit )
{
	CDNSQLConnection* pCon= g_SQLConnectionManager.FindServerLogDB(0);
	TLogFile log;
	log.unLogType		= 1;
	log.unServerType	= 1;
	log.unWorldSetID	= 1;
	log.uiAccountDBID	= 3080;
	log.biCharDBID		= 10000;
	log.uiSessionID		= 1000;
	wcscpy( log.wszBuf, L"testtest" );

	pCon->QueryLog( &log );
}