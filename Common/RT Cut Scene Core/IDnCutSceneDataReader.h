#pragma once

#include "EternityEngine.h"
#include "EtActionBase.h"
#include "CameraData.h"


typedef CEtActionBase::ActionElementStruct ActionEleInfo;

// 컷신 데이터를 읽을 수 있는 인터페이스
class IDnCutSceneDataReader
{
public:
	// 전체 신 정보
	struct S_SCENE_INFO
	{
		float fSceneStartFadeOutTime;
		float fSceneEndFadeInTime;

		bool bUseSceneEndClippingTime;
		float fSceneEndClipTime;

		bool bUseSignalSound;

		string strMainCharacterName;

		float fFogFarDelta;
		bool bAcademicNotSupported;

		S_SCENE_INFO() : fSceneStartFadeOutTime( 0.0f ), 
						 fSceneEndFadeInTime( 0.0f ),
						 bUseSceneEndClippingTime( false ),
						 fSceneEndClipTime( 0.0f ),
						 bUseSignalSound( false ),
						 fFogFarDelta( 0.0f ),
						 bAcademicNotSupported( false )
						 {

						 }

	};

	struct S_CS_INFO_BASE
	{
		int iID;
		string strActorName;
		float fStartTime;
		float fTimeLength;
		float fUnit;
		float fSpeed;

		S_CS_INFO_BASE( void ) : iID( -1 ),
								 fStartTime( 0.0f ),
								 fTimeLength( 1.0f ),
								 fUnit( 1.0f ),
								 fSpeed( 60.0f )
		{

		}

		virtual ~S_CS_INFO_BASE() {};
	};


	struct S_ACTION_INFO : public S_CS_INFO_BASE
	{	
		enum
		{
			AT_ONCE,
			AT_REPEAT,
			AT_CONTINUE,
			AT_COUNT,
		};

		S_ACTION_INFO() : iActionType( AT_COUNT ), bUseAniDistance( false ), bFitAniDistanceYToMap( false ), 
						  iAnimationIndex( 0 ), iNextActionFrame( 0 ), fAnimationSpeed( 0.0f ), bUseStartRotation( false ), fStartRotation( 0.0f ),
						  bUseStartPosition( false )
		{
			ZeroMemory( vStartPos, sizeof(vStartPos) );
		}

		string strActionName;		 // 컷신 툴에서 정의한 액션을 의미함
		int iActionType;
		bool bUseAniDistance;		 // 애니메이션의 키를 사용하느냐(x,z)
		bool bFitAniDistanceYToMap;	 // 애니메이션의 키를 사용하되 지형과 Y 높이를 상대적으로 맞출 것인가.
		int iAnimationIndex;		 // 액션 파일 내부에 있는 액션 리스트의 인덱스. 컷신 툴에서만 사용함.
		string strAnimationName;
		int iNextActionFrame;		 // Action Element 의 Next Action 프레임
		float fAnimationSpeed;
		bool bUseStartRotation;
		float fStartRotation;
		bool bUseStartPosition;
		EtVector3 vStartPos;
	};


	struct S_KEY
	{
		int iID;
		EtVector3 vPos;
		float fTimeLength;

		S_KEY( void ) : iID( -1 ), vPos( 0.0f, 0.0f, 0.0f ), fTimeLength( 0.0f ) {}

		bool operator == ( int id ) const
		{
			return iID == id;
		}
	};


	// 키 프레임 정보들.
	struct S_KEY_INFO : public S_CS_INFO_BASE
	{
		enum
		{
			MOVE,
			MULTI_MOVE,
			ROTATION,
			SHOW,
			HIDE,
			//SCALE,
			COUNT,
		};

		string strKeyName;
		int iKeyType;
		EtVector3	vStartPos;
		EtVector3	vDestPos;
		vector<S_KEY> vlMoveKeys;			// MULTI_MOVE 타입일 경우 이동 키들을 모아둔다.
		float		fRotDegree;
		float		fScale;
		bool		bRotateFront;
		bool		bFitYPosToMap;

