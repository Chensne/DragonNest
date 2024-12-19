#include "StdAfx.h"
#include "DnChangeProjectileProcessor.h"
#include "DnActor.h"
#include "DnProjectile.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )




CDnChangeProjectileProcessor::CDnChangeProjectileProcessor( DnActorHandle hActor, 
															int iSourceWeaponID, int iChangeWeaponID ) : IDnSkillProcessor( hActor ),
																										 m_pProjectile( NULL ),
																										 m_iSourceWeaponID( iSourceWeaponID ),
																										 m_iChangeWeaponID( iChangeWeaponID )
{
	//_ASSERT( m_hHasActor );
	if( m_hHasActor )
	{
		m_pProjectile = new CDnProjectile( ARG_STATIC_FUNC(m_hHasActor) hActor, false, false );
		m_pProjectile->CDnWeapon::Initialize( m_iChangeWeaponID, 0 );
		m_pProjectile->CDnWeapon::CreateObject();
		m_hProjectile = m_pProjectile->GetMySmartPtr();
	}

	m_iType = CHANGE_PROJECTILE;
}


CDnChangeProjectileProcessor::~CDnChangeProjectileProcessor(void)
{
	if( !m_hProjectile && m_pProjectile )
	{
		g_Log.Log( LogType::_SESSIONCRASH, L"[CDnChangeProjectileProcessor] Projectile 두번 삭제합니다~~~\n" );
	}
	else
	{
		SAFE_DELETE( m_pProjectile );
	}
}


void CDnChangeProjectileProcessor::SetHasActor( DnActorHandle hActor )
{
	_ASSERT( hActor && "void CDnChangeProjectileProcessor::SetHasActor( DnActorHandle hActor ), Actor is NULL!!" );
	IDnSkillProcessor::SetHasActor( hActor );

	if( NULL == m_pProjectile )
	{
		m_pProjectile = new CDnProjectile( ARG_STATIC_FUNC(m_hHasActor) hActor, false, false );
		m_pProjectile->CDnWeapon::Initialize( m_iChangeWeaponID, 0 );
		m_pProjectile->CDnWeapon::CreateObject();
	}
}



void CDnChangeProjectileProcessor::OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill )
{
	
}


void CDnChangeProjectileProcessor::Process( LOCAL_TIME LocalTime, float fDelta )
{

}


void CDnChangeProjectileProcessor::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

}


bool CDnChangeProjectileProcessor::IsFinished( void )
{
	return true;
}