
#include "stdafx.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

#if defined( _HSHIELD )
class HackShield_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
		m_hHSSever = _AhnHS_CreateServerObject("./DragonNest.hsb");
		
		EXPECT_NE( m_hHSSever, ANTICPX_INVALID_HANDLE_VALUE );
	}
	virtual void TearDown()
	{
		_AhnHS_CloseServerHandle( m_hHSSever );
	}

	AHNHS_SERVER_HANDLE m_hHSSever;
};

TEST_F( HackShield_unittest, FunctionTest )
{
	UINT uiCount	= 32;
	UINT uiLoop		= 100;
	std::vector<AHNHS_CLIENT_HANDLE>	vHandle;
	std::vector<std::string>			vString;
	vHandle.reserve( uiCount );

	double	dTime[4];
	for( UINT i=0 ; i<_countof(dTime) ; ++i )
		dTime[i] = 0;

	vString.reserve( 4 );
	vString.push_back( "_AhnHS_CreateClientObject() " );
	vString.push_back( "_AhnHS_MakeRequest() " );
	vString.push_back( "_AhnHS_VerifyResponseEx() " );
	vString.push_back( "_AhnHS_CloseClientHandle() " );

	boost::timer t;
	for( UINT i=0 ; i<uiLoop ; ++i )
	{
		t.restart();
		for( UINT j=0 ; j<uiCount ; ++j )
			vHandle.push_back( _AhnHS_CreateClientObject( m_hHSSever ) );
		//EXPECT_TRUE( t.elapsed() == 0 ) << t.elapsed();
		dTime[0] += t.elapsed();

		t.restart();
		AHNHS_TRANS_BUFFER Packet = { 0, };
		for( UINT j=0 ; j<uiCount ; ++j )
			unsigned long nRet = _AhnHS_MakeRequest( vHandle[j], &Packet);
		//EXPECT_TRUE( t.elapsed() == 0 ) << t.elapsed();
		dTime[1] += t.elapsed();

		AHNHS_TRANS_BUFFER TransBuffer = {0,};
		ZeroMemory(&TransBuffer, sizeof(AHNHS_TRANS_BUFFER));
		unsigned long ulErrorCode;

		t.restart();
		for( UINT j=0 ; j<uiCount ; ++j )
			unsigned long ulRet = _AhnHS_VerifyResponseEx( vHandle[j], (unsigned char*)&TransBuffer, sizeof(TransBuffer), &ulErrorCode); 
		//EXPECT_TRUE( t.elapsed() == 0 ) << t.elapsed();
		dTime[2] += t.elapsed();

		t.restart();
		for( UINT j=0 ; j<uiCount ; ++j )
			_AhnHS_CloseClientHandle( vHandle[j] );
		dTime[3] += t.elapsed();
	}

	for( UINT i=0 ; i<_countof(dTime) ; ++i )
	{
		std::cout << vString[i] << static_cast<int>((dTime[i]/(double)uiLoop)*1000) << std::endl;
	}
	//std::cout << "Finish" << std::endl;
}
#endif
#endif // #if !defined( _FINAL_BUILD )
