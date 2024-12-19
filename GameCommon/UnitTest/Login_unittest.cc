
#include "stdafx.h"
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"
#include "DNExtManager.h"
#include "EtResourceMng.h"
#include "DNSQLWorldManager.h"
#include "DNSQLMembershipManager.h"

extern TLoginConfig g_Config;

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

class Login_unittest:public testing::Test
{
protected:

	static void SetUpTestCase()
	{
		g_Config.szResourcePath = "r:\\GameRes";

		// ResourceMng 생성
		CEtResourceMng::CreateInstance();

		// Path 설정
		std::string szResource = g_Config.szResourcePath + "\\Resource";
		std::string szNationStr;
		if( szNationStr.empty() && !g_Config.szResourceNation.empty() ) szNationStr = g_Config.szResourceNation;
		if( !szNationStr.empty() ) 
		{
			szResource += szNationStr;
			CEtResourceMng::GetInstance().AddResourcePath( szResource, true );
			szResource = g_Config.szResourcePath + "\\Resource";
		}

		CEtResourceMng::GetInstance().AddResourcePath( szResource, true );

		g_pExtManager = new CDNExtManager();
	}

	static void TearDownTestCase()
	{
		SAFE_DELETE( g_pExtManager );
	}

	virtual void SetUp() 
	{
		m_pUserConnection = new CDNUserConnection();
	}
	virtual void TearDown()
	{
		SAFE_DELETE( m_pUserConnection );
	}

	CDNUserConnection *m_pUserConnection;
};

TEST_F( Login_unittest, PACKET_TEST )
{
	/*
	CSCheckVersion Check;

#if defined(_JP)
	Check.cNation = NATION_JAPAN;
#elif defined(_CH)
	Check.cNation = NATION_CHINA;
#elif defined(_US)
	Check.cNation = NATION_USA;
#elif defined(_TW)
	Check.cNation = NATION_TAIWAN;
#elif defined (_SG)
	Check.cNation = NATION_SINGAPORE;
#elif defined(_TH)
	Check.cNation = NATION_THAILAND;
#else
	Check.cNation = NATION_KOREA;
#endif

	Check.cVersion = 1;
	Check.bCheck = false;
	Check.nMajorVersion = 1100;
	Check.nMinorVersion = 0;

	m_pUserConnection->RequestCheckVersion(&Check, sizeof(Check));
	*/
}


#endif // #if !defined( _FINAL_BUILD )
