#pragma once
#include "DnCountCtl.h"

class CDnDamageCount : public CDnCountCtl
{
public:
	CDnDamageCount( CEtUIDialog *pParent );
	virtual ~CDnDamageCount(void);

	enum eDamageType {
		Damage_Normal = 0,
		Damage_Critical,
		Damage_Resist,
		Damage_CriticalRes,
#ifdef PRE_ADD_DECREASE_EFFECT
		Damage_DecreaseEffect,
#endif // PRE_ADD_DECREASE_EFFECT
	};

protected:
	float m_fSizeValue;
	float m_fVelocity;
	float m_fTotalElapsedTime;
	bool m_bImpactInc;
	bool m_bImpactDec;
	eDamageType m_DamageType;

public:	
	void SetCritical();
	void SetResist();
	void SetCriticalRes();
#ifdef PRE_ADD_DECREASE_EFFECT
	void SetDecreaseEffect();
#endif // PRE_ADD_DECREASE_EFFECT

	void SetHasElement(int hasElement);

public:
	virtual void SetValue( int nValue );
	virtual float GetControlWidth();
	virtual float GetControlHeight();

public:
	virtual void Render( float fElapsedTime );
	virtual void Process( float fElapsedTime );
};