		S_KEY_INFO( void )
		{
			vStartPos.x = vStartPos.y = vStartPos.z = 0.0f;
			vDestPos.x = vDestPos.y = vDestPos.z = 0.0f;
			fRotDegree = 0.0f;
			iKeyType = 0;			// 0 move, 1 rotation, 2 scale
			fScale = 1.0f;
			bRotateFront = false;
			bFitYPosToMap = true;
		}
	};

	//// 키 셋트 정보
	//struct S_KEY_SET_INFO : public 
	//{
	//	string					strKeySetName;
	//	vector<S_KEY_INFO*>		vlKeys;

	//	S_KEY_SET_INFO( void ) {};
	//};

	// 이벤트 정보
	struct S_EVENT_INFO : public S_CS_INFO_BASE
	{
		enum
		{
			CAMERA,
			PARTICLE,
			DOF,
			FADE,
			PROP,
			SOUND_1,
			SOUND_2,
			SOUND_3,
			SOUND_4,
			SOUND_5,
			SOUND_6,
			SOUND_7,
			SOUND_8,
			SUBTITLE,
			IMAGE,
//#ifdef PRE_ADD_FILTEREVENT
			COLORFILTER,
//#endif // PRE_ADD_FILTEREVENT
			TYPE_COUNT
		};

		int iType;
		string strEventName;

		virtual ~S_EVENT_INFO( void ) {};

		virtual void copy_from( const S_EVENT_INFO* pEventInfo ) = 0;
		virtual S_EVENT_INFO* clone( void ) const = 0;
	};

	// 카메라 이벤트
	struct S_CAMERA_EVENT : public S_EVENT_INFO
	{
		enum
		{
			TRACE_NONE,
			TRACE_ACTOR,
			//TRACE_ACTOR_ONLY_LOOKAT,
			TRACE_COUNT,
		};

		string strImportFilePath;					// 임포팅한 카메라 파일의 풀 패스
		string strImportFileName;					// 임포팅한 카메라의 파일 이름
		EtVector3 vCamStartOffset;					// 카메라 시작 오프셋 위치
		CCameraData* pCameraData;					// 카메라 파일 로더 및 데이터 홀더
		map<string, bool> mapActorsShowHide;		// 액터의 show/hide 여부


		S_CAMERA_EVENT( void ) : pCameraData( new CCameraData ),
								 vCamStartOffset( 0.0f, 0.0f, 0.0f ) { iType = CAMERA; };
		S_CAMERA_EVENT( const char* pFileName ) : strImportFilePath( pFileName ), 
												  pCameraData( new CCameraData ),
												  vCamStartOffset( 0.0f, 0.0f, 0.0f )
		{
			strEventName.assign( pFileName );
			iType = CAMERA;
		}

		~S_CAMERA_EVENT( void )
		{
			SAFE_DELETE( pCameraData );
		}

		// 복사함수들.. 카메라 데이터가 로드된 상태라면 약간 무겁게? //
		// pCamera 데이터도 힙 할당 받아서 사용하므로 copy 함수 사용시 주의! 해제 안하고 copy를 받아버리면 메모리 누수가 일어날 수 있다!
		void copy_from( const S_EVENT_INFO* pEventInfo )
		{
			const S_CAMERA_EVENT* pRhs = static_cast<const S_CAMERA_EVENT*>(pEventInfo);
			strEventName = pRhs->strEventName;
			*pCameraData = *(pRhs->pCameraData);
			strImportFilePath = pRhs->strImportFilePath;
			iID = pRhs->iID;
			fStartTime = pRhs->fStartTime;
			fTimeLength = pRhs->fTimeLength;
			fSpeed = pRhs->fSpeed;
			vCamStartOffset = pRhs->vCamStartOffset;
			fUnit = pRhs->fUnit;
			mapActorsShowHide = pRhs->mapActorsShowHide;
		}

		S_EVENT_INFO* clone( void ) const
		{
			S_CAMERA_EVENT* pCameraEventInfo = new S_CAMERA_EVENT( *this );
			pCameraEventInfo->pCameraData = new CCameraData(*pCameraData);
			return pCameraEventInfo;
		}
		//////////////////////////////////////////////////////////////////////
	};


