#pragma once

#include <MemoryShuffleDataType.h>

class CDnState {
public:
	CDnState();
	virtual ~CDnState();

	enum ElementEnum {
		Fire,
		Ice,
		Light,
		Dark,

		ElementEnum_Amount = 4,
	};

	enum ValueType 
	{
		ValueTypeNone,
		ValueTypeAbsolute = 0x01,
		ValueTypeRatio = 0x02,
	};

protected:
	ValueType m_ValueType;

	MSDT_DECL(int) m_nStrength;	// 힘
	MSDT_DECL(float) m_fStrengthRatio;

	MSDT_DECL(int) m_nAgility;		// 민첩
	MSDT_DECL(float) m_fAgilityRatio;

	MSDT_DECL(int) m_nIntelligence;// 지능
	MSDT_DECL(float) m_fIntelligenceRatio;

	MSDT_DECL(int) m_nStamina;		// 체력
	MSDT_DECL(float) m_fStaminaRatio;

	MSDT_DECL(INT64) m_nMaxHP;
	MSDT_DECL(float) m_fMaxHPRatio;

	MSDT_DECL(int) m_nMaxSP;
	MSDT_DECL(float) m_fMaxSPRatio;

	MSDT_DECL(int) m_nMoveSpeed;	// 이동속도
	MSDT_DECL(float) m_fMoveSpeedRatio;

	MSDT_DECL(int) m_nAttackP[2];	// 물리공격력
	MSDT_DECL(float) m_fAttackPRatio[2];

	MSDT_DECL(int) m_nAttackM[2];	// 마법공격력
	MSDT_DECL(float) m_fAttackMRatio[2];

	MSDT_DECL(int) m_nDefenseP;	// 물리방어력
	MSDT_DECL(float) m_fDefensePRatio;

	MSDT_DECL(int) m_nDefenseM;	// 마법방어력
	MSDT_DECL(float) m_fDefenseMRatio;

	MSDT_DECL(int) m_nStiff;
	MSDT_DECL(float) m_fStiffRatio;

	MSDT_DECL(int) m_nStiffResistance;
	MSDT_DECL(float) m_fStiffResistanceRatio;

	MSDT_DECL(int) m_nCritical;
	MSDT_DECL(float) m_fCriticalRatio;

	MSDT_DECL(int) m_nCriticalResistance;
	MSDT_DECL(float) m_fCriticalResistanceRatio;

	MSDT_DECL(int) m_nStun;
	MSDT_DECL(float) m_fStunRatio;

	MSDT_DECL(int) m_nStunResistance;
	MSDT_DECL(float) m_fStunResistanceRatio;

	MSDT_DECL(int) m_nFinalDamage;
	MSDT_DECL(float) m_fFinalDamageRatio;

	MSDT_DECL(float) m_fDownDelayProb;			// 다운 딜레이 (%)

	MSDT_DECL(float) m_fElementAttack[ElementEnum_Amount];	// 속성 공격력
	MSDT_DECL(float) m_fElementDefense[ElementEnum_Amount];	// 속성 방어력(저항력)

	MSDT_DECL(int) m_nRecoverySP; // SP 회복량
	MSDT_DECL(float) m_fRecoverySPRatio;

	MSDT_DECL(int) m_nSuperAmmor; // 스킬슈퍼아머
	MSDT_DECL(float) m_fSuperAmmorRatio;

	MSDT_DECL(int) m_nSpirit;
	MSDT_DECL(float) m_fSpiritRatio;

	MSDT_DECL(int) m_nSafeZoneMoveSpeed;	// 안전지역 이동속도
	MSDT_DECL(float) m_fSafeZoneMoveSpeedRatio;

	MSDT_DECL(float) m_fAddExp;				// #45525 획득 경험치 증가 비율

	MSDT_DECL(int) m_nAttackPMinMax[2];	
	MSDT_DECL(int) m_nAttackMMinMax[2];	

	static char *s_szElementStr[ElementEnum_Amount];

public:
	bool IsExistValueType( ValueType Type ) { return ( (int)m_ValueType & Type ) ? true : false; }
	ValueType GetValueType(){ return m_ValueType; }

	void ResetState();
	void CalcValueType();

