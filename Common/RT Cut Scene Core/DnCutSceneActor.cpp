#include "StdAfx.h"
#include "DnCutSceneActor.h"
#include "DNTableFile.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCutSceneActor::CDnCutSceneActor(void) : m_vStaticPos( 0.0f, 0.0f, 0.0f ),
										   m_fFrame( 0.0f ),
										   m_fRotationY( 0.0f ),
										   m_bUseSignalSound( false ),
										   m_vScale( 1.0f, 1.0f, 1.0f ),
										   m_iActorType( -1 ),
										   m_apWeaponCnt(0)
{
	CDnCutSceneActionBase::Initialize( this );

	// 얼굴 텍스쳐 바꿔주는 기능을 위해 관련 자료 테이블에서 읽어옴

	//DNTableFileFormat*  pFaceTable = new DNTableFileFormat( "FaceTable.ext" );
	//DNTableFileFormat*  pActorFaceTable = new DNTableFileFormat( "ActorFaceTable.ext" );

	//if( !pActorFaceTable->IsExistItem(  ) ) return false;
	//char szLabel[32];
	//SocialStruct *pSocialStruct;
	//std::string szName;
	//int nFaceAniIndex;
	//for( DWORD i=0; i<50; i++ ) 
	//{
	//	sprintf_s( szLabel, "_Name%d", i + 1 );
	//	szName = pActorFaceTable->GetFieldFromLablePtr( nClassID, szLabel )->GetString();
	//	sprintf_s( szLabel, "_FaceID%d", i + 1 );
	//	nFaceAniIndex = pActorFaceTable->GetFieldFromLablePtr( nClassID, szLabel )->GetInteger();

	//	if( szName.empty() || nFaceAniIndex < 1) continue;
	//	if( !pFaceTable->IsExistItem( nFaceAniIndex ) ) continue;

	//	pSocialStruct = new SocialStruct;
	//	pSocialStruct->szName = szName;

	//	LoadFaceAni( nFaceAniIndex, pSocialStruct );

	//	m_pVecSocialList.push_back( pSocialStruct );
	//	m_szMapSearch.insert( make_pair( szName, (int)m_pVecSocialList.size() - 1 ) );
	//}
}

CDnCutSceneActor::~CDnCutSceneActor(void)
{
	m_apWeaponCnt = 0;

	SAFE_RELEASE_SPTR( m_hCurChannel );
	
	vector<S_PARTICLE_INFO>::iterator iter = m_vlParticles.begin();
	for( iter; iter != m_vlParticles.end(); ++iter )
		SAFE_RELEASE_SPTR( iter->hParticle );

	m_hObject->Release();
	FreeAction();
}


bool CDnCutSceneActor::LoadSkin(const char* pSkinName, const char* pAniName )
{
	m_strSkinName.assign( pSkinName );
	m_strAniName.assign( pAniName );

	if( m_hObject ) 
		return true;

	m_hObject = EternityEngine::CreateAniObject( m_strSkinName.c_str(), m_strAniName.c_str() );
	m_hObject->GetSkinInstance()->SetRenderAlphaTwoPass( true );
	m_hObject->ShowObject( true );

	// 컷신툴에서는 anidistance 를 따로 구해서 더해준다. 
	// 엔진 디폴트에서는 Y 값만 계산해서 적용하도록 되어있어서 여기서 끈다.
	m_hObject->SetCalcPositionFlag( 0 );

	return true;
}


int CDnCutSceneActor::GetAniIndex( const char* pAniName )
{
	if( !m_hObject ) 
		return -1;

	for( int i = 0; i < m_hObject->GetAniCount(); ++i ) 
	{
		if( strcmp( m_hObject->GetAniName(i), pAniName ) == NULL ) 
			return i;
	}

	return -1;
}


void CDnCutSceneActor::SetPosition( const EtVector3& vPos )
{
	m_matExWorld.m_vPosition = vPos;
	m_vStaticPos = vPos;
}


void CDnCutSceneActor::ChangeAnimation( int iAniIndex, float fFrame, float fBlendFrame )
{
	m_iActionIndex = iAniIndex;
	m_fFrame = fFrame;
}


void CDnCutSceneActor::GetAniIndexList( vector<int>& vlAniIndex )
{
	vlAniIndex = m_vlAniIndexList;
}


void CDnCutSceneActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hObject )
		return;

	if( -1 == m_iActionIndex )
		return;

	if( m_iActionIndex >= (int)m_vlAniIndexList.size() )
		return;

	if( m_vlAniIndexList.at(m_iActionIndex)  == -1 )
		return;

	float fPrevFrame = m_fFrame;

	float fAniLength = (float)m_hObject->GetLastFrame( m_vlAniIndexList.at(m_iActionIndex) );
	m_fFrame += fDelta*60.0f;
	if( m_fFrame >= fAniLength )
		m_fFrame = 0.0f;

	ProcessSignal( m_pVecActionElementList->at( m_iActionIndex ), LocalTime, m_fFrame, fPrevFrame );

	m_hObject->SetAniFrame( m_vlAniIndexList.at(m_iActionIndex), m_fFrame );

	EtMatrix matWorld, matScale;
	EtMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	EtMatrixMultiply( &matWorld, &matScale, m_matExWorld );

	m_hObject->Update( &matWorld );

	// 리스트에 있는 시그널 업데이트
	UpdateSignal( LocalTime, fDelta );
}


// 시그널이 날라오면 활성화된 이펙트 등등을 리스트에 담아두고 업데이트 시켜줌.
void CDnCutSceneActor::OnSignal( int iSignalType, int iSignalArrayIndex, void* pSignalData, LOCAL_TIME LocalTime, LOCAL_TIME StartTime, LOCAL_TIME EndTime )
{
	//#28239 관련 - 서버쪽 Hit판정과 동일하게 스케일값 계산
	float fScale = 1.0f;
	
	fScale = GetScale();

	switch( iSignalType )
	{
		case STE_Particle:
			{
				ParticleStruct* pParticle = static_cast<ParticleStruct*>(pSignalData);

				if( -1 == pParticle->nParticleDataIndex )
					break;

				MatrixEx Cross = *GetMatrixEx();
				Cross.MoveLocalZAxis( pParticle->vPosition->z * fScale );
				Cross.MoveLocalXAxis( pParticle->vPosition->x * fScale );
				Cross.MoveLocalYAxis( pParticle->vPosition->y * fScale );
				if( pParticle->vRotation )
				{
					Cross.RotateYaw( pParticle->vRotation->y );
					Cross.RotateRoll( pParticle->vRotation->z );
					Cross.RotatePitch( pParticle->vRotation->x );
				}

				bool bIsPlaying = false;
				int iNumParticle = (int)m_vlParticles.size();
				for( int iParticle = 0; iParticle < iNumParticle; ++iParticle )
				{
					S_PARTICLE_INFO& ParticleInfo = m_vlParticles.at( iParticle );
					if( ParticleInfo.pParticleStruct->nParticleDataIndex == pParticle->nParticleDataIndex )
						if( ParticleInfo.hParticle )
						{
							if( ParticleInfo.hParticle->IsPlay() &&
								(ParticleInfo.iActionIndex == m_iActionIndex) &&
								(ParticleInfo.iSignalIndex == iSignalArrayIndex) )
							{
								bIsPlaying = true;
								break;
							}
						}
				}

				if( false == bIsPlaying )
				{
					EtBillboardEffectHandle hParticle = EternityEngine::CreateBillboardEffect( pParticle->nParticleDataIndex, Cross );

					hParticle->EnableTracePos( (pParticle->bIterator == TRUE) ? true : false );
					hParticle->EnableLoop( (pParticle->bLoop == TRUE) ? true : false );
					hParticle->SetScale( pParticle->fScale + 1.0f );
					hParticle->SetCullDist( 10000000.0f );
					hParticle->Show( true );
					hParticle->EnableCull( false );

					S_PARTICLE_INFO ParticleInfo;
					ParticleInfo.hParticle = hParticle;
					ParticleInfo.iActionIndex = m_vlAniIndexList.at( m_iActionIndex );
					ParticleInfo.iSignalIndex = iSignalArrayIndex;
					ParticleInfo.pParticleStruct = pParticle;
					m_vlParticles.push_back( ParticleInfo );
				}

				//if( pParticle->bLinkObject )
				//{

				//}
			}
			break;

		case STE_Particle_LoopEnd:
			{
				Particle_LoopEndStruct* pParticleLoopEnd = static_cast<Particle_LoopEndStruct*>( pSignalData );

				int iNumParticleInfo = (int)m_vlParticles.size();
				for( int iParticleInfo = 0; iParticleInfo < iNumParticleInfo; ++iParticleInfo )
				{
					S_PARTICLE_INFO& ParticleInfo = m_vlParticles.at( iParticleInfo );
					if( 0 == _stricmp( ParticleInfo.pParticleStruct->szFileName, pParticleLoopEnd->szFileName ) )
					{
						if( ParticleInfo.hParticle )
							ParticleInfo.hParticle->StopPlay();
					}
				}
			}
			break;

		case STE_FX:
			{
				FXStruct* pFXStruct = static_cast<FXStruct*>(pSignalData);

				if( -1 == pFXStruct->nFXDataIndex )
					break;

				MatrixEx Cross = *GetMatrixEx();
				Cross.MoveLocalZAxis( pFXStruct->vOffset->z * fScale );
				Cross.MoveLocalXAxis( pFXStruct->vOffset->x * fScale );
				Cross.MoveLocalYAxis( pFXStruct->vOffset->y * fScale );

				if( pFXStruct->szBoneName && strlen(pFXStruct->szBoneName) > 0) {
					EtMatrix mat;
					GetAniBoneMatrix( pFXStruct->szBoneName , mat);
					memcpy( &Cross.m_vXAxis, &mat._11, sizeof(EtVector3) );
					memcpy( &Cross.m_vYAxis, &mat._21, sizeof(EtVector3) );
					memcpy( &Cross.m_vZAxis, &mat._31, sizeof(EtVector3) );
					Cross.m_vPosition += *(EtVector3*)&mat._41;
					Cross.m_vPosition -= GetMatrixEx()->m_vPosition;
				}

				if( pFXStruct->vRotation ) {
					Cross.RotateYaw( pFXStruct->vRotation->y );
					Cross.RotateRoll( pFXStruct->vRotation->z );
					Cross.RotatePitch( pFXStruct->vRotation->x );
				}

				EtEffectObjectHandle hEffectObject = EternityEngine::CreateEffectObject( pFXStruct->nFXDataIndex, Cross );
				hEffectObject->SetScale( (pFXStruct->fScale + 1.0f) );
				hEffectObject->SetLoop( pFXStruct->bLoop ? true : false );

				S_EFFECT_OBJECT_INFO EffectObjectInfo;
				EffectObjectInfo.hEffectObject = hEffectObject;
				EffectObjectInfo.pFXStruct = pFXStruct;
				m_vlEffectObjects.push_back( EffectObjectInfo );
			}
			break;

		case STE_FX_LoopEnd:
			{
				FX_LoopEndStruct* pFXLoopEndStruct = static_cast<FX_LoopEndStruct*>(pSignalData);
				for( int i = 0; i < (int)m_vlEffectObjects.size(); ++i )
				{
					S_EFFECT_OBJECT_INFO& EffectObject = m_vlEffectObjects.at( i );
					if( EffectObject.hEffectObject )
					{
						if( 0 == _stricmp( EffectObject.pFXStruct->szFileName, pFXLoopEndStruct->szFileName ) )
						{
							EffectObject.hEffectObject->SetLoop( false );
							if( TRUE == pFXLoopEndStruct->bImmediate )
								SAFE_RELEASE_SPTR( EffectObject.hEffectObject );
						}
					}
				}
			}
			break;

		case STE_ActionObject:
			break;

		case STE_Sound:
			{
				if( m_bUseSignalSound )
				{
					SoundStruct* pSound = static_cast<SoundStruct*>(pSignalData);
					if( pSound->nVecSoundIndex == NULL ) break;
					if( pSound->nVecSoundIndex->empty() ) break;
					
					if( m_hCurChannel )
					{
						CEtSoundEngine::GetInstance().RemoveChannel( m_hCurChannel );
						m_hCurChannel.Identity();
					}

					MatrixEx Cross = *GetMatrixEx();
					if( rand()%100 < pSound->nRandom )
					{
						m_hCurChannel = CEtSoundEngine::GetInstance().PlaySound( "2D", (*pSound->nVecSoundIndex)[0], false, true );
						if ( m_hCurChannel ) 
						{
							m_hCurChannel->SetVolume( pSound->fVolume );
							m_hCurChannel->SetRollOff( 3, 0.0f, 1.0f, pSound->fRange * pSound->fRollOff, 1.0f, pSound->fRange, 0.0f );
							m_hCurChannel->SetPosition( Cross.m_vPosition );
							m_hCurChannel->Resume();
						}
					}
				}
			}

		case STE_SocialAction:
			{
				//SocialActionStruct* pSocialActionStruct = (SocialActionStruct*)pSignalData;
				//if( rand()%100 < pStruct->nRandom ) 
				//{
				//	SetFaceAction( pStruct->szActionName );
				//}
			}
			break;

		case STE_AlphaBlending:
			{
				AlphaBlendingStruct *pAlphaBlendStruct = (AlphaBlendingStruct *)pSignalData;
				float fValue = 1.f / ( EndTime - StartTime - 16.6666f ) * ( LocalTime - StartTime );
				fValue = EtClamp( fValue, 0.0f, 1.0f );
				float fCurAlpha = pAlphaBlendStruct->fStartAlpha + ( ( pAlphaBlendStruct->fEndAlpha - pAlphaBlendStruct->fStartAlpha ) * fValue );
				fCurAlpha = EtClamp( fCurAlpha, 0.0f, 1.0f );
				m_hObject->SetObjectAlpha( fCurAlpha );
			}
	}
}

