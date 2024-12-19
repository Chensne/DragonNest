#pragma once

class CDNUserRepository;


class CDNManager
{
private:
	CSyncLock m_UserLock;

	typedef std::map<UINT, CDNUserRepository*> TMapUser;			// first: AccountDBID
	TMapUser m_pMapUserList;

public:
	CDNManager(void);
	~CDNManager(void);

	CDNUserRepository *AddUser(UINT nAccountDBID);
	bool DelUser(UINT nAccountDBID);

	CDNUserRepository *GetUser(const char* pszAccountName);
	CDNUserRepository *GetUser(UINT nAccountDBID);
};

extern CDNManager *g_pManager;