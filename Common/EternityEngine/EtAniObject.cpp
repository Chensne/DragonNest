#include "StdAfx.h"
#include "EtAniObject.h"
#include "EtSaveMat.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

using namespace EternityEngine;

CSyncLock CEtAniObject::s_CalcAniLock;

CEtAniObject::CEtAniObject(void)
{
	m_bShadowCast = true;
	m_bShadowReceive = false;
	m_nCalcPositionFlag = CALC_POSITION_Y;
	m_nSaveMatIndex = -1;
	EnableLightMapInfluence( true );
	m_bSkipPhysics = false;
	m_bForceSkipSimulateAni = false;

	m_nExtraAniIndex = -1;
	m_fExtraAniFrame = 0.0f;

	m_bAniObject = true;
}

CEtAniObject::~CEtAniObject(void)
{
	SAFE_RELEASE_SPTR( m_hAni );
}

int CEtAniObject::Initialize( EtSkinHandle hSkin, EtAniHandle hAni )
{
	bool bSkinReady, bAniReady;

	m_hSkin = hSkin;
	m_hAni = hAni;
	m_pSkinInstance = new CEtSkinInstance();

	bSkinReady = !m_hSkin->AddCallback( this );
	bAniReady = false;

	if( m_hAni )
	{
		bAniReady = !m_hAni->AddCallback( this );
		if( ( bSkinReady ) && ( bAniReady ) )
		{
			if( m_hSkin->GetMeshHandle() ) {
				m_hSkin->GetMeshHandle()->LinkToAni( m_hAni );
			}
		}
	}

	if( bSkinReady )
	{
		CommonInitialize();
	}

	SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 1 ) );
	SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) );

	return ET_OK;
}

EtMatrix *CEtAniObject::GetBoneTransMat( int nBoneIndex )
{
	EtMatrix BoneWorldMat;
	static EtMatrix BoneMat;

	nBoneIndex = m_hAni->ConvertBoneIndex( nBoneIndex );
	if( !GetEtSaveMat()->IsValidIndex( m_nSaveMatIndex ) )
	{
		CalcAni();
	}

	std::vector< EtMatrix > &vecInvWordlList = GetMesh()->GetInvWorldMatList();
	// 임시 siva - 에서 에러처리로 바꿉니다.
	if( nBoneIndex < 0 || nBoneIndex >= (int)vecInvWordlList.size() ) {
		_ASSERT(0&&"본 인덱스가 잘못됫습니다.");
	}
	else 
	{
		EtMatrixInverse( &BoneWorldMat, NULL, &vecInvWordlList[ nBoneIndex ] );

		if( GetEtSaveMat()->IsValidIndex( m_nSaveMatIndex + nBoneIndex ) )
		{
			return EtMatrixMultiply( &BoneMat, &BoneWorldMat, GetEtSaveMat()->GetMatrix( m_nSaveMatIndex + nBoneIndex ) );
		}
	}
	EtMatrixIdentity( &BoneMat );

	return &BoneMat;
}

EtMatrix *CEtAniObject::GetDummyTransMat( int nDummyIndex )
{
	EtMeshHandle hMesh = m_hSkin->GetMeshHandle();
	if( hMesh )
	{
		std::vector< EtMatrix > &vecDummyMat = hMesh->GetDummyMatrixList();
		std::vector< std::string > &vecParentName = hMesh->GetDummyParentNameList();
		if( nDummyIndex < ( int )vecDummyMat.size() )
		{
			static EtMatrix ReturnMat;
			int nParentIndex = -1;
			if( m_hAni )
			{
				nParentIndex = m_hAni->GetBoneIndex( vecParentName[ nDummyIndex ].c_str() );
			}
			if( nParentIndex == -1 )
			{
				ReturnMat = vecDummyMat[ nDummyIndex ];
			}
			else
			{
				EtMatrixMultiply( &ReturnMat, &vecDummyMat[ nDummyIndex ], GetBoneTransMat( nParentIndex ) );
			}
			return &ReturnMat;
		}
	}

	return NULL;
}

EtMatrix *CEtAniObject::GetBoneMat( int nBoneIndex )
{
	nBoneIndex = m_hAni->ConvertBoneIndex( nBoneIndex );
	if( !GetEtSaveMat()->IsValidIndex( m_nSaveMatIndex ) )
	{
		CalcAni();
	}

	if( GetEtSaveMat()->IsValidIndex( m_nSaveMatIndex ) )
	{
		return GetEtSaveMat()->GetMatrix( m_nSaveMatIndex + nBoneIndex );
	}

	return NULL;
}

