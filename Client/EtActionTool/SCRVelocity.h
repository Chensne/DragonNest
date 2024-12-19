#pragma once

#include "SignalCustomRender.h"
#include "EtMatrixEx.h"
class CSCRVelocity : public CSignalCustomRender 
{
public:
	CSCRVelocity( const char *szSignalName );
	virtual ~CSCRVelocity();

protected:
	//	std::vector<EtVector3> m_vVecList[2];
	MatrixEx m_matExWorld;

public:
	virtual CSignalCustomRender *Clone() { return new CSCRVelocity( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );

};
