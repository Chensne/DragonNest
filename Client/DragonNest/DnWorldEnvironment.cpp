#include "StdAfx.h"
#include "DnWorldEnvironment.h"
#include "DnWorld.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
#include "GameOption.h"
#include "EtBackBufferMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define DEFAULT_FAR_VALUE	2000

CDnWorldEnvironment::CDnWorldEnvironment()
{
	m_bActivate = false;

	
	m_pColorAdjFilter = NULL;
	m_pBloomFilter = NULL;
	m_pHeatHazeFilter = NULL;

	
	m_nEnviBGMIndex = -1;

	m_nDOFCameraEffectIndex = -1;

	memset( m_bControlColorFilter, 0, sizeof(m_bControlColorFilter) );
	memset( m_fControlColorFilterDurationDelta, 0, sizeof(m_fControlColorFilterDurationDelta) );
	memset( m_fControlColorFilterDuration, 0, sizeof(m_fControlColorFilterDuration) );
	m_fControlColorAdjSaturation = 0.f;	

	m_pOutlineFilter = NULL;
	m_pTransitionFilter = NULL;

	//blondy
	m_fColorAdjSaturationBackUp = 1.0f;
	//blondy end

}

CDnWorldEnvironment::~CDnWorldEnvironment()
{
	/*
	SAFE_RELEASE_SPTR( m_hSkybox );
	SAFE_RELEASE_SPTRVEC( m_hVecLight );
	SAFE_DELETE( m_pColorAdjFilter );
	SAFE_DELETE( m_pBloomFilter );	
	SAFE_DELETE( m_pHeatHazeFilter );
	SAFE_DELETE( m_pOutlineFilter );
	SAFE_DELETE( m_pTransitionFilter );

	SAFE_RELEASE_SPTR( m_hRain );
	SAFE_RELEASE_SPTR( m_hSplashDrop );
	SAFE_RELEASE_SPTR( m_hSnowEffect );
	SAFE_DELETE_PVEC( m_pVecLightInfo );
	SAFE_RELEASE_SPTRVEC( m_hVecLensFlare );	

	if( m_nEnviBGMIndex != -1 ) {
		CEtSoundEngine::GetInstance().RemoveSound( m_nEnviBGMIndex );
		m_nEnviBGMIndex = -1;
	}
	SAFE_RELEASE_SPTR( m_hEnviBGMChannel );
	*/
	Finalize();
}
 
bool CDnWorldEnvironment::LoadEnvironment( const char *szEnviFileName )
{
	bool bResult = m_EnvInfo.Load(szEnviFileName);
	if (bResult)
	{
		m_fColorAdjSaturationBackUp = m_EnvInfo.GetColorAdjSaturation();
		ResetSaturation();
	}
	
	return bResult;
}

void CDnWorldEnvironment::ApplyColorFilter( EtTextureHandle hDestTarget, EtTextureHandle hSrcTarget )
{
	if( !m_pColorAdjFilter ) {
		return;
	}

	float fSat = 0.f;
	fSat = m_pColorAdjFilter->GetSceneSaturation();
	m_pColorAdjFilter->SetSceneSaturation( 1.0f );

	bool bEnableZ = GetEtDevice()->EnableZ( false );	
	bool bAlphaEnable = GetEtDevice()->EnableAlphaBlend( true );
	GetEtDevice()->SetDepthStencilSurface( NULL );
	m_pColorAdjFilter->SetTexture( hSrcTarget->GetMyIndex() );
	m_pColorAdjFilter->DrawFilter( hDestTarget, 1, EtVector2(0, 0), EtVector2(1, 1) , true);
	m_pColorAdjFilter->SetTexture( GetEtBackBufferMng()->GetBackBufferIndex() );	
	GetEtDevice()->RestoreRenderTarget();
	GetEtDevice()->RestoreDepthStencil();
	GetEtDevice()->EnableZ( bEnableZ );
	GetEtDevice()->EnableAlphaBlend( bAlphaEnable );	
	m_pColorAdjFilter->SetSceneSaturation( fSat );

}


