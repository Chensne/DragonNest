#pragma once

#include "LiteHTMLReader.h"

class CDnMiniSiteChecker : public ILiteHTMLReaderEvents
{
public:
	CDnMiniSiteChecker();
	virtual ~CDnMiniSiteChecker();

	bool Process(float fDelta);
	const std::string& GetMiniSiteURL() const { return m_MiniSiteURL; }
	const std::wstring& GetMiniSiteTooltip();

protected:
	virtual void BeginParse(DWORD dwAppData, bool &bAbort) {}
	virtual void StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort){}
	virtual void EndTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort) {}
	virtual void Characters(const CStringW &rText, DWORD dwAppData, bool &bAbort);
	virtual void Comment(const CStringW &rComment, DWORD dwAppData, bool &bAbort) {}
	virtual void EndParse(DWORD dwAppData, bool bIsAborted) {}

private:
	float GetMiniSiteCheckTime() const;

	CLiteHTMLReader m_HTMLReader;
	WCHAR* m_pHtml;
	float m_MiniSiteCheckTimer;
	std::string m_MiniSiteURL;
	std::wstring m_MiniSiteButtonTooltip;
};