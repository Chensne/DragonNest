// EngineTest.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "EngineTest.h"
#include "EternityEngine.h"
#include "CrossVector.h"
#include "CameraData.h"
#include "FrameSync.h"
#include "EtOptionController.h"
#include "EtResourceMng.h"

//#include "EtUIDialog.h"

using namespace EternityEngine;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

HWND g_hWnd;
CCrossVector g_CameraCross;
EtCameraHandle g_hCamera, g_hCamera2;
EtAniObjectHandle g_hObject, g_hObject2;
EtObjectHandle g_hPickObj;
CCrossVector g_PickObjCross;
float g_fFrame = 0.0f;
POINT g_MousePoint;
EtTerrainHandle g_hTerrain;
EtParticleHandle g_hParticle;
CCrossVector g_PatCross;
int g_nEffectDataIndex = -1;

EtMatrix g_LightViewMat;

CCrossVector g_AniObjCross;
EtAniObjectHandle g_hAniObject;
float g_fFrame1 = 0.0f;
float g_fFrame2 = 0.0f;

EtLineTrailHandle g_hLineTrail;
CCrossVector g_LineTrailCross;
bool g_bLineTrailStart = false;

float g_fCameraFrame = 0.0f;
CCameraData g_CameraData;

CFrameSync g_FrameSync;
extern EtTextureHandle g_hDepthMap;

void ProcessCamera()
{
	int i;
	EtQuat Quat1, Quat2;
	EtVector3 vPosition1, vPosition2;
	float fPositionWeight, fQuatWeight;

	for( i = 0; i < ( int )g_CameraData.m_vecPosition.size(); i++ )
	{
		if( g_CameraData.m_vecPosition[ i ].nTime > g_fCameraFrame )
		{
			vPosition1 = g_CameraData.m_vecPosition[ i - 1 ].vPosition;
			vPosition2 = g_CameraData.m_vecPosition[ i ].vPosition;
			fPositionWeight = ( g_fCameraFrame - g_CameraData.m_vecPosition[ i - 1 ].nTime ) / ( g_CameraData.m_vecPosition[ i ].nTime - g_CameraData.m_vecPosition[ i - 1 ].nTime );
			EtVec3Lerp( &vPosition1, &vPosition1, &vPosition2, fPositionWeight );
			break;
		}
	}

	for( i = 0; i < ( int )g_CameraData.m_vecRotation.size(); i++ )
	{
		if( g_CameraData.m_vecRotation[ i ].nTime > g_fCameraFrame )
		{
			Quat1 = g_CameraData.m_vecRotation[ i - 1 ].qRotation;
			Quat2 = g_CameraData.m_vecRotation[ i ].qRotation;
			fQuatWeight = ( g_fCameraFrame - g_CameraData.m_vecRotation[ i - 1 ].nTime ) / ( g_CameraData.m_vecRotation[ i ].nTime - g_CameraData.m_vecRotation[ i - 1 ].nTime );
			EtQuaternionSlerp( &Quat1, &Quat1, &Quat2, fQuatWeight );
			break;
		}
	}

	EtMatrix CameraMat;

	EtMatrixTransformation( &CameraMat, NULL, NULL, NULL, NULL, &Quat1, &vPosition1 );
	g_hCamera->Update( &CameraMat );

//	g_fCameraFrame = 0.0f;
	g_fCameraFrame += 0.05f;

	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 )
	{
		g_fCameraFrame = 0.0f;
	}
}

float CalcMovement( float &fCurSpeed, float fElapsedTime, float fMaxSpeed, float fMinSpeed, float fAccel )
{
	float fFinalSpeed, fMovement, fDiffTime;

	fFinalSpeed = fCurSpeed + fElapsedTime * fAccel;
	if( ( fFinalSpeed > fMaxSpeed ) && ( fCurSpeed <= fMaxSpeed ) )
	{
		fDiffTime =  ( fMaxSpeed - fCurSpeed ) / ( fFinalSpeed - fCurSpeed ) * fElapsedTime;
		fMovement = fCurSpeed * fDiffTime + fAccel * fDiffTime * fDiffTime * 0.5f;
		fMovement += fMaxSpeed * ( fElapsedTime - fDiffTime );
		fCurSpeed = fMaxSpeed;
	}
	else if( ( fFinalSpeed < fMinSpeed ) && ( fCurSpeed >= fMinSpeed ) )
	{
		fDiffTime =  ( fCurSpeed - fMinSpeed ) / ( fCurSpeed - fFinalSpeed ) * fElapsedTime;
		fMovement = fCurSpeed * fDiffTime + fAccel * fDiffTime * fDiffTime * 0.5f;
		fMovement += fMinSpeed * ( fElapsedTime - fDiffTime );
		fCurSpeed = fMinSpeed;
	}
	else
	{
		fMovement = fCurSpeed * fElapsedTime + fAccel * fElapsedTime * fElapsedTime * 0.5f;
		fCurSpeed = fFinalSpeed;
	}

	return fMovement;
}

