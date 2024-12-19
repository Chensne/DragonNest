#pragma once

#include "SignalCustomRender.h"
#include "EtMatrixEx.h"
class CSCRDecal : public CSignalCustomRender 
{
public:
	CSCRDecal( const char *szSignalName );
	virtual ~CSCRDecal();

protected:
	CString m_szPrevFileName;
	MatrixEx m_matExWorld;
	EtTextureHandle m_hTexture;

public:
	virtual CSignalCustomRender *Clone() { return new CSCRDecal( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void Release();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver );

};