bool CDnWorldEnvironment::Initialize( DnCameraHandle hCamera, int nCameraFar, float fWaterFarRatio )
{
	m_hEnviCamera = hCamera;
	// Ambient Setting
	EtColor Color = m_EnvInfo.GetAmbient();
	m_PrevAmbient = EternityEngine::GetGlobalAmbient();
	EternityEngine::SetGlobalAmbient( &EtColor( Color.b, Color.g, Color.r, 1.f ) );

	// Color Filter
	SAFE_DELETE( m_pColorAdjFilter );
	if( m_EnvInfo.IsEnableColorFilter() ) {
		m_pColorAdjFilter = (CEtColorAdjustTexFilter*)EternityEngine::CreateFilter( SF_COLORADJUSTTEX );
		m_pColorAdjFilter->SetParam( *m_EnvInfo.GetColorAdjShadow(), *m_EnvInfo.GetColorAdjMidtones(), *m_EnvInfo.GetColorAdjHighlights(), m_EnvInfo.GetColorAdjSaturation() );
	}

	bool bEnableVolumeFog = true;
	if( CGameOption::GetInstance().GetCurGraphicQuality() >= 2 )
	{
		m_EnvInfo.EnableBloomFilter(false);//m_bEnableBloomFilter = false;
		m_EnvInfo.EnableHeatHaze(false);//m_bEnableHeatHaze = false;
		m_EnvInfo.EnableWeather(false);//m_bEnableWeather = false;
		m_EnvInfo.EnableSplashDrop(false);//m_bEnableSplashDrop = false;
		m_EnvInfo.EnableSnow(false);//m_bEnableSnowEffect = false;
		m_EnvInfo.EnableDOF(false);//m_bEnableDOF = false;
		bEnableVolumeFog = false;
	}

	// Bloom Filter
	SAFE_DELETE( m_pBloomFilter );
	if( m_EnvInfo.IsEnableBloomFilter() ) {
		m_pBloomFilter = (CEtBloomFilter*)EternityEngine::CreateFilter( SF_BLOOM );
		m_pBloomFilter->SetBloomScale( m_EnvInfo.GetBloomScale() );
		m_pBloomFilter->SetBloomSensitivity( m_EnvInfo.GetBloomSensitivity() );
	}
	
	// Fog Setting
	if( hCamera ) {		
		SCameraInfo *pInfo = hCamera->GetCameraInfo();
		m_PrevCameraInfo = *pInfo;

		pInfo->fFogNear = m_EnvInfo.GetFogNear();;
		pInfo->fFogFar = m_EnvInfo.GetFogFar();
		if( nCameraFar > 0 ) {
			ASSERT( pInfo->fFogFar < nCameraFar );

			if( pInfo->fFogFar < nCameraFar + DEFAULT_FAR_VALUE )
				pInfo->fFar = (float)nCameraFar;
			else
				pInfo->fFar = pInfo->fFogFar + DEFAULT_FAR_VALUE;
		}
		else
			pInfo->fFar = pInfo->fFogFar + DEFAULT_FAR_VALUE;

		pInfo->fWaterFarRatio = fWaterFarRatio;

		Color = m_EnvInfo.GetFogColor();
		pInfo->FogColor = EtColor( Color.b, Color.g, Color.r, 1.f );

		hCamera->DeActivate();
		hCamera->Activate();
	}
	// Environment Texture Setting
	SAFE_RELEASE_SPTR( m_hEnviTexture );
	if( m_EnvInfo.GetEnvironmentTexture() && strlen(m_EnvInfo.GetEnvironmentTexture()) > 0 ) {
		m_hEnviTexture = EternityEngine::LoadEnvTexture( m_EnvInfo.GetEnvironmentTexture() );
	}

	// Light Setting
	EtLightHandle hLight;
	EtLensFlareHandle hLensFlare;
	CEnvInfo::LightInfoStruct *pStruct;
	for( DWORD i=0; i<m_EnvInfo.GetLightCount(); i++ ) {
		pStruct = m_EnvInfo.GetLight(i);
		pStruct->pInfo->Diffuse.r *= pStruct->pInfo->Diffuse.a;
		pStruct->pInfo->Diffuse.g *= pStruct->pInfo->Diffuse.a;
		pStruct->pInfo->Diffuse.b *= pStruct->pInfo->Diffuse.a;
		pStruct->pInfo->Specular.r *= pStruct->pInfo->Specular.a;
		pStruct->pInfo->Specular.g *= pStruct->pInfo->Specular.a;
		pStruct->pInfo->Specular.b *= pStruct->pInfo->Specular.a;
		hLight = EternityEngine::CreateLight( pStruct->pInfo );
		m_hVecLight.push_back( hLight );

		// LensFlare Setting
		if( pStruct->bEnable ) {
			MatrixEx MatExTemp;
			MatExTemp.m_vZAxis = pStruct->pInfo->Direction;
			MatExTemp.MakeUpCartesianByZAxis();
			MatExTemp.RotateYAxis( pStruct->fRevisionYAxisAngle );
			MatExTemp.RotatePitch( pStruct->fRevisionXAxisAngle );
			hLensFlare = CEtLensFlare::CreateLensFlare( MatExTemp.m_vZAxis, CEtResourceMng::GetInstance().GetFullName( pStruct->szSunTexture ).c_str(), CEtResourceMng::GetInstance().GetFullName( pStruct->szLensFlareTexture ).c_str() );
			if( !hLensFlare ) continue;
			hLensFlare->SetFlareRadius( pStruct->fSunScale, pStruct->fLensScale );

			m_hVecLensFlare.push_back( hLensFlare );
		}
	}
	// SkyBox Setting
	SAFE_RELEASE_SPTR( m_hSkybox );
	if( m_EnvInfo.GetSkyBox() && strlen(m_EnvInfo.GetSkyBox()) > 0 ) {
		m_hSkybox = EternityEngine::CreateStaticObject( m_EnvInfo.GetSkyBox() );
		if( m_hSkybox ) 
		{
			m_hSkybox->EnableCull( false );
			m_hSkybox->EnableWaterCast( true );
		}
	}	

	// Environment BGM
	if( m_EnvInfo.GetEnviBGM() && strlen(m_EnvInfo.GetEnviBGM()) > 0 ) {
		m_nEnviBGMIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( m_EnvInfo.GetEnviBGM() ).c_str(), false, true );
		if( m_nEnviBGMIndex != -1 ) {
			m_hEnviBGMChannel = CEtSoundEngine::GetInstance().PlaySound( "BGM", m_nEnviBGMIndex, true, false );
			if( m_hEnviBGMChannel ) {
				m_hEnviBGMChannel->SetVolume( m_EnvInfo.GetEnviBGMRatio() );
				m_hEnviBGMChannel->Resume();
			}
		}
	}
	// Weather Setting
	SAFE_RELEASE_SPTR( m_hRain );
	if( m_EnvInfo.IsEnableWeather() ) {
		m_hRain = CEtRainEffect::CreateRainEffect( CEtResourceMng::GetInstance().GetFullName( m_EnvInfo.GetWeatherTexture() ).c_str() );
		if( m_hRain ) {
			m_hRain->SetRainSpeed( m_EnvInfo.GetWeatherSpeed() );
			m_hRain->SetStretchValue( m_EnvInfo.GetWeatherStretch() );
			m_hRain->SetLayerCount( m_EnvInfo.GetWeatherLayerCount() );
			m_hRain->SetRainSpeedDecreaseRate( m_EnvInfo.GetWeatherLayerDecreaseRate() );
			m_hRain->SetRainDir( *m_EnvInfo.GetWeatherLayerDir() );
		}
	}	

	// Splash Drop Setting
	SAFE_RELEASE_SPTR( m_hSplashDrop );
	if( m_EnvInfo.IsEnableSplashDrop() ) {
		m_hSplashDrop = CEtRainDropSplash::CreateRainDropSplash( CEtResourceMng::GetInstance().GetFullName( m_EnvInfo.GetSplashDropTexture() ).c_str(), m_EnvInfo.GetSplashDropSize() );
		if( m_hSplashDrop ) {
			m_hSplashDrop->SetLifeTime( m_EnvInfo.GetSplashDropLifeTime() );
			m_hSplashDrop->SetCreationRadius( (float)m_EnvInfo.GetSplashDropRadius() );
			m_hSplashDrop->SetSplashPerSecond( m_EnvInfo.GetSplashDropCount() );
		}
	}
	
	// Snow Effect
	SAFE_RELEASE_SPTR( m_hSnowEffect );
	if( m_EnvInfo.IsEnableSnow() ) {
		m_hSnowEffect = CEtSnowEffect::CreateSnowEffect( CEtResourceMng::GetInstance().GetFullName( m_EnvInfo.GetSnowTexture() ).c_str(), m_EnvInfo.GetSnowCount(), m_EnvInfo.GetSnowSize() );
		m_hSnowEffect->SetSnowTimeOut( m_EnvInfo.GetSnowTimeOut() );
		m_hSnowEffect->SetSnowHeight( m_EnvInfo.GetSnowHeight() );
		m_hSnowEffect->SetIsUp(m_EnvInfo.GetIsUp());
		m_hSnowEffect->SetSnowAniSpeed(m_EnvInfo.GetSnowAniSpeed());
	}

	// Head Haze Filter Setting
	SAFE_DELETE( m_pHeatHazeFilter );
	if( m_EnvInfo.IsEnableHeatHaze() ) {
		m_pHeatHazeFilter = (CEtHeatHazeFilter*)EternityEngine::CreateFilter( SF_HEATHAZE );
		m_pHeatHazeFilter->SetTexture("Perturbation.dds");
		m_pHeatHazeFilter->SetDistortionPower( m_EnvInfo.GetHeatHazeDistortionPower() );
		m_pHeatHazeFilter->SetWaveSpeed( m_EnvInfo.GetHeatHazeWaveSpeed() );
	}

	// DOF Filter Setting
	if( m_EnvInfo.IsEnableDOF() ) {
		if( hCamera ) {
			m_nDOFCameraEffectIndex = hCamera->DepthOfField( -1, m_EnvInfo.GetDOFFocusDistance(), m_EnvInfo.GetDOFNearStart(), m_EnvInfo.GetDOFNearEnd(), m_EnvInfo.GetDOFFarStart(), m_EnvInfo.GetDOFFarEnd(), m_EnvInfo.GetDOFNearBlur(), m_EnvInfo.GetDOFFarBlur(), true );
		}
	}

	// Camera Blur Setting
	if( hCamera ) {
		// 위 DOFCameraEffect 처럼 Index하나 구해놔서 삭제하는 코드 둘까 하다가 어차피 항상 RemoveAllCameraEffect로 삭제되기때문에 그냥 두기로한다.
		hCamera->CreateCameraBlurFilter();
	}

	// Volume Fog
	if( ( CDnWorld::IsActive() ) && ( bEnableVolumeFog ) )
	{
		std::vector<CEtWorldEventControl *> pVecControl;
		CDnWorld::GetInstance().FindEventControlFromUniqueID( ETE_VolumeFog, &pVecControl );

		EtVolumeFogHandle hVolumeFog;
		BOOL bWeatherCondition[CDnWorld::WeatherEnum_Amount];
		for( DWORD i=0; i<pVecControl.size(); i++ ) {
			CEtWorldEventControl *pControl = pVecControl[i];
			for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) {
				CEtWorldEventArea *pArea = pControl->GetAreaFromIndex(j);
				VolumeFogStruct *pStruct = (VolumeFogStruct *)pArea->GetData();
				memcpy( bWeatherCondition, &pStruct->bApplyFineDay, sizeof(BOOL) * CDnWorld::WeatherEnum_Amount );
				if( bWeatherCondition[ CDnWorld::GetInstance().GetCurrentWeather() ] == FALSE ) continue;

				float fRadius = max( pArea->GetOBB()->Extent[0], pArea->GetOBB()->Extent[2] );
				float fHeight = pArea->GetMax()->y - pArea->GetMin()->y;
				D3DXCOLOR Color = *pStruct->vFogColor;
				EtVector3 vCenter = pArea->GetOBB()->Center;
				vCenter.y = CDnWorld::GetInstance().GetHeight( vCenter );
				vCenter.y += pArea->GetMin()->y;

				hVolumeFog = CEtVolumeFog::CreateVolumeFog( pStruct->szTextureName, vCenter, fRadius, pStruct->fDensity, fHeight, Color );
				m_hVecVolumeFog.push_back( hVolumeFog );
			}
		}
	}

	m_pOutlineFilter = (CEtOutlineFilter*)EternityEngine::CreateFilter( SF_OUTLINE );
	m_pTransitionFilter = (CEtTransitionFilter*)EternityEngine::CreateFilter( SF_TRANSITION );

	SAFE_RELEASE_SPTR( m_hEnviEffect );
	if( m_EnvInfo.GetEnvironmentEffect() && strlen(m_EnvInfo.GetEnvironmentEffect()) > 0 ) {
		m_hEnviEffect = (new CDnEtcObject(true))->GetMySmartPtr();
		if( !m_hEnviEffect->Initialize( NULL, NULL, m_EnvInfo.GetEnvironmentEffect() ) )
			SAFE_RELEASE_SPTR( m_hEnviEffect );
		if( m_hEnviEffect ) m_hEnviEffect->SetActionQueue( "Idle" );
	}

	m_bActivate = true;
	hCamera->ForceRefreshEngineCamera();
	
	return true;
}

