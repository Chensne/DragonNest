#include "StdAfx.h"
#include "DnGaugeFace.h"
#include "MAActorRenderBase.h"
#include "DnActor.h"
#include "DnTableDB.h"
#include "DnWorld.h"
#include "EtDrawQuad.h"
#include "DnPlayerActor.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif

extern bool g_bSkipScene;

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGaugeFace::CDnGaugeFace( CEtUIDialog *pParent )
	: CEtUIControl( pParent )
{
	m_nFaceID = -1;
	m_FaceType = FT_PLAYER;
	m_bDamage = false;
	m_dwFaceColor = 0xffffffff;
	m_bDeleteObject = false;
	m_bBoss = false;
	m_pColorAdjFilter = NULL;
	m_bUseTempTextureForDeviceLost = false;
}

CDnGaugeFace::~CDnGaugeFace(void)
{
	SAFE_RELEASE_SPTR( m_hFaceTexture );
	SAFE_RELEASE_SPTR( m_hMaskTexture );
	SAFE_RELEASE_SPTR( m_hBossMaskTexture );
	SAFE_DELETE( m_pColorAdjFilter );
	SAFE_RELEASE_SPTR( m_hBossPortraitTexture );
	SAFE_RELEASE_SPTR( m_hTempTextureForDeviceLost );
}

void CDnGaugeFace::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );
	SAFE_RELEASE_SPTR( m_hMaskTexture );
	SAFE_RELEASE_SPTR( m_hBossMaskTexture );
	m_hMaskTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "GaugeFaceMask.dds" ).c_str(), RT_TEXTURE );
	m_hBossMaskTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "BossFaceMask.dds" ).c_str(), RT_TEXTURE );

	const int nTextureSize = 256;
	m_hFaceTexture = CEtTexture::CreateRenderTargetTexture( nTextureSize, nTextureSize, FMT_A8R8G8B8, USAGE_DEFAULT, POOL_DEFAULT );
	m_hTempTextureForDeviceLost = CEtTexture::CreateNormalTexture( nTextureSize, nTextureSize, FMT_A8R8G8B8, USAGE_DEFAULT, POOL_MANAGED );

	SCameraInfo CameraInfo;
	CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	CEtRTTRenderer::Initialize( CameraInfo, nTextureSize, nTextureSize );

	SAFE_DELETE( m_pColorAdjFilter );
	m_pColorAdjFilter = (CEtColorAdjustTexFilter*)EternityEngine::CreateFilter( SF_COLORADJUSTTEX );
	m_pColorAdjFilter->SetParam( EtVector4(-0.071f, -0.107f, -0.071f, 0.0f), EtVector4(0.0f, 0.0f, 0.0f, 0.0f), EtVector4(0.821f, 0.393f, 0.464f, 0.0f), 0.9f );
	m_pColorAdjFilter->Enable( false );
}

void CDnGaugeFace::Process( float fElapsedTime )
{
	if( m_bDamage )
	{
		if( m_fFaceTime > 1.0f )
		{
			m_dwFaceColor = 0xffffffff;
			m_bDamage = false;
		}
		else if( m_fFaceTime > 0.5f )
		{
			m_dwFaceColor = 0xffff0000;
		}
		else if( m_fFaceTime > 0.3f )
		{
			m_dwFaceColor = 0xffffffff;
		}

		m_fFaceTime += fElapsedTime;
	}
}

void CDnGaugeFace::SetFaceType( GAUGE_FACE_TYPE FaceType )
{
	m_FaceType = FaceType;
	CalcFaceCoord();
}

