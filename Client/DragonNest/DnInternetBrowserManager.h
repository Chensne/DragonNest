#pragma once

#ifdef PRE_MOD_BROWSER

#include <exdisp.h>
#include "DnInternetBrowserIESink.h"
#include "DnInternetBrowserDefine.h"
#include "DnCashShopDefine.h"
extern CComModule _Module;

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class CDnInternetBrowserManager
{
public:
	CDnInternetBrowserManager();
	virtual ~CDnInternetBrowserManager();

	void Initialize();

	eBrowserError OpenBrowser(const std::string& url, const SUICoord& coord, eBrowserType type, const std::wstring& msgBoxString);
	eBrowserError OpenChargeBrowser(eCashChargeBrowserType type, std::string& url, int nWidth, int nHeight);

	bool IsFullScreenMode() const { return m_bFullScreenMode; }
	bool ProcessBrowser();

	void SetBrowserOpenedType(eBrowserType type);

	void CloseBrowser();

	LRESULT WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

private:
	bool m_bBrowserOpened;
	bool m_bFullScreenMode;
	eBrowserType m_BrowserOpenedType;
};

#endif // PRE_MOD_BROWSER