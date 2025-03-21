#include "StdAfx.h"
#include "ActionObject.h"
#include "GlobalValue.h"
#include "ActionElement.h"

#include "resource.h"
#include "ModifyLengthDlg.h"
#include "RenderBase.h"
#include "VelocityFunc.h"

CActionObject::CActionObject()
: m_vVelocity( 0.f, 0.f, 0.f )
, m_vResistance( 0.f, 0.f, 0.f )
{
	SetType( CActionBase::OBJECT );
	m_LocalTime = m_AniTime = 0;
	m_nLoopCount = 0;
	m_fCurFrame = 0.f;
	m_bStop = true;
	m_bPause = false;
	m_nAniIndex = -1;
	m_fPrevFrame = 0.f;
	m_nTotalFrame = 1;

	m_fMoveYDistancePerSec = 0.0f;
	m_fLeftMoveYDistance = 0.0f;
	m_fOriginalYPos = 0.0f;

	// #56216.		
	m_bRotate = false;
	m_fSpeed = 0.0f;
	m_vAxisPos.x = 0.0f; m_vAxisPos.y = 0.0f; m_vAxisPos.z = 0.0f;
	//EtMatrixIdentity( &m_AxisRotation );		
}

CActionObject::~CActionObject()
{
	SAFE_RELEASE_SPTR( m_ObjectHandle );
}


void CActionObject::InitPropertyInfo()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );

	PropertyGridBaseDefine Default[] = {
		{ "Common", "Path", CUnionValueProperty::String, "전체 경로", FALSE },
		{ "Common", "Skin Name", CUnionValueProperty::String, "스킨 파일 이름", FALSE },
		{ "Common", "Animation Name", CUnionValueProperty::String_FileOpen, "에니메이션 파일 이름|Eternity Animation File (*.ani)|*.ani", TRUE },
		{ "Common", "Action Name", CUnionValueProperty::String, "액션 파일 이름", FALSE },
		{ "Environment", "Environment Texture", CUnionValueProperty::String_FileOpen, "Texture File|All Texture Files|*.dds;*.jpg;*.tga;*.bmp", TRUE },
		{ "Infomation", "Ani Count", CUnionValueProperty::Integer, "에니메이션 갯수", FALSE },
		{ "Infomation", "SubMesh Count", CUnionValueProperty::Integer, "서브메쉬 갯수", FALSE },
		{ "Infomation", "Polygon Count", CUnionValueProperty::Integer, "폴리곤 갯수", FALSE },
		NULL,
	};

	AddPropertyInfo( Default );
}

void CActionObject::Process( LOCAL_TIME LocalTime )
{
//	if( m_LocalTime == LocalTime ) return;
	float fDelta = ( LocalTime - m_LocalTime ) * 0.001f;
	m_LocalTime = LocalTime;

	if( m_bStop ) {
		m_AniTime = LocalTime;
	}

	float fFps = (float)CGlobalValue::GetInstance().GetFPS();
	if( m_bPause == true ) {
		m_AniTime = LocalTime - (LOCAL_TIME)( ( m_fCurFrame / fFps ) * 1000.f );
	}
	float fTemp = ( ( LocalTime - m_AniTime ) / 1000.f ) * fFps;
	float fTemp2 = fTemp;
	float fAniLength = (float)m_nTotalFrame;//(float)m_ObjectHandle->GetAniLength( m_nAniIndex ) - 1.f;

	float fFrame = fTemp2 - (float)( ((int)fTemp / (int)fAniLength) * fAniLength );
	//float fFrame = fTemp2 - ( (fTemp / fAniLength) * (float)fAniLength );

	// 루프 다 돌았으면 마지막 프레임 유지 
	if( m_nLoopCount - ( (int)( fTemp / fAniLength ) ) < 1 ) {
		m_bStop = true;
		m_fCurFrame = fAniLength;
		m_fPrevFrame = fAniLength;

		// #56216.
		m_matExWorld.Identity();
		SetRotate( false );
	}
	else if( m_bStop == false ) {
		if( fFrame != m_fCurFrame ) {
			m_fPrevFrame = m_fCurFrame;
			m_fCurFrame = fFrame;
		}

		if( m_fPrevFrame == m_nTotalFrame )
		{
			m_fPrevFrame = 0.f;			
		}

		// 프레임의 끝. #56216.
		if( (int)(::ceil( fFrame )) >= m_nTotalFrame )
		{			
			m_matExWorld.Identity();
			SetRotate( false );
		}

	}
	
	if( m_ObjectHandle ) {

		//// #56216.
		if( m_bRotate )
			RotateObject( fDelta );

		m_ObjectHandle->SetCalcPositionFlag( CALC_POSITION_X | CALC_POSITION_Y | CALC_POSITION_Z );
		m_ObjectHandle->Update( m_matExWorld );

		if( m_nAniIndex != -1 ) {
			m_ObjectHandle->SetAniFrame( m_nAniIndex, m_fCurFrame );

		}
		if( !m_bPause && !m_bStop ) {
			if( (int)m_fPrevFrame != (int)m_fCurFrame ) {
				m_ObjectHandle->SetObjectAlpha( 1.f );
				m_ObjectHandle->RestoreCustomParam( -1, -1 );
			}
		}
		
	}
	ProcessVelocity( LocalTime, fDelta );
	CActionBase::Process( LocalTime );	
}