void TestAccel()
{
	EtVector3 Accel, Gravity, Position;
	float fTime;

	int i;
	Accel = EtVector3( 0.0f, 3600.0f, 0.0f );
	Gravity = EtVector3( 0.0f, -980.0f, 0.0f );
	Position = EtVector3( 0.0f, 0.0f, 0.0f );

	fTime = 5.0f;
	Position.x += CalcMovement( Accel.x, fTime, 6000.0f, 0.0f, Gravity.x );
	Position.y += CalcMovement( Accel.y, fTime, 6000.0f, 0.0f, Gravity.y );
	Position.z += CalcMovement( Accel.z, fTime, 6000.0f, 0.0f, Gravity.z );
	Accel = EtVector3( 0.0f, 3600.0f, 0.0f );
	Gravity = EtVector3( 0.0f, -980.0f, 0.0f );
	Position = EtVector3( 0.0f, 0.0f, 0.0f );
	fTime = 10.0f;
	Position.x += CalcMovement( Accel.x, fTime, 6000.0f, 0.0f, Gravity.x );
	Position.y += CalcMovement( Accel.y, fTime, 6000.0f, 0.0f, Gravity.y );
	Position.z += CalcMovement( Accel.z, fTime, 6000.0f, 0.0f, Gravity.z );


	for( i = 0; i < 40; i++ )
	{
		fTime = 0.1f;
		Position.x += CalcMovement( Accel.x, fTime, 6000.0f, -FLT_MAX, Gravity.x );
		Position.y += CalcMovement( Accel.y, fTime, 6000.0f, -FLT_MAX, Gravity.y );
		Position.z += CalcMovement( Accel.z, fTime, 6000.0f, -FLT_MAX, Gravity.z );
	}

	Accel = EtVector3( 0.0f, 3600.0f, 0.0f );
	Position = EtVector3( 0.0f, 0.0f, 0.0f );
	for( i = 0; i < 20; i++ )
	{
		fTime = 0.2f;
		Position.x += CalcMovement( Accel.x, fTime, 6000.0f, -FLT_MAX, Gravity.x );
		Position.y += CalcMovement( Accel.y, fTime, 6000.0f, -FLT_MAX, Gravity.y );
		Position.z += CalcMovement( Accel.z, fTime, 6000.0f, -FLT_MAX, Gravity.z );
	}

	Accel = EtVector3( 0.0f, 3600.0f, 0.0f );
	Position = EtVector3( 0.0f, 0.0f, 0.0f );
	for( i = 0; i < 10; i++ )
	{
		fTime = 0.4f;
		Position.x += CalcMovement( Accel.x, fTime, 6000.0f, -FLT_MAX, Gravity.x );
		Position.y += CalcMovement( Accel.y, fTime, 6000.0f, -FLT_MAX, Gravity.y );
		Position.z += CalcMovement( Accel.z, fTime, 6000.0f, -FLT_MAX, Gravity.z );
	}

	Accel = EtVector3( 0.0f, 3600.0f, 0.0f );
	Position = EtVector3( 0.0f, 0.0f, 0.0f );
	fTime = 4.0f;
	Position.x += CalcMovement( Accel.x, fTime, 6000.0f, -FLT_MAX, Gravity.x );
	Position.y += CalcMovement( Accel.y, fTime, 6000.0f, -FLT_MAX, Gravity.y );
	Position.z += CalcMovement( Accel.z, fTime, 6000.0f, -FLT_MAX, Gravity.z );

	int qqq;
	qqq = 0;
}

/*class CCollisionDraw : public CEtCustomRender
{
public:
	CCollisionDraw(){}
	virtual ~CCollisionDraw(){}

public:
	void RenderAlpha( float fElapsedTime );
};

CCollisionDraw *g_CustomDraw;*/