void CDnWorldEnvironment::Finalize()
{
	if( m_bActivate == false ) return;

	m_EnvInfo.Reset();

	
	SAFE_RELEASE_SPTRVEC( m_hVecLight );
	
	SAFE_RELEASE_SPTR( m_hSkybox );
	SAFE_RELEASE_SPTR( m_hEnviTexture );
	SAFE_DELETE( m_pColorAdjFilter );
	SAFE_DELETE( m_pBloomFilter );	
	SAFE_DELETE( m_pHeatHazeFilter );
	SAFE_DELETE( m_pOutlineFilter );
	SAFE_DELETE( m_pTransitionFilter );
	SAFE_RELEASE_SPTRVEC( m_hVecLensFlare );
	SAFE_RELEASE_SPTR( m_hRain );
	SAFE_RELEASE_SPTR( m_hSplashDrop );
	SAFE_RELEASE_SPTR( m_hSnowEffect );
	SAFE_RELEASE_SPTRVEC( m_hVecVolumeFog );
	SAFE_RELEASE_SPTR( m_hEnviEffect );

	memset( m_bControlColorFilter, 0, sizeof(m_bControlColorFilter) );
	memset( m_fControlColorFilterDurationDelta, 0, sizeof(m_fControlColorFilterDurationDelta) );
	memset( m_fControlColorFilterDuration, 0, sizeof(m_fControlColorFilterDuration) );
	m_fControlColorAdjSaturation = 0.f;

	// Ambient Restore
	EternityEngine::SetGlobalAmbient( &m_PrevAmbient );

	// Fog Restore
	if( m_hEnviCamera ) {
		SCameraInfo *pInfo = m_hEnviCamera->GetCameraInfo();
		*pInfo = m_PrevCameraInfo;

		if( m_nDOFCameraEffectIndex != -1 )
			m_hEnviCamera->RemoveCameraEffect( m_nDOFCameraEffectIndex );

		m_nDOFCameraEffectIndex = -1;
		m_hEnviCamera->DeActivate();
		m_hEnviCamera->Activate();
	}
	m_nDOFCameraEffectIndex = -1;
	
	// Sound Restore
	if( CEtSoundEngine::IsActive() ) {
		if( m_nEnviBGMIndex != -1 ) {
			CEtSoundEngine::GetInstance().RemoveSound( m_nEnviBGMIndex );
			m_nEnviBGMIndex = -1;
		}
		SAFE_RELEASE_SPTR( m_hEnviBGMChannel );
	}
	else {
		m_nEnviBGMIndex = -1;
		m_hEnviBGMChannel.Identity();
	}

	m_bActivate = false;
}

