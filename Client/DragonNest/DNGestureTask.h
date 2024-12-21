
#pragma once

#include "Task.h"
#include "MessageListener.h"
#include "DNProtocol.h"
#include "DNPacket.h"
#include "MIInventoryItem.h"

class CDnGestureTask : public CTask, public CTaskListener, public CSingleton<CDnGestureTask>
{
public:
	CDnGestureTask();
	virtual ~CDnGestureTask();

	enum
	{
		NUM_KEYWORD = 9 //RLKT PROBLEM!!
	};

#if defined(PRE_ADD_GUILD_GESTURE)
	enum eGestureType
	{
		GestureType_Normal = 0,
		GestureType_GuildWarWin = 1,
		GestureType_GuildReward = 2,
	};
#endif // PRE_ADD_GUILD_GESTURE

	struct SGestureInfo
	{
		bool bUsable;	// 무료거나 결제했다면 서버로부터 리스트받아 true로 설정.
		int nID;
		tstring szName;
		int nIconID;
		string szActionName;
		int nMsgID;
		tstring szKeyword[NUM_KEYWORD];
		tstring szDescription;
		int nUnlockLevel;	// -1이면 캐시. -2면 길드우승스킬
		string szReactionName1;
		string szReactionName2;

#if defined(PRE_ADD_GUILD_GESTURE)
		eGestureType _Type;	//제스처 타입
#endif // PRE_ADD_GUILD_GESTURE

	};

	bool Initialize();
	void Finalize();

	void OnDisconnectTcp(bool bValidDisconnect) {}
	void OnDisconnectUdp(bool bValidDisconnect) {}

	void OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize);

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	DWORD GetVisibleGestureCount();
	SGestureInfo *GetVisibleGestureInfoFromIndex( DWORD nIndex );
	SGestureInfo *GetGestureInfoFromID( int nID );

	// localplayer 사용
	void UseGesture( int nGestureID, bool bUseByChat = false );

	// 채팅명령어로 사용
	void UseGestureByChat( LPCWSTR wszMessage );

	// 다른 플레이어 응답
	void OnRecvCmdAction( LPCWSTR wszActorName, int nGestureID );

	// 이젠 캐시아닌 제스처에 대해선 서버 클라 리스트 주고 받는 거 없이 스스로 처리한다.
	void RefreshGestureList( int nCurrentLevel, bool bOnLevelUp = false );

protected:

	// 도배 금지
	float m_fPaperingRemainTime;
	std::list<float> m_listRecentChatTime;
	bool CheckPapering();

	void OnRecvGestureMessage(int nSubCmd, char *pData, int nSize);
	void OnRecvGuildMessage(int nSubCmd, char *pData, int nSize);

	virtual void OnRecvGestureList( SCGestureList *pPacket );
	virtual void OnRecvGestureAdd( SCGestureAdd *pPacket );
	virtual void OnRecvUseGesture( SCUseGesture *pPacket );

	virtual void OnRecvGuildWarPreWin( SCGuildWarPreWin *pPacket );
	virtual void OnRecvGuildWarWinSkill( SCGuildWarWinSkill *pPacket );
	virtual void OnRecvGuildWarUserWinSkill( SCGuildWarUserWinSkill *pPacket );
	int m_nRequestGuildWarSkillID;

	SGestureInfo *GetGestureInfoFromActionName( const char *szActionName );
	SGestureInfo *GetGestureInfoFromGestureName( LPCWSTR wszName );
	SGestureInfo *GetGestureInfoFromKeyword( LPCWSTR wszKeyword );

	std::vector<SGestureInfo *> m_pVecGestureInfo;

	// 아이템태스크를 보니 리스트를 받고있는 도중엔 퀵슬롯 Add요청을 할 수 없도록 되어있다.
	// 아이템태스크가 문제없이 작동하니 그대로 따라가기로 한다.
	bool m_bRequestQuickSlot;

	void OnRecvGuildRewardGestureList( SCGestureList *pPacket );
};

#define GetGestureTask()	CDnGestureTask::GetInstance()