void CEtAniObject::ResetAniFrame()
{
	m_vecAniInfo.clear();
	m_vecBlendAniInfo.clear();
	m_vecDisableBone.clear();
	m_vecBoneRotation.clear();
	m_vecBoneScale.clear();
	m_nSaveMatIndex = -1;
}

void CEtAniObject::SetAniFrame( int nAni, float fFrame, int nBoneIndex )
{
	ASSERT( fFrame >= 0.0f );

	if( nBoneIndex == 0 )	// 셋팅하는 본이 루트본일 경우는 모든 애니메이션을 초기화 시켜준다.
	{
		ResetAniFrame();
	}

	SAniInfo AniInfo;

	if( nAni == -1 ) return;
	AniInfo.BoneAniInfo.nAni = nAni;
	AniInfo.BoneAniInfo.fFrame = fFrame;
	AniInfo.nBoneIndex = nBoneIndex;
	m_vecAniInfo.push_back( AniInfo );
}

void CEtAniObject::BlendAniFrame( int nBlendAni, float fBlendFrame, float fBlendWeight, int nBlendBoneIndex )
{
	SAniInfo AniInfo;

	if( nBlendAni == -1 ) return;
	if( fBlendWeight < 0.0f || fBlendWeight > 10.0f )
	{
		ASSERT( 0 && "잘못된 블렌드 웨이트 값" );
		return;
	}
	AniInfo.BoneAniInfo.nAni = nBlendAni;
	AniInfo.BoneAniInfo.fFrame = fBlendFrame;
	AniInfo.BoneAniInfo.fWeight = fBlendWeight;
	AniInfo.nBoneIndex = nBlendBoneIndex;
	m_vecBlendAniInfo.push_back( AniInfo );
	m_vecDisableBone.resize( m_vecBlendAniInfo.size() );
}

void CEtAniObject::BlendAniFrame( int nBlendAni, float fBlendFrame, float fBlendWeight, int nBlendBoneIndex, std::vector< int > &vecDisableBone )
{
	BlendAniFrame( nBlendAni, fBlendFrame, fBlendWeight, nBlendBoneIndex );
	int nBlendCount = ( int )m_vecBlendAniInfo.size();
	if( nBlendCount == 0 )
	{
		return;
	}
	if( !vecDisableBone.empty() )
	{
		m_vecDisableBone[ nBlendCount - 1 ].resize( vecDisableBone.size() );
		std::copy( vecDisableBone.begin(), vecDisableBone.end(), m_vecDisableBone[ nBlendCount - 1 ].begin() );
	}
}

void CEtAniObject::BlendAniFrame( int nBlendAni, float fBlendFrame, float fBlendWeight, int nBlendBoneIndex, int nDisableBoneIndex )
{
	BlendAniFrame( nBlendAni, fBlendFrame, fBlendWeight, nBlendBoneIndex );
	int nBlendCount = ( int )m_vecBlendAniInfo.size();
	if( nBlendCount == 0 )
	{
		return;
	}
	if( nDisableBoneIndex != -1 )
	{
		m_vecDisableBone[ nBlendCount - 1 ].insert( m_vecDisableBone[ nBlendCount - 1 ].begin(), nDisableBoneIndex );
	}
	// 미리 CalcAni()가 불려서 m_nSaveMatIndex가 Valid한 값이라도 중간에 BlendAni나 SetBoneRotation이 불리면 
	// m_nSaveMatIndex가 Invalid함을 표시한다 이렇게 하는 이유는 m_nSaveMatIndex를 -1로 바꿔줄 경우 애니메이션 계산 되면서
	// 계속 메모리가 할당 될 가능성이 있다.
	m_bValidSaveMatIndex = false;
}