	int GetStrength() { return m_nStrength; }
	int GetAgility() { return m_nAgility; }
	int GetIntelligence() { return m_nIntelligence; }
	int GetStamina() { return m_nStamina; }
	int GetAttackPMin() { return m_nAttackP[0]; }
	int GetAttackPMax() { return m_nAttackP[1]; }
	int GetAttackMMin() { return m_nAttackM[0]; }
	int GetAttackMMax() { return m_nAttackM[1]; }
	int GetDefenseP() { return m_nDefenseP; }
	int GetDefenseM() { return m_nDefenseM; }
	INT64 GetMaxHP() { return m_nMaxHP; }
	int GetMaxSP() { return m_nMaxSP; }
	int GetMoveSpeed() { return m_nMoveSpeed; }
	int GetStiff() { return m_nStiff; }
	int GetStiffResistance() { return m_nStiffResistance; }
	int GetCritical() { return m_nCritical; }
	int GetCriticalResistance() { return m_nCriticalResistance; }
	int GetStun() { return m_nStun; }
	int GetStunResistance() { return m_nStunResistance; }
	float GetElementAttack( ElementEnum Type ) { return m_fElementAttack[Type]; }
	float GetElementDefense( ElementEnum Type ) { return m_fElementDefense[Type]; }
	float GetDownDelayProb() { return m_fDownDelayProb; }
	int GetRecoverySP() { return m_nRecoverySP; }
	int GetSuperAmmor() { return m_nSuperAmmor; }
	int GetFinalDamage() { return m_nFinalDamage; }
	int GetSpirit() { return m_nSpirit; }
	int GetSafeZoneMoveSpeed() { return m_nSafeZoneMoveSpeed; }
	float GetAddExp() { return m_fAddExp; };

	inline void SetStrength( int nValue ) { m_nStrength = nValue; }
	inline void SetAgility( int nValue ) { m_nAgility = nValue; }
	inline void SetIntelligence( int nValue ) { m_nIntelligence = nValue; }
	inline void SetStamina( int nValue ) { m_nStamina = nValue; }
	inline void SetAttackPMin( int nValue ) { m_nAttackP[0] = nValue; }
	inline void SetAttackPMax( int nValue ) { m_nAttackP[1] = nValue; }
	inline void SetAttackMMin( int nValue ) { m_nAttackM[0] = nValue; }
	inline void SetAttackMMax( int nValue ) { m_nAttackM[1] = nValue; }
	inline void SetDefenseP( int nValue ) { m_nDefenseP = nValue; }
	inline void SetDefenseM( int nValue ) { m_nDefenseM = nValue; }
	inline void SetMaxHP( INT64 nValue ) { m_nMaxHP = nValue; }
	inline void SetMaxSP( int nValue ) { m_nMaxSP = nValue; }
	inline void SetMoveSpeed( int nValue ) { m_nMoveSpeed = nValue; }
	inline void SetStiff( int nValue ) { m_nStiff = nValue; }
	inline void SetStiffResistance( int nValue ) { m_nStiffResistance = nValue; }
	inline void SetCritical( int nValue ) { m_nCritical = nValue; }
	inline void SetCriticalResistance( int nValue ) { m_nCriticalResistance = nValue; }
	inline void SetStun( int nValue ) { m_nStun = nValue; }
	inline void SetStunResistance( int nValue ) { m_nStunResistance = nValue; }
	inline void SetElementAttack( ElementEnum Type, float fValue ) { m_fElementAttack[Type] = fValue; }
	inline void SetElementDefense( ElementEnum Type, float fValue ) { m_fElementDefense[Type] = fValue; }
	inline void SetDownDelayProb( float fValue ) { m_fDownDelayProb = fValue; }
	inline void SetRecoverySP( int nValue ) { m_nRecoverySP = nValue; }
	inline void SetSuperAmmor( int nValue ) { m_nSuperAmmor = nValue; }
	inline void SetFinalDamage( int nValue ) { m_nFinalDamage = nValue; }
	inline void SetSpirit( int nValue ) { m_nSpirit = nValue; }
	inline void SetSafeZoneMoveSpeed( int nValue ) { m_nSafeZoneMoveSpeed = nValue; }
	inline void SetAddExp( float fValue ) { m_fAddExp = fValue; };

	inline void SetCashAttackPMinMax(int nValueMin, int nValueMax) { m_nAttackPMinMax[0] = nValueMin; m_nAttackPMinMax[1] = nValueMax; }
	inline void SetCashAttackMMinMax(int nValueMin, int nValueMax) { m_nAttackMMinMax[0] = nValueMin; m_nAttackMMinMax[1] = nValueMax; }

	int GetCashAttackPMin() { return m_nAttackPMinMax[0]; }
	int GetCashAttackPMax() { return m_nAttackPMinMax[1]; }
	int GetCashAttackMMin() { return m_nAttackMMinMax[0]; }
	int GetCashAttackMMax() { return m_nAttackMMinMax[1]; }

