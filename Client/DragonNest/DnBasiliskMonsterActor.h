#pragma once

#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"
#include "MAMultiDamage.h"


struct BasiliskProperty 
{
	// �Ƹ� �ϳ��� ������
	float fArmorDurability;

	// �Ƹ� �ϳ��� ���� �ۼ�Ʈ
	float	fArmorDefencePercent;

	// �Ƹ� ����
	float fArmorCnt;

	// ���Ÿ� ���� ���� �ۼ�Ʈ
	float fRangeDefencePercent;

	// ��ҿ� �¾����� �Ƹ� ������ ���̴� ��
	float fArmorDurabilityDamage;

	// ���� ���������� �ƸӰ� ���̴� ������ 
	float	fFall_to_the_ground_damage;

	// �ٸ� ����
	float	fLegCnt;

	// �ٸ� �Ѵ� �¾����� �Դ� ������
	float	fLegDurabilityDamage;

	// �ٸ� �ϳ��� ������
	float	fLegDurability;

	// �ٸ��� ������ �ð��� ȸ����
	float fLegDurabilityRepairValue;

	// �ٸ��� ������ ȸ���� ƽī��Ʈ
	float fLegDurabilityRepairTime;

	// �ٸ� �������� �󸶳� ���������� �뷱���� �Ҿ������.
	float	fLoseBalanceValue;

	// �ٸ� �������� �󸶳� ���������� �뷱�� �Ҿ���� ���¿��� ��������
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
		//float fLegDurability;		// �� �ٸ��� ������	
	};

	typedef std::map<std::string, LEG_INFO>		LEG_INFO_DAMAGE_MAP;
	typedef LEG_INFO_DAMAGE_MAP::iterator		LEG_INFO_DAMAGE_MAP_IT;

	LEG_INFO_DAMAGE_MAP	m_LegDamageMap;
	
	// ��ü ���� ������ �� ( ��ȣ�� ������... )
	float	m_fArmorTotalDurability;

	// ����ִ� (?) ���� ����
	int		m_nArmorCnt;

	// �ٸ� ������...
	float	m_fLegTotalDurablility;

	// ȸ���� ������ �ð�
	LOCAL_TIME	m_nLastLegRepairTime;


	
};