void CEtAniObject::SetBoneRotation( int nBoneIndex, EtVector3 &BoneRotation )
{
	SBoneRotationInfo BoneRotationInfo;

	if( nBoneIndex == -1 ) {
		ASSERT( 0 && "Rotate Bone Index -1" );
		return;
	}
	BoneRotationInfo.nBoneIndex = nBoneIndex;
	BoneRotationInfo.BoneRotation = BoneRotation;
	m_vecBoneRotation.push_back( BoneRotationInfo );
	// 미리 CalcAni()가 불려서 m_nSaveMatIndex가 Valid한 값이라도 중간에 BlendAni나 SetBoneRotation이 불리면 
	// m_nSaveMatIndex가 Invalid함을 표시한다 이렇게 하는 이유는 m_nSaveMatIndex를 -1로 바꿔줄 경우 애니메이션 계산 되면서
	// 계속 메모리가 할당 될 가능성이 있다.
	m_bValidSaveMatIndex = false;
}

void CEtAniObject::SetBoneScale( int nBoneIndex, float fScale )
{
	SBoneScaleInfo BoneScaleInfo;

	if( nBoneIndex == -1 ) {
		ASSERT( 0 && "Scale Bone Index -1" );
		return;
	}
	BoneScaleInfo.nBoneIndex = nBoneIndex;
	BoneScaleInfo.fScale = fScale;
	m_vecBoneScale.push_back( BoneScaleInfo );
	m_bValidSaveMatIndex = false;
}

void CEtAniObject::ReBuildSkin( EtSkinHandle hSkin )
{	
	if( ! hSkin )	return;

	SAFE_RELEASE_SPTR( m_hSkin );
	SAFE_DELETE( m_pSkinInstance );

	bool bSkinReady, bAniReady;
	m_hSkin = hSkin;
	m_pSkinInstance = new CEtSkinInstance();

	bSkinReady = !m_hSkin->AddCallback( this );
	bAniReady = false;

	if( m_hAni )
	{
		bAniReady = !m_hAni->AddCallback( this );
		if( ( bSkinReady ) && ( bAniReady ) )
		{
			if( m_hSkin->GetMeshHandle() ) {
				m_hSkin->GetMeshHandle()->LinkToAni( m_hAni );
			}
		}
	}

	if( bSkinReady )
	{
		CommonInitialize();
	}
}

void CEtAniObject::CalcAni()
{
	// 다른 쓰레드에서 동시에 호출되는 경우 있다.. 막아야 한다..
	ScopeLock< CSyncLock > Lock( s_CalcAniLock );

	if( !m_hAni )
	{
		return;
	}

	int i, j;

	m_hAni->Reset();
	m_hAni->SetCalcPositionFlag( m_nCalcPositionFlag );
	if( m_nExtraAniIndex != -1 )
	{
		SBoneAniInfo BoneInfo;
		BoneInfo.nAni = m_nExtraAniIndex;
		BoneInfo.fFrame = m_fExtraAniFrame;
		BoneInfo.fWeight = 0.0f;
		m_hAni->SetAni( &BoneInfo, 0 );
	}
	else
	{
		for( i = 0; i < ( int )m_vecAniInfo.size(); i++ )
		{
			m_hAni->SetAni( &m_vecAniInfo[ i ].BoneAniInfo, m_vecAniInfo[ i ].nBoneIndex );
		}
		for( i = 0; i < ( int )m_vecBlendAniInfo.size(); i++ )
		{
			for( j = 0; j < ( int )m_vecDisableBone[ i ].size(); j++ )
			{
				m_hAni->EnableBlend( m_vecDisableBone[ i ][ j ], false );
			}
			m_hAni->BlendAni( &m_vecBlendAniInfo[ i ].BoneAniInfo, m_vecBlendAniInfo[ i ].nBoneIndex );
			for( j = 0; j < ( int )m_vecDisableBone[ i ].size(); j++ )
			{
				m_hAni->EnableBlend( m_vecDisableBone[ i ][ j ], true );
			}
		}
		for( i = 0; i < ( int )m_vecBoneRotation.size(); i++ )
		{
			m_hAni->SetBoneRotation( m_vecBoneRotation[ i ].nBoneIndex, &m_vecBoneRotation[ i ].BoneRotation );
		}
		for( i = 0; i < ( int )m_vecBoneScale.size(); i++ )
		{
			m_hAni->SetBoneScale( m_vecBoneScale[ i ].nBoneIndex, m_vecBoneScale[ i ].fScale );
		}
	}

	m_nSaveMatIndex = m_hAni->CalcAni( GetMesh()->GetInvWorldMatList() );
	m_bValidSaveMatIndex = true;
}

