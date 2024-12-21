#include "StdAfx.h"

#ifdef PRE_MOD_BROWSER

#include "DnInternetBrowserManager.h"
#ifdef _CH
#include "DnSDOAService.h"
#endif
#include "GameOption.h"
#include "DnMainFrame.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CComModule _Module;
BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

CComQIPtr<IWebBrowser2> pWebBrowser;
CComObject<CDnInternetBrowserIESink>* pIESink;
HWND g_hwndBrowser;
WNDPROC g_ChildWndProc;

CDnInternetBrowserManager::CDnInternetBrowserManager()
{
	m_bBrowserOpened = false;
	m_bFullScreenMode = false;
}

void CDnInternetBrowserManager::Initialize()
{
	_Module.Init(ObjectMap, CDnMainFrame::GetInstance().GetHInst());
}

eBrowserError CDnInternetBrowserManager::OpenBrowser(const std::string& url, const SUICoord& coord, eBrowserType type, const std::wstring& msgBoxString)
{
	if (m_bBrowserOpened || IsWindow(g_hwndBrowser))
		return eERRBROWSER_ALREADY_OPENED;

	if (SUCCEEDED(OleInitialize(NULL)))
	{
		HRESULT hr;

		if (pIESink)
			pIESink = NULL;

		if (pWebBrowser == NULL)
		{
			AtlAxWinInit();

			RECT mainRect;
			SUICoord correctCoord = coord;
			if (CGameOption::GetInstance().m_bWindow)
			{
				GetWindowRect(CDnMainFrame::GetInstance().GetHWnd(), &mainRect);
				correctCoord.fX = mainRect.left + coord.fX;
				correctCoord.fY = mainRect.top + coord.fY;
			}

			g_hwndBrowser = CreateWindow(L"AtlAxWin80", L"Shell.Explorer.2", WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER, (int)correctCoord.fX, (int)correctCoord.fY, (int)correctCoord.fWidth, (int)correctCoord.fHeight, 
				CDnMainFrame::GetInstance().GetHWnd(), (HMENU)0, CDnMainFrame::GetInstance().GetHInst(), NULL);
			g_ChildWndProc = (WNDPROC)SetWindowLongPtr(g_hwndBrowser, GWL_WNDPROC, (LONG_PTR)ChildWndProc);

			CComPtr<IUnknown> punkIE;
			if (AtlAxGetControl(g_hwndBrowser, &punkIE) == S_OK)
			{
				pWebBrowser = punkIE;

				if (pIESink == NULL)
				{
					hr = CComObject<CDnInternetBrowserIESink>::CreateInstance(&pIESink);
					if(FAILED(hr))
						_ASSERT(0);
					pIESink->SetManager(this);
				}

				AtlGetObjectSourceInterface(punkIE, &(pIESink->m_libid), &(pIESink->m_iid), &(pIESink->m_wMajorVerNum), &(pIESink->m_wMinorVerNum));
				hr = pIESink->DispEventAdvise(pWebBrowser, &(pIESink->m_iid));

				if (FAILED(hr))
					_ASSERT(0);
			}
		}
		else
		{
			return eERRBROWSER_CANT_OPEN_GENERAL;
		}

		if (CGameOption::IsActive() && CGameOption::GetInstance().m_bWindow == false && GetEtDevice())
		{
			LPDIRECT3DDEVICE9 pDevice = (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr();
			if (pDevice == NULL)
				return eERRBROWSER_CANT_OPEN_D3D;

			pDevice->SetDialogBoxMode(TRUE);
			m_bFullScreenMode = true;
		}
		else
		{
			m_bFullScreenMode = false;
		}

		CComVariant vUrl(url.c_str()), vEmpty;
		hr = pWebBrowser->Navigate2(&vUrl, &vEmpty, &vEmpty, &vEmpty, &vEmpty);
		if (SUCCEEDED(hr))
		{
			pWebBrowser->put_Visible(VARIANT_TRUE);
			ShowWindow((HWND)g_hwndBrowser, SW_SHOW);
			InvalidateRect(g_hwndBrowser, NULL, true);

			m_bBrowserOpened = true;
		}

		OleUninitialize();

		return eERRBROWSER_NONE;
	}

	return eERRBROWSER_CANT_OPEN_GENERAL;
}

eBrowserError CDnInternetBrowserManager::OpenChargeBrowser(eCashChargeBrowserType type, std::string& url, int nWidth, int nHeight)
{
	if (type == eCCBT_SDOA_BROWSER)
	{
#if defined _CH && defined _AUTH
		if (m_bBrowserOpened || CDnSDOAService::IsActive() == false)
			return;
		CDnSDOAService::GetInstance().SetTaskBarPosition( 2000, 0 );	// x좌표 최대치로 해두면 알아서 좌측 끝에 붙는다.

		if (CDnSDOAService::GetInstance().ShowChargeDlg(true) == false)
			return eERRBROWSER_CANT_OPEN_GENERAL;

		m_bBrowserOpened = true;
		CDnInterface::GetInstance().DisableAllDlgs(true, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4773 ));	// UISTRING : 캐시 충전 처리중입니다

		return eERRBROWSER_NONE;
#else
		return eERRBROWSER_CANT_OPEN_GENERAL;
#endif
	}
	else
	{
		if (m_bBrowserOpened || IsWindow(g_hwndBrowser))
			return eERRBROWSER_ALREADY_OPENED;

		if (SUCCEEDED(OleInitialize(NULL)))
		{
			HRESULT hr;

			if (pIESink)
				pIESink = NULL;

			if (pWebBrowser == NULL)
			{
				if (type == eCCBT_FULL_BROWSER)
				{
					hr = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (LPVOID*)&pWebBrowser);
					if(FAILED(hr) || pWebBrowser == NULL)
					{
						_ASSERT(0);
						return eERRBROWSER_CANT_OPEN_COCREATE_FAIL;
					}

					pWebBrowser->get_HWND((long*)&g_hwndBrowser);
				}
				else if (type == eCCBT_INGAME_BROWSER)
				{
					AtlAxWinInit();

					RECT mainRect;
					if (CGameOption::GetInstance().m_bWindow)
					{
						GetWindowRect(CDnMainFrame::GetInstance().GetHWnd(), &mainRect);
						mainRect.left += 300;
						mainRect.top += 50;
					}
					else
					{
						mainRect.left = 300;
						mainRect.top = 50;
					}

					if (nWidth <= 0 || nHeight <= 0)
					{
						_ASSERT(0);
						return eERRBROWSER_SIZE_FAIL;
					}

					g_hwndBrowser = CreateWindow(L"AtlAxWin80", L"Shell.Explorer.2", WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER, mainRect.left, mainRect.top, nWidth, nHeight, 
						CDnMainFrame::GetInstance().GetHWnd(), (HMENU)0, CDnMainFrame::GetInstance().GetHInst(), NULL);
#ifdef PRE_FIX_NOMOVE_BROWSER
					g_ChildWndProc = (WNDPROC)SetWindowLongPtr(g_hwndBrowser, GWL_WNDPROC, (LONG_PTR)ChildWndProc);
#endif

					OutputDebug("charge browser OPEN - left:%ld top:%ld width:%d height:%d\n", mainRect.left, mainRect.top, nWidth, nHeight);

					CComPtr<IUnknown> punkIE;
					if (AtlAxGetControl(g_hwndBrowser, &punkIE) == S_OK)
					{
						pWebBrowser = punkIE;

						if (pIESink == NULL)
						{
							hr = CComObject<CDnInternetBrowserIESink>::CreateInstance(&pIESink);
							if(FAILED(hr))
							{
								_ASSERT(0);
								return eERRBROWSER_CANT_OPEN_CHARGEBROWSER_ATTACH_IE_EVENT_FAIL;
							}
							pIESink->SetManager(this);
						}

						AtlGetObjectSourceInterface(punkIE, &(pIESink->m_libid), &(pIESink->m_iid), &(pIESink->m_wMajorVerNum), &(pIESink->m_wMinorVerNum));
						hr = pIESink->DispEventAdvise(pWebBrowser, &(pIESink->m_iid));

						if (FAILED(hr))
						{
							_ASSERT(0);
							return eERRBROWSER_CANT_OPEN_CHARGEBROWSER_ATTACH_IE_EVENT_FAIL;
						}
					}
				}
				else
				{
					_ASSERT(0);
					return eERRBROWSER_CANT_OPEN_CHARGEBROWSER_TYPE_FAIL;
				}
			}
			else
			{
				return eERRBROWSER_CANT_OPEN_GENERAL;
			}

			if (type == eCCBT_INGAME_BROWSER)
			{
				if (CGameOption::IsActive() && CGameOption::GetInstance().m_bWindow == false && GetEtDevice())
				{
					LPDIRECT3DDEVICE9 pDevice = (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr();
					if (pDevice == NULL)
						return eERRBROWSER_CANT_OPEN_D3D;

					pDevice->SetDialogBoxMode(TRUE);
					m_bFullScreenMode = true;
				}
				else
				{
					m_bFullScreenMode = false;
				}
			}

			if (url.empty())
				return eERRBROWSER_NO_URL;

			CComVariant vUrl(url.c_str()), vEmpty;

			VARIANT vFlags;
			V_VT(&vFlags) = VT_I4;
			V_I4(&vFlags) = navNoReadFromCache;

			hr = pWebBrowser->Navigate2(&vUrl, &vFlags, &vEmpty, &vEmpty, &vEmpty);
			if (SUCCEEDED(hr))
			{
				pWebBrowser->put_Visible(VARIANT_TRUE);
				ShowWindow((HWND)g_hwndBrowser, SW_SHOW);
				if (type == eCCBT_FULL_BROWSER)
				{
					SetForegroundWindow((HWND)g_hwndBrowser);
					SetActiveWindow((HWND)g_hwndBrowser);
				}

				InvalidateRect(g_hwndBrowser, NULL, true);

				m_bBrowserOpened = true;
				CDnInterface::GetInstance().DisableAllDlgs(true, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4773 ));	// UISTRING : 캐시 충전 처리중입니다
			}
			else
			{
				return eERRBROWSER_NAVIGATE_FAIL;
			}

			OleUninitialize();

			return eERRBROWSER_NONE;
		}
	}

	return eERRBROWSER_CANT_OPEN_GENERAL;
}

