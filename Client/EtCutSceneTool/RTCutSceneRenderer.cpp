#include "StdAfx.h"
#include <wx/wx.h>
#include <wx/filename.h>
#include <float.h>
#include "EternityEngine.h"
#include "Timer.h"
#include "EtCamera.h"
#include "Fade.h"
#include "RTCutSceneRenderer.h"
#include "DnCutSceneWorldSector.h"
#include "DnCutSceneActProp.h"

#include "DnCutSceneWorld.h"
#include "DnCutSceneActor.h"
#include "DnCutSceneWeapon.h"
#include "RTToolActorProcessor.h"

#include "CamController.h"
#include "IntersectUtil.h"
#include "ToolData.h"
#include "CommandSet.h"
#include "DnCutScenePlayer.h"
#include "LuaDelegate.h"

#include "EtSoundEngine.h"
#include "EtResourceMng.h"

#include "DnCutSceneTable.h"
#include "AxisRenderObject.h"
#include "EtOptionController.h"
#include "EtUI.h"
#include "BlindCaptionDlg.h"

#include "EtUIXML.h"

#include "RTCutSceneMakerFrame.h"

#include "SundriesFunc.h"
#include "DnCommonUtil.h"



const float SPHERE_RADIUS = 50.0f;
const float SPHERE_SIZE_DIVIDER = 3500.0f;


CRTCutSceneRenderer::CRTCutSceneRenderer( HWND hWnd ) : m_bActivate( false ),
														m_pWorld( new CDnCutSceneWorld ),
														m_pCamController( new CCamController ),
														//m_LocalTime( timeGetTime() ),
														m_iMode( EDITOR_MODE ),
														m_pSprite( NULL ),
														m_pFade( NULL ),
														m_pCutScenePlayer( new CDnCutScenePlayer ),
														m_pActorProcessor( NULL ),
														m_bCamOnPlay( false ),
														m_pSelectedActor( NULL ),
														m_iPrevClkXPos( 0 ),
														m_iPrevClkYPos( 0 ),
														m_pSphereMesh( NULL ),
														m_pSphereBuffer( NULL ),
														m_dwNumTriangle( 0 ),
														m_bEditedKeyInfo( false ),
														m_iSelectedSubKey( -1 ),
														m_bSelectedStartPos( false ),
														m_pSelectedKeyPos( NULL ),
														m_bShowSpotPos( false ),
														m_pSoundEngine( NULL ),
														m_iSelectedObjectType( SO_COUNT ),
														m_iSelectedParticleEventID( -1 ),
														m_bShowMaxCameraPath( false ),
														m_iSelectedCameraEventID( -1 ),
														m_bRenderFog( true ),
														m_pSelectedMaxCamOffset( NULL ),
														m_pPropAxisObject( new CAxisRenderObject ),
														m_bShowAxis( false ),
														m_pDOFFilter( NULL ),
														//m_pCaptionDlg( NULL ),
														m_iWidth( 0 ),
														m_iHeight( 0 ),
														m_bShowSubtitle( false ),
														m_pFont( NULL ),
														m_bShowLetterBox( true ),
														m_bLockSelect(false)														
{
	SGraphicOption Option;
	GetEtOptionController()->GetGraphicOption( Option );
	Option.bEnableDOF = true;
	InitializeEngine( hWnd, 1, 1, true, false, &Option, false );
	CEtRenderStack::EnableDepthRender( true );

	// Sound Device Initialize
	m_pSoundEngine = new CEtSoundEngine;
	m_pSoundEngine->Initialize();
	m_pSoundEngine->CreateChannelGroup( "NULL" );
	m_pSoundEngine->CreateChannelGroup( "3D" );
	m_pSoundEngine->CreateChannelGroup( "2D" );

	CDnCutSceneTable::CreateInstance();
	//CEtFontMng::CreateInstance();

	CEtMaterial::LoadCompiledShaders();

	//CEtFontMng::GetInstance().Initialize( "R:\\GameRes\\Resource\\Fonts\\ui.fontset" );

	//CEtMaterial::LoadCompiledShaders();

	InitilaizeEffect();

	//CEtResourceMng::GetInstance().AddResourcePath( pShaderPath );
	//CEtResourceMng::GetInstance().AddResourcePath( pResourcePath );
	//CEtResourceMng::GetInstance().AddResourcePath( pMapPath, true );
	//AddResourceDir( pSoxPath );

	//AddResourceDir( "./Resource/Char/Player" );
	//AddResourceDir( "./Resource/Char/NPC" );
	//AddResourceDir( "./Resource/Char/Monster" );

	SCameraInfo CamInfo;
	CamInfo.fFar = 100000.0f;
	CamInfo.fFogNear = 100000.0f;
	CamInfo.fFogFar = 100000.0f;

	m_hCamera = CreateCamera( &CamInfo );

	m_pDOFFilter = ( CEtDOFFilter * )EternityEngine::CreateFilter( SF_DOF );

	TOOL_DATA.SetRenderer( this );
	m_pCutScenePlayer->Initialize( TOOL_DATA.GetCoreData() );

	EtInterface::xml::SetXML( &GetEtUIXML() );

	// 축 좌표
	// Make Axis
	m_avAxis[0] = D3DXVECTOR3( -1, 0, 0 );
	m_avAxis[1] = D3DXVECTOR3( 1, 0, 0 );

	m_avAxis[2] = D3DXVECTOR3( 0, -1, 0 );
	m_avAxis[3] = D3DXVECTOR3( 0, 1, 0 );

	m_avAxis[4] = D3DXVECTOR3( 0, 0, -1 );
	m_avAxis[5] = D3DXVECTOR3( 0, 0, 1 );

	m_avAlphabetX[0] = D3DXVECTOR3( -0.1f, -0.1f, 0.f );
	m_avAlphabetX[1] = D3DXVECTOR3( 0.1f, 0.1f, 0.f );
	m_avAlphabetX[2] = D3DXVECTOR3( 0.1f, -0.1f, 0.f );
	m_avAlphabetX[3] = D3DXVECTOR3( -0.1f, 0.1f, 0.f );

	m_avAlphabetY[0] = D3DXVECTOR3( -0.1f, 0.1f, 0.f );
	m_avAlphabetY[1] = D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_avAlphabetY[2] = D3DXVECTOR3( 0.1f, 0.1f, 0.f );
	m_avAlphabetY[3] = D3DXVECTOR3( -0.1f, -0.1f, 0.f );

	m_avAlphabetZ[0] = D3DXVECTOR3( -0.1f, -0.1f, 0.f );
	m_avAlphabetZ[1] = D3DXVECTOR3( 0.1f, -0.1f, 0.f );
	m_avAlphabetZ[2] = D3DXVECTOR3( 0.1f, 0.1f, 0.f );
	m_avAlphabetZ[3] = D3DXVECTOR3( -0.1f, -0.1f, 0.f );
	m_avAlphabetZ[4] = D3DXVECTOR3( -0.1f, 0.1f, 0.f );
	m_avAlphabetZ[5] = D3DXVECTOR3( 0.1f, 0.1f, 0.f );


	// 화면에 편의성으로 보여줄 원 메시 생성 /////////////////////////////////////////////
	LPDIRECT3DDEVICE9 pd3dDevice = (LPDIRECT3DDEVICE9)(GetEtDevice()->GetDevicePtr());
	D3DXCreateSphere( pd3dDevice, SPHERE_RADIUS, 10, 10, &m_pSphereMesh, NULL );
	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
	LPDIRECT3DINDEXBUFFER9 pIB = NULL;
	m_pSphereMesh->GetVertexBuffer( &pVB );
	m_pSphereMesh->GetIndexBuffer( &pIB );
	DWORD dwBytesPerVertex = m_pSphereMesh->GetNumBytesPerVertex();
	DWORD dwNumVertices = m_pSphereMesh->GetNumVertices();
	DWORD dwNumIndices = m_pSphereMesh->GetNumFaces() * 3;

	m_pSphereBuffer = new SPrimitiveDraw3D[ dwNumIndices ];
	m_vlpBufferToDraw.push_back( new SPrimitiveDraw3D[ dwNumIndices ] );

	D3DVERTEXBUFFER_DESC VBDesc;
	pVB->GetDesc( &VBDesc );

	D3DINDEXBUFFER_DESC IBDesc;
	pIB->GetDesc( &IBDesc );

	assert( IBDesc.Format == D3DFMT_INDEX16 );

	BYTE* pVertices = NULL;
	WORD* pIndices = NULL;
	pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_READONLY );
	pIB->Lock( 0, 0, (void**)&pIndices, D3DLOCK_READONLY );
	for( DWORD dwIndex = 0; dwIndex < dwNumIndices; ++dwIndex )
	{
		EtVector3* pvPos = (EtVector3*)(pVertices + (pIndices[dwIndex]*dwBytesPerVertex));
		m_pSphereBuffer[ dwIndex ].Position = *pvPos;
		m_pSphereBuffer[ dwIndex ].dwColor = 0x44ff0000;
	}

	m_dwNumTriangle = dwNumIndices / 3;

	SAFE_RELEASE( pVB );
	SAFE_RELEASE( pIB );
	SAFE_RELEASE( m_pSphereMesh );
	/////////////////////////////////////////////////////////////////////////////////////

	m_pSprite = (LPD3DXSPRITE)GetEtDevice()->CreateSprite();
	m_pFade = new CFade( pd3dDevice, m_pSprite );

	m_pPropAxisObject->Initialize();
	EtMatrix matIdentity;
	EtMatrixIdentity( &matIdentity );
	m_pPropAxisObject->Update( &matIdentity );
	m_pPropAxisObject->Show( false );

	// 축 표시 객체를 풀링함.. 일단 5개 정도만.
	for( int i = 0; i < 5; ++i )
	{
		CAxisRenderObject* pNewAxisObject = new CAxisRenderObject;
		pNewAxisObject->Initialize();

		EtMatrix matIdentity;
		EtMatrixIdentity( &matIdentity );
		pNewAxisObject->Update( &matIdentity );
		pNewAxisObject->Show( false );

		m_dqpAxisPool.push_back( pNewAxisObject );
	}

	// for testing..
	//_LoadActor( "Warrior" );
	//_LoadMap( "Magic_Vill" );
}

CRTCutSceneRenderer::~CRTCutSceneRenderer(void)
{	
	m_mapLockSelectedActor.clear();
	map<KeyInfo*, CAxisRenderObject*>::iterator it = m_mapKeyInfoAxisObject.begin();
	for( ; it != m_mapKeyInfoAxisObject.end(); ++it )
		delete it->second;

	CDnCutSceneTable::DestroyInstance();
	//CEtFontMng::DestroyInstance();

	int iNumAxsisRender = (int)m_dqpAxisPool.size();
	for( int i = 0; i < iNumAxsisRender; ++i )
	{
		delete m_dqpAxisPool.at( i );
	}

	map<CDnCutSceneActor*, CAxisRenderObject*>::iterator iter = m_mapActorAxisObject.begin();
	for( iter; iter != m_mapActorAxisObject.end(); ++iter )
		delete iter->second;

	SAFE_RELEASE( m_pFont );

	SAFE_DELETE( m_pPropAxisObject );

	SAFE_DELETEA( m_pSphereBuffer );
	
	int iNumBuffers = (int)m_vlpBufferToDraw.size();
	for( int i = 0; i < iNumBuffers; ++i )
		SAFE_DELETEA( m_vlpBufferToDraw.at( i ) );

	//SAFE_RELEASE( m_pSphereMesh );

	//SAFE_DELETE( m_pCaptionDlg );

	SAFE_DELETE( m_pActorProcessor );

	SAFE_RELEASE( m_pSprite );

	SAFE_DELETE( m_pFade );

	m_pWorld->Destroy();
	SAFE_DELETE( m_pWorld );
	//CEtResource::DeleteAllObject();

	SAFE_DELETE( m_pCutScenePlayer );

	for_each( m_vlpActors.begin(), m_vlpActors.end(), DeleteData<CDnCutSceneActor*>() );

	SAFE_DELETE( m_pCamController );

	SAFE_DELETE( m_pDOFFilter );

	if( m_pSoundEngine ) 
	{
		m_pSoundEngine->Finalize();
		SAFE_DELETE( m_pSoundEngine );
	}

	FinalizeEngine();
}


CAxisRenderObject* CRTCutSceneRenderer::_GetAxisRenderObject( void )
{
	if( m_dqpAxisPool.empty() )
	{
		CAxisRenderObject* pNewAxisObject = new CAxisRenderObject;
		pNewAxisObject->Initialize();

		EtMatrix matIdentity;
		EtMatrixIdentity( &matIdentity );
		pNewAxisObject->Update( &matIdentity );

		m_dqpAxisPool.push_back( pNewAxisObject );
	}

	CAxisRenderObject* pAxisObject = m_dqpAxisPool.front();
	pAxisObject->Show( true );

	m_dqpAxisPool.pop_front();

	return pAxisObject;
}



void CRTCutSceneRenderer::_ReturnAxisRenderObject( CAxisRenderObject* pAxisRenderObject )
{
	EtMatrix matIdentity;
	EtMatrixIdentity( &matIdentity );
	pAxisRenderObject->Update( &matIdentity );

	pAxisRenderObject->Show( false );

	m_dqpAxisPool.push_back( pAxisRenderObject );
}


void CRTCutSceneRenderer::OnResize( int iWidth, int iHeight )
{
	// 디바이스 소실 및 화면 리사이즈
	ReinitializeEngine( iWidth, iHeight );

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	if( m_pSprite )
	{
		m_pSprite->OnLostDevice();
	}

	SAFE_RELEASE( m_pFont );
	D3DXCreateFont( (LPDIRECT3DDEVICE9)(GetEtDevice()->GetDevicePtr()), 0, 7, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
					OUT_DEFAULT_PRECIS, 5, DEFAULT_PITCH | FF_DONTCARE, L"굴림체", &m_pFont );

	//m_pFont->OnLostDevice();
	//m_pFont->OnResetDevice();

	CEtDevice* pDevice = GetEtDevice();
	EtViewPort Viewport;
	pDevice->GetViewport( &Viewport );

	m_pCamController->SetViewport( Viewport );

	EtVector3 vCamPos = m_pCamController->GetCamPos();
	EtVector3 vLookAt = m_pCamController->GetLookAt();
	EtVector3 vUp = m_pCamController->GetUpVector();

	m_hCamera->LookAt( vCamPos, vLookAt, vUp );
}




void CRTCutSceneRenderer::_FillActorsAniElementData( CDnCutSceneActor* pActor )
{
	TOOL_DATA.ClearActorsAnimations( pActor->GetName() );

	DWORD dwNumAni = pActor->GetElementCount();
	for( DWORD dwAni = 0; dwAni < dwNumAni; ++dwAni )
	{
		ActionEleInfo* pAniElement = pActor->GetElement( dwAni );
		bool bResult = TOOL_DATA.AddActorsAnimation( pActor->GetName(), pAniElement );
		assert( bResult );
	}
}


shared_ptr<CDnCutSceneWeapon> 
CRTCutSceneRenderer::_LoadWeaponFromResource( int iWeaponTableID )
{
	DNTableFileFormat* pWeaponTable = TOOL_DATA.GetWeaponTable();
	if( !pWeaponTable->IsExistItem( iWeaponTableID ) ) 
		return shared_ptr<CDnCutSceneWeapon>();

	DNTableFileFormat* pFileTable = TOOL_DATA.GetFileTable();

	shared_ptr<CDnCutSceneWeapon> pNewWeapon = shared_ptr<CDnCutSceneWeapon>(new CDnCutSceneWeapon);
	pNewWeapon->SetResPathFinder( &TOOL_DATA );

	//string strSkinFileName = pWeaponTable->GetFieldFromLablePtr( iWeaponTableID, "_SkinName" )->GetString();
	//string strAniFileName = pWeaponTable->GetFieldFromLablePtr( iWeaponTableID, "_AniName" )->GetString();
	//string strActFileName = pWeaponTable->GetFieldFromLablePtr( iWeaponTableID, "_ActName" )->GetString();

	string strSkinFileName = CommonUtil::GetFileNameFromFileEXT( pWeaponTable, iWeaponTableID, "_SkinName", pFileTable );
	string strAniFileName = CommonUtil::GetFileNameFromFileEXT( pWeaponTable, iWeaponTableID, "_AniName", pFileTable );
	string strActFileName = CommonUtil::GetFileNameFromFileEXT( pWeaponTable, iWeaponTableID, "_ActName", pFileTable );

	if( strSkinFileName.empty() )
		return shared_ptr<CDnCutSceneWeapon>();

	wxCSConv MBConv( wxFONTENCODING_CP949 );

	string strSkinFilePath;
	wxChar awcSkinFileName[ MAX_PATH ] = { 0 };
	MBConv.MB2WC( awcSkinFileName, strSkinFileName.c_str(), MAX_PATH );
	wxString wstrSkinFileName( awcSkinFileName );
	wstrSkinFileName.MakeLower();
	TOOL_DATA.GetFullPathA( wstrSkinFileName.c_str(), strSkinFilePath );

	string strAniFilePath;
	wxChar awcAniFileName[ MAX_PATH ] = { 0 };
	MBConv.MB2WC( awcAniFileName, strAniFileName.c_str(), MAX_PATH );
	wxString wstrAniFileName( awcAniFileName );
	wstrAniFileName.MakeLower();
	TOOL_DATA.GetFullPathA( wstrAniFileName.c_str(), strAniFilePath );

	pNewWeapon->LoadSkin( strSkinFilePath.c_str(), strAniFilePath.c_str() );

	string strActFilePath;
	wxChar awcActFileName[ MAX_PATH ] = { 0 };
	MBConv.MB2WC( awcActFileName, strActFileName.c_str(), MAX_PATH );
	wxString wstrActFileName( awcActFileName );
	wstrActFileName.MakeLower();
	TOOL_DATA.GetFullPathA( wstrActFileName.c_str(), strActFilePath );
	pNewWeapon->LoadAction( strActFilePath.c_str() );

	pNewWeapon->SetEquipType( pWeaponTable->GetFieldFromLablePtr( iWeaponTableID, "_EquipType" )->GetInteger() );

	return pNewWeapon;
}



