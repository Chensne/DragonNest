
#pragma once

#include "Connection.h"
#include "Log.h"

#if defined(_TH) && defined(_FINAL_BUILD)

class CDNAsiaSoftOTP : public CConnection
{
public:
	struct Status
	{
		enum eStatus 
		{
			Free = 0x4ab83721,
			Using = 0x64cb4325,
		};
	};

	CDNAsiaSoftOTP(const char * pszIP, short nPort);
	~CDNAsiaSoftOTP();

	void Reconnect(DWORD CurTick) {}
	bool Connect();

	void OnConnected();
	void OnDisconnected();

	//parse
	int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	//use
	bool GetLock();

	//value
	void SetValue(const char * pDomain, const char * pMasterAkeyID, const char * pszResponse, UINT nSessionID);

private:
	volatile long m_lStatus;

	//OTP Value
	std::string m_strDomain;
	std::string m_strMasterKey;
	std::string m_strResponse;
	UINT m_nSessionID;
	
	int _AddSendData(char *pData, int nLen);
	int _ConvertOTPResultCode(const char * pszStatus);

	DWORD m_dwSendedTime;
	CSyncLock m_Sync;
	
	//use
	void _ReleaseLock();

	//value
	void _ResetValue();

	//send
	int _SendOPT(const char * pDomain, const char * pMasterAkeyID, const char * pszResponse, UINT nSessionID);

	class _ScopeReleaseLockAndDetachConnection
	{
	public:
		_ScopeReleaseLockAndDetachConnection(CDNAsiaSoftOTP * pOTP)
		{
			_ASSERT(pOTP != NULL);
			m_pOTP = pOTP;
		}
		~_ScopeReleaseLockAndDetachConnection()
		{
			m_pOTP->DetachConnection(L"~ScopeReleaseLockAndDetachConnection()");
			g_Log.Log(LogType::_ASIASOFTLOG, L"DetachConnection(~ScopeReleaseLockAndDetachConnection())");
		}

	private:
		CDNAsiaSoftOTP * m_pOTP;
	};
};

#endif		//#ifdef _TH