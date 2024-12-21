#include "DnDefenseMonsterActor.h"

class CDnLamiaMonsterActor : public CDnDefenseMonsterActor, public TBoostMemoryPool< CDnLamiaMonsterActor >
{
public:
	CDnLamiaMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnLamiaMonsterActor();

protected:
	DnWeaponHandle m_hSubWeapon[2];

protected:
	virtual void DefenseWeapon( bool bCrash );

public:
	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );
	virtual void DetachWeapon(int nEquipIndex = 0 );
};