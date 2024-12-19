#include "StdAfx.h"

#ifdef PRE_MOD_BROWSER

#include "DnInternetBrowserIESink.h"
#include "DnInternetBrowserManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnInternetBrowserIESink::CDnInternetBrowserIESink()
{
}

HRESULT __stdcall CDnInternetBrowserIESink::OnQuit()
{
	if (m_pMgr == NULL)
		return S_FALSE;

	return S_OK;
}

HRESULT __stdcall CDnInternetBrowserIESink::Quit()
{
	if (m_pMgr->IsFullScreenMode())
	{
		LPDIRECT3DDEVICE9 pDevice = (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr();
		if (pDevice == NULL)
			return S_FALSE;
		pDevice->SetDialogBoxMode(false);
	}

	m_pMgr->CloseBrowser();

	return S_OK;
}

HRESULT __stdcall CDnInternetBrowserIESink::OnClose(BOOL bIsChildWindow, BOOL *pbCancel)
{
	if (m_pMgr == NULL)
		return S_FALSE;

	*pbCancel = VARIANT_TRUE;

	m_pMgr->CloseBrowser();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDnInternetBrowserIESink::ShowMessage(HWND hwnd,
					LPOLESTR lpstrText,
					LPOLESTR lpstrCaption,
					DWORD dwType,
					LPOLESTR lpstrHelpFile,
					DWORD dwHelpContext,
					LRESULT *plResult)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDnInternetBrowserIESink::ShowHelp(HWND hwnd,
								   LPOLESTR pszHelpFile,
								   UINT uCommand,
								   DWORD dwData,
								   POINT ptMouse,
								   IDispatch *pDispatchObjectHit
								   )
{
	return S_OK;
}

#endif // PRE_MOD_BROWSER