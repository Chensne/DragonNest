#pragma once

#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"
#include "MAMultiDamage.h"


struct BasiliskProperty 
{
	// 아머 하나당 내구도
	float fArmorDurability;

	// 아머 하나당 방어력 퍼센트
	float	fArmorDefencePercent;

	// 아머 갯수
	float fArmorCnt;

	// 원거리 공격 방어력 퍼센트
	float fRangeDefencePercent;

	// 평소에 맞았을때 아머 내구도 깍이는 값
	float fArmorDurabilityDamage;

	// 땅에 떨어졌을때 아머가 깍이는 내구도 
	float	fFall_to_the_ground_damage;

	// 다리 갯수
	float	fLegCnt;

	// 다리 한대 맞았을때 입는 데미지
	float	fLegDurabilityDamage;

	// 다리 하나당 내구도
	float	fLegDurability;

	// 다리의 내구도 시간당 회복값
	float fLegDurabilityRepairValue;

	// 다리의 내구도 회복할 틱카운트
	float fLegDurabilityRepairTime;

	// 다리 내구도가 얼마나 떨어졌을때 밸런스를 잃어버리냐.
	float	fLoseBalanceValue;

	// 다리 내구도가 얼마나 떨어졌을때 밸런스 잃어버린 상태에서 떨어지냐
	float	fFallDownValue;

};

class CDnBasiliskMonsterActor : public TDnMonsterActor< MASingleBody, MAMultiDamage >
{
public:
	CDnBasiliskMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnBasiliskMonsterActor();

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new MAStaticMovement();
		return pMovement;
	}

	virtual bool Initialize();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam );
	virtual void ProcessDie( LOCAL_TIME LocalTime, float fDelta );
	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true, bool bFromStateBlow = false, bool bSkillChain = false );

	virtual float CalcDamage( DnActorHandle hHitter, SHitParam &HitParam  );

	// Additional Packet Message
	// Damage Packet
	virtual void OnDamageWriteAdditionalPacket( CPacketCompressStream *pStream );
	virtual void OnDamageReadAdditionalPacket( CPacketCompressStream *pStream );

public:
	void SetMeshDiffuseColor(const char* szMeshName, EtColor& clr);
	void ClearMeshDiffuseColor() { m_MeshDiffuseColorMap.clear(); }

	bool LoadShellMesh();

	void OnFallToTheGround();

	bool	IsLoseBalance() 
	{
		if ( m_fLegTotalDurablility <= m_Property.fLegDurability*m_Property.fLegCnt-m_Property.fLoseBalanceValue )
			return true; 

		return false;
	}
	bool	IsFallDown() 
	{
		if ( m_fLegTotalDurablility <= m_Property.fLegDurability*m_Property.fLegCnt-m_Property.fLoseBalanceValue-m_Property.fFallDownValue )
			return true; 

		return false;
	}

private:
	void _InitRegionDamage();
	bool _LoadProperty(const char* szFile);
	float _CalcDamage( int nOverTurned, int AttackType, int AttackFoot, float nOriginalDamage, int nArmorCnt );

protected:
	float m_fShakeDelta;
	int m_nCustomParamIndex;
	std::string m_szPrevAction;

	
	std::map<std::string, EtColor> m_MeshDiffuseColorMap;
	std::vector<EtAniObjectHandle>	m_VecShellObject;

	lua_State* m_pLua;
	BasiliskProperty	m_Property;

	struct LEG_INFO
	{
		std::string szBoneName;
		std::string szSubMeshName;
		//float fLegDurability;		// 각 다리의 내구도	
	};

	typedef std::map<std::string, LEG_INFO>		LEG_INFO_DAMAGE_MAP;
	typedef LEG_INFO_DAMAGE_MAP::iterator		LEG_INFO_DAMAGE_MAP_IT;

	LEG_INFO_DAMAGE_MAP	m_LegDamageMap;
	
	// 전체 껍질 내구도 합 ( 보호막 에너지... )
	float	m_fArmorTotalDurability;

	// 살아있는 (?) 껍질 갯수
	int		m_nArmorCnt;

	// 다리 에너지...
	float	m_fLegTotalDurablility;

	// 회복한 마지막 시간
	LOCAL_TIME	m_nLastLegRepairTime;


	
};