	struct S_PARTICLE_EVENT : public S_EVENT_INFO
	{
		string				strImportFilePath;
		string				strImportFileName;
		EtVector3			vPos;
		int					iParticleDataIndex;				// 엔진 상에서 사용하는 파티클 데이터 인덱스

		S_PARTICLE_EVENT( void ) : iParticleDataIndex( -1 ), vPos( 0.0f, 0.0f, 0.0f )
		{ 
			iType = PARTICLE;
		};

		~S_PARTICLE_EVENT( void ) {};

		void copy_from( const S_EVENT_INFO* pEventInfo )
		{
			const S_PARTICLE_EVENT* pRhs = static_cast<const S_PARTICLE_EVENT*>(pEventInfo);
			*this = *pRhs;
		}

		S_EVENT_INFO* clone( void ) const
		{
			S_PARTICLE_EVENT* pParticleEventInfo = new S_PARTICLE_EVENT( *this );
			return pParticleEventInfo;
		}
	};

	
	// DOF 이벤트
	struct S_DOF_EVENT : public S_EVENT_INFO
	{
		bool bEnable;
		bool bUseNowValueAsStart;

		float fNearStartFrom;
		float fNearEndFrom;
		float fFarStartFrom;
		float fFarEndFrom;
		float fFocusDistFrom;
		float fNearBlurSizeFrom;
		float fFarBlurSizeFrom;

		float fNearStartDest;
		float fNearEndDest;
		float fFarStartDest;
		float fFarEndDest;
		float fFocusDistDest;
		float fNearBlurSizeDest;
		float fFarBlurSizeDest;


		S_DOF_EVENT( void ) : fNearStartFrom( 0.0f )
							 ,fNearEndFrom( 0.0f )
							 ,fFarStartFrom( 0.0f )
							 ,fFarEndFrom( 0.0f )
							 ,fFocusDistFrom( 0.0f )
							 ,fNearBlurSizeFrom( 0.0f )
							 ,fFarBlurSizeFrom( 0.0f )
							 ,fNearStartDest( 0.0f )
							 ,fNearEndDest( 0.0f )
							 ,fFarStartDest( 0.0f )
							 ,fFarEndDest( 0.0f )
							 ,fFocusDistDest( 0.0f )
							 ,fNearBlurSizeDest( 0.0f )
							 ,fFarBlurSizeDest( 0.0f )
							 ,bUseNowValueAsStart( false )
							 ,bEnable( true )

		{
			iType = DOF;
		}

		void copy_from( const S_EVENT_INFO* pEventInfo )
		{
			const S_DOF_EVENT* pRhs = static_cast<const S_DOF_EVENT*>(pEventInfo);
			*this = *pRhs;
		}

		S_EVENT_INFO* clone( void ) const
		{
			S_DOF_EVENT* pParticleEventInfo = new S_DOF_EVENT( *this );
			return pParticleEventInfo;
		}							  
	};

	struct S_SOUND_EVENT : public S_EVENT_INFO
	{
		string				strImportFilePath;
		string				strImportFileName;
		//EtVector3			vPos;
		int					iSoundDataIndex;
		float				fVolume;

		S_SOUND_EVENT( void ) : iSoundDataIndex( -1 ), /*vPos( 0.0f, 0.0f, 0.0f ),*/ fVolume( 1.0f )
		{
			iType = SOUND_1;		// 초기값이 첫번째 사운드 채널이고 추후에 용도에 맞게 바꿔주어야 한다!
		}

		~S_SOUND_EVENT( void ) {};

		void copy_from( const S_EVENT_INFO* pEventInfo )
		{
			const S_SOUND_EVENT* pRhs = static_cast<const S_SOUND_EVENT*>(pEventInfo);
			*this = *pRhs;
		}

		S_EVENT_INFO* clone( void ) const
		{
			S_SOUND_EVENT* pSoundEventInfo = new S_SOUND_EVENT( *this );
			return pSoundEventInfo;
		}
	};
	
	struct S_FADE_EVENT : public S_EVENT_INFO
	{
		enum
		{
			FADE_IN,
			FADE_OUT,
		};

		int					iFadeKind;
		int					iColorRed;
		int					iColorBlue;
		int					iColorGreen;

