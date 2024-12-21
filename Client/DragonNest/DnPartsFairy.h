#pragma once

#include "DnParts.h"

class CDnPartsFairy : public CDnParts 
{
public:
	CDnPartsFairy();
	virtual ~CDnPartsFairy();

	enum FairyEffectType {
		None,
		ChangeWeaponAction,
	};

protected:
	FairyEffectType m_EffectType;
	std::string m_szChangeWeaponActionStr;

#ifdef PRE_MOD_CASHFAIRY
	EtMatrix m_matCurrent;
	EtMatrix m_matCalculated;
	EtObjectHandle m_hParentObjectForCalc;
	int m_nLinkBoneIndexForCalc;
#endif

public:
	virtual bool Initialize( int nPartsTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE );
	virtual bool CreateObject( DnActorHandle hActor, const char *szAniName );
	virtual bool FreeObject();
#ifdef PRE_MOD_CASHFAIRY
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	void LinkFairy( DnActorHandle hActor );
#endif

	FairyEffectType GetFairyEffectType() { return m_EffectType; }
	const char *GetChangeWeaponActionStr() { return m_szChangeWeaponActionStr.c_str(); }
};