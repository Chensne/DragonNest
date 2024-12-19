#include "StdAfx.h"
#include "DnCutSceneWeapon.h"
#include "DnCutSceneActor.h"
#include "DNTableFile.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCutSceneWeapon::CDnCutSceneWeapon(void) : m_pHasActor( NULL ), m_iEquipIndex( -1 ), m_iLinkBoneIndex( -1 ), m_iEquipType( -1 )
{
	CDnCutSceneActionBase::Initialize( this );
}

CDnCutSceneWeapon::~CDnCutSceneWeapon(void)
{
	if( m_hObject )
		m_hObject->Release();

	FreeAction();
}


bool CDnCutSceneWeapon::LoadSkin( const char* pSkinName, const char* pAniName )
{
	m_strSkinName.assign( pSkinName );
	m_strAniName.assign( pAniName );

	if( m_hObject )
		return true;

	m_hObject = EternityEngine::CreateAniObject( m_strSkinName.c_str(), m_strAniName.c_str() );
	m_hObject->GetSkinInstance()->SetRenderAlphaTwoPass( true );
	m_hObject->ShowObject( true );

	m_hObject->SetCalcPositionFlag( 0 );

	return true;
}


void CDnCutSceneWeapon::SetName( const wchar_t* pWeaponName )
{
	if( pWeaponName )
		m_strWeaponName.assign( pWeaponName );
}


void CDnCutSceneWeapon::Process( LOCAL_TIME LocalTime, float fDelta )
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

	m_hObject->Update( m_matExWorld );

	// 리스트에 있는 시그널 업데이트
	UpdateSignal( LocalTime, fDelta );
}


void CDnCutSceneWeapon::OnSignal( int iSignalType, int iSignalArrayIndex, void* pSignalData, LOCAL_TIME LocalTime, LOCAL_TIME StartTime, LOCAL_TIME EndTime )
{

}


void CDnCutSceneWeapon::UpdateSignal( LOCAL_TIME LocalTime, float fDelta )
{

}

void CDnCutSceneWeapon::LinkWeapon( CDnCutSceneActor* pActor, int iEquipIndex, const char *szBoneName /*= NULL*/ )
{
	if( !m_hObject ) return;
	m_pHasActor = pActor;
	m_iEquipIndex = iEquipIndex;
	if( m_pHasActor )
	{
		string szLinkBoneName;
		if( szBoneName ) 
		{
			szLinkBoneName = szBoneName;
		}
		else
		{
			switch( iEquipIndex )
			{
				case 0:
					szLinkBoneName = "~BoxBone01";
					break;
				case 1:
					szLinkBoneName = "~BoxBone02";
					break;
			}
		}

		int iBoneIndex = -1;
		if( !szLinkBoneName.empty() )
			iBoneIndex = m_pHasActor->GetBoneIndex( szLinkBoneName.c_str() );

		m_hObject->SetParent( m_pHasActor->GetObjectHandle(), iBoneIndex );
		m_iLinkBoneIndex = iBoneIndex;
	}

	if( IsExistAction( "Idle" ) )
		SetAction( "Idle", 0.f, 0.f );

	//m_bLinkBone = true;
}

void CDnCutSceneWeapon::LinkWeapon( CDnCutSceneActor* pActor, std::shared_ptr<CDnCutSceneWeapon> pWeapon )
{
	if( !m_hObject ) 
		return;

	m_pParentWeapon = pWeapon;
	m_pHasActor = pActor;

	string strLinkBoneName = "~BoxBone02";
	int nBoneIndex = m_pParentWeapon->GetBoneIndex( strLinkBoneName.c_str() );
	if(nBoneIndex==-1) {
		OutputDebug("Can't Find ~BoxBone02 \n");
		return;
	}
	m_hObject->SetParent( m_pParentWeapon->GetObjectHandle(), nBoneIndex );
	//m_bLinkBone = true;
	m_iLinkBoneIndex = nBoneIndex;

	if( IsExistAction( "Idle" ) )
		SetAction( "Idle", 0.f, 0.f );
}

int CDnCutSceneWeapon::GetBoneIndex( const char* pBoneName )
{
	if( !m_hObject ) 
		return -1;

	EtAniHandle handle = m_hObject->GetAniHandle();

	if( !handle ) 
		return -1;

	return handle->GetBoneIndex( pBoneName );
}

// from IDnCutSceneRenderObject
int CDnCutSceneWeapon::GetAniIndex( const char* pAniName )
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

// bintitle.
void CDnCutSceneWeapon::Show( bool bShow )
{
	m_hObject->ShowObject( bShow );
}