#include "StdAfx.h"
#include <shlwapi.h>
#include "ObjectSkin.h"

#include "resource.h"
#include "MainFrm.h"
#include "EtViewerDoc.h"
#include "EtViewerView.h"

#include "GlobalValue.h"
#include "UserMessage.h"

#include "PaneDefine.h"
#include "EtViewer.h"

#include "RenderBase.h"

float CObjectSkin::s_fFramePerSec = 60;
CObjectSkin::CObjectSkin()
{
	m_Type = CObjectBase::SKIN;
	m_nAniIndex = -1;
	m_LocalTime = m_AniTime = 0;
	m_nLoopCount = 0;
	m_fCurFrame = 0.f;
	m_bStop = true;
	m_bPause = false;
	m_bLinkAniInfo = false;
	m_bRegSubMesh = false;
	m_vRotation = EtVector3( 0.f, 0.f, 0.f );

	m_dwVertexCount = 0;
	m_szSimFileName[0] = '\0';
	m_bShowDummy = false;
	m_eSkinType = SKIN_PARTS;
}

CObjectSkin::~CObjectSkin()
{
	CObjectBase *pRoot = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
	for( DWORD i=0; i<pRoot->GetChildCount(); i++ ) {
		CObjectSkin *pSkin = dynamic_cast<CObjectSkin *>(pRoot->GetChild(i));
		if( !pSkin ) continue;
		EtAniObjectHandle hHandle = pSkin->GetObjectHandle();
		if( !hHandle ) continue;
		hHandle->SetParent( CEtObject::Identity(), -1 );
	}
	SAFE_RELEASE_SPTR( m_ObjectHandle );
}

void CObjectSkin::Activate()
{
	if( !m_ObjectHandle ) return;
	// Property
	m_dwVertexCount = 0;
	for( int i=0; i<m_ObjectHandle->GetSubMeshCount(); i++ ) {
		CEtSubMesh *pSubmesh = m_ObjectHandle->GetSubMesh( i );
		m_dwVertexCount += (DWORD)pSubmesh->GetVertexCount();
	}

	m_szVecEffectList.clear();
	for( int i=0; i<m_ObjectHandle->GetSubMeshCount(); i++ ) {
		EtMaterialHandle Handle = m_ObjectHandle->GetMaterial( i );
		bool bFlag = true;
		for( DWORD i=0; i<m_szVecEffectList.size(); i++ ) {
			if( _stricmp( m_szVecEffectList[i], Handle->GetFileName() ) == NULL ) {
				bFlag = false;
				break;
			}
		}
		if( bFlag == true ) {
			m_szVecEffectList.push_back( Handle->GetFileName() );
		}
	}

	CObjectBase::Activate();
	RemakeSubmesh();

	if( m_eSkinType == SKIN_WEAPON ) return;
	if( LinkAccessoryParts() ) return;
	if( GetParent() == NULL ) return;
	DWORD dwChildCount = GetParent()->GetChildCount();
	for( int i=0; i<dwChildCount; i++ )
	{
		CObjectBase* pChild = GetParent()->GetChild( i );
		if( pChild && pChild->GetType() == CObjectBase::ANIMATION )
		{
			CObjectAnimation* pAnimation = (CObjectAnimation*)pChild;
			if( pAnimation )
			{
				LoadAnimation( pAnimation->GetFileName() );

				// 로드되자마자 애니 있을 경우 해당 애니의 프레임까지 적용시켜준다.
				Stop();
				Resume();
				Play( pAnimation->GetFileName(), pAnimation->GetCurAniIndex() );
				Pause();
				SetCurFrame( pAnimation->GetCurFrame() );
			}
		}
	}
	Show( m_bShow );
}

