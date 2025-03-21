#pragma once

#include "SignalCustomRender.h"
#include "EtMatrixEx.h"
class CSCRAlphaBlend : public CSignalCustomRender 
{
public:
	CSCRAlphaBlend( const char *szSignalName );
	virtual ~CSCRAlphaBlend();

protected:
	CString m_szPrevFileName;
	MatrixEx m_matExWorld;
	EtTextureHandle m_hTexture;

public:
	virtual CSignalCustomRender *Clone() { return new CSCRAlphaBlend( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
};