void CDnGaugeFace::SetFaceID( int nID ) 
{ 
	m_nFaceID = nID; 
	CalcFaceCoord();

	if( m_nFaceID != -1 )
	{
		DNTableFileFormat*  pActorTable = GetDNTable( CDnTableDB::TACTOR );

		if( !pActorTable || !pActorTable->IsExistItem(m_nFaceID + 1) )
			return;

#ifdef PRE_FIX_MEMOPT_EXT
		const char *szCamFileName = CommonUtil::GetFileNameFromFileEXT(pActorTable, m_nFaceID + 1, "_CamName");
#else
		const char *szCamFileName = pActorTable->GetFieldFromLablePtr( m_nFaceID + 1, "_CamName" )->GetString();
#endif
		if( szCamFileName == NULL )
			return;

		std::string szExt = szCamFileName;
		int nLength = (int)szExt.size();
		if( nLength >= 5 )
		{
			if( szExt[nLength-4] == '.' && szExt[nLength-3] == 'd' && szExt[nLength-2] == 'd' && szExt[nLength-1] == 's' )
			{
				// 이번 프레임은 skn, cam이 아니라 dds로 찍어야한다.
				SAFE_RELEASE_SPTR( m_hBossPortraitTexture );
				m_hBossPortraitTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( szCamFileName ).c_str(), RT_TEXTURE );
				return;
			}
		}
		// 일반 오브젝트 설정땐 설정된 dds 해제시킨다.
		SAFE_RELEASE_SPTR( m_hBossPortraitTexture );

		CResMngStream Stream( szCamFileName );
		if( Stream.IsValid() )
		{
			Stream.Read( &m_CamHeader, sizeof( SCameraFileHeader ) );
			Stream.Seek( CAMERA_HEADER_RESERVED, SEEK_CUR );
			Stream.Close();
		}

		EtMatrix CamMat, ProjMat;
		EtMatrixRotationQuaternion( &CamMat, &m_CamHeader.qDefaultRotation );

		CamMat._41 = m_CamHeader.vDefaultPosition.x;
		CamMat._42 = m_CamHeader.vDefaultPosition.y;
		CamMat._43 = m_CamHeader.vDefaultPosition.z;

		SCameraInfo CameraInfo;
		CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
		CameraInfo.fFOV = m_CamHeader.fDefaultFOV;
		InitCamera( CameraInfo );
		UpdateCamera( CamMat );
		m_hCamera->SetAspectRatio( (m_FaceCoord.fWidth * DEFAULT_UI_SCREEN_WIDTH) / (m_FaceCoord.fHeight * DEFAULT_UI_SCREEN_HEIGHT) );		// 준영씨 해둔건 1.15넹;
	}
}

void CDnGaugeFace::CalcFaceCoord()
{
	if( ( m_nFaceID == -1 ) )
	{
		return;
	}

	int nCountX, nCountY;
	float fTexWidth, fTexHeight;
	nCountX = 256 / FACE_ICON_XSIZE;
	nCountY = 256 / FACE_ICON_YSIZE;
	fTexWidth = FACE_ICON_XSIZE / ( float )256;
	fTexHeight = FACE_ICON_YSIZE / ( float )256;

	if( ( m_FaceType == FT_PLAYER ) || ( m_FaceType == FT_MINI_PLAYER ) )
	{
		m_FaceUV.SetCoord( ( m_nFaceID % nCountX ) * fTexWidth, ( m_nFaceID / nCountX ) * fTexHeight, fTexWidth, fTexHeight );
	}
	else
	{
		m_FaceUV.SetCoord( ( m_nFaceID % nCountX + 1 ) * fTexWidth, ( m_nFaceID / nCountX ) * fTexHeight, -fTexWidth, fTexHeight );
	}

	m_FaceCoord = m_Property.UICoord;
	switch( m_FaceType )
	{
		case FT_PLAYER:
			m_FaceCoord.fX += m_FaceCoord.fWidth * 0.05f;
			m_FaceCoord.fY += m_FaceCoord.fHeight * 0.1f;
			m_FaceCoord.fWidth *= 0.8f;
			m_FaceCoord.fHeight *= 0.8f;
			break;
		case FT_MINI_PLAYER:
			m_FaceCoord.fX += m_FaceCoord.fWidth * 0.1f;
			m_FaceCoord.fY += m_FaceCoord.fHeight * 0.1f;
			m_FaceCoord.fWidth *= 0.8f;
			m_FaceCoord.fHeight *= 0.8f;
			break;
		case FT_ENEMY:
			m_FaceCoord.fX += m_FaceCoord.fWidth * 0.15f;
			m_FaceCoord.fY += m_FaceCoord.fHeight * 0.1f;
			m_FaceCoord.fWidth *= 0.8f;
			m_FaceCoord.fHeight *= 0.8f;
			break;
		case FT_MINI_ENEMY:
			m_FaceCoord.fX += m_FaceCoord.fWidth * 0.1f;
			m_FaceCoord.fY += m_FaceCoord.fHeight * 0.1f;
			m_FaceCoord.fWidth *= 0.8f;
			m_FaceCoord.fHeight *= 0.8f;
			break;
	}
}