	// 비율
	float GetStrengthRatio() { return m_fStrengthRatio; }
	float GetAgilityRatio() { return m_fAgilityRatio; }
	float GetIntelligenceRatio() { return m_fIntelligenceRatio; }
	float GetStaminaRatio() { return m_fStaminaRatio; }
	float GetAttackPMinRatio() { return m_fAttackPRatio[0]; }
	float GetAttackPMaxRatio() { return m_fAttackPRatio[1]; }
	float GetAttackMMinRatio() { return m_fAttackMRatio[0]; }
	float GetAttackMMaxRatio() { return m_fAttackMRatio[1]; }
	float GetDefensePRatio() { return m_fDefensePRatio; }
	float GetDefenseMRatio() { return m_fDefenseMRatio; }
	float GetStiffRatio() { return m_fStiffRatio; }
	float GetStiffResistanceRatio() { return m_fStiffResistanceRatio; }
	float GetCriticalRatio() { return m_fCriticalRatio; }
	float GetCriticalResistanceRatio() { return m_fCriticalResistanceRatio; }
	float GetStunRatio() { return m_fStunRatio; }
	float GetStunResistanceRatio() { return m_fStunResistanceRatio; }
	float GetMaxHPRatio() { return m_fMaxHPRatio; }
	float GetMaxSPRatio() { return m_fMaxSPRatio; }
	float GetMoveSpeedRatio() { return m_fMoveSpeedRatio; }
	float GetRecoverySPRatio() { return m_fRecoverySPRatio; }
	float GetSuperAmmorRatio() { return m_fSuperAmmorRatio; }
	float GetFinalDamageRatio() { return m_fFinalDamageRatio; }
	float GetSpiritRatio() { return m_fSpiritRatio; }
	float GetSafeZoneMoveSpeedRatio() { return m_fSafeZoneMoveSpeedRatio; }

	inline void SetStrengthRatio( float fValue ) { m_fStrengthRatio = fValue; }
	inline void SetAgilityRatio( float fValue ) { m_fAgilityRatio = fValue; }
	inline void SetIntelligenceRatio( float fValue ) { m_fIntelligenceRatio = fValue; }
	inline void SetStaminaRatio( float fValue ) { m_fStaminaRatio = fValue; }
	inline void SetAttackPMinRatio( float fValue ) { m_fAttackPRatio[0] = fValue; }
	inline void SetAttackPMaxRatio( float fValue ) { m_fAttackPRatio[1] = fValue; }
	inline void SetAttackMMinRatio( float fValue ) { m_fAttackMRatio[0] = fValue; }
	inline void SetAttackMMaxRatio( float fValue ) { m_fAttackMRatio[1] = fValue; }
	inline void SetDefensePRatio( float fValue ) { m_fDefensePRatio = fValue; }
	inline void SetDefenseMRatio( float fValue ) { m_fDefenseMRatio = fValue; }

	inline void SetStiffRatio( float fValue ) { m_fStiffRatio = fValue; }
	inline void SetStiffResistanceRatio( float fValue ) { m_fStiffResistanceRatio = fValue; }
	inline void SetCriticalRatio( float fValue ) { m_fCriticalRatio = fValue; }
	inline void SetCriticalResistanceRatio( float fValue ) { m_fCriticalResistanceRatio = fValue; }
	inline void SetStunRatio( float fValue ) { m_fStunRatio = fValue; }
	inline void SetStunResistanceRatio( float fValue ) { m_fStunResistanceRatio = fValue; }

	inline void SetMaxHPRatio( float fValue ) { m_fMaxHPRatio = fValue; }
	inline void SetMaxSPRatio( float fValue ) { m_fMaxSPRatio = fValue; }
	inline void SetMoveSpeedRatio( float fValue ) { m_fMoveSpeedRatio = fValue; }
	inline void SetRecoverySPRatio( float fValue ) { m_fRecoverySPRatio = fValue; }
	inline void SetSuperAmmorRatio( float fValue ) { m_fSuperAmmorRatio = fValue; }
	inline void SetFinalDamageRatio( float fValue ) { m_fFinalDamageRatio = fValue; }
	inline void SetSpiritRatio( float fValue ) { m_fSpiritRatio = fValue; }
	inline void SetSafeZoneMoveSpeedRatio( float fValue ) { m_fSafeZoneMoveSpeedRatio = fValue; }

	CDnState &operator += ( CDnState & e );
	CDnState &operator -= ( CDnState & e );

	void MergeState( CDnState &State, ValueType eType );
	void CalculateRatioValue( CDnState &State );
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	void CalculateRatioValue( float fRatio );
#endif
};