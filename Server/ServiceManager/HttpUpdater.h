
#pragma once

#include <wininet.h>

class RequestParam
{
public:
	RequestParam(const wstring& n, const wstring& v)
		: name(n), value(v)
	{

	}

	RequestParam(const wstring& n, const wstring& v, const wstring& b)
		: name(n), value(v), buffer(b)
	{

	}

	~RequestParam()
	{

	}

public:
	static wstring boundary;

public:
	wstring name;
	wstring value;
	wstring buffer;
};

class CHttpUpdater
{
public:
	CHttpUpdater();
	~CHttpUpdater();

	bool QueryPost(const TCHAR * pIP, const TCHAR * pUser, const TCHAR * pPass, const TCHAR * pURL, const void * pData, int nLen);
	bool QueryPostUpload(const TCHAR* pIP, const TCHAR* pUser, const TCHAR* pPass, const TCHAR* pRL, const void* pData, int nLen, const TCHAR* pParamName, const TCHAR* pAttachFile);
	bool QueryPostUpload(const TCHAR* pIP, const TCHAR* pUser, const TCHAR* pPass, const TCHAR* pURL, const vector<RequestParam>& parameters, const vector<RequestParam>& files);

private:
	bool Connect(const TCHAR *ip, const TCHAR *userid, const TCHAR *password);
	void Disconnect();

	bool OpenRequest(const TCHAR * pURL);
	bool SendRequest(const void * pData, int nLen);
	bool SendUploadRequest(const vector<RequestParam>& parameters, const vector<RequestParam>& files);

	HINTERNET m_hSession;
	HINTERNET m_hConnection;
	HINTERNET m_hRequest;
};