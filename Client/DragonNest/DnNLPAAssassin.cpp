#include "StdAfx.h"
#include "DnNLPAAssassin.h"
#include "DnWeapon.h"
#include "DNProtocol.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined( PRE_ADD_ASSASSIN )

TDnPlayerAssassin< CDnPlayerActor >::RotateBoneStruct TDnPlayerAssassin< CDnPlayerActor >::s_RotateBoneDefine[] = { 
	{ "Bip01 Spine", -1, 0.3f, true },
	{ "Bip01 Spine1", -1, 0.3f, true },
	{ "Bip01 Neck", -1, 0.4f, true },
	{ "Bip01 L Clavicle", -1, -0.8f, false },
	{ "Bip01 R Clavicle", -1, -0.8f, false },
	NULL,
};

CDnNLPAAssassin::CDnNLPAAssassin( int nClassID, bool bProcess )
: TDnPlayerAssassin< CDnPlayerActor >( nClassID, bProcess )
{
	m_fPitchDelta = 0.f;
	m_fTargetPitchAngle = 0.f;
	m_fCurrentPitchAngle = 0.f;
}

CDnNLPAAssassin::~CDnNLPAAssassin()
{
}

void CDnNLPAAssassin::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnPlayerAssassin< CDnPlayerActor >::Process( LocalTime, fDelta );

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

void CDnNLPAAssassin::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
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

	TDnPlayerAssassin< CDnPlayerActor >::OnDispatchMessage( dwActorProtocol, pPacket );
}

#endif	// #if defined( PRE_ADD_ASSASSIN )