bool CActionObject::LoadSkin( const char *pFileName )
{
	if( m_szSkinName == pFileName ) return true;

	m_szSkinName = pFileName;
	SAFE_RELEASE_SPTR( m_ObjectHandle );

	if( m_szAniName.IsEmpty() ) {
//		m_ObjectHandle = EternityEngine::CreateStaticObject( m_szSkinName );
		m_ObjectHandle = EternityEngine::CreateAniObject( m_szSkinName, NULL );
	}
	else {
		m_ObjectHandle = EternityEngine::CreateAniObject( m_szSkinName, m_szAniName );

		SAFE_DELETE_VEC( m_szVecAniNameList );
		if( m_ObjectHandle ) {
			for( int i=0; i<m_ObjectHandle->GetAniCount(); i++ ) {
				m_szVecAniNameList.push_back( m_ObjectHandle->GetAniName( i ) );
			}
		}
	}
	if( m_ObjectHandle ) m_ObjectHandle->EnableCull( false );

	return true;
}

bool CActionObject::LoadAni( const char *pFileName )
{
	if( m_szAniName == pFileName ) return true;

	m_szAniName = pFileName;
	if( m_szSkinName.IsEmpty() || !m_ObjectHandle ) return false;

	SAFE_RELEASE_SPTR( m_ObjectHandle );
	m_ObjectHandle = EternityEngine::CreateAniObject( m_szSkinName, m_szAniName );
	if( m_ObjectHandle ) m_ObjectHandle->EnableCull( false );

	SAFE_DELETE_VEC( m_szVecAniNameList );
	for( int i=0; i<m_ObjectHandle->GetAniCount(); i++ ) {
		m_szVecAniNameList.push_back( m_ObjectHandle->GetAniName( i ) );
	}

	return true;
}

#ifdef _CHECK_WALKFRONT
bool CActionObject::LoadAction( const char *pFileName, bool bCheckOnly )
#else
bool CActionObject::LoadAction( const char *pFileName )
#endif
{
	if( m_szActName == pFileName ) return true;

	m_szActName = pFileName;

	FILE *fp;
	fopen_s( &fp, m_szActName, "rb" );
	if( fp == NULL ) return false;
	ActionHeader Header;

	fread( &Header, sizeof(ActionHeader), 1, fp );

	CModifyLengthDlg::s_nType = 1;
	CModifyLengthDlg::s_bYesAll = false;

	for( int i=0; i<Header.nActionCount; i++ ) {
		CActionElement *pElement = new CActionElement;
		pElement->SetParent( this );
		AddChild( pElement );

#ifdef _CHECK_WALKFRONT
		if (bCheckOnly)
		{
			if (pElement->ImportForCheck(fp,Header.nVersion) == false)
			{
				RemoveChild(pElement);
				fclose(fp);
				return false;
			}
		}
		else
		{
			if( pElement->ImportObject( fp , Header.nVersion ) == false ) {
				RemoveChild( pElement );
				fclose(fp);
				return false;
			}
		}
#else
		if( pElement->ImportObject( fp ) == false ) {
			RemoveChild( pElement );
			//			SAFE_DELETEA( pElement );
			fclose(fp);
			return false;
		}
#endif
		pElement->Activate();
	}

	fclose(fp);

	return true;
}

