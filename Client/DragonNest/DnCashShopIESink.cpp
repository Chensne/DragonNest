#include "StdAfx.h"
#include "DnCashShopIESink.h"
#include "DnCashShopTask.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnCashShopIESink::CDnCashShopIESink()
: m_Type( eCHARGE )
, m_pTask( NULL )
{
}

HRESULT __stdcall CDnCashShopIESink::OnQuit()
{
	if (m_pTask == NULL)
		return S_FALSE;

	//m_pTask->REquest

	return S_OK;
}

HRESULT __stdcall CDnCashShopIESink::Quit()
{
	if (m_pTask->GetFullScreenModeFlag())
	{
		LPDIRECT3DDEVICE9 pDevice = (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr();
		if (pDevice == NULL)
			return S_FALSE;
		pDevice->SetDialogBoxMode(false);
	}

	if (m_Type == eCHARGE)
		m_pTask->CloseChargeBrowser();
	else 
		m_pTask->CloseBrowser();

	return S_OK;
}

HRESULT __stdcall CDnCashShopIESink::OnClose(BOOL bIsChildWindow, BOOL *pbCancel)
{
	if (m_pTask == NULL)
		return S_FALSE;

	*pbCancel = VARIANT_TRUE;

	if (m_Type == eCHARGE)
		m_pTask->CloseChargeBrowser();
	else 
		m_pTask->CloseBrowser();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDnCashShopIESink::ShowMessage(HWND hwnd,
					LPOLESTR lpstrText,
					LPOLESTR lpstrCaption,
					DWORD dwType,
					LPOLESTR lpstrHelpFile,
					DWORD dwHelpContext,
					LRESULT *plResult)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDnCashShopIESink::ShowHelp(HWND hwnd,
								   LPOLESTR pszHelpFile,
								   UINT uCommand,
								   DWORD dwData,
								   POINT ptMouse,
								   IDispatch *pDispatchObjectHit
								   )
{
	return S_OK;
}