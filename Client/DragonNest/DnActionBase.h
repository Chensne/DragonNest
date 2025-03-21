
#pragma once

#include "EtActionBase.h"
#include "Timer.h"
#include "SignalHeader.h"
#include "BackgroundLoader.h"

enum ButtonTypeEnum {
	LM_BUTTON = 0,
	RM_BUTTON = 1,
	WM_BUTTON = 2,
	Front_Key = 3,
	Back_Key = 4,
	Left_Key = 5,
	Right_Key = 6,
	Jump_Key = 7,
	Pick_Key = 8,
	Key_R = 9,
};

class CDnRenderBase;
class CDnActionBase : public CEtActionBase
{
public:
	CDnActionBase();
	virtual ~CDnActionBase();

protected:
	CDnRenderBase *m_pRender;
	std::vector<int> m_nVecAniIndexList;

	std::string m_szAction;
	LOCAL_TIME m_LocalTime;
	LOCAL_TIME m_ActionTime;
	LOCAL_TIME m_FirstActionTime;
	float m_fFrame;
	float m_fPrevFrame;
	MSDT_DECL(int) m_nActionIndex;
	std::string m_szActionQueue;
	float m_fQueueBlendFrame;
	float m_fQueueStartFrame;

	std::string m_szCustomAction;
	LOCAL_TIME m_CustomActionTime;
	int m_nCustomActionIndex;
	float m_fCustomPrevFrame;
	bool m_bCustomProcessSignal;

	float m_fActionQueueDelay;

	int m_nLoopCount;
	float m_fFps;
	static map<int, DnActorHandle> s_mapAcademicSummonMonsterPreload;

protected:
	void ResetActionBase();
	static void LoadSoundStruct( SoundStruct *pStruct );

public:
	// 한기: 이동 불가 상태효과에서 현재 진행중인 액션을 곧바로 취소시키기 위해서 protected 에서 public으로 뺍니다.
	virtual void SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop = false );

	bool Initialize( CDnRenderBase *pRender );
	virtual bool LoadAction( const char *szFullPathName );
	virtual void FreeAction();	
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex ) {}


#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	static void CheckInitSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase );
#endif
	static void CheckPreSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase );
	static void CheckPostSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase );

	void ProcessSignal( ActionElementStruct *pStruct, float fFrame, float fPrevFrame );

	void ReloadAction();

	const char *GetCurrentAction();
	int GetCurrentActionIndex() { return m_nActionIndex; };
	inline std::string& GetCurrentPlayAction() { return m_szAction; }

	float GetCurFrame() { return m_fFrame; }
	void CalcAniIndex();
	void CacheAniIndex();
	void GetCachedAniIndexList( std::vector<int>& vlIndex ) { vlIndex = m_nVecAniIndexList; };
	bool IsSignalRange( SignalTypeEnum Type, int nSignalIndex = -1 );
	int GetCachedAniIndex( const char *szActionName );

	void ProcessAction( LOCAL_TIME LocalTime, float fDelta );

	virtual void SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f );
	virtual void SetActionQueueDelay( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, int nDelay = 30 );

	void SetFPS( float fValue );
	float GetFPS();

	// 메인 액션과 다른 엑션을 돌릴때 사용
	virtual void SetCustomAction( const char *szActionName, float fFrame );
	bool IsCustomAction();
	void ResetCustomAction();
	bool IsCustomProcessSignal() { return m_bCustomProcessSignal; }

	virtual void OnNextAction( const char* szPrevAction, const char *szNextAction ) {}
	virtual void OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time) {}
	virtual void OnChangeAction( const char *szPrevAction ) {}
	virtual void OnChangeActionQueue( const char *szPrevAction ) {}
	virtual void OnChangeCustomAction() {}
	virtual void OnLoopAction( float fFrame, float fPrevFrame ) {}
	virtual void OnBeginProcessSignal() {}
	virtual void OnEndProcessSignal() {}

	// 둘다 쓸일은 없지만 컷신에서 Action 을 자체적으로 컨트롤하기땜에 필요함.
	void ClearActionQueue( void ) { m_szActionQueue.clear(); };
	void SetActionIndex( int nIndex ) { m_nActionIndex = nIndex; }
#ifdef PACKET_DELAY
	LOCAL_TIME GetLocalTime() { return m_LocalTime; }
#endif //PACKET_DELAY

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	bool SetEnableSignal( SignalTypeEnum SignalType, void* pSignalData );
#endif

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
	static void SetMemoryOptimize( int nStep );
#endif
};