#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <EXDISPID.h>

#define SINKID_CASHSHOPEVENTS 0

class CDnCashShopTask;
class ATL_NO_VTABLE CDnCashShopIESink : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDnCashShopIESink>,
	public IDispEventImpl<SINKID_CASHSHOPEVENTS, CDnCashShopIESink, &DIID_DWebBrowserEvents2>,
	public IDocHostShowUI
{
public:
	CDnCashShopIESink();
	virtual ~CDnCashShopIESink() {}

BEGIN_COM_MAP(CDnCashShopIESink)
	COM_INTERFACE_ENTRY_IID(DIID_DWebBrowserEvents2, CDnCashShopIESink)
END_COM_MAP()

BEGIN_SINK_MAP(CDnCashShopIESink)
	SINK_ENTRY_EX(SINKID_CASHSHOPEVENTS, DIID_DWebBrowserEvents2, DISPID_QUIT, Quit)
	SINK_ENTRY_EX(SINKID_CASHSHOPEVENTS, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
	SINK_ENTRY_EX(SINKID_CASHSHOPEVENTS, DIID_DWebBrowserEvents2, DISPID_WINDOWCLOSING, OnClose)
END_SINK_MAP()

	void SetTask(CDnCashShopTask* pTask) { m_pTask = pTask; }

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

	enum eCashShopSinkType
	{
		eCHARGE,
		eGENERAL,
	};

	void SetCashShopSinkType(eCashShopSinkType type) { m_Type = type; }

private:
	CDnCashShopTask*	m_pTask;
	eCashShopSinkType	m_Type;
};