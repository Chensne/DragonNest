#pragma once

#include "SignalCustomRender.h"
class CSCRHit : public CSignalCustomRender 
{
public:
	CSCRHit( const char *szSignalName );
	virtual ~CSCRHit();

protected:
	std::vector<EtVector3> m_vVecList[5];

public:
	virtual CSignalCustomRender *Clone() { return new CSCRHit( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver );

};