void CDnGaugeFace::Render( float fElapsedTime )
{
	if( !IsShow() )
		return;

	if( m_bUseTempTextureForDeviceLost )
	{
		if( IsLocalPlayerGaugeFace() && m_hTempTextureForDeviceLost )
		{
			m_hFaceTexture->Copy( m_hTempTextureForDeviceLost );
			m_bUseTempTextureForDeviceLost = false;
		}
	}

	SUIElement *pElement;
	
	pElement = GetElement(1);
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, m_Property.UICoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, m_Property.UICoord );
	
	m_FaceUV.SetCoord( 0, 0, 1.0f, 1.0f );	
	if( m_hFaceTexture ) m_pParent->DrawSprite( m_hFaceTexture, m_FaceUV, m_dwFaceColor, m_FaceCoord );

	pElement = GetElement(0);
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, m_Property.UICoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, m_Property.UICoord );
}

void CDnGaugeFace::OnDamage()
{
	m_bDamage = true;
	m_fFaceTime = 0.0f;
	m_dwFaceColor = 0xffff0000;
}

void CDnGaugeFace::BeginRTT()
{
	CEtRTTRenderer::BeginRTT();

	m_TempAmbient = *CEtLight::GetGlobalAmbient();
	CEtLight::SetGlobalAmbient( &EtColor( 0.568f, 0.752f, 0.850f, 1.0f ) );

	m_nDirLightCount = CEtLight::GetDirLightCount();
	m_nDirLightCount = min( m_nDirLightCount, 2 );
	for( int i = 0; i < m_nDirLightCount; ++i )
	{
		m_TempLightDiffuse[i] = CEtLight::GetDirLightInfo(i)->Diffuse;
		m_TempLightDir[i] = CEtLight::GetDirLightInfo(i)->Direction;
		CEtLight::GetDirLightInfo(i)->Diffuse = ( i == 0 ? EtColor( 1.164f, 1.116f, 0.828f, 1.2f ) : EtColor( 0.28f, 0.44f, 0.81f, 1.0f ) );
		CEtLight::GetDirLightInfo(i)->Direction = ( i == 0 ? EtVector3( -0.143f, -0.806f, -0.573f ) : EtVector3( -0.607f, -0.716f, 0.343f ) );
	}
}

void CDnGaugeFace::EndRTT()
{
	CEtLight::SetGlobalAmbient( &m_TempAmbient );
	for( int i = 0; i < m_nDirLightCount; ++i )
	{
		CEtLight::GetDirLightInfo(i)->Diffuse = m_TempLightDiffuse[i];
		CEtLight::GetDirLightInfo(i)->Direction = m_TempLightDir[i];
	}
	CEtRTTRenderer::EndRTT();
}

