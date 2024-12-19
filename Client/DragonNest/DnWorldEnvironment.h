#pragma once

#include "DnCamera.h"
#include "EtRainEffect.h"
#include "EtLensFlare.h"
#include "EtRainDropSplash.h"
#include "EtVolumeFog.h"
#include "EtSnowEffect.h"

#include "envinfo.h"

#define DEFAULT_LIGHTMAP_SIZE		128

class CDnWorldEnvironment {
public:
	CDnWorldEnvironment();
	virtual ~CDnWorldEnvironment();

	enum ColorFilterParamEnum {
		Shadow,
		Midtone,
		Highlight,
		Saturation,

		ColorFilterParamEnum_Amount,
	};

protected:
	bool m_bActivate;

	CEnvInfo m_EnvInfo;

	EtTextureHandle m_hEnviTexture;

	EtObjectHandle m_hSkybox;
	std::vector<EtLightHandle> m_hVecLight;
	
	std::vector<EtLensFlareHandle> m_hVecLensFlare;
	std::vector<EtVolumeFogHandle> m_hVecVolumeFog;
	
	
	//blondy 파일에서 읽어낸 원본을 덮어써서 부득이 하게 백업한다
	float m_fColorAdjSaturationBackUp;
	//blondy 

	int m_nDOFCameraEffectIndex;

	// Outline Filter
	CEtOutlineFilter	*m_pOutlineFilter;
	CEtTransitionFilter *m_pTransitionFilter;

	EtColor m_PrevAmbient;
	SCameraInfo m_PrevCameraInfo;

	CEtColorAdjustTexFilter *m_pColorAdjFilter;
	CEtBloomFilter *m_pBloomFilter;
	CEtHeatHazeFilter *m_pHeatHazeFilter;
	EtRainHandle m_hRain;
	EtRainDropSplashHandle m_hSplashDrop;
	EtSnowHandle m_hSnowEffect;

	int m_nEnviBGMIndex;
	EtSoundChannelHandle m_hEnviBGMChannel;
	DnCameraHandle m_hEnviCamera;

	// Control 용 변수
	bool m_bControlColorFilter[ColorFilterParamEnum_Amount];
	float m_fControlColorFilterDurationDelta[ColorFilterParamEnum_Amount];
	float m_fControlColorFilterDuration[ColorFilterParamEnum_Amount];

	EtVector4 m_vControlColorAdjShadow;
	EtVector4 m_vControlColorAdjMidtone;
	EtVector4 m_vControlColorAdjHighlight;
	float m_fControlColorAdjSaturation;

	DnEtcHandle m_hEnviEffect;

public:
	bool LoadEnvironment( const char *szEnviFileName );
	bool Initialize( DnCameraHandle hCamera, int nCameraFar = -1, float fWaterFarRatio = 0.5f );
	void Finalize();
	void Process( LOCAL_TIME LocalTime, float fDelta );

	int GetLightmapBlurSize() { return m_EnvInfo.GetLightmapBlurSize(); }

	const char *GetEnviBGMName() { return m_EnvInfo.GetEnviBGM(); }
	float GetEnviBGMRatio() { return m_EnvInfo.GetEnviBGMRatio(); }
	void SetEnviBGMRatio( float fRatio ) { m_EnvInfo.SetEnviBGMRatio(fRatio); }
	void SetEnviBGMVolume( float fValue );

	EtVector4 ControlColorFilter( ColorFilterParamEnum Type, EtVector4 vValue, int nDuration );
	float ControlColorFilter( ColorFilterParamEnum Type, float fValue, int nDuration );
	float GetColorSaturation() const { return m_EnvInfo.GetColorAdjSaturation(); }
	EtVector4* GetColorHighLight() { return m_EnvInfo.GetColorAdjHighlights(); }
	const EtVector4* GetColorHighLight() const { return m_EnvInfo.GetColorAdjHighlights(); }

	int GetDOFEffectIndex( void ) { return m_nDOFCameraEffectIndex; };

	CEtOutlineFilter	*GetOutlineFilter() {return m_pOutlineFilter; }
	CEtTransitionFilter *GetTransitionFilter() {return m_pTransitionFilter; }

	//blondy
	void ResetSaturation();
	//blondy end

	void ApplyColorFilter( EtTextureHandle hDestTarget, EtTextureHandle hSrcTarget );	


	// PRE_ADD_FILTEREVENT
	void SetSceneAbsoluteColor( bool bMonochrome, EtVector3 & vColor, float fVolume );
	//


};