bool CObjectSkin::LinkAccessoryParts()
{
	if( m_eSkinType == SKIN_EARRING || m_eSkinType == SKIN_WING || m_eSkinType == SKIN_TAIL )
	{
		CObjectBase* pParent = GetParent();
		if( pParent != NULL )
		{
			if( pParent->GetType() == CObjectBase::SCENE )
			{
				DWORD dwChildCount = pParent->GetChildCount();
				for( DWORD i=0; i<dwChildCount; i++ )
				{
					CObjectBase* pChild = pParent->GetChild( i );
					if( pChild && pChild->GetType() == CObjectBase::SKIN )
					{
						CObjectSkin* pSkin = (CObjectSkin*)pChild;
						if( pSkin == NULL ) continue;
						if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
						if( StrStrI( pSkin->GetFileName(), "dummy" ) )
						{
							EtAniObjectHandle hDummy = pSkin->GetObjectHandle();
							if( m_ObjectHandle && hDummy )
							{
								std::string szBoneName;
								switch( m_eSkinType )
								{
								case SKIN_EARRING: szBoneName = "#Cash_EarRing"; break;
								case SKIN_WING: szBoneName = "#Cash_Wing"; break;
								case SKIN_TAIL: szBoneName = "#Cash_Tail"; break;
								}
								m_ObjectHandle->SetParent( hDummy, szBoneName.c_str() );
							}
							break;
						}
					}	
				}
			}
		}
		return true;
	}
	return false;
}

void CObjectSkin::InitPropertyInfo()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );
	
	PropertyGridBaseDefine Default[] = {
		{ "Common", "Polygon Count", CUnionValueProperty::Integer, "Polygon Count", FALSE },
		{ "Common", "Submesh Count", CUnionValueProperty::Integer, "Submesh Count", FALSE },
		{ "Common", "World Position", CUnionValueProperty::Vector3, "Object World Position", TRUE },
		{ "Common", "World Rotation", CUnionValueProperty::Vector3, "Object World Rotation", TRUE },
		{ "Shader", "Using Shader Count", CUnionValueProperty::Integer, "Shader Count", FALSE },
		{ "Shader", "Using Shader", CUnionValueProperty::Integer_Combo, "NULL", TRUE },
		{ "Render", "Show BoundingBox", CUnionValueProperty::Boolean, "Show/Hide Bounding Box", TRUE },
		{ "Render", "Show CollisionMesh", CUnionValueProperty::Boolean, "Show/Hide Collsion Mesh", TRUE },
		{ "Render", "Show WireFrame", CUnionValueProperty::Boolean, "Show/Hide WireFrame", TRUE },
		{ "Render", "Show Dummy", CUnionValueProperty::Boolean, "Show/Hide Dummy", TRUE },
		NULL,
	};

	CString szStr;
	if( m_szVecEffectList.size() > 0 ) {
		szStr = "Shader Name";
		for( DWORD i=0; i<m_szVecEffectList.size(); i++ ) {
			szStr += "|" + m_szVecEffectList[i];
		}
		Default[5].szDescription = szStr.GetBuffer();
	}


	((IDirect3DDevice9*)GetEtDevice()->GetDevicePtr())->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	AddPropertyInfo( Default );
}

void CObjectSkin::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			pVariable->SetVariable( (int)m_dwVertexCount );
			break;
		case 1:
			if( m_ObjectHandle ) pVariable->SetVariable( m_ObjectHandle->GetSubMeshCount() );
			break;
		case 4:
			pVariable->SetVariable( (int)m_szVecEffectList.size() );
			break;
		case 6:
			if( m_ObjectHandle ) pVariable->SetVariable( m_ObjectHandle->IsShowBoundingBox() );
			break;
		case 7:
			if( m_ObjectHandle ) pVariable->SetVariable( m_ObjectHandle->IsShowCollisionPrimitive() );
			break;
		case 8:
			break;
		case 10:
			break;
		case 11:
			break;
		default:
			break;
	}
}

