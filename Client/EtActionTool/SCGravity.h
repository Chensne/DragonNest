#pragma once

#include "SignalCustomRender.h"

class CSCRGravity : public CSignalCustomRender 
{
public:
	CSCRGravity( const char *szSignalName );
	virtual ~CSCRGravity();

protected:
	std::vector<EtVector3> m_vVecList[5];

public:
	virtual CSignalCustomRender *Clone() { return new CSCRGravity( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver );

};
