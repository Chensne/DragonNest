#pragma once

#include "EtMatrixEx.h"
#include "EtWorld.h"



// 애니메이션 객체의 정보를 모아놓은 곳
struct S_CS_ACTOR_INFO
{
	int iActorIndexInProcessorVector;				// ActorProcessor 에서 보관하는 벡터의 인덱스.

	void*					pActor;
	EtAniObjectHandle		hObject;				// 핸들은 외부에서 AddObjectToPlay() 함수를 호출할 때 넣어서 줘야 함.
	// 플레이어인 경우엔 컴바인된 aniobject 핸들을 갖고 있으므로 EnableCull 멤버 함수를 따로 호출해주어야 된다.
	// 애니메이션은 디폴트로 갖고 있는 멤버 aniobject 의 자식으로 지정되어있어서 상관 없음.
	EtAniObjectHandle		hMergedObject;
	string					strActorName;
	MatrixEx			matExFinal;			// Ani Distance 까지 고려된 최종 위치
	EtVector3				vPrevPos;
	bool					bUseAniDistance;
	bool					bKeyUseYPosToMap;		// 키 시퀀스에서 YPosToMap 플래그가 켜져있는 경우.
	bool					bFitAniDistanceYToMap;	// Ani Distance 를 사용할 경우.. Y 높이를 자동으로 맞춥니다.
	bool					bInfluenceLightmap;		// 라이트맵의 영향을 받는지 안 받는지.
	EtVector3				vAniDistanceOffset;		// Ani Distance 를 사용할 경우.. 이 오프셋 포지션에 값이 나온다.
	EtVector3				vPos;					// Ani Distance 를 적용하기 전의 원래 포지션
	float					fRotYDegree;			// Rotation 값
	float					fNowRotProceed;	
	EtVector3				vDefaultPos;			// 초기 위치 값
	float					fDefaultYRotation;		// 초기 Y 회전 값
	EtMatrix				matRotY;				// Rotation 에 해당하는 변환 매트릭스
	vector<int>				vlAniIndex;				// 액션 인덱스를 받아 실제 애니메이션의 인덱스를 갖고 있는 벡터 리스트. 툴에서만 쓰임. 아마도..
	int						iAnimationType;
	int						iNowAniIndex;
	string					strNowActionName;
	float					fSequenceLiveElapsedSec;
	DWORD					dwSequenceStartTime;
	float					fAnimationSpeed;
	float					fFrame;
	bool					bElapsed;				// 1회 이상 애니메이션 루프가 있었음.
	bool					bHided;					// 컷신에 등장하지 않는 액터임

	void*					pNowActionSequence;
	void*					pNowKeySequence;

	bool				    bPlayer; // 플레이어여부.
	bool					bClone;  // #60295 칼리 오프닝 컷씬 칼리 모습이 보이지 않음. - 플레이어액터를 여러개 생성하여 사용한 경우 제거하기위한 플레그.

	S_CS_ACTOR_INFO( void ) : iActorIndexInProcessorVector( 0 ),
							  pActor( NULL ), fFrame( 0.0f ), iNowAniIndex( -1 ), iAnimationType( 0 ), 
							  bUseAniDistance( false ), bKeyUseYPosToMap( false ), bFitAniDistanceYToMap( false ), vAniDistanceOffset( 0.0f, 0.0f, 0.0f ),
							  vPos( 0.0f, 0.0f, 0.0f ),
							  fRotYDegree( 0.0f ), fNowRotProceed( 0.0f ), bElapsed( false ),
							  vDefaultPos( 0.0f, 0.0f, 0.0f ), fDefaultYRotation( 0.0f ),
							  pNowActionSequence( NULL ),
							  pNowKeySequence( NULL ),
							  bHided( false ),
							  bInfluenceLightmap( false ),
							  dwSequenceStartTime(0),
							  fAnimationSpeed(0.0f),
							  fSequenceLiveElapsedSec(0.0f),
							  bPlayer(false),
							  bClone(false)
	{
		EtMatrixIdentity( &matRotY );
	};	
};


struct S_CS_PROP_INFO
{
	int iPropID;
	string strPropActioName;
};


// 커스텀 액터 프로세서임!
class ICustomActorProcessor
{
protected:
	vector<S_CS_ACTOR_INFO*>		m_vlpActors;			// 오브젝트들 끼리 애니메이션 객체 정보 및 제어를 공유
	map<string, S_CS_ACTOR_INFO*>	m_mapActors;

	CEtWorld*						m_pWorld;
	vector<S_CS_PROP_INFO*>			m_vlpProps;


protected:
	ICustomActorProcessor(void);							// 홀로 객체생성 될 수 없음.

public:
	virtual ~ICustomActorProcessor(void);

	virtual void AddObjectToPlay( S_CS_ACTOR_INFO* pNewObject );
	virtual void AddPropToPlay( S_CS_PROP_INFO* pPropInfo );
	virtual void CalcAniDistance(  S_CS_ACTOR_INFO* pActorInfo, float fCurFrame, float fPrevFrame, EtVector3& DistVec ) = 0;
	virtual void SetWorld( CEtWorld* pWorld ) { m_pWorld = pWorld; };
	virtual void SetPropAction( CEtWorldProp* pProp, const char* pActionName, LOCAL_TIME AbsoluteTime, float fFrame ) = 0;
	virtual void OnChangeAction( S_CS_ACTOR_INFO* pActionChangeActor ) {};
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) = 0;
	virtual void OnEndPlayScene( void ) {};
	virtual void ShowSubtitle( const wchar_t* pSubtitle ) = 0;
	virtual void HideSubtitle( void ) = 0;
	virtual int GetActionIndex( int iActorIndex, const char* pActionName ) = 0;		// 주인공 캐릭터들이 달라지게 되는 경우도 있으므로 확실하게 인덱스를 얻어온다.
	virtual float GetActorScale( int iActorIndex ) = 0;
	virtual void ShowProp( CEtWorldProp* pProp, bool bShow ) = 0;

	int GetNumActors( void ) { return (int)m_vlpActors.size(); };
	S_CS_ACTOR_INFO* GetActorInfoByIndex( int iIndex ) { return m_vlpActors.at( iIndex ); };
	S_CS_ACTOR_INFO* GetActorInfoByName( const char* pActorName );

	// 액터와는 관계 없지만은.. 툴과 클라가 쓰는 페이드 인 아웃이 다름 -_-
	virtual void FadeIn( float fSpeed, DWORD dwColor ) {};
	virtual void FadeOut( float fSpeed, DWORD dwColor ) {};


	virtual void ShowEffect( void * pActor, bool bShow ){} // #54681 - 플레이어캐릭터의 Hide처리.
};