CDnCutSceneActor* CRTCutSceneRenderer::_LoadActorFromResource( const char* pActorResName )
{
	CDnCutSceneActor* pNewActor = new CDnCutSceneActor;
	pNewActor->SetResPathFinder( &TOOL_DATA );

	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar wcaActorResName[ MAX_PATH ];
	ZeroMemory( wcaActorResName, sizeof(wcaActorResName) );
	MBConv.MB2WC( wcaActorResName, pActorResName, 256 );

	const S_ACTOR_RES_INFO* pActorResInfo = TOOL_DATA.GetActorResInfoByName( wcaActorResName );

	string strSkinFilePath;
	TOOL_DATA.GetFullPathA( pActorResInfo->strSkinFileName, strSkinFilePath );

	string strAniFilePath;
	TOOL_DATA.GetFullPathA( pActorResInfo->strAniFileName, strAniFilePath );
	pNewActor->LoadSkin( strSkinFilePath.c_str(), strAniFilePath.c_str() );

	string strActionFilePath;
	TOOL_DATA.GetFullPathA( pActorResInfo->strActionFileName, strActionFilePath );
	pNewActor->LoadAction( strActionFilePath.c_str() );

	// 타입 셋팅
	DNTableFileFormat* pActorTable = TOOL_DATA.GetActorTable();
	DNTableFileFormat* pSox = NULL;
	int iActorExtentionTableID = 0;
	int iActorTypeInTable = pActorTable->GetFieldFromLablePtr( pActorResInfo->iTableID, "_Class" )->GetInteger();
	
	if( iActorTypeInTable < CDnCutSceneActor::CHARACTER_TYPE_BOUND )
	{
		pNewActor->SetActorType( CDnCutSceneActor::PLAYER );

		// 플레이어무기설정 - 상점용 1레벨 무기들 .bintitle.
		int weaponIdx[] = { 269486016, 271058880, 275777472, 273156032, 290457536, 268438576 }; // WARRIOR, ARCHER, SOCERESS, CLERIC, ACADEMIC, Kali.
		if( iActorTypeInTable < CDnCutSceneActor::PLAYER_MAX )
		{
			shared_ptr<CDnCutSceneWeapon> pWeapon = _LoadWeaponFromResource( weaponIdx[ iActorTypeInTable ] );
			pWeapon->Show( false );	
			pNewActor->AttachWeapon( pWeapon, 0 );			
		}
	}
	else
	if( iActorTypeInTable < 100 )
	{
		pNewActor->SetActorType( CDnCutSceneActor::MONSTER );

		pSox = TOOL_DATA.GetMonsterTable();
		vector<int> vlItemIDs;
		pSox->GetItemIDListFromField( "_ActorTableID", pActorResInfo->iTableID, vlItemIDs );
		//_ASSERT( false == vlItemIDs.empty() );
		if( false == vlItemIDs.empty() )
			iActorExtentionTableID = vlItemIDs.front();
	}
	else
	{
		pNewActor->SetActorType( CDnCutSceneActor::NPC );

		pSox = TOOL_DATA.GetNPCTable();
		vector<int> vlItemIDs;
		pSox->GetItemIDListFromField( "_ActorIndex", pActorResInfo->iTableID, vlItemIDs );
		//_ASSERT( false == vlItemIDs.empty() );
		if( false == vlItemIDs.empty() )
			iActorExtentionTableID = vlItemIDs.front();
	}

	// 일단 몬스터/NPC 인 경우만 무기를 들려준다.
	if( pSox && iActorExtentionTableID != 0 )
	{
		// 몬스터/NPC 별로 무기를 갖고 있다면 무기를 달아준다.
		char szWeaponLable[32];
		for( int j = 0; j < 2; j++ )
		{
			sprintf_s( szWeaponLable, "_Weapon%dIndex", j + 1 );

			int iWeaponTableID = pSox->GetFieldFromLablePtr( iActorExtentionTableID, szWeaponLable )->GetInteger();
			if( iWeaponTableID < 1 )
				continue;

			//shared_ptr<CDnCutSceneWeapon> pWeapon = CDnCutSceneWeapon::Create( iWeaponTableID );
			shared_ptr<CDnCutSceneWeapon> pWeapon = _LoadWeaponFromResource( iWeaponTableID );
			pNewActor->AttachWeapon( pWeapon, j );
		}
	}

	return pNewActor;
}


CDnCutSceneActor* CRTCutSceneRenderer::_CreateActor( const wchar_t* pActorName, const char* pActorResName )
{
	CDnCutSceneActor* pNewActor = _LoadActorFromResource( pActorResName );
	if( pNewActor )
	{
		pNewActor->SetName( pActorName );
		_FillActorsAniElementData( pNewActor );

		// 가운데 지점에 셋팅
		EtVector3 vLookAt = m_pCamController->GetLookAt();
		float fLookAtHeight = m_pWorld->GetHeight( vLookAt.x, vLookAt.z );
		EtVector3 vPos( vLookAt.x, fLookAtHeight, vLookAt.z );
		pNewActor->SetPosition( vPos );

		// 액터들을 컬링을 꺼줌
		pNewActor->GetAniObjectHandle()->EnableCull( false );
		pNewActor->GetAniObjectHandle()->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 3 ) );

		return pNewActor;
	}

	return NULL;
}


void CRTCutSceneRenderer::_LoadActor( const wchar_t* pActorName, const char* pActorResName )
{
	CDnCutSceneActor* pNewActor = _CreateActor( pActorName, pActorResName );
	if( pNewActor )
	{
		m_setActors.insert( pActorName );
		m_vlpActors.push_back( pNewActor );
		m_mapActors.insert( make_pair(tstring(pActorName), pNewActor) );
	}
}




void CRTCutSceneRenderer::_LoadMap( const char* pMapName, const char* pEnvFilePath )
{
	const char* pMapPath = LUA_DELEGATE.GetString( "mapdata_path" );
	m_pWorld->SetResPathFinder( &TOOL_DATA );
	bool bResult = m_pWorld->Initialize( pMapPath, pMapName );
	assert( bResult && "맵 로딩 실패!" );

	// 실제로 맵 로딩
	int nLoadEnum = CEtWorldSector::LSE_Terrain | CEtWorldSector::LSE_Prop | CEtWorldSector::LSE_Water | CEtWorldSector::LSE_Decal;
	m_pWorld->ForceInitialize( nLoadEnum );
	/*
	m_pWorld->Update( EtVector3( 0.f, 0.f, 0.f ), EtVector3( 0.f, 0.f, 1.f ), 10000.f );
	m_pWorld->Process( 0, 0.0f );
	*/


	// 환경 셋팅
	m_pWorld->InitializeEnvironment( pEnvFilePath, pMapPath );

	// 액션 프랍들을 데이터에 넣어준다.
	CDnCutSceneWorldSector* pSector = static_cast<CDnCutSceneWorldSector*>(m_pWorld->GetSector( 0.0f, 0.0f ));
	TOOL_DATA.ClearPropInfo();
	int iNumActionProp = pSector->GetNumActionProp();
	for( int iProp = 0; iProp < iNumActionProp; ++iProp )
	{
		CDnCutSceneActProp* pProp = static_cast<CDnCutSceneActProp*>(pSector->GetActionProp( iProp ));
		S_PROP_INFO PropInfo;

		const char* pPropName = pProp->GetPropName();
		wxCSConv MBConv( wxFONTENCODING_CP949 );
		wxChar wcaBuf[ 256 ];
		ZeroMemory( wcaBuf, sizeof(wcaBuf) );
		MBConv.MB2WC( wcaBuf, pPropName, strlen(pPropName) );

		PropInfo.strPropName.assign( wcaBuf );
		PropInfo.iPropID = pProp->GetCreateUniqueID();
		int iNumAction = pProp->GetElementCount();
		for( int iAction = 0; iAction < iNumAction; ++iAction )
		{
			CEtActionBase::ActionElementStruct* pElement = pProp->GetElement( iAction );
			wxCSConv MBConv( wxFONTENCODING_CP949 );
			ZeroMemory( wcaBuf, sizeof(wcaBuf) );
			MBConv.MB2WC( wcaBuf, pElement->szName.c_str(), (int)pElement->szName.length() );
			PropInfo.strActionList.push_back( wcaBuf );
			PropInfo.vldwLength.push_back( pElement->dwLength );
		}

		TOOL_DATA.AddActionPropInfo( PropInfo );
	}

	// 카메라 초기값 설정.
	float fMapCenterX = 0.0f;
	float fMapCenterZ = 0.0f;
	float fMapCenterY = m_pWorld->GetHeight( fMapCenterX, fMapCenterZ );
	EtVector3 vCamPos( fMapCenterX, fMapCenterY+100.0f, fMapCenterZ-300.0f );
	EtVector3 vLookAt( fMapCenterX, fMapCenterY, fMapCenterZ );
	EtVector3 vUp( 0.0f, 1.0f, 0.0f );

	m_pCamController->SetOriCamPos( vCamPos );
	m_pCamController->SetOriLookAt( vLookAt );
	m_pCamController->SetOriUpVector( vUp );

	if( m_bRenderFog )
	{
		SAFE_RELEASE_SPTR( m_hCamera );

		SCameraInfo CamInfo;
		CamInfo.fFar = m_pWorld->GetFogFar() + 200000.f; // 임시 20미터
		CamInfo.fFogNear = m_pWorld->GetFogNear();
		CamInfo.fFogFar = m_pWorld->GetFogFar();
		EtColor FogColor = m_pWorld->GetFogColor();		
		CamInfo.FogColor = EtColor( FogColor.b, FogColor.g, FogColor.r, 1.f );

		m_hCamera = CreateCamera( &CamInfo );

		m_hCamera->LookAt( vCamPos, vLookAt, vUp );
	}

	// DOF 필터 설정. 맵에서 갖고 온다.
	if( m_pWorld->IsEnableDOF() )
	{
		 m_pDOFFilter->SetNearDOFStart( m_pWorld->GetDOFNearStart() );
		 m_pDOFFilter->SetNearDOFEnd( m_pWorld->GetDOFNearEnd() );
		 m_pDOFFilter->SetFarDOFStart( m_pWorld->GetDOFFarStart() );
		 m_pDOFFilter->SetFarDOFEnd( m_pWorld->GetDOFFarEnd() );

		 m_pDOFFilter->SetFocusDistance( m_pWorld->GetDOFFocusDistance() );
		 m_pDOFFilter->SetNearBlurSize( m_pWorld->GetDOFNearBlur() );
		 m_pDOFFilter->SetFarBlurSize( m_pWorld->GetDOFFarBlur() );

		 m_pDOFFilter->Enable( true );
	}
	else
		m_pDOFFilter->Enable( false );

	m_matDefaultProj = *(m_hCamera->GetProjMat());
}




void CRTCutSceneRenderer::_UpdateMap( void )
{
	wxString strRegisteredMapName;

	int iNumRes = TOOL_DATA.GetNumRegRes();
	for( int iRes = 0; iRes < iNumRes; ++iRes )
	{
		int iResourceKind = TOOL_DATA.GetRegResKindByIndex( iRes );
		if( CDnCutSceneData::RT_RES_MAP == iResourceKind )
		{
			TOOL_DATA.GetRegResNameByIndex( iRes, strRegisteredMapName );
			if( m_strNowMapName != strRegisteredMapName )
			{
				wxCSConv MBConv( wxFONTENCODING_CP949 );
				char caBuf[ 256 ];
				MBConv.WC2MB( caBuf, strRegisteredMapName.c_str(), 256 );

				if( m_pWorld )
					m_pWorld->Destroy();

				const wxChar* pEnvFileName = TOOL_DATA.GetEnvFileName( strRegisteredMapName );
				wxString strEnvFileName( pEnvFileName );
				strEnvFileName.MakeLower();

				string strEnvFilePath;
				TOOL_DATA.GetFullPathA( strEnvFileName.c_str(), strEnvFilePath );
				_LoadMap( caBuf, strEnvFilePath.c_str() );
				m_strNowMapName.assign( strRegisteredMapName.c_str() );
				return;
			}
		}
	}

	// 맵이 없으면 현재 맵 해제
	if( strRegisteredMapName.empty() )
	{
		m_strNowMapName.clear();
		m_pWorld->Destroy();
		SAFE_DELETE( m_pWorld );
		m_pWorld = new CDnCutSceneWorld;
	}
}



void CRTCutSceneRenderer::_UpdateActors( void )
{
	wxString strNowRegisteredResName;
	wxString strActorResName;

	// 새롭게 업데이트 된 액터 모음
	set<tstring> setNewActors;
	int iNumRes = TOOL_DATA.GetNumRegRes();
	for( int iRes = 0; iRes < iNumRes; ++iRes )
	{
		TOOL_DATA.GetRegResNameByIndex( iRes, strNowRegisteredResName );
		int iOffset = (int)strNowRegisteredResName.find_last_of( wxT("_"), strNowRegisteredResName.length() );
		strActorResName = strNowRegisteredResName.substr( 0, iOffset );

		if( CDnCutSceneData::RT_RES_ACTOR == TOOL_DATA.GetResourceKind( strActorResName ) )
			setNewActors.insert( tstring(strNowRegisteredResName.c_str()) );
	}

	tstring strUpdatedActorResName;

	vector<tstring> vlDifference;
	set_difference( setNewActors.begin(), setNewActors.end(), m_setActors.begin(), m_setActors.end(), back_inserter(vlDifference) );

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	char caBuf[ 256 ];

	int iSize = (int)vlDifference.size();
	for( int i = 0; i < iSize; ++i )
	{
		strUpdatedActorResName = vlDifference.at( i );
		int iOffset = (int)strUpdatedActorResName.find_last_of( wxT("_"), strUpdatedActorResName.length() );
		wxString strActorResName = strUpdatedActorResName.substr( 0, iOffset );

		MBConv.WC2MB( caBuf, strActorResName.c_str(), 256 );
		_LoadActor( strUpdatedActorResName.c_str(), caBuf );
	}

	vlDifference.clear();

	set_difference( m_setActors.begin(), m_setActors.end(), setNewActors.begin(), setNewActors.end(), back_inserter(vlDifference) );

	iSize = (int)vlDifference.size();
	for( int i = 0; i < iSize; ++i )
	{
		strUpdatedActorResName = vlDifference.at( i );

		set<tstring>::iterator iter = m_setActors.find( strUpdatedActorResName );
		m_setActors.erase( iter );

		map<tstring, CDnCutSceneActor*>::iterator iterMap = m_mapActors.find( strUpdatedActorResName );
		vector<CDnCutSceneActor*>::iterator iterVec = find( m_vlpActors.begin(), m_vlpActors.end(), iterMap->second );

		// 현재 선택되어있는 액터이거나 키가 있는 녀석이면 초기화
		_DeSelectActor();

		MBConv.WC2MB( caBuf, strUpdatedActorResName.c_str(), 256 );

		if( m_SelectedKeyInfo.strActorName == caBuf )
		{
			m_SelectedKeyInfo.strActorName.clear();
			m_bEditedKeyInfo = false;
			m_bSelectedStartPos = false;
			m_iSelectedSubKey = -1;
			m_pSelectedKeyPos = NULL;
		}

		delete iterMap->second;
		m_vlpActors.erase( iterVec );
		m_mapActors.erase( iterMap );
	}

	// 기존의 액터들의 값을 업데이팅 이부분도 좀 손봐야 겠네
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	const ActionInfo* pSelectedActionInfo = TOOL_DATA.GetActionInfoByID( iSelectedObjectID );
	tstring strSelectedActionsActor;
	if( pSelectedActionInfo )
	{
		wxCSConv MBConv( wxFONTENCODING_CP949 );
		wchar_t wcaBuf[ 256 ];
		ZeroMemory( wcaBuf, sizeof(wcaBuf) );
		MBConv.MB2WC( wcaBuf, pSelectedActionInfo->strActorName.c_str(), 256 );
		strSelectedActionsActor.assign( wcaBuf );
	}
	else
	{
		const KeyInfo* pSelectedKeyInfo = TOOL_DATA.GetKeyInfoByID( iSelectedObjectID );
		if( pSelectedKeyInfo )
		{
			wxCSConv MBConv( wxFONTENCODING_CP949 );
			wchar_t wcaBuf[ 256 ];
			ZeroMemory( wcaBuf, sizeof(wcaBuf) );
			MBConv.MB2WC( wcaBuf, pSelectedKeyInfo->strActorName.c_str(), 256 );
			strSelectedActionsActor.assign( wcaBuf );
		}
	}

	iNumRes = TOOL_DATA.GetNumRegRes();
	for( int iRes = 0; iRes < iNumRes; ++iRes )
	{
		TOOL_DATA.GetRegResNameByIndex( iRes, strNowRegisteredResName );
		int iOffset = (int)strNowRegisteredResName.find_last_of( wxT("_"), strNowRegisteredResName.length() );

		strActorResName = strNowRegisteredResName.substr( 0, iOffset );
		if( CDnCutSceneData::RT_RES_ACTOR == TOOL_DATA.GetResourceKind( strActorResName ) )
		{
			map<tstring, CDnCutSceneActor*>::iterator iter = m_mapActors.find( tstring(strNowRegisteredResName.c_str()) );
			if( m_mapActors.end() != iter )
			{
				CDnCutSceneActor* pActor = iter->second;
				EtVector3 vPos = TOOL_DATA.GetRegResPos( strNowRegisteredResName.c_str() );
				float fRotation = TOOL_DATA.GetRegResRot( strNowRegisteredResName.c_str() );
				bool bFitYPosToMap = TOOL_DATA.GetThisActorsFitYPosToMap( strNowRegisteredResName.c_str() );
				if( bFitYPosToMap && m_pWorld )
				{
					 vPos.y = m_pWorld->GetHeight( vPos.x, vPos.z );
					 TOOL_DATA.SetRegResPos( strNowRegisteredResName.c_str(), vPos );
				}

				pActor->SetPosition( vPos );
				//pKeyObject->pAniObjectInfoToPlay->crossPos.Reset();
				pActor->SetRotationY( fRotation );
				//float fDegree = fRot * (180.0f/ET_PI); //(fRot - ET_PI/2.0f) * (180.0f/ET_PI);
				//pKeyObject->pAniObjectInfoToPlay->crossPos.RotateYaw( fDegree );
				
				// TODO: 회전값도 먹도록
				if( pSelectedActionInfo )
				{
					if( strSelectedActionsActor == strNowRegisteredResName )
					{
						pActor->SetAction( pSelectedActionInfo->iAnimationIndex );
					}
				}
			}
		}
	}
}




