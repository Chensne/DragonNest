#include "StdAfx.h"
#include "DnEyeLightTrailObject.h"
#include "DnCamera.h"
#include "DnActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_DN_SMART_PTR_STATIC( CDnEyeLightTrailObject, 10 )

CDnEyeLightTrailObject::CDnEyeLightTrailObject( CDnRenderBase *pRenderBase )
: CDnUnknownRenderObject( true )
{
	m_pRenderBase = pRenderBase;
	m_nPointIndex = -1;
	m_fScaleMin = m_fScaleMax = 1.f;
	m_bShow = true;
}

CDnEyeLightTrailObject::~CDnEyeLightTrailObject()
{
	Finalize();
}

EtMatrix CDnEyeLightTrailObject::GetBillboardMatrix( EtVector3 vPos )
{
	EtMatrix Mat;
	EtMatrixIdentity(&Mat);

	DnCameraHandle hDnCamera = CDnCamera::GetActiveCamera();

	*(EtVector3*)&Mat._31 = vPos - hDnCamera->GetMatEx()->GetPosition();
	EtVec3Normalize((EtVector3*)&Mat._31, (EtVector3*)&Mat._31);
	*(EtVector3*)&Mat._21 = EtVector3(0,1,0);
	EtVec3Cross( (EtVector3*)&Mat._11, (EtVector3*)&Mat._21, (EtVector3*)&Mat._31);
	EtVec3Normalize((EtVector3*)&Mat._11, (EtVector3*)&Mat._11);
	EtVec3Cross( (EtVector3*)&Mat._21, (EtVector3*)&Mat._31, (EtVector3*)&Mat._11);
	*(EtVector3*)&Mat._41 = vPos;
	return Mat;
}

void CDnEyeLightTrailObject::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_VecPointList.empty() )
	{
		return;
	}

	EtMatrix matBone = m_pRenderBase->GetBoneMatrix( m_szLinkBoneName.c_str() );	

	if( m_nPointIndex == -1 ) {
		for( DWORD i=0; i<m_VecPointList.size(); i++ ) {
			m_VecPointList[i].vPos = *(EtVector3*)&matBone._41;
		}
		m_nPointIndex = 0;
	}
	else {
		EtVector3 vPos = *(EtVector3*)&matBone._41;
		EtVector3 vPrevPos = m_VecPointList[(m_nPointIndex + m_VecPointList.size() - 1 ) % m_VecPointList.size()].vPos;

		EtVector3 vDiff = vPrevPos - vPos;

		float fLength = EtVec3Length( &vDiff );
		if( fLength > 10000.f || ( vPrevPos.x == FLT_MAX && vPrevPos.y == FLT_MAX && vPrevPos.z == FLT_MAX ) ) {
			fLength = 0.f;
		}
		if( fLength > 14.f ) {
			int nCount = (int)(fLength / 7.f);
			if( nCount > 100 ) nCount = 100;
			for( int i = 0; i < nCount; i++) {
				float fRatio = (float)(i+1) / nCount;
				EtVec3Lerp( &m_VecPointList[m_nPointIndex].vPos, &vPrevPos, &vPos, fRatio );
				m_nPointIndex++;
				m_nPointIndex %= m_VecPointList.size();
			}
		}
		else {
			m_VecPointList[m_nPointIndex].vPos = vPos;
			m_nPointIndex++;
			m_nPointIndex %= m_VecPointList.size();
		}
	}


	for( DWORD i=0; i<m_VecPointList.size(); i++ ) {
		EtMatrix Mat, MatScale;

		float fRatio = (float)((i - m_nPointIndex + m_VecPointList.size())%m_VecPointList.size()) / (m_VecPointList.size()-1);
		float fScale = m_fScaleMin + ( m_fScaleMax * fRatio );
		EtMatrixScaling(&MatScale, fScale, fScale, fScale );

		/*
		static float fEyeDepth = 20.f;

		Mat = GetBillboardMatrix( m_VecPointList[i].vPos );
		*(EtVector3*)&Mat._41 -= (*(EtVector3*)&Mat._31) * fEyeDepth;
		EtMatrixMultiply(&Mat, &MatScale, &Mat);
		*/
		Mat = GetBillboardMatrix( m_VecPointList[i].vPos );
		EtMatrixMultiply(&Mat, &MatScale, &Mat);
		m_VecPointList[i].hObject->Update( &Mat );
	}
}

bool CDnEyeLightTrailObject::Initialize( const char *szSkinName, const char *szLinkBoneName, int nCount, float fScaleMin, float fScaleMax )
{
	Finalize();

	m_szLinkBoneName = szLinkBoneName;
	m_fScaleMin = fScaleMin;
	m_fScaleMax = fScaleMax;
	for( int i=0; i<nCount; i++ ) {
		PointStruct Struct;
		Struct.hObject = EternityEngine::CreateStaticObject( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str() );
		if( !Struct.hObject ) return false;
		Struct.hObject->GetSkinInstance()->SetSkipBakeDepth( true );
		Struct.hObject->GetSkinInstance()->SetSrcBlend( BLEND_SRCCOLOR );
		Struct.hObject->GetSkinInstance()->SetDestBlend( BLEND_DESTALPHA );
		Struct.vPos.x = Struct.vPos.y = Struct.vPos.z = FLT_MAX;

		m_VecPointList.push_back( Struct );
	}

	return true;
}

void CDnEyeLightTrailObject::Finalize()
{
	for( DWORD i=0; i<m_VecPointList.size(); i++ ) {
		SAFE_RELEASE_SPTR( m_VecPointList[i].hObject );
	}
}

void CDnEyeLightTrailObject::Show( bool bShow ) 
{ 
	if( m_bShow == bShow ) return;

	m_bShow = bShow;
	for( DWORD i=0; i<m_VecPointList.size(); i++ ) {
		m_VecPointList[i].hObject->ShowObject( bShow );
	}
}