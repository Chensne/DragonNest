#pragma once
#include "DnSkill.h"


class CDnSkillPropComponent
{
private:
	CDnSkill::SkillInfo m_SkillInfo;
	vector<CDnSkill::StateEffectStruct> m_vlStateEffectList;
	DnSkillHandle m_hSkill;
	DnActorHandle m_hSkillUser;

public:
	CDnSkillPropComponent(void);
	~CDnSkillPropComponent(void);

	const CDnSkill::SkillInfo& GetSkillInfo( void ) { return m_SkillInfo; };
	int GetNumStateEffect( void ) { return (int)m_vlStateEffectList.size(); };
	const CDnSkill::StateEffectStruct* GetStateEffectInfo( int iIndex ) { return &(m_vlStateEffectList.at(iIndex)); };

	void Initialize( int iSkillID, int iSkillLevel );
	void OnSignal( DnActorHandle hActor, DnPropHandle hProp, SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, 
				   LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	void SetSkillUser( DnActorHandle hActor ) { m_hSkillUser = hActor; m_SkillInfo.hSkillUser = m_hSkillUser; };

#ifdef PRE_ADD_HIT_PROP_ADD_SE
	//void ApplyStateBlowsToTheseTargets( DnActorHandle hPropActor, DNVector(DnActorHandle)& vlhActors );
	void ApplyStateBlowsToTheseTargets( DnActorHandle hPropActor, DNVector(DnActorHandle)& vlhActors, HitStruct * pStruct=NULL );
#endif // #ifdef PRE_ADD_HIT_PROP_ADD_SE
};