void CRTCutSceneRenderer::UpdateRenderObject( void )
{
	_UpdateMap();
	_UpdateActors();
	//_SoundUpdate();
}


void CRTCutSceneRenderer::SetAnimation( const wchar_t* pActorName, int iAnimationIndex )
{
	map<tstring,CDnCutSceneActor*>::iterator iter = m_mapActors.find( tstring(pActorName) );
	if( m_mapActors.end() != iter )
	{
		CDnCutSceneActor* pActor = iter->second;
		pActor->SetAction( iAnimationIndex );
	}
}


void CRTCutSceneRenderer::_InitToPlay( void )
{
	m_pFade->Reset();

	SAFE_RELEASE_SPTR( m_hSelectedParticleEvent );

	// 액션들을 등록한다
	m_pCutScenePlayer->Clear();

	SAFE_DELETE( m_pActorProcessor );
	m_pActorProcessor = new CRTToolActorProcessor;
	static_cast<CRTToolActorProcessor*>(m_pActorProcessor)->SetFadeObject( m_pFade );

	DNTableFileFormat* pActorTable = TOOL_DATA.GetActorTable();
	DNTableFileFormat* pSox = NULL;

	const SceneInfo* pSceneInfo = TOOL_DATA.GetSceneInfo();

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	char caBuf[ 256 ];
	int iNumActor = TOOL_DATA.GetNumActors();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		wxString strActorName;
		TOOL_DATA.GetActorNameByIndex( iActor, strActorName );

		// 이 컷신에서 미지원 액터라면 패스.
		wxString strActorResName = strActorName.substr( 0, strActorName.find_last_of(wxT("_")) );
		const S_ACTOR_RES_INFO* pActorResInfo = TOOL_DATA.GetActorResInfoByName( strActorResName );
		int iActorTypeInTable = pActorTable->GetFieldFromLablePtr( pActorResInfo->iTableID, "_Class" )->GetInteger();
		const SceneInfo* pSceneInfo = TOOL_DATA.GetSceneInfo();
		if( pSceneInfo->bAcademicNotSupported &&
			CDnCutSceneActor::ACADEMIC == iActorTypeInTable )
		{
			// 아카데믹 캐릭터는 지원하지 않음. 패스.
			wxMessageBox( wxT("아카데믹 캐릭터는 이 컷신에서 지원하지 않도록 셋팅되어있습니다. 확인해 주시기 바랍니다."), wxT("아카데믹 미지원"), 
						  wxOK|wxCENTRE|wxICON_INFORMATION, TOOL_DATA.GetMainFrame() );
			continue;
		}

		ZeroMemory( caBuf, sizeof(caBuf) );
		MBConv.WC2MB( caBuf, strActorName.c_str(), 256 );

		//CDnCutScenePlayer::S_ACTOR_OBJECT_INFO* pNewActorToPlay = new CDnCutScenePlayer::S_ACTOR_OBJECT_INFO;
		S_CS_ACTOR_INFO* pNewActorToPlay = new S_CS_ACTOR_INFO;

		map<tstring, CDnCutSceneActor*>::iterator iter = m_mapActors.find( tstring( strActorName.c_str() ) );
		assert( m_mapActors.end() != iter );
		CDnCutSceneActor* pNowDnCutSceneActor = iter->second;

		pNowDnCutSceneActor->SetUseSignalSound( pSceneInfo->bUseSignalSound );

		pNewActorToPlay->pActor = pNowDnCutSceneActor;
		pNewActorToPlay->hObject = pNowDnCutSceneActor->GetAniObjectHandle();
		pNewActorToPlay->strActorName.assign( caBuf );
		pNewActorToPlay->vPos = *(pNowDnCutSceneActor->GetPosition());
		pNewActorToPlay->fRotYDegree = pNowDnCutSceneActor->GetRotationY();
		EtMatrixRotationY( &pNewActorToPlay->matRotY, pNewActorToPlay->fRotYDegree*ET_PI / 180.0f );
		pNowDnCutSceneActor->GetAniIndexList( pNewActorToPlay->vlAniIndex );	// 이건 커스텀 Actor Processor 에서 맡아서 하는게 맞다.
		pNewActorToPlay->iNowAniIndex = 0;					// 애니메이션 인덱스를 0으로 초기화

		// #54681 - 플레이어캐릭터의 Hide처리.
		if( pNowDnCutSceneActor->GetActorType() == CDnCutSceneActor::PLAYER )
			pNewActorToPlay->bPlayer = true;

		// 액터들을 컷신 플레이어에 등록한다.
		m_pActorProcessor->AddObjectToPlay( pNewActorToPlay );

		delete pNewActorToPlay;
	}

	// 원래 프랍의 액션을 저장해둔다.'
	if( m_pWorld )
	{
		m_vlDefaultActionName.clear();
		CDnCutSceneWorldSector* pSector = static_cast<CDnCutSceneWorldSector*>(m_pWorld->GetSector( 0.0f, 0.0f ));
		if( pSector )
		{
			int iNumActionProp = pSector->GetNumActionProp();
			for( int iProp = 0; iProp < iNumActionProp; ++iProp )
			{
				CDnCutSceneActProp* pProp = static_cast<CDnCutSceneActProp*>(pSector->GetActionProp( iProp ));

				if( pProp->GetCurrentAction() )
					m_vlDefaultActionName.push_back( string(pProp->GetCurrentAction()) );
				else
					m_vlDefaultActionName.push_back( string() );
			}
		}
	}

	// 카메라 물려주고
	m_pCutScenePlayer->SetCamera( m_hCamera );

	// DOF 필터
	m_pCutScenePlayer->SetDOFFilter( m_pDOFFilter );

	// 월드 객체도 물려준다. CEtWorld 타입으로 넘기면 된다.
	m_pCutScenePlayer->SetWorld( m_pWorld );

	m_pCutScenePlayer->MakeUpSequences( m_pActorProcessor );
	m_pCutScenePlayer->StartPlay( 0 );
}








void CRTCutSceneRenderer::SetMode( int iMode )
{
	int iPrevMode = m_iMode;
	m_iMode = iMode;

	switch( iMode )
	{
		case PLAY_MODE:
		case FREE_TIME_MODE:
			{
				_InitToPlay();
				m_pPropAxisObject->Show( false );
				
				if( iMode == FREE_TIME_MODE )
					CEtSoundEngine::GetInstance().SetMute( "NULL", true );
				else
					CEtSoundEngine::GetInstance().SetMute( "NULL", false );
			}
			break;
		
		case EDITOR_MODE:
			{
				// 편집모드로 다시 돌려놓는다.
				// FREE_TIME 에서 이동했을 경우엔 포지션을 복사해둔다. 
				// 아닌 경우는 CDnCutSceneActor 의 crossPos 변수가 자동으로 박히기 때문에 알아서 제자리로 돌아온다.
				if( FREE_TIME_MODE == iPrevMode )
				{
					wxCSConv MBConv( wxFONTENCODING_CP949 );
					char caBuf[ 256 ];

					int iNumActors = TOOL_DATA.GetNumActors();
					for( int iActor = 0; iActor < iNumActors; ++iActor )
					{
						wxString strActorName;
						TOOL_DATA.GetActorNameByIndex( iActor, strActorName );

						map<tstring, CDnCutSceneActor*>::iterator iter = m_mapActors.find( strActorName.c_str() );
						assert( m_mapActors.end() != iter );
						CDnCutSceneActor* pNowActor = iter->second;

						// TOOD: 회전은 나중에..
						ZeroMemory( caBuf, sizeof(caBuf) );
						MBConv.WC2MB( caBuf, strActorName.c_str(), 256 );
						//MatrixEx vCross = m_pCutScenePlayer->GetActorMatrixEx( caBuf );
						S_CS_ACTOR_INFO* pActorInfo = m_pActorProcessor->GetActorInfoByName( caBuf );
						pNowActor->SetPosition( pActorInfo->matExFinal.GetPosition() );
					}
				}
				else
				{
					int iNumActors = TOOL_DATA.GetNumActors();
					for( int iActor = 0; iActor < iNumActors; ++iActor )
					{
						wxString strActorName;
						TOOL_DATA.GetActorNameByIndex( iActor, strActorName );

						map<tstring, CDnCutSceneActor*>::iterator iter = m_mapActors.find( strActorName.c_str() );
						assert( m_mapActors.end() != iter );
						CDnCutSceneActor* pNowActor = iter->second;

						EtVector3 vOriPos = TOOL_DATA.GetRegResPos( strActorName.c_str() );
						float fOriRot = TOOL_DATA.GetRegResRot( strActorName.c_str() );

						pNowActor->SetPosition( vOriPos );
						pNowActor->Show( true );

						// 액션 프랍들은 원래 액션으로 셋팅해준다.
						if( m_pWorld )
						{
							CDnCutSceneWorldSector* pSector = static_cast<CDnCutSceneWorldSector*>(m_pWorld->GetSector( 0.0f, 0.0f ));
							int iNumActionProp = pSector->GetNumActionProp();
							for( int iProp = 0; iProp < iNumActionProp; ++iProp )
							{
								CDnCutSceneActProp* pProp = static_cast<CDnCutSceneActProp*>(pSector->GetActionProp( iProp ));

								if( false == m_vlDefaultActionName.at(iProp).empty() )
									pProp->SetAction( m_vlDefaultActionName.at(iProp).c_str(), 0.0f, 0.0f );
							}
						}
					}
				}

				// 재생중인 사운드 정지
				m_pCutScenePlayer->ResetPlayingSoundChannels();

				int iNumActor = (int)m_vlpActors.size();
				for( int iActor = 0; iActor < iNumActor; ++iActor )
				{
					CDnCutSceneActor* pActor = m_vlpActors.at( iActor );
					pActor->OnEndCutScene();
				}

				// 카메라를 원래대로 돌려놓음
				EtVector3 vCamPos = m_pCamController->GetCamPos();
				EtVector3 vLookAt = m_pCamController->GetLookAt();
				EtVector3 vUp = m_pCamController->GetUpVector();

				// DOF 필터 설정. 맵에서 갖고 온다.
				if( m_pWorld && m_pWorld->IsEnableDOF() )
				{
					m_pDOFFilter->SetNearDOFStart( m_pWorld->GetDOFNearStart() );
					m_pDOFFilter->SetNearDOFEnd( m_pWorld->GetDOFNearEnd() );
					m_pDOFFilter->SetFarDOFStart( m_pWorld->GetDOFFarStart() );
					m_pDOFFilter->SetFarDOFEnd( m_pWorld->GetDOFFarEnd() );

					m_pDOFFilter->SetFocusDistance( m_pWorld->GetDOFFocusDistance() );
					m_pDOFFilter->SetNearBlurSize( m_pWorld->GetDOFNearBlur() );
					m_pDOFFilter->SetFarBlurSize( m_pWorld->GetDOFFarBlur() );
				}
				else
					m_pDOFFilter->Enable( false );

				m_hCamera->LookAt( vCamPos, vLookAt, vUp );
				m_hCamera->SetProjMat( m_matDefaultProj );

				if( m_pWorld )
				{
					//PRE_ADD_FILTEREVENT
					m_pWorld->SetSceneAbsoluteColor( false, EtVector3(1.0f,1.0f,1.0f), 1.0f );
					//

					CDnCutSceneWorldSector* pSector = static_cast<CDnCutSceneWorldSector*>(m_pWorld->GetSector( 0.0f, 0.0f ));
					if( pSector )
					{
						int iNumActionProp = pSector->GetNumActionProp();
						for( int iProp = 0; iProp < iNumActionProp; ++iProp )
						{
							CDnCutSceneActProp* pProp = static_cast<CDnCutSceneActProp*>(pSector->GetActionProp( iProp ));
							pProp->OnResetCutScene();
						}
					}
				}

				if( m_bShowAxis )
					m_pPropAxisObject->Show( true );

				m_bShowSubtitle = false;
			}
			break;
	}

}



void CRTCutSceneRenderer::SeeThisActor( const wchar_t* pActorName )
{
	map<tstring, CDnCutSceneActor*>::iterator iter = m_mapActors.find( pActorName );
	if( m_mapActors.end() != iter )
	{
		EtVector3 vLookAt = *(iter->second->GetPosition());
		//EtVector3 vLookAtDelta = m_pCamController->GetLookAt() - vLookAt;
		//m_pCamController->SetOriLookAt( vLookAt );
		////m_pCamController->SetLookAt( vLookAt );
		////m_pCamController->Set

		//EtVector3 vCamPos = m_pCamController->GetCamPos();
		////EtVector3 vLookAt = m_pCamController->GetLookAt();
		//EtVector3 vUp = m_pCamController->GetUpVector();

		//vCamPos += vLookAtDelta;
		//m_pCamController->SetOriCamPos( vCamPos );

		//m_pCamController->Reset();

		EtVector3 vCamPos( vLookAt.x, vLookAt.y+100.0f, vLookAt.z-300.0f );
		//EtVector3 vLookAt( fMapCenterX, fMapCenterY, fMapCenterZ );
		EtVector3 vUp( 0.0f, 1.0f, 0.0f );

		m_pCamController->SetOriCamPos( vCamPos );
		m_pCamController->SetOriLookAt( vLookAt );
		m_pCamController->SetOriUpVector( vUp );

		//EtVector3 vCamPos = m_pCamController->GetCamPos();
		//EtVector3 vLookAt = m_pCamController->GetLookAt();
		//EtVector3 vUp = m_pCamController->GetUpVector();
		
		m_hCamera->LookAt( vCamPos, vLookAt, vUp );
	}
}


void CRTCutSceneRenderer::SelectActor( const wchar_t* pActorName )
{
	map<tstring, CDnCutSceneActor*>::iterator iter = m_mapActors.find( pActorName );
	
	if( m_mapActors.end() != iter )
	{
		_SelectActor( iter->second );

		// 파티클 이벤트가 선택되어있다면 해제시켜 줌
		SAFE_RELEASE_SPTR( m_hSelectedParticleEvent );

		SeeThisActor( pActorName );
	}
}




//void CRTCutSceneRenderer::SelectParticle( int iEventID )
//{
//	const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( iEventID );
//
//	if( pEventInfo )
//	{
//		// 액터가 선택되어있다면 해제시켜 줌
//		if( m_pSelectedActor )
//			m_pSelectedActor = NULL;
//
//		SAFE_RELEASE_SPTR( m_hSelectedParticleEvent );
//
//		m_iSelectedObjectType = SO_PARTICLE;
//		m_iSelectedParticleEventID = iEventID;
//
//		const ParticleEventInfo* pParticleEvent = static_cast<const ParticleEventInfo*>(pEventInfo);
//		EtMatrix matWorld;
//		
//		EtMatrixTranslation( &matWorld, pParticleEvent->vPos.x, pParticleEvent->vPos.y, pParticleEvent->vPos.z );
//
//		if( -1 != pParticleEvent->iParticleDataIndex )
//		{
//			m_hSelectedParticleEvent = EternityEngine::CreateBillboardEffect( pParticleEvent->iParticleDataIndex, &matWorld );
//			m_hSelectedParticleEvent->EnableLoop( true );
//
//			wxCSConv MBConv( wxFONTENCODING_CP949 );
//			wchar_t wcaBuf[ 256 ];
//			ZeroMemory( wcaBuf, sizeof(wcaBuf) );
//			MBConv.MB2WC( wcaBuf, pParticleEvent->strEventName.c_str(), 256 );
//			m_strSelectedParticleName.assign( wcaBuf );
//		}
//	}
//}




void CRTCutSceneRenderer::ToggleCamMode( bool bToggleCamMode )
{
	m_bCamOnPlay = bToggleCamMode;
}


void CRTCutSceneRenderer::ToggleShowSpotPos( bool bToggleSpotPos )
{
	m_bShowSpotPos = bToggleSpotPos;
}


void CRTCutSceneRenderer::ToggleLetterBox( bool bToggleLetterBox )
{
	m_bShowLetterBox = bToggleLetterBox;	
}


void CRTCutSceneRenderer::SetActivate( bool bActive )
{
	m_bActivate = bActive;
}


