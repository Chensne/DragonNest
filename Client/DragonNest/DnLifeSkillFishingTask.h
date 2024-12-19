#pragma once

#include "Task.h"
#include "MessageListener.h"


#define FISHING_RANDOM_STAND_TIME	80.0f

enum eFishingState { STATE_NONE = 0, STATE_FISHING, STATE_FISHING_END, STATE_SHOW_RESULT };

class CDnLifeSkillFishingTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDnLifeSkillFishingTask>
{
public:
	CDnLifeSkillFishingTask();
	virtual ~CDnLifeSkillFishingTask();

public:
	bool Initialize();
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

public:
	void OnRecvFishingPlayer( SCFishingPlayer* pPacket );		// 낚시 중인 케릭터 정보 받음
	void SendFishingReady();									// 낚시 준비를 요청
	void OnRecvFishingReady( SCFishingReady* pPacket );			// 낚시 준비 완료 받음

	void SendFishingStart();									// 낚시 시작을 요청
	void OnRecvCastBaitResult( SCCastBait* pPacket );			// 낚시 시작 오류 받음
	void OnRecvFishingStart( SCFishingPattern* pPacket );		// 낚시 패턴 받고 시작
	void OnRecvFishingSync( SCFishingSync* pPacket );			// 낚시 도중 싱크 맞추기

	void OnRecvStopFishingResult( SCStopFishing* pPacket );		// 낚시 취소시 오류 받음
	void OnRecvFishingEnd();									// 낚시 끝남
	void OnRecvFishingResult( SCFishingReward* pPacket );		// 낚시 결과 받음
	
	void DoFishingEnd();						// 낚기 동작 시작

	void SendFishingCancel( bool bRevisePos );	// 낚시 취소를 서버로 통보

	eFishingState GetFishingState() { return m_eFishingState; }

	void SendPullingRodElapsedTime( bool bPullingRod );
	bool IsRecieveFishingEnd() { return m_bRecieveFishingEnd; }

	bool IsNowFishing();
	bool IsRequestFishingReady() { return m_bRequestFishingReady; }
#ifdef PRE_ADD_CASHFISHINGITEM
	int GetFishingRodItemID() { return m_nFishingRodItemID; }
#endif // PRE_ADD_CASHFISHINGITEM

private:
	void ResetFishing( bool bRevisePos );
	bool CheckFishingTool();
	int CheckFishingArea();							// 낚시지역을 검사해서 올바른 낚시 지역이면 낚시지역ID를 리턴 불가능 지역은 -1 리턴
	void SendAction( eActor::eCSActor eAction );

private:
	eFishingState m_eFishingState;			// 낚시 상태

	bool		m_bRequestFishingReady;
	bool		m_bRunningFishingTask;		// 낚시 태스크 동작
	bool		m_bActionNormalStand2;		// 자동낚시에서 2번째 노멀스탠드 동작 중인지 체크
	bool		m_bRecieveFishingEnd;		// 서버로 부터 낚시 끝남를 받았는가?
#ifdef PRE_ADD_CASHFISHINGITEM
	bool		m_bCashFishingRod;			// 낚싯대가 캐시템일 경우
	bool		m_bCashFishingBait;			// 미끼가 캐시템일 경우
	int			m_nFishingRodItemID;		// 낚싯대 아이템 아이디
#endif // PRE_ADD_CASHFISHINGITEM
	int			m_nFishingRodIndex;			// 낚싯대 인벤토리 인덱스
	int			m_nFishingBaitIndex;		// 낚시 미끼 인벤토리 인덱스
//	float		m_fFishingRandomStandTime;	// 랜덤하게 2번째 스탠드동작 취하는 시간
	float		m_fRotate;					// Fishing Area의 방향
	EtVector3	m_vecPlayerStartPos;		// 케릭터 낚시 시작전 위치 (수동<->자동 전환 시 캐릭터 앞으로 이동되는 것 보정용)
	EtVector2	m_vecAreaDir;				// 물가쪽 방향 (Event Area의 방향)
	SCFishingReward	m_stFishingReward;		// 낚시 결과 정보

#ifdef PRE_ADD_FISHING_RESULT_SOUND
	int m_nFishingSuccessSound;
	int m_nFishingFailSound;
	EtSoundChannelHandle m_hFishingSound;
#endif
};

#define GetLifeSkillFishingTask()		CDnLifeSkillFishingTask::GetInstance()

