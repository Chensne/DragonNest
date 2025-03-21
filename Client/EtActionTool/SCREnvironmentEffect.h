#pragma once

#include "SignalCustomRender.h"
#include "EtMatrixEx.h"
class CSCREnvironmentEffect : public CSignalCustomRender 
{
public:
	CSCREnvironmentEffect( const char *szSignalName );
	virtual ~CSCREnvironmentEffect();

protected:
//	std::vector<EtVector3> m_vVecList[2];
	MatrixEx m_matExWorld;

public:
	virtual CSignalCustomRender *Clone() { return new CSCREnvironmentEffect( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );

};
