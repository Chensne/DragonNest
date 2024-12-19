#pragma once

#include "SignalCustomRender.h"
#include "EtMatrixEx.h"
class CSCRFX : public CSignalCustomRender 
{
public:
	CSCRFX( const char *szSignalName );
	virtual ~CSCRFX();

protected:
	CString m_szPrevFileName;
	MatrixEx m_matExWorld;
	int m_nFXIndex;
	EtEffectObjectHandle m_hFX;

public:
	virtual CSignalCustomRender *Clone() { return new CSCRFX( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void Release();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver );

};