void CRTCutSceneRenderer::_DrawAxis( const EtVector3& vPosition, float fRot )
{
	EtVector3 vCamPos = m_pCamController->GetCamPos();
	EtMatrix matRot;
	EtMatrixRotationY( &matRot, fRot );
	//EtVector3 vLookAt = m_pCamController->GetLookAt();

	float fDistance = EtVec3Length( &( vPosition - vCamPos ) );
	DWORD dwColor[3] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
	float fScale = fDistance / 25.f;

	// Draw Axis
	EtVector3 vAxis[6];
	for( int i=0; i<6; i++ ) {
		vAxis[i] = m_avAxis[i] * fScale;
		EtVec3TransformCoord( &vAxis[ i ], &vAxis[ i ], &matRot );
		vAxis[i] += vPosition;
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vAxis[i-1], vAxis[i], dwColor[i/2] );
		}
	}

	// Draw Alphabet
	EtMatrix matScale, mat;
	EtMatrix matTemp = *m_hCamera->GetViewMat();

	EtMatrixInverse( &matTemp, NULL, &matTemp );
	D3DXMatrixScaling( &matScale, fScale, fScale, fScale );

	// Draw Alphabet X
	mat = matTemp;
	mat._41 = vPosition.x + ( 1.2f * fScale );
	mat._42 = vPosition.y + ( 0.2f * fScale );
	mat._43 = vPosition.z;
	EtMatrixMultiply( &mat, &matRot, &mat );
	EtMatrixMultiply( &mat, &matScale, &mat );

	EtVector3 vAlphabetX[4];
	for( int i=0; i<4; i++ ) {
		EtVec3TransformCoord( &vAlphabetX[i], &m_avAlphabetX[i], &mat );
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vAlphabetX[i-1], vAlphabetX[i], dwColor[0] );
		}
	}

	// Draw Alphabet Y
	mat = matTemp;
	mat._41 = vPosition.x + ( 0.2f * fScale );
	mat._42 = vPosition.y + ( 1.2f * fScale );
	mat._43 = vPosition.z;
	EtMatrixMultiply( &mat, &matRot, &mat );
	EtMatrixMultiply( &mat, &matScale, &mat );

	EtVector3 vAlphabetY[4];
	for( int i=0; i<4; i++ ) {
		EtVec3TransformCoord( &vAlphabetY[i], &m_avAlphabetY[i], &mat );
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vAlphabetY[i-1], vAlphabetY[i], dwColor[1] );
		}
	}

	// Draw Alphabet Z
	mat = matTemp;
	mat._41 = vPosition.x + ( 0.2f * fScale );
	mat._42 = vPosition.y;
	mat._43 = vPosition.z + ( 1.2f * fScale );
	EtMatrixMultiply( &mat, &matScale, &mat );

	EtVector3 vAlphabetZ[6];
	for( int i=0; i<6; i++ ) {
		EtVec3TransformCoord( &vAlphabetZ[i], &m_avAlphabetZ[i], &mat );
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vAlphabetZ[i-1], vAlphabetZ[i], dwColor[2] );
		}
	}

}





SPrimitiveDraw3D* CRTCutSceneRenderer::_GetAvailablePBuffer( void )
{
	SPrimitiveDraw3D* pResult = NULL;

	int iNumBuffer = (int)m_vlpBufferToDraw.size();
	for( int iBuffer = 0; iBuffer < iNumBuffer; ++iBuffer )
	{
		if( 0 == m_vlpBufferToDraw.at( iBuffer )->dwColor )
			pResult = m_vlpBufferToDraw.at( iBuffer );
	}

	if( NULL == pResult )
	{
		pResult = new SPrimitiveDraw3D[ m_dwNumTriangle*3 ];
		pResult->dwColor = 0;
		m_vlpBufferToDraw.push_back( pResult );
	}

	return pResult;
}



void CRTCutSceneRenderer::_ResetPBufferUseMark( void )
{
	int iNumBuffer = (int)m_vlpBufferToDraw.size();
	for( int iBuffer = 0; iBuffer < iNumBuffer; ++iBuffer )
	{
		SPrimitiveDraw3D* pBuffer = m_vlpBufferToDraw.at( iBuffer );
		pBuffer->dwColor = 0;
	}
}



void CRTCutSceneRenderer::_MakeSphereWorld( const EtVector3& vPos, EtMatrix& matWorld )
{
	float fLength = EtVec3Length( &(vPos - m_pCamController->GetCamPos()) );
	float fScale = fLength / SPHERE_SIZE_DIVIDER;
	EtMatrix matScale;
	EtMatrixScaling( &matScale, fScale, fScale, fScale );

	EtMatrixTranslation( &matWorld, vPos.x, vPos.y, vPos.z );
	matWorld = matScale * matWorld;
}



void CRTCutSceneRenderer::_DrawCoordText2D( EtVector3& vPos, DWORD dwColor, const char* pName/* = NULL*/ )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );

	EtViewPort Viewport;
	GetEtDevice()->GetViewport( &Viewport );
	EtMatrix matIdentity;
	EtMatrixIdentity( &matIdentity );

	EtVector3 vProjectedPos;
	EtVector2 v2DPos;

	if( NULL == pName )
		sprintf_s( caBuf, "(%2.2f, %2.2f, %2.2f)", vPos.x, vPos.y, vPos.z );
	else
		sprintf_s( caBuf, "%s (%2.2f, %2.2f, %2.2f)", pName, vPos.x, vPos.y, vPos.z );

	EtVec3Project( &vProjectedPos, &vPos, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matIdentity );
	
	v2DPos.x = vProjectedPos.x / (float)Viewport.Width; 
	v2DPos.y = vProjectedPos.y / (float)Viewport.Height;

	EternityEngine::DrawText2D( v2DPos, caBuf, dwColor );
}




void CRTCutSceneRenderer::_DrawKeyPath( const wchar_t* pActorName )
{
	EtVector3 vCamPos = m_pCamController->GetCamPos();
	
	int iNumKey = TOOL_DATA.GetThisActorsKeyNum( pActorName );
	for( int iKey = 0; iKey < iNumKey; ++iKey )
	{
		KeyInfo* pKeyInfo = const_cast<KeyInfo*>(TOOL_DATA.GetThisActorsKeyInfoByIndex( pActorName, iKey ));

		switch( pKeyInfo->iKeyType )
		{
			case KeyInfo::MOVE:
				{
					EternityEngine::DrawLine3D( pKeyInfo->vStartPos, pKeyInfo->vDestPos, 0xffff0000 );
					
					// 시작점과 끝 점에 구 하나씩 ㅋㅋ
					LPDIRECT3DDEVICE9 pd3dDevice = (LPDIRECT3DDEVICE9)(GetEtDevice()->GetDevicePtr());

					EtMatrix matWorld;
					_MakeSphereWorld( pKeyInfo->vStartPos, matWorld );

					SPrimitiveDraw3D* pBuffer = _GetAvailablePBuffer();
					for( DWORD dwVertex = 0; dwVertex < m_dwNumTriangle*3; ++dwVertex )
					{
						pBuffer[ dwVertex ].dwColor = m_pSphereBuffer[ dwVertex ].dwColor;
						EtVec3TransformCoord( &pBuffer[dwVertex].Position, &m_pSphereBuffer[ dwVertex ].Position, &matWorld );
					}

					EternityEngine::DrawTriangle3D( pBuffer, m_dwNumTriangle );
					if( m_bShowSpotPos )
						_DrawCoordText2D( pKeyInfo->vStartPos, 0xffff0000 );

					_MakeSphereWorld( pKeyInfo->vDestPos, matWorld );
					pBuffer = _GetAvailablePBuffer();
					for( DWORD dwVertex = 0; dwVertex < m_dwNumTriangle*3; ++dwVertex )
					{
						pBuffer[ dwVertex ].dwColor = m_pSphereBuffer[ dwVertex ].dwColor;
						EtVec3TransformCoord( &pBuffer[dwVertex].Position, &m_pSphereBuffer[ dwVertex ].Position, &matWorld );
					}

					EternityEngine::DrawTriangle3D( pBuffer, m_dwNumTriangle );
					if( m_bShowSpotPos )
						_DrawCoordText2D( pKeyInfo->vDestPos, 0xffff0000 );
				}
				break;

			case KeyInfo::MULTI_MOVE:
				{
					LPDIRECT3DDEVICE9 pd3dDevice = (LPDIRECT3DDEVICE9)(GetEtDevice()->GetDevicePtr());
					EtMatrix matWorld;

					int iNumSubKey = (int)pKeyInfo->vlMoveKeys.size();
					for( int iSubKey = 0; iSubKey < iNumSubKey; ++iSubKey )
					{
						SubKey Key = pKeyInfo->vlMoveKeys.at( iSubKey );
						if( iSubKey < iNumSubKey-1 )
						{
							SubKey NextKey = pKeyInfo->vlMoveKeys.at( iSubKey+1 );
							EternityEngine::DrawLine3D( Key.vPos, NextKey.vPos, 0xff00ff00 );
						}

						SPrimitiveDraw3D* pBuffer = _GetAvailablePBuffer();

						_MakeSphereWorld( Key.vPos, matWorld );

						for( DWORD dwVertex = 0; dwVertex < m_dwNumTriangle*3; ++dwVertex )
						{
							pBuffer[ dwVertex ].dwColor = m_pSphereBuffer[ dwVertex ].dwColor;
							EtVec3TransformCoord( &pBuffer[dwVertex].Position, &m_pSphereBuffer[ dwVertex ].Position, &matWorld );
						}

						EternityEngine::DrawTriangle3D( pBuffer, m_dwNumTriangle );

						if( m_bShowSpotPos )
							_DrawCoordText2D( Key.vPos, 0xff00ff00 );
					}

				}
				break;

			case KeyInfo::ROTATION:
				break;
		}
	}

	// 액션을 뒤져서 Ani Distance 사용을 사용하는 액션이 있다면 마찬가지로 표시해준다.
	// 전제조건은 AniDistance 를 사용하는 액션인 경우 키와 겹치지 않는다는 것.. 정도.
	EtVector3 vKeyBasePos( 0.0f, 0.0f, 0.0f );
	EtVector3 vAccumedAniDistance( 0.0f, 0.0f, 0.0f );
	float fAccumedRotate = TOOL_DATA.GetRegResRot( pActorName );
	int iNumAction = TOOL_DATA.GetThisActorsActionNum( pActorName );
	for( int iAction = 0; iAction < iNumAction; ++iAction )
	{
		const ActionInfo* pActionInfo = TOOL_DATA.GetThisActorsActionInfoByIndex( pActorName, iAction );
		
		if( pActionInfo->bUseStartRotation )
			fAccumedRotate = pActionInfo->fStartRotation;

		if( pActionInfo->bUseAniDistance )
		{
			// 액션 직전에 있는 키를 찾아낸다. 없으면 패스.
			float fClosestKeyDist = 1000000000.0f;
			const KeyInfo* pClosestKey = NULL;
			int iNumKey = TOOL_DATA.GetThisActorsKeyNum( pActorName );
			for( int iKey = 0; iKey < iNumKey; ++iKey )
			{
				const KeyInfo* pKeyInfo = TOOL_DATA.GetThisActorsKeyInfoByIndex( pActorName, iKey );
				float fDist = pActionInfo->fStartTime - pKeyInfo->fStartTime;
				if( fDist >= 0.0f )
				{
					if( fDist <= fClosestKeyDist )
					{
						if( pKeyInfo->iKeyType == KeyInfo::MOVE ||
							pKeyInfo->iKeyType == KeyInfo::MULTI_MOVE )
						{
							pClosestKey = pKeyInfo;
							fClosestKeyDist = fDist;
						}
						else
						if( pKeyInfo->iKeyType == KeyInfo::ROTATION )
						{
							fAccumedRotate = pKeyInfo->fRotDegree;
						}
					}
				}
				else
					break;
			}

			if( pClosestKey )
			{
				float fProceed = 0.0f;
				if( pActionInfo->fStartTime - pClosestKey->fStartTime > pClosestKey->fTimeLength )
					fProceed = 1.0f;
				else
				{
					float fElapsedTime = (pActionInfo->fStartTime - pClosestKey->fStartTime);
					fProceed = fElapsedTime / pClosestKey->fTimeLength;
				}

				switch( pClosestKey->iKeyType )
				{
					case KeyInfo::MOVE:
						{
							EtVector3 vDir = pClosestKey->vDestPos - pClosestKey->vStartPos;
							vKeyBasePos =  pClosestKey->vStartPos + vDir*fProceed;
						}
						break;

					case KeyInfo::MULTI_MOVE:
						{
							// 이동 서브셋키는 무조건 3개 이상임.
							float fOffsetTimeLength = pClosestKey->fStartTime;
							int iOffsetIndex = 0;
							EtVector3 vDir, vStartPos;
							int iNumSubKey = (int)pClosestKey->vlMoveKeys.size();
							for( int iSubKey = 0; iSubKey < iNumSubKey; ++iSubKey )
							{
								if( fOffsetTimeLength + pClosestKey->vlMoveKeys.at( iSubKey ).fTimeLength < pActionInfo->fStartTime )
									fOffsetTimeLength += pClosestKey->vlMoveKeys.at( iSubKey ).fTimeLength;
								else
								{
									if( 0 == iSubKey )
									{
										vStartPos = pClosestKey->vStartPos;
										vDir = pClosestKey->vlMoveKeys.at(iSubKey).vPos - vStartPos;
									}
									else
									{
										vStartPos = pClosestKey->vlMoveKeys.at(iSubKey-1).vPos;
										vDir = pClosestKey->vlMoveKeys.at(iSubKey).vPos - vStartPos;
									}

									iOffsetIndex = iSubKey;
									break;
								}
							}

							float fSubSetProceed = (fOffsetTimeLength - pActionInfo->fStartTime) / pClosestKey->vlMoveKeys.at( iOffsetIndex ).fTimeLength;
							vKeyBasePos = vStartPos + vDir*fSubSetProceed;
						}
						break;

						// 나머지 키 타입은 무시
					default:
						break;
				}

				EtAniObjectHandle hActor = m_pSelectedActor->GetAniObjectHandle();
				int iAniIndex = m_pSelectedActor->GetCachedAniIndex(pActionInfo->iAnimationIndex);
				EtVector3 vAniDistance;

				hActor->CalcAniDistance( iAniIndex, hActor->GetLastFrame( iAniIndex ), 0.0f, vAniDistance );
				vAniDistance.y = 0.0f;

				EtMatrix matRotY;
				EtMatrixRotationY( &matRotY, -(fAccumedRotate*ET_PI / 180.0f) );
				EtVec3TransformCoord( &vAniDistance, &vAniDistance, &matRotY );

				EtVector3 vFinalAniDistancePos = vKeyBasePos + vAccumedAniDistance + vAniDistance;

				if( pActionInfo->bFitAniDistanceYToMap && m_pWorld )
				{
					vFinalAniDistancePos.y = m_pWorld->GetHeight( vFinalAniDistancePos.x, vFinalAniDistancePos.z );
				}

				EternityEngine::DrawLine3D( vKeyBasePos+vAccumedAniDistance, vFinalAniDistancePos, 0xff0000ff );

				LPDIRECT3DDEVICE9 pd3dDevice = (LPDIRECT3DDEVICE9)(GetEtDevice()->GetDevicePtr());

				EtMatrix matWorld;
				_MakeSphereWorld( vFinalAniDistancePos, matWorld );

				SPrimitiveDraw3D* pBuffer = _GetAvailablePBuffer();
				for( DWORD dwVertex = 0; dwVertex < m_dwNumTriangle*3; ++dwVertex )
				{
					pBuffer[ dwVertex ].dwColor = m_pSphereBuffer[ dwVertex ].dwColor;
					EtVec3TransformCoord( &pBuffer[dwVertex].Position, &m_pSphereBuffer[ dwVertex ].Position, &matWorld );
				}

				EternityEngine::DrawTriangle3D( pBuffer, m_dwNumTriangle );

				if( m_bShowSpotPos )
					_DrawCoordText2D( vFinalAniDistancePos, 0xff0000ff );

				vAccumedAniDistance += vAniDistance;
			}
		}
	}
}




void CRTCutSceneRenderer::_DrawLetterBox( void )
{
	// 클라이언트에서는 1024 해상도일 때 100 픽셀씩 사용함.
	float fRatio = 100.0f / 1024.0f;

	CEtSprite::GetInstance().Begin( D3DXSPRITE_DONOTSAVESTATE | D3DXSPRITE_ALPHABLEND );
	
	CEtSprite::GetInstance().Flush();
		
	SUICoord UpCoord( 0.0f, 0.0f, 1.0f, (fRatio*(float)m_iHeight) / (float)m_iHeight );
	CEtSprite::GetInstance().DrawRect( UpCoord, 0xff000000 );

	SUICoord DownCoord( 0.0f, 1.0f - (fRatio*(float)m_iHeight) / (float)m_iHeight, 1.0f, (fRatio*(float)m_iWidth) / (float)m_iHeight );
	CEtSprite::GetInstance().DrawRect( DownCoord, 0xff000000 );

	CEtSprite::GetInstance().End();
}