		S_FADE_EVENT( void ) : iFadeKind( 0 ), iColorRed( 0 ), iColorGreen( 0 ), iColorBlue( 0 )
		{
			iType = FADE;
		}

		void copy_from( const S_EVENT_INFO* pEventInfo )
		{
			const S_FADE_EVENT* pRhs = static_cast<const S_FADE_EVENT*>(pEventInfo);
			*this = *pRhs;
		}

		S_EVENT_INFO* clone( void ) const
		{
			S_FADE_EVENT* pFadeEventInfo = new S_FADE_EVENT( *this );
			return pFadeEventInfo;
		}
	};

	struct S_PROP_EVENT : public S_EVENT_INFO
	{
		string strActPropName;
		int iPropID;
		string strActionName;
		bool bShow;

		S_PROP_EVENT( void ) : iPropID( -1 ), bShow( true )
		{
			iType = PROP;
		}

		void copy_from( const S_EVENT_INFO* pEventInfo )
		{
			const S_PROP_EVENT* pRhs = static_cast<const S_PROP_EVENT*>(pEventInfo);
			*this = *pRhs;
		}

		S_EVENT_INFO* clone( void ) const
		{
			S_PROP_EVENT* pFadeEventInfo = new S_PROP_EVENT( *this );
			return pFadeEventInfo;
		}
	};


	struct S_SUBTITLE_EVENT : public S_EVENT_INFO
	{
		int iUIStringID;
		//DWORD dwColor;
		
		S_SUBTITLE_EVENT( void ) : iUIStringID( -1 )/*, dwColor( 0 )*/
		{
			iType = SUBTITLE;
		};

		void copy_from( const S_EVENT_INFO* pEventInfo )
		{
			const S_SUBTITLE_EVENT* pRhs = static_cast<const S_SUBTITLE_EVENT*>(pEventInfo);
			*this = *pRhs;
		}

		S_EVENT_INFO* clone( void ) const
		{
			S_SUBTITLE_EVENT* pSubtitleEventInfo = new S_SUBTITLE_EVENT( *this );
			return pSubtitleEventInfo;
		}
	};

	struct S_IMAGE_EVENT : public S_EVENT_INFO
	{
		enum
		{
			IMAGE_IN,
			IMAGE_OUT,
		};

		int					iFadeKind;
		string				strFileName;

		// 이미지 배치 좌표 정보.. 해상도가 다양하다보니 아무래도 화면 비율로 해야할 듯. 
		// 자세히 어떻게 해야할지 제대로 고민해봐야할 듯.
		// 0, 0 을 화면 중심으로 보면 편할까.
		float				fXPos;
		float				fYPos;

		S_IMAGE_EVENT( void ) : iFadeKind( IMAGE_IN ), fXPos( 0.0f ), fYPos( 0.0f )
		{
			iType = IMAGE;
		}

		void copy_from( const S_EVENT_INFO* pEventInfo )
		{
			const S_IMAGE_EVENT* pRhs = static_cast<const S_IMAGE_EVENT*>(pEventInfo);
			*this = *pRhs;
		}

		S_EVENT_INFO* clone( void ) const
		{
			S_IMAGE_EVENT* pImageEventInfo = new S_IMAGE_EVENT( *this );
			return pImageEventInfo;
		}
	};

//#ifdef PRE_ADD_FILTEREVENT
	struct S_COLORFILTER_EVENT : public S_EVENT_INFO
	{
		int bMonochrome;
		EtVector3 vColor;
		float fVolume;

		S_COLORFILTER_EVENT( void )
		{
			iType = COLORFILTER;		// 초기값이 첫번째 사운드 채널이고 추후에 용도에 맞게 바꿔주어야 한다!
			bMonochrome = false;
			vColor = EtVector3(1.0f,1.0f,1.0f);
			fVolume = 1.0f;			
		}

		~S_COLORFILTER_EVENT(){};

		void copy_from( const S_EVENT_INFO* pEventInfo )
		{
			const S_COLORFILTER_EVENT* pRhs = static_cast<const S_COLORFILTER_EVENT*>(pEventInfo);
			*this = *pRhs;
		}

