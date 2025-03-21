#pragma once
#include "idnskillprocessor.h"
#include "DnSkill.h"


// 알케미스트의 "칵테일" 스킬에서 사용.
// 스킬에 설정된 상태효과 데이터들을 현재 버블 갯수에 따라 사용하지 않을 것들을 제거시키고
// 스킬 사용이 끝나면 원래대로 복구 시킨다.
// 버블을 소모하지 않음. 버블을 소모해야 한다면 새로 발현 타입 만들어야 함.
class CDnStateEffectApplyOnOffByBubbleProcessor : public IDnSkillProcessor, 
												  public TBoostMemoryPool< CDnStateEffectApplyOnOffByBubbleProcessor >
{
private:
	struct S_STATE_EFFECT_ON_OFF
	{
		int iBubbleID;
		int iBubbleCount;
		vector<int> vlApplyList;

		S_STATE_EFFECT_ON_OFF( void ) : iBubbleID( 0 ), iBubbleCount( 0 ) {};
	};

	DNVector(S_STATE_EFFECT_ON_OFF) m_vlApplyOnOff;
	DNVector(CDnSkill::StateEffectStruct) m_vlSEBackup;

public:
	CDnStateEffectApplyOnOffByBubbleProcessor( DnActorHandle hActor, const char* pArg );
	virtual ~CDnStateEffectApplyOnOffByBubbleProcessor(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void SelectAvailableSE( DNVector(CDnSkill::StateEffectStruct)& vlSEList );
	void RestoreSEList( DNVector(CDnSkill::StateEffectStruct)& vlSEList );

	int GetNumArgument( void ) { return 1; };
};