/*#include "CollisionTest.h"
#include "EtGenerateCollisionMesh.h"
CCrossVector g_ColCross1, g_ColCross2;
SSphere g_Sphere1, g_Sphere2;
SRay g_Ray1, g_Ray2;
STriangle g_Tri1, g_Tri2;
SBox g_Box1, g_Box2;
SCylinder g_Cylinder1, g_Cylinder2;
SCapsule g_Capsule1, g_Capsule2;
CEtCustomMeshStream g_ColDraw1, g_ColDraw2;
DWORD g_dwLineColor = 0xffffffff;

void InitCollision()
{
	g_Sphere1.Center = EtVector3( 0.0f, 300.0f, 100.0f );
	g_Sphere1.fRadius = 100.0f;
	GenerateCollisionMesh( g_ColDraw1, g_Sphere1 );

	g_Tri1.vPoint[ 0 ] = EtVector3( 400.0f, 300.0f, 100.0f );
	g_Tri1.vPoint[ 1 ] = EtVector3( 500.0f, 400.0f, 100.0f );
	g_Tri1.vPoint[ 2 ] = EtVector3( 600.0f, 300.0f, 100.0f );
	g_Tri1.vNormal = EtVector3( 0.0f, 0.0f, 1.0f );
	GenerateCollisionMesh( g_ColDraw2, g_Tri1 );

	g_CustomDraw = new CCollisionDraw();
}*/
/*void CheckCollisionMove()
{
	SFindCollisionInfo CollisionInfo;
	if( FindSphereToTri( g_Sphere1, g_Tri1, CollisionInfo ) )
	{
		g_dwLineColor = 0xffff0000;
	}
	else
	{
		g_dwLineColor = 0xffffffff;
	}

	if( GetAsyncKeyState( VK_LEFT ) & 0x8000 )
	{
		g_Sphere1.Center -= g_CameraCross.m_XVector;
		GenerateCollisionMesh( g_ColDraw1, g_Sphere1 );
	}
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 )
	{
		g_Sphere1.Center += g_CameraCross.m_XVector;
		GenerateCollisionMesh( g_ColDraw1, g_Sphere1 );
	}
	if( GetAsyncKeyState( VK_UP ) & 0x8000 )
	{
		g_Sphere1.Center += g_CameraCross.m_ZVector;
		GenerateCollisionMesh( g_ColDraw1, g_Sphere1 );
	}
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000 )
	{
		g_Sphere1.Center -= g_CameraCross.m_ZVector;
		GenerateCollisionMesh( g_ColDraw1, g_Sphere1 );
	}
	if( GetAsyncKeyState( VK_HOME ) & 0x8000 )
	{
		g_Sphere1.Center += g_CameraCross.m_YVector;
		GenerateCollisionMesh( g_ColDraw1, g_Sphere1 );
	}
	if( GetAsyncKeyState( VK_END ) & 0x8000 )
	{
		g_Sphere1.Center -= g_CameraCross.m_YVector;
		GenerateCollisionMesh( g_ColDraw1, g_Sphere1 );
	}
}

void CCollisionDraw::RenderAlpha( float fElapsedTime )
{
	EtMatrix WorldMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	EtMatrixIdentity( &WorldMat );
	GetEtDevice()->SetWorldTransform( &WorldMat );
	GetEtDevice()->SetViewTransform( hCamera->GetViewMat() );
	GetEtDevice()->SetProjTransform( hCamera->GetProjMat() );
	GetEtDevice()->EnableLight( false );
	GetEtDevice()->SetVertexShader( NULL );
	GetEtDevice()->SetPixelShader( NULL );
	( ( IDirect3DDevice9 * )GetEtDevice()->GetDevicePtr() )->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	( ( IDirect3DDevice9 * )GetEtDevice()->GetDevicePtr() )->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	( ( IDirect3DDevice9 * )GetEtDevice()->GetDevicePtr() )->SetRenderState( D3DRS_TEXTUREFACTOR, g_dwLineColor );
	( ( IDirect3DDevice9 * )GetEtDevice()->GetDevicePtr() )->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	g_ColDraw1.Draw( 0, 0, g_ColDraw1.GetIndexCount() / 3 );
	g_ColDraw2.Draw( 0, 0, g_ColDraw2.GetIndexCount() / 3 );
	( ( IDirect3DDevice9 * )GetEtDevice()->GetDevicePtr() )->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}
*/

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;
	BOOL bGotMsg;
	bool bShaderDebug;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ENGINETEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	TestAccel();

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENGINETEST));

	bShaderDebug = false;
	if( strstr( ( const char *)lpCmdLine, ( const char *)L"ShaderDeubg" ) )
	{
		bShaderDebug = true;
	}

	SOBB Box;
	SSegment Segment;
	Box.Center = EtVector3( 200.0f, 200.0f, 200.0f );
	Box.Axis[ 0 ] = EtVector3( 1.0f, 0.0f, 0.0f );
	Box.Axis[ 1 ] = EtVector3( 0.0f, 1.0f, 0.0f );
	Box.Axis[ 2 ] = EtVector3( 0.0f, 0.0f, 1.0f );
	Box.Extent[ 0 ] = 100.0f;
	Box.Extent[ 1 ] = 100.0f;
	Box.Extent[ 2 ] = 100.0f;
	Box.CalcVertices();
	Segment.vOrigin = EtVector3( 200.0f, 200.0f, 0.0f );
	Segment.vDirection = EtVector3( 400.0f, 200.0f, 400.0f ) - Segment.vOrigin;

	SGraphicOption Option;
	Option.DynamicShadowType = ST_NONE;
//	InitializeEngine( g_hWnd, 1016, 734, true, false, bShaderDebug );
	InitializeEngine( g_hWnd, 1600, 900, true, false, NULL, bShaderDebug );

	char szCurDir[ _MAX_PATH ], szTargetDir[ _MAX_PATH ];
	GetCurrentDirectoryA( _MAX_PATH, szCurDir );
	strcat( szCurDir, "\\Data\\" );
	CEtResourceMng *pResMng = new CEtResourceMng( false );
	pResMng->Initialize();
	CEtResourceMng::GetInstance().AddResourcePath( szCurDir );
	strcpy( szTargetDir, szCurDir );
	strcat( szTargetDir, "Ogre\\" );
	CEtResourceMng::GetInstance().AddResourcePath( szTargetDir );
	strcpy( szTargetDir, szCurDir );
	strcat( szTargetDir, "Warrior\\" );
	CEtResourceMng::GetInstance().AddResourcePath( szTargetDir );
	strcpy( szTargetDir, szCurDir );
	strcat( szTargetDir, "Archer\\" );
	CEtResourceMng::GetInstance().AddResourcePath( szTargetDir );
	GetCurrentDirectoryA( _MAX_PATH, szCurDir );
	strcat( szCurDir, "\\Data\\" );
	strcat( szCurDir, "Shaders\\" );
	CEtResourceMng::GetInstance().AddResourcePath( szCurDir );
	InitilaizeEffect();

	g_FrameSync.Begin( -1 );

//	CFileStream Stream( "D:\\Work\\DragonNest\\Client\\EngineTest\\Data\\Scene01.cam" );
//	g_CameraData.LoadResource( &Stream );

//	InitCollision();

	SLightInfo LightInfo;

	LightInfo.bCastShadow = false;