		S_EVENT_INFO* clone( void ) const
		{
			S_COLORFILTER_EVENT* pEventInfo = new S_COLORFILTER_EVENT( *this );
			return pEventInfo;
		}
	};
//#endif // PRE_ADD_FILTEREVENT


public:
	IDnCutSceneDataReader(void);
	virtual ~IDnCutSceneDataReader(void);

	virtual int GetNumRegResource( void ) const = 0;
	virtual const char* GetRegResNameByIndex( int iIndex ) const = 0;
	virtual int GetRegResKindByIndex( int iIndex ) const = 0;
	virtual bool IsRegResource( const char* pResName ) = 0;
	virtual int GetNumActors( void ) const = 0;
	virtual const char* GetActorByIndex( int iActorIndex ) = 0;
	virtual const EtVector3& GetRegResPos( const char* pResName ) = 0;
	virtual float GetRegResRot( const char* pResName ) = 0;
	virtual int GetMonsterTableID( const char* pActorName ) = 0;

	// 신 전체 속성 데이터 조회
	virtual const S_SCENE_INFO* GetSceneInfo( void ) = 0;

	// 액션 관련
	virtual const S_ACTION_INFO* GetActionInfoByID( int iID ) = 0;
	virtual int GetThisActorsActionNum( const char* pActorName ) = 0;
	virtual const S_ACTION_INFO* GetThisActorsActionInfoByIndex( const char* pActorName, int iActionIndex ) = 0;
	virtual const S_KEY_INFO* GetKeyInfoByID( int iID ) = 0;
	virtual int GetThisActorsKeyNum( const char* pActorName ) = 0;
	virtual const S_KEY_INFO* GetThisActorsKeyInfoByIndex( const char* pActorName, int iKeyIndex ) = 0;
	virtual int GetThisActorsAnimationNum( const char* pActorName ) = 0;
	virtual const ActionEleInfo* GetThisActorsAnimation( const char* pActorName, int iAnimationIndex ) = 0;
	virtual bool GetThisActorsFitYPosToMap( const char* pActorName ) = 0;
	virtual bool GetThisActorsInfluenceLightmap( const char* pActorName ) = 0;
	virtual bool GetThisActorsScaleLock( const char* pActorName ) = 0;

	// 이벤트 관련.. 이번엔 타입 하나로 밀어붙여 볼까. ㅋㅋ
	virtual const S_EVENT_INFO* GetEventInfoByID( int iID ) = 0;
	virtual int GetThisTypesEventNum( int iEventType ) = 0;
	virtual const S_EVENT_INFO* GetEventInfoByIndex( int iEventType, int iIndex ) = 0;

	// 데이터 로드
	virtual bool LoadFromFile( const char* pFileName ) = 0;
	virtual bool LoadFromStringBuffer( const char* pFileName ) = 0;
};

typedef IDnCutSceneDataReader::S_SCENE_INFO SceneInfo;
typedef IDnCutSceneDataReader::S_ACTION_INFO ActionInfo;
typedef IDnCutSceneDataReader::S_KEY_INFO KeyInfo;
typedef IDnCutSceneDataReader::S_KEY SubKey;

typedef IDnCutSceneDataReader::S_EVENT_INFO EventInfo;
typedef IDnCutSceneDataReader::S_CAMERA_EVENT CamEventInfo;
typedef IDnCutSceneDataReader::S_PARTICLE_EVENT ParticleEventInfo;
typedef IDnCutSceneDataReader::S_DOF_EVENT DofEventInfo;
typedef IDnCutSceneDataReader::S_SOUND_EVENT SoundEventInfo;
typedef IDnCutSceneDataReader::S_FADE_EVENT FadeEventInfo;
typedef IDnCutSceneDataReader::S_PROP_EVENT PropEventInfo;
typedef IDnCutSceneDataReader::S_SUBTITLE_EVENT SubtitleEventInfo;
typedef IDnCutSceneDataReader::S_IMAGE_EVENT ImageEventInfo;
//#ifdef PRE_ADD_FILTEREVENT
typedef IDnCutSceneDataReader::S_COLORFILTER_EVENT ColorFilterEventInfo;
//#endif // PRE_ADD_FILTEREVENT