void CRTCutSceneRenderer::_ProcessAxisObject( LOCAL_TIME LocalTime, float fDelta )
{
	map<CDnCutSceneActor*, CAxisRenderObject*>::iterator iterMap = m_mapActorAxisObject.begin();
	for( iterMap; iterMap != m_mapActorAxisObject.end(); ++iterMap )
	{
		MatrixEx MatrixEx = *(iterMap->first->GetMatrixEx());
		iterMap->second->SetWorld( *(EtMatrix*)&MatrixEx );

		EtVector3 vActorPos = *iterMap->first->GetPosition();
		EtVector3 vCamPos = m_pCamController->GetCamPos();
		float fDistance = EtVec3Length( &( vActorPos - vCamPos ) );
		float fScale = fDistance / 300.f;
		iterMap->second->SetScale( fScale );		
		iterMap->first->GetAniObjectHandle()->DrawBoundingBox();
		
		// 좌표 찍기.
		char acBuffer[ 256 ];
		ZeroMemory( acBuffer, sizeof(acBuffer) );
		wxCSConv MBConv( wxFONTENCODING_CP949 );
		MBConv.WC2MB( acBuffer, iterMap->first->GetName(), 256 );

		_DrawCoordText2D( vActorPos, 0xffff0000, acBuffer );
	}


	map<KeyInfo*, CAxisRenderObject*>::iterator it = m_mapKeyInfoAxisObject.begin();
	for( ; it != m_mapKeyInfoAxisObject.end(); ++it )
	{		
		//MatrixEx MatrixEx = *(it->first->GetMatrixEx());
		MatrixEx MatrixEx;
		MatrixEx.SetPosition( ( m_bSelectedStartPos == true ? it->first->vStartPos : it->first->vDestPos ) );
		it->second->SetWorld( *(EtMatrix*)&MatrixEx );

		EtVector3 vActorPos = it->first->vDestPos;
		EtVector3 vCamPos = m_pCamController->GetCamPos();
		float fDistance = EtVec3Length( &( vActorPos - vCamPos ) );
		float fScale = fDistance / 300.f;
		it->second->SetScale( fScale );				

	}

	// Lock Actor들.
	if( !m_mapLockSelectedActor.empty() )
	{
		std::map<std::wstring, CDnCutSceneActor *>::iterator it = m_mapLockSelectedActor.begin();
		for( ; it!=m_mapLockSelectedActor.end(); it++ )
		{
			it->second->GetAniObjectHandle()->DrawBoundingBox( 0xffff0000 );
		}
	}
	

}



void CRTCutSceneRenderer::OnRender( LOCAL_TIME LocalTime, float fDelta )
{
	if( false == m_bActivate )
		Sleep( 50 );

	EtVector3 vCamPos = m_pCamController->GetCamPos();
	EtVector3 vLookAt = m_pCamController->GetLookAt();
	EtVector3 vUp = m_pCamController->GetUpVector();

	EtMatrix matWorld;
	m_pCamController->GetMatrix( matWorld );

	//m_pWorld-

	//m_hCamera->LookAt( vCamPos, vLookAt, vUp );
	//m_hCamera->Update( &m_CameraCross );

	EtVector3 vDir = vLookAt - vCamPos;
	EtVec3Normalize( &vDir, &vDir );
	if( m_pWorld )
	{
		m_pWorld->Update( EtVector3( 0.0f, 0.0f, 0.0f), vDir, 1.0f );
		m_pWorld->Process( LocalTime, fDelta );
	}

	CEtSoundEngine::GetInstance().Process( fDelta );

	float fDistance = EtVec3Length( &( m_pPropAxisObject->GetPosition() - vCamPos ) );
	float fScale = fDistance / 300.f;

	if( m_bShowAxis )
		m_pPropAxisObject->SetScale( fScale );

	switch( m_iMode )
	{
		case EDITOR_MODE:
			{
				int iNumActors = (int)m_vlpActors.size();
				for( int iActor = 0; iActor < iNumActors; ++iActor )
					m_vlpActors.at(iActor)->Process( LocalTime, 0.0f );

				EtVector3 vLookAt(m_pCamController->GetLookAt());
				_DrawAxis( vLookAt, 0.0f );
				//_DrawCoordText2D( vLookAt, 0xffff0000 );

				//if( m_pSelectedActor )
				switch( m_iSelectedObjectType )
				{
					case SO_ACTOR:
						{
							// 크로스 벡터에서 사용하는 회전 방향은 반대임.
							if( m_pSelectedActor )
							{
								//float fRot = m_pSelectedActor->GetRotationY()*ET_PI / 180.0f;
								//_DrawAxis( EtVector3(*m_pSelectedActor->GetPosition()), -fRot );

								// 갖고 있는 키들의 패스를 보여준다. 그냥 포인트마다 라인으로 찍찍~
								// 번호도 찍어주고~
								// 키의 종류마다 색깔을 다르게 보여주자.
								// Start, End 구분
								_DrawKeyPath( m_pSelectedActor->GetName() );
							}
						}
						break;

					case SO_PARTICLE:
						{
							const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( m_iSelectedParticleEventID );
							if( pEventInfo )
							{
								// 로테이션은 일단 임시로..
								const ParticleEventInfo* pParticleEventInfo = static_cast<const ParticleEventInfo*>(pEventInfo);
								if( m_hSelectedParticleEvent != NULL )
								{
									EtMatrix* pMatWorld = m_hSelectedParticleEvent->GetWorldMat();
									EtVector3 vPos( pMatWorld->_41, pMatWorld->_42, pMatWorld->_43 );

									_DrawAxis( vPos, 0.0f );
								}
							}
						}
						break;

					case SO_MAX_CAMERA:
						{
							const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( m_iSelectedCameraEventID );
							if( pEventInfo )
							{
								const CamEventInfo* pCamEventInfo = static_cast<const CamEventInfo*>(pEventInfo);

								// 카메라의 패스를 라인으로 렌더링~
								int iNumPosKey = (int)pCamEventInfo->pCameraData->m_vecPosition.size();
								for( int i = 0; i < iNumPosKey-1; ++i )
								{
									SCameraPositionKey& PosKey = pCamEventInfo->pCameraData->m_vecPosition.at( i );
									SCameraPositionKey& NextPosKey = pCamEventInfo->pCameraData->m_vecPosition.at( i+1 );
									EtVector3 vWorldPos = pCamEventInfo->vCamStartOffset + PosKey.vPosition;
									EtVector3 vWorldNextPos = pCamEventInfo->vCamStartOffset + NextPosKey.vPosition;

									// 회전적용.
									SCameraRotationKey& RotateKey = pCamEventInfo->pCameraData->m_vecRotation.at( i );
									SCameraRotationKey& nextRotateKey = pCamEventInfo->pCameraData->m_vecRotation.at( i+1 );
									EtQuat quat = RotateKey.qRotation;
									EtQuat quatNext = nextRotateKey.qRotation;
									EtMatrix matKey, nextMatKey;
									EtMatrixRotationQuaternion( &matKey, &quat );
									EtMatrixRotationQuaternion( &nextMatKey, &quatNext );

									EtVec3TransformCoord( &vWorldPos, &vWorldPos, &matKey );
									EtVec3TransformCoord( &vWorldNextPos, &vWorldNextPos, &nextMatKey );
									//

									EternityEngine::DrawLine3D( vWorldPos, vWorldNextPos, 0xff00ff00 );

									if( 0 == i || iNumPosKey-2 == i )
									{
										EtVector3 vPos;
										DWORD dwColor = 0;
										if( 0 == i )
										{
											vPos = vWorldPos;			// 첫 시작 점
											dwColor = 0x44ff0000;
										}
										else
										{
											vPos = vWorldNextPos;		// 끝 점
											dwColor = 0x440000ff;
										}

										EtMatrix matWorld;
										_MakeSphereWorld( vPos, matWorld );

										SPrimitiveDraw3D* pBuffer = _GetAvailablePBuffer();
										for( DWORD dwVertex = 0; dwVertex < m_dwNumTriangle*3; ++dwVertex )
										{
											pBuffer[ dwVertex ].dwColor = dwColor;//m_pSphereBuffer[ dwVertex ].dwColor;
											EtVec3TransformCoord( &pBuffer[dwVertex].Position, &m_pSphereBuffer[dwVertex].Position, &matWorld );
										}

										EternityEngine::DrawTriangle3D( pBuffer, m_dwNumTriangle );

										if( m_bShowSpotPos )
											_DrawCoordText2D( vPos, 0xffff0000 );
									}
								}
							}
						}
						break;
				}
			}
			break;

		case PLAY_MODE:
			{
				assert( m_pCutScenePlayer );
				m_pCutScenePlayer->Process( LocalTime );

				if( m_bCamOnPlay )
				{
					EtVector3 vCamPos = m_pCamController->GetCamPos();
					EtVector3 vLookAt = m_pCamController->GetLookAt();
					EtVector3 vUp = m_pCamController->GetUpVector();

					m_hCamera->LookAt( vCamPos, vLookAt, vUp );
					
					// 프로젝션이 컷신에 들어간 맥스카메라의 FOV 로 바뀔 수가 있으므로 계속 디폴트로 셋팅해준다.
					m_hCamera->SetProjMat( m_matDefaultProj );

					_DrawAxis( EtVector3(m_pCamController->GetLookAt()), 0.0f );

					if( m_pSelectedActor )
					{
						float fRot = m_pSelectedActor->GetRotationY()*ET_PI / 180.0f;
						_DrawAxis( EtVector3(*m_pSelectedActor->GetPosition()), -fRot );
					}
				}
			}
			break;

		case FREE_TIME_MODE:
			{
				assert( m_pCutScenePlayer );
				m_pCutScenePlayer->ResetLiveSequences();
				m_pCutScenePlayer->Process( LocalTime );

				if( m_bCamOnPlay )
				{
					EtVector3 vCamPos = m_pCamController->GetCamPos();
					EtVector3 vLookAt = m_pCamController->GetLookAt();
					EtVector3 vUp = m_pCamController->GetUpVector();

					m_hCamera->LookAt( vCamPos, vLookAt, vUp );

					// 프로젝션이 컷신에 들어간 맥스카메라의 FOV 로 바뀔 수가 있으므로 계속 디폴트로 셋팅해준다.
					m_hCamera->SetProjMat( m_matDefaultProj );

					_DrawAxis( EtVector3(m_pCamController->GetLookAt()), 0.0f );

					if( m_pSelectedActor )
					{
						float fRot = m_pSelectedActor->GetRotationY()*ET_PI / 180.0f;
						_DrawAxis( EtVector3(*m_pSelectedActor->GetPosition()), -fRot );
					}
				}
			}
			break;
	}

	//MatrixEx Cross;
	//Cross.RotatePitch(-45.f);
	//Cross.MoveFrontBack( -3000.f );
	//m_hCamera->Update( Cross );

	_ProcessAxisObject( LocalTime, fDelta );

	ClearScreen( 0xff0000ff, 1.0f, 0 );
	RenderFrame( fDelta );

	if( PLAY_MODE == m_iMode )
	{
		m_pFade->Render( fDelta );
	}

	if( m_bShowLetterBox )
	{
		_DrawLetterBox();
	}

	if( m_bShowSubtitle )
	{
		SUICoord Coord;
		Coord.fWidth = 0.0f;
		Coord.fHeight = 0.2f;
		Coord.fX = (Coord.fWidth / 2.0f) / Coord.fWidth;
		Coord.fY = (Coord.fHeight / 2.0f) / Coord.fHeight;
		SFontDrawEffectInfo Info;

		int iFontSetIndex = 0;
		Info.dwFontColor = 0xffffffff;
		Info.dwEffectColor = 0xff000000;
		Info.nDrawType = SFontDrawEffectInfo::SHADOW;
		//CEtFontMng::GetInstance().DrawTextW( iFontSetIndex, 15, m_strSubtitle.c_str(), DT_CENTER | DT_WORDBREAK, Coord, 0, Info );

		SIZE size;
		GetTextExtentPoint32( m_pFont->GetDC(), m_strSubtitle.c_str(), (int)m_strSubtitle.length(), &size );

		RECT rect;
		SetRect( &rect, (m_iWidth-size.cx) / 2, m_iHeight-50 - size.cy, (m_iWidth-size.cx)/2 + size.cx, m_iHeight-50 );
		//SetRect( &rect, 0, m_iHeight-50 - size.cy, size.cx, m_iHeight-50 );
		m_pFont->DrawTextW( NULL, m_strSubtitle.c_str(), (INT)m_strSubtitle.length(), &rect, DT_CENTER, 0xff000000 );
		
		rect.left -= 1;
		rect.right -= 1;
		rect.top -= 1;
		rect.bottom -= 1;
		m_pFont->DrawTextW( NULL, m_strSubtitle.c_str(), (INT)m_strSubtitle.length(), &rect, DT_CENTER, 0xffffffff );
	}

	ShowFrame( NULL );

	m_pCamController->SetViewMatrix( *m_hCamera->GetViewMat() );
	m_pCamController->SetProjMatrix( *m_hCamera->GetProjMat() );

	_ResetPBufferUseMark();
}



void CRTCutSceneRenderer::GetHeightPos( EtVector3& vPos )
{
	// 맵이 있다면 가운데 지점에 셋팅
	if( m_pWorld )
	{
		EtVector3 vLookAt = m_pCamController->GetLookAt();
		float fLookAtHeight = m_pWorld->GetHeight( vLookAt.x, vLookAt.z );
		vPos.x = vLookAt.x;
		vPos.y = fLookAtHeight;
		vPos.z = vLookAt.z;
	}
	else
	{
		vPos.x = 0; vPos.y = 0.0f; vPos.z = 0.0f;
	}
}



float CRTCutSceneRenderer::GetMapHeight( float fXPos, float fYPos )
{
	float fResult = 0.0f;

	if( m_pWorld )
	{
		fResult = m_pWorld->GetHeight( fXPos, fYPos );
	}

	return fResult;
}