bool CActionObject::SaveAction()
{
	if( m_szActName.IsEmpty() ) {
		if( m_szSkinName.IsEmpty() ) return true;

		m_szActName.Format( "%s%s.act", m_szPath, GetName() );
	}

#ifdef _CHECK_WALKFRONT
	std::vector<std::string> checkFolderArray;
	checkFolderArray.push_back("Monster");
	if (CheckActionExist(checkFolderArray, "Walk_Front") == eERROR_NOACTION)
	{
		if (MessageBox(0, "경고!! Walk_Front 액션이 빠져있습니다.\n\n움직이는 몬스터의 경우 Walk_Front 액션이 없으면 어느순간 행동이 멈춰버릴 수 있습니다.\n\n정말 저장하시겠습니까?", "경고", MB_YESNO) == IDNO)
			return true;
	}
#endif

	FILE *fp;
	fopen_s( &fp, m_szActName, "wb" );
	if( fp == NULL ) return false;

	ActionHeader Header;
	sprintf_s( Header.szHeaderString, "Eternity Action File" );
	Header.nVersion = 1;
	Header.nActionCount = (int)m_pVecChild.size();

	fwrite( &Header, sizeof(ActionHeader), 1, fp );
	if( CActionBase::ExportObject( fp, Header.nActionCount ) == false ) {
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

#ifdef _CHECK_WALKFRONT
CActionObject::eCheckWalkFrontResult CActionObject::CheckActionExistWithLoadFile(const std::string& fileName, const std::string& checkActionName)
{
	LoadAction(fileName.c_str(), true);

	if (IsActionExist(checkActionName))
		return eOK_EXISTACTION;

	return eERROR_NOACTION;
}

CActionObject::eCheckWalkFrontResult CActionObject::CheckActionExist(const std::vector<std::string>& checkPathArray, const std::string& checkActionName) const
{
	std::string pathWrapper(m_szPath);
	bool bCheck = false;
	std::vector<std::string>::const_iterator pathIter = checkPathArray.begin();
	for (; pathIter != checkPathArray.end(); ++pathIter)
	{
		const std::string& checkPathName = *pathIter;
		if (pathWrapper.find(checkPathName.c_str()) != std::string::npos)
		{
			bCheck = true;
			break;
		}
	}

	if (bCheck)
	{
		if (IsActionExist(checkActionName))
			return eOK_EXISTACTION;
	}
	else
	{
		return eOK_NONEED;
	}

	return eERROR_NOACTION;
}

bool CActionObject::IsActionExist(const std::string& checkActionName) const
{
	std::vector<CActionBase*>::const_iterator iter = m_pVecChild.begin();
	for (; iter != m_pVecChild.end(); ++iter)
	{
		CActionBase* pCurAction = (*iter);
		if (pCurAction)
		{
			std::string actionNameWrapper(pCurAction->GetName());
			if (actionNameWrapper.compare(checkActionName.c_str()) == 0)
				return true;
		}
	}

	return false;
}
#endif // _CHECK_WALKFRONT

void CActionObject::SetPath( CString szStr )
{
	m_szPath = szStr;
}

void CActionObject::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			pVariable->SetVariable( m_szPath.GetBuffer() );
			break;
		case 1:
			if( m_szSkinName.IsEmpty() ) pVariable->SetVariable( "Not Found" );
			else {
				char szTemp[512] = { 0, };
				_GetFileName( szTemp, _countof(szTemp), m_szSkinName );
				sprintf_s( szTemp, "%s.skn", szTemp );
				pVariable->SetVariable( szTemp );
			}
			break;
		case 2:
			if( m_szAniName.IsEmpty() ) pVariable->SetVariable( "Not Found" );
			if( !m_szAniName.IsEmpty() ) {
				char szTemp[512] = { 0, };
				_GetFileName( szTemp, _countof(szTemp), m_szAniName );
				sprintf_s( szTemp, "%s.ani", szTemp );
				pVariable->SetVariable( szTemp );
			}
			break;
		case 3:
			if( m_szActName.IsEmpty() ) pVariable->SetVariable( "Not Found" );
			else {
				char szTemp[512] = { 0, };
				_GetFileName( szTemp, _countof(szTemp), m_szActName );
				sprintf_s( szTemp, "%s.act", szTemp );
				pVariable->SetVariable( szTemp );
			}
			break;
		case 4:
			if( CRenderBase::GetInstance().GetEnvironmentTexture() == NULL ||
				strlen( CRenderBase::GetInstance().GetEnvironmentTexture() ) == 0 ) pVariable->SetVariable( "Not Found" );
			else {
				pVariable->SetVariable( (char*)CRenderBase::GetInstance().GetEnvironmentTexture() );
			}
			break;
		case 5:
			{
				int nCount = 0;
				if( m_ObjectHandle && m_ObjectHandle->GetAniHandle() ) {
					nCount = m_ObjectHandle->GetAniCount();
				}
				pVariable->SetVariable( nCount );
			}
			break;
		case 6:
			{
				int nCount = 0;
				if( m_ObjectHandle ) {
					nCount = m_ObjectHandle->GetSubMeshCount();
				}
				pVariable->SetVariable( nCount );
			}
			break;
		case 7:
			{
				int nCount = 0;
				if( m_ObjectHandle ) {
					for( int i=0; i<m_ObjectHandle->GetSubMeshCount(); i++ ) {
						nCount += m_ObjectHandle->GetSubMesh(i)->GetVertexCount();
					}
				}
				pVariable->SetVariable( nCount );
			}
			break;
	}
}

