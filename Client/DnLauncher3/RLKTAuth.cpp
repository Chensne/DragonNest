#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "RLKTAuth.h"
#include "StringUtil.h"
#pragma comment(lib,"ws2_32.lib")
HTTPClient gHTTP;
std::string MD5PasswordHash;

HTTPClient::HTTPClient()
{
}


HTTPClient::~HTTPClient()
{
}



std::vector<std::string> &SplitStringDelim(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

bool HTTPClient::CheckLogin(std::string data, DWORD dataSize){

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
#ifdef DEBUG
		std::cout << "WSAStartup failed.\n";
		system("pause");
#endif
		return FALSE;
	}
	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct hostent *host;
	host = gethostbyname("reborn.skydn.net");
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);


	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0){
#ifdef DEBUG
		std::cout << "Could not connect";
		system("pause");
#endif
		return FALSE;
	}
#ifdef DEBUG
	std::cout << "Connected.\n";
#endif
	std::string Header = "GET /wooo-raru?r=" + data;
	Header += " HTTP/1.1\r\nHost: reborn.dragonnest.ro\r\nUser-Agent: dragonnest.ro\r\nAccept: */*\r\nConnection: close\r\n\r\n";

	//calculam content-length pt ca php sucks.
	//Header.append("Content-Length: " + std::to_string(PostData.length()));
	//Header.append("\r\n\r\n");

	//il transferam in header
	//Header.append(PostData);

	send(Socket, Header.c_str(), Header.size(), 0);
	// doar de test.


	char buffer[10000];
	int nDataLength;
	while ((nDataLength = recv(Socket, buffer, 10000, 0)) > 0){
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
			i += 1;
		}
	}
	std::string cont = buffer;
	int pos = cont.find("RLKTRSP=");
	std::string response = cont.substr(pos+8, cont.size() - pos-8); //???!!!!

	std::vector<std::string> ResponseData;
	SplitStringDelim(response, ',', ResponseData);

	if (ResponseData[0].compare("NOK") == 0)
	{
		MessageBoxA(NULL, "Invalid username or password! Try again!", "Warning!", MB_OK);
		return FALSE;
	}
	ResponseData[3] = ResponseData[3].substr(0,32); //take only md5!


	if (ResponseData[0].compare("OK") == 0)
	{
		if (ResponseData[3].size() != 32)
		{
			MessageBoxA(NULL, FormatA("size = %d", ResponseData[3].size()).c_str() , "~~", MB_OK);
			MessageBoxA(NULL, "Server Error, try to log again!", "~~", MB_OK);
			return FALSE;
		}else{
			MD5PasswordHash = ResponseData[3];
		}
		return TRUE;
	}else{
		MessageBoxA(NULL, "Server Error!", "Error!", MB_OK);
		return FALSE;
	}


	///MessageBoxA(NULL, response.c_str(), "OK", MB_OK);

	//#endif
	closesocket(Socket);
	WSACleanup();
	//	system("pause");
	return TRUE;
}