//	LightInfo.Direction = EtVector3( 0.38933533f, -0.85627538f, -0.30888590f );
	LightInfo.Direction = EtVector3( -1.0f, -1.0f, 1.0f );
//	LightInfo.Direction = EtVector3( -0.78558189f, -0.58445191f, -0.20316739f );
	EtVec3Normalize( &LightInfo.Direction, &LightInfo.Direction );
	CreateLight( &LightInfo );
	SetGlobalAmbient( &EtColor( 0.4f, 0.4f, 0.4f, 1.0f ) );
//	SetGlobalAmbient( &EtColor( 0.0f, 0.0f, 0.0f, 1.0f ) );

	LightInfo.Diffuse = EtColor( 1.0f, 0.0f, 0.0f, 1.0f );
	LightInfo.Direction = EtVector3( 1.0f, -1.0f, 1.0f );
//	CreateLight( &LightInfo );

	SCameraInfo CameraInfo;
/*	CameraInfo.fNear = 3.0f;
	CameraInfo.fFar = 300.0f;*/

/*	CameraInfo.Type = CT_ORTHOGONAL;
	CameraInfo.fViewWidth = 2000.0f;
	CameraInfo.fViewHeight = 2000.0f;*/

	CameraInfo.fFar = 50000.0f;
//	CameraInfo.fFar = 100000.0f;

	CameraInfo.fFogNear = 40000.0f;
	CameraInfo.fFogFar = 50000.0f;
	CameraInfo.fFOV = ET_PI / 4;
	g_hCamera = CreateCamera( &CameraInfo );
	g_CameraCross.SetPosition( EtVector3( 402.0f, 300.0f, -206.0f ) );
//	g_CameraCross.SetPosition( EtVector3( 0.0f, 45.0f, 0.0f ) );

	g_CameraCross.SetPosition( EtVector3( 402.0f, 300.0f, -206.0f ) );
	g_CameraCross.RotateYaw( -10 );
	g_CameraCross.RotatePitch( 15 );
	g_hCamera->Update( g_CameraCross );

	int i, j;
#if 0
	for( i = 0; i < 5; i++ )
	{
		for( j = 0; j < 5; j++ )
		{
			EtAniObjectHandle hObjHandle;
			CCrossVector ObjCross;
			ObjCross.SetPosition( EtVector3( i * 200.0f + 200.0f, 0.0f, j * 200.0f + 200.0f ) );
			hObjHandle = CreateAniObject( "Ogre.skn", "Ogre.ani" );
			hObjHandle->Update( ObjCross );
			hObjHandle->SetAniFrame( 0, ( float )( rand() % hObjHandle->GetAniLength( 0 ) ) );
			hObjHandle->EnableShadowCast( false );
//			hObjHandle->ShowBoundingBox( true );
		}
	}

/*	srand( 2000 );
	for( i = 0; i < 100; i++ )
	{
		EtObjectHandle hObjHandle;
		CCrossVector ObjCross;
		ObjCross.SetPosition( EtVector3( ( float )( rand() % 25600 ), 0.0f, ( float )( rand() % 25600 ) ) );
		hObjHandle = CreateStaticObject( "Tree01.skn" );
		hObjHandle->Update( ObjCross );
		hObjHandle->EnableLightMapCast( true );
	}*/

/*	EtAniObjectHandle g_hAniObject;
	g_AniObjCross.SetPosition( EtVector3( 400.0f, 0.0f, 400.0f ) );
//	g_AniObjCross.RotateYaw( 30.0f );
	g_hAniObject = CreateAniObject( "Archer.skn", "Archer.ani" );
//	g_hAniObject->LinkSkin( "Sword.skn", "BoxBone01" );
	g_hAniObject->Update( g_AniObjCross );
	g_hAniObject->ShowBoundingBox( true );*/
//	g_hAniObject->SetObjectAlpha( 0.5f );
//	hObjHandle->Update( ObjCross );

/*	int nBoneIndex;
	EtObjectHandle hWeapon;
	nBoneIndex = g_hAniObject->GetAniHandle()->GetBoneIndex( "BoxBone01" );
	hWeapon = CreateStaticObject( "Sword.skn" );
	hWeapon->SetParent( g_hAniObject, nBoneIndex );*/

/*	LoadEnvTexture( "BlueSky_Cloud01.dds" );

	EtObjectHandle  hShpere;
	CCrossVector SphereCross;
	SphereCross.SetPosition( EtVector3( 0.0f, 300.0f, 0.0f ) );
	hShpere = CreateStaticObject( "Sphere.skn" );
	hShpere->Update( SphereCross );*/

#endif

	CCrossVector ObjCross;
	ObjCross.SetPosition( EtVector3( 400.0f, 0.0f, 400.0f ) );
	ObjCross.RotateYaw( 180.0f );
	g_hObject = CreateAniObject( "Warrior.skn", "Warrior.ani" );
	g_hObject->EnableShadowCast( true );
	g_hObject->Update( ObjCross );

	ObjCross.SetPosition( EtVector3( 600.0f, 0.0f, 400.0f ) );
//	g_hObject2 = CreateAniObject( "Ogre.skn", "Warrior.ani" );
	g_hObject2 = CreateAniObject( "Ogre.skn", "Ogre.ani" );
	g_hObject2->EnableShadowCast( true );
	g_hObject2->Update( ObjCross );

