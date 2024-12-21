#pragma once

// ���� ������ ������ �ʿ�� ���� ���ܵǴ� ��Ȳ�Դϴ�. < �з��� ���ؼ� �۸��� Parts�� ������ DnParts�� ��� �ȹ޽��ϴ� > ĳ���� ��Ţ�� ������ �Ǳ⶧���� �̷��� �����մϴ�.
// ĳ���Ϳ��� �������� ������ ��ġ���ʰ� ĳ���͸� ���Ͽ� Ż�Ϳ��� ������ ��ġ�� ��������� , ��ü ��ü�� ĳ���Ͱ� �����ϰ� �Ǵ�, ���ܵǴ� ��Ȳ�Դϴ�. <���� Ż�� ���԰��� Ʋ���ϴ�.>


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

