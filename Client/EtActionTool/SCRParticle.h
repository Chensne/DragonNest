#pragma once

#include "SignalCustomRender.h"
#include "EtMatrixEx.h"
class CSCRParticle : public CSignalCustomRender 
{
public:
	CSCRParticle( const char *szSignalName );
	virtual ~CSCRParticle();

protected:
	CString m_szPrevFileName;
	MatrixEx m_matExWorld;
	int m_nParticleIndex;
	EtBillboardEffectHandle m_hParticle;
	bool m_bLoop;
	bool m_bIterator;

public:
	virtual CSignalCustomRender *Clone() { return new CSCRParticle( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void Release();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver );

};