#if 0
	CCrossVector g_PatCross;
	int nParticleDataIndex;
	g_PatCross.SetPosition( EtVector3( 500.0f, 200.0f, 200.0f ) );
	nParticleDataIndex = LoadParticleData( "brooMagicball_Small.ptc" );
	g_hParticle = CreateParticle( nParticleDataIndex, g_PatCross );
	g_hParticle->EnableLoop( true );
	g_hParticle->EnableIterate( true );
#endif

#if 0
	g_nEffectDataIndex = LoadEffectData( "battest.eff" );
#endif

#if 1
	STerrainInfo TerrainInfo;
	short *pHeight;
	DWORD *pLayer;
	char *pGrass;

	pHeight = new short[ 129 * 129 ];
	pLayer = new DWORD[ 129 * 129 ];
	pGrass = new char[ 128 * 128 ];
	TerrainInfo.pHeight = pHeight;
	TerrainInfo.pLayerDensity = pLayer;
	TerrainInfo.pGrassBuffer = pGrass;
	TerrainInfo.fGrassWidth[ 0 ] = 300.0f;
	TerrainInfo.fGrassWidth[ 1 ] = 300.0f;
	TerrainInfo.fGrassWidth[ 2 ] = 300.0f;
	TerrainInfo.fGrassWidth[ 3 ] = 300.0f;
	TerrainInfo.fMinGrassHeight[ 0 ] = 100.0f;
	TerrainInfo.fMinGrassHeight[ 1 ] = 100.0f;
	TerrainInfo.fMinGrassHeight[ 2 ] = 100.0f;
	TerrainInfo.fMinGrassHeight[ 3 ] = 100.0f;
	TerrainInfo.fMaxGrassHeight[ 0 ] = 150.0f;
	TerrainInfo.fMaxGrassHeight[ 1 ] = 150.0f;
	TerrainInfo.fMaxGrassHeight[ 2 ] = 150.0f;
	TerrainInfo.fMaxGrassHeight[ 3 ] = 150.0f;

	for( i = 0; i < 129; i++ )
	{
		for( j = 0; j < 129; j++ )
		{
			pHeight[ i * 129 + j ] = 0;
			pLayer[ i * 129 + j ] = 0xff000000;
		}
	}

	memset( pGrass, 0, 128 * 128 );
	for( i = 0; i < 13500; i++ )
	{
		pGrass[ ( rand() % 25 ) * 128 + ( rand() % 25 ) ] = rand() % 4 + 1;
//		pGrass[ rand() % ( 128 * 128 ) ] = rand() % 4 + 1;
	}

	for( i = 0; i < 50; i++ )
	{
		for( j = 0; j < 50; j++ )
		{
//			pGrass[ i * 128 + j ] = 0x01;
		}
	}

//	g_hTerrain = CreateTerrain( &TerrainInfo );

	for( i = 0; i < 129; i++ )
	{
		for( j = 0; j < 129; j++ )
		{
			float fLength;

			fLength = sqrtf( float( ( j - 10 ) * ( j - 10 ) + ( i - 10 ) * ( i - 10 ) ) );
			if( fLength > 8.0f )
			{
				pHeight[ i * 129 + j ] = 0;
			}
			else if( fLength > 3.0f )
			{
				pHeight[ i * 129 + j ] = ( short )( 4000 - fLength * 500 );
			}
			else
			{
				pHeight[ i * 129 + j ] = 2500;
			}
			if( fLength > 8.9f )
			{
				pLayer[ i * 129 + j ] = 0xff000000;
			}
			else
			{
//				pLayer[ i * 129 + j ] = 0xff000000;
				pLayer[ i * 129 + j ] = 0x000000ff;
			}


			fLength = sqrtf( float( ( j - 30 ) * ( j - 30 ) + ( i - 10 ) * ( i - 10 ) ) );
			if( fLength > 8.0f )
			{
//				pHeight[ i * 129 + j ] = 0;
			}
			else if( fLength > 3.0f )
			{
				//				pHeight[ i * 129 + j ] = 2000;
				pHeight[ i * 129 + j ] = ( short )( 2000 - fLength * 250 );
			}
			else
			{
				pHeight[ i * 129 + j ] = 1250;
			}
			if( fLength <= 8.9f )
			{
				pLayer[ i * 129 + j ] = 0x000000ff;
			}
			pLayer[ i * 129 + j ] = 0x000000ff;
//			pHeight[ i * 129 + j ] = 0.0f;

			fLength = sqrtf( float( ( j - 60 ) * ( j - 60 ) + ( i - 60 ) * ( i - 60 ) ) );
			if( fLength > 8.0f )
			{
				//				pHeight[ i * 129 + j ] = 0;
			}
			else if( fLength > 3.0f )
			{
				//				pHeight[ i * 129 + j ] = 2000;
				pHeight[ i * 129 + j ] = ( short )( 2000 - fLength * 250 );
			}
			else
			{
				pHeight[ i * 129 + j ] = 1250;
			}
			if( fLength <= 8.9f )
			{
				pLayer[ i * 129 + j ] = 0x000000ff;
			}
			pLayer[ i * 129 + j ] = 0x000000ff;
			fLength = sqrtf( float( ( j - 55 ) * ( j - 55 ) + ( i - 10 ) * ( i - 10 ) ) );
			if( fLength > 8.0f )
			{
				//				pHeight[ i * 129 + j ] = 0;
			}
			else if( fLength > 3.0f )
			{
				//				pHeight[ i * 129 + j ] = 2000;
				pHeight[ i * 129 + j ] = ( short )( 2000 - fLength * 250 );
			}
			else
			{
				pHeight[ i * 129 + j ] = 1250;
			}
			if( fLength <= 8.9f )
			{
				pLayer[ i * 129 + j ] = 0x000000ff;
			}
			pLayer[ i * 129 + j ] = 0x000000ff;

			pHeight[ i * 129 + j ] = 0;
		}
	}

	TerrainInfo.Type = TT_CLIFF;
	TerrainInfo.fTextureDistance = 500.0f;
	TerrainInfo.TerrainOffset = EtVector3( -1000.0f, 0.0f, -1000.0f );
	g_hTerrain = CreateTerrain( &TerrainInfo );
	g_hTerrain->InitializeBlock();
	g_hTerrain->SetTexture( 0, "GroundGrass.dds" );
	g_hTerrain->SetTexture( 1, "GroundGrass.dds" );
	g_hTerrain->SetTexture( 2, "GroundGrass.dds" );
	g_hTerrain->SetTexture( 3, "GroundGrass.dds" );
	g_hTerrain->SetGrassTexture( "grassPack.dds" );

