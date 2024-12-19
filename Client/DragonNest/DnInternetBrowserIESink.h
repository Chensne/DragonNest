#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <EXDISPID.h>

#ifdef PRE_MOD_BROWSER

#define SINKID_CASHSHOPEVENTS 0

class CDnInternetBrowserManager;
class ATL_NO_VTABLE CDnInternetBrowserIESink : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDnInternetBrowserIESink>,
	public IDispEventImpl<SINKID_CASHSHOPEVENTS, CDnInternetBrowserIESink, &DIID_DWebBrowserEvents2>,
	public IDocHostShowUI
{
public:
	CDnInternetBrowserIESink();
	virtual ~CDnInternetBrowserIESink() {}

BEGIN_COM_MAP(CDnInternetBrowserIESink)
	COM_INTERFACE_ENTRY_IID(DIID_DWebBrowserEvents2, CDnInternetBrowserIESink)
END_COM_MAP()

BEGIN_SINK_MAP(CDnInternetBrowserIESink)
	SINK_ENTRY_EX(SINKID_CASHSHOPEVENTS, DIID_DWebBrowserEvents2, DISPID_QUIT, Quit)
	SINK_ENTRY_EX(SINKID_CASHSHOPEVENTS, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
	SINK_ENTRY_EX(SINKID_CASHSHOPEVENTS, DIID_DWebBrowserEvents2, DISPID_WINDOWCLOSING, OnClose)
END_SINK_MAP()

	void SetManager(CDnInternetBrowserManager* pMgr) { m_pMgr = pMgr; }

	HRESULT __stdcall Quit();
	HRESULT __stdcall OnQuit();
	HRESULT __stdcall OnClose(BOOL bIsChildWindow, BOOL *pbCancel);

	HRESULT STDMETHODCALLTYPE ShowMessage(HWND hwnd,
		LPOLESTR lpstrText,
		LPOLESTR lpstrCaption,
		DWORD dwType,
		LPOLESTR lpstrHelpFile,
		DWORD dwHelpContext,
		LRESULT *plResult);
	HRESULT STDMETHODCALLTYPE ShowHelp(HWND hwnd,
		LPOLESTR pszHelpFile,
		UINT uCommand,
		DWORD dwData,
		POINT ptMouse,
		IDispatch *pDispatchObjectHit
		);

private:
	CDnInternetBrowserManager* m_pMgr;
};

#endif // PRE_MOD_BROWSER