void CEtAniObject::CreateSimulation( const char *szFileName )
{
	std::map<std::string, int > boneIndexMap;
	int nBoneCount = m_hAni->GetBoneCount();
	for( int i = 0; i < nBoneCount; i++) {
		boneIndexMap[ m_hAni->GetBone(i)->GetName() ] = i;
	}
	m_Spring.Create( szFileName, GetMesh()->GetInvWorldMatList(), boneIndexMap );
}

void CEtAniObject::SimulateAni()
{
	// 애니 잘못 연결하면 아래 InvWorldMatList 얻어오는데서 덤프납니다.
	if( m_hAni->GetBoneCount() > (int)(GetMesh()->GetInvWorldMatList().size()) )
		return;

	int i;
	m_Spring.Simulate( m_WorldMat, GetMesh()->GetInvWorldMatList() );

	int nBoneCount = m_hAni->GetBoneCount();

	EtMatrix *pLockedMatrices = NULL;
	m_nSaveMatIndex = GetEtSaveMat()->LockMatrix( nBoneCount, &pLockedMatrices);	
	for( i = 0; i < nBoneCount; i++) {
		EtMatrix TransMat, VertexTransMat;
		if( i == 0 ) {
			EtMatrixIdentity( &VertexTransMat );
			pLockedMatrices[ i ] = VertexTransMat;
		}
		else {
			EtMatrix InvWorldMat;
			EtMatrixInverse(&InvWorldMat, NULL, &m_WorldMat);
			int nChildOffset = 0;
			if( m_hAni->GetBone( i )->GetChildBoneCount() > 0 ) {
				nChildOffset = m_hAni->GetBone( i )->GetChild( 0 )->GetBoneIndex() - m_hAni->GetBone( i )->GetBoneIndex();
			}
			else {
				nChildOffset = m_hAni->GetBone( i )->GetParent()->GetBoneIndex() - m_hAni->GetBone( i )->GetBoneIndex();
			}			
			TransMat = m_Spring.GetMatrix( i, nChildOffset );
			EtMatrixMultiply( &TransMat, &TransMat, &InvWorldMat);
			EtMatrixMultiply( &VertexTransMat, &GetMesh()->GetInvWorldMatList()[i], &TransMat );
			pLockedMatrices[ i ] = VertexTransMat;
		}
	}
}

void CEtAniObject::ForceSkipSimulateAni( bool bForceSkip, bool bApplyChild )
{
	if( m_Spring.IsEnable() )
	{
		m_bForceSkipSimulateAni = bForceSkip;
	}

	if( bApplyChild )
	{
		for( int i = 0; i < ( int )m_vecChild.size(); i++ )
		{
			if( m_vecChild[ i ]->IsAniObject() )
				( ( EtAniObjectHandle )m_vecChild[ i ] )->ForceSkipSimulateAni( bForceSkip );
		}
	}
}