void CActionObject::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 2:
			{
				if( LoadAni( pVariable->GetVariableString() ) == true ) {
					char szTemp[512] = { 0, };
					_GetFileName( szTemp, _countof(szTemp), m_szAniName );
					sprintf_s( szTemp, "%s.ani", szTemp );
					pVariable->SetVariable( szTemp );
				}
				else {
					pVariable->SetVariable( "Not Found" );
				}
			}
			break;
		case 4:
			{
				CRenderBase::GetInstance().SetEnvironmentTexture( pVariable->GetVariableString() );
			}
			break;
	}
}

int CActionObject::GetAniCount()
{
	if( !m_ObjectHandle ) return 0;
	if( !m_ObjectHandle->GetAniHandle() ) return 0;
	return m_ObjectHandle->GetAniCount();
}

CString CActionObject::GetAniName( int nAniIndex )
{
	if( nAniIndex < 0 || nAniIndex >= (int)m_szVecAniNameList.size() ) return CString("");
	return m_szVecAniNameList[nAniIndex];
}

int CActionObject::GetAniLength( CString szAniName )
{
	for( DWORD i=0; i<m_szVecAniNameList.size(); i++ ) {
		if( m_szVecAniNameList[i] == szAniName ) return m_ObjectHandle->GetAniLength(i);
	}
	assert(0);
	return 0;
}

int CActionObject::GetAniIndex( CString szAniName )
{
	for( DWORD i=0; i<m_szVecAniNameList.size(); i++ ) {
		if( m_szVecAniNameList[i] == szAniName ) return (int)i;
	}
//	assert(0);
	return -1;
}

bool CActionObject::IsCanPlay( CString szAniName )
{
	if( !m_ObjectHandle && szAniName != "") return false;
	/*
	if( m_ObjectHandle->GetAniCount() == 0 ) return false;
	if( GetAniIndex( szAniName ) == -1 ) return false;
	*/
	return true;
}

void CActionObject::SetPlay( CString szAniName )
{
//	if( szAniName.IsEmpty() ) return;
	if( !IsCanPlay( szAniName ) ) return;

	Play( szAniName, 1, 0.f );
	Resume();
	Stop();
	SetCurFrame( 0.f );
}

void CActionObject::Play( CString szAniName, int nLoopCount, float fFrame )
{
	// #56216.
	EtMatrixIdentity( (EtMatrix *)(&m_matExWorld) );

	if( !szAniName.IsEmpty() ) {
		m_nAniIndex = GetAniIndex( szAniName );
	}
	else {
		m_nAniIndex = -1;
	}
	m_AniTime = m_LocalTime;
	m_nLoopCount = nLoopCount;
	if( fFrame != 0.f ) {
		m_AniTime -= (LOCAL_TIME)( ( fFrame / (float)CGlobalValue::GetInstance().GetFPS() ) * 1000.f );
	}
	else
		m_fPrevFrame = 0.f;
	m_bStop = false;
}

void CActionObject::Stop()
{
	m_bStop = true;
	SetRotate( false ); // #56216.
}

void CActionObject::Pause()
{
	m_bPause = true;

	SetRotate( false ); // #56216.
}

void CActionObject::Resume()
{
	m_bPause = false;
	m_AniTime = m_LocalTime;
	m_AniTime -= (LOCAL_TIME)( ( m_fCurFrame / (float)CGlobalValue::GetInstance().GetFPS() ) * 1000.f );

	SetRotate( true ); // #56216.
}

