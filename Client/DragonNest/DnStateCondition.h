#pragma once

#include "DnState.h"
class CDnStateCondition : public CDnState {
public:
	CDnStateCondition();
	virtual ~CDnStateCondition();

	enum StateConditionEnum {
		Unknown = 0,
		HP,
		SP,
		Strength,
		Agility,
		Intelligence,
		Stamina,
		Attack,
		Defense,
		MoveSpeed,
		DownDelay,
		StiffProb,
		CriticalProb,
		StunProb,
		StiffResistance,
		CriticalResistance,
		StunResistance,
		FireStrength,
		IceStrength,
		WindStrength,
		EarthStrength,
		FireDefense,
		IceDefense,
		WindDefense,
		EarthDefense,
		Sleep,
		Freeze,
		FlashStone,
		Poison,

		StateConditionEnum_Amount,
	};

	enum ValueTypeEnum {
		Absolute,
		Ratio,
	};

protected:
	StateConditionEnum m_StateCondition;
	ValueTypeEnum m_ValueType;
	float m_fLifeDelta;
	float m_fDelta;
	float m_fValue;

public:
	virtual void Initialize( StateConditionEnum StateCondition, ValueTypeEnum Type, int nTime, float fValue );

	// �ߵ���
	virtual bool OnActivate() { return true; }
	// ������
	virtual bool OnDeactivate() { return true; }
	// ��~��~ ����
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}
};