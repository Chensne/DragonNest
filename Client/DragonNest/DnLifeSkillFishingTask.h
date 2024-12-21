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
	void OnRecvFishingPlayer( SCFishingPlayer* pPacket );		// ���� ���� �ɸ��� ���� ����
	void SendFishingReady();									// ���� �غ� ��û
	void OnRecvFishingReady( SCFishingReady* pPacket );			// ���� �غ� �Ϸ� ����

	void SendFishingStart();									// ���� ������ ��û
	void OnRecvCastBaitResult( SCCastBait* pPacket );			// ���� ���� ���� ����
	void OnRecvFishingStart( SCFishingPattern* pPacket );		// ���� ���� �ް� ����
	void OnRecvFishingSync( SCFishingSync* pPacket );			// ���� ���� ��ũ ���߱�

	void OnRecvStopFishingResult( SCStopFishing* pPacket );		// ���� ��ҽ� ���� ����
	void OnRecvFishingEnd();									// ���� ����
	void OnRecvFishingResult( SCFishingReward* pPacket );		// ���� ��� ����
	
	void DoFishingEnd();						// ���� ���� ����

	void SendFishingCancel( bool bRevisePos );	// ���� ��Ҹ� ������ �뺸

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
	int CheckFishingArea();							// ���������� �˻��ؼ� �ùٸ� ���� �����̸� ��������ID�� ���� �Ұ��� ������ -1 ����
	void SendAction( eActor::eCSActor eAction );

private:
	eFishingState m_eFishingState;			// ���� ����

	bool		m_bRequestFishingReady;
	bool		m_bRunningFishingTask;		// ���� �½�ũ ����
	bool		m_bActionNormalStand2;		// �ڵ����ÿ��� 2��° ��ֽ��ĵ� ���� ������ üũ
	bool		m_bRecieveFishingEnd;		// ������ ���� ���� ������ �޾Ҵ°�?
#ifdef PRE_ADD_CASHFISHINGITEM
	bool		m_bCashFishingRod;			// ���˴밡 ĳ������ ���
	bool		m_bCashFishingBait;			// �̳��� ĳ������ ���
	int			m_nFishingRodItemID;		// ���˴� ������ ���̵�
#endif // PRE_ADD_CASHFISHINGITEM
	int			m_nFishingRodIndex;			// ���˴� �κ��丮 �ε���
	int			m_nFishingBaitIndex;		// ���� �̳� �κ��丮 �ε���
//	float		m_fFishingRandomStandTime;	// �����ϰ� 2��° ���ĵ嵿�� ���ϴ� �ð�
	float		m_fRotate;					// Fishing Area�� ����
	EtVector3	m_vecPlayerStartPos;		// �ɸ��� ���� ������ ��ġ (����<->�ڵ� ��ȯ �� ĳ���� ������ �̵��Ǵ� �� ������)
	EtVector2	m_vecAreaDir;				// ������ ���� (Event Area�� ����)
	SCFishingReward	m_stFishingReward;		// ���� ��� ����

#ifdef PRE_ADD_FISHING_RESULT_SOUND
	int m_nFishingSuccessSound;
	int m_nFishingFailSound;
	EtSoundChannelHandle m_hFishingSound;
#endif
};

#define GetLifeSkillFishingTask()		CDnLifeSkillFishingTask::GetInstance()

