#include "StdAfx.h"
#include "DnNLPAArcher.h"
#include "DnWeapon.h"

TDnPlayerArcher< CDnPlayerActor >::RotateBoneStruct TDnPlayerArcher< CDnPlayerActor >::s_RotateBoneDefine[] = { 
	{ "Bip01 Spine", -1, 0.3f },
	{ "Bip01 Spine1", -1, 0.3f },
	{ "Bip01 Neck", -1, 0.4f },
	NULL,
};

CDnNLPAArcher::CDnNLPAArcher( CMultiRoom *pRoom, int nClassID )
: TDnPlayerArcher< CDnPlayerActor >( pRoom, nClassID )
{
	m_fPitchDelta = 0.f;
	m_fTargetPitchAngle = 0.f;
	m_fCurrentPitchAngle = 0.f;
}

CDnNLPAArcher::~CDnNLPAArcher()
{
}

void CDnNLPAArcher::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnPlayerArcher< CDnPlayerActor >::Process( LocalTime, fDelta );

	if( strcmp( GetCurrentAction(), "Stand" ) == NULL && !m_bShootMode )
	{
		if( strstr( m_szCustomAction.c_str(), "Shoot" ) ) {
			float fFrame = ( ( CDnActionBase::m_LocalTime - m_CustomActionTime ) / 1000.f ) * CDnActionBase::m_fFPS;
			CmdStop( "Shoot_Stand", 0, 8.f, fFrame );
			ResetCustomAction();
		}
	}
	if( m_fPitchDelta > 0.f ) {
		m_fPitchDelta -= fDelta;
		if( m_fPitchDelta < 0.f ) m_fPitchDelta = 0.f;
		m_fPitchAngle = m_fTargetPitchAngle + ( ( m_fCurrentPitchAngle - m_fTargetPitchAngle ) * ( 1.f / 0.2f * m_fPitchDelta ) );
	}
}

void CDnNLPAArcher::OnDispatchMessage( CDNUserSession *pSession, DWORD dwActorProtocol, BYTE *pPacket )
{
	switch( dwActorProtocol ) {
		case eActor::CS_PLAYERPITCH:
			{
				CMemoryStream Stream( pPacket, 256 );
				m_fCurrentPitchAngle = m_fPitchAngle;
				Stream.Read( &m_fTargetPitchAngle, sizeof(float) );
				m_fPitchDelta = 0.2f;
			}
			return;
	}

	TDnPlayerArcher< CDnPlayerActor >::OnDispatchMessage( pSession, dwActorProtocol, pPacket );
}
