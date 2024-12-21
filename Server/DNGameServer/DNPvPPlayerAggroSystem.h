
#pragma once

#include "DNAggroSystem.h"

class CPvPScoreSystem;
class CDNPvPPlayerAggroSystem : public CDNAggroSystem, public TBoostMemoryPool<CDNPvPPlayerAggroSystem>
{
public:

	CDNPvPPlayerAggroSystem( DnActorHandle hActor );
	virtual ~CDNPvPPlayerAggroSystem();

	void					Die( CPvPScoreSystem* pScoreSystem, DnActorHandle hHitter );

	virtual void			OnProcessAggro( const LOCAL_TIME LocalTime, const float fDelta );
	virtual DnActorHandle	OnGetAggroTarget( bool& bIsProvocationTarget, DnActorHandle hExceptActor=CDnActor::Identity(), DNVector(DnActorHandle)*	vTarget=NULL );
	virtual void			OnDamageAggro( DnActorHandle hActor, CDnDamageBase::SHitParam& HitParam, int iDamage );
	virtual void			OnStateBlowAggro( DnBlowHandle hBlow );
	virtual bool			bOnCheckPlayerBeginStateBlow( CDnPlayerActor* pPlayer );
	virtual void			OnAggroRegulation( DnActorHandle hActor, int& iAggroValue );

protected:

	UINT	m_uiMaxAggroValue;
};
