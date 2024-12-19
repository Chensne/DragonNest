#pragma once

#include "EtMatrixEx.h"
#include "EtWorld.h"



// �ִϸ��̼� ��ü�� ������ ��Ƴ��� ��
struct S_CS_ACTOR_INFO
{
	int iActorIndexInProcessorVector;				// ActorProcessor ���� �����ϴ� ������ �ε���.

	void*					pActor;
	EtAniObjectHandle		hObject;				// �ڵ��� �ܺο��� AddObjectToPlay() �Լ��� ȣ���� �� �־ ��� ��.
	// �÷��̾��� ��쿣 �Ĺ��ε� aniobject �ڵ��� ���� �����Ƿ� EnableCull ��� �Լ��� ���� ȣ�����־�� �ȴ�.
	// �ִϸ��̼��� ����Ʈ�� ���� �ִ� ��� aniobject �� �ڽ����� �����Ǿ��־ ��� ����.
	EtAniObjectHandle		hMergedObject;
	string					strActorName;
	MatrixEx			matExFinal;			// Ani Distance ���� ����� ���� ��ġ
	EtVector3				vPrevPos;
	bool					bUseAniDistance;
	bool					bKeyUseYPosToMap;		// Ű ���������� YPosToMap �÷��װ� �����ִ� ���.
	bool					bFitAniDistanceYToMap;	// Ani Distance �� ����� ���.. Y ���̸� �ڵ����� ����ϴ�.
	bool					bInfluenceLightmap;		// ����Ʈ���� ������ �޴��� �� �޴���.
	EtVector3				vAniDistanceOffset;		// Ani Distance �� ����� ���.. �� ������ �����ǿ� ���� ���´�.
	EtVector3				vPos;					// Ani Distance �� �����ϱ� ���� ���� ������
	float					fRotYDegree;			// Rotation ��
	float					fNowRotProceed;	
	EtVector3				vDefaultPos;			// �ʱ� ��ġ ��
	float					fDefaultYRotation;		// �ʱ� Y ȸ�� ��
	EtMatrix				matRotY;				// Rotation �� �ش��ϴ� ��ȯ ��Ʈ����
	vector<int>				vlAniIndex;				// �׼� �ε����� �޾� ���� �ִϸ��̼��� �ε����� ���� �ִ� ���� ����Ʈ. �������� ����. �Ƹ���..
	int						iAnimationType;
	int						iNowAniIndex;
	string					strNowActionName;
	float					fSequenceLiveElapsedSec;
	DWORD					dwSequenceStartTime;
	float					fAnimationSpeed;
	float					fFrame;
	bool					bElapsed;				// 1ȸ �̻� �ִϸ��̼� ������ �־���.
	bool					bHided;					// �ƽſ� �������� �ʴ� ������

	void*					pNowActionSequence;
	void*					pNowKeySequence;

	bool				    bPlayer; // �÷��̾��.
	bool					bClone;  // #60295 Į�� ������ �ƾ� Į�� ����� ������ ����. - �÷��̾���͸� ������ �����Ͽ� ����� ��� �����ϱ����� �÷���.

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


// Ŀ���� ���� ���μ�����!
class ICustomActorProcessor
{
protected:
	vector<S_CS_ACTOR_INFO*>		m_vlpActors;			// ������Ʈ�� ���� �ִϸ��̼� ��ü ���� �� ��� ����
	map<string, S_CS_ACTOR_INFO*>	m_mapActors;

	CEtWorld*						m_pWorld;
	vector<S_CS_PROP_INFO*>			m_vlpProps;


protected:
	ICustomActorProcessor(void);							// Ȧ�� ��ü���� �� �� ����.

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
	virtual int GetActionIndex( int iActorIndex, const char* pActionName ) = 0;		// ���ΰ� ĳ���͵��� �޶����� �Ǵ� ��쵵 �����Ƿ� Ȯ���ϰ� �ε����� ���´�.
	virtual float GetActorScale( int iActorIndex ) = 0;
	virtual void ShowProp( CEtWorldProp* pProp, bool bShow ) = 0;

	int GetNumActors( void ) { return (int)m_vlpActors.size(); };
	S_CS_ACTOR_INFO* GetActorInfoByIndex( int iIndex ) { return m_vlpActors.at( iIndex ); };
	S_CS_ACTOR_INFO* GetActorInfoByName( const char* pActorName );

	// ���Ϳʹ� ���� ��������.. ���� Ŭ�� ���� ���̵� �� �ƿ��� �ٸ� -_-
	virtual void FadeIn( float fSpeed, DWORD dwColor ) {};
	virtual void FadeOut( float fSpeed, DWORD dwColor ) {};


	virtual void ShowEffect( void * pActor, bool bShow ){} // #54681 - �÷��̾�ĳ������ Hideó��.
};
