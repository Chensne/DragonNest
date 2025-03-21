#pragma once

#include "Task.h"
#include "MessageListener.h"


class CDnRadioMsgTask : public CTask, public CTaskListener, public CSingleton<CDnRadioMsgTask>
{
	enum
	{
		RADIOMSG_DELAY = 3000,	// 3초에 한번씩만 전송 가능
#if defined(PRE_ADD_ACADEMIC)
	//#ifdef PRE_ADD_KALI
	//	NUM_JOB = 6,
	//#else
	//	NUM_JOB = 5,
	//#endif

#ifdef PRE_ADD_MACHINA
	NUM_JOB = 9,
#else
	NUM_JOB = 7,
#endif
#else
		NUM_JOB = 4,
#endif	// #if defined(PRE_ADD_ACADEMIC)
		//rlkt CHECK!!
		NUM_SOUNDMSG = 6 ,       // 사운드가 포함된 매크로의 갯수.
	};

public:
	CDnRadioMsgTask();
	virtual ~CDnRadioMsgTask();

	struct SRadioMsgInfo {
		int nID;
		tstring szName;
		int nIconID;
		int nMacroNum;	// 0이면 RadioMsg, 1~4면 매크로.
		tstring szMsg[NUM_JOB];
		std::vector<int> vecMsgSndIndex[NUM_JOB];
		EtTextureHandle hHeadIconTex;
	};

	struct STransformMsgInfo // 변신된 액터는 메세지와 사운드만 교체해준다.
	{
		std::vector<int> vecMsgSndIndex;
		std::vector<int> vecMsgIndex;
	};

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void OnDisconnectTcp(bool bValidDisconnect) {}
	void OnDisconnectUdp(bool bValidDisconnect) {}
	void OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize);

	void Initialize();
	void Finalize();

	DWORD GetRadioMsgCount() { return (DWORD)m_pVecRadioMsgInfo.size(); }
	SRadioMsgInfo *GetRadioMsgInfoFromIndex( DWORD nIndex );
	SRadioMsgInfo *GetRadioMsgInfoFromID( int nID );

	void UseRadioMsg( USHORT nRadioID );

	void LoadTransformSound();
	void ClearTransformSound();
	void EnableTransformSound(int nTransformID,bool bEnable); // 변신할때의 사운드.

protected:
	std::vector<SRadioMsgInfo *> m_pVecRadioMsgInfo;
	void OnRecvRadioMsgMessage(int nSubCmd, char * pData, int nSize);
	//recv
	void OnRecvUseRadio(SCUseRadio * pPacket);

	LOCAL_TIME m_RadioMsgSendTime;
	EtSoundChannelHandle m_hCurVoiceChannel;
	int m_nCurSessionID;

	std::map< int , STransformMsgInfo > m_mapTransformRadioMsgInfo;
	bool m_bEnableTransformRadioMsg;
	int m_nTrasnformMsgIndex;
};

#define GetRadioMsgTask()	CDnRadioMsgTask::GetInstance()