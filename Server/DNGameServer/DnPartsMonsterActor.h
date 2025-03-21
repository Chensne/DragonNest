#pragma once

#include "DnMonsterActor.h"
#include "MAWalkMovementNav.h"
#include "MASingleBody.h"
#include "MAMultiDamage.h"
#if defined( PRE_ADD_LOTUSGOLEM )
#include "MAAiScript.h"
#endif // #if defined( PRE_ADD_LOTUSGOLEM )

class CDnPartsMonsterActor : public TDnMonsterActor< MASingleBody, MAMultiDamage >
{
public:
	CDnPartsMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnPartsMonsterActor();

#if defined( PRE_ADD_LOTUSGOLEM )
	void ProcessPartsAI( const std::vector<AIPartsProcessor>& vData );
#endif // #if defined( PRE_ADD_LOTUSGOLEM )

	virtual MAMovementBase* CreateMovement();
	virtual void CmdMoveNavi( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );

public:

	virtual bool Initialize();
	virtual bool IsPartsMonsterActor(){ return true; }
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void UpdateNaviMode();
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );

	virtual void	OnMultiDamage(MonsterParts& Parts) { }
	virtual float	CalcDamage( CDnDamageBase *pHitter, SHitParam &HitParam );

	virtual bool	IsLimitAction( const char* pszActionName );
	virtual bool	IsLimitAction( DnSkillHandle hSkill );

	virtual bool	bIsDestroyParts( const UINT uiTableID );
	virtual bool	bIsDestroySetParts( const UINT uiSetID );
	virtual bool	OnbIsCalcDamage( const int iSkillIndex ){ return true; }
	virtual void	_OnAddMonsterParts(const MonsterParts::_Info& partsInfo)	{}

#if defined(PRE_FIX_59347)
	virtual void ApplyPartsDamage(float& fDamage, DnActorHandle hHitterActor);
#endif // PRE_FIX_59347

protected:
	virtual void	_OnRefreshParts( MonsterParts* pParts );
	virtual void	_OnDestroyParts( MonsterParts* pParts );

private:
	
	std::list<std::pair<UINT,float>>	m_listDestroySetParts;
	std::list<MonsterParts*>			m_listDestroyParts;

	void			_ProcessParts( const float fDelta );
	void			_OnPartsDamage( CDnDamageBase* pHitter, SHitParam& HitParam, MonsterParts& Parts );
	void			_SendPartsHP( MonsterParts* pParts, UINT uiSessionID=0 );

protected:

	LOCAL_TIME m_nLastUpdateNaviTime;

#if defined(PRE_FIX_66687)
public:
	void ApplyPartsHP(float fRate);
#endif // PRE_FIX_66687
};