bool CActionObject::IsStop()
{
	return m_bStop;
}


bool CActionObject::IsPause()
{
	return m_bPause;
}

void CActionObject::SetCurFrame( float fFrame )
{
	m_fCurFrame = fFrame;
	if( m_fCurFrame < 0.f ) m_fCurFrame = 0.f;
	else if( m_fCurFrame >= (float)m_nTotalFrame ) m_fCurFrame = (float)m_nTotalFrame;

	// MoveY 시그널 값으로 이동된 것 리셋.
	m_matExWorld.m_vPosition.y = m_fOriginalYPos;
	ResetMoveYDistance();
}

float CActionObject::GetCurFrame()
{
	return m_fCurFrame;
}

float CActionObject::GetPrevFrame()
{
	return m_fPrevFrame;
}

void CActionObject::SetLoopCount( int nLoopCount )
{
	if( m_nLoopCount >= nLoopCount ) {
		float fTemp = ( ( m_LocalTime - m_AniTime ) / 1000.f ) * (float)CGlobalValue::GetInstance().GetFPS();
		float fAniLength = (float)m_nTotalFrame;

		m_nLoopCount = (int)( fTemp / fAniLength ) + 1;
	}
	else m_nLoopCount = nLoopCount;
}

int CActionObject::GetLoopCount()
{
	return m_nLoopCount;
}

EtVector3 CActionObject::GetAniDistance()
{
	if( !m_ObjectHandle ) return EtVector3( 0.f, 0.f, 0.f );
	if( m_nAniIndex == -1 ) return EtVector3( 0.f, 0.f, 0.f );
	EtVector3 vDist;
	m_ObjectHandle->CalcAniDistance( m_nAniIndex, m_fCurFrame, 0.f, vDist );
	vDist.y = m_matExWorld.m_vPosition.y;
	return vDist;
}

bool CActionObject::CheckExistLinkAniAction( const char *pFileName )
{
	FILE *fp;
	fopen_s( &fp, pFileName, "rb" );
	if( fp == NULL ) return false;
	ActionHeader Header;

	fread( &Header, sizeof(ActionHeader), 1, fp );

	for( int i=0; i<Header.nActionCount; i++ ) {
		CActionElement *pElement = new CActionElement;
		pElement->SetParent( this );

		if( pElement->CheckImportLinkAni( fp ,Header.nVersion) == true ) {
			fclose(fp);
			SAFE_DELETE( pElement );
			return true;
		}
		SAFE_DELETE( pElement );
	}

	fclose(fp);

	return false;
}

void CActionObject::ProcessVelocity( LOCAL_TIME LocalTime, float fDelta )
{
	float fValue = (float)CGlobalValue::GetInstance().GetFPS() / 60.f * fDelta;


	EtVector3 vVelocity;
	if( m_vVelocity.x ) {
		float fTemp = m_vVelocity.x;
		float fMin = ( fTemp > 0.f ) ? 0.f : -FLT_MAX;
		float fMax = ( fTemp > 0.f ) ? FLT_MAX : 0.f;	

		float fTemp2 = m_vVelocity.x;

		m_matExWorld.m_vPosition.x += CalcMovement( m_vVelocity.x, fDelta, fMax, fMin, m_vResistance.x );

		if( m_vVelocity.x * fTemp <= 0.f ) {
			m_matExWorld.m_vPosition.x = 0.f;
			m_vVelocity.x = 0.f;
		}
	}
	else m_matExWorld.m_vPosition.x = 0.f;

	if( m_vVelocity.z ) {
		float fTemp = m_vVelocity.z;
		float fMin = ( fTemp > 0.f ) ? 0.f : -FLT_MAX;
		float fMax = ( fTemp > 0.f ) ? FLT_MAX : 0.f;	

		float fTemp2 = m_vVelocity.z;

		m_matExWorld.m_vPosition.z += CalcMovement( m_vVelocity.z, fDelta, fMax, fMin, m_vResistance.z );

		if( m_vVelocity.z * fTemp <= 0.f ) {
			m_matExWorld.m_vPosition.z = 0.f;
			m_vVelocity.z = 0.f;
		}
	}
	else m_matExWorld.m_vPosition.z = 0.f;
	if( m_vVelocity.y ) {
		m_matExWorld.m_vPosition.y += CalcMovement( m_vVelocity.y, fValue, FLT_MAX, -FLT_MAX, m_vResistance.y );
		if( m_vVelocity.y == 0.f ) m_vVelocity.y += 0.0000001f;

		if( m_matExWorld.m_vPosition.y <= 0.f ) {
			m_matExWorld.m_vPosition.y = 0.f;
			m_vVelocity.y = 0.f;
		}
	}

	// #48950 MoveY 시그널 처리.
	if( 0.0f != m_fMoveYDistancePerSec )
	{
		if( 0.0f == m_fOriginalYPos )
			m_fOriginalYPos = m_matExWorld.m_vPosition.y;

		float fMoveDistance = m_fMoveYDistancePerSec * fDelta;
		m_matExWorld.m_vPosition.y += fMoveDistance;

		if( 0.0f < fMoveDistance )
			m_fLeftMoveYDistance -= fMoveDistance;
		else
			m_fLeftMoveYDistance += fMoveDistance;

		// 남은 거리를 넘어설 경우 도착한 거임.
		// 위치를 셋팅해주고 끝냄.
		if( m_fLeftMoveYDistance < 0.0f )
		{
			if( 0.0f < fMoveDistance )
				m_matExWorld.m_vPosition.y -= (fMoveDistance + m_fLeftMoveYDistance);
			else
				m_matExWorld.m_vPosition.y += (fMoveDistance - m_fLeftMoveYDistance);

			ResetMoveYDistance();
		}
	}
	
}


