#pragma once
#include "DnCutSceneActionBase.h"
#include "IDnCutSceneRenderObject.h"
#include "EtMatrixEx.h"
#include "EtSoundEngine.h"
#include "DnCutSceneWeapon.h"

// 동영상 툴에서 사용하는 액터들의 표현
class CDnCutSceneActor : public CDnCutSceneActionBase,
						 public IDnCutSceneRenderObject
{
public:
	struct S_PARTICLE_INFO
	{
		int iActionIndex;			// 이 파티클이 생성된 액션
		int iSignalIndex;			// 이 파티클이 생성된 시그널 인덱스
		EtBillboardEffectHandle hParticle;
		const ParticleStruct* pParticleStruct;

		S_PARTICLE_INFO( void ) : pParticleStruct( NULL ), iActionIndex(0), iSignalIndex(0)
		{};
	};

	struct FindParticleInfo : public unary_function<const S_PARTICLE_INFO&, bool>
	{
		int m_iIndexToFind;

		FindParticleInfo( int iIndex ) : m_iIndexToFind( iIndex ) {}

		bool operator () ( const S_PARTICLE_INFO& Info )
		{
			return (Info.pParticleStruct->nParticleDataIndex == m_iIndexToFind);
		}
	};

	struct S_EFFECT_OBJECT_INFO
	{
		EtEffectObjectHandle hEffectObject;
		const FXStruct* pFXStruct;

		S_EFFECT_OBJECT_INFO( void ) : pFXStruct( NULL )
		{};
	};

	enum
	{
		PLAYER,
		MONSTER,
		NPC,
	};

	enum
	{
		WARRIOR = 0,
		ARCHER,
		SOCERESS,
		CLERIC,
		ACADEMIC,
		Kali, // bintitle.

		PLAYER_MAX
	};

	enum
	{
		CHARACTER_TYPE_BOUND = 9,			// 클라의 CDnActorState::ActorTypeEnum 참조.
	};

private:
	string					m_strSkinName;
	string					m_strAniName;
	tstring					m_strActorName;

	EtAniObjectHandle		m_hObject;

	MatrixEx			m_matExWorld;
	float					m_fRotationY;
	EtVector3				m_vStaticPos;
	EtVector3				m_vScale;

	// 시그널 종류대로 리스트 생성
	vector<S_PARTICLE_INFO> m_vlParticles;
	vector<S_EFFECT_OBJECT_INFO> m_vlEffectObjects;
	//vector<EtEffectObjectHandle> m_vlhEffectObjects;
	//vector<EtcObjectSignalStruct

	float					m_fFrame;

	EtSoundChannelHandle	m_hCurChannel;
	bool					m_bUseSignalSound;

	int						m_iActorType;

	// 무기
	std::shared_ptr<CDnCutSceneWeapon> m_apWeapon[ 2 ];
	int m_apWeaponCnt; // bintitle.

	// Face Ani 부분. ////////////////////////////////////////////////////////////////////////
	struct FrameStruct 
	{
		float fFrame;
		EtTextureHandle hTexture;
	};

	struct SocialStruct 
	{
		std::string szName;
		float fTotalFrame;
		std::vector<FrameStruct *> pVecList;
	};

	std::vector<SocialStruct*> m_pVecSocialList;
	std::map<std::string, int> m_szMapSearch;

	LOCAL_TIME m_LocalTime;
	std::string m_szSocialAction;
	int m_nCurrentSocialActionIndex;
	LOCAL_TIME m_AniFrame;

	float m_fFps;
	///////////////////////////////////////////////////////////////////////////////////////////


public:
	CDnCutSceneActor(void);
	virtual ~CDnCutSceneActor(void);

	bool LoadSkin( const char* pSkinName, const char* pAniName );

	void ChangeAnimation( int iAniIndex, float fFrame, float fBlendFrame = 0.0f );
	
	// from IDnCutSceneRenderObject
	int GetAniIndex( const char* pAniName );
	int GetCachedAniIndex( int iIndex ) { return m_vlAniIndexList.at(iIndex); };

	void SetName( const wchar_t* pActorName ) { m_strActorName.assign( pActorName ); };
	const wchar_t* GetName( void ) { return m_strActorName.c_str(); };

	void SetPosition( const EtVector3& vPos );
	const EtVector3* GetPosition( void ) const { return &(m_matExWorld.m_vPosition); };

	// TODO: 이 함수는 제대로 되는지 확인을 해봐야 한다.
	void SetRotationY( float fRot );
	float GetRotationY( void ) { return m_fRotationY; };

	void SetStaticPosition( EtVector3& vPos ) { m_vStaticPos = vPos; };
	EtVector3* GetStaticPosition( void ) { return &m_vStaticPos; };
	void Show( bool bShow ) { m_hObject->ShowObject( bShow ); };

	EtAniObjectHandle GetAniObjectHandle( void ) { return m_hObject; };
	void GetAniIndexList( /*IN OUT*/ vector<int>& vlAniIndex );

	void Process( LOCAL_TIME LocalTime, float fDelta );

	void OnSignal( int iSignalType, int iSignalArrayIndex, void* pSignalData, LOCAL_TIME LocalTime, LOCAL_TIME StartTime, LOCAL_TIME EndTime );
	void UpdateSignal( LOCAL_TIME LocalTime, float fDelta );

	const MatrixEx* GetMatrixEx( void ) { return &m_matExWorld; };
	void SetMatrixEx( const MatrixEx& matExWorld ) { m_matExWorld = matExWorld; };

	void SetUseSignalSound( bool bUseSignalSound ) { m_bUseSignalSound = bUseSignalSound; };

	void GetAniBoneMatrix( const char *szBoneName, EtMatrix& matBone );
	void OnEndCutScene( void );

	void SetFaceAction( const char* pFaceActionName );

	void SetScale( float fScale );
	float GetScale( void ) { return m_vScale.x; };

	void SetActorType( int iActorType ) { m_iActorType = iActorType; }
	int GetActorType( void ) { return m_iActorType; };

	void AttachWeapon(std::shared_ptr<CDnCutSceneWeapon> pWeapon, int iIndex );
	void LinkWeapon( int iEquipIndex );
	void ShowWeapon( bool bShow ); // bintitle.
	
	EtAniObjectHandle GetObjectHandle( void ) { return m_hObject; };
	int GetBoneIndex( const char* pBoneName );
};
