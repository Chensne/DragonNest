#pragma once

extern std::string MD5PasswordHash;

enum eRequest
{
	GET,
	POST,
	PUT,
	HEAD
};

class HTTPClient
{
public:
	HTTPClient();
	~HTTPClient();
	bool CheckLogin(std::string, DWORD dataSize);
};
extern HTTPClient gHTTP;