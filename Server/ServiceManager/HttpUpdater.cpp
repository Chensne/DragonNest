
#include "stdafx.h"
#include "HttpUpdater.h"
#include <sstream>

wstring RequestParam::boundary;

CHttpUpdater::CHttpUpdater()
{
	m_hSession = NULL;
	m_hConnection = NULL;
	m_hRequest = NULL;
}

CHttpUpdater::~CHttpUpdater()
{
}

bool CHttpUpdater::QueryPost(const TCHAR * pIP, const TCHAR * pUser, const TCHAR * pPass, const TCHAR * pURL, const void * pData, int nLen)
{
	if (Connect(pIP, pUser, pPass) == false)
		return false;

	if (OpenRequest(pURL) == false)
		return false;

	if (SendRequest(pData, nLen) == false)
		return false;

	Disconnect();
	return true;
}

bool CHttpUpdater::QueryPostUpload(const TCHAR * pIP, const TCHAR * pUser, const TCHAR * pPass, const TCHAR * pURL, const vector<RequestParam>& parameters, const vector<RequestParam>& files)
{
	if (Connect(pIP, pUser, pPass) == false)
		return false;

	if (OpenRequest(pURL) == false)
		return false;

	if (SendUploadRequest(parameters, files) == false)
		return false;

	Disconnect();
	return true;
}

bool CHttpUpdater::Connect(const TCHAR *ip, const TCHAR *userid, const TCHAR *password)
{
	Disconnect();

	m_hSession = InternetOpen(_T("GSM"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	m_hConnection = InternetConnect(m_hSession, ip, INTERNET_DEFAULT_HTTP_PORT, userid, password, INTERNET_SERVICE_HTTP, 0, 1);
	
	return m_hConnection != NULL ? true : false;
}

void CHttpUpdater::Disconnect()
{
	if (m_hConnection)
		InternetCloseHandle(m_hConnection);
	if (m_hSession)
		InternetCloseHandle(m_hSession);
	if (m_hRequest)
		InternetCloseHandle(m_hRequest);

	m_hConnection = 0;
	m_hSession = 0;
	m_hRequest = 0;
}

bool CHttpUpdater::OpenRequest(const TCHAR * pURL)
{
	static LPCWSTR accept[2] = { L"*/*", NULL };
	m_hRequest = HttpOpenRequest(m_hConnection, L"POST", pURL, NULL, NULL, NULL, 0, 0);

	return m_hRequest != NULL ? true : false;
}

bool CHttpUpdater::SendRequest(const void * pData, int nLen)
{
	if (nLen > 4095)
		return false;

	DWORD nRet;
	if (m_hRequest)
	{
		//post header
		TCHAR szLen[MAX_PATH];
		TCHAR szHeader[4096];
		memset(szLen, 0, sizeof(szLen));
		memset(szHeader, 0, sizeof(szHeader));
		
		//wsprintf(szLen, L"%d", strlen((char*)pData));
		wsprintf(szLen, L"%d", nLen);
		 
		lstrcpy(szHeader, L"POST Admin/AdminRequest.aspx HTTP/1.1\r\n");
		lstrcpy(szHeader, L"Accept: */*\r\n");
		lstrcat(szHeader, L"Content-type: application/x-www-form-urlencoded\r\n");
		lstrcat(szHeader, L"User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\n");
		lstrcat(szHeader, L"Content-length: ");
		lstrcat(szHeader, szLen);
		lstrcat(szHeader, L"\r\n\n");
		
		if (HttpAddRequestHeaders(m_hRequest, szHeader, -1L, HTTP_ADDREQ_FLAG_ADD) == TRUE)
		{
			char szTemp[4096];
			memset(szTemp,0,sizeof(szTemp));
			memcpy(szTemp, pData, nLen);
			if (HttpSendRequest(m_hRequest, szHeader, (DWORD)_tcslen(szHeader), (LPVOID)pData, nLen) == TRUE)
				return true;
			nRet = ::GetLastError();
		}
		nRet = ::GetLastError();
	}
	return false;
}

bool CHttpUpdater::SendUploadRequest(const vector<RequestParam>& parameters, const vector<RequestParam>& files)
{
	if (!m_hRequest)
		return false;

	if (RequestParam::boundary.empty())
		return false;

	wstringstream ss;
	for each (RequestParam param in parameters)
	{
		ss << L"-----------------------" << RequestParam::boundary << L"\r\n";
		ss << L"Content-Disposition: form-data; ";
		ss << L"name=\"" << param.name << L"\"\r\n\r\n";
		ss << param.value << L"\r\n";
	}

	for each (RequestParam param in files)
	{
		ss << L"-----------------------" << RequestParam::boundary << L"\r\n";
		ss << L"Content-Disposition: form-data; ";
		ss << L"name=\"" << param.name << L"\"; ";
		ss << L"filename=\"" << param.value << L"\"\r\n";
		ss << L"Content-Type: octet-stream;\r\n\r\n";
		ss << param.buffer << L"\r\n\r\n";
	}

	ss << L"-----------------------" << RequestParam::boundary << L"--\r\n";
	wstring content = ss.str();

	ss.str(L"");
	ss << L"Accept: */*\r\n";
	ss << L"Content-Type: multipart/form-data; boundary=---------------------" << RequestParam::boundary << L"\r\n";
	ss << L"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.2)\r\n";
	//ss << L"Content-length: " << content.length() << L"\r\n";
	wstring header = ss.str();

	OutputDebugString(header.c_str());
	OutputDebugString(L"\r\n");
	OutputDebugString(content.c_str());

	if (!HttpAddRequestHeaders(m_hRequest, header.c_str(), -1L, HTTP_ADDREQ_FLAG_ADD))
		return false;

	char* buffer = new char[content.length() * 4 + 1];
	ZeroMemory(buffer, content.length() * 4 + 1);
	WideCharToMultiByte(CP_UTF8, 0, content.c_str(), -1, buffer, (int)content.length() * 4, NULL, NULL);

	if (!HttpSendRequest(m_hRequest, header.c_str(), (DWORD)header.length(), (LPVOID)buffer, (DWORD)content.length() * 4))
	{
		delete buffer;
		return false;
	}

	delete buffer;

	BYTE result[8192];
	ZeroMemory(result, sizeof(result));

	DWORD bytesAvailable = 0;
	if (!InternetQueryDataAvailable(m_hRequest, &bytesAvailable, 0, 0))
		return false;

	DWORD offset = 0;
	DWORD bytesRead = 0;

	while (0 < bytesAvailable)
	{
		if (offset + bytesAvailable >= 8192)
			break;

		if (!InternetReadFile(m_hRequest, result + offset, bytesAvailable, &bytesRead))
			return false;

		offset += bytesRead;

		if (!InternetQueryDataAvailable(m_hRequest, &bytesAvailable, 0, 0))
			return false;
	}

	FILE* p = fopen("report_result.txt", "w+");
	fwrite(result, 1, 8192, p);
	fclose(p);

	return true;
}