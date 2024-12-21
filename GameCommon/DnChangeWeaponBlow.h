#pragma once
#include "dnblow.h"

class CDnChangeWeaponBlow : public CDnBlow, public TBoostMemoryPool< CDnChangeWeaponBlow >
{
protected:
	

public:
	CDnChangeWeaponBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnChangeWeaponBlow(void);

private:
	int m_nWeaponIndex;	// ��/�ι���
	int m_nWeaponID;	// �ٲ� ���� ID

	DnWeaponHandle m_hNewWeapon;	// �ٲ� ����
	
	void CreateWeapon();

	static int ms_InstanceCount;
	static std::vector<DnWeaponHandle> ms_WeaponList;
	
public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual bool CanBegin( void );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
