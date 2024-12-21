#include "StdAfx.h"
#include "DnPartsVehicleEffect.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


DECL_DN_SMART_PTR_STATIC( CDnPartsVehicleEffect, 100 )

CDnPartsVehicleEffect::CDnPartsVehicleEffect()
: CDnUnknownRenderObject( false )
{
	m_EffectType = VehicleEffectType::None;
}

CDnPartsVehicleEffect::~CDnPartsVehicleEffect()
{
}

DnPartsVehicleEffectHandle CDnPartsVehicleEffect::CreatePartsVehicleEffect( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID )
{
	if( nTableID < 1 ) return CDnPartsVehicleEffect::Identity();

	DnPartsVehicleEffectHandle hVehicleEffect = (new CDnPartsVehicleEffect)->GetMySmartPtr();
	if( !hVehicleEffect ) return CDnPartsVehicleEffect::Identity();

	hVehicleEffect->Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID );
	return hVehicleEffect;
}

bool CDnPartsVehicleEffect::Initialize( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TFAIRY );
	if( !pSox ) return false;

	m_EffectType = (VehicleEffectType)pSox->GetFieldFromLablePtr( nTableID, "_FairyType" )->GetInteger();
	m_szChangeVehicleActionStr = pSox->GetFieldFromLablePtr( nTableID, "_WeaponEffectAction" )->GetString();

	CDnItem::Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID );

	return true;
}
