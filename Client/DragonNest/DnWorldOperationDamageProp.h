#pragma once

#include "DnWorldOperationProp.h"
#include "DnDamageBase.h"
#include "MAActorProp.h"


class CDnWorldOperationDamageProp : public CDnWorldOperationProp,
									public CDnDamageBase,
									public MAActorProp
{
private:


public:
	CDnWorldOperationDamageProp();
	virtual ~CDnWorldOperationDamageProp();

protected:
	virtual bool InitializeTable( int nTableID );

public:
	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); }

	DnActorHandle GetMonsterHandle() { return m_hMonster; }

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };
};