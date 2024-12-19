#pragma once

#include "DNFieldBlock.h"
#include "DNFieldData.h"

const int VIEWRANGE = 1800;		// 시야처리

typedef vector<CDNFieldBlock*> TVecBlocks;

class CDNNpcObject;
class CDNNpcObjectManager;
class CDNField
{
protected:
	//Channels
	int m_nChannelID;
	int m_nChannelAtt;
	int m_nChannelMaxUser;

	int m_nMapIndex;
	// int m_nMapArrayIndex;

	int m_nMinX, m_nMinZ;
	int m_nMaxX, m_nMaxZ;

	UINT m_nWidth, m_nHeight;
	UINT m_nWidthNo, m_nHeightNo;

	TMapInfo* m_pMapInfo;
	CDNFieldData* m_pFieldData;

	TVecBlocks m_FieldBlocks;
	int m_nPermitLevel;

public:
	CDNNpcObjectManager* m_pNpcObjectManager;

public:
	CDNField(void);
	virtual ~CDNField(void);

	bool Init(int nChannelID, int nChannelAtt, int nChannelMaxUser, int nMapIndex, const TMapInfo *pMapInfo, CDNFieldData *pFieldData, int nPermitLevel);
	bool Final();

	bool EnterObject(CDNBaseObject *pObj);
	bool LeaveObject(CDNBaseObject *pObj);
	void FinalUser( CDNBaseObject* pObj );

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	void GetWaitUserList( std::list<CDNUserSession*>& UserList, int iLimitLevel=-1, BYTE cPvPChannelType = static_cast<BYTE>(PvPCommon::RoomType::max) );
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	void GetWaitUserList( std::list<CDNUserSession*>& UserList, int iLimitLevel=-1 );
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	virtual int SendFieldMessage(CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData);
	void DoUpdate(DWORD CurTick);

	CDNFieldBlock* GetFieldBlock(TPosition *pPos);
	void GetViewRange(TPosition *pCurPos, int &sxp, int &szp, int &exp, int &ezp);
	void GetDoubleViewRange(TPosition *pCurPos, int &sxp, int &szp, int &exp, int &ezp);

	void RequestGenerationNpc(int nNpcID, TPosition *pCurPos, float fRotate);	// 치트용?

	bool AddNpcObject(CDNNpcObject *pNpcObj);
	CDNNpcObject* GetNpcObjectByNpcObjID(UINT nNpcObjID);

	void SendFieldNotice(int cLen, const WCHAR *pwszChatMsg, int nShowSec);
	void SendChannelChat(int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg, int nRet = ERROR_NONE);
	void SendDebugChat(const WCHAR* szMsg);

	inline CDNFieldData* GetFieldData() { return m_pFieldData; }
	inline int GetMapIndex() { return m_nMapIndex; }
	inline int GetChnnelID() { return  m_nChannelID; }
	inline int GetChannelAtt() { return m_nChannelAtt; }

	// 
	bool bIsUseLastMapIndex(){ return bIsPvPVillage() || bIsGMVillage() || bIsDarkLairVillage() || bIsFarmVillage(); }

	virtual bool bIsPvPVillage(){ return false; }
	virtual bool bIsPvPLobby(){ return false; }
	virtual bool bIsDarkLairVillage(){ return false; }
	virtual bool bIsFarmVillage() { return false; }
	virtual GameTaskType::eType GetGameTaskType(){ return GameTaskType::Normal; }
	int GetPermitLevel() { return m_nPermitLevel; }
	bool bIsGMVillage(){ return GetChannelAtt()&GlobalEnum::CHANNEL_ATT_GM ? true : false; }
#if defined(PRE_ADD_DWC)
	virtual bool bIsDWCVillage(){ return false; }
#endif
};