void CObjectSkin::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 2:
			{
				m_Cross.m_vPosition = pVariable->GetVariableVector3();
			}
			break;
		case 3:
			{
				EtVector3 vPos = m_Cross.m_vPosition;
				m_Cross.Identity();
				m_Cross.m_vPosition = vPos;
				m_vRotation = pVariable->GetVariableVector3();
				m_Cross.RotateYaw( m_vRotation.y );
				m_Cross.RotatePitch( m_vRotation.x );
				m_Cross.RotateRoll( m_vRotation.z );
			}
			break;
		case 6:
			if( m_ObjectHandle ) m_ObjectHandle->ShowBoundingBox( pVariable->GetVariableBool() );
			break;
		case 7:
			if( m_ObjectHandle ) m_ObjectHandle->ShowCollisionPrimitive( pVariable->GetVariableBool() );
			break;
		case 8:
			((IDirect3DDevice9*)GetEtDevice()->GetDevicePtr())->SetRenderState( D3DRS_FILLMODE, pVariable->GetVariableBool() ? D3DFILL_WIREFRAME : D3DFILL_SOLID );
			break;
		case 9:
			m_bShowDummy = pVariable->GetVariableBool();
			break;
		case 10:
			break;
		case 11:
			break;
		default:
			break;
	}
}

void CObjectSkin::OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
}

