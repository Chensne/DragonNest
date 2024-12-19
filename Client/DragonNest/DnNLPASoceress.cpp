#include "StdAfx.h"
#include "DnNLPASoceress.h"
#include "DnWeapon.h"
#include "DNProtocol.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

TDnPlayerSoceress< CDnPlayerActor >::RotateBoneStruct TDnPlayerSoceress< CDnPlayerActor >::s_RotateBoneDefine[] = { 
	{ "Bip01 Spine", -1, 0.3f, true },
	{ "Bip01 Spine1", -1, 0.3f, true },
	{ "Bip01 Neck", -1, 0.4f, true },
	{ "Bip01 L Clavicle", -1, -0.8f, false },
	{ "Bip01 R Clavicle", -1, -0.8f, false },
	NULL,
};

CDnNLPASoceress::CDnNLPASoceress( int nClassID, bool bProcess )
: TDnPlayerSoceress< CDnPlayerActor >( nClassID, bProcess )
{
	m_fPitchDelta = 0.f;
	m_fTargetPitchAngle = 0.f;
	m_fCurrentPitchAngle = 0.f;
}

CDnNLPASoceress::~CDnNLPASoceress()
{
}

void CDnNLPASoceress::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnPlayerSoceress< CDnPlayerActor >::Process( LocalTime, fDelta );

	if( IsSwapSingleSkin() ) return;
	if( strcmp( GetCurrentAction(), "Stand" ) == NULL && !m_bShootMode )
	{
		if( strstr( m_szCustomAction.c_str(), "Shoot" ) ) {
			float fFrame = ( ( CDnActionBase::m_LocalTime - m_CustomActionTime ) / 1000.f ) * CDnActionBase::m_fFps;
			CmdStop( "Shoot_Stand", 0, 8.f, fFrame );
			ResetCustomAction();
			ResetMixedAnimation();
		}
	}
	if( m_fPitchDelta > 0.f ) {
		m_fPitchDelta -= fDelta;
		if( m_fPitchDelta < 0.f ) m_fPitchDelta = 0.f;
		m_fPitchAngle = m_fTargetPitchAngle + ( ( m_fCurrentPitchAngle - m_fTargetPitchAngle ) * ( 1.f / 0.2f * m_fPitchDelta ) );
	}
}

void CDnNLPASoceress::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
{
	switch( dwActorProtocol ) {
		case eActor::SC_PLAYERPITCH:
			{
				CMemoryStream Stream( pPacket, 256 );
				m_fCurrentPitchAngle = m_fPitchAngle;
				Stream.Read( &m_fTargetPitchAngle, sizeof(float) );
				m_fPitchDelta = 0.2f;
			}
			return;
	}

	TDnPlayerSoceress< CDnPlayerActor >::OnDispatchMessage( dwActorProtocol, pPacket );
}
