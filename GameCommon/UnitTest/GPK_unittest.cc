
#include "stdafx.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

#if defined( _GPK )

class GPK_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
		m_pDynCode = GPKCreateSvrDynCode();
		EXPECT_TRUE( m_pDynCode != NULL );

		char szSvrDir[MAX_PATH] = { 0, };
		char szCltDir[MAX_PATH] = { 0, };

#if defined(BIT64)
		sprintf(szSvrDir, "./DynCodeBin/Server64");
#else
		sprintf(szSvrDir, "./DynCodeBin/Server");
#endif
		sprintf(szCltDir, "./DynCodeBin/Client");

		int nBinCount = m_pDynCode->LoadBinary(szSvrDir, szCltDir);

		EXPECT_NE( nBinCount, 0 );
		EXPECT_TRUE( m_pDynCode->LoadAuthFile("AuthData.dat") );
	}
	virtual void TearDown()
	{
		m_pDynCode->Release();
	}

	SGPK::IGPKSvrDynCode*	m_pDynCode;
};

TEST_F( GPK_unittest, FunctionTest )
{
	double dwTime[6];
	for( UINT i=0 ; i<_countof(dwTime) ; ++i )
		dwTime[i] = 0;

	for( UINT i=0 ; i<1000 ; ++i )
	{
		const unsigned char *pCode = NULL;

		boost::timer t;
		int iIdx = m_pDynCode->GetRandIdx();
		dwTime[0] += t.elapsed();
		EXPECT_TRUE( t.elapsed() == 0 ) << "GetRandIdx() " << t.elapsed();

		t.restart();
		int iCodeLen = m_pDynCode->GetCltDynCode( iIdx, &pCode );
		dwTime[1] += t.elapsed();
		EXPECT_TRUE( t.elapsed() == 0 ) << "GetCltDynCode() " << t.elapsed();

		char szBuf[32] = "Server DATA!!!";
		int nLen = (int)(strlen(szBuf)) + 1;
		t.restart();
		EXPECT_TRUE( m_pDynCode->Encode( (unsigned char*)&szBuf, nLen, iIdx ) );
		dwTime[2] += t.elapsed();
		EXPECT_TRUE( t.elapsed() == 0 ) << "Encode() " << t.elapsed();

		t.restart();
		SGPK::IGPKCSAuth* pGpkCmd = m_pDynCode->AllocAuthObject();
		dwTime[3] += t.elapsed();
		EXPECT_TRUE( pGpkCmd != NULL );
		EXPECT_TRUE( t.elapsed() == 0 ) << "AllocAuthObject() " << t.elapsed();

		t.restart();
		const unsigned char *p = NULL;
		nLen = pGpkCmd->GetAuthData(&p, NULL, NULL);
		dwTime[4] += t.elapsed();
		EXPECT_TRUE( t.elapsed() == 0 ) << "GetAuthData() " << t.elapsed();

		t.restart();
		pGpkCmd->Release();
		dwTime[5] += t.elapsed();
		EXPECT_TRUE( t.elapsed() == 0 ) << "Release() " << t.elapsed();
	}

	for( UINT i=0 ; i<_countof(dwTime) ; ++i )
		std::cout << "[" << i << "] " << dwTime[i]/static_cast<double>(1000) << "ms" << std::endl;
}

#endif // #if defined( _GPK )
#endif // #if !defined( _FINAL_BUILD )
