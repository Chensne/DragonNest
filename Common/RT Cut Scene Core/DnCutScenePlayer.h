#pragma once
//#include "Timer.h"
#include "EtMatrixEx.h"
#include "EternityEngine.h"
#include "IDnCutSceneDataReader.h"
#include "ICustomActorProcessor.h"

//#ifdef PRE_ADD_FILTEREVENT
#include "DnCutSceneWorld.h"
//#endif // PRE_ADD_FILTEREVENT

class CEtWorld;
class CEtDOFFilter;


// 실제로 데이터를 받아서 플레이
class CDnCutScenePlayer
{
public:
	enum
	{
		ACTION_SEQUENCE,
		KEY_SEQUENCE,
		CAM_EVENT_SEQUENCE,
		PARTICLE_EVENT_SEQUENCE,
		DOF_EVENT_SEQUENCE,
		//SOUND_EVENT_SEQUENCE,
		FADE_EVENT_SEQUENCE,
		PROP_EVENT_SEQUENCE,
		SOUND_EVENT_1_SEQUENCE,
		SOUND_EVENT_2_SEQUENCE,
		SOUND_EVENT_3_SEQUENCE,
		SOUND_EVENT_4_SEQUENCE,
		SOUND_EVENT_5_SEQUENCE,
		SOUND_EVENT_6_SEQUENCE,
		SOUND_EVENT_7_SEQUENCE,
		SOUND_EVENT_8_SEQUENCE,
		SUBTITLE_EVENT_SEQUENCE,
// #ifdef PRE_ADD_FILTEREVENT
		COLORFILTER_EVENT_SEQUENCE,
//#endif // PRE_ADD_FILTEREVENT
		SEQUENCE_COUNT,
	};

	struct S_SEQUENCE
	{
		S_CS_ACTOR_INFO*		pAniObjectInfoToPlay;
		int iType;

		// ms 단위
		DWORD			dwStartTime;
		DWORD			dwTimeLength;

		S_SEQUENCE*		pPrevSequence;			// 같은 타입의 이전 시퀀스의 포인터를 갖고 있는다.
		S_SEQUENCE*		pNextSequence;			// 같은 타입의 다음 시퀀스의 포인터를 갖고 있는다.

		S_SEQUENCE( void )  : pAniObjectInfoToPlay( NULL ), iType( SEQUENCE_COUNT ),
							  dwStartTime( 0 ), dwTimeLength( 0 ), pPrevSequence( NULL ), pNextSequence( NULL )
							  {}
		virtual ~S_SEQUENCE( void )
		{}
	};

	struct S_ACTION_SEQUENCE : public S_SEQUENCE
	{
		ActionInfo*		pActionInfo;
		
		S_ACTION_SEQUENCE( void ) : pActionInfo( NULL )
		{
			iType = ACTION_SEQUENCE;
		}

		S_ACTION_SEQUENCE( const S_SEQUENCE* pObject ) : pActionInfo( NULL )
		{
			const S_ACTION_SEQUENCE* pRhs = static_cast<const S_ACTION_SEQUENCE*>(pObject);
			*this = *(pRhs);
			pActionInfo = new ActionInfo(*(pRhs->pActionInfo));
		}

		~S_ACTION_SEQUENCE( void )
		{
			SAFE_DELETE( pActionInfo );
		}
	};


	struct S_KEY_SEQUENCE : public S_SEQUENCE
	{
		const KeyInfo*			pKeyInfo;

		S_KEY_SEQUENCE( void ) : pKeyInfo( NULL ) 
		{
			iType = KEY_SEQUENCE;
		};

		S_KEY_SEQUENCE( const S_SEQUENCE* pObject ) : pKeyInfo( NULL )
		{
			const S_KEY_SEQUENCE* pRhs = static_cast<const S_KEY_SEQUENCE*>(pObject);
			*this = *(pRhs);
			pKeyInfo = new KeyInfo( *(pRhs->pKeyInfo) );
		}

		~S_KEY_SEQUENCE( void )
		{
			SAFE_DELETE( pKeyInfo );
		}
	};


	// 카메라 이벤트 시퀀스
	struct S_CAM_EVENT_SEQUENCE : public S_SEQUENCE
	{
		CamEventInfo* pCamEventInfo;
		float fFOVKeyFrame;
		float fPosKeyFrame;
		float fLookAtKeyFrame;
		float fRotKeyFrame;

		S_CAM_EVENT_SEQUENCE( void ) : pCamEventInfo( NULL ), fPosKeyFrame( 0.0f ), fLookAtKeyFrame( 0.0f ), fRotKeyFrame( 0.0f ), fFOVKeyFrame( 0.0f )
		{
			iType = CAM_EVENT_SEQUENCE;
		};