void CEtAniObject::InitRender( int nSaveMatIndex )
{
	if( !IsShow() )
	{
		return;
	}

	// #25479
	// 마을에서 /go 404하는거 말고, 게임들어간 상태에서 또 다시 /go 404 할때
	// 게임태스크의 Process함수의 EtInterface::Process( fDelta ); 이 라인이 수행되면서 페이드 아웃이 끝났을때 다음번 스테이지 이니셜을 한다.
	// 이때 몬스터도 생성하고 몬스터가 들고있는 프레일 오브젝트도 생성하는데...
	// 그 아래 EternityEngine::RenderFrame( fDelta ); 라인이 프로세스호출하기전에 먼저 수행되는 바람에, 포지션값이 FLT_MAX 초기값 상태 그대로 있게 된다.
	// 이 상태에서 행렬계산하는건 어차피 다음번 프로세스 호출되고 나서 다시 호출될때 재계산될테니 상관없는데,
	// 시뮬레이션은 값을 누적해서 사용하기때문에 한번이라도 FLT_MAX값이 들어가있는 상태에서 계산하면 계속 이상한 값으로 남아있게 되버린다.
	// 그래서 아래와 같이 FLT_MAX인지 판단해서 만약 Invalid하다면 SimulateAni를 호출하지 않도록 하겠다.
	bool bInvalidPos = ( m_PrevWorldMat._11 == FLT_MAX );

	// 유효한 최상위 부모의 월드행렬값이 이상하면 시뮬레이션 하지 않게 처리한다.
	// 말타면서 쓰레드로 부모 설정하는것, 게이지Face에서 강제로 InitRender호출하는 것들때문에, 프로세스 돌기전에 들어오는 경우가 있는 거 같다.
	if( m_hParent )
	{
		EtObjectHandle hParent = m_hParent;
		while( 1 )
		{
			if( hParent->GetParent() )
				hParent = hParent->GetParent();
			else
				break;
		}

		if( hParent->GetWorldMat()->_41 == FLT_MAX )
			bInvalidPos = true;
	}

	int i;

	if( m_hParent )
	{
		CalcParentBoneMat();
	}

	if( m_hAni )
	{
		if( m_Spring.IsEnable() ) {
			if( m_bSkipPhysics ) {
				CalcAni();
				m_Spring.InputKeyframe( m_WorldMat, m_nSaveMatIndex );
			}
			else {
				if( m_bForceSkipSimulateAni == false && bInvalidPos == false )
					SimulateAni();
			}
		}
		else {
			if( m_hParent ) {
				if( m_hAni->GetMyIndex() != m_hParent->GetAniFileIndex() )
				{
					nSaveMatIndex = -1;
				}
				else if( m_nLinkBoneIndex != -1 )
				{
					// SaveMatIndex에 대해서 간단히 설명하겠다.
					// 주된 목적은 같은 ani파일을 사용하는 파츠들의 애니 계산을 여러번 반복해서 하지 않기 위해 같은 SaveMatIndex값을 넣어두고,
					// (보통 동일 캐릭의 머리, 헤어, 머지오브젝트, 워려의 건틀렛 등이 하나의 부모아래 있는 자식이기때문에 같은 SaveMatIndex를 가지게 됩니다.)
					// RenderStack::RenderBlock 함수에서 SetWorldMatArray 호출할때
					// 해당 Object의 WorldMat와 설정된 SaveMat값을 곱해서 EtSaveMat::m_vecTransMat에 넣어두고 쓰게된다.
					//
					// 그런데 위 Ani파일만 검사하는거로는 문제가 발생할 수 있다.
					// 만약 같은 SaveMatIndex를 사용하는 Object들 중에
					// 누구는 m_nLinkBoneIndex가 -1인데 누구는 특정 본인덱스값이 들어있다면, 두 Object의 WorldMat가 달라질수밖에 없다.
					// 이때 렌더링 루틴을 타면서 먼저 RenderBlock이 호출되는 곳에서 TransMat를 한번 구해놓고는
					// 이 후 그 값을 공유해서 쓰기 때문에, 특정 오브젝트에 잘못 링크된것처럼 보이게 된다.
					// 그래서 이렇게 LinkBoneIndex가 -1이 아니라면 부모로부터 전달받은 SaveMatIndex를 무효화시키고
					// 스스로 계산한 m_nSaveMatIndex를 사용하도록 한다.
					nSaveMatIndex = -1;
				}
			}
			bool bMySaveMatInvalid = false;
			if( ( !GetEtSaveMat()->IsValidIndex( m_nSaveMatIndex ) ) || ( !m_bValidSaveMatIndex ) )
			{
				bMySaveMatInvalid = true;
			}
			if( ( !GetEtSaveMat()->IsValidIndex( nSaveMatIndex ) ) && ( bMySaveMatInvalid ) )
			{
				CalcAni();
			}
			else if( !GetEtSaveMat()->IsValidIndex( m_nSaveMatIndex ) )
			{
				m_nSaveMatIndex = nSaveMatIndex;
			}
		}
	}

	for( i = 0; i < ( int )m_vecChild.size(); i++ )
	{
		m_vecChild[ i ]->InitRender( m_nSaveMatIndex );
	}
}

void CEtAniObject::OnLoadComplete( CBackgroundLoader *pLoader )
{
	if( m_hAni )
	{
		if( ( m_hSkin->IsReady() ) && ( m_hAni->IsReady() ) )
		{
			if( m_hSkin->GetMeshHandle() ) {
				m_hSkin->GetMeshHandle()->LinkToAni( m_hAni );
			}
		}
	}
	if( m_hSkin->IsReady() )
	{
		return CEtObject::OnLoadComplete( pLoader );
	}

	CBackgroundLoaderCallback::OnLoadComplete( pLoader );
}

