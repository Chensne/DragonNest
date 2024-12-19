
#include "Stdafx.h"

#if defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
#include "DNNHNNetCafe.h"

CDNNHNNetCafe * g_pNHNNetCafe = NULL;

CDNNHNNetCafe::CDNNHNNetCafe()
{
	m_dwReconnectTick = 0; 
}

CDNNHNNetCafe::~CDNNHNNetCafe()
{
	NetCafe_Finalize();
}

void CDNNHNNetCafe::PushUserLogOutPool(const char * pszUserID)
{
	if (pszUserID == NULL) return;
	std::string strUserID = pszUserID;
	m_vUserLogOutPool.push_back(strUserID);
}

void CDNNHNNetCafe::FlushUserLogOutPool()
{
	if (m_vUserLogOutPool.empty()) return;

	int nRet = HAN_NETCAFE_E_UNKNOWN;
	std::vector <std::string>::iterator ii;
	for (ii = m_vUserLogOutPool.begin(); ii != m_vUserLogOutPool.end(); )
	{
		nRet = HAN_NETCAFE_E_UNKNOWN;
		nRet = HanNetCafe_User_Logout((*ii).c_str());		
		if (nRet == HAN_NETCAFE_OK || nRet == HAN_NETCAFE_E_NOT_REGISTERED_USER)
			ii = m_vUserLogOutPool.erase(ii);
		else
			ii++;
	}
}

void CDNNHNNetCafe::Reconnect(DWORD CurTick)
{
	if (m_dwReconnectTick + 5000 < CurTick){
		m_dwReconnectTick = CurTick;

		if (NetCafe_Initialize() == true)	//NHN NetCafe Module Init
			g_Log.Log(LogType::_FILELOG, L"g_pNHNNetCafe->Initialize() Reconnect OK!!\r\n");
	}
}

bool CDNNHNNetCafe::NetCafe_Initialize()
{
	int nRet = HanNetCafe_Init();
		
	if (nRet == HAN_NETCAFE_OK)
		return true;

	_ASSERT_EXPR(nRet == HAN_NETCAFE_E_NOT_INITIALIZED, L"Netcafe WebServer Init Fail!!!!");
	return false;
}

void CDNNHNNetCafe::NetCafe_Finalize()
{
	int nRet = HanNetCafe_Cleanup();
	_ASSERT(HAN_NETCAFE_OK != nRet);
}

bool CDNNHNNetCafe::NetCafe_UserLogin(const char * pszUserID, const char * pszIP, bool &bNetCafeIP)
{
	int nRet = HanNetCafe_User_Login(pszUserID, pszIP);
	//int nRet = HanNetCafe_User_Login(pszUserID, "61.32.8.204");

	bNetCafeIP = (nRet == HAN_NETCAFE_OK) ? true : false;
	return (nRet == HAN_NETCAFE_OK || nRet == HAN_NETCAFE_OK_NOT_NETCAFE_IP) ? true : false;
}

bool CDNNHNNetCafe::NetCafe_UserLogOut(const char * pszUserID)
{
	FlushUserLogOutPool();

	int nRet = HanNetCafe_User_Logout(pszUserID);
	if (nRet == HAN_NETCAFE_E_RETRIEVE_INFORM)
		PushUserLogOutPool(pszUserID);
	return (nRet == HAN_NETCAFE_OK || nRet == HAN_NETCAFE_E_NOT_REGISTERED_USER) ? true : false;
}

bool CDNNHNNetCafe::NetCafe_GetNetCafeCode(const char * pszUserID, char * pszOutBuffer, int nSize)
{
	int nRet = HanNetCafe_Get_NetCafe_Code(pszUserID, pszOutBuffer, nSize);	
	return (nRet == HAN_NETCAFE_OK) ? true : false;
}

bool CDNNHNNetCafe::NetCafe_GetProductCode(const char * pszUserID, char * pszOutbuffer, int nSize)
{
	int nRet = HanNetCafe_Get_Product_Code(pszUserID, pszOutbuffer, nSize);
	return (nRet == HAN_NETCAFE_OK) ? true : false;
}

#endif