//	EternityEngine::CreateFilter( SF_RADIALBLUR );

//	g_hTerrain->DrawGrid( true, 0 );

/*	g_hTerrain->SetTexture( 4, "dry_earth04.dds" );
	g_hTerrain->SetTexture( 5, "dry_earth04.dds" );
	g_hTerrain->SetTexture( 6, "dry_earth04.dds" );
	g_hTerrain->SetTexture( 7, "dry_earth04.dds" );
	g_hTerrain->SetTextureDistance( 4, 100000.0f );
	g_hTerrain->SetTextureDistance( 5, 100000.0f );
	g_hTerrain->SetTextureDistance( 6, 100000.0f );
	g_hTerrain->SetTextureDistance( 7, 100000.0f );*/

//	delete [] pHeight;
//	delete [] pLayer;

//	CreateFilter( SF_COLORADJUST2 );
//	CEtBloomFilter *pFilter;
//	pFilter = ( CEtBloomFilter * )CreateFilter( SF_BLOOM );
//	pFilter->SetBloomSensitivity( 1.0f );

/*	EtObjectHandle hObjHandle;
	CCrossVector ObjCross;
//	ObjCross.SetPosition( EtVector3( 400.0f, 0.0f, 1000.0f ) );
	ObjCross.SetPosition( EtVector3( 0.0f, 0.0f, 0.0f ) );
	ObjCross.RotateYaw( 90.0f );
	hObjHandle = CreateStaticObject( "Elfland_Root01.skn" );
	hObjHandle->Update( ObjCross );
	hObjHandle->EnableLightMapCast( true );
	hObjHandle->ShowBoundingBox( true );
	hObjHandle->ShowCollisionPrimitive( true );*/

	g_hTerrain->BakeLightMap( -1, 256, 256, 1 );

#if 0
	CEtUIDialog *pDialog;

	pDialog = new CEtUIDialog();
	pDialog->Initialize( "ttt" );
#endif

#endif

#if 0
	EtTextureHandle hTrailTexture;
	hTrailTexture = LoadTexture( "ArrowTrail.dds" );
	g_hLineTrail = CreateLineTrail( hTrailTexture, 0.3f, 20.0f, 30.0f );
//	g_hLineTrail->SetLineColor( EtColor( 1.0f, 0.0f, 0.0f, 1.0f ) );
	g_LineTrailCross.RotateYaw( -90.0f );
	g_LineTrailCross.SetPosition( EtVector3( 100.0f, 100.0f, 100.0f ) );
#endif

#if 0
	SCameraInfo RenderTargetCameraInfo;
	RenderTargetCameraInfo.Target = CT_RENDERTARGET;
	RenderTargetCameraInfo.fWidth = 256.0f;
	RenderTargetCameraInfo.fHeight = 256.0f;
	g_hCamera2 = CreateCamera( &RenderTargetCameraInfo );
	//	g_hCamera2->AddRenderObject( hObjHandle );
	g_hCamera2->Update( g_CameraCross );
	//	g_hCamera2->AddRenderSkin( "mada.skn", ObjCross );
	//	g_hCamera2->RenderSkinList();
	g_hCamera2->RenderTerrain();
	//	SAFE_RELEASE_SPTR( g_hCamera2 );
