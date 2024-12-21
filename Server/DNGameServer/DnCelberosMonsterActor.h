#pragma once


#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"

#include "DnPartsMonsterActor.h"

class CDnCelberosMonsterActor : public CDnPartsMonsterActor, public TBoostMemoryPool< CDnCelberosMonsterActor >
{
public:
	CDnCelberosMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnCelberosMonsterActor();

protected:

	float m_fShakeDelta;

	enum StateBuf
	{
		IceBuf = 0,
		FireBuf,
		DarkBuf,
		MaxBufCount
	};

	enum DeadActionDefType
	{
		ActionName,
		ActionBoneName,
		FixBoneName,
		MaxDeadActionDef
	};

	struct SDeadActorActionInfo
	{
		std::string names[MaxDeadActionDef];

		void Set(const std::string& act, const std::string& actBone, const std::string& fixBone)
		{
			names[ActionName]		= act;
			names[ActionBoneName]	= actBone;
			names[FixBoneName]		= fixBone;
		}
	};

	bool m_bBufDieArray[MaxBufCount];
	std::map<StateBuf, SDeadActorActionInfo>	m_DeadActionInfoList;

public:
	virtual bool Initialize();

	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );

	virtual bool CheckDamageAction( DnActorHandle hActor );
	virtual void OnDrop( float fCurVelocity );

	virtual bool OnPreAiProcess(const char* szPrevAction, LOCAL_TIME time);
	virtual void OnChangeActionQueue( const char *szPrevAction );
	virtual void OnFinishAction(const char* szPrevAction, LOCAL_TIME time);
	virtual bool OnbIsCalcDamage( const int iSkillIndex );

	void		 CmdMixPartsDeadAction();
	void		 CmdMixPartsDeadAction(StateBuf state);
	StateBuf	 GetBufStateByPartsID(int partNum) const;

	virtual void _OnAddMonsterParts(const MonsterParts::_Info& partsInfo);
	virtual void _OnDestroyParts(MonsterParts* pParts);
	virtual void _OnRefreshParts(MonsterParts* pParts);
};

