#pragma once

#include "urlmon.h"

class CLauncherSession;

class CPatchStatusChecker : public IBindStatusCallback
{
public:
	CPatchStatusChecker(CLauncherSession* pSession);
	virtual ~CPatchStatusChecker();

	STDMETHOD(OnStartBinding)( 
		/* [in] */ DWORD dwReserved,
		/* [in] */ IBinding *pib) { return E_NOTIMPL; }

	STDMETHOD(GetPriority)( 
		/* [out] */ LONG *pnPriority) { return E_NOTIMPL; }

	STDMETHOD(OnLowResource)( 
		/* [in] */ DWORD reserved) { return E_NOTIMPL; }

	STDMETHOD(OnProgress)( 
		/* [in] */ ULONG ulProgress,
		/* [in] */ ULONG ulProgressMax,
		/* [in] */ ULONG ulStatusCode,
		/* [in] */ LPCWSTR szStatusText);

	STDMETHOD(OnStopBinding)( 
		/* [in] */ HRESULT hresult,
		/* [unique][in] */ LPCWSTR szError) { return E_NOTIMPL; }

	STDMETHOD(GetBindInfo)( 
		/* [out] */ DWORD *grfBINDF,
		/* [unique][out][in] */ BINDINFO *pbindinfo) { return E_NOTIMPL; }

	STDMETHOD(OnDataAvailable)( 
		/* [in] */ DWORD grfBSCF,
		/* [in] */ DWORD dwSize,
		/* [in] */ FORMATETC *pformatetc,
		/* [in] */ STGMEDIUM *pstgmed) { return E_NOTIMPL; }

	STDMETHOD(OnObjectAvailable)( 
		/* [in] */ REFIID riid,
		/* [iid_is][in] */ IUnknown *punk) { return E_NOTIMPL; }

	STDMETHOD_(ULONG, AddRef)() { return 0; }
	STDMETHOD_(ULONG, Release)() { return 0; }
	STDMETHOD(QueryInterface)(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) { return E_NOTIMPL; }

public:
	void SetKey(const wchar_t* pKey) { key = pKey; }

private:
	CLauncherSession* m_pSession;
	std::wstring key;
};