#endif

	// 기본 메시지 루프입니다.
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
	while ( WM_QUIT != msg.message )
	{
		bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
		if( bGotMsg )
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			float fZValue;
			EtVector4 Coord;
			EtMatrix *pViewMat, *pProjMat, ViewProjMat;
			pViewMat = g_hCamera->GetViewMat();
			pProjMat = g_hCamera->GetProjMat();
			EtMatrixMultiply( &ViewProjMat, pViewMat, pProjMat );
			Coord = EtVector4( 0.0f, 0.0f, 150.0f, 1.0f );
			EtVec4Transform( &Coord, &Coord, &ViewProjMat );
			fZValue = Coord.z / Coord.w;

			float fCalcZ;
			fCalcZ = pProjMat->_43 / ( fZValue - pProjMat->_33 );

			static float fMoveSpeed = 10.0f;
			CCrossVector ObjCross;

			if( GetAsyncKeyState( 'W' ) & 0x8000 )
			{
#if 0
				if( g_hAniObject )
				{
					EtVector3 CollisionNormal, Direction;
					float fContactTime;
					ObjCross = g_AniObjCross;
					ObjCross.MoveFrontBack( fMoveSpeed );
					if( g_hAniObject->TestCollision( ObjCross, CollisionNormal, fContactTime ) )
					{
						CollisionNormal.y = 0.0f;
						EtVec3Normalize( &CollisionNormal, &CollisionNormal );
						Direction = CollisionNormal + ObjCross.m_ZVector;
						EtVec3Normalize( &Direction, &Direction );
						Direction *= fMoveSpeed;
//						Direction *= fMoveSpeed * fabs( EtVec3Dot( &Direction, &ObjCross.m_ZVector ) );
					}
					else
					{
						Direction = fMoveSpeed * ObjCross.m_ZVector;
					}
					g_AniObjCross.m_PosVector = g_AniObjCross.m_PosVector + Direction;
				}
#else
				g_CameraCross.MoveFrontBack( fMoveSpeed );
#endif
			}
			if( GetAsyncKeyState( 'S' ) & 0x8000 )
			{
#if 0
				if( g_hAniObject )
				{
					g_AniObjCross.MoveFrontBack( -fMoveSpeed );
				}
#else
				g_CameraCross.MoveFrontBack( -fMoveSpeed );
#endif
			}
			if( GetAsyncKeyState( 'A' ) & 0x8000 )
			{
#if 0
				if( g_hAniObject )
				{
					g_AniObjCross.MoveRightLeft( -fMoveSpeed );
				}
#else
				g_CameraCross.MoveRightLeft( -fMoveSpeed );
#endif
			}
			if( GetAsyncKeyState( 'D' ) & 0x8000 )
			{
#if 0
				if( g_hAniObject )
				{
					g_AniObjCross.MoveRightLeft( fMoveSpeed );
				}
#else
				g_CameraCross.MoveRightLeft( fMoveSpeed );
#endif
			}
			if( GetAsyncKeyState( VK_F2 ) & 0x8000 )
			{
				g_CameraCross.m_ZVector = EtVector3( 0.0f, -1.0f, 1.01f );
				EtVec3Normalize( &g_CameraCross.m_ZVector, &g_CameraCross.m_ZVector );
				g_CameraCross.UpdateVectors();
				g_CameraCross.SetPosition( 402.0f, 300.0f, -206.0f );
			}
			if( GetAsyncKeyState( VK_F3 ) & 0x8000 )
			{
				g_CameraCross.m_ZVector = EtVector3( 0.0f, -1.01f, 1.0f );
				EtVec3Normalize( &g_CameraCross.m_ZVector, &g_CameraCross.m_ZVector );
				g_CameraCross.UpdateVectors();
				g_CameraCross.SetPosition( 402.0f, 300.0f, -206.0f );
			}
//			CheckCollisionMove();
			if( g_hObject )
			{
				static int nAniIndex = 0;
				g_hObject->SetAniFrame( nAniIndex, 0.0f );
				g_fFrame += 1.0f;
				if( g_fFrame >= g_hObject->GetAniLength( nAniIndex ) - 1 )
				{
					g_fFrame = 0.0f;
				}
				int nBoneIndex = g_hObject->GetAniHandle()->GetBoneIndex( "Bip01 Head" );
				static float fRotationRadius = 0.0f;
//				g_hObject->SetBoneRotation( nBoneIndex, EtVector3( fRotationRadius, 0.0f, 0.0f ) );
				fRotationRadius += 1.0f;
				if( fRotationRadius > 360.0f )
				{
					fRotationRadius = 0.0f;
				}
			}
			if( g_hObject2 )
			{
				static int nAniIndex2 = 7;
				g_hObject2->SetAniFrame( nAniIndex2, 0.0f );
				g_fFrame2 += 1.0f;
				if( g_fFrame2 >= g_hObject2->GetAniLength( nAniIndex2 ) - 1 )
				{
					g_fFrame2 = 0.0f;
				}
			}
			if( g_hAniObject )
			{
				static int nAni1 = 4;
				static int nAni2 = 17;
				EtVector3 Rotation1, Rotation2, Rotation3;
				g_hAniObject->SetAniFrame( nAni1, g_fFrame1 );
				g_hAniObject->SetAniFrame( nAni2, g_fFrame2, g_hAniObject->GetAniHandle()->GetBoneIndex( "Bip01 Spine" ) );
				Rotation1 = EtVector3( 0.0f, 45.0f, 0.0f );
				Rotation2 = EtVector3( 0.0f, 45.0f, 0.0f );
				Rotation3 = EtVector3( 0.0f, 15.0f, 0.0f );
/*				g_hAniObject->SetBoneRotation( g_hAniObject->GetAniHandle()->GetBoneIndex( "Bip01 Spine" ), Rotation1 );
				g_hAniObject->SetBoneRotation( g_hAniObject->GetAniHandle()->GetBoneIndex( "Bip01 Spine1" ), Rotation2 );
//				g_hAniObject->SetBoneRotation( g_hAniObject->GetAniHandle()->GetBoneIndex( "Bip01 R Clavicle" ), Rotation2 );
				g_hAniObject->SetBoneRotation( g_hAniObject->GetAniHandle()->GetBoneIndex( "Bip01 L Clavicle" ), Rotation3 );
				g_hAniObject->SetBoneRotation( g_hAniObject->GetAniHandle()->GetBoneIndex( "Bip01 Neck" ), Rotation3 );*/
				g_fFrame1 += 0.5f;
				if( g_fFrame1 >= g_hAniObject->GetAniLength( nAni1 ) - 1 )
				{
					g_fFrame1 = 0.0f;
				}
				g_fFrame2 += 0.5f;
				if( g_fFrame2 >= g_hAniObject->GetAniLength( nAni2 ) - 1 )
				{
					g_fFrame2 = 0.0f;
				}
				g_hAniObject->Update( g_AniObjCross );
			}
			if( g_hCamera )
			{
				g_hCamera->Update( g_CameraCross );
				
			}
			if( g_hCamera2 )
			{
				g_hCamera2->Update( g_CameraCross );
			}
			if( GetAsyncKeyState( VK_SPACE ) & 0x8000 )
			{
#if 1
				if( g_nEffectDataIndex != -1 )
				{
					CreateEffectObject( g_nEffectDataIndex, NULL );
				}
#endif
#if 0
				g_LineTrailCross.SetPosition( EtVector3( 100.0f, 100.0f, 100.0f ) );
				g_bLineTrailStart = true;
#endif
				while( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) 
				{
				}
			}
			if( ( g_hLineTrail ) && ( g_bLineTrailStart ) )
			{
				if( g_LineTrailCross.m_PosVector.x < 500.0f )
				{
					g_LineTrailCross.MoveUpDown( 5.0f );
				}
				else
				{
					g_LineTrailCross.MoveUpDown( -5.0f );
				}
				g_LineTrailCross.MoveFrontBack( 20.0f );
				g_hLineTrail->AddPoint( g_LineTrailCross.m_PosVector );
				if( g_LineTrailCross.m_PosVector.x > 300.0f )
				{
					g_bLineTrailStart = false;
				}
			}
			
//			ProcessCamera();

			g_FrameSync.CheckSync();
			g_FrameSync.UpdateTime();
			char szString[ 1024 ];
			sprintf_s( szString, 1024, "FPS : %f", g_FrameSync.GetFps() );
			EternityEngine::DrawText2D( EtVector2( 0.01f, 0.01f ), szString, 0xffffffff );

//			if( GetEtOptionController()->IsEnableBakeDepthMap() )
//			{
//				GetEtDevice()->SetRenderTarget( g_hDepthMap->GetSurfaceLevel(), 1 );
//			}
			ClearScreen( 0xff000000, 1.0f, 0 );
			RenderFrame();
			ShowFrame( NULL );
		}
	}

	return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
