#pragma once
#include "sdkconfig.h"
#include <string>
#include <map>
#include "SmartPtr.h"

using namespace std;
class CServerInterface;

class PTSDK_STUFF CSessionInfo: public RefCount
{
protected:
	CSessionInfo(void);
	CSessionInfo(int id);
	~CSessionInfo(void);

friend class CServerInterface;

public:
	enum STATE{ begin = 0, end = -1,
	ptAuth = 1,
	ekeyAuth,
	questReservePwd,
	checkPtid,
	changePwd,
	changeGameId,
	ecardAuth,
	questECardNo
	};
		
	int GetState();

	unsigned int GetSessionId();

	void PutValue(const char * valueName, const char * value);
	void PutValue(const char * valueName, int value);
	void GetValue(const char * valueName, int &value);

	const char* GetValue(const char * valueName);
	int GetValueInt(const char* valueName);

	void Clean();

protected:	
	map< string , string > m_ContentMap;
	int m_State;
	unsigned int m_SessionId;
	CWithLock m_lock;

	void GetValue(const char * valueName, string &value);
};

typedef SmartPtr<CSessionInfo> CSessionPtr;