// 마우스 이벤트 관련
void CRTCutSceneRenderer::OnLButtonDown( int iXPos, int iYPos )
{
	bool bSelectKeyAxis = false;
	bool bSelectKeyInfo = false;

	m_iPrevClkXPos = iXPos;
	m_iPrevClkYPos = iYPos;

//	if( !bIsLockSelect() )
		m_pCamController->OnRButtonDown( iXPos, iYPos );

	// 우선 ray 를 만든다
	EtMatrix matWorld;
	EtMatrixIdentity( &matWorld );

	EtVector3 vMousePos( (float)iXPos, (float)iYPos, 0.0f );
	EtVector3 vMousePosEndZ( (float)iXPos, (float)iYPos, 1.0f );

	EtVector3 vRayStart, vRayEnd, vRayDir;
	EtViewPort Viewport;
	GetEtDevice()->GetViewport( &Viewport );
	EtVec3Unproject( &vRayStart, &vMousePos, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );
	EtVec3Unproject( &vRayEnd, &vMousePosEndZ, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );
	vRayDir = vRayEnd - vRayStart;
	EtVec3Normalize( &vRayDir, &vRayDir );

	// axis 에 클릭이 됐는지
	bool bAxisHasSelected = false;
	map<CDnCutSceneActor*, CAxisRenderObject*>::iterator iter = m_mapActorAxisObject.begin();
	for( iter; iter != m_mapActorAxisObject.end(); ++iter )
	{
		int iSelectedAxis = iter->second->CheckAxis( m_pCamController->GetCamPos(), vRayStart, vRayDir );
		if( CAxisRenderObject::AXIS_NONE != iSelectedAxis )
			bAxisHasSelected = true;
	}

	map<KeyInfo*, CAxisRenderObject*>::iterator iterKey = m_mapKeyInfoAxisObject.begin();
	for( ; iterKey != m_mapKeyInfoAxisObject.end(); ++iterKey )
	{
		int iSelectedAxis = iterKey->second->CheckAxis( m_pCamController->GetCamPos(), vRayStart, vRayDir );
		if( CAxisRenderObject::AXIS_NONE != iSelectedAxis )
		{
			bAxisHasSelected = true;

			bSelectKeyAxis = true;
		}
	}
	

	// 카메라에서 가까이 있는 놈 기준으로 정렬
	struct SortByCameraDistance : public binary_function<const CDnCutSceneActor*, const CDnCutSceneActor*, bool>
	{
		EtVector3 m_vCamPos;

		SortByCameraDistance( const EtVector3& vCamPos ) : m_vCamPos( vCamPos ) {};

		bool operator () ( const CDnCutSceneActor* pA, const CDnCutSceneActor* pB )
		{
			EtVector3 vADir = m_vCamPos - *(pA->GetPosition());
			EtVector3 vBDir = m_vCamPos - *(pB->GetPosition());

			return EtVec3LengthSq( &vADir ) < EtVec3LengthSq( &vBDir );
		}
	};

	sort( m_vlpActors.begin(), m_vlpActors.end(), SortByCameraDistance(m_pCamController->GetCamPos()) );

	// 피킹
	EtObjectHandle hPickedObject;// = Pick( iXPos, iYPos );
//	if( !bIsLockSelect() )
//	{
		CDnCutSceneActor* pActor = NULL;
		int iNumActor = (int)m_vlpActors.size();
		for( int i = 0; i < iNumActor; ++i )
		{
			pActor = m_vlpActors.at( i );

			// Lock Actor는 제외.
			if( IsLockActor( pActor ) )
				continue;

			vector<SCollisionPrimitive*>* pvlCollisionPrimitives = pActor->GetAniObjectHandle()->GetCollisionPrimitive();
			if( false == pvlCollisionPrimitives->empty() )
			{
				SSegment Segment;
				Segment.vOrigin = vRayStart;
				Segment.vDirection = vRayDir*10000.0f;
				SCollisionResponse res;
				if( pActor->GetAniObjectHandle()->FindSegmentCollision( Segment, res ) )
				{
					hPickedObject = pActor->GetAniObjectHandle();
					break;
				}
			}
			else
			{
				SAABox AABB;
				pActor->GetAniObjectHandle()->GetBoundingBox( AABB );
				float fDistance = FLT_MAX;
				if( TestLineToBox( vRayStart, vRayDir, AABB, fDistance ) )
				{
					hPickedObject = pActor->GetAniObjectHandle();
					break;
				}
			}
		}
//	}

	
	/*if( pActor && IsLockActor( pActor ) )
	{
		if( m_pSelectedActor == pActor )
			_DeSelectActor();
		return;
	}*/


	bool bActorHasPicked = false;
	if( hPickedObject )
	{
		bActorHasPicked = true;

		if( !(m_pSelectedActor && 
			  m_pSelectedActor->GetAniObjectHandle() == hPickedObject) )
		{
			int iNumActor = (int)m_vlpActors.size();
			for( int iActor = 0; iActor < iNumActor; ++iActor )
			{
				CDnCutSceneActor* pActor = m_vlpActors.at( iActor );
				EtAniObjectHandle AniObject = pActor->GetAniObjectHandle();
				if( hPickedObject == AniObject )
				{
					if( m_pSelectedActor )
						_DeSelectActor();

					_SelectActor( pActor );
					return;
				}
			}
		}
	}

	if( m_pSelectedActor )
	{
		// 선택된 액터의 키, 멀티 무브 키가 있으면 각 지점마다 피킹한다.
		int iNumKey = TOOL_DATA.GetThisActorsKeyNum( m_pSelectedActor->GetName() );

		// 우선 ray 를 만든다
		EtMatrix matWorld;
		EtMatrixIdentity( &matWorld );

		EtVector3 vMousePos( (float)iXPos, (float)iYPos, 0.0f );
		EtVector3 vMousePosEndZ( (float)iXPos, (float)iYPos, 1.0f );

		EtVector3 vRayStart, vRayEnd, vRayDir;
		EtViewPort Viewport;
		GetEtDevice()->GetViewport( &Viewport );
		EtVec3Unproject( &vRayStart, &vMousePos, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );
		EtVec3Unproject( &vRayEnd, &vMousePosEndZ, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );
		vRayDir = vRayEnd - vRayStart;
		EtVec3Normalize( &vRayDir, &vRayDir );

		for( int iKey = 0; iKey < iNumKey; ++iKey )
		{
			const KeyInfo* pKeyInfo = TOOL_DATA.GetThisActorsKeyInfoByIndex( m_pSelectedActor->GetName(), iKey );
			
			// 값이 수정될 경우 원래 구조체를 복사해뒀다가 마우스 버튼을 뗄 때 그냥 커맨드로 날리면 된다.
			switch( pKeyInfo->iKeyType )
			{
				case KeyInfo::MOVE:
					{
						// 시작 점과 끝 점을 피킹
						float fLength = EtVec3Length( &(pKeyInfo->vStartPos - m_pCamController->GetCamPos()) );
						float fRadius = SPHERE_RADIUS * (fLength / SPHERE_SIZE_DIVIDER);
						bool bClkedStartPos = CIntersectUtil::RaySphereIntersect( vRayStart, vRayDir, fRadius, pKeyInfo->vStartPos, NULL, NULL );
						if( bClkedStartPos )
						{
							m_SelectedKeyInfo = *pKeyInfo;
							m_pSelectedKeyPos = const_cast<EtVector3*>(&(pKeyInfo->vStartPos));
							m_bEditedKeyInfo = false;
							m_bSelectedStartPos = true;
							m_iSelectedSubKey = -1;
														
							bSelectKeyInfo = true;
							SelectKeyInfo( pKeyInfo );
						}

						fLength = EtVec3Length( &(pKeyInfo->vDestPos - m_pCamController->GetCamPos()) );
						fRadius = SPHERE_RADIUS * (fLength / SPHERE_SIZE_DIVIDER);
						bool bClkedDestPos = CIntersectUtil::RaySphereIntersect( vRayStart, vRayDir, fRadius, pKeyInfo->vDestPos, NULL, NULL );
						if( bClkedDestPos )
						{
							m_SelectedKeyInfo = *pKeyInfo;
							m_pSelectedKeyPos = const_cast<EtVector3*>(&(pKeyInfo->vDestPos));
							m_bEditedKeyInfo = false;
							m_bSelectedStartPos = false;
							m_iSelectedSubKey = -1;

							bSelectKeyInfo = true;
							SelectKeyInfo( pKeyInfo );
						}
					}
					break;

				case KeyInfo::MULTI_MOVE:
					{
						float fLength = EtVec3Length( &(pKeyInfo->vStartPos - m_pCamController->GetCamPos()) );
						float fRadius = SPHERE_RADIUS * (fLength / SPHERE_SIZE_DIVIDER);
						bool bClkedStartPos = CIntersectUtil::RaySphereIntersect( vRayStart, vRayDir, fRadius, pKeyInfo->vStartPos, NULL, NULL );
						if( bClkedStartPos )
						{
							m_SelectedKeyInfo = *pKeyInfo;
							m_pSelectedKeyPos = const_cast<EtVector3*>(&(pKeyInfo->vStartPos));
							m_bEditedKeyInfo = false;
							m_bSelectedStartPos = true;
							m_iSelectedSubKey = -1;
							
							bSelectKeyInfo = true;
							SelectKeyInfo( pKeyInfo );
						}

						// TODO: 각 지점마다 피킹 체크
						int iNumSubKey = (int)pKeyInfo->vlMoveKeys.size();
						for( int iSubKey = 0; iSubKey < iNumSubKey; ++iSubKey )
						{
							const SubKey& Key = pKeyInfo->vlMoveKeys.at( iSubKey );
							
							float fLength = EtVec3Length( &(Key.vPos - m_pCamController->GetCamPos()) );
							float fRadius = SPHERE_RADIUS * (fLength / SPHERE_SIZE_DIVIDER);
							bool bIntersect = CIntersectUtil::RaySphereIntersect( vRayStart, vRayDir, fRadius, Key.vPos, NULL, NULL );
							if( bIntersect )
							{
								m_SelectedKeyInfo = *pKeyInfo;
								m_pSelectedKeyPos = const_cast<EtVector3*>(&(Key.vPos));
								m_bEditedKeyInfo = false;
								m_bSelectedStartPos = false;
								m_iSelectedSubKey = iSubKey;
								
								bSelectKeyInfo = true;
								SelectKeyInfo( pKeyInfo );
							}
						}
					}
					break;
			}
		}
	
	}

	if( !m_pSelectedKeyPos && m_bShowMaxCameraPath )
	{
		// 우선 ray 를 만든다
		EtMatrix matWorld;
		EtMatrixIdentity( &matWorld );

		EtVector3 vMousePos( (float)iXPos, (float)iYPos, 0.0f );
		EtVector3 vMousePosEndZ( (float)iXPos, (float)iYPos, 1.0f );

		EtVector3 vRayStart, vRayEnd, vRayDir;
		EtViewPort Viewport;
		GetEtDevice()->GetViewport( &Viewport );
		EtVec3Unproject( &vRayStart, &vMousePos, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );
		EtVec3Unproject( &vRayEnd, &vMousePosEndZ, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );
		vRayDir = vRayEnd - vRayStart;
		EtVec3Normalize( &vRayDir, &vRayDir );

		// 시작 점과 끝 점을 피킹
		const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( m_iSelectedCameraEventID );
		if( pEventInfo )
		{
			const CamEventInfo* pCamEventInfo = static_cast<const CamEventInfo*>(pEventInfo);

			SCameraPositionKey& StartKey = pCamEventInfo->pCameraData->m_vecPosition.front();
			SCameraPositionKey& EndKey = pCamEventInfo->pCameraData->m_vecPosition.back();
			EtVector3 vStartPos = StartKey.vPosition + pCamEventInfo->vCamStartOffset;
			EtVector3 vEndPos = EndKey.vPosition + pCamEventInfo->vCamStartOffset;

			float fLength = EtVec3Length( &(StartKey.vPosition- m_pCamController->GetCamPos()) );
			float fRadius = SPHERE_RADIUS * (fLength / SPHERE_SIZE_DIVIDER);
			bool bClkedStartPos = CIntersectUtil::RaySphereIntersect( vRayStart, vRayDir, fRadius, vStartPos, NULL, NULL );
			if( bClkedStartPos )
			{
				m_vOriMaxCamOffset = pCamEventInfo->vCamStartOffset;;
				m_pSelectedMaxCamOffset = &const_cast<CamEventInfo*>(pCamEventInfo)->vCamStartOffset;
				m_bEditedKeyInfo = false;
				m_bSelectedStartPos = true;
			}

			fLength = EtVec3Length( &(EndKey.vPosition - m_pCamController->GetCamPos()) );
			fRadius = SPHERE_RADIUS * (fLength / SPHERE_SIZE_DIVIDER);
			bool bClkedDestPos = CIntersectUtil::RaySphereIntersect( vRayStart, vRayDir, fRadius, vEndPos, NULL, NULL );
			if( bClkedDestPos )
			{
				//m_SelectedKeyInfo = *pKeyInfo;
				m_vOriMaxCamOffset = pCamEventInfo->vCamStartOffset;
				m_pSelectedMaxCamOffset = &const_cast<CamEventInfo*>(pCamEventInfo)->vCamStartOffset;
				m_bEditedKeyInfo = false;
				m_bSelectedStartPos = false;
			}
		}
	}

	// 액터 피킹이 아니고, 축 선택이 된 것이 아니고, 키 패스나 카메라가 선택된 것이 아니라면 바깥 부분 클릭한 것이므로 액터 디셀렉트!
	//if( !bIsLockSelect() && false == bActorHasPicked && false == bAxisHasSelected && NULL == m_pSelectedKeyPos && NULL == m_pSelectedMaxCamOffset )
	if( false == bActorHasPicked && false == bAxisHasSelected && NULL == m_pSelectedKeyPos && NULL == m_pSelectedMaxCamOffset )
		_DeSelectActor();


	if( bSelectKeyAxis == false && bSelectKeyInfo == false )
	{
		_DeSelectKeyAxis();	

		if( m_bEditedKeyInfo )
		{
			if( false == m_SelectedKeyInfo.strActorName.empty() )
			{			
				m_SelectedKeyInfo.strActorName.clear();
				m_iSelectedSubKey = -1;
				m_bEditedKeyInfo = false;
				m_bSelectedStartPos = false;
				m_pSelectedKeyPos = NULL;
				
			}
		}
	}
	
	m_bSelectedKeyAxis = bSelectKeyAxis;
	m_bSelectKeyInfo = bSelectKeyInfo;
	//if( bSelectKeyAxis && m_SelectedKeyInfo.strActorName.empty() )	
	//{
	//	map<KeyInfo*, CAxisRenderObject*>::iterator it = m_mapKeyInfoAxisObject.begin();
	//	if( it != m_mapKeyInfoAxisObject.end() )
	//		m_SelectedKeyInfo = *(it->first);
	//}

}

void CRTCutSceneRenderer::OnRButtonDown( int iXPos, int iYPos )
{
	/*if( bIsLockSelect() )
	{
		m_iPrevClkXPos = iXPos;
		m_iPrevClkYPos = iYPos;

		m_pCamController->OnRButtonDown( iXPos, iYPos );
	}*/
	m_pCamController->OnLButtonDown( iXPos, iYPos );
}


void CRTCutSceneRenderer::OnCButtonDown(int iXPos, int iYPos )
{
	m_pCamController->OnCButtonDown( iXPos, iYPos );
}


void CRTCutSceneRenderer::OnLButtonDrag( int iXPos, int iYPos )
{
	if( false == m_SelectedKeyInfo.strActorName.empty() )
	{
		EtMatrix matWorld;
		EtMatrixIdentity( &matWorld );

		EtVector3 vMouseStart( (float)m_iPrevClkXPos, (float)m_iPrevClkYPos, 0.0f );
		EtVector3 vMouseEnd( (float)iXPos, (float)iYPos, 0.0f );

		EtVector3 vStart, vEnd, vDelta;
		EtViewPort Viewport;
		GetEtDevice()->GetViewport( &Viewport );
		EtVec3Unproject( &vStart, &vMouseStart, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );
		EtVec3Unproject( &vEnd, &vMouseEnd, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );

		//EtVector3 vLength = *m_pSelectedKeyPos - vStart;
		//float fRayStartToKeyLength = EtVec3Length( &vLength );

		//vDelta = vEnd - vStart;
		//vDelta *= fRayStartToKeyLength/10.0f;

		//// 마우스 delta 값을 적용
		//*m_pSelectedKeyPos += vDelta;

		//if( m_SelectedKeyInfo.bFitYPosToMap )
		//	m_pSelectedKeyPos->y = m_pWorld->GetHeight( m_pSelectedKeyPos->x, m_pSelectedKeyPos->z );

		//m_bEditedKeyInfo = true;


		//--------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------
	
		bool bSelectedAxis = false;
		map<KeyInfo*, CAxisRenderObject*>::iterator iter = m_mapKeyInfoAxisObject.begin();
		for( iter; iter != m_mapKeyInfoAxisObject.end(); ++iter )
		{
			int iSelectedAxis = iter->second->GetSelectedAxis();
			if( CAxisRenderObject::AXIS_NONE != iSelectedAxis )
			{
				/*MatrixEx MatrixEx = *(iter->first->GetMatrixEx());

				EtVector3 vLength = MatrixEx.GetPosition() - vStart;
				float fRayStartToKeyLength = EtVec3Length( &vLength );

				vDelta = vEnd - vStart;
				vDelta *= fRayStartToKeyLength/10.0f;*/

				//EtMatrix matRot = MatrixEx;
				//matRot._41 = matRot._42 = matRot._43 = 0.0f;
				//matRot._11 = matRot._22 = matRot._33 = matRot._44 = 1.0f;

				MatrixEx MatrixEx;

				EtVector3 vLength = *m_pSelectedKeyPos - vStart;
				float fRayStartToKeyLength = EtVec3Length( &vLength );

				vDelta = vEnd - vStart;
				vDelta *= fRayStartToKeyLength/10.0f;

				// 마우스 delta 값을 적용
				//*m_pSelectedKeyPos += vDelta;


				switch( iSelectedAxis )
				{
				case CAxisRenderObject::AXIS_X:
					bSelectedAxis = true;
					//MatrixEx.MoveLocalXAxis( vDelta.x );
					m_pSelectedKeyPos->x += vDelta.x;
					break;

				case CAxisRenderObject::AXIS_Y:
					bSelectedAxis = true;
					//MatrixEx.MoveLocalYAxis( vDelta.y );
					m_pSelectedKeyPos->y += vDelta.y;
					break;

				case CAxisRenderObject::AXIS_Z:
					bSelectedAxis = true;
					//MatrixEx.MoveLocalZAxis( vDelta.z );
					m_pSelectedKeyPos->z += vDelta.z;
					break;
				}

				// 맵에 y 위치가 맞도록 되어있다면..
				if( iter->first->bFitYPosToMap )
					m_pSelectedKeyPos->y = m_pWorld->GetHeight( m_pSelectedKeyPos->x, m_pSelectedKeyPos->z );
				
				m_bEditedKeyInfo = true;

				break;
			}
		}


		//--------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------

		if( bSelectedAxis == false )	
		{
			EtVector3 vLength = *m_pSelectedKeyPos - vStart;
			float fRayStartToKeyLength = EtVec3Length( &vLength );

			vDelta = vEnd - vStart;
			vDelta *= fRayStartToKeyLength/10.0f;

			// 마우스 delta 값을 적용
			*m_pSelectedKeyPos += vDelta;

			if( m_SelectedKeyInfo.bFitYPosToMap )
				m_pSelectedKeyPos->y = m_pWorld->GetHeight( m_pSelectedKeyPos->x, m_pSelectedKeyPos->z );

			m_bEditedKeyInfo = true;
		}

		m_iPrevClkXPos = iXPos;
		m_iPrevClkYPos = iYPos;
	}
	else
	if( m_pSelectedMaxCamOffset )
	{
		assert( m_pSelectedMaxCamOffset );

		const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( m_iSelectedCameraEventID );
		if( pEventInfo )
		{
			const CamEventInfo* pCamEventInfo = static_cast<const CamEventInfo*>(pEventInfo);

			EtMatrix matWorld;
			EtMatrixIdentity( &matWorld );

			EtVector3 vMouseStart( (float)m_iPrevClkXPos, (float)m_iPrevClkYPos, 0.0f );
			EtVector3 vMouseEnd( (float)iXPos, (float)iYPos, 0.0f );

			EtVector3 vStart, vEnd, vDelta;
			EtViewPort Viewport;
			GetEtDevice()->GetViewport( &Viewport );
			EtVec3Unproject( &vStart, &vMouseStart, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );
			EtVec3Unproject( &vEnd, &vMouseEnd, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );

			EtVector3 vCamPos;
			if( m_bSelectedStartPos )
				vCamPos = pCamEventInfo->pCameraData->m_vecPosition.front().vPosition;
			else
				vCamPos = pCamEventInfo->pCameraData->m_vecPosition.back().vPosition;
							
			EtVector3 vEditedPos = *m_pSelectedMaxCamOffset + vCamPos;
			EtVector3 vLength = vEditedPos - vStart;
			float fRayStartToCamOffsetLength = EtVec3Length( &vLength );

			vDelta = vEnd - vStart;
			vDelta *= fRayStartToCamOffsetLength/10.0f;

			*m_pSelectedMaxCamOffset += vDelta;
			m_bEditedKeyInfo = true;

			m_iPrevClkXPos = iXPos;
			m_iPrevClkYPos = iYPos;
		}
	}
	else
	if( NULL == m_pSelectedActor )
	{
		m_pCamController->OnRButtonCon( iXPos, iYPos );

		EtVector3 vCamPos = m_pCamController->GetCamPos();
		EtVector3 vLookAt = m_pCamController->GetLookAt();
		EtVector3 vUp = m_pCamController->GetUpVector();

		m_hCamera->LookAt( vCamPos, vLookAt, vUp );
	}

}