void CDnCutSceneActor::GetAniBoneMatrix( const char *szBoneName, EtMatrix& matBone )
{
	EtAniObjectHandle hObject = GetAniObjectHandle();
	if( !hObject ) 
	{
		EtMatrix MatIdent;
		EtMatrixIdentity(&MatIdent);
		matBone = MatIdent;
	}
	EtMatrix mat;
	int nBoneIndex = hObject->GetAniHandle()->GetBoneIndex( szBoneName );
	if( nBoneIndex == -1 )
	{
		matBone = mat;
		return;
	}

	EtMatrixMultiply( &matBone, hObject->GetBoneTransMat( nBoneIndex ), hObject->GetWorldMat() );
}


void CDnCutSceneActor::UpdateSignal( LOCAL_TIME LocalTime, float fDelta )
{
	//MatrixEx Cross = m_hObject->//*GetEtMatrixEx();
	//MatrixEx Cross = *(m_hObject->GetWorldMat());
	MatrixEx Cross = *GetMatrixEx();

	if( m_hCurChannel )
		m_hCurChannel->SetPosition( Cross.m_vPosition );

	vector<S_PARTICLE_INFO>::iterator iterParticle = m_vlParticles.begin();
	for( iterParticle; iterParticle != m_vlParticles.end(); )
	{
		if( NULL == iterParticle->hParticle )
		{
			iterParticle = m_vlParticles.erase( iterParticle );
			continue;
		}
		else
		{
			const ParticleStruct* pStruct = iterParticle->pParticleStruct;
			if( pStruct->bLinkObject )
			{
				iterParticle->hParticle->SetLink( m_matExWorld, false, 0.f, 
					pStruct->szBoneName, *pStruct->vPosition, *pStruct->vRotation , GetAniObjectHandle() );	
			}
		}

		++iterParticle;
	}


	vector<S_EFFECT_OBJECT_INFO>::iterator iterEffectObject = m_vlEffectObjects.begin();
	for( iterEffectObject; iterEffectObject != m_vlEffectObjects.end(); )
	{
		if( NULL == iterEffectObject->hEffectObject )
		{
			iterEffectObject = m_vlEffectObjects.erase( iterEffectObject );
			continue;
		}
		else
		{
			const FXStruct* pStruct = iterEffectObject->pFXStruct;
			if( pStruct->bLinkObject )
			{
				iterEffectObject->hEffectObject->SetLink( m_matExWorld, pStruct->szBoneName, *pStruct->vOffset, *pStruct->vRotation , m_hObject );

			}
		}

		++iterEffectObject;
	}

	// TODO: iterator 라던지 관리되어야 하는 놈들만 리스트에 넣자.
	//MatrixEx MatExTemp;
	//int iNumParticle = (int)m_vlParticles.size();
	//for( int iParticle = 0; iParticle < iNumParticle; ++iParticle )
	//{
	//	S_PARTICLE_INFO& ParticleInfo = m_vlParticles.at(iParticle);
	//	MatExTemp = Cross;
	//	MatExTemp.MoveLocalZAxis( ParticleInfo.pParticleStruct->vPosition->z	);
	//	MatExTemp.MoveLocalXAxis( ParticleInfo.pParticleStruct->vPosition->x );
	//	MatExTemp.MoveLocalYAxis( ParticleInfo.pParticleStruct->vPosition->y );

	//	ParticleInfo.hParticle->SetWorldMat( MatExTemp );
	//}
}


