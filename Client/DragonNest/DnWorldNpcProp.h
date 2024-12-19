#pragma once

#include "DnWorldActProp.h"
#include "MAActorProp.h"

class CDnWorldNpcProp : public CDnWorldActProp, public MAActorProp {
public:
	CDnWorldNpcProp();
	virtual ~CDnWorldNpcProp();

protected:
	virtual bool InitializeTable( int nTableID );

public:
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; }
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual void Show( bool bShow );
};