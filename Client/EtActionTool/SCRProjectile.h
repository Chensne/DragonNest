#pragma once

#include "SignalCustomRender.h"
#include "EtMatrixEx.h"
class CSCRProjectile : public CSignalCustomRender 
{
public:
	CSCRProjectile( const char *szSignalName );
	virtual ~CSCRProjectile();

protected:
	MatrixEx m_matExWorld;

public:
	virtual CSignalCustomRender *Clone() { return new CSCRProjectile( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver );

};
