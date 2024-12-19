#pragma once

#include "SignalCustomRender.h"
#include "EtMatrixEx.h"


class CSCRMarkProjectile : public CSignalCustomRender 
{
public:
	CSCRMarkProjectile( const char *szSignalName );
	virtual ~CSCRMarkProjectile();

public:
	virtual CSignalCustomRender *Clone() { return new CSCRMarkProjectile( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void Release();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver );

public:
	enum eMarkType
	{
		MARK_NONE = -1,
		MARK_DECAL = 0,
		MARK_FX,
		MARK_PARTICLE,
	};

protected:
	eMarkType m_eMarkType;
	CString m_szPrevFileName;
	MatrixEx m_matExWorld;
	EtTextureHandle m_hTexture;
	int m_nFXIndex;
	EtEffectObjectHandle m_hFX;
	int m_nParticleIndex;
	EtBillboardEffectHandle m_hParticle;
};