		S_CAM_EVENT_SEQUENCE( const S_SEQUENCE* pObject ) : pCamEventInfo( NULL ), fPosKeyFrame( 0.0f ), fLookAtKeyFrame( 0.0f ), fRotKeyFrame( 0.0f )
		{
			const S_CAM_EVENT_SEQUENCE* pRhs = static_cast<const S_CAM_EVENT_SEQUENCE*>(pObject);
			*this = *(pRhs);
			pCamEventInfo = static_cast<CamEventInfo*>(pRhs->pCamEventInfo->clone());
		}

		~S_CAM_EVENT_SEQUENCE( void ) 
		{ 
			SAFE_DELETE( pCamEventInfo );
		}
	};


	// 파티클 이벤트 시퀀스
	struct S_PARTICLE_EVENT_SEQUENCE : public S_SEQUENCE
	{
		ParticleEventInfo* pParticleEventInfo;
		EtBillboardEffectHandle hParticle;
		MatrixEx crossWorld;
		
		S_PARTICLE_EVENT_SEQUENCE( void ) : pParticleEventInfo( NULL ) 
		{
			iType = PARTICLE_EVENT_SEQUENCE;
		}

		~S_PARTICLE_EVENT_SEQUENCE( void ) 
		{ 
			SAFE_DELETE( pParticleEventInfo );
			SAFE_RELEASE_SPTR( hParticle );
		};
	};


	// 사운드 이벤트 시퀀스
	struct S_SOUND_EVENT_SEQUENCE : public S_SEQUENCE
	{
		SoundEventInfo* pSoundEventInfo;
		EtSoundChannelHandle hSound;
	
		S_SOUND_EVENT_SEQUENCE( void ) : pSoundEventInfo( NULL )
		{
			iType = SOUND_EVENT_1_SEQUENCE;		// Note: 기본이 1인거고 생성 후에 제대로 셋팅해 줘야 함
		}

		~S_SOUND_EVENT_SEQUENCE( void )
		{
			SAFE_DELETE( pSoundEventInfo );
			//SAFE_RELEASE_SPTR( hSound );
		}
	};

	// DOF 이벤트 시퀀스
	struct S_DOF_EVENT_SEQUENCE : public S_SEQUENCE
	{
		DofEventInfo* pDofEventInfo;

		S_DOF_EVENT_SEQUENCE( void ) : pDofEventInfo( NULL )
		{
			iType = DOF_EVENT_SEQUENCE;
		}

		~S_DOF_EVENT_SEQUENCE( void )
		{
			SAFE_DELETE( pDofEventInfo );
		}
	};

	// 페이드 인 아웃 이벤트 시퀀스
	struct S_FADE_EVENT_SEQUENCE : public S_SEQUENCE
	{
		FadeEventInfo* pFadeEventInfo;

		S_FADE_EVENT_SEQUENCE( void ) : pFadeEventInfo( NULL )
		{
			iType = FADE_EVENT_SEQUENCE;
		}

		~S_FADE_EVENT_SEQUENCE( void )
		{
			SAFE_DELETE( pFadeEventInfo );
		}
	};

	// 프랍 액션 이벤트 시퀀스
	struct S_PROP_EVENT_SEQUENCE : public S_SEQUENCE
	{
		PropEventInfo* pPropEventInfo;
		
		S_PROP_EVENT_SEQUENCE( void ) : pPropEventInfo( NULL )
		{
			iType = PROP_EVENT_SEQUENCE;
		}

		~S_PROP_EVENT_SEQUENCE( void )
		{
			SAFE_DELETE( pPropEventInfo );
		}
	};

	// 자막 이벤트 시퀀스
	struct S_SUBTITLE_EVENT_SEQUENCE : public S_SEQUENCE
	{
		SubtitleEventInfo* pSubtitleEventInfo;

		S_SUBTITLE_EVENT_SEQUENCE( void ) : pSubtitleEventInfo( NULL )
		{
			iType = SUBTITLE_EVENT_SEQUENCE;
		}

		~S_SUBTITLE_EVENT_SEQUENCE( void )
		{
			SAFE_DELETE( pSubtitleEventInfo );
		}
	};

// #ifdef PRE_ADD_FILTEREVENT
	// 컬러필터 이벤트 시퀀스
	struct S_COLORFILTER_EVENT_SEQUENCE : public S_SEQUENCE
	{
		ColorFilterEventInfo* pEventInfo;

		S_COLORFILTER_EVENT_SEQUENCE( void ) : pEventInfo( NULL )
		{
			iType = COLORFILTER_EVENT_SEQUENCE;
		}

		~S_COLORFILTER_EVENT_SEQUENCE( void )
		{
			SAFE_DELETE( pEventInfo );
		}
	};
// #endif PRE_ADD_FILTEREVENT

private:
	IDnCutSceneDataReader*			m_pCutSceneData;

	// 현재 무대인 월드 객체
	CEtWorld*						m_pWorld;

