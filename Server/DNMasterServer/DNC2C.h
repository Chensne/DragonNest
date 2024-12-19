#pragma once

#if defined( PRE_ADD_CHNC2C )

#include "../ServerCommon/CH/sdogsdk.h"

class CDNC2C
{
public:
	CDNC2C();
	~CDNC2C();	

	void AddCoin();
	void DeductCoin();

	void Run();	
	bool Init(const std::string & strRemoteIP, int RemotePort, const std::string & strMatrixID, const std::string & DeviceID);
	void SendGetBalance(int nRetCode, INT64 nCoin, const char* szSeqID);
	void SendAddCoin(int nRetCode, const char* szSeqID);
	void SendReduceCoin(int nRetCode, const char* szSeqID);

private:
	bool m_bStart;	
	unsigned long m_SeqID;
	OpenGame::ISDOGService* m_pService;
};

extern CDNC2C* g_pDnC2C;
#endif //#if defined( PRE_ADD_CHNC2C )