bool CDnInternetBrowserManager::ProcessBrowser()
{
	if (m_bBrowserOpened)
	{
		BOOL bIsActivate = IsWindow(g_hwndBrowser);

		if (bIsActivate == false)
		{
			m_bBrowserOpened = false;

			pWebBrowser->Quit();
			pWebBrowser.Release();

			CDnInterface::GetInstance().OnCloseBrowser(m_BrowserOpenedType);

			if (CGameOption::IsActive() && CGameOption::GetInstance().m_bWindow == false && GetEtDevice())
			{
				LPDIRECT3DDEVICE9 pDevice = (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr();
				if (pDevice == NULL)
					return false;

				pDevice->SetDialogBoxMode(FALSE);
			}
		}
	}

	return true;
}

void CDnInternetBrowserManager::SetBrowserOpenedType(eBrowserType type)
{
	m_BrowserOpenedType = type;
}

void CDnInternetBrowserManager::CloseBrowser()
{
	DestroyWindow(g_hwndBrowser);
}

LRESULT CDnInternetBrowserManager::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case WM_KEYUP:
		{
			if (m_bBrowserOpened && pWebBrowser)
			{
				CComQIPtr<IOleInPlaceActiveObject, &IID_IOleInPlaceActiveObject> pIOIPAO(pWebBrowser) ;

				if(pIOIPAO)
				{
					MSG msg;
					msg.message = message;
					msg.wParam = wParam;
					msg.lParam = lParam;

					pIOIPAO->TranslateAccelerator(&msg);
				}
			}
		}
		break;
	}

#ifdef PRE_FIX_CASHSHOP_CHARGE_WNDPROC
	if (m_bBrowserOpened && pWebBrowser && IsWindow(g_hwndBrowser))
		return DefWindowProc( g_hwndBrowser, message, wParam, lParam );
	else
		return 0;
#else
	return DefWindowProc( g_hwndBrowser, message, wParam, lParam );
#endif
}

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_WINDOWPOSCHANGING:
		{
			((WINDOWPOS*)lParam)->flags |= SWP_NOMOVE;
		}
		break;
	}

	return CallWindowProc(g_ChildWndProc, hWnd, message, wParam, lParam);
}

#endif // PRE_MOD_BROWSER