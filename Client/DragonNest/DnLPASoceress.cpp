#include "StdAfx.h"
#include "DnLPASoceress.h"
#include "InputWrapper.h"
#include "EtSoundEngine.h"
#include "DnWeapon.h"
#include "DnPlayerCamera.h"
#include "DnWorld.h"
#include "DnSkill.h"
#include "DNProtocol.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define REFRESH_PITCH_TICK 500

TDnPlayerSoceress< CDnLocalPlayerActor >::RotateBoneStruct TDnPlayerSoceress< CDnLocalPlayerActor >::s_RotateBoneDefine[] = { 
	{ "Bip01 Spine", -1, 0.3f, true },
	{ "Bip01 Spine1", -1, 0.3f, true },
	{ "Bip01 Neck", -1, 0.4f, true },
	{ "Bip01 L Clavicle", -1, -0.8f, false },
	{ "Bip01 R Clavicle", -1, -0.8f, false },
	NULL,
};

CDnLPASoceress::CDnLPASoceress( int nClassID, bool bProcess )
: TDnPlayerSoceress< CDnLocalPlayerActor >( nClassID, bProcess )
{
	m_fPrevPitchAngle = 0.f;
	m_LastSendPitchMsg = 0;

	m_ChargeTime = 0;
}

CDnLPASoceress::~CDnLPASoceress()
{
}

void CDnLPASoceress::ProcessInput( LOCAL_TIME LocalTime, float fDelta )
{
	CDnLocalPlayerActor::ProcessInput( LocalTime, fDelta );

	if( IsSwapSingleSkin() ) return;

	if(m_bShootMode)
		return;

	if( IsCustomAction() ) {
		if( strstr( GetCurrentAction(), "Move" ) || strcmp( GetCurrentAction(), "Stand" ) == NULL ) {
			std::string szMixedAction;
			std::string szCategoryStr;
			if( strstr( m_szCustomAction.c_str(), "Shoot" ) ) szCategoryStr = "Shoot_";
			else if( strstr( m_szCustomAction.c_str(), "Charge" ) ) szCategoryStr = "Charge_";
			else szCategoryStr = "Move_";

			float fFrame = ( ( CDnActionBase::m_LocalTime - m_CustomActionTime ) / 1000.f ) * CDnActionBase::m_fFps;

			if( strstr( m_szCustomAction.c_str(), "Stand" ) && !strstr( GetCurrentAction(), "Stand" ) ) {
				if( strstr( GetCurrentAction(), "Front" ) ) szMixedAction = szCategoryStr + "Front";
				else if( strstr( GetCurrentAction(), "Back" ) ) szMixedAction = szCategoryStr + "Back";
				else if( strstr( GetCurrentAction(), "Left" ) ) szMixedAction = szCategoryStr + "Left";
				else if( strstr( GetCurrentAction(), "Right" ) ) szMixedAction = szCategoryStr + "Right";
			}
			else if( strstr( m_szCustomAction.c_str(), "Front" ) && !strstr( GetCurrentAction(), "Front" ) ) {
				if( strstr( GetCurrentAction(), "Stand" ) ) szMixedAction = szCategoryStr + "Stand";
				else if( strstr( GetCurrentAction(), "Back" ) ) szMixedAction = szCategoryStr + "Back";
				else if( strstr( GetCurrentAction(), "Left" ) ) szMixedAction = szCategoryStr + "Left";
				else if( strstr( GetCurrentAction(), "Right" ) ) szMixedAction = szCategoryStr + "Right";
			}
			else if( strstr( m_szCustomAction.c_str(), "Back" ) && !strstr( GetCurrentAction(), "Back" ) ) {
				if( strstr( GetCurrentAction(), "Stand" ) ) szMixedAction = szCategoryStr + "Stand";
				else if( strstr( GetCurrentAction(), "Front" ) ) szMixedAction = szCategoryStr + "Front";
				else if( strstr( GetCurrentAction(), "Left" ) ) szMixedAction = szCategoryStr + "Left";
				else if( strstr( GetCurrentAction(), "Right" ) ) szMixedAction = szCategoryStr + "Right";
			}
			else if( strstr( m_szCustomAction.c_str(), "Left" ) && !strstr( GetCurrentAction(), "Left" ) ) {
				if( strstr( GetCurrentAction(), "Stand" ) ) szMixedAction = szCategoryStr + "Stand";
				else if( strstr( GetCurrentAction(), "Front" ) ) szMixedAction = szCategoryStr + "Front";
				else if( strstr( GetCurrentAction(), "Back" ) ) szMixedAction = szCategoryStr + "Back";
				else if( strstr( GetCurrentAction(), "Right" ) ) szMixedAction = szCategoryStr + "Right";
			}
			else if( strstr( m_szCustomAction.c_str(), "Right" ) && !strstr( GetCurrentAction(), "Right" ) ) {
				if( strstr( GetCurrentAction(), "Stand" ) ) szMixedAction = szCategoryStr + "Stand";
				else if( strstr( GetCurrentAction(), "Front" ) ) szMixedAction = szCategoryStr + "Front";
				else if( strstr( GetCurrentAction(), "Back" ) ) szMixedAction = szCategoryStr + "Back";
				else if( strstr( GetCurrentAction(), "Left" ) ) szMixedAction = szCategoryStr + "Left";
			}
			if( !szMixedAction.empty() ) {
				if( strcmp( szMixedAction.c_str(), "Move_Stand" ) == NULL ) szMixedAction = "Stand";
				ResetMixedAnimation();
				if( !m_szActionBoneName.empty() && !m_szMaintenanceBoneName.empty() )
					CmdMixedAction( m_szActionBoneName.c_str(), m_szMaintenanceBoneName.c_str(), szMixedAction.c_str(), 0, fFrame, 8.f );
				SetCustomAction( szMixedAction.c_str(), m_fCustomPrevFrame );
			}
		}
	}
}

void CDnLPASoceress::ResetActor()
{
	TDnPlayerSoceress< CDnLocalPlayerActor >::ResetActor();

	m_LastSendPitchMsg = 0;
	m_fPrevPitchAngle = 0.f;
}

void CDnLPASoceress::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnPlayerSoceress< CDnLocalPlayerActor >::Process( LocalTime, fDelta );

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
//	if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) 
	{
		if( IsDie() || IsDown() ) {
			m_fPitchAngle = 0.f;
		}
		else {
			float fDot = EtVec3Dot( &EtVector3( 0.f, 1.f, 0.f ), &m_Crosshair.m_vZAxis );
			if( fDot >= 1.f ) fDot = 1.f;
			m_fPitchAngle = EtToDegree( EtAcos(fDot) ) - 90.f;
		}
	}
//	else m_fPrevPitchAngle = m_fPitchAngle;

	if( fabs( m_fPrevPitchAngle - m_fPitchAngle ) > 10.f && LocalTime - m_LastSendPitchMsg > REFRESH_PITCH_TICK ) {
		BYTE pBuffer[128];
		CMemoryStream Stream( pBuffer, 128 );

		Stream.Write( &m_fPitchAngle, sizeof(float) );

		Send( eActor::CS_PLAYERPITCH, &Stream );
		m_fPrevPitchAngle = m_fPitchAngle;
		m_LastSendPitchMsg = LocalTime;
	}
}

bool CDnLPASoceress::IsCanToggleBattleMode()
{
	if( strstr( GetCurrentAction(), "Charge_" ) || strstr( m_szCustomAction.c_str(), "Charge_" ) ) return false;
	return true;
}