#pragma once
#include "DnBlow.h"
//#include "DnState.h"
#include "DnActorState.h"

class CDnBasicBlow : public CDnBlow, public TBoostMemoryPool< CDnBasicBlow >
{
public:
	CDnBasicBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnBasicBlow(void);

	bool m_bNeedRefreshState;
	bool m_bRefreshAll;
	CDnActorState::AddBlowStateType m_eAddBlowStateType;
	CDnActorState::StateTypeEnum m_eRefreshState;

protected:
	CDnState m_State;

protected:
	CDnActorState::StateTypeEnum SetBlowState_001();
	CDnActorState::StateTypeEnum SetBlowState_002();
	CDnActorState::StateTypeEnum SetBlowState_003();
	CDnActorState::StateTypeEnum SetBlowState_004();
	CDnActorState::StateTypeEnum SetBlowState_005();
	CDnActorState::StateTypeEnum SetBlowState_006();
	CDnActorState::StateTypeEnum SetBlowState_007();
	CDnActorState::StateTypeEnum SetBlowState_008();
	CDnActorState::StateTypeEnum SetBlowState_009();
	CDnActorState::StateTypeEnum SetBlowState_010();
	CDnActorState::StateTypeEnum SetBlowState_015();
	CDnActorState::StateTypeEnum SetBlowState_017();
	CDnActorState::StateTypeEnum SetBlowState_019();
	CDnActorState::StateTypeEnum SetBlowState_020();
	CDnActorState::StateTypeEnum SetBlowState_021();
	CDnActorState::StateTypeEnum SetBlowState_022();
	CDnActorState::StateTypeEnum SetBlowState_023();
	CDnActorState::StateTypeEnum SetBlowState_024();
	CDnActorState::StateTypeEnum SetBlowState_026();
	CDnActorState::StateTypeEnum SetBlowState_028();
	CDnActorState::StateTypeEnum SetBlowState_029();
	CDnActorState::StateTypeEnum SetBlowState_032();
	CDnActorState::StateTypeEnum SetBlowState_033();
	CDnActorState::StateTypeEnum SetBlowState_034();
	CDnActorState::StateTypeEnum SetBlowState_035();
	CDnActorState::StateTypeEnum SetBlowState_036();
	CDnActorState::StateTypeEnum SetBlowState_037();
	CDnActorState::StateTypeEnum SetBlowState_038();
	CDnActorState::StateTypeEnum SetBlowState_039();
	CDnActorState::StateTypeEnum SetBlowState_048();
	CDnActorState::StateTypeEnum SetBlowState_049();
	CDnActorState::StateTypeEnum SetBlowState_058();
	CDnActorState::StateTypeEnum SetBlowState_059();
	CDnActorState::StateTypeEnum SetBlowState_061();
	CDnActorState::StateTypeEnum SetBlowState_064();
	CDnActorState::StateTypeEnum SetBlowState_075();
	CDnActorState::StateTypeEnum SetBlowState_076();
	CDnActorState::StateTypeEnum SetBlowState_082();
	CDnActorState::StateTypeEnum SetBlowState_087();
	CDnActorState::StateTypeEnum SetBlowState_088();
	CDnActorState::StateTypeEnum SetBlowState_089();
	CDnActorState::StateTypeEnum SetBlowState_090();
	CDnActorState::StateTypeEnum SetBlowState_093();
	CDnActorState::StateTypeEnum SetBlowState_094();
	CDnActorState::StateTypeEnum SetBlowState_123();
	CDnActorState::StateTypeEnum SetBlowState_124();
	CDnActorState::StateTypeEnum SetBlowState_125();
	CDnActorState::StateTypeEnum SetBlowState_126();
	CDnActorState::StateTypeEnum SetBlowState_127();
	CDnActorState::StateTypeEnum SetBlowState_128();
	CDnActorState::StateTypeEnum SetBlowState_200();
	CDnActorState::StateTypeEnum SetBlowState_201();
	CDnActorState::StateTypeEnum SetBlowState_202();
	CDnActorState::StateTypeEnum SetBlowState_203();
	CDnActorState::StateTypeEnum SetBlowState_234();

	void CalcRefreshTypes( void );

	void RefreshActorState();
	void AddBlowState();
	void SetBlowState();

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void DelBlowState();

	void SetModifyFloatValue( float fValue );
	const CDnState *GetState() { return &m_State; }

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

#ifdef _GAMESERVER
	void CheckHPMPFullWhenBegin();
#endif
};