void CDnCutSceneActor::SetRotationY( float fRotation )
{
	EtVector3 vPos = m_matExWorld.GetPosition();
	m_matExWorld.Identity();
	m_matExWorld.SetPosition( vPos );
	m_matExWorld.RotateYaw( fRotation );
	m_fRotationY = fRotation;
	Show( true );
	//m_crossPos.MakeUpCartesianByZAxis();
}


void CDnCutSceneActor::OnEndCutScene( void )
{
	vector<S_PARTICLE_INFO>::iterator iter = m_vlParticles.begin();
	for( iter; iter != m_vlParticles.end(); ++iter )
	{
		if( iter->hParticle )
			iter->hParticle->StopPlay();
	}


	vector<S_EFFECT_OBJECT_INFO>::iterator it = m_vlEffectObjects.begin();
	for( ; it != m_vlEffectObjects.end(); ++it)
	{
		SAFE_RELEASE_SPTR( it->hEffectObject );
	}
	m_vlEffectObjects.clear();

	UpdateSignal( 0, 0.0f );
}


void CDnCutSceneActor::SetFaceAction( const char* pFaceActionName )
{
	
}


void CDnCutSceneActor::SetScale( float fScale )
{
	m_hObject->SetCollisionScale( fScale );
	m_vScale.x = m_vScale.y = m_vScale.z = fScale;
}