void CObjectSkin::Process( LOCAL_TIME LocalTime )
{
	m_LocalTime = LocalTime;
	if( m_ObjectHandle ) {		
		if( m_szSimFileName[0] != '\0' ) {
			if( GetAsyncKeyState(VK_F5) < 0 ) {
				m_ObjectHandle->CreateSimulation( m_szSimFileName );
			}
			BOOL bIsActive = (GetActiveWindow() == AfxGetMainWnd()->m_hWnd);
			if( bIsActive ) {
				const float fSpeed = 500.f;
				if( GetAsyncKeyState(VK_LEFT) < 0 ) {
					m_Cross.m_vPosition.x -= fSpeed / CRenderBase::GetInstance().GetFPS();
				}
				if( GetAsyncKeyState(VK_RIGHT) < 0 ) {
					m_Cross.m_vPosition.x += fSpeed / CRenderBase::GetInstance().GetFPS();
				}
				if( GetAsyncKeyState(VK_UP) < 0 ) {
					m_Cross.m_vPosition.z += fSpeed / CRenderBase::GetInstance().GetFPS();
				}
				if( GetAsyncKeyState(VK_DOWN) < 0 ) {
					m_Cross.m_vPosition.z -= fSpeed / CRenderBase::GetInstance().GetFPS();
				}
				if( GetAsyncKeyState(VK_PRIOR) < 0 ) {
					m_Cross.m_vPosition.y += fSpeed / CRenderBase::GetInstance().GetFPS();
				}
				if( GetAsyncKeyState(VK_NEXT) < 0 ) {
					m_Cross.m_vPosition.y -= fSpeed / CRenderBase::GetInstance().GetFPS();
				}
				Sleep(20);
			}
		}

		m_ObjectHandle->SetCalcPositionFlag( CALC_POSITION_X | CALC_POSITION_Y | CALC_POSITION_Z );
		m_ObjectHandle->Update( m_Cross );

		if( m_nAniIndex != -1 ) {
			if( m_bStop ) {
				m_AniTime = LocalTime;
			}

			if( m_bPause == true ) {
				m_AniTime = LocalTime - (LOCAL_TIME)( ( m_fCurFrame / s_fFramePerSec ) * 1000.f );
			}
			float fTemp = ( ( LocalTime - m_AniTime ) / 1000.f ) * s_fFramePerSec;
			float fTemp2 = fTemp;
			float fAniLength = (float)m_ObjectHandle->GetAniLength( m_nAniIndex );

			float fFrame = fTemp2 - (float)( ((int)fTemp / (int)fAniLength) * fAniLength );

			// 루프 다 돌았으면 마지막 프레임 유지 
			if( m_nLoopCount - ( (int)( fTemp / fAniLength ) ) < 1 ) {
				m_bStop = true;
				m_fCurFrame = fAniLength;
			}
			else {
				m_fCurFrame = fFrame;
			}

			m_ObjectHandle->SetAniFrame( m_nAniIndex, m_fCurFrame );	
			m_ObjectHandle->CalcAni();

			if( m_bShowDummy ) {
				std::vector< std::string > &szDummyNameList = m_ObjectHandle->GetMesh()->GetDummyNameList();
				for each( std::string szDummyName in szDummyNameList ) {					

					EtVector3 vLocalPos = m_ObjectHandle->GetMesh()->GetDummyPosByName( szDummyName.c_str() );
					EtMatrix mat = *m_ObjectHandle->GetWorldMat();

					char *pParentName = m_ObjectHandle->GetMesh()->GetDummyParentByName( szDummyName.c_str() );
					if( pParentName != NULL && pParentName[0] != '\0' && m_ObjectHandle->GetAniHandle() && m_ObjectHandle->GetAniHandle()->GetBoneIndex( pParentName ) != -1) {
						EtMatrixMultiply( &mat, m_ObjectHandle->GetBoneTransMat(  m_ObjectHandle->GetAniHandle()->GetBoneIndex( pParentName ) ), &mat);
					}
					else {
						EtMatrix MatRot;
						EtMatrixRotationY(&MatRot, atan2f( m_ObjectHandle->GetBoneTransMat( 0 )->_31, m_ObjectHandle->GetBoneTransMat( 0 )->_33 ));
						MatRot._41 = m_ObjectHandle->GetBoneTransMat( 0 )->_41;
						MatRot._43 = m_ObjectHandle->GetBoneTransMat( 0 )->_43;
						
						EtMatrixMultiply( &mat, &MatRot, &mat);
					}

					EtMatrix MatLocal;
					EtMatrixTranslation(&MatLocal, vLocalPos.x, vLocalPos.y, vLocalPos.z);
					EtMatrixMultiply(&mat, &MatLocal, &mat);

					EternityEngine::DrawPoint3D( EtVector3(mat._41, mat._42, mat._43), D3DCOLOR_XRGB(255, 0, 255) );
				}
			}

			// 윈도 정보 업뎃
			if( m_bLinkAniInfo == true ) {
				CWnd *pWnd = GetPaneWnd( ANI_PANE );
				if( pWnd ) pWnd->SendMessage( UM_ANICONTROL_UPDATE_FRAME );
			}
		}
		else {
			if( m_bShowDummy ) {
				std::vector< std::string > &szDummyNameList = m_ObjectHandle->GetMesh()->GetDummyNameList();
				for each( std::string szDummyName in szDummyNameList ) {

					EtVector3 vLocalPos = m_ObjectHandle->GetMesh()->GetDummyPosByName( szDummyName.c_str() );
					EtMatrix mat = *m_ObjectHandle->GetWorldMat();

					char *pParentName = m_ObjectHandle->GetMesh()->GetDummyParentByName( szDummyName.c_str() );
					if( pParentName != NULL && pParentName[0] != '\0' ) {
						
						for( int i = 0; i < (int)m_ObjectHandle->GetMesh()->GetBoneNameList().size(); i++) 
						{
							if(strcmp(m_ObjectHandle->GetMesh()->GetBoneNameList()[i].c_str(), pParentName)==0)
							{
								EtMatrix BoneMat;
								EtMatrixInverse( &BoneMat, NULL, &m_ObjectHandle->GetMesh()->GetInvWorldMatList()[i]);
								EtMatrixMultiply( &mat, &BoneMat, &mat);
								break;
							}
						}
					}

					EtMatrix MatLocal;
					EtMatrixTranslation(&MatLocal, vLocalPos.x, vLocalPos.y, vLocalPos.z);
					EtMatrixMultiply(&mat, &MatLocal, &mat);

					EternityEngine::DrawPoint3D( EtVector3(mat._41, mat._42, mat._43), D3DCOLOR_XRGB(255, 0, 255) );
				}
			}
		}

	}
	CObjectBase::Process( LocalTime );
}