void CDnWorldEnvironment::SetEnviBGMVolume( float fValue )
{
	if( !m_hEnviBGMChannel ) return;

	m_hEnviBGMChannel->SetVolume( fValue );
}

EtVector4 CDnWorldEnvironment::ControlColorFilter( ColorFilterParamEnum Type, EtVector4 vValue, int nDuration ) 
{
	if( m_bControlColorFilter[Type] ) {
		switch( Type ) {
			case Shadow: m_EnvInfo.SetColorAdjShadow(*m_pColorAdjFilter->GetSceneShadow()); break;
			case Midtone: m_EnvInfo.SetColorAdjMidtones(*m_pColorAdjFilter->GetSceneMidTone()); break;
			case Highlight: m_EnvInfo.SetColorAdjHighlights(*m_pColorAdjFilter->GetSceneHilight()); break;
		}
	}

	m_bControlColorFilter[Type] = true;
	m_fControlColorFilterDuration[Type] = nDuration * 0.001f;
	m_fControlColorFilterDurationDelta[Type] = m_fControlColorFilterDuration[Type];
	switch( Type ) {
		case Shadow: 
			m_vControlColorAdjShadow = vValue;
			return *m_EnvInfo.GetColorAdjShadow();
		case Midtone: 
			m_vControlColorAdjMidtone = vValue;
			return *m_EnvInfo.GetColorAdjMidtones();
		case Highlight:
			m_vControlColorAdjHighlight = vValue;
			return *m_EnvInfo.GetColorAdjHighlights();
	}
	return EtVector4( 0.f, 0.f, 0.f, 0.f );
}