	EtCameraHandle					m_hCamera;
	CEtDOFFilter*					m_pDOFFilter;
	S_SUBTITLE_EVENT_SEQUENCE*		m_pLiveSubtitleSequence;

	// 현재 화면에 떠 있는 파티클
	vector<EtBillboardEffectHandle>		m_vlhParticles;

	// 외부에서 셋팅해주는 커스텀 액터 프로세서
	ICustomActorProcessor*			m_pActorProcessor;

	vector<S_SEQUENCE*>				m_vlpSequenceDB;			// 반드시 시간 순으로 정렬되어 있어야 함
	
	deque<S_SEQUENCE*>				m_dLiveSequences;

	LOCAL_TIME						m_StartTime;
	LOCAL_TIME						m_PrevTime;

	int								m_iNowLiveSequence;

	S_ACTION_SEQUENCE*				m_pLastProcessedActionSeq;
	
	LOCAL_TIME						m_WholeSceneLength;		// 전체 신 길이.
	bool							m_bSceneEnd;
	
	EtMatrix						m_matOriProj;

	set<S_SEQUENCE*>				m_setProcessedTrigger;

	set<string>						m_setHideActor;

	// 각 액터별로 맨 처음 액션을 갖고 있다가 컷신이 시작되면 0 프레임으로 초기화 시켜준다.
	// 벡터의 인덱스는 각 액터의 인덱스와 같다.
	vector<S_ACTION_SEQUENCE*>		m_vlpFirstActionSequence;


	//struct S_SOUND_CHANNEL_INFO
	//{
	//	EtSoundChannelHandle hChannel;
	//	LOCAL_TIME StartTime;
	//};
	vector<EtSoundChannelHandle>	m_vlhPlayingChannel;

private:
	int _SearchFOVOffsetIndex( vector<SCameraFOVKey>& vlCamFOV, int iFrame );
	int _SearchPosOffsetIndex( vector<SCameraPositionKey>& vlCamPosition, int iFrame );
	int _SearchRotOffsetIndex( vector<SCameraRotationKey>& vlCamRotation, int iFrame );
	//int _SearchLaySequence( LOCAL_TIME ElapsedTime, int iType );
	void _CalcRotateFront( S_CS_ACTOR_INFO* pActorInfo, EtVector3& vDir );

	void _BeginParticleEvent( S_SEQUENCE* pParticleEvent );
	void _BeginSoundEvent( S_SEQUENCE* pSoundEvent );
	void _BeginFadeEvent( S_SEQUENCE* pFadeEvent );
	void _ProcessPropActionEvent( S_SEQUENCE* pPropActionEvent, LOCAL_TIME AbsoluteTime, LOCAL_TIME ElapsedTime );
	void _BeginSubtitleEvent( S_SEQUENCE* pEvent );

	//void _FigureOutEndSeuqnces( LOCAL_TIME ElapsedTime );
	//void _FigureOutLiveSequences( LOCAL_TIME ElapsedTime );
	void _UpdateSequence( LOCAL_TIME AbsoluteTime, LOCAL_TIME ElapsedTime );
	//void _ProcessSequences( LOCAL_TIME ElapsedTime );
	void _ProcessActors( LOCAL_TIME AbsoluteTime, LOCAL_TIME ElapsedTime );
	void _ProcessSubtitle( LOCAL_TIME ElapsedTime );


public:
	CDnCutScenePlayer(void);
	~CDnCutScenePlayer(void);

	void Initialize( IDnCutSceneDataReader* pCutSceneData );
	void Clear( void );

	void MakeUpSequences( ICustomActorProcessor* pCustomActorProcessor );
	void StartPlay( LOCAL_TIME StartTime );
	void StopPlay();
	//void AddPlaySequence( S_SEQUENCE* pNewSequence );
	void SetWorld( CEtWorld* pWorld ) { m_pWorld = pWorld; };
	void SetCamera( EtCameraHandle hCamera ) { m_hCamera = hCamera; };
	EtCameraHandle GetCamera( void ) { return m_hCamera; };
	void SetDOFFilter( CEtDOFFilter* pFilter ) { m_pDOFFilter = pFilter; };
	void ResetLiveSequences( void );

	//const MatrixEx& GetActorEtMatrixEx( const char* pActorName );
	
	void Process( LOCAL_TIME AbsoluteTime );

	void ResetPlayingSoundChannels( void );
	void FadeInPlayingSoundChannels( void );
	void FadeOutPlayingSoundChannels( void );

	bool IsEndScene( void ) { return (NULL == m_pActorProcessor) || (m_bSceneEnd); };

	// 초기 셋팅부터 컷신 파일엔 있으나 나오지 말아야 하는 액터들
	void HideActor( const char* pActorName ) 
	{ 
		m_setHideActor.insert( string(pActorName) );
	};
	bool IsHideActor( const char* pActorName ) { return (m_setHideActor.find( string(pActorName) ) != m_setHideActor.end()); }
};
