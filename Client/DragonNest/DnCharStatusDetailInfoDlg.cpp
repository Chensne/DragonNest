#include "StdAfx.h"
#include "DnCharStatusDetailInfoDlg.h"
#include "DnCharStatusBaseInfoDlg.h"
#include "DnPlayerActor.h"
#include "DnInterfaceString.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharStatusDetailInfoDlg::CDnCharStatusDetailInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) : CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_bActorStateRefresh = true;

	for( int x=0; x<eDetailInfoAttributes::MAX; x++ )
		m_pStatic_Attribute[x] = NULL;
}

CDnCharStatusDetailInfoDlg::~CDnCharStatusDetailInfoDlg(void)
{
}

void CDnCharStatusDetailInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharStatusDetailInfoDlg.ui" ).c_str(), bShow );
}

void CDnCharStatusDetailInfoDlg::InitialUpdate()
{
	m_pStatic_Attribute[eDetailInfoAttributes::CRITICAL_PROB] = GetControl<CDnCharStatusStatic>("ID_CRITICAL_PROB");
	m_pStatic_Attribute[eDetailInfoAttributes::STUN_PROB] = GetControl<CDnCharStatusStatic>("ID_STUN_PROB");
	m_pStatic_Attribute[eDetailInfoAttributes::STIFF_PROB] = GetControl<CDnCharStatusStatic>("ID_STIFF_PROB");

	m_pStatic_Attribute[eDetailInfoAttributes::CRITICAL_RESIST] = GetControl<CDnCharStatusStatic>("ID_CRITICAL_RESIST");
	m_pStatic_Attribute[eDetailInfoAttributes::STUN_RESIST] = GetControl<CDnCharStatusStatic>("ID_STUN_RESIST");
	m_pStatic_Attribute[eDetailInfoAttributes::STIFF_RESIST] = GetControl<CDnCharStatusStatic>("ID_STIFF_RESIST");
	
	m_pStatic_Attribute[eDetailInfoAttributes::FIRE_ATTACK] = GetControl<CDnCharStatusStatic>("ID_ATT_FIRE");
	m_pStatic_Attribute[eDetailInfoAttributes::ICE_ATTACK] = GetControl<CDnCharStatusStatic>("ID_ATT_ICE");
	m_pStatic_Attribute[eDetailInfoAttributes::LIGHT_ATTACK] = GetControl<CDnCharStatusStatic>("ID_ATT_LIGHT");
	m_pStatic_Attribute[eDetailInfoAttributes::DARK_ATTACK] = GetControl<CDnCharStatusStatic>("ID_ATT_DARK");

	m_pStatic_Attribute[eDetailInfoAttributes::FIRE_DEFENSE] = GetControl<CDnCharStatusStatic>("ID_DEFENSE_FIRE");
	m_pStatic_Attribute[eDetailInfoAttributes::ICE_DEFENSE] = GetControl<CDnCharStatusStatic>("ID_DEFENSE_ICE");
	m_pStatic_Attribute[eDetailInfoAttributes::LIGHT_DEFENSE] = GetControl<CDnCharStatusStatic>("ID_DEFENSE_LIGHT");
	m_pStatic_Attribute[eDetailInfoAttributes::DARK_DEFENSE] = GetControl<CDnCharStatusStatic>("ID_DEFENSE_DARK");
}

void CDnCharStatusDetailInfoDlg::Show( bool bShow )
{
	if( bShow != m_bShow && bShow == true )
	{
		m_bActorStateRefresh = true;
	}

	CDnCustomDlg::Show( bShow );
}

void CDnCharStatusDetailInfoDlg::Render( float fElapsedTime )
{
	if( !IsShow() )		
		return;

#ifdef PRE_FIX_CHARSTATUS_REFRESH
	if( m_bActorStateRefresh )
	{
		RefreshStatus();
		m_bActorStateRefresh = false;
	}
#else
	RefreshStatus();
#endif

	CEtUIDialog::Render( fElapsedTime );
}


void CDnCharStatusDetailInfoDlg::OnRefreshPlayerStatus()
{
	m_bActorStateRefresh = true;
}