float CDnWorldEnvironment::ControlColorFilter( ColorFilterParamEnum Type, float fValue, int nDuration ) 
{
	if( NULL == m_pColorAdjFilter )
		return 0.f;

	if( m_bControlColorFilter[Type] ) {
		switch( Type ) {
			case Saturation: m_EnvInfo.SetColorAdjSaturation(m_pColorAdjFilter->GetSceneSaturation()); break;
		}
	}
	m_bControlColorFilter[Type] = true;
	m_fControlColorFilterDuration[Type] = nDuration * 0.001f;
	m_fControlColorFilterDurationDelta[Type] = m_fControlColorFilterDuration[Type];
	switch( Type ) {
		case Saturation: 
			m_fControlColorAdjSaturation = fValue;
			return m_EnvInfo.GetColorAdjSaturation();
	}
	return 0.f;
}

//blondy 컬러필터효과를 임의로 해제 할수있는 메소드 

void CDnWorldEnvironment::ResetSaturation()
{
	//m_fColorAdjSaturation = m_fColorAdjSaturation; 

	m_bControlColorFilter[Saturation] = false;	
	m_fControlColorFilterDuration[Saturation] = 0.0f;

	if( m_pColorAdjFilter )
	m_pColorAdjFilter->SetSceneSaturation( m_fColorAdjSaturationBackUp );	

}