void CObjectSkin::LoadSkin( const char *szFileName, bool bCreateObject )
{
	EtAniObjectHandle TempHandle = m_ObjectHandle;
	
	QueueAnimation();

	m_ObjectHandle = EternityEngine::CreateAniObject( szFileName, NULL );
	if( !m_ObjectHandle ) return;
	m_ObjectHandle->EnableCull( false );
	m_ObjectHandle->ShowObject( m_bShow );

	// 같은 파일을 읽을경우가 잇어서 먼저 읽구 지운다.
	SAFE_RELEASE_SPTR( TempHandle );

//	DeleteFile( "MergeTemp.ani" );

	/*
	if( !m_szSkinName.IsEmpty() && m_ItemID ) {
		RemakeSubmesh();
		m_bRegSubMesh = true;
	}
	*/
	m_szSkinName = szFileName;
}

void CObjectSkin::RemakeSubmesh()
{
	if( !m_ObjectHandle ) return;
	if( m_bRegSubMesh == true ) return;

	m_bRegSubMesh = true;
	int nCount = m_ObjectHandle->GetSubMeshCount();
	for( int i=0; i<nCount; i++ ) {
		CString szSubMeshName = m_ObjectHandle->GetSubMeshName( i );

		CObjectBase *pBase = new CObjectSubmesh;
		pBase->SetName( szSubMeshName );

		CGlobalValue::GetInstance().AddObject( m_ItemID, pBase );
	}
}

void CObjectSkin::ReleaseSubmesh()
{
	if( !m_ObjectHandle ) return;

	for( DWORD i=0; i<GetChildCount(); i++ ) {
		CObjectBase *pBase = GetChild(i);
		if( !pBase ) continue;
		if( pBase->GetType() != CObjectBase::SUBMESH ) continue;

		CGlobalValue::GetInstance().RemoveObjectFromTreeID( pBase->GetTreeItemID() );
		i--;
	}
}

bool CObjectSkin::QueueAnimation()
{
	if( m_szVecAniList.size() == 0 ) return true;
	CEtResource::FlushWaitDelete();
	EtAniHandle MergeHandle, DummyHandle;
	MergeHandle = EternityEngine::LoadAni( m_szVecAniList[0] );

	int nOffset = MergeHandle->GetAniCount();
	m_nVecAniOffset.clear();
	m_nVecAniOffset.push_back( nOffset );

	bool bResult = true;
	for( DWORD i=1; i<m_szVecAniList.size(); i++ ) {
		DummyHandle = EternityEngine::LoadAni( m_szVecAniList[i] );
		if( !DummyHandle ) {
			assert(0);
			SAFE_RELEASE_SPTR( DummyHandle );
			bResult = false;
			continue;
		}
		if( EternityEngine::MergeAni( MergeHandle, DummyHandle ) != ET_OK ) {
			assert(0);
			SAFE_RELEASE_SPTR( DummyHandle );
			bResult = false;
			continue;
		}

		nOffset = DummyHandle->GetAniCount();
		m_nVecAniOffset.push_back( nOffset );

		SAFE_RELEASE_SPTR( DummyHandle );
	}

	static int nRefAvoidCount = 0;
	++nRefAvoidCount;
	char szTempName[512], szPath[1024];
	GetCurrentDirectory( 1024, szPath );
	sprintf_s( szTempName, "%s\\__MergeSkinTemp%d.tmp", szPath, nRefAvoidCount );

	EternityEngine::SaveAni( MergeHandle, szTempName );
	SAFE_RELEASE_SPTR( MergeHandle );

	EtAniObjectHandle TempHandle = m_ObjectHandle;
	m_ObjectHandle = EternityEngine::CreateAniObject( m_szSkinName, szTempName );
	m_ObjectHandle->EnableCull( false );
	m_ObjectHandle->ShowObject( m_bShow );

	LinkAccessoryParts();

	SAFE_RELEASE_SPTR( TempHandle );
	DeleteFile( szTempName );
	CEtResource::FlushWaitDelete();
	return bResult;

}

bool CObjectSkin::IsExistAnimation( const char *szFileName )
{
	for( DWORD i=0; i<m_szVecAniList.size(); i++ ) {
		if( _stricmp( szFileName, m_szVecAniList[i] ) == NULL ) return true;
	}
	return false;
}

