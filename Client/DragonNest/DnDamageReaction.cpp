#include "StdAfx.h"
#include "EtDrawQuad.h"
#include "DnDamageReaction.h"
#include "DnInterface.h"
#include "DnBlindDlg.h"
#include "GameOption.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define DIRECTION_HIT_DURATION (0.4f)
#define DIRECTION_ALPHA_FINISH_DURATION (1.0f)
//#define DIRECTION_ALPHA_START_DURATION (0.1f)
#define ALPHA_RATIO (0.6f)

extern float s_fCrossHairHeightRatio;// = 0.46f;		// define in DnLocalPlayerActor.cpp 

CDnDamageReaction::CDnDamageReaction()
{
	m_fHealthWarningPercent = 1.f;
	m_fHealthWarningTime = 1000.f;
	m_fWarningSoundTime = 0.f;
	m_nWarningState = 0;
	m_nPrevHP = 0;
	m_bGuildWarAlarm = false;
	m_fGuildWarAlarmTime = 0.f;
	memset( m_nHealthWarningSound, 0, sizeof( m_nHealthWarningSound ) );
}

CDnDamageReaction::~CDnDamageReaction()
{
	Clear();
}

void CDnDamageReaction::Clear()
{
	for( int i = 0; i < 2; i++) {
		SAFE_RELEASE_SPTR( m_hHealthWarningChannel[ i ] );
		SAFE_RELEASE_SPTR( m_hDamageDirectionTexture[ i ] );
		CEtSoundEngine::GetInstance().RemoveSound( m_nHealthWarningSound[ i ] );		
	}
	SAFE_RELEASE_SPTR( m_hHealthWarningTexture );
}

void CDnDamageReaction::Initialize()
{
	m_hHealthWarningTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "LowHealthWarning.dds" ).c_str(), RT_TEXTURE );
	m_hDamageDirectionTexture[ 0 ] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "DamageDirection04.dds" ).c_str(), RT_TEXTURE );	// melee
	m_hDamageDirectionTexture[ 1 ] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "DamageDirection03.dds" ).c_str(), RT_TEXTURE );	// range

	m_nHealthWarningSound[ 0 ] = CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10026 ), false, false );
	m_nHealthWarningSound[ 1 ] = CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10027 ), false, false );
	m_hHealthWarningChannel[ 0 ] = CEtSoundEngine::GetInstance().PlaySound( "3D", m_nHealthWarningSound[ 0 ], true, true );
	m_hHealthWarningChannel[ 1 ] = CEtSoundEngine::GetInstance().PlaySound( "3D", m_nHealthWarningSound[ 1 ], true, true );

	if( m_hHealthWarningChannel[ 0 ] ) 
		m_hHealthWarningChannel[ 0 ]->SetVolume( 1.f );
	if( m_hHealthWarningChannel[ 1 ] )
		m_hHealthWarningChannel[ 1 ]->SetVolume( 1.f );
}

void CDnDamageReaction::Process( float fDelta, int nHP, int nHPPercent )
{
	int nState = 0;
	if( nHP > 0 && nHPPercent <= 20 && nHPPercent > 10 ) {
		nState = 1;	
	}
	else if ( nHP > 0 && nHPPercent <= 10 ) {
		nState = 2;			
	}

	if( m_bGuildWarAlarm )
		nState = 2;

	if( nState != m_nWarningState ) {
		m_fWarningSoundTime = 0.f;
		switch( nState ) {
			case 0:
				if( m_hHealthWarningChannel[ 0 ] && !m_hHealthWarningChannel[ 0 ]->IsPause() ) m_hHealthWarningChannel[ 0 ]->Pause();
				if( m_hHealthWarningChannel[ 1 ] && !m_hHealthWarningChannel[ 1 ]->IsPause() ) m_hHealthWarningChannel[ 1 ]->Pause();
				break;
			case 1:
				if( m_hHealthWarningChannel[ 0 ] && m_hHealthWarningChannel[ 0 ]->IsPause() ) m_hHealthWarningChannel[ 0 ]->Resume();
				if( m_hHealthWarningChannel[ 1 ] && !m_hHealthWarningChannel[ 1 ]->IsPause() ) m_hHealthWarningChannel[ 1 ]->Pause();
				break;
			case 2:
				if( m_hHealthWarningChannel[ 0 ] && !m_hHealthWarningChannel[ 0 ]->IsPause() ) m_hHealthWarningChannel[ 0 ]->Pause();
				if( m_hHealthWarningChannel[ 1 ] && m_hHealthWarningChannel[ 1 ]->IsPause() ) m_hHealthWarningChannel[ 1 ]->Resume();
				break;
		}
	}
	else {
		m_fWarningSoundTime += fDelta;
		if( m_fWarningSoundTime > 15.f ) {
			if( m_hHealthWarningChannel[ 0 ] && !m_hHealthWarningChannel[ 0 ]->IsPause() ) m_hHealthWarningChannel[ 0 ]->Pause();
			if( m_hHealthWarningChannel[ 1 ] && !m_hHealthWarningChannel[ 1 ]->IsPause() ) m_hHealthWarningChannel[ 1 ]->Pause();
		}
	}

	m_nWarningState = nState;
}