//blondy end

void CDnWorldEnvironment::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_pColorAdjFilter ) {
		bool bParamModified = false;
		for( int i=0; i<ColorFilterParamEnum_Amount; i++ ) {
			if( !m_bControlColorFilter[i] ) continue;
			m_fControlColorFilterDurationDelta[i] -= fDelta;
			if( m_fControlColorFilterDurationDelta[i] < 0.f ) {
				m_fControlColorFilterDurationDelta[i] = 0.f;
				m_bControlColorFilter[i] = false;
			}
			float fRatio = 1.f - ( ( 1.f / m_fControlColorFilterDuration[i] ) * m_fControlColorFilterDurationDelta[i] );

			switch( i ) {
				case Shadow:
					{
						EtVector4 vTemp;
						D3DXVec4Lerp( &vTemp, m_EnvInfo.GetColorAdjShadow(), &m_vControlColorAdjShadow, fRatio );
						if( !m_bControlColorFilter[i] ) 
						{
							m_EnvInfo.SetColorAdjShadow(m_vControlColorAdjShadow);
							bParamModified = true;
						}
					}
					break;
				case Midtone: 
					{
						EtVector4 vTemp;
						D3DXVec4Lerp( &vTemp, m_EnvInfo.GetColorAdjMidtones(), &m_vControlColorAdjMidtone, fRatio );
						if( !m_bControlColorFilter[i] ) 
						{
							m_EnvInfo.SetColorAdjMidtones(m_vControlColorAdjMidtone);
							bParamModified = true;
						}
					}
					break;
				case Highlight:
					{
						EtVector4 vTemp;
						D3DXVec4Lerp( &vTemp, m_EnvInfo.GetColorAdjHighlights(), &m_vControlColorAdjHighlight, fRatio );
						if (m_pColorAdjFilter)
						{
							EtVector4* pCurShadowColor = m_pColorAdjFilter->GetSceneShadow();
							EtVector4* pCurMidtone = m_pColorAdjFilter->GetSceneMidTone();
							float curSaturation = m_pColorAdjFilter->GetSceneSaturation();

							if (pCurShadowColor && pCurMidtone)
								m_pColorAdjFilter->SetParam(*pCurShadowColor, *pCurMidtone, vTemp, curSaturation);
						}
						if( !m_bControlColorFilter[i] ) 
						{
							m_EnvInfo.SetColorAdjHighlights(m_vControlColorAdjHighlight);
							bParamModified = true;
						}
					}
					break;
				case Saturation: 
					{
						float fTemp;
						fTemp = m_EnvInfo.GetColorAdjSaturation() + ( ( m_fControlColorAdjSaturation - m_EnvInfo.GetColorAdjSaturation() ) * fRatio );
						m_pColorAdjFilter->SetSceneSaturation( fTemp );
						if( !m_bControlColorFilter[i] ) m_EnvInfo.SetColorAdjSaturation(m_fControlColorAdjSaturation);
					}
					break;
			}
		}
		if( bParamModified )
		{
			m_pColorAdjFilter->SetParam( *m_EnvInfo.GetColorAdjShadow(), *m_EnvInfo.GetColorAdjMidtones(), *m_EnvInfo.GetColorAdjHighlights(), m_EnvInfo.GetColorAdjSaturation());
		}
	}
	if( CDnWorld::IsActive() ) {
		if( m_hRain ) {
			EtVector3 vMovement = *CDnWorld::GetInstance().GetUpdatePosition() - *CDnWorld::GetInstance().GetUpdatePrevPosition();
			m_hRain->SetMovement( vMovement );
		}
		if( m_hEnviEffect ) {
			if( CDnCamera::GetActiveCamera() ) {
				m_hEnviEffect->SetPosition( CDnCamera::GetActiveCamera()->GetMatEx()->m_vPosition );
			}
		}
	}
}


// PRE_ADD_FILTEREVENT
void CDnWorldEnvironment::SetSceneAbsoluteColor( bool bMonochrome, EtVector3 & vColor, float fVolume )
{
	if( m_pColorAdjFilter )
		m_pColorAdjFilter->SetSceneAbsoluteColor( bMonochrome, vColor, fVolume );
}
//