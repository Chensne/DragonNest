#include "StdAfx.h"
#include "EtOptionController.h"
#include "EtShadowMap.h"
#include "EtLiSPShadowMap.h"
#include "EtTerrainArea.h"
#include "EtWater.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtOptionController g_OptionController;

CEtOptionController::CEtOptionController(void)
{
}

CEtOptionController::~CEtOptionController(void)
{
}

void CEtOptionController::Initialize( SGraphicOption &Option )
{
	m_Option = Option;
	ValidateOption();
	GenerateOptionMacro();
	CheckShadowMap();
	CreateSimpleWaterMap(0);
}

void CEtOptionController::ValidateOption()
{
	m_Option.nMaxDirLightCount = min( MAX_DIRECTIONAL_LIGHT_COUNT, m_Option.nMaxDirLightCount );
	m_Option.nMaxPointLightCount = min( MAX_POINT_LIGHT_COUNT, m_Option.nMaxPointLightCount );
	m_Option.nMaxSpotLightcount = min( MAX_SPOT_LIGHT_COUNT, m_Option.nMaxSpotLightcount );

	if( GetEtDevice()->GetSimultaneousRTs() <= 1 )
	{
		m_Option.bEnableDOF = false;
		m_Option.bEnableMotionBlur = false;
	}
	else if( ( m_Option.bEnableDOF ) && ( ( m_Option.bEnableMotionBlur ) ) )
	{
		if( GetEtDevice()->GetSimultaneousRTs() <= 2 )
		{
			m_Option.bEnableMotionBlur = false;
		}
	}

	// 간단한 쉐이더 밖에 못 돌리는 그래픽 카드인가.
	m_Option.bIsOnlyLowShaderAvailable = GetEtDevice()->IsOnlyLowShaderAvailable();
}

void CEtOptionController::GenerateOptionMacro()
{
	GetEtDevice()->ClearEffectMacro();
	GetEtDevice()->AddEffectMacro( "ETERNITY_ENGINE", "Eternity" );

	if( m_Option.bUseTerrainLightMap )
	{
		GetEtDevice()->AddEffectMacro( "USE_TERRAIN_LIGHTMAP", "1" );
	}

	if( m_Option.DynamicShadowType == ST_SHADOWMAP )
	{
		GetEtDevice()->AddEffectMacro( "SIMPLE_SHADOWMAP", "1" );
	}
	else if( m_Option.DynamicShadowType == ST_DEPTHSHADOWMAP )
	{
		GetEtDevice()->AddEffectMacro( "DEPTH_SHADOWMAP", "1" );
	}

/*	if( m_Option.bEnableDOF ) // MRT 안되는 카드 있어서 끈다.
	{
		GetEtDevice()->AddEffectMacro( "BAKE_DEPTHMAP", "1" );
	}
	if( m_Option.bEnableMotionBlur )
	{
		GetEtDevice()->AddEffectMacro( "BAKE_VELOCITY", "1" );
	}*/

	char szString[ 1024 ];
	if( m_Option.nMaxDirLightCount <= 0 )
	{
		GetEtDevice()->AddEffectMacro( "DISABLE_DIR_LIGHT", "1" );
	}
	else
	{
		GetEtDevice()->AddEffectMacro( "DIR_LIGHT_COUNT", itoa( m_Option.nMaxDirLightCount, szString, 10 ) );
	}
	if( m_Option.nMaxPointLightCount <= 0 )
	{
		GetEtDevice()->AddEffectMacro( "DISABLE_POINT_LIGHT", "1" );
	}
	else
	{
		GetEtDevice()->AddEffectMacro( "POINT_LIGHT_COUNT", itoa( m_Option.nMaxPointLightCount, szString, 10 ) );
	}
	if( m_Option.nMaxSpotLightcount <= 0 )
	{
		GetEtDevice()->AddEffectMacro( "DISABLE_SPOT_LIGHT", "1" );
	}
	else
	{
		GetEtDevice()->AddEffectMacro( "SPOT_LIGHT_COUNT", itoa( m_Option.nMaxSpotLightcount, szString, 10 ) );
	}
	if( m_Option.bSoftShadow ) 
	{
		GetEtDevice()->AddEffectMacro( "SOFT_SHADOW", "1" );
	}
}

void CEtOptionController::SetGraphicOption( SGraphicOption &Option )
{
	SGraphicOption OldOption;
	ReloadType Type;

	Type = RELOAD_NONE;
	OldOption = m_Option;
	m_Option = Option;
	GenerateOptionMacro();
	if( OldOption.bUseTerrainLightMap != m_Option.bUseTerrainLightMap )
	{
		Type = RELOAD_TERRAIN_MATERIAL; 
	}
	if( OldOption.bEnableDOF != m_Option.bEnableDOF )
	{
		Type = RELOAD_ALL_MATERIAL; 
	}
	if( OldOption.bEnableMotionBlur != m_Option.bEnableMotionBlur )
	{
		Type = RELOAD_ALL_MATERIAL; 
	}
	if( OldOption.DynamicShadowType != m_Option.DynamicShadowType ) 
	{
		Type = RELOAD_ALL_MATERIAL; 
		CheckShadowMap();
	}
	else if( ( m_Option.DynamicShadowType != ST_NONE ) && ( OldOption.ShadowQuality != m_Option.ShadowQuality ) )
	{
		Type = RELOAD_ALL_MATERIAL; 
		GetEtShadowMap()->Initialize( m_Option.ShadowQuality, m_Option.DynamicShadowType );
	}
	if( Type == RELOAD_ALL_MATERIAL )
	{
		CEtMaterial::ReloadMaterial();
	}
	else if( Type == RELOAD_TERRAIN_MATERIAL )
	{
		CEtTerrainArea::ReloadMaterial();
	}
}

void CEtOptionController::CheckShadowMap()
{
	if( m_Option.DynamicShadowType == ST_NONE )
	{
		DeleteShadowMap();
	}
	else
	{
		CreateSimpleSahdowMap( m_Option.ShadowQuality, m_Option.DynamicShadowType );
	}
}
