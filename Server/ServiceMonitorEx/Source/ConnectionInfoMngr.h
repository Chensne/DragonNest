

#pragma once

#include "Singleton.hpp"

struct stConnectionInfo
{
	ULONG nConID;
	char szUserName[NAMELENMAX];
	char szPassWord[NAMELENMAX];
	char szIpAddress[IPLENMAX];
	WORD wPort;
};

class CConnectionInfoMngr
{

private:
	DECLARE_SINGLETON_CLASS(CConnectionInfoMngr);

public:
	CConnectionInfoMngr();

	bool LoadData ();
	void ConnectToServerManager ();
	bool GetConInfo (UINT nConID, stConnectionInfo& ConInfo);
	

private:
	std::vector<stConnectionInfo> m_vConnectionList;

};