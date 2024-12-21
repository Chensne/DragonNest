#pragma once

#include "EtWorld.h"
#include "EtRainEffect.h"
#include "EtLensFlare.h"
#include "EtRainDropSplash.h"
#include "EtSnowEffect.h"
#include "IResourcePathFinder.h"
#include "envinfo.h"

class CEtWorldGrid;

// EtWorld 를 상속받아 배경을 렌더링
class CDnCutSceneWorld : public CEtWorld
{
private:
	string				m_strResourceTilePath;
	string				m_strResourcePropPath;
	string				m_strResourceEnviPath;

	

	string				m_strEnvironmentName;

	CEtColorAdjustTexFilter* m_pColorAdjFilter;
	CEtBloomFilter*		m_pBloomFilter;

	EtColor				m_PrevAmbient;
	EtTextureHandle		m_hEnviTexture;

	EtRainHandle		m_hRain;

	EtRainDropSplashHandle m_hSplashDrop;

	CEtHeatHazeFilter*	m_pHeatHazeFilter;

	EtSnowHandle m_hSnow;

	vector<EtLightHandle> m_vlLight;
	vector<EtLensFlareHandle> m_vlLensFlare;

	EtObjectHandle		m_hSkybox;

	IResourcePathFinder* m_pResPathFinder;

	CEnvInfo m_EnvInfo;

//#ifdef PRE_ADD_FILTEREVENT
	bool m_bMonochrome;
	EtVector3 m_vFilterEventColor;
	float m_fFilterEventVolume;
//#endif PRE_ADD_FILTEREVENT

private:
	bool _LoadEnvironmentFile( const char* pFileName );

protected:
	CEtWorldGrid* AllocGrid( void );

	void _FinalizeEnvironment( void );

public:
	CDnCutSceneWorld(void);
	virtual ~CDnCutSceneWorld(void);

	// from CEtWorld
	bool Initialize( const char *szWorldFolder, const char *szGridName, bool bForceInitializeSector = false );
	void Destroy( void );

	bool InitializeEnvironment( const char* pFileName, const char* pResourceFolder );

	void SetResPathFinder( IResourcePathFinder* pResPathFinder ) { m_pResPathFinder = pResPathFinder; };
	IResourcePathFinder* GetResPathFinder( void ) { return m_pResPathFinder; };

	float GetFogNear( void ) { return m_EnvInfo.GetFogNear(); };
	float GetFogFar( void ) { return m_EnvInfo.GetFogFar(); };
	DWORD GetFogColor( void ) { return m_EnvInfo.GetFogColor(); };

	bool IsEnableDOF() const { return m_EnvInfo.IsEnableDOF(); }
	float GetDOFFocusDistance() const { return m_EnvInfo.GetDOFFocusDistance(); }
	float GetDOFNearStart() const { return m_EnvInfo.GetDOFNearStart(); }
	float GetDOFNearEnd() const { return m_EnvInfo.GetDOFNearEnd(); }
	float GetDOFFarStart() const { return m_EnvInfo.GetDOFFarStart(); }
	float GetDOFFarEnd() const { return m_EnvInfo.GetDOFFarEnd(); }
	float GetDOFNearBlur() const { return m_EnvInfo.GetDOFNearBlur(); }
	float GetDOFFarBlur() const { return m_EnvInfo.GetDOFFarBlur(); }

	//#ifdef PRE_ADD_FILTEREVENT
	void SetSceneAbsoluteColor( bool bMonochrome, EtVector3 & vColor, float fVolume );
	//#endif PRE_ADD_FILTEREVENT

};
