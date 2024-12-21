#pragma once

#if defined(_US) && defined(_FINAL_BUILD)

#include <atlbase.h>
#include <atlstr.h>
#include <comutil.h>

#pragma comment(lib, "comsuppw.lib")

#include <map>

typedef IPIObject* (*CreatePIObjectFuncPtr)();
typedef IWLAuthorizeObject* (*CreateWLAuthorizeObjectPtr)();

class handle_wrapper
{
public:
	handle_wrapper(void):h_(NULL){}
	virtual ~handle_wrapper(void){}
	void close_handle() { if(NULL != h_) {CloseHandle(h_); h_=NULL;}}
	void reset(HANDLE h){ close_handle(); h_ = h; }
	HANDLE to_handle() { return h_; }
private: // non-copyable object
	handle_wrapper( const handle_wrapper& rhs );
	const handle_wrapper& operator=(const handle_wrapper& rhs); 
private:
	HANDLE	 h_;
};

extern handle_wrapper pi_sdk_handle;
extern CreatePIObjectFuncPtr ptr_CreatePIObjectFuncPtr;
extern CreateWLAuthorizeObjectPtr ptr_CreateWLAuthorizeObjectPtr;

class CDNNexonPI
{
private:
	std::auto_ptr<IPIObject> m_PI;

public:
	CDNNexonPI(void);
	~CDNNexonPI(void);

	bool get_func_ptr(handle_wrapper& handle_, CreatePIObjectFuncPtr&, CreateWLAuthorizeObjectPtr&);

	void InitConfiguration();
	ImmigratorErrors RequestRegistration(const char *pAccountName, const char *pUserPwd, const char *pUserIp, unsigned int *pnUserNo);
	ImmigratorErrors RequestLogin(const unsigned int nUserNo, const char *pUserPwd, const char *pUserIp, ImmigratorUserEntity *pUserEntity);
	ImmigratorErrors RequestLogoff(const unsigned int nUserNo, const char *pUserIp);

	// must be less than 2000 characters
	ImmigratorErrors RequestBanAccount(const unsigned int nUserNo, const unsigned int nReasonCode, const char *pServerIp, const char *pStartTime, const char *pEndTime, const char *pszComment);
	ImmigratorErrors RequestUnbanAccount(const unsigned int nUserNo, const unsigned int nReasonCode, const char* pServerIp, const char* pStartDate, const char* pEndDate, const char* pszComment);
	ImmigratorErrors RequestCancelRegister(const unsigned int nUserNo, const char* pUserPwd, const unsigned int nReasonCode, const char* pUserIp);
};

extern CDNNexonPI *g_pNexonPI;

#endif	// _US