void CRTCutSceneRenderer::OnLButtonUp( int iXPos, int iYPos )
{
	if( m_bEditedKeyInfo )
	{
		if( false == m_SelectedKeyInfo.strActorName.empty() )
		{
			EtVector3 vTemp = *m_pSelectedKeyPos;
			
			// 실제 데이터 값의 원래 위치를 복구 시켜주고 정식으로 커맨드 날려서 값을 바꿔준다.
			if( m_bSelectedStartPos )
			{
				*m_pSelectedKeyPos = m_SelectedKeyInfo.vStartPos;
				m_SelectedKeyInfo.vStartPos = vTemp;
			}
			else
			{
				if( -1 == m_iSelectedSubKey )
				{
					*m_pSelectedKeyPos = m_SelectedKeyInfo.vDestPos;
					m_SelectedKeyInfo.vDestPos = vTemp;
				}
				else
				{
					*m_pSelectedKeyPos = m_SelectedKeyInfo.vlMoveKeys.at(m_iSelectedSubKey).vPos;
					m_SelectedKeyInfo.vlMoveKeys.at(m_iSelectedSubKey).vPos = vTemp;
				}
			}

			CKeyPropChange KeyChange( &TOOL_DATA, &m_SelectedKeyInfo );
			TOOL_DATA.RunCommand( &KeyChange );

			if( m_bSelectedKeyAxis == false && m_bSelectKeyInfo == false )
			{
				m_SelectedKeyInfo.strActorName.clear();
				m_iSelectedSubKey = -1;
				m_bEditedKeyInfo = false;
				m_bSelectedStartPos = false;
				m_pSelectedKeyPos = NULL;
			}
		}
		else
		if( m_bShowMaxCameraPath )
		{
			EtVector3 vTemp = *m_pSelectedMaxCamOffset;
			*m_pSelectedMaxCamOffset = m_vOriMaxCamOffset;

			const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( m_iSelectedCameraEventID );
			if( pEventInfo )
			{
				EventInfo* pNewCamEvent = pEventInfo->clone();
				static_cast<CamEventInfo*>(pNewCamEvent)->vCamStartOffset = vTemp;
				CEventPropChange EventDataChange( &TOOL_DATA, pNewCamEvent );
				TOOL_DATA.RunCommand( &EventDataChange );

				delete pNewCamEvent;

				m_bEditedKeyInfo = false;
				m_bSelectedStartPos = false;
				m_pSelectedMaxCamOffset = NULL;
			}
		}
	}
}


void CRTCutSceneRenderer::OnRButtonUp( int iXPos, int iYPos )
{
	//if( bIsLockSelect() )
	//	OnLButtonUp( iXPos, iYPos );
}

void CRTCutSceneRenderer::OnRButtonDrag( int iXPos, int iYPos )
{
	/*if( bIsLockSelect() )
	{			
		m_pCamController->OnRButtonCon( iXPos, iYPos );

		EtVector3 vCamPos = m_pCamController->GetCamPos();
		EtVector3 vLookAt = m_pCamController->GetLookAt();
		EtVector3 vUp = m_pCamController->GetUpVector();

		m_hCamera->LookAt( vCamPos, vLookAt, vUp );		
	}
	else*/
		m_pCamController->OnLButtonCon( iXPos, iYPos );	
}


void CRTCutSceneRenderer::OnCButtonDrag( int iXPos, int iYPos )
{
	m_pCamController->OnCButtonCon( iXPos, iYPos );

	EtVector3 vCamPos = m_pCamController->GetCamPos();
	EtVector3 vLookAt = m_pCamController->GetLookAt();
	EtVector3 vUp = m_pCamController->GetUpVector();

	m_hCamera->LookAt( vCamPos, vLookAt, vUp );

	wxString strCursorPos;
	strCursorPos.Printf( wxT("(%2.2f, %2.2f, %2.2f)"), vLookAt.x, vLookAt.y, vLookAt.z );
	static_cast<CRTCutSceneMakerFrame*>( TOOL_DATA.GetMainFrame() )->SetCursorPosText( strCursorPos );
}


void CRTCutSceneRenderer::OnMouseWheel( int iWheelDelta )
{
	m_pCamController->OnMouseWheel( -iWheelDelta );

	EtVector3 vCamPos = m_pCamController->GetCamPos();
	EtVector3 vLookAt = m_pCamController->GetLookAt();
	EtVector3 vUp = m_pCamController->GetUpVector();

	m_hCamera->LookAt( vCamPos, vLookAt, vUp );
}

// bintitle.
#include "wx/event.h"
void CRTCutSceneRenderer::OnKeyDown(wxKeyEvent& event)
{
	//if( event.GetKeyCode() == WXK_UP )
	//	m_pCamController->OnMoveFrontAndBack( -30.0f );

	//else if( event.GetKeyCode() == WXK_DOWN )
	//	m_pCamController->OnMoveFrontAndBack( 30.0f );

	//else if( event.GetKeyCode() == WXK_LEFT )
	//	m_pCamController->OnMoveLeftAndRight( 30.0f );

	//else if( event.GetKeyCode() == WXK_RIGHT )
	//	m_pCamController->OnMoveLeftAndRight( -30.0f );

	switch( event.GetKeyCode() )
	{
	case WXK_UP :
		m_pCamController->OnMoveFrontAndBack( -30.0f );
		break;

	case WXK_DOWN :
		m_pCamController->OnMoveFrontAndBack( 30.0f );
		break;

	case WXK_LEFT :
		m_pCamController->OnMoveLeftAndRight( 30.0f );
		break;

	case WXK_RIGHT :
		m_pCamController->OnMoveLeftAndRight( -30.0f );
		break;

	case WXK_NUMPAD7 :

		break;

	case WXK_NUMPAD9 :

		break;
	}

	
	EtVector3 vCamPos = m_pCamController->GetCamPos();
	EtVector3 vLookAt = m_pCamController->GetLookAt();
	EtVector3 vUp = m_pCamController->GetUpVector();

	m_hCamera->LookAt( vCamPos, vLookAt, vUp );
}

void CRTCutSceneRenderer::OnKeyUp(wxKeyEvent& event)
{
	switch( event.GetKeyCode() )
	{
	case WXK_F4 : 
		if( m_pSelectedActor )
		{
			AddLockActor( m_pSelectedActor );
			_DeSelectActor();
		}
		break;

	case WXK_F5 : ClearLockActor();
		break;
	}
}

void CRTCutSceneRenderer::AddLockActor( CDnCutSceneActor * pActor )
{
	if( IsLockActor( pActor ) )
		return;

	// Lock 추가.
	m_mapLockSelectedActor.insert( make_pair( std::wstring(pActor->GetName()), pActor ) );
}

void CRTCutSceneRenderer::ClearLockActor()
{
	m_mapLockSelectedActor.clear();
}

bool CRTCutSceneRenderer::IsLockActor( CDnCutSceneActor * pActor )
{		
	std::wstring strName( pActor->GetName() );
	std::map<std::wstring, CDnCutSceneActor *>::iterator it = m_mapLockSelectedActor.find( strName );
	if( it != m_mapLockSelectedActor.end() )
	{
		if( it->first == strName )
			return true;
	}

	return false;
}


void CRTCutSceneRenderer::ToggleWeapon( bool bWeapon )
{
	CDnCutSceneActor * pActor = NULL;
	int size = m_vlpActors.size();
	for( int i=0; i<size; ++i )
	{
		pActor = m_vlpActors[ i ];
		if( pActor->GetActorType() == CDnCutSceneActor::PLAYER )
		{
			pActor->ShowWeapon( bWeapon );
		}
	}	
}


bool CRTCutSceneRenderer::IsObjectSelected( void )
{
	bool bResult = false;

	switch( m_iSelectedObjectType )
	{
		case SO_ACTOR:
			bResult = m_pSelectedActor != NULL ? true : false;
			break;

		case SO_PARTICLE:
			bResult = m_hSelectedParticleEvent != NULL ? true : false;
			break;

		//case SO_MAX_CAMERA:
		//	bResult = m_Selected
		//	break;
	}

	return bResult;
}



void CRTCutSceneRenderer::UnSelectObject( void )
{
	switch( m_iSelectedObjectType )
	{
		case SO_ACTOR:
			_DeSelectActor();
			break;

		case SO_PARTICLE:
			SAFE_RELEASE_SPTR( m_hSelectedParticleEvent );
			m_iSelectedObjectType = SO_COUNT;
			break;

		case SO_MAX_CAMERA:
			m_iSelectedObjectType = SO_COUNT;
			m_iSelectedCameraEventID = -1;
			m_strSelectedCamEventName.clear();
			break;
	}
}



void CRTCutSceneRenderer::UpdateSelectedObject( void )
{
	if( m_hSelectedParticleEvent )
	{
		const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( m_iSelectedParticleEventID );
		if( pEventInfo )
		{
			const ParticleEventInfo* pParticleEvent = static_cast<const ParticleEventInfo*>(pEventInfo);
			EtMatrix matWorld;
			EtMatrixTranslation( &matWorld, pParticleEvent->vPos.x, pParticleEvent->vPos.y, pParticleEvent->vPos.z );
			m_hSelectedParticleEvent->SetWorldMat( &matWorld );
		}
	}

	//switch( m_iSelectedObjectType )
	//{
	//	//case SO_ACTOR:
	//	//	m_pSelectedActor = NULL;
	//	//	m_iSelectedObjectType = SO_COUNT;
	//	//	break;

	//	case SO_PARTICLE:
	//		{
	//			if( m_hSelectedParticleEvent )
	//			{
	//				const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( iID );
	//				if( pEventInfo )
	//				{
	//					const ParticleEventInfo* pParticleEvent = static_cast<const ParticleEventInfo*>(pEventInfo);
	//					EtMatrix matWorld;
	//					EtMatrixTranslation( &matWorld, pParticleEvent->vPos.x, pParticleEvent->vPos.y, pParticleEvent->vPos.z );
	//					m_hSelectedParticleEvent->SetWorldMat( matWorld );
	//				}
	//			}
	//		}
	//		//OnSelectEventUnit( CToolData::PARTICLE, m_iSelectedParticleEventID );
	//		//SelectParticle( m_iSelectedParticleEventID );
	//		break;
	//}
}



	
const wchar_t* CRTCutSceneRenderer::GetSelectedObjectName( void )
{
	const wchar_t* pResult = NULL;

	switch( m_iSelectedObjectType )
	{
		case SO_ACTOR:
			pResult = m_pSelectedActor->GetName();
			break;

		case SO_PARTICLE:
			pResult = m_strSelectedParticleName.c_str();
			break;

		case SO_MAX_CAMERA:
			pResult = m_strSelectedCamEventName.c_str();
			break;
	}

	return pResult;
}





CDnCutSceneActor* CRTCutSceneRenderer::_FindActorByName( const wchar_t* pActorName )
{
	CDnCutSceneActor* pResult = NULL;

	int iNumActor = (int)m_vlpActors.size();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		CDnCutSceneActor* pActor = m_vlpActors.at( iActor );
		if( wcscmp(pActor->GetName(), pActorName) == 0 )
		{
			pResult = pActor;
			break;
		}
	}
		
	return pResult;
}





void CRTCutSceneRenderer::SetActorAlpha( const wchar_t* pActorName, float fAlpha )
{
	CDnCutSceneActor* pActor = _FindActorByName( pActorName );

	if( pActor )
	{
		pActor->GetAniObjectHandle()->SetObjectAlpha( fAlpha );
	}
}


void CRTCutSceneRenderer::SetActorScale( const wchar_t* pActorName, float fScale )
{
	CDnCutSceneActor* pActor = _FindActorByName( pActorName );

	if( pActor )
	{
		pActor->SetScale( fScale );
	}
	
}




bool CRTCutSceneRenderer::MoveObjectByDragging( const wchar_t* pActorName, int iMouseXPos, int iMouseYPos )
{
	if( m_pSelectedKeyPos || m_pSelectedMaxCamOffset )
	{
		return false;
	}

	EtMatrix matWorld;
	EtMatrixIdentity( &matWorld );

	EtVector3 vMouseStart( (float)m_iPrevClkXPos, (float)m_iPrevClkYPos, 0.0f );
	EtVector3 vMouseEnd( (float)iMouseXPos, (float)iMouseYPos, 0.0f );

	EtVector3 vStart, vEnd, vDelta;
	EtViewPort Viewport;
	GetEtDevice()->GetViewport( &Viewport );
	EtVec3Unproject( &vStart, &vMouseStart, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );
	EtVec3Unproject( &vEnd, &vMouseEnd, &Viewport, m_hCamera->GetProjMat(), m_hCamera->GetViewMat(), &matWorld );

	EtVector3 vNewPos;

	bool bProcessed = false;

	// 선택된 축이 있으면 이동시켜 줌
	// axis 에 클릭이 됐는지
	map<CDnCutSceneActor*, CAxisRenderObject*>::iterator iter = m_mapActorAxisObject.begin();
	for( iter; iter != m_mapActorAxisObject.end(); ++iter )
	{
		int iSelectedAxis = iter->second->GetSelectedAxis();
		if( CAxisRenderObject::AXIS_NONE != iSelectedAxis )
		{
			MatrixEx MatrixEx = *(iter->first->GetMatrixEx());

			EtVector3 vLength = MatrixEx.GetPosition() - vStart;
			float fRayStartToKeyLength = EtVec3Length( &vLength );

			vDelta = vEnd - vStart;
			vDelta *= fRayStartToKeyLength/10.0f;

			//EtMatrix matRot = MatrixEx;
			//matRot._41 = matRot._42 = matRot._43 = 0.0f;
			//matRot._11 = matRot._22 = matRot._33 = matRot._44 = 1.0f;

			EtMatrix matRot;
			float fYRotation = iter->first->GetRotationY();
			EtMatrixRotationY( &matRot, fYRotation*ET_PI / 180.0f );

			EtVec3TransformCoord( &vDelta, &vDelta, &matRot );

			switch( iSelectedAxis )
			{
				case CAxisRenderObject::AXIS_X:
					MatrixEx.MoveLocalXAxis( vDelta.x );
					break;

				case CAxisRenderObject::AXIS_Y:
					MatrixEx.MoveLocalYAxis( vDelta.y );
					break;

				case CAxisRenderObject::AXIS_Z:
					MatrixEx.MoveLocalZAxis( vDelta.z );
					break;
			}

			vNewPos = MatrixEx.GetPosition();

			bool bFitYPosToMap = TOOL_DATA.GetThisActorsFitYPosToMap( pActorName );

			// 맵에 y 위치가 맞도록 되어있다면..
			if( bFitYPosToMap )
				vNewPos.y = m_pWorld->GetHeight( vNewPos.x, vNewPos.z );

			iter->first->SetPosition( vNewPos );
			bProcessed = true;
			break;
		}
	}

	if( false == bProcessed )
	{	
		switch( m_iSelectedObjectType )
		{
			case SO_ACTOR:
				{
					// 실제 포지션 변경 커맨드를 날린다.
					CDnCutSceneActor* pActor = _FindActorByName( pActorName );

					if( pActor )
					{
						vNewPos = *pActor->GetPosition();//TOOL_DATA.GetRegResPos( pActorName );
					}

					EtVector3 vCamPos = m_pCamController->GetCamPos();
					EtVector3 vLength = vNewPos - vStart;
					float fCamToActorLength = EtVec3Length( &vLength );

					vDelta = vEnd - vStart;
					vDelta *= fCamToActorLength/10.0;

					vNewPos += vDelta;

					bool bFitYPosToMap = TOOL_DATA.GetThisActorsFitYPosToMap(pActorName);

					// 맵에 y 위치가 맞도록 되어있다면..
					if( bFitYPosToMap )
						vNewPos.y = m_pWorld->GetHeight( vNewPos.x, vNewPos.z );

					pActor->SetPosition( vNewPos );

				}
				break;

			case SO_PARTICLE:
				{
					EtMatrix* pMatWorld = m_hSelectedParticleEvent->GetWorldMat();
					vNewPos.x = pMatWorld->_41;
					vNewPos.y = pMatWorld->_42;
					vNewPos.z = pMatWorld->_43;

					EtVector3 vCamPos = m_pCamController->GetCamPos();
					EtVector3 vLength = vNewPos - vStart;
					float fCamToActorLength = EtVec3Length( &vLength );

					vDelta = vEnd - vStart;
					vDelta *= fCamToActorLength/10.0;

					vNewPos += vDelta;

					pMatWorld->_41 = vNewPos.x;
					pMatWorld->_42 = vNewPos.y;
					pMatWorld->_43 = vNewPos.z;
				}
				break;

			case SO_MAX_CAMERA:
				{

				}
				break;
		}
	}

	m_iPrevClkXPos = iMouseXPos;
	m_iPrevClkYPos = iMouseYPos;

	return true;
}



void CRTCutSceneRenderer::GetSelectedObjectPos( EtVector3* pvPos/*const wchar_t* pActorName*/ )
{
	//const EtVector3* pResult = NULL;

	switch( m_iSelectedObjectType )
	{
		case SO_ACTOR:
			if( m_pSelectedActor )
			{
				*pvPos = *m_pSelectedActor->GetPosition();
			}
			break;

		case SO_PARTICLE:
			{
				EtMatrix* pMatWorld = m_hSelectedParticleEvent->GetWorldMat();
				pvPos->x = pMatWorld->_41;
				pvPos->y = pMatWorld->_42;
				pvPos->z = pMatWorld->_43;
			}
			break;

		case SO_MAX_CAMERA:
			{
				
			}
			break;
	}
}