void CDnGaugeFace::RenderRTT( float fElapsedTime )
{
	if( !m_hFaceObject )
	{
		return;
	}

	if( !CDnActor::s_hLocalActor )
	{
		return;
	}

	// ZWrite로 마스크를 쓰워서 옆테두리 z값으로 깔아둔 다음 그 안에다가 Face그린다.
	bool bAlpha = GetEtDevice()->EnableAlphaBlend( true );
	GetEtDevice()->SetColorWriteEnable( 0 );
	EtTextureHandle hMaskTex = m_bBoss ? m_hBossMaskTexture : m_hMaskTexture;
	DrawQuadWithTex( &EtVector2( 0.0f, 0.0f ), &EtVector2( 1.0f, 1.0f ), &EtVector2( 0.0f, 0.0f ), &EtVector2( 1.0f, 1.0f ), hMaskTex->GetTexturePtr() );
	GetEtDevice()->SetColorWriteEnable( CW_ALL );
	GetEtDevice()->EnableAlphaBlend( bAlpha );

	if( m_hBossPortraitTexture )
	{
		GetEtDevice()->EnableAlphaBlend( true );
		DrawQuadWithTex( &EtVector2( 0.0f, 0.0f ), &EtVector2( 1.0f, 1.0f ), &EtVector2( 0.0f, 0.0f ), &EtVector2( 1.0f, 1.0f ), m_hBossPortraitTexture->GetTexturePtr(), 0.1f );
		GetEtDevice()->EnableAlphaBlend( bAlpha );
		m_hFaceTexture->Copy( m_hRenderTarget );
	}
	else
	{
		EtMatrix WorldMat, SaveWorldMat;
		EtMatrixIdentity( &WorldMat );
		SaveWorldMat = *m_hFaceObject->GetWorldMat();
		EtObjectHandle hParent;
		int nLinkBoneIndex = -1;
		float fObjectAlpha = 1.0f;
		bool bShowObject = false;
		bool bAlphaProcess = false;
		bool bBattleModeChanged = false;
		if( m_hFaceObject->IsAniObject() )
		{
			// 자신의 초상화라면, BattleMode검사를 한다.
			// 여기서 MAPartsBody까지 접근하고싶지는 않았는데, 배틀모드 변환타임이 딱 여기밖에 없어서 이렇게 한다.
			// 다음에 개편할때는 파티원처럼 별도의 렌더아바타를 만들어서 초상화그리는게 나을거 같다.
			MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody*>(CDnActor::s_hLocalActor.GetPointer());
			if( pPartsBody->GetObjectHandle() == m_hFaceObject )
			{
				CDnPlayerActor *pPlayerActor = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
				if( pPlayerActor->IsBattleMode() )
				{
#ifdef PRE_ADD_37745
					pPlayerActor->SetBattleMode( false , true );
#else
					pPlayerActor->SetBattleMode( false );
#endif
					bBattleModeChanged = true;
				}

				// 소서 회피기때 알파값 0이 된다. 그래서 알파처리 해줘야한다.
				bAlphaProcess = true;
			}

			// 프레일같은 무기를 시뮬레이션 두번 돌릴 경우 연산이 이상하게 되서 프레일 스프링이 심하게 꼬이게 된다.
			// 그래서 이렇게 InitRender하기전에 강제로 꺼뒀다가 제대로 반영될 다음번 InitRender에서 다시 해제한다.
			( ( EtAniObjectHandle )m_hFaceObject )->ForceSkipSimulateAni( true );
			( ( EtAniObjectHandle )m_hFaceObject )->SetExtraAniFrame( m_nAniIndex, m_fFrame );

			// 말같은데 타고있다면 부모계산때문에 매트릭스 어긋나게 된다. 강제로 없는 것처럼 설정한다.
			if( m_hFaceObject->GetParent() )
			{
				hParent = m_hFaceObject->GetParent();
				nLinkBoneIndex = m_hFaceObject->GetLinkBoneIndex();
				EtObjectHandle hTempObject;
				m_hFaceObject->SetParent( hTempObject, -1 );

				// 말에 타려고할때 SoftAppear때문에 알파가 0일수도 있다.
				bAlphaProcess = true;
			}

			if( bAlphaProcess )
			{
				fObjectAlpha = m_hFaceObject->GetObjectAlpha();
				bShowObject = m_hFaceObject->IsShow();
				m_hFaceObject->SetObjectAlpha( 1.0f );
				if( !bShowObject ) m_hFaceObject->ShowObject( true );
			}
			m_hFaceObject->SetSaveMatIndex( -1 );
		}
		m_hFaceObject->Update( &WorldMat );
		m_hFaceObject->InitRender();

		CEtRTTRenderer::RenderRTT( fElapsedTime );

		if( m_pColorAdjFilter )
		{
			// 프레리에서 쓰는걸로 통일해서 사용한다.
			float fSat = 0.f;
			fSat = m_pColorAdjFilter->GetSceneSaturation();
			m_pColorAdjFilter->SetSceneSaturation( 1.0f );
			bool bEnableZ = GetEtDevice()->EnableZ( false );	
			bool bAlphaEnable = GetEtDevice()->EnableAlphaBlend( true );
			GetEtDevice()->SetDepthStencilSurface( NULL );
			m_pColorAdjFilter->SetTexture( m_hRenderTarget->GetMyIndex() );
			m_pColorAdjFilter->DrawFilter( m_hFaceTexture, 1, EtVector2(0, 0), EtVector2(1, 1) , true );
			GetEtDevice()->RestoreRenderTarget();
			GetEtDevice()->RestoreDepthStencil();
			GetEtDevice()->EnableZ( bEnableZ );
			GetEtDevice()->EnableAlphaBlend( bAlphaEnable );
			m_pColorAdjFilter->SetSceneSaturation( fSat );
		}
		if( IsLocalPlayerGaugeFace() && m_hTempTextureForDeviceLost )
		{
			// 렌더할때마다 구웠더니 프레임 튀는 현상이 심해서 못하겠다.
			//m_hTempTextureForDeviceLost->Copy( m_hFaceTexture, D3DX_FILTER_NONE );
		}

		if( m_hFaceObject->IsAniObject() )
		{
			if( bBattleModeChanged )
			{
				CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

#ifdef PRE_ADD_37745
				pPlayerActor->SetBattleMode( true , true );
#else
				pPlayerActor->SetBattleMode( true );
#endif
			}

			( ( EtAniObjectHandle )m_hFaceObject )->ForceSkipSimulateAni( false );
			( ( EtAniObjectHandle )m_hFaceObject )->SetExtraAniFrame( -1, 0.0f );
			if( hParent && nLinkBoneIndex >= 0 )
			{
				m_hFaceObject->SetParent( hParent, nLinkBoneIndex );
			}
			if( bAlphaProcess )
			{
				m_hFaceObject->SetObjectAlpha( fObjectAlpha );
				if( !bShowObject ) m_hFaceObject->ShowObject( bShowObject );
			}
			m_hFaceObject->SetSaveMatIndex( -1 );
		}
		m_hFaceObject->Update( &SaveWorldMat );
		m_hFaceObject->InitRender();
	}
}

