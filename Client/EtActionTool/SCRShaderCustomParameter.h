#pragma once

#include "SignalCustomRender.h"
#include "EtMatrixEx.h"
class CSCRShaderCustomParameter : public CSignalCustomRender 
{
public:
	CSCRShaderCustomParameter( const char *szSignalName );
	virtual ~CSCRShaderCustomParameter();

protected:
	CString m_szPrevFileName;
	MatrixEx m_matExWorld;
	EtTextureHandle m_hTexture;

public:
	virtual CSignalCustomRender *Clone() { return new CSCRShaderCustomParameter( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
};
