#pragma once
#include "DnMessageList.h"
#include "DnMessageData.h"

#if 0

class CDnMessageManager
{
public:
	CDnMessageManager(void);
	virtual ~CDnMessageManager(void);

protected:
	CDnMessageData m_MessageData;
	//CDnMessageList m_GlobalMessage;
	CDnMessageList m_TaskMessage;

public:
	bool AddMessage( int nMainCmd, int nSubCmd, int nStringIndex = 0 );
	bool RemoveMessage( int nMainCmd, int nSubCmd );

public:
	static CDnMessageManager& GetInstance();
};

#define GetMessageManager()		CDnMessageManager::GetInstance()

#endif