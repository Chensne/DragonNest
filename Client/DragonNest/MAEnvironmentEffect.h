#pragma once

#include "EtMatrixEx.h"
class CEtSoundChannel;
class MAEnvironmentEffect {
public:
	MAEnvironmentEffect();
	virtual ~MAEnvironmentEffect();

	enum ConditionEnum {
		Step,
		Landing,
		Down,
		Tumble,

		ConditionEnum_Amount = 4,
	};

protected:
	int m_nEnviActorTableID;
	int m_nEnviEffect[ConditionEnum_Amount][TileTypeEnum_Amount];
	char m_cEnviEffectType[ConditionEnum_Amount][TileTypeEnum_Amount];

	std::vector<int> m_nVecEnviSound[ConditionEnum_Amount][TileTypeEnum_Amount];
	float m_fEnviSoundVolume;
	float m_fEnviSoundRange;
	float m_fEnviSoundRolloff;

protected:
	void ReleaseEnvironmentEffect();
	
public:
	virtual bool Initialize( int nActorTableID );
	bool InitializeUsingEnvironmentEffect();

	bool CreateEnviEffectParticle( ConditionEnum Condition, TileTypeEnum TileType, MatrixEx *pCross );
	EtSoundChannelHandle CreateEnviEffectSound( ConditionEnum Condition, TileTypeEnum TileType, EtVector3 &vPos );
};
