#pragma once

#include "DNBaseObject.h"

const int FIELDBLOCKRANGE = 3000;

typedef list<CDNBaseObject*> TObjectList;

class CDNFieldBlock
{
private:
	TObjectList m_pObjectList;
	CSyncLock m_ObjectLock;

	int m_nMinX, m_nMinZ;	// x, y가 x, z이라는데?
	int m_nMaxX, m_nMaxZ;

public:
	int m_nXPoint, m_nZPoint;

public:
	CDNFieldBlock(void);
	virtual ~CDNFieldBlock(void);

	bool Init(int nXPoint, int nZPoint, int nMinX, int nMinZ);
	bool Final();

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	void GetWaitUserList( std::list<CDNUserSession*>& UserList, int iLimitLevel=-1, BYTE cPvPChannelType = static_cast<BYTE>(PvPCommon::RoomType::max) );
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	void GetWaitUserList( std::list<CDNUserSession*>& UserList, int iLimitLevel=-1 );
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	int SendFieldMessage (CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData);
	int SendUserFieldMessage (CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData);
	int SendPvPLobbyMessage( CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData );

	bool AddBaseObject(CDNBaseObject *pObj);
	bool DelBaseObject(CDNBaseObject *pObj);

	void SendFieldNotice(int cLen, const WCHAR *pwszChatMsg, int nShowSec);
	void SendChannelChat(int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg, int nRet = ERROR_NONE);
	void SendDebugChat(const WCHAR* szMsg);
};
