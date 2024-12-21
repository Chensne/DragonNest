#pragma once

class CDNUserSession;
class CDNUserSendManager;
class CDNUserQuest;

struct __CmdLineDefineStruct
{
	int nIndex;
	TCHAR* szString;
	TCHAR* szComment;
	TCHAR* szCommentEn;
};

class CDNCheatCommand:public TBoostMemoryPool<CDNCheatCommand>
{
private:
	CDNUserSession* m_pSession;
	// CDNUserSendManager* m_pSendManager;

	std::wstring _GetUserName();
	CDNUserQuest* _GetQuest();

public:
	bool m_bLucky;
public:
	CDNCheatCommand(CDNUserSession *pUserSession);
	virtual ~CDNCheatCommand();

	int	Command(std::wstring& szCheat);
	bool OnCommand(int nCmdID, std::vector<std::wstring>& tokens);

private:

	bool isValidAdmin(std::wstring cName);

	bool _bCommandCheatSet( std::vector<std::wstring>& tokens );

};