bool CObjectSkin::LoadAnimation( const char *szFileName )
{
	if( !m_ObjectHandle || m_szSkinName.IsEmpty() ) return false;
	for( DWORD i=0; i<m_szVecAniList.size(); i++ ) {
		if( _stricmp( szFileName, m_szVecAniList[i] ) == NULL ) {
			return false;
		}
	}

	m_szVecAniList.push_back( szFileName );

	bool bResult = QueueAnimation();
	if( bResult == false ) {
		m_szVecAniList.erase( m_szVecAniList.end() - 1 );
	}
	return bResult;
}

void CObjectSkin::RemoveAnimation( const char *szFileName )
{
	if( !m_ObjectHandle || m_szSkinName.IsEmpty() ) return;
	bool bFlag = false;
	for( DWORD i=0; i<m_szVecAniList.size(); i++ ) {
		if( _stricmp( szFileName, m_szVecAniList[i] ) == NULL ) {
			bFlag = true;
			m_szVecAniList.erase( m_szVecAniList.begin() + i );
		}
	}
	if( bFlag == false ) return;

	m_nAniIndex = -1;
	QueueAnimation();
	if( m_szVecAniList.size() == 0 ) {
		LoadSkin( m_szSkinName, true );
	}
}

void CObjectSkin::LoadSimulation( const char *szFileName )
{
	if(!m_ObjectHandle->GetAniHandle()) {
		return;
	}
	m_ObjectHandle->CreateSimulation( szFileName );
	strcpy_s(m_szSimFileName, szFileName);
}

void CObjectSkin::SetCurFrame( float fFrame )
{
	if( m_nAniIndex == -1 ) return;
	m_fCurFrame = fFrame;
	if( m_fCurFrame < 0.f ) m_fCurFrame = 0.f;
	else if( m_fCurFrame >= (float)m_ObjectHandle->GetAniLength( m_nAniIndex ) ) m_fCurFrame = (float)m_ObjectHandle->GetAniLength( m_nAniIndex );
}

void CObjectSkin::Play( const char *szFileName, int nAniIndex, int nLoopCount, float fFrame )
{
	int nIndex = -1;
	int nPrevCount = 0;
	for( DWORD i=0; i<m_szVecAniList.size(); i++ ) {
		if( _stricmp( szFileName, m_szVecAniList[i] ) == NULL ) {
			nIndex = nPrevCount + nAniIndex;
			break;
		}
		else nPrevCount += m_nVecAniOffset[i];
	}

	m_nAniIndex = nIndex;
	m_AniTime = m_LocalTime;
	m_nLoopCount = nLoopCount;
	if( fFrame != 0.f ) {
		m_AniTime -= (LOCAL_TIME)( ( fFrame / s_fFramePerSec ) * 1000.f );
	}
	m_bStop = false;

	if( nIndex == -1 ) return;
}

void CObjectSkin::Stop()
{
	m_bStop = true;
}

void CObjectSkin::Pause()
{
	m_bPause = true;
}

void CObjectSkin::Resume()
{
	m_bPause = false;
	m_AniTime = m_LocalTime;
	m_AniTime -= (LOCAL_TIME)( ( m_fCurFrame / s_fFramePerSec ) * 1000.f );
}

void CObjectSkin::LinkAniInfo( bool bEnable, CObjectAnimation *pChild )
{
	if( m_bLinkAniInfo == bEnable ) return;

	m_bLinkAniInfo = bEnable;
}

int CObjectSkin::GetChildAniIndex( const char *szFileName, int nAniIndex )
{
	int nIndex = -1;
	int nPrevCount = 0;
	for( DWORD i=0; i<m_szVecAniList.size(); i++ ) {
		if( _stricmp( szFileName, m_szVecAniList[i] ) == NULL ) {
			nIndex = nPrevCount + nAniIndex;
			break;
		}
		else nPrevCount += m_nVecAniOffset[i];
	}
	return nIndex;
}
void CObjectSkin::SetLoopCount( int nLoopCount )
{
	if( m_nLoopCount >= nLoopCount ) {
		float fTemp = ( ( m_LocalTime - m_AniTime ) / 1000.f ) * s_fFramePerSec;
		float fAniLength = (float)m_ObjectHandle->GetAniLength( m_nAniIndex );

		m_nLoopCount = (int)( fTemp / fAniLength ) + 1;
	}
	else m_nLoopCount = nLoopCount;
}

