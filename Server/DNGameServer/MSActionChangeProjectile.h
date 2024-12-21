#pragma once

#include "MSActionTypeBase.h"
class CDnProjectile;

class MSActionChangeProjectile : public MSActionTypeBase {
public:
	static int ID;
	MSActionChangeProjectile();
	virtual ~MSActionChangeProjectile();

protected:
	int m_nSourceWeaponID;
	int m_nChangeWeaponID;
	CDnProjectile *m_pProjectile;

public:
	void Initialize( int nSourceWeaponID, int nChangeWeaponID );

	virtual void Execute( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsFinishAction();

	int GetSourceWeaponID() { return m_nSourceWeaponID; }
	int GetChangeWeaponID() { return m_nChangeWeaponID; }
	CDnProjectile *GetProjectile() { return m_pProjectile; }
};