void CDnGaugeFace::BakePortrait()
{
	if( m_nFaceID == -1 )
	{
		return;
	}

	AddObject( m_hFaceObject, m_bDeleteObject );
	SetRenderFrameCount( 1 );
}

void CDnGaugeFace::UpdatePortrait( EtObjectHandle hHandle, bool bDelete, bool bBoss, int nAniIndex, float fFrame )
{
	m_bBoss = bBoss;
	m_bDeleteObject = bDelete;
	m_hFaceObject = hHandle;
	m_nAniIndex = nAniIndex;
	m_fFrame = fFrame;
	BakePortrait();
}

bool CDnGaugeFace::IsUpdatePortrait()
{
	return ( !!m_hFaceTexture );
}

void CDnGaugeFace::SetTexture( EtTextureHandle hTexture)
{
	SAFE_RELEASE_SPTR( m_hFaceTexture );
	m_hFaceTexture = hTexture;
}

void CDnGaugeFace::BackupPortrait()
{
	// 고스트 되는 타임에만 임시텍스처로 복사해두기로 한다.
	if( IsLocalPlayerGaugeFace() && m_hTempTextureForDeviceLost )
	{
		m_hTempTextureForDeviceLost->Copy( m_hFaceTexture, D3DX_FILTER_NONE );
	}
}

void CDnGaugeFace::OnLostDevice()
{
	if( IsLocalPlayerGaugeFace() && m_hFaceTexture )
	{
		m_bUseTempTextureForDeviceLost = true;
	}
}

void CDnGaugeFace::OnResetDevice()
{
	// 여기 OnReset 호출될 타이밍에서는 m_hFaceTexture의 OnReset이 아직 호출되지 않은 상태라,
	// 다른데서 처리해야한다.
}

bool CDnGaugeFace::IsLocalPlayerGaugeFace()
{
	if( m_nFaceID != -1 && m_FaceType == FT_PLAYER && m_bBoss == false )
		return true;
	return false;
}