void CDnCharStatusDetailInfoDlg::RefreshStatus()
{
	if( !CDnActor::s_hLocalActor )
		return;

	DnPlayerActorHandle hActor = CDnActor::s_hLocalActor;
	CDnState *pState = const_cast<CDnState*>(hActor->GetBaseState());
	CDnState *pTotalState = const_cast<CDnState*>(hActor->GetStateStep(0));
#if defined(PRE_ADD_SKILLBUF_RENEW) && defined(_CLIENT) && defined(PRE_ADD_STATUS_BUFF_INFORMATION)
	CDnState *pBuffState = (CDnState *)hActor->GetBuffState();
#endif

	if( !pState || !pTotalState ) 
		return;

#if defined(PRE_ADD_SKILLBUF_RENEW) && defined(_CLIENT) && defined(PRE_ADD_STATUS_BUFF_INFORMATION)
	m_pStatic_Attribute[eDetailInfoAttributes::CRITICAL_PROB]->AddBuffValue( pBuffState->GetCritical() );
	m_pStatic_Attribute[eDetailInfoAttributes::STUN_PROB]->AddBuffValue( pBuffState->GetStun() );
	m_pStatic_Attribute[eDetailInfoAttributes::STIFF_PROB]->AddBuffValue( pBuffState->GetStiff() );

	m_pStatic_Attribute[eDetailInfoAttributes::CRITICAL_RESIST]->AddBuffValue( pBuffState->GetCriticalResistance() );
	m_pStatic_Attribute[eDetailInfoAttributes::STUN_RESIST]->AddBuffValue( pBuffState->GetStunResistance() );
	m_pStatic_Attribute[eDetailInfoAttributes::STIFF_RESIST]->AddBuffValue( pBuffState->GetStiffResistance() );

	m_pStatic_Attribute[eDetailInfoAttributes::FIRE_ATTACK]->AddBuffValue( pBuffState->GetElementAttack( CDnState::Fire ) * 100.f );
	m_pStatic_Attribute[eDetailInfoAttributes::ICE_ATTACK]->AddBuffValue( pBuffState->GetElementAttack( CDnState::Ice ) * 100.f );
	m_pStatic_Attribute[eDetailInfoAttributes::LIGHT_ATTACK]->AddBuffValue( pBuffState->GetElementAttack( CDnState::Light ) * 100.f );
	m_pStatic_Attribute[eDetailInfoAttributes::DARK_ATTACK]->AddBuffValue( pBuffState->GetElementAttack( CDnState::Dark ) * 100.f );

	m_pStatic_Attribute[eDetailInfoAttributes::FIRE_DEFENSE]->AddBuffValue( pBuffState->GetElementDefense( CDnState::Fire ) * 100.f );
	m_pStatic_Attribute[eDetailInfoAttributes::ICE_DEFENSE]->AddBuffValue( pBuffState->GetElementDefense( CDnState::Ice ) * 100.f );
	m_pStatic_Attribute[eDetailInfoAttributes::LIGHT_DEFENSE]->AddBuffValue( pBuffState->GetElementDefense( CDnState::Light ) * 100.f );
	m_pStatic_Attribute[eDetailInfoAttributes::DARK_DEFENSE]->AddBuffValue( pBuffState->GetElementDefense( CDnState::Dark ) * 100.f );
#endif

#ifdef PRE_ADD_CHAR_SATUS_RENEW

	m_pStatic_Attribute[eDetailInfoAttributes::CRITICAL_PROB]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::CRIT_RATIO , CDnCharStatusBaseInfoDlg::CalcCriticalRate( pTotalState->GetCritical() , hActor ) );
	m_pStatic_Attribute[eDetailInfoAttributes::CRITICAL_PROB]->SetValue( 
		CDnCharStatusStatic::TPYE_05, 
		true, 
		pTotalState->GetCritical() ,
		pState->GetCritical() , 
		pTotalState->GetCritical() - pState->GetCritical() , 
		CDnCharStatusBaseInfoDlg::CalcCriticalRate( pTotalState->GetCritical() , hActor )
		);

	m_pStatic_Attribute[eDetailInfoAttributes::STUN_PROB]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::STUN_RATIO , 0 );
	m_pStatic_Attribute[eDetailInfoAttributes::STUN_PROB]->SetValue( 
		CDnCharStatusStatic::TYPE_01, 
		true, 
		pTotalState->GetStun(), 
		pState->GetStun() , 
		pTotalState->GetStun() - pState->GetStun()
		);

	m_pStatic_Attribute[eDetailInfoAttributes::STIFF_PROB]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::STIFF_RATIO , 0);
	m_pStatic_Attribute[eDetailInfoAttributes::STIFF_PROB]->SetValue( 
		CDnCharStatusStatic::TYPE_01, 
		true, 
		pTotalState->GetStiff() , 
		pState->GetStiff() , 
		pTotalState->GetStiff() - pState->GetStiff()
		);

	m_pStatic_Attribute[eDetailInfoAttributes::CRITICAL_RESIST]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::CRIT_RESIST , CDnCharStatusBaseInfoDlg::CalcCriticalRegistanceRate( pTotalState->GetCriticalResistance() , hActor ) );
	m_pStatic_Attribute[eDetailInfoAttributes::CRITICAL_RESIST]->SetValue( 
		CDnCharStatusStatic::TPYE_05,
		true, 
		pTotalState->GetCriticalResistance() , 
		pState->GetCriticalResistance() , 
		pTotalState->GetCriticalResistance() - pState->GetCriticalResistance() , 
		CDnCharStatusBaseInfoDlg::CalcCriticalRate( pTotalState->GetCriticalResistance() , hActor )				
		);

	m_pStatic_Attribute[eDetailInfoAttributes::STUN_RESIST]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::STUN_RESIST  , 0);
	m_pStatic_Attribute[eDetailInfoAttributes::STUN_RESIST]->SetValue( 
		CDnCharStatusStatic::TYPE_01, 
		true, 
		pTotalState->GetStunResistance(), 
		pState->GetStunResistance() , 
		pTotalState->GetStunResistance() - pState->GetStunResistance()
		);

	m_pStatic_Attribute[eDetailInfoAttributes::STIFF_RESIST]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::STIFF_RESIT , 0);
	m_pStatic_Attribute[eDetailInfoAttributes::STIFF_RESIST]->SetValue( 
		CDnCharStatusStatic::TYPE_01, 
		true, 
		pTotalState->GetStiffResistance(), 
		pState->GetStiffResistance() , 
		pTotalState->GetStiffResistance() - pState->GetStiffResistance()
		);

	m_pStatic_Attribute[eDetailInfoAttributes::FIRE_ATTACK]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::FIRE_DAMAGE , pTotalState->GetElementAttack( CDnState::Fire ) * 100.0f );
	m_pStatic_Attribute[eDetailInfoAttributes::FIRE_ATTACK]->SetValue_Float( 
		CDnCharStatusStatic::TYPE_03, 
		true, 
		pTotalState->GetElementAttack( CDnState::Fire ) * 100.0f , 
		pState->GetElementAttack( CDnState::Fire ) * 100.0f , 
		pTotalState->GetElementAttack( CDnState::Fire ) * 100.0f - pState->GetElementAttack( CDnState::Fire ) * 100.0f
		);

	m_pStatic_Attribute[eDetailInfoAttributes::ICE_ATTACK]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::WATER_DAMAGE , pTotalState->GetElementAttack( CDnState::Ice ) * 100.0f );
	m_pStatic_Attribute[eDetailInfoAttributes::ICE_ATTACK]->SetValue_Float( 
		CDnCharStatusStatic::TYPE_03, 
		true, 
		pTotalState->GetElementAttack( CDnState::Ice ) * 100.0f , 
		pState->GetElementAttack( CDnState::Ice ) * 100.0f , 
		pTotalState->GetElementAttack( CDnState::Ice ) * 100.0f - pState->GetElementAttack( CDnState::Ice ) * 100.0f
		);

	m_pStatic_Attribute[eDetailInfoAttributes::LIGHT_ATTACK]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::LIGHT_DAMAGE , pTotalState->GetElementAttack( CDnState::Light ) * 100.0f );
	m_pStatic_Attribute[eDetailInfoAttributes::LIGHT_ATTACK]->SetValue_Float( 
		CDnCharStatusStatic::TYPE_03,
		true, 
		pTotalState->GetElementAttack( CDnState::Light ) * 100.0f , 
		pState->GetElementAttack( CDnState::Light ) * 100.0f , 
		pTotalState->GetElementAttack( CDnState::Light ) * 100.0f - pState->GetElementAttack( CDnState::Light ) * 100.0f
		);

	m_pStatic_Attribute[eDetailInfoAttributes::DARK_ATTACK]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::DARK_DAMAGE , pTotalState->GetElementAttack( CDnState::Dark ) * 100.0f );
	m_pStatic_Attribute[eDetailInfoAttributes::DARK_ATTACK]->SetValue_Float( 
		CDnCharStatusStatic::TYPE_03, 
		true, 
		pTotalState->GetElementAttack( CDnState::Dark ) * 100.0f , 
		pState->GetElementAttack( CDnState::Dark ) * 100.0f , 
		pTotalState->GetElementAttack( CDnState::Dark ) * 100.0f - pState->GetElementAttack( CDnState::Dark ) * 100.0f
		);

	m_pStatic_Attribute[eDetailInfoAttributes::FIRE_DEFENSE]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::FIRE_DEFENSE , pTotalState->GetElementDefense( CDnState::Fire ) * 100.0f );
	m_pStatic_Attribute[eDetailInfoAttributes::FIRE_DEFENSE]->SetValue_Float( 
		CDnCharStatusStatic::TYPE_03, 
		true, 
		pTotalState->GetElementDefense( CDnState::Fire ) * 100.0f , 
		pState->GetElementDefense( CDnState::Fire ) * 100.0f , 
		pTotalState->GetElementDefense( CDnState::Fire ) * 100.0f - pState->GetElementDefense( CDnState::Fire ) * 100.0f
		);


	m_pStatic_Attribute[eDetailInfoAttributes::ICE_DEFENSE]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::WATER_DEFENSE , pTotalState->GetElementDefense( CDnState::Ice ) * 100.0f );
	m_pStatic_Attribute[eDetailInfoAttributes::ICE_DEFENSE]->SetValue_Float( 
		CDnCharStatusStatic::TYPE_03,
		true, 
		pTotalState->GetElementDefense( CDnState::Ice ) * 100.0f , 
		pState->GetElementDefense( CDnState::Ice ) * 100.0f , 
		pTotalState->GetElementDefense( CDnState::Ice ) * 100.0f - pState->GetElementDefense( CDnState::Ice ) * 100.0f
		);

	m_pStatic_Attribute[eDetailInfoAttributes::LIGHT_DEFENSE]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::LIGHT_DEFENSE , pTotalState->GetElementDefense( CDnState::Light ) * 100.0f );
	m_pStatic_Attribute[eDetailInfoAttributes::LIGHT_DEFENSE]->SetValue_Float( 
		CDnCharStatusStatic::TYPE_03, 
		true, 
		pTotalState->GetElementDefense( CDnState::Light ) * 100.0f , 
		pState->GetElementDefense( CDnState::Light ) * 100.0f , 
		pTotalState->GetElementDefense( CDnState::Light ) * 100.0f - pState->GetElementDefense( CDnState::Light ) * 100.0f
		);

	m_pStatic_Attribute[eDetailInfoAttributes::DARK_DEFENSE]->AddDetailString( CDnCharStatusBaseInfoDlg::eDetailString::DARK_DEFENSE , pTotalState->GetElementDefense( CDnState::Dark ) * 100.0f );
	m_pStatic_Attribute[eDetailInfoAttributes::DARK_DEFENSE]->SetValue_Float( 
		CDnCharStatusStatic::TYPE_03, 
		true, 
		pTotalState->GetElementDefense( CDnState::Dark ) * 100.0f , 
		pState->GetElementDefense( CDnState::Dark ) * 100.0f , 
		pTotalState->GetElementDefense( CDnState::Dark ) * 100.0f - pState->GetElementDefense( CDnState::Dark ) * 100.0f
		);
#endif

}