void CDnCutSceneActor::AttachWeapon(std::shared_ptr<CDnCutSceneWeapon> pWeapon, int iIndex )
{
	//_ASSERT( iIndex < 2  && pWeapon.get() );
	if( (iIndex < 0 || 2 <= iIndex) || NULL == pWeapon.get() )
		return;

	m_apWeapon[ iIndex ] = pWeapon;
	
	LinkWeapon( iIndex );

	++m_apWeaponCnt; // bintitle.
}


void CDnCutSceneActor::LinkWeapon( int iEquipIndex )
{
	switch( m_apWeapon[ iEquipIndex ]->GetEquipType() )
	{
		case CDnCutSceneWeapon::Sword:
		case CDnCutSceneWeapon::Axe:
		case CDnCutSceneWeapon::Hammer:
		case CDnCutSceneWeapon::Staff:
		case CDnCutSceneWeapon::Book:
		case CDnCutSceneWeapon::Orb:
		case CDnCutSceneWeapon::Puppet:
		case CDnCutSceneWeapon::Mace:
		case CDnCutSceneWeapon::Flail:
		case CDnCutSceneWeapon::Wand:
		case CDnCutSceneWeapon::Shield:
		case CDnCutSceneWeapon::Cannon:
		case CDnCutSceneWeapon::BubbleGun:
		case CDnCutSceneWeapon::Chakram:
		case CDnCutSceneWeapon::Fan:
			m_apWeapon[ iEquipIndex ]->LinkWeapon( this, iEquipIndex );
			break;
		case CDnCutSceneWeapon::Gauntlet:
		case CDnCutSceneWeapon::Glove:
		case CDnCutSceneWeapon::Charm:
			m_apWeapon[ iEquipIndex ]->LinkWeapon( this, iEquipIndex, "" );
			break;
		case CDnCutSceneWeapon::SmallBow:
		case CDnCutSceneWeapon::BigBow:
		case CDnCutSceneWeapon::CrossBow:
			m_apWeapon[ iEquipIndex ]->LinkWeapon( this, iEquipIndex );
			if( iEquipIndex == 0 && m_apWeapon[0] && m_apWeapon[ 1 ] ) {
				m_apWeapon[ 1 ]->LinkWeapon( this, m_apWeapon[ 0 ] );
			}
			break;
		case CDnCutSceneWeapon::Arrow:
			if( !m_apWeapon[ 0 ] ) 
				break;
			m_apWeapon[ iEquipIndex ]->LinkWeapon( this, m_apWeapon[ 0 ] );
			break;
	}
}


int CDnCutSceneActor::GetBoneIndex( const char* pBoneName )
{
	if( !m_hObject ) 
		return -1;

	if( !m_hObject->GetAniHandle() ) 
		return -1;

	return m_hObject->GetAniHandle()->GetBoneIndex( pBoneName );
}


// bintitle.
void CDnCutSceneActor::ShowWeapon( bool bShow )
{
	for( int i=0; i<m_apWeaponCnt; ++i )
		m_apWeapon[ i ]->Show( bShow );
}