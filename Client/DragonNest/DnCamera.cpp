#include "StdAfx.h"
#include "DnCamera.h"
#include "DnCEQuake.h"
#include "DnCEDof.h"
#include "DnCERadialBlur.h"
#include "DnCECameraBlur.h"
#include "DnCECameraSwing.h"
#include "DnCECameraMovement.h"
#include "DnCECameraKeyFrame.h"
#include "EtOptionController.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_SMART_PTR_STATIC( CDnCamera, 2 )

DnCameraHandle CDnCamera::s_hActiveCamera;
DnCameraHandle CDnCamera::s_hActiveCameraQueue;
int CDnCamera::s_nCameraEffectCount = 0;

CDnCamera::CDnCamera()
{
	m_LocalTime = 0;
	m_CameraType = CameraTypeEnum::Unknown;
	m_bDestroy = false;
}

CDnCamera::~CDnCamera()
{
	SAFE_DELETE_PVEC( m_pVecCameraEffect );
	if( s_hActiveCamera == GetMySmartPtr() ) {
		DeActivate();
		s_hActiveCamera.Identity();
	}
	SAFE_RELEASE_SPTR( m_hCamera );
}

void CDnCamera::ProcessClass( LOCAL_TIME LocalTime, float fDelta )
{
	if( s_hActiveCameraQueue ) {
		SetActiveCamera( s_hActiveCameraQueue );
		s_hActiveCameraQueue.Identity();
	}
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );
	std::vector< CSmartPtr<CDnCamera> > vecDeleteList;
	int nCount = GetItemCount();
	for( int i=0; i<nCount; i++ ) {
		CSmartPtr<CDnCamera> Temp = GetItem(i);
		if( Temp && Temp->IsDestroy() )
		{
			vecDeleteList.push_back( Temp );
		}
	}
	nCount = ( int )vecDeleteList.size();
	for( int i = 0; i < nCount; i++ )
	{
		SAFE_RELEASE_SPTR( vecDeleteList[ i ] );
	}

	if( !s_hActiveCamera ) return;

	s_hActiveCamera->m_LocalTime = LocalTime;

	s_hActiveCamera->Process( LocalTime, fDelta );

//	s_hActiveCamera->m_hCamera->Update( s_hActiveCamera->m_Cross );
}

void CDnCamera::SetActiveCamera( DnCameraHandle hCamera, bool bForce )
{
	if( bForce ) {
		if( s_hActiveCamera ) s_hActiveCamera->DeActivate();

		ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

		int nCount = GetItemCount();
		for( int i=0; i<nCount; i++ ) {
			CSmartPtr<CDnCamera> Temp = GetItem(i);
			if( Temp == hCamera ) {
				s_hActiveCamera = hCamera;
				s_hActiveCamera->Activate();
			}
		}
	}
	else {
		s_hActiveCameraQueue = hCamera;
	}
}

void CDnCamera::ReleaseClass()
{
	DeleteAllObject();
}

DnCameraHandle CDnCamera::FindCamera( CDnCamera::CameraTypeEnum Type )
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	int nCount = GetItemCount();
	for( int i=0; i<nCount; i++ ) {
		CSmartPtr<CDnCamera> Temp = GetItem(i);
		if( Temp && Temp->GetCameraType() == Type ) return Temp;
	}
	return CDnCamera::Identity();
}

void CDnCamera::ProcessEffect( LOCAL_TIME LocalTime, float fDelta )
{
	for( DWORD i=0; i<m_pVecCameraEffect.size(); i++ ) {
		CDnCameraEffectBase *pBase = m_pVecCameraEffect[i];
		if( pBase->IsDestroy() ) {
			SAFE_DELETE( pBase );
			m_pVecCameraEffect.erase( m_pVecCameraEffect.begin() + i );
			i--;
			continue;
		}
		pBase->Process( LocalTime, fDelta );
	}
}

CDnCameraEffectBase *CDnCamera::FindCameraEffect( int nIndex )
{
	for( DWORD i=0; i<m_pVecCameraEffect.size(); i++ ) {
		if( m_pVecCameraEffect[i]->GetIndex() == nIndex ) return m_pVecCameraEffect[i];
	}
	return NULL;
}

void CDnCamera::RemoveCameraEffect( int nIndex )
{
	for( DWORD i=0; i<m_pVecCameraEffect.size(); i++ ) {
		if( m_pVecCameraEffect[i]->GetIndex() == nIndex ) {
			SAFE_DELETE( m_pVecCameraEffect[i] );
			m_pVecCameraEffect.erase( m_pVecCameraEffect.begin() + i );
			break;
		}
	}
}