//  설명:
//
//    Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
//    해당 코드가 Win32 시스템과 호환되도록
//    하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
//    해당 응용 프로그램에 연결된
//    '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ENGINETEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ENGINETEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1600, 900, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;
	    
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
			case 'X':
				g_hTerrain->UpdateHeight( 0, 0, 128, 128 );
				break;
			case 'C':
				g_hTerrain->UpdateLayer( 0, 100, 30, 125 );
				break;
			case 'Z':
//				g_hTerrain->ChangeBlockType( TT_CLIFF );
//				ReinitializeEngine( 1024, 768 );
//				g_hTerrain->SetTextureDistance( 3, 10000.0f );
//				g_hTerrain->UpdateTextureCoord( 0, 0, 10, 10 );
				break;
		}
		break;
	case WM_LBUTTONDOWN:
	{
		EtVector3 PickPos;
		POINT ptCursor;
		GetCursorPos( &ptCursor );
		ScreenToClient( g_hWnd, &ptCursor );

//		g_hPickObj = Pick( ptCursor.x, ptCursor.y );
/*		if( g_hTerrain )
		{
			g_hTerrain->Pick( ptCursor.x, ptCursor.y, PickPos );
		}*/
		break;
	}
	case WM_LBUTTONUP:
	{
		g_hPickObj.Identity();
		break;
	}
	case WM_MOUSEMOVE:
		POINT CurPoint;

		CurPoint.x = LOWORD( lParam );
		CurPoint.y = HIWORD( lParam );
		if( wParam == MK_LBUTTON )
		{
			g_CameraCross.RotateYawByWorld( ( float )( CurPoint.x - g_MousePoint.x ) );
			g_CameraCross.RotatePitch( ( float )( CurPoint.y - g_MousePoint.y ) );
		}
		else if( wParam == MK_RBUTTON )
		{
			g_CameraCross.MoveRightLeft( ( float )( g_MousePoint.x - CurPoint.x ) );
			g_CameraCross.MoveUpDown( ( float )( CurPoint.y - g_MousePoint.y ) );
		}
		g_MousePoint = CurPoint;
		break;
	case WM_MOUSEWHEEL:
		if( HIWORD( wParam ) - WHEEL_DELTA > 0 )
		{
			g_CameraCross.MoveFrontBack( -20 );
		}
		else
		{
			g_CameraCross.MoveFrontBack( 20 );
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		FinalizeEngine();
		CEtResource::DeleteAllObject();
		PostQuitMessage(0);
		break;
	default:
		{
/*			if( CEtUIDialog::StaticMsgProc( hWnd, message, wParam, lParam ) )
			{
				return 1;
			}
			else*/
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
	}

/*	if( CEtUIDialog::StaticMsgProc( hWnd, message, wParam, lParam ) )
	{
		return 1;
	}*/

	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