int CRTCutSceneRenderer::GetSelectedObjectID( void )
{
	int iResult = -1;

	switch( m_iSelectedObjectType )
	{
		// TODO: 아직 액터에서는 쓰지 않음
		case SO_ACTOR:
			break;

		case SO_PARTICLE:
			iResult = m_iSelectedParticleEventID;
			break;

		case SO_MAX_CAMERA:
			break;
	}

	return iResult;
}



void CRTCutSceneRenderer::RotateObjectByWheelDelta( int iWheelDelta )
{
	if( m_pSelectedActor )
	{
		float fRotDegree = m_pSelectedActor->GetRotationY();
		fRotDegree += (float)iWheelDelta*0.1f;

		if( fRotDegree > 360.0f )
			fRotDegree -= 360.0f;
		else
		if( fRotDegree < -360.0f )
			fRotDegree += 360.0f;

		m_pSelectedActor->SetRotationY( fRotDegree );
	}
}


float CRTCutSceneRenderer::GetRenderActorRotDegree( const wchar_t* pActorName )
{
	float fResult = 0.0f;

	if( pActorName )
	{
		tstring strFindActorName( pActorName );

		map<tstring, CDnCutSceneActor*>::iterator iter = m_mapActors.find( strFindActorName.c_str() );
		if( m_mapActors.end() != iter )
		{
			if( strFindActorName == iter->second->GetName() )
			{
				fResult = iter->second->GetRotationY();
			}
		}

		//if( m_pSelectedActor )
		//{
		//	fResult = m_pSelectedActor->GetRotationY();
		//}
	}

	return fResult;
}


void CRTCutSceneRenderer::OnPostLoadFile( void )
{
	int iNumActor = (int)m_vlpActors.size();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		CDnCutSceneActor* pActor = m_vlpActors.at( iActor );
		int iNumAniElement = TOOL_DATA.GetThisActorsAnimationNum( pActor->GetName() );
		
		if( 0 == iNumAniElement )
		{
			_FillActorsAniElementData( pActor );
		}

		// 보스 몬스터로 셋팅된 애들이 스케일 값이 다를 수가 있으므로 스케일 값을 셋팅해준다.
		wxString strActorResName;
		wxString strActorName;
		strActorName = pActor->GetName();
		strActorResName = strActorName.substr( 0, strActorName.find_last_of(wxT("_")) );
		const S_ACTOR_RES_INFO* pActorResInfo = TOOL_DATA.GetActorResInfoByName( strActorResName.c_str() );
		vector<const S_MONSTER_INFO*> vlResult;
		TOOL_DATA.GatherBossInfoOfThisActor( pActorResInfo->iTableID, vlResult );
		int iMonsterTableID = TOOL_DATA.GetMonsterTableID( pActor->GetName() );
		for( int i = 0; i < (int)vlResult.size(); ++i )
		{
			const S_MONSTER_INFO* pBossMonsterInfo = vlResult.at( i );

			if( iMonsterTableID == pBossMonsterInfo->iMonsterTableID )
			{
				pActor->SetScale( pBossMonsterInfo->m_fScale );
				break;
			}
		}
	}
}



void CRTCutSceneRenderer::OnSelectEventUnit( int iEventType, int iID )
{
	// 카메라나 기타 등등 다 숨김
	m_bShowMaxCameraPath = false;
	m_iSelectedCameraEventID = -1;
	SAFE_RELEASE_SPTR( m_hCameraMesh );
	//m_hCameraMesh->Release();

	switch( iEventType )
	{
		case CToolData::MAX_CAMERA:
				{
					const CamEventInfo* pCamEventInfo = static_cast<const CamEventInfo*>( TOOL_DATA.GetEventInfoByID(iID) );
					assert( pCamEventInfo );
					if( pCamEventInfo )
					{
						wxChar wcaBuffer[ MAX_PATH ];
						ZeroMemory( wcaBuffer, sizeof(wcaBuffer) );
						wxCSConv MBConv( wxFONTENCODING_CP949 );
						MBConv.MB2WC( wcaBuffer, pCamEventInfo->strEventName.c_str(), 256 );

						m_strSelectedCamEventName = wcaBuffer;

						m_bShowMaxCameraPath = true;
						m_iSelectedCameraEventID = iID;

						m_iSelectedObjectType = SO_MAX_CAMERA;

						// 이건 나중에 하자.. 메시 나중에 다시 달라고 해서,, 일단 핵심 기능 먼저 하자.
						//m_hCameraMesh = EternityEngine::CreateStaticObject( "Camera_mesh.msh" );
						// m_hCameraMesh->GetVertexStream()
					}
				}
				break;

		case CToolData::PARTICLE:
			{
				const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( iID );

				if( pEventInfo )
				{
					// 액터가 선택되어있다면 해제시켜 줌
					_DeSelectActor();

					SAFE_RELEASE_SPTR( m_hSelectedParticleEvent );

					m_iSelectedObjectType = SO_PARTICLE;
					m_iSelectedParticleEventID = iID;

					const ParticleEventInfo* pParticleEvent = static_cast<const ParticleEventInfo*>(pEventInfo);
					EtMatrix matWorld;

					EtMatrixTranslation( &matWorld, pParticleEvent->vPos.x, pParticleEvent->vPos.y, pParticleEvent->vPos.z );

					if( -1 != pParticleEvent->iParticleDataIndex )
					{
						m_hSelectedParticleEvent = EternityEngine::CreateBillboardEffect( pParticleEvent->iParticleDataIndex, &matWorld );
						m_hSelectedParticleEvent->EnableLoop( true );

						wxCSConv MBConv( wxFONTENCODING_CP949 );
						wchar_t wcaBuf[ 256 ];
						ZeroMemory( wcaBuf, sizeof(wcaBuf) );
						MBConv.MB2WC( wcaBuf, pParticleEvent->strEventName.c_str(), 256 );
						m_strSelectedParticleName.assign( wcaBuf );
					}

					// 카메라가 자동으로 보도록 해준다.
					EtVector3 vLookAt = pParticleEvent->vPos;
					EtVector3 vCamPos( vLookAt.x, vLookAt.y+100.0f, vLookAt.z-300.0f );
					EtVector3 vUp( 0.0f, 1.0f, 0.0f );

					m_pCamController->SetOriCamPos( vCamPos );
					m_pCamController->SetOriLookAt( vLookAt );
					m_pCamController->SetOriUpVector( vUp );

					m_hCamera->LookAt( vCamPos, vLookAt, vUp );
				}

			}
			break;
	}
}



void CRTCutSceneRenderer::SetRenderFog( bool bRenderFog )
{
	bool bChanged = (bRenderFog != m_bRenderFog);

	if( bChanged )
	{
		m_bRenderFog = bRenderFog;

		EtVector3 vCamPos = m_pCamController->GetCamPos();
		EtVector3 vLookAt = m_pCamController->GetLookAt();
		EtVector3 vUp = m_pCamController->GetUpVector();

		if( m_bRenderFog )
		{
			SAFE_RELEASE_SPTR( m_hCamera );

			SCameraInfo CamInfo;
			CamInfo.fFar = m_pWorld->GetFogFar() + 200000.f; // 임시 20미터
			CamInfo.fFogNear = m_pWorld->GetFogNear();
			CamInfo.fFogFar = m_pWorld->GetFogFar();
			CamInfo.FogColor = m_pWorld->GetFogColor();

			m_hCamera = CreateCamera( &CamInfo );
			m_hCamera->LookAt( vCamPos, vLookAt, vUp );
		}
		else
		{
			SAFE_RELEASE_SPTR( m_hCamera );

			SCameraInfo CamInfo;
			CamInfo.fFar = 100000.0f;
			CamInfo.fFogNear = 100000.0f;
			CamInfo.fFogFar = 100000.0f;

			m_hCamera = CreateCamera( &CamInfo );
			m_hCamera->LookAt( vCamPos, vLookAt, vUp );
		}

		if( PLAY_MODE == m_iMode )
			m_pCutScenePlayer->SetCamera( m_hCamera );
	}
}


void CRTCutSceneRenderer::OnSelecteProp( int iPropID )
{
	if( -1 != iPropID )
	{
		CEtWorldProp* pProp = m_pWorld->GetSector( 0.0f, 0.0f )->GetPropFromCreateUniqueID( iPropID );
		if( pProp )
		{
			CEtWorldSector* pSector = m_pWorld->GetSector( 0.0f, 0.0f );
			int iWidthCount = pSector->GetTileWidthCount();
			int iHeightCount = pSector->GetTileHeightCount();
			float fTileSize = pSector->GetTileSize();

			EtVector3 vPos = *(pProp->GetPosition()) - EtVector3( (iWidthCount*fTileSize)/2.0f, 0.0f, (iHeightCount*fTileSize)/2.0f );
			m_pPropAxisObject->SetPosition( vPos );
			m_pPropAxisObject->SetRotation( *(pProp->GetRotation()) );
		}

		m_pPropAxisObject->Show( true );
		m_bShowAxis = true;
	}
	else
	{
		m_pPropAxisObject->Show( false );
		m_bShowAxis = false;
	}
}


void CRTCutSceneRenderer::ShowSubtitle( const wchar_t* pSubtitle )
{
	//m_pCaptionDlg->SetCaption( pSubtitle );
	//m_pCaptionDlg->Show( true );
	m_strSubtitle.assign( pSubtitle );
	m_bShowSubtitle = true;
}


void CRTCutSceneRenderer::HideSubtitle( void )
{
	m_bShowSubtitle = false;
	//m_pCaptionDlg->Show( false );
}



void CRTCutSceneRenderer::_SelectActor( CDnCutSceneActor* pActor )
{
	m_pSelectedActor = pActor;

	CAxisRenderObject* pAxisObject = _GetAxisRenderObject();
	m_mapActorAxisObject.insert( make_pair(m_pSelectedActor, pAxisObject) );

	m_iSelectedObjectType = SO_ACTOR;	
}

void CRTCutSceneRenderer::SelectKeyInfo( const KeyInfo * pKey )
{		
	KeyInfo * _key = const_cast<KeyInfo *>( pKey );
	map<KeyInfo*, CAxisRenderObject*>::iterator it = m_mapKeyInfoAxisObject.find( _key );
	if( it != m_mapKeyInfoAxisObject.end() )
		return;

	_DeSelectKeyAxis();

	CAxisRenderObject * pAxisObject = _GetAxisRenderObject();
	m_mapKeyInfoAxisObject.insert( make_pair(_key, pAxisObject) );
}


void CRTCutSceneRenderer::_DeSelectActor( void )
{
	if( m_pSelectedActor )
	{
		map<CDnCutSceneActor*, CAxisRenderObject*>::iterator iter = m_mapActorAxisObject.find( m_pSelectedActor );
		if( m_mapActorAxisObject.end() != iter )
		{
			_ReturnAxisRenderObject( iter->second );
			m_mapActorAxisObject.erase( iter );
		}

		m_pSelectedActor->GetAniObjectHandle()->SetObjectAlpha( 1.0f );
		m_pSelectedActor = NULL;
		m_iSelectedObjectType = SO_COUNT;
	}
}

void CRTCutSceneRenderer::_DeSelectKeyAxis()
{
	map<KeyInfo*, CAxisRenderObject*>::iterator it = m_mapKeyInfoAxisObject.begin();
	for( ; it != m_mapKeyInfoAxisObject.end(); it++ )
		_ReturnAxisRenderObject( it->second );
	m_mapKeyInfoAxisObject.clear();
}


const EtVector3& CRTCutSceneRenderer::GetLookAt( void )
{
	 return m_pCamController->GetLookAt();
}


void CRTCutSceneRenderer::ChangeActor( const wchar_t* pOldActorName, const wchar_t* pNewActorResName )
{
	if( NULL == pOldActorName )
		return;

	const S_ACTOR_RES_INFO* pActorResInfo = TOOL_DATA.GetActorResInfoByName( pNewActorResName );
	if( NULL == pActorResInfo )
		return;

	map<tstring, CDnCutSceneActor*>::iterator iter = m_mapActors.find( pOldActorName );
	if( m_mapActors.end() != iter )
	{
		// 새로 만들 액터를 만들고 해당 액션이 있는지 다 검사한다.
		wxCSConv MBConv( wxFONTENCODING_CP949 );
		char acBuf[ 256 ];
		ZeroMemory( acBuf, sizeof(acBuf) );
		MBConv.WC2MB( acBuf, pNewActorResName, 256 );

		// 기존 이름 그대로, 문자열을 변경시켜서 바꿔준다.
		// 기존 것은 맵에서 제거. 새로 추가한다. v
		tstring strOriActorName = iter->first;
		int iOffset = (int)strOriActorName.find_last_of( wxT("_") );
		tstring strDescription = strOriActorName.substr( iOffset, strOriActorName.length() );

		tstring strNewActorName( pNewActorResName );
		strNewActorName.append( strDescription.c_str() );

		CDnCutSceneActor* pNewActor = _CreateActor( strNewActorName.c_str(), acBuf );

		// 이미 커맨드쪽에서 기존의 액터 이름은 새로 바뀔 액터 이름으로 바뀌어있으므로
		// 액션 시퀀스들을 찾으려면 새로운 이름으로 찾아야 한다.
		int iNumAction = TOOL_DATA.GetThisActorsActionNum( strNewActorName.c_str() );
		for( int iAction = 0; iAction < iNumAction; ++iAction )
		{
			const ActionInfo* pActionInfo = TOOL_DATA.GetThisActorsActionInfoByIndex( strNewActorName.c_str(), iAction );
			const ActionEleInfo* pActionEleInfo = pNewActor->GetElement( pActionInfo->strAnimationName.c_str() );
			assert( pActionEleInfo );
			if( pActionEleInfo )
				const_cast<ActionInfo*>(pActionInfo)->iAnimationIndex = pNewActor->GetElementIndex( pActionInfo->strAnimationName.c_str() );
		}

		// 따로 받아뒀던 자료구조들 갱신. 
		set<tstring>::iterator iterActorSet = m_setActors.find( pOldActorName );
		_ASSERT( m_setActors.end() != iterActorSet );

		if( m_setActors.end() != iterActorSet )
			m_setActors.erase( iterActorSet );
		m_setActors.insert( strNewActorName );

		int iNumActors = (int)m_vlpActors.size();
		for( int i = 0; i < iNumActors; ++i )
		{
			if( iter->second == m_vlpActors.at( i ) )
			{
				m_vlpActors.at( i ) = pNewActor;
			}
		}

		map<CDnCutSceneActor*, CAxisRenderObject*>::iterator iterAxisMap = m_mapActorAxisObject.begin();
		for( iterAxisMap; iterAxisMap != m_mapActorAxisObject.end(); ++iterAxisMap )
		{
			if( iterAxisMap->first == iter->second )
			{
				m_mapActorAxisObject.insert( make_pair(pNewActor, iterAxisMap->second) );
				m_mapActorAxisObject.erase( iterAxisMap );
				break;
			}
		}

		pNewActor->SetPosition( *(iter->second->GetPosition()) );
		pNewActor->SetRotationY( iter->second->GetRotationY() );

		if( iter->second == m_pSelectedActor )
			m_pSelectedActor = pNewActor;

		delete iter->second;
		m_mapActors.erase( iter );

		m_mapActors.insert( make_pair(strNewActorName, pNewActor) );
	}
}


bool CRTCutSceneRenderer::CanChangeActor( const wchar_t* pOldActorName, const wchar_t* pNewActorResName )
{
	if( NULL == pOldActorName )
		return false;

	const S_ACTOR_RES_INFO* pActorResInfo = TOOL_DATA.GetActorResInfoByName( pNewActorResName );
	if( NULL == pActorResInfo )
		return false;

	bool bCanChangeActor = true;

	map<tstring, CDnCutSceneActor*>::iterator iter = m_mapActors.find( pOldActorName );
	if( m_mapActors.end() != iter )
	{	
		// 기존의 액터와 애니메이션이 호환되는지 다 체크한다.
		wxCSConv MBConv( wxFONTENCODING_CP949 );
		char acBuf[ 256 ];
		ZeroMemory( acBuf, sizeof(acBuf) );

		MBConv.WC2MB( acBuf, pNewActorResName, 256 );
		CDnCutSceneActor* pTempActor = _LoadActorFromResource( acBuf );

		wxString strMessage;
		wxChar wcaBuf[ 256 ];
		int iNumAction = TOOL_DATA.GetThisActorsActionNum( pOldActorName );
		for( int iAction = 0; iAction < iNumAction; ++iAction )
		{
			const ActionInfo* pActionInfo = TOOL_DATA.GetThisActorsActionInfoByIndex( pOldActorName, iAction );
			const ActionEleInfo* pActionEleInfo = pTempActor->GetElement( pActionInfo->strAnimationName.c_str() );
			if( NULL == pActionEleInfo )
			{
				ZeroMemory( wcaBuf, sizeof(wcaBuf) );
				MBConv.MB2WC( wcaBuf, pActionInfo->strAnimationName.c_str(), 256 );

				wxString strBuf;
				strBuf.Printf( wxT("[%s], "), wcaBuf );
				strMessage.Append( strBuf );
				bCanChangeActor = false;
			}
		}

		if( false == bCanChangeActor )
		{
			// 액터 바꿀 수 없다. 중지.
			strMessage.Append( wxT("액션이 없어서 액터를 바꿀 수 없습니다.") );
			wxMessageBox( strMessage, wxT("Error!!"), wxOK|wxCENTRE|wxICON_ERROR, TOOL_DATA.GetMainFrame() );

			delete pTempActor;
		}
	}

	return bCanChangeActor;
}