void CObjectSkin::Show( bool bShow )
{
	CObjectBase::Show( bShow );

	if( !m_ObjectHandle ) return;

	m_ObjectHandle->ShowObject( bShow );
}

void CObjectSkin::SaveSkin( CString szFileName )
{
	if( !m_ObjectHandle ) return;
	CString szSaveFileName;
	if( szFileName.IsEmpty() ) {
		szSaveFileName = m_ObjectHandle->GetSkin()->GetFullName();
	}
	else {
		szSaveFileName = szFileName;
		m_szSkinName = szFileName;
	}

	if( m_ObjectHandle->GetSkin()->Save( szSaveFileName ) != ET_OK ) {
		CString szStr;
		szStr.Format( "저장 실패 : %s", szSaveFileName );
		MessageBox( ((CMainFrame*)AfxGetMainWnd())->m_hWnd, szStr, "에러", MB_OK );
	}

	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( m_pVecChild[i]->GetType() == CObjectBase::SUBMESH ) {
			m_pVecChild[i]->SetModify( false );
		}
	}

	if( !szFileName.IsEmpty() ) {
		LoadSkin( szFileName, true );
		char szTemp[512] = { 0, };
		_GetFullFileName( szTemp, _countof(szTemp), szFileName.GetBuffer() );
		SetName( CString(szTemp) );

//		CWorkspacePaneView *pView = (CWorkspacePaneView *)GetPaneWnd( WORKSPACE_PANE );
//		pView->SendMessage( UM_REFRESH_PANE
	}
}

void CObjectSkin::ExportObject( FILE *fp, int &nCount )
{
	fwrite( &m_Type, sizeof(int), 1, fp );
	WriteCString( &m_szSkinName, fp );
	fwrite( &m_Cross.m_vPosition, sizeof(EtVector3), 1, fp );
	fwrite( &m_vRotation, sizeof(EtVector3), 1, fp );
	fwrite( &m_bShow, sizeof(bool), 1, fp );

	nCount++;

	CObjectBase::ExportObject( fp, nCount );
}

void CObjectSkin::ImportObject( FILE *fp )
{
	EtVector3 vPos, vRot;

	ReadCString( &m_szSkinName, fp );
	fread( &m_Cross.m_vPosition, sizeof(EtVector3), 1, fp );
	fread( &m_vRotation, sizeof(EtVector3), 1, fp );

	// 서브메시껀 메시가 바뀔 수도 있고, 구해와서 다시 체크박스 해줘야해서, 애매하다.
	fread( &m_bShow, sizeof(bool), 1, fp );

	LoadSkin( m_szSkinName, true );

	if( strstr( m_szSkinName, "Weapon" ) )
	{
		if( StrStrI( m_szSkinName, "Gauntlet" ) == 0 &&
			StrStrI( m_szSkinName, "Academicglove" ) == 0 &&
			StrStrI( m_szSkinName, "Charm" ) == 0 )
			SetSkinType( CObjectSkin::SKIN_WEAPON );
	}
	else if( strstr( m_szSkinName, "Cash_Item" ) )
	{
		if( StrStrI( m_szSkinName, "EarRing" ) )
			SetSkinType( CObjectSkin::SKIN_EARRING );
		else if( StrStrI( m_szSkinName, "Wing" ) )
			SetSkinType( CObjectSkin::SKIN_WING );
		else if( StrStrI( m_szSkinName, "Tail" ) )
			SetSkinType( CObjectSkin::SKIN_TAIL );
	}
}

void CObjectSkin::GetChildFileName( std::vector<CString> &szVecResult )
{
	if( !m_szSkinName.IsEmpty() ) szVecResult.push_back( m_szSkinName );

	CObjectBase::GetChildFileName( szVecResult );
}