void CDnCamera::RemoveAllCameraEffect()
{
	for( DWORD i=0; i<m_pVecCameraEffect.size(); i++ ) 
	{
		if( !m_pVecCameraEffect[i]->IsDefaultEffect() )
		{
			SAFE_DELETE( m_pVecCameraEffect[i] );
			m_pVecCameraEffect.erase( m_pVecCameraEffect.begin() + i );
			i--;
		}
	}
}

void CDnCamera::ResetRadialBlurEffect()
{
	for( DWORD i=0; i<m_pVecCameraEffect.size(); i++ ) 
	{
		if( dynamic_cast<CDnCERadialBlur*>(m_pVecCameraEffect[i]) )
		{
			SAFE_DELETE( m_pVecCameraEffect[i] );
			m_pVecCameraEffect.erase( m_pVecCameraEffect.begin() + i );
			i--;
		}
	}
}

void CDnCamera::ResetQuakeEffect()
{
	for( DWORD i=0; i<m_pVecCameraEffect.size(); i++ ) 
	{
		if( dynamic_cast<CDnCEQuake*>(m_pVecCameraEffect[i]) )
		{
			SAFE_DELETE( m_pVecCameraEffect[i] );
			m_pVecCameraEffect.erase( m_pVecCameraEffect.begin() + i );
			i--;
		}
	}
}


int CDnCamera::KeyFrame( char * pStrCamFile, bool bDefaultEffect )
{
	CDnCECameraKeyFrame * pEffect = new CDnCECameraKeyFrame( GetMySmartPtr(), pStrCamFile );
	m_pVecCameraEffect.push_back( pEffect );

	pEffect->SetIndex( s_nCameraEffectCount );
	s_nCameraEffectCount++;
	return pEffect->GetIndex();
}


int CDnCamera::Movement( DWORD dwDelay, EtVector2 vDir, float fSpeedBegin, float fSpeedEnd, bool bDefaultEffect )
{
	CDnCEMovement * pEffect = new CDnCEMovement( GetMySmartPtr(), dwDelay, vDir, fSpeedBegin, fSpeedEnd, bDefaultEffect );
	m_pVecCameraEffect.push_back( pEffect );

	pEffect->SetIndex( s_nCameraEffectCount );
	s_nCameraEffectCount++;
	return pEffect->GetIndex();
}


int CDnCamera::Swing( DWORD dwDelay, float fAngle, float fSpeed, BOOL bSmooth, bool bDefaultEffect )
{
	CDnCESwing * pEffect = new CDnCESwing( GetMySmartPtr(), dwDelay, fAngle, fSpeed, bSmooth, bDefaultEffect );
	m_pVecCameraEffect.push_back( pEffect );

	pEffect->SetIndex( s_nCameraEffectCount );
	s_nCameraEffectCount++;
	return pEffect->GetIndex();
}

int CDnCamera::Quake( DWORD dwDelay, float fBeginRatio, float fEndRatio, float fValue, bool bDefaultEffect )
{
	CDnCEQuake *pEffect = new CDnCEQuake( GetMySmartPtr(), dwDelay, fBeginRatio, fEndRatio, fValue, bDefaultEffect );
	m_pVecCameraEffect.push_back( pEffect );

	pEffect->SetIndex( s_nCameraEffectCount );
	s_nCameraEffectCount++;
	return pEffect->GetIndex();
}

int CDnCamera::DepthOfField( DWORD dwDelay, float fFocusDist, float fNearStart, float fNearEnd, float fFarStart, float fFarEnd, float fNearBlur, float fFarBlur, bool bDefaultEffect )
{
	CDnCEDof *pEffect = new CDnCEDof( GetMySmartPtr(), dwDelay, fFocusDist, fNearStart, fNearEnd, fFarStart, fFarEnd, fNearBlur, fFarBlur, bDefaultEffect );
	m_pVecCameraEffect.push_back( pEffect );

	pEffect->SetIndex( s_nCameraEffectCount );
	s_nCameraEffectCount++;
	return pEffect->GetIndex();
}

