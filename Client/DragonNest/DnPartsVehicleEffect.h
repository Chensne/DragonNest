#pragma once

// 따로 파츠를 구성할 필요는 없는 예외되는 상황입니다. < 분류를 위해서 작명은 Parts로 했지만 DnParts는 상속 안받습니다 > 캐릭터 이큅과 연관이 되기때문에 이렇게 설정합니다.
// 캐릭터에게 직접적인 영향을 끼치지않고 캐릭터를 통하여 탈것에게 영향을 끼치는 경우이지만 , 객체 자체는 캐릭터가 관리하게 되는, 예외되는 상황입니다. <기존 탈것 슬롯과는 틀립니다.>


#include "DnUnknownRenderObject.h"
#include "DnItem.h"

class CDnPartsVehicleEffect : public CDnUnknownRenderObject< CDnPartsVehicleEffect >, 
	public CDnItem
{
public:
	CDnPartsVehicleEffect();
	virtual ~CDnPartsVehicleEffect();

	enum VehicleEffectType {
		None,
		ChangeWeaponAction,
	};

protected:
	VehicleEffectType m_EffectType;
	std::string m_szChangeVehicleActionStr;

public:
	static DnPartsVehicleEffectHandle CreatePartsVehicleEffect( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE );

public:
	virtual bool Initialize( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE );

	VehicleEffectType GetVehicleEffectType() { return m_EffectType; }
	const char *GetChangeVehicleActionStr() { return m_szChangeVehicleActionStr.c_str(); }
};