// #56216.
void CActionObject::SetAxisPosition( EtVector3 & vPos )
{
	m_vAxisPos = vPos;
//	EtMatrixTranslation( &m_AxisRotation, vPos.x, vPos.y, vPos.z );	
}


void CActionObject::RotateObject( float fDelta ) 
{	
	float fValue = (float)CGlobalValue::GetInstance().GetFPS() / 60.f * fDelta;

	EtMatrix * matWorld = (EtMatrix *)(&m_matExWorld);		


	//// 회전축좌표이동.
	m_matExWorld.MoveLocalXAxis( m_vAxisPos.x );
	m_matExWorld.MoveLocalYAxis( m_vAxisPos.y );
	m_matExWorld.MoveLocalZAxis( m_vAxisPos.z );
		
	// 회전.
	m_matExWorld.RotateYaw( -(m_fSpeed * fValue) );

	

	//EtVector3 vDist = GetAniDistance();
	//EtVector3 vPos = m_vAxisPos - vDist;		
	////EtVector3 vPos = m_vAxisPos;

	//// 원점이동.
	//EtMatrix matPos;
	//EtMatrixTranslation( &matPos, vPos.x, vPos.y, vPos.z );
	//EtMatrixMultiply( matWorld, matWorld, &matPos );

	//// 회전.
	//EtMatrix matRot;
	//EtMatrixRotationY( &matRot, D3DXToRadian( m_fSpeed * fValue ) );
	////EtMatrixMultiply( matWorld, &matRot, matWorld);
	//*matWorld = matRot;
	//
	////// AniDistance 적용.
	////EtMatrix matPosAni;
	////EtMatrixTranslation( &matPosAni, vDist.x, vDist.y, vDist.z );
	////EtMatrixMultiply( matWorld, matWorld, &matPosAni );
	//




	//EtMatrix * pSaveMat = NULL;

	///*static bool bFirst = true;
	//if( m_bRotate && bFirst )
	//{
	//	bFirst = false;*/

	//	pSaveMat = GetEtSaveMat()->GetMatrix( m_ObjectHandle->GetSaveMatIndex() );
	////}

	//
	//EtVector3 vDist = GetAniDistance();

	//// 애니메이션행렬에서 AniDistance를 제거.	
	//if( pSaveMat )
	//	pSaveMat->_41 = pSaveMat->_42 = pSaveMat->_43 = 0.0f;


	////// 원점이동.
	////EtVector3 vDist = GetAniDistance();
	////EtVector3 vPos = m_vAxisPos - vDist;		
	////
	////EtMatrix matPos;
	////EtMatrixTranslation( &matPos, vPos.x, vPos.y, vPos.z );
	////EtMatrixMultiply( matWorld, matWorld, &matPos );


	//EtMatrix matRot;
	//EtMatrixRotationY( &matRot, D3DXToRadian( m_fSpeed * fValue ) );
	////EtMatrixMultiply( matWorld, matWorld, &matRot );
	//*matWorld = matRot;

}

