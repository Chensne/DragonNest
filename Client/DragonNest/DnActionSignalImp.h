#pragma once

#include "TSmartPtrSignalImp.h"
#include "DnActionBase.h"
#include "EtMatrixEx.h"
#include "DnEyeLightTrailObject.h"

struct EtcObjectSignalStruct {
	bool bLinkObject;
	bool bDefendenceParent;
	EtVector3 vOffset;
	EtVector3 vRotate;
	char szBoneName[ 255 ];
	bool bClingTerrain;

	EtcObjectSignalStruct() : bLinkObject( false ), bDefendenceParent( false ), bClingTerrain( false )
	{
		vOffset = EtVector3( 0.0f, 0.0f, 0.0f );
		vRotate = EtVector3( 0.0f, 0.0f, 0.0f );
		ZeroMemory( szBoneName, sizeof(szBoneName) );
	}
};

struct LinkParticleSignalStruct {
	EtVector3 vOffset;
	EtVector3 vRotate;
	bool bClingTerrain;
	bool bDefendenceParent;
	char szBoneName[255];
	bool bDeleteByChangeAction;
};

struct NormalParticleSignalStruct {
	bool bDefendenceParent;
	bool bDeleteByChangeAction;
};

struct LinkFXSignalStruct {
	EtVector3 vOffset;
	EtVector3 vRotate;
	bool bClingTerrain;
	bool bUseTraceHitProjectileActorAdditionalHeight;
	char szBoneName[255];
};

struct ShaderCustomParamValueSignalStruct {
	int nCustomParamIndex;
	int nParamType;
	int nSubMeshIndex;
	LOCAL_TIME EndTime;
	bool bApply;
	bool bContinueToNextAction;
};

struct LinkDecalSignalStruct {
	EtVector3 vOffset;
	float fRadius;
	float fRotate;
};

struct SoundSignalStruct {
	bool bChangeActionFadeVolume;
	bool bBeginFade;
	float fFadeDelta;
	float fDelta;
	float fVolume;
	bool bLoop;
#ifdef PRE_MOD_RELEASE_SOUND_SIGNAL
	CEtActionSignal *pSignal;
#endif
};

struct OtherSelfEffectSignalStruct {
	float fLifeTime;
	float fCurTime;
	EtVector4 vBeginColor;
	EtVector4 vEndColor;
	float fBeginAlpha;
	float fEndAlpha;
};

struct EffectTrailSignalStruct{
	bool bLoop;
	char szBoneName[255];
	EtVector3 *vOffset;
};

class CEtRTTRenderer;
class MAActorRenderBase;
class CDnActionSignalImp :
	virtual public CDnActionBase,
	public TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>,
	public TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>,
	public TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>,
	public TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>,
	public TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>,
	public TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>,
	public TSmartPtrSignalImp<EtDecalHandle, LinkDecalSignalStruct>,
	public TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>,
	public TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>,
	public TSmartPtrSignalImp<DnEyeLightTrailHandle, TSmartPtrSignalStruct::Default>,
	public TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>,
	public TSmartPtrSignalImp<EtAniObjectHandle, OtherSelfEffectSignalStruct>,
	public TSmartPtrSignalImp<EtEffectTrailHandle , EffectTrailSignalStruct>
{
public:
	CDnActionSignalImp();
	virtual ~CDnActionSignalImp();

	enum
	{
		EtcObject = 0x0001,
		LinkParticle = 0x0002,
		ParticleSmartPtrSignal = 0x0004,
		LinkFX = 0x0010,
		EffectSmartPtr = 0x0020,
		ShaderCustomParamValue = 0x0040,
		DecalSignal = 0x0080,
		OutlineSignal = 0x0100,
		SoundChannelSignal = 0x02000,
		EyeLightTrailSignal = 0x04000,
		EffectTrailSignal = 0x08000,

		All = EtcObject|LinkParticle|ParticleSmartPtrSignal|LinkFX|EffectSmartPtr|ShaderCustomParamValue|DecalSignal|OutlineSignal|SoundChannelSignal|EyeLightTrailSignal|EffectTrailSignal,
	};

protected:

protected:
//	std::vector<EtSoundChannelHandle> m_hVecCurChannel;
	EtSoundChannelHandle m_hCurVoiceChannel;
	SoundSignalStruct m_CurVoiceChannelData;
	bool m_bPlaySignalSound;		// 컷신 재생 중일 때는 시그널에 걸린 사운드를 출력을 CutSceneTask 쪽에서 제어할 수 있다.
	
	EtVector3 m_vActionSignalScale;

	bool m_bRTTRenderMode;
	CEtRTTRenderer *m_pRTTRenderer;

#ifndef _FINAL_BUILD
	struct DrawHitSignalStruct {
		int nIndex;
		float fDelta;
		EtVector3 vOffset;
		std::vector<EtVector3> vVecList[5];
	};
	std::vector<DrawHitSignalStruct *> m_pVecDrawHitSignalList;
	std::map<int, bool> m_nMapHitSignal;
#endif

protected:
	bool IsEyeLightTrailRange( int nHashCode );
	bool IsEffectTrailRange( int nHashCode );
	virtual EtLightHandle CreateLight( SLightInfo *pInfo );

public:
	virtual MatrixEx *GetObjectCross() { return NULL; }
	virtual CDnRenderBase *GetRenderBase() {return NULL;}
	virtual EtAniObjectHandle GetAniObjectHandle() {return CEtAniObject::Identity(); }
	EtMatrix GetAniBoneMatrix( const char *szBoneName );
	EtMatrix GetDummyBoneMatrix( const char *szBoneName );
	virtual float GetTraceHitProjectileActorAdditionalHeight( void ) { return 0.0f; };
 
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime,int nSignalIndex );

	void Process( LOCAL_TIME LocalTime, float fDelta );

	void SetPlaySignalSound( bool bPlaySignalSound ) { m_bPlaySignalSound = bPlaySignalSound; };

	void ReleaseSignalImp( DWORD dwMask = All, bool bSoftDestruct = true );

	void SetActionSignalScale( EtVector3 &vScale ) { m_vActionSignalScale = vScale; }
	EtVector3 *GetActionSignalScale() { return &m_vActionSignalScale; }

	virtual void OnChangeAction( const char *szPrevAction );

	void SetRTTRenderMode( bool bRTTRender, CEtRTTRenderer *pRTTRenderer = NULL );
	EtBillboardEffectHandle CreateBillboardEffect( int nIndex, MatrixEx &Cross );
	EtEffectObjectHandle CreateEffectObject( int nIndex, MatrixEx &Cross );
	EtAniObjectHandle CreateAniObject( const char *pSkinName, const char *pAniName );

#ifndef _FINAL_BUILD
	void DrawHitSignal( HitStruct *pHit, int nIndex, float fDelta = 1.f );
	void ProcessDrawHitSignsl( LOCAL_TIME LocalTime, float fDelta );
#endif

	void ApplyAlphaToSignalImps( float fAlpha );
};