void CDnDamageReaction::Render( float fDelta, int nHP, float fHPPercentFloat, EtVector3 vPos )
{
	float fHP = fHPPercentFloat;

	DWORD dwAlphaTest = 0, dwAlphaBlend = 0;
	GetEtDevice()->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTest);
	GetEtDevice()->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);

	GetEtDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
	GetEtDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	bool bEnableZ = GetEtDevice()->EnableZ( false );

	if( GetInterface().GetBlindDialog()->GetBlindMode() != CDnBlindDlg::modeClosed || nHP == 0 )
	{
		fHP = 1.0f;
	}

	if( nHP < m_nPrevHP ) {
		m_fHealthWarningTime = 0.f;		
	}
	m_nPrevHP = nHP;
	m_fHealthWarningTime += fDelta;

	m_fHealthWarningPercent += (fHP - m_fHealthWarningPercent) * min(1.0f, ( fDelta * 1.f ));

	// Render Red Warning Image	
	float fAlphaPercent, fDuration, fFastShakeDuration = 2.5f;

	if( m_bGuildWarAlarm )
	{
		fAlphaPercent = abs(sinf( m_fGuildWarAlarmTime ));

		m_fGuildWarAlarmTime -= fDelta;

		if( m_fGuildWarAlarmTime < 0.f )
			m_bGuildWarAlarm = false;
	}
	else
	{
		fAlphaPercent = (1.0f - (max(0,(m_fHealthWarningPercent - 0.02f)) / 0.2f));

		fFastShakeDuration = 2.5f;
		fDuration = m_fHealthWarningTime > fFastShakeDuration ? 3.5f : 2.5f;

		fAlphaPercent += 0.6f * sinf( ET_PI * (m_fHealthWarningTime / fDuration) * 7.0f ) * max( 0.15f + (1.0f-m_fHealthWarningPercent)*0.2f , (1 - (m_fHealthWarningTime / fFastShakeDuration) ));
		fAlphaPercent *= 0.7f;
		fAlphaPercent = min( fAlphaPercent , 1.0f);
	}

	if( fAlphaPercent > 0.f ) {
		BYTE btAlpha = (BYTE)(255 * fAlphaPercent);
		if( m_hHealthWarningTexture ) {
			DrawQuadWithDiffuseTex( &EtVector2(0, 0), &EtVector2(1, 1),
				&EtVector2(0, 0), &EtVector2(1, 1),
				m_hHealthWarningTexture->GetTexturePtr(), D3DCOLOR_ARGB(btAlpha, 255,255,255) );
		}
	}

	if( m_hDamageDirectionTexture[0] && m_hDamageDirectionTexture[1] && !m_Hitters.empty() ) {
		int nWidth = GetEtDevice()->Width();		
		int nHeight = GetEtDevice()->Height();

		float fWidth = 512.0f / nWidth;
		float fHeight = 512.0f / nHeight;

		EtVector2 vPoint[ 4 ];
		STextureDiffuseVertex2D Vertices[ 4 ];

		GetEtDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		GetEtDevice()->SetPixelShader( NULL );
		GetEtDevice()->SetVertexShader( NULL );

		Vertices[ 0 ].TexCoordinate = EtVector2( 0, 0 );		
		Vertices[ 1 ].TexCoordinate = EtVector2( 1, 0 );				
		Vertices[ 2 ].TexCoordinate = EtVector2( 1, 1 );		
		Vertices[ 3 ].TexCoordinate = EtVector2( 0, 1 );

		for( std::vector< boost::tuple< DnActorHandle, int, float > >::iterator it = m_Hitters.begin(); it != m_Hitters.end(); ) {

			DnActorHandle hActor = it->get<0>();
			int nDistanceType = it->get<1>();
			float &fDurationTime = it->get<2>();

			if( !hActor || fDurationTime < -DIRECTION_ALPHA_FINISH_DURATION ) {
				it = m_Hitters.erase( it );
			}
			else {

				EtVector3 vCamDir(0, 0, 1);
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( hCamera )
				{
					vCamDir = hCamera->GetMatEx()->m_vZAxis;
					vCamDir.y = 0.0f;
					EtVec3Normalize(&vCamDir, &vCamDir);
				}

				EtVector3 vDir = *hActor->GetPosition() - vPos;
				EtVec3Normalize(&vDir, &vDir);
				
				float fAngle = EtAcos( EtVec3Dot(&vDir, &vCamDir) );
				EtVector3 vCross;
				EtVec3Cross(&vCross, &vDir, &vCamDir);
				if( vCross.y > 0.0f ) {
					fAngle = - fAngle;
				}

				static float fAlphaRatio = ALPHA_RATIO;

				BYTE cAlpha = (fDurationTime >= 0.0f) ? (BYTE)(255*fAlphaRatio) : EtClamp( (int)((fAlphaRatio*255) - (fAlphaRatio*255) * (-fDurationTime) / DIRECTION_ALPHA_FINISH_DURATION) , 0, (int)(255*fAlphaRatio));

				/*if( it->second > DIRECTION_HIT_DURATION - DIRECTION_ALPHA_START_DURATION ) {
					cAlpha = fAlphaRatio * 255 * (it->second - (DIRECTION_HIT_DURATION - DIRECTION_ALPHA_START_DURATION)) / DIRECTION_ALPHA_START_DURATION;
				}*/

				D3DCOLOR Color =  D3DCOLOR_ARGB(cAlpha, 255, 255, 255);

				float fRadius = 1.0f + (1.0f-(fDurationTime / DIRECTION_HIT_DURATION)) * 0.07f;
				
				vPoint[ 0 ] = EtVector2( -0.5f, -0.5f)*fRadius;
				vPoint[ 1 ] = EtVector2(  0.5f, -0.5f)*fRadius;
				vPoint[ 2 ] = EtVector2( 0.5f, 0.5f)*fRadius;
				vPoint[ 3 ] = EtVector2( -0.5f, 0.5f)*fRadius;
				EtMatrix MatRot;
				EtMatrixRotationZ(&MatRot, fAngle );
				for( int i = 0; i < 4; i++) {
					EtVec2TransformCoord(&vPoint[ i ], &vPoint[ i ], &MatRot);
					vPoint[ i ].x *= fWidth;
					vPoint[ i ].y *= fHeight;
					vPoint[ i ] += EtVector2( 0.5f, s_fCrossHairHeightRatio );
				}
				
				Vertices[ 0 ].Position = EtVector4( vPoint[0].x * nWidth - 0.5f, vPoint[0].y * nHeight - 0.5f, 0.0f, 1.0f );
				Vertices[ 1 ].Position = EtVector4( vPoint[1].x * nWidth - 0.5f, vPoint[1].y * nHeight - 0.5f, 0.0f, 1.0f );
				Vertices[ 2 ].Position = EtVector4( vPoint[2].x * nWidth - 0.5f, vPoint[2].y * nHeight - 0.5f, 0.0f, 1.0f );
				Vertices[ 3 ].Position = EtVector4( vPoint[3].x * nWidth - 0.5f, vPoint[3].y * nHeight - 0.5f, 0.0f, 1.0f );
				Vertices[ 0 ].Color = Color;
				Vertices[ 1 ].Color = Color;
				Vertices[ 2 ].Color = Color;
				Vertices[ 3 ].Color = Color;

				int nTextureIndex = 0;
				if( nDistanceType == CDnDamageBase::Melee ) {
					if( !CGameOption::GetInstance().bDamageDirMelee )  {
						++it;
						continue;
					}
					nTextureIndex = 0;
				}
				else if( nDistanceType == CDnDamageBase::Range) {
					if( !CGameOption::GetInstance().bDamageDirRange )  {
						++it;
						continue;
					}
					nTextureIndex = 1;
				}
				else {
					ASSERT( false );
				}

				GetEtDevice()->SetTexture( 0, m_hDamageDirectionTexture[ nTextureIndex ]->GetTexturePtr() );

				GetEtDevice()->DrawPrimitiveUP( PT_TRIANGLEFAN, 2, Vertices, sizeof( STextureDiffuseVertex2D ) );
				fDurationTime -= fDelta;
				++it;
			}
		}
	}	
	GetEtDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTest );
	GetEtDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlend );
	GetEtDevice()->EnableZ( bEnableZ );
}

void CDnDamageReaction::SetHitterHandle( DnActorHandle hHitter, int nDistanceType )
{
	if( !EtInterface::g_bEtUIRender ) return;

	for( std::vector< boost::tuple< DnActorHandle, int, float > >::iterator it = m_Hitters.begin(); it != m_Hitters.end(); ++it) {
		if( it->get<0>() == hHitter && it->get<1>() == nDistanceType ) {
			it->get<2>() = DIRECTION_HIT_DURATION;
			return;
		}
	}

	m_Hitters.push_back( boost::make_tuple( hHitter, nDistanceType, DIRECTION_HIT_DURATION) );
}
