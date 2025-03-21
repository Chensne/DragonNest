#include "StdAfx.h"
#include "DNNexonPI.h"

#if defined(_US) && defined(_FINAL_BUILD)

#if defined(_LOGINSERVER)
extern TLoginConfig g_Config;
#elif defined(_MASTERSERVER)
extern TMasterConfig g_Config;
#endif	// 

CDNNexonPI *g_pNexonPI = NULL;

#define EXPORT_FUNC ("CreatePIObject")
#define EXPORT_FUNC2 ("CreateWLAuthorizeObject")

// handle wrapper
handle_wrapper pi_sdk_handle; 
CreatePIObjectFuncPtr ptr_CreatePIObjectFuncPtr = NULL;
CreateWLAuthorizeObjectPtr ptr_CreateWLAuthorizeObjectPtr = NULL;

void get_current_directory( std::wstring& dll_path )
{
	wchar_t dll_path_buff[2048]={0};
	unsigned int n_size = 1023;
	DWORD dwRet = ::GetModuleFileNameW( NULL , dll_path_buff , n_size);
	dll_path = dll_path_buff ;
	size_t pos = dll_path.find_last_of((L"\\"));
	// get path
	dll_path = dll_path.substr( 0 , pos );
}	  

CDNNexonPI::CDNNexonPI(void)
{
	get_func_ptr(pi_sdk_handle, ptr_CreatePIObjectFuncPtr, ptr_CreateWLAuthorizeObjectPtr);

	InitConfiguration();
}

CDNNexonPI::~CDNNexonPI(void)
{
}

bool CDNNexonPI::get_func_ptr( handle_wrapper& handle_, CreatePIObjectFuncPtr& func_ptr, CreateWLAuthorizeObjectPtr& func_ptr2)
{
	std::wstring dll_path;

	get_current_directory( dll_path );
	dll_path +=  L"\\ImmigratorSDK.dll";
	handle_.reset(::LoadLibraryW(dll_path.c_str()));

	if( NULL == handle_.to_handle() ) return false ;

	func_ptr = (CreatePIObjectFuncPtr)GetProcAddress( (HMODULE)handle_.to_handle() , EXPORT_FUNC);

	//	func_ptr2 = (CreateWLAuthorizeObjectPtr)GetProcAddress( (HMODULE)handle_.to_handle() , EXPORT_FUNC2);

	if( func_ptr != NULL)
		return true;

	handle_.reset(NULL);

	return false; 
}

void CDNNexonPI::InitConfiguration()
{
	m_PI.reset(ptr_CreatePIObjectFuncPtr());
	m_PI->InitConfiguration(g_Config.PIInfo.szIP, g_Config.PIInfo.nPort, g_Config.szRequestPage, 30, g_Config.szUserAgent, g_Config.szServiceCode, 1);
}

ImmigratorErrors CDNNexonPI::RequestRegistration(const char *pAccountName, const char *pUserPwd, const char *pUserIp, unsigned int *pnUserNo)
{
	return m_PI->RequestRegistration(pAccountName, pUserPwd, pUserIp, pnUserNo);
}

ImmigratorErrors CDNNexonPI::RequestLogin(const unsigned int nUserNo, const char *pUserPwd, const char *pUserIp, ImmigratorUserEntity *pUserEntity)
{
	return m_PI->RequestLogin(nUserNo, pUserPwd, pUserIp, pUserEntity);
}

ImmigratorErrors CDNNexonPI::RequestLogoff(const unsigned int nUserNo, const char *pUserIp)
{
	return m_PI->RequestLogoff(nUserNo, pUserIp);
}

// must be less than 2000 characters
ImmigratorErrors CDNNexonPI::RequestBanAccount(const unsigned int nUserNo, const unsigned int nReasonCode, const char *pServerIp, const char *pStartTime, const char *pEndTime, const char *pszComment)
{
	return m_PI->RequestBanAccount(nUserNo, nReasonCode, pServerIp, pStartTime, pEndTime, pszComment);
}

ImmigratorErrors CDNNexonPI::RequestUnbanAccount(const unsigned int nUserNo, const unsigned int nReasonCode, const char* pServerIp, const char* pStartDate, const char* pEndDate, const char* pszComment)
{
	return m_PI->RequestUnbanAccount(nUserNo, nReasonCode, pServerIp, pStartDate, pEndDate, pszComment);
}

ImmigratorErrors CDNNexonPI::RequestCancelRegister(const unsigned int nUserNo, const char* pUserPwd, const unsigned int nReasonCode, const char* pUserIp)
{
	return m_PI->RequestCancelRegister(nUserNo, pUserPwd, nReasonCode, pUserIp);
}

#endif	// _US