int CDnCamera::RadialBlur( DWORD dwDelay, EtVector2 vBlurCenter, float fBlurSize, float fBeginRatio, float fEndRatio, bool bDefaultEffect )
{
	CDnCERadialBlur *pEffect=  new CDnCERadialBlur( GetMySmartPtr(), dwDelay, vBlurCenter, fBlurSize, fBeginRatio, fEndRatio, bDefaultEffect );
	m_pVecCameraEffect.push_back( pEffect );

	pEffect->SetIndex( s_nCameraEffectCount );
	s_nCameraEffectCount++;
	return pEffect->GetIndex();
}

int CDnCamera::CreateCameraBlurFilter()
{
	for( DWORD i=0; i<m_pVecCameraEffect.size(); i++ ) 
	{
		if( dynamic_cast<CDnCECameraBlur*>(m_pVecCameraEffect[i]) )
			return -1;
	}

	// 카메라블러 이펙트객체는 타겟용으로 쓸 백버퍼 가지고 있기때문에 미리 하나만 만들어두고 필요할때 셋팅값 바꿔가면서 사용한다.
	CDnCECameraBlur *pEffect = new CDnCECameraBlur( GetMySmartPtr() );
	m_pVecCameraEffect.push_back( pEffect );

	pEffect->SetIndex( s_nCameraEffectCount );
	s_nCameraEffectCount++;
	return pEffect->GetIndex();
}

int CDnCamera::SetCameraBlur( LOCAL_TIME StartTime, DWORD dwDelay, float fBlendFactor )
{
	CDnCECameraBlur *pCameraBlur = NULL;
	for( DWORD i=0; i<m_pVecCameraEffect.size(); i++ ) 
	{
		pCameraBlur = dynamic_cast<CDnCECameraBlur*>(m_pVecCameraEffect[i]);
		if( pCameraBlur )
		{
			pCameraBlur->SetBlur( StartTime, dwDelay, fBlendFactor );
			return 0;
		}
	}
	return -1;
}

void CDnCamera::SetFreeze( bool bFlag, int nIndex )
{ 
	if( bFlag == false ) {
		if( nIndex == -1 )
			m_nMapFreezeList.clear();
		else m_nMapFreezeList.erase( nIndex ); 
	}
	else {
		std::map<int, float>::iterator it = m_nMapFreezeList.find(nIndex);
		if( it == m_nMapFreezeList.end() )
			m_nMapFreezeList.insert( make_pair( nIndex, 0.f ) );
		else it->second = 0.f;
	}
}

void CDnCamera::SetFreeze( float fResistanceRatio, int nIndex )
{
	std::map<int, float>::iterator it = m_nMapFreezeList.find(nIndex);
	if( it == m_nMapFreezeList.end() ) {
		m_nMapFreezeList.insert( make_pair( nIndex, fResistanceRatio ) );
	}
	else {
		it->second = fResistanceRatio;
	}
}

bool CDnCamera::IsFreeze( int nIndex ) 
{ 
	if( nIndex == -1 ) {
		if( m_nMapFreezeList.empty() ) return false;
		std::map<int, float>::iterator it = m_nMapFreezeList.begin();
		for( ; it!=m_nMapFreezeList.end(); it++ ) {
			if( it->second == 0.f ) return true;
		}
		return false;
	}

	std::map<int, float>::iterator it = m_nMapFreezeList.find(nIndex);
	if( it == m_nMapFreezeList.end() ) return false;
	return true;
}

float CDnCamera::GetFreezeResistanceRatio( int nIndex )
{
	if( nIndex == -1 ) {
		std::map<int, float>::iterator it;
		float fResistance = FLT_MAX;
		for( it=m_nMapFreezeList.begin(); it!=m_nMapFreezeList.end(); it++ ) {
			if( it->second < fResistance ) fResistance = it->second;
		}
		if( fResistance == FLT_MAX ) return 1.f;
		return fResistance;
	}

	std::map<int, float>::iterator it = m_nMapFreezeList.find(nIndex);
	if( it == m_nMapFreezeList.end() ) {
		return 1.f;
	}
	return it->second;
}

void CDnCamera::SetDestroy()
{
	m_bDestroy = true;
}

void CDnCamera::ResetCamera()
{
	RemoveAllCameraEffect();
	m_nMapFreezeList.clear();
}

void CDnCamera::ForceRefreshEngineCamera()
{
	if( m_hCamera ) m_hCamera->Update( m_matExWorld );
}

bool CDnCamera::DeActivate()
{
	RemoveAllCameraEffect();
	return true;
}


void CDnCamera::SetFOV( float fov )
{
	m_hCamera->SetFOV( fov );
}