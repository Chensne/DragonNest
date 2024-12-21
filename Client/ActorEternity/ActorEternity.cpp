/**********************************************************************
 *<
	FILE: ActorEt.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#include "Stdafx.h"
#include "Windows.h"
#include "ActorEternity.h"
#include "phyexp.h"	
#include "bipexp.h"	
#include "DumpUtil.h"
#include "CollisionPrimitive.h"
#include "SaveCamera.h"
//#include "SaveNavigation.h"

#include "MAXScrpt.h"
#include "definsfn.h"

def_visible_primitive( save_ani,"SaveAni"); 


#define CAPS_RADIUS		0	// these are the ParamBlock indexes
#define CAPS_HEIGHT		1
#define CAPS_CENTERS	2
#define CAPS_SIDES		3
#define CAPS_HSEGS		4
#define CAPS_SMOOTHON	5
#define CAPS_SLICEON	6
#define CAPS_SLICEFROM	7
#define CAPS_SLICETO	8
#define CAPS_GENUVS		9

static Class_ID CAPS_CLASS_ID(0x6d3d77ac, 0x79c939a9);


#define ActorEt_CLASS_ID	Class_ID(0x60f6a2ef, 0xd1ef347d)

ActorEt theActorEt;


class ActorEtClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return &theActorEt; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return ActorEt_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("ActorEt"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	

};

static ActorEtClassDesc ActorEtDesc;
ClassDesc2* GetActorEtDesc() { return &ActorEtDesc; }


int g_nAniQuality = 0;


static BOOL CALLBACK ActorEtDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
			theActorEt.Init(hWnd);
			break;

		case WM_DESTROY:
			theActorEt.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch ( HIWORD( wParam ) ) 
			{
				case EN_SETFOCUS: DisableAccelerators(); 
					break;
				case EN_KILLFOCUS: EnableAccelerators(); 
					break;
			}		
			switch ( LOWORD( wParam ) ) 
			{
				case IDC_SAVEMESH:
					theActorEt.SaveMesh( hWnd );
					break;
				case IDC_SAVEANI:
					theActorEt.SaveAni( hWnd );
					break;
				case IDC_SAVECAMERA:
					theActorEt.SaveCamera( hWnd );
					break;
				case IDC_SAVE_NAVIGATION:
					theActorEt.SaveNavigation( hWnd );
					break;
			}
			break;


		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			theActorEt.m_pMaxInterface->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK ActorEtOptionDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			theActorEt.InitOption( hWnd );
			break;

		case WM_DESTROY:
			theActorEt.DestroyOption( hWnd );
			break;

		case WM_COMMAND:
			switch ( LOWORD( wParam ) ) 
			{
				case IDC_EXPORT_SELECTED:
					theActorEt.m_nExportSelected = _GetCheckBox( hWnd, IDC_EXPORT_SELECTED );
					theActorEt.m_RegUtil.SetKeyValue( "ExportSelected", theActorEt.m_nExportSelected );
					break;
				case IDC_USE_STRIP:
					theActorEt.m_nUseStrip = _GetCheckBox( hWnd, IDC_USE_STRIP );
					theActorEt.m_RegUtil.SetKeyValue( "UseStrip", theActorEt.m_nUseStrip );
					break;
				case IDC_GENERATE_LOD:
					theActorEt.m_nGenerateLOD = _GetCheckBox( hWnd, IDC_GENERATE_LOD );
					theActorEt.m_RegUtil.SetKeyValue( "GenerateLOD", theActorEt.m_nGenerateLOD );
					break;
				case IDC_GENERATE_UV_ANI:
					theActorEt.m_nGenerateUVAni = _GetCheckBox( hWnd, IDC_GENERATE_UV_ANI );
					theActorEt.m_RegUtil.SetKeyValue( "GenerateUVAni", theActorEt.m_nGenerateUVAni );
					break;
				case IDC_EXPORT_VERTEXCOLOR:
					theActorEt.m_nExportVertexColor = _GetCheckBox( hWnd, IDC_EXPORT_VERTEXCOLOR );
					theActorEt.m_RegUtil.SetKeyValue( "ExportVertexColor", theActorEt.m_nExportVertexColor );
					break;
				case IDC_EXPORT_TARGET_CAMERA:
					theActorEt.m_nExportTargetCamera = _GetCheckBox( hWnd, IDC_EXPORT_TARGET_CAMERA );
					theActorEt.m_RegUtil.SetKeyValue( "ExportTargetCamera", theActorEt.m_nExportTargetCamera );
					break;
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}



//--- ActorEt -------------------------------------------------------
ActorEt::ActorEt()
{
	iu = NULL;
	m_pMaxInterface = NULL;	
	hPanel = NULL;
	m_hOptionPanel = NULL;

	m_RegUtil.SetRegistryPath( "Software\\Eyedentitygames\\ActorEt" );

	m_nExportSelected = 0;
	m_nUseStrip = 1;
	m_nGenerateLOD = 1;
	m_nGenerateUVAni = 0;
	m_nExportTargetCamera = 0;

	m_nNoInfluenceBoneError = 0;
	m_nInfluenceBoneExceedError = 0;
	m_nMaxWeightZeroError = 0;
}

ActorEt::~ActorEt()
{

}

void ActorEt::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->m_pMaxInterface = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		ActorEtDlgProc,
		_T("ActorEt - Export"),
		0);

	m_hOptionPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_OPTION),
		ActorEtOptionDlgProc,
		_T("ActorEt - Option"),
		0);

}
	
void ActorEt::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->m_pMaxInterface = NULL;
	ip->DeleteRollupPage(hPanel);
	ip->DeleteRollupPage(m_hOptionPanel);
	hPanel = NULL;
	m_hOptionPanel = NULL;
}

void ActorEt::Init(HWND hWnd)
{
}

void ActorEt::Destroy(HWND hWnd)
{
}

void ActorEt::InitOption(HWND hWnd)
{
	m_nUseStrip = 1;
	m_nGenerateLOD = 1;
	m_nGenerateUVAni = 0;
	m_nExportTargetCamera = 0;

	m_RegUtil.GetKeyValue( "UseStrip", m_nUseStrip );
	_SetCheckBox( hWnd, IDC_USE_STRIP, m_nUseStrip ? 1 : 0 );

	m_RegUtil.GetKeyValue( "GenerateLOD", m_nGenerateLOD );
	_SetCheckBox( hWnd, IDC_GENERATE_LOD, m_nGenerateLOD ? 1 : 0 );

	m_RegUtil.GetKeyValue( "GenerateUVAni", m_nGenerateUVAni );
	_SetCheckBox( hWnd, IDC_GENERATE_UV_ANI, m_nGenerateUVAni ? 1 : 0 );

	m_RegUtil.GetKeyValue( "ExportVertexColor", m_nExportVertexColor );
	_SetCheckBox( hWnd, IDC_EXPORT_VERTEXCOLOR, m_nExportVertexColor ? 1 : 0 );

	m_RegUtil.GetKeyValue( "ExportTargetCamera", m_nExportTargetCamera );
	_SetCheckBox( hWnd, IDC_EXPORT_TARGET_CAMERA, m_nExportTargetCamera ? 1 : 0 );
}

void ActorEt::DestroyOption(HWND hWnd)
{
	m_RegUtil.SetKeyValue( "UseStrip", m_nUseStrip );
	m_RegUtil.SetKeyValue( "GenerateLOD", m_nGenerateLOD );
	m_RegUtil.SetKeyValue( "GenerateUVAni", m_nGenerateUVAni );
}

void ActorEt::SaveMesh( HWND hWnd )
{
	OPENFILENAME Ofn;
	char szFullFileName[ _MAX_PATH ], szFileName[ _MAX_PATH ], szSkinName[ _MAX_PATH ], *pFindStr;

	m_pBoundingBox = NULL;
	memset( szFullFileName, 0, _MAX_PATH );
	memset( &Ofn, 0, sizeof( OPENFILENAME ) );
	Ofn.lStructSize = sizeof( OPENFILENAME );
	Ofn.hwndOwner = hWnd;
	Ofn.lpstrFilter =  "Eternity Engine Mesh File (*.msh)\0*.msh\0\0";
	Ofn.lpstrFile = szFullFileName;
	Ofn.nMaxFile = _MAX_PATH;
	Ofn.lpstrFileTitle = szFileName;
	Ofn.nMaxFileTitle = _MAX_PATH;
	Ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	Ofn.lpstrDefExt = "msh";

	if( GetSaveFileName( &Ofn ) )
	{
		m_MaterialList.clear();
		GetNodeList();
		WriteMesh( Ofn.lpstrFile );
		strcpy( szSkinName, Ofn.lpstrFile );
		pFindStr = strrchr( szSkinName, '.' ); 
		if( pFindStr )
		{
			strcpy( pFindStr, ".skn" );
		}
		else
		{
			strcat( szSkinName, ".skn" );
		}
		WriteSkin( szSkinName, Ofn.lpstrFileTitle );
	}
}

void ActorEt::SaveAni( HWND hWnd, char *pFileName )
{
	if( pFileName )
	{
		GetNodeList();
		WriteAni( pFileName );
	}
	else
	{
		OPENFILENAME Ofn;
		char szFullFileName[ _MAX_PATH ], szFileName[ _MAX_PATH ];

		memset( szFullFileName, 0, _MAX_PATH );
		memset( &Ofn, 0, sizeof( OPENFILENAME ) );
		Ofn.lStructSize = sizeof( OPENFILENAME );
		Ofn.hwndOwner = hWnd;
		Ofn.lpstrFilter =  "Eternity Engine Animation File (*.ani)\0*.ani\0\0";
		Ofn.lpstrFile = szFullFileName;
		Ofn.nMaxFile = _MAX_PATH;
		Ofn.lpstrFileTitle = szFileName;
		Ofn.nMaxFileTitle = _MAX_PATH;
		Ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		Ofn.lpstrDefExt = "ani";
		if( GetSaveFileName( &Ofn ) )
		{
			GetNodeList();
			WriteAni( Ofn.lpstrFile );
		}
	}
}

void ActorEt::SaveCamera( HWND hWnd )
{
	OPENFILENAME Ofn;
	char szFullFileName[ _MAX_PATH ], szFileName[ _MAX_PATH ];

	m_pBoundingBox = NULL;
	memset( szFullFileName, 0, _MAX_PATH );
	memset( &Ofn, 0, sizeof( OPENFILENAME ) );
	Ofn.lStructSize = sizeof( OPENFILENAME );
	Ofn.hwndOwner = hWnd;
	Ofn.lpstrFilter =  "Eternity Engine Camera File (*.cam)\0*.cam\0\0";
	Ofn.lpstrFile = szFullFileName;
	Ofn.nMaxFile = _MAX_PATH;
	Ofn.lpstrFileTitle = szFileName;
	Ofn.nMaxFileTitle = _MAX_PATH;
	Ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	Ofn.lpstrDefExt = "msh";

	if( GetSaveFileName( &Ofn ) )
	{
		GetNodeList();
		WriteCamera( Ofn.lpstrFile );
	}
}

void ActorEt::SaveNavigation( HWND hWnd )
{
	OPENFILENAME Ofn;
	char szFullFileName[ _MAX_PATH ], szFileName[ _MAX_PATH ];

	m_pBoundingBox = NULL;
	memset( szFullFileName, 0, _MAX_PATH );
	memset( &Ofn, 0, sizeof( OPENFILENAME ) );
	Ofn.lStructSize = sizeof( OPENFILENAME );
	Ofn.hwndOwner = hWnd;
	Ofn.lpstrFilter =  "Eternity Engine Navigation Mesh File (*.nav)\0*.nav\0\0";
	Ofn.lpstrFile = szFullFileName;
	Ofn.nMaxFile = _MAX_PATH;
	Ofn.lpstrFileTitle = szFileName;
	Ofn.nMaxFileTitle = _MAX_PATH;
	Ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	Ofn.lpstrDefExt = "nav";

	if( GetSaveFileName( &Ofn ) )
	{
		GetNodeList();
		WriteNavigation( Ofn.lpstrFile );
	}
}

void ActorEt::GetNodeList()
{
	int i;

	m_vecNodeList.clear();
	for( i = 0; i < m_pMaxInterface->GetRootNode()->NumberOfChildren(); i ++ )
	{
		AddNodeList( m_pMaxInterface->GetRootNode()->GetChildNode( i ) );
	}
}

void ActorEt::AddNodeList( INode *pNode )
{
	int i;
	SNodeInfo NodeInfo;

	if( pNode == NULL )
	{
		return;
	}

	TCHAR *pNodeName;
	pNodeName = pNode->GetName();

	NodeInfo.pNode = pNode;
	NodeInfo.nNodeFlag = 0;
	if( IsMeshObject( pNode ) )
	{
		NodeInfo.nNodeFlag |= NODE_FLAG_MESH;
	}
	if( IsBoneObject( pNode ) )
	{
		NodeInfo.nNodeFlag |= NODE_FLAG_BONE;
	}
	if( IsCameraObject( pNode ) )
	{
		NodeInfo.nNodeFlag |= NODE_FLAG_CAMERA;
	}
	if( IsDummyObject( pNode ) ) 
	{
		NodeInfo.nNodeFlag |= NODE_FLAG_DUMMY;
	}
	if( IsCollisionMeshObject( pNode ) )
	{
		NodeInfo.nNodeFlag = 0;		// Collision Mesh 이면서 다른 종류일수 없다.
		NodeInfo.nNodeFlag |= NODE_FLAG_COLLISION;
	}
	m_vecNodeList.push_back( NodeInfo );

	for( i = 0; i < pNode->NumberOfChildren(); i++ )
	{
		AddNodeList( pNode->GetChildNode( i ) );
	}
}

bool ActorEt::IsMeshObject( INode *pNode )
{
	Object *pObject;
	TCHAR *pNodeName;

	pObject = pNode->EvalWorldState( 0 ).obj;	// 시간 적당히 넣어줘야함..
	pNodeName = pNode->GetName();

	if( stricmp( pNodeName, "BoundingBox" ) == 0 )
	{
		m_pBoundingBox = pNode;
		return false;
	}
	if( ( m_nExportSelected ) && ( !pNode->Selected() ) )
	{
		return false;
	}
	if( pNode->IsFrozen() )
	{
		return false;
	}
	if( pObject->SuperClassID() != GEOMOBJECT_CLASS_ID )
	{
		return false;
	}

	// Check Collision Mesh
	if( pObject->ClassID() == Class_ID( BOXOBJ_CLASS_ID, 0 ) )
	{
		return false;
	}
	if( pObject->ClassID() == Class_ID( SPHERE_CLASS_ID, 0 ) )
	{
		return false;
	}
	if( pObject->ClassID() == Class_ID( CYLINDER_CLASS_ID, 0 ) )
	{
		return false;
	}
	if( pObject->ClassID() == CAPS_CLASS_ID )
	{
		return false;
	}

	// Check Bone
	if( pObject->ClassID() == Class_ID( TARGET_CLASS_ID, 0 ) )
	{
		return false;
	}
	if( pObject->ClassID() == BONE_OBJ_CLASSID )
	{
		return false;
	}
	if( pObject->ClassID() == BIPSLAVE_CONTROL_CLASS_ID )
	{
		return false;
	}
	if( pObject->ClassID() == BIPBODY_CONTROL_CLASS_ID )
	{
		return false;
	}
	if( pObject->ClassID() == FOOTPRINT_CLASS_ID )
	{
		return false;
	}
	if( pObject->ClassID() == Class_ID( 0x9125, 0 ) )
	{
		return false;
	}

	// BoxBone Check
	if( pNode->GetName()[ 0 ] == '~' )
	{
		return false;
	}
	if( pNode->GetName()[ 0 ] == '#' )
	{
		return false;
	}
	if( pNode->GetName()[ 0 ] == '@' )
	{
		return false;
	}
	return true;
}

bool ActorEt::IsBoneObject( INode *pNode )
{
	if( pNode->IsRootNode() ) // 최상위 본 제외 Scene Root
	{
		return false;
	}
	if( pNode->IsFrozen() )
	{
		return false;
	}

	TCHAR *pNodeName;
	pNodeName = pNode->GetName();
	if( pNode->GetName()[ 0 ] == '~' )
	{
		return true;
	}
	if( pNode->GetName()[ 0 ] == '#' )
	{
		return false;
	}
	if( stricmp( pNodeName, "BoundingBox" ) == 0 )
	{
		return false;
	}
	if( pNode->GetName()[ 0 ] == '@' )
	{
		return false;
	}

	Object *pObject;

	pObject = pNode->EvalWorldState( 0 ).obj;
	if( pObject->ClassID() == Class_ID( BONE_CLASS_ID, 0 ) )
	{
		return true;
	}
	if( pObject->ClassID() == BONE_OBJ_CLASSID )
	{
		return true;
	}
	if( pObject->ClassID() == Class_ID( DUMMY_CLASS_ID, 0 ) )
	{
		return false;
	}

	Control *pCont = pNode->GetTMController();
	if( ( pCont->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ) || ( pCont->ClassID() == BIPBODY_CONTROL_CLASS_ID ) ) 
	{
		return true;
	}

	// Has Animation??
	CSaveAni SaveAni;
	if( SaveAni.IsExistAniKey( pNode, m_pMaxInterface ) )
	{
		return true;
	}

	return false;
}

bool ActorEt::IsCollisionMeshObject( INode *pNode )
{
	Object *pObject;

	pObject = pNode->EvalWorldState( 0 ).obj;

	if( pNode->IsFrozen() )
	{
		return false;
	}
	if( ( m_nExportSelected ) && ( !pNode->Selected() ) )
	{
		return false;
	}

	TCHAR *pNodeName;
	pNodeName = pNode->GetName();
	if( pNode->GetName()[ 0 ] == '~' )
	{
		return false;
	}
	if( pNode->GetName()[ 0 ] == '#' )
	{
		return false;
	}
	if( stricmp( pNodeName, "BoundingBox" ) == 0 )
	{
		return false;
	}

	if( pObject->ClassID() == Class_ID( BOXOBJ_CLASS_ID, 0 ) )
	{
		return true;
	}
	if( pObject->ClassID() == Class_ID( SPHERE_CLASS_ID, 0 ) )
	{
		return true;
	}
	if( pObject->ClassID() == Class_ID( CYLINDER_CLASS_ID, 0 ) )
	{
		return true;
	}
	if( pObject->ClassID() == CAPS_CLASS_ID )
	{
		return true;
	}
	if( pNode->GetName()[ 0 ] == '@' )
	{
		return true;
	}

	return false;
}

bool ActorEt::IsCameraObject( INode *pNode )
{
	Object *pObject;

	pObject = pNode->EvalWorldState( 0 ).obj;	// 시간 적당히 넣어줘야함..
	if( ( m_nExportSelected ) && ( !pNode->Selected() ) )
	{
		return false;
	}
	if( pNode->IsFrozen() )
	{
		return false;
	}
	if( pObject->SuperClassID() == CAMERA_CLASS_ID )
	{
		return true;
	}

	return false;
}

bool ActorEt::IsDummyObject( INode *pNode )
{
	if( ( m_nExportSelected ) && ( !pNode->Selected() ) )
	{
		return false;
	}
	if( pNode->IsFrozen() )
	{
		return false;
	}
	if( pNode->GetName()[0] == '#' ) {
		return true;
	}
	return false;
}

void ActorEt::GetSelectedBone( std::vector< std::string > &vecSelectedBone )
{
	int i;

	GetNodeList();
	for( i = 0; i < ( int )m_vecNodeList.size(); i++ )
	{
		if( m_vecNodeList[ i ].pNode->Selected() )
		{
			if( m_vecNodeList[ i ].nNodeFlag & NODE_FLAG_BONE )
			{
				vecSelectedBone.push_back( m_vecNodeList[ i ].pNode->GetName() );
			}
		}
	}
}

int ActorEt::GetNodeCount( int nNodeFlag )
{
	int i, nNodeCount;

	nNodeCount = 0;
	for( i = 0; i < ( int )m_vecNodeList.size(); i++ )
	{
		if( m_vecNodeList[ i ].nNodeFlag & nNodeFlag )
		{
			nNodeCount++;
		}
	}

	return nNodeCount;
}

INode *ActorEt::FindNode( int nIndex, int nNodeFlag )
{
	int i, nNodeCount;

	nNodeCount = 0;
	for( i = 0; i < ( int )m_vecNodeList.size(); i++ )
	{
		if( m_vecNodeList[ i ].nNodeFlag & nNodeFlag )
		{
			if( nNodeCount == nIndex )
			{
				return m_vecNodeList[ i ].pNode;
			}
			nNodeCount++;
		}
	}

	return NULL;
}

SNodeInfo *ActorEt::FindNodeInfo( int nIndex, int nNodeFlag )
{
	int i, nNodeCount;

	nNodeCount = 0;
	for( i = 0; i < ( int )m_vecNodeList.size(); i++ )
	{
		if( m_vecNodeList[ i ].nNodeFlag & nNodeFlag )
		{
			if( nNodeCount == nIndex )
			{
				return &m_vecNodeList[ i ];
			}
			nNodeCount++;
		}
	}

	return NULL;
}

DWORD WINAPI fn( LPVOID arg )
{
	return( 0 ); // Dummy function for progress bar
}

void ActorEt::WriteMesh( const char *pFileName )
{
	int i, nWriteCount;
	FILE *fp;

	fp = fopen( pFileName, "wb" );
	if( fp == NULL )
	{
		char str[255];
		sprintf(str, "[%s] 파일이 읽기 전용이라\n 저장되지 않았습니다.", pFileName);
		MessageBox(0, str, "Error Message", MB_OK );
		return;
	}

	SMeshFileHeader MeshHeader;

	memset( &MeshHeader, 0, sizeof( SMeshFileHeader ) );
	strcpy( MeshHeader.szHeaderString, MESH_FILE_STRING );
	MeshHeader.nVersion = MESH_FILE_VERSION;
	MeshHeader.nSubMeshCount = GetNodeCount( NODE_FLAG_MESH );	
	MeshHeader.nLODCount = 1;	// 우선은 1로 셋팅한다. LOD만들면서 수정하자..
	MeshHeader.bUVAni = false;		// 우선 UV ani 사용안한다. 
	fwrite( &MeshHeader, sizeof( SMeshFileHeader ), 1, fp );	

	MeshHeader.nBoneCount = GetNodeCount( NODE_FLAG_BONE );
	for( i = 0; i < MeshHeader.nBoneCount; i++ )
	{
		INode *pFindNode;
		Matrix3 WorldMat;
		char szBoneName[ 256 ];

		pFindNode = FindNode( i, NODE_FLAG_BONE );
		memset( szBoneName, 0, 256 );
		strcpy( szBoneName, pFindNode->GetName() );
		fwrite( szBoneName, 256, 1, fp );

		WorldMat = pFindNode->GetNodeTM( 0 );
		WorldMat = Inverse( WorldMat );
		DumpMatrix( fp, WorldMat );
	}

	m_pMaxInterface->ProgressStart( "Save Mesh...", true, fn, NULL );
	nWriteCount = 0;
	MeshHeader.MaxVec = Point3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	MeshHeader.MinVec = Point3( FLT_MAX, FLT_MAX, FLT_MAX );
	for( i = 0; i < MeshHeader.nSubMeshCount; i++ )
	{
		SNodeInfo *pNodeInfo;
		bool bUseBone;

		pNodeInfo = FindNodeInfo( i, NODE_FLAG_MESH );
		bUseBone = false;
		if( pNodeInfo->nNodeFlag & NODE_FLAG_BONE )
		{
			bUseBone = true;
		}
		nWriteCount += ProcessSubMesh( fp, pNodeInfo->pNode, MeshHeader.MaxVec, MeshHeader.MinVec, bUseBone );
		m_pMaxInterface->ProgressUpdate( ( int )( i * 100.0f / MeshHeader.nSubMeshCount ) ); 
		if( m_pMaxInterface->GetCancel() )
		{
			break;
		}
	}

	int nCollisionNodeCount, nCollisionPrimitiveCount = 0;
	nCollisionNodeCount = GetNodeCount( NODE_FLAG_COLLISION );
	if( nCollisionNodeCount )
	{
		for( i = 0; i < nCollisionNodeCount; i++ )
		{
			nCollisionPrimitiveCount += ProcessCollisionPrimitive( fp, FindNode( i, NODE_FLAG_COLLISION ), MeshHeader.MaxVec, MeshHeader.MinVec );
		}
	}
	MeshHeader.nDummyCount = GetNodeCount( NODE_FLAG_DUMMY );
	for( i = 0; i < MeshHeader.nDummyCount; i++) 
	{
		INode *pFindNode = FindNode( i, NODE_FLAG_DUMMY );
		TimeValue StartTime;
		Matrix3 LocalMat;
		AffineParts Affine;
		char szDummyName[ 256 ];

		StartTime = m_pMaxInterface->GetAnimRange().Start();
		LocalMat = pFindNode->GetNodeTM( StartTime );
		if( pFindNode->GetParentNode() )
		{
			LocalMat = LocalMat * Inverse( pFindNode->GetParentNode()->GetNodeTM( StartTime ) );
		}
		decomp_affine( LocalMat, &Affine );

		memset( szDummyName, 0, 256 );
		strcpy( szDummyName, pFindNode->GetName() );
		fwrite( szDummyName, 256, 1, fp );

		memset( szDummyName, 0, 256 );
		if( pFindNode->GetParentNode() )
		{
			strcpy( szDummyName, pFindNode->GetParentNode()->GetName() );
		}
		fwrite( szDummyName, 256, 1, fp );

		DumpMatrix( fp, LocalMat );
	}
	if( m_pBoundingBox )
	{
		MeshHeader.MaxVec = Point3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		MeshHeader.MinVec = Point3( FLT_MAX, FLT_MAX, FLT_MAX );
		CalcBoundingBox( m_pBoundingBox, MeshHeader.MaxVec, MeshHeader.MinVec );
	}

	MeshHeader.nSubMeshCount = nWriteCount;
	MeshHeader.nCollisionPrimitiveCount = nCollisionPrimitiveCount;
	fseek( fp, 0, SEEK_SET );
	fwrite( &MeshHeader, sizeof( SMeshFileHeader ), 1, fp );

	fclose( fp );
	m_pMaxInterface->ProgressEnd();
}

int ActorEt::ProcessSubMesh( FILE *fp, INode *pNode, Point3 &MaxVec, Point3 &MinVec, bool bUseBone )
{
	int i, nNeedDel;
	int nVertexCount, nFaceCount;
	bool bPhysique;

	TriObject *pTri = GetTriObjectFromNode( pNode, 0, nNeedDel );
	if( pTri == NULL )
	{
		return 0;
	}

	Mesh *pMesh = &pTri->GetMesh();
	pMesh->buildNormals();
	nVertexCount = pMesh->getNumVerts();
	nFaceCount = pMesh->getNumFaces();
	if( ( nVertexCount == 0 ) || ( nFaceCount == 0 ) )
	{
		if( nNeedDel )
		{
			delete pTri;
		}
		return 0;
	}

	std::vector< Mtl * > MaterialList;
	GetMaterialList( pNode, MaterialList );

	std::vector< SMAXVertex > VertexList;
	std::vector< std::string > BoneList;
	bPhysique = GetVertexList( pNode, pMesh, VertexList, BoneList );

	int nSaveCount = 0;
	CSaveMesh *pSaveMesh;
	char szNodeName[ 256 ], szParentName[ 256 ], szOutputName[ 256 ];

	strcpy( szNodeName, pNode->GetName() );
	if( bUseBone )
	{
		strcpy( szParentName, pNode->GetName() );
	}
	else
	{
		strcpy( szParentName, pNode->GetParentNode()->GetName() );
	}
	pSaveMesh = new CSaveMesh[ MaterialList.size() ];
	for( i = 0; i < MaterialList.size(); i++ )
	{
		if( MaterialList.size() > 1 )
		{
			sprintf( szOutputName, "%s_%d", szNodeName, nSaveCount );
		}
		else
		{
			strcpy( szOutputName, szNodeName );
		}
		pSaveMesh[ i ].Initialize( pNode, pMesh, MaterialList[ i ], &BoneList, MaterialList.size(), i, VertexList.size() );
		pSaveMesh[ i ].SetMeshName( szOutputName, szParentName );
		if( pSaveMesh[ i ].BuildSaveMesh( m_nUseStrip > 0 ) )
		{
			pSaveMesh[ i ].WriteMesh( fp, VertexList, BoneList, m_nUseStrip > 0, bPhysique, m_nExportVertexColor > 0 );
			m_MaterialList.push_back( MaterialList[ i ] );
			nSaveCount++;
		}
	}

	GetBoundingBox( VertexList, MaxVec, MinVec );

	if( pSaveMesh )
	{
		delete [] pSaveMesh;
	}

	if( nNeedDel )
	{
		delete pTri;
	}

	return nSaveCount;
}

TriObject *ActorEt::GetTriObjectFromNode( INode *pNode, TimeValue Time, int &deleteIt )
{
	deleteIt = FALSE;
	Object *obj = pNode->EvalWorldState( Time ).obj;

	if( obj == NULL )
	{
		return NULL;
	}
	if( obj->CanConvertToType( Class_ID( TRIOBJ_CLASS_ID, 0 ) ) )
	{ 
		TriObject *tri = ( TriObject * )obj->ConvertToType( Time, Class_ID( TRIOBJ_CLASS_ID, 0 ) );
		if( obj != tri )
		{
			deleteIt = TRUE;
		}
		return tri;
	}
	else
	{
		return NULL;
	}
}

void ActorEt::GetMaterialList( INode *pNode, std::vector< Mtl * > &MaterialList )
{
	Mtl *pMtl;

	pMtl = pNode->GetMtl();
	if( pMtl == NULL )
	{
		return;
	}

	if( pMtl->NumSubMtls() > 1 )
	{
		int i;
		Mtl *pSubMtl;
		for( i = 0; i < pMtl->NumSubMtls(); i++ )
		{
			if( pMtl->ClassID() == Class_ID( DMTL_CLASS_ID, 0 ) )
			{
				if( !( ( StdMat * )pMtl )->MapEnabled( i ) )
				{					
					continue;
				}
			}
			pSubMtl = pMtl->GetSubMtl( i );
			if( pSubMtl )
			{
				MaterialList.push_back( pSubMtl );
			}
		}
	}
	else
	{
		MaterialList.push_back( pMtl );
	}
}

bool ActorEt::GetVertexList( INode *pNode, std::vector< SMAXVertex > &VertexList, std::vector< std::string > &BoneList )
{
	int nNeedDel;
	TriObject *pTri = GetTriObjectFromNode( pNode, 0, nNeedDel );
	if( pTri )
	{
		Mesh *pMesh = &pTri->GetMesh();
		bool bRet = GetVertexList( pNode, pMesh, VertexList, BoneList );
		if( nNeedDel )
		{
			delete pTri;
		}
		return bRet;
	}

	return false;
}

bool ActorEt::GetVertexList( INode *pNode, Mesh *pMesh, std::vector< SMAXVertex > &VertexList, std::vector< std::string > &BoneList )
{
	int i, nVertexCount;
	Matrix3 TM, NormalTM;

	TM = pNode->GetObjTMAfterWSM( 0 );
//	TM.RotateZ( 3.141592654f );
	NormalTM = TM;
	NormalTM.SetRow( 3, Point3( 0, 0, 0 ) );
	nVertexCount = pMesh->getNumVerts();
	for( i = 0; i < nVertexCount; i++ )
	{
		SMAXVertex Vertex;
		Point3 Point;

		Point = TM * pMesh->verts[i];
		Vertex.Vertex.x = Point.x;
		Vertex.Vertex.y = Point.z;
		Vertex.Vertex.z = Point.y;

		if( pMesh->numCVerts )
		{
			Point = pMesh->vertCol[ i ];
			if( Point.x > 1.0f )
			{
				Point.x = 1.0f;
			}
			if( Point.y > 1.0f )
			{
				Point.y = 1.0f;
			}
			if( Point.z > 1.0f )
			{
				Point.z = 1.0f;
			}
			Vertex.dwVertexColor = 0xff000000 + ( ( DWORD )( Point.x * 255 ) << 16 ) + ( ( DWORD )( Point.y * 255 ) << 8 ) + ( DWORD )( Point.z * 255 );
		}
		else
		{
			Vertex.dwVertexColor = 0xffffffff;
		}

		memset( Vertex.fWeight, 0, sizeof( float ) * 4 );
		Vertex.nBone[ 0 ] = -1;
		Vertex.nBone[ 1 ] = -1;
		Vertex.nBone[ 2 ] = -1;
		Vertex.nBone[ 3 ] = -1;

		VertexList.push_back( Vertex );
	}

	m_nNoInfluenceBoneError = 0;
	m_nInfluenceBoneExceedError = 0;
	m_nMaxWeightZeroError = 0;
	if( GetPhysiqueData( pNode, VertexList, BoneList ) == 0 )
	{
		if( GetSkinData( pNode, VertexList, BoneList ) == 0 )
		{
			return false;
		}
	}

	char szPhysiqueErrorMsg[255]={0,};
	if( m_nNoInfluenceBoneError != 0 ) {
		sprintf(szPhysiqueErrorMsg, "아무본에도 영향 받지 않는\n버텍스가 %d개 있습니다.", m_nNoInfluenceBoneError);
		MessageBox(0, szPhysiqueErrorMsg, "Error", MB_OK );
	}
	if( m_nInfluenceBoneExceedError != 0 ) {
		sprintf(szPhysiqueErrorMsg, "영향받는 본갯수가 4개를 초과한\n버텍스가 %d개 있습니다.", m_nInfluenceBoneExceedError);
		MessageBox(0, szPhysiqueErrorMsg, "Error", MB_OK );
	}
	if( m_nMaxWeightZeroError != 0 ) {
		sprintf(szPhysiqueErrorMsg, "가중치 값들의 총합이 0인\n버텍스가 %d개 있습니다.", m_nMaxWeightZeroError);
		MessageBox(0, szPhysiqueErrorMsg, "Error", MB_OK );
	}
	return true;
}

Point3 ActorEt::GetNodeCenterPos( INode *pNode )
{
	Point3 CenterPos(0,0,0);
	
	std::vector< SMAXVertex > VertexList;
	std::vector< std::string > BoneList;
	GetVertexList( pNode, VertexList, BoneList );
	if( VertexList.empty() ) {
		Matrix3 Mat = pNode->GetNodeTM(0);
		CenterPos = Mat.GetRow(3);
		std::swap(CenterPos.y, CenterPos.z);
	}
	else {
		for( int j = 0; j < (int)VertexList.size(); j++) {
			CenterPos += VertexList[j].Vertex;
		}
		CenterPos /= VertexList.size();
	}
	return CenterPos;
}

struct SortPhysiqueByWeight
{
	const bool operator() (const SPhysiqueInfo &a, const SPhysiqueInfo b)
	{
		return ( a.fWeight > b.fWeight );
	}
};

Modifier *FindPhysiqueModifier(INode* pNode)
{
	// Get object from node. Abort if no object.
	Object* ObjectPtr = pNode->GetObjectRef();
			

	if (!ObjectPtr) return NULL;

	// Is derived object ?
	while (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID && ObjectPtr)
	{
		// Yes -> Cast.
		IDerivedObject *DerivedObjectPtr = (IDerivedObject *)(ObjectPtr);
						
		// Iterate over all entries of the modifier stack.
		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			// Get current modifier.
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			// Is this Physique ?
			if (ModifierPtr->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
			{
				// Yes -> Exit.
				return ModifierPtr;
			}

			// Next modifier stack entry.
			ModStackIndex++;
		}
		ObjectPtr = DerivedObjectPtr->GetObjRef();
	}

	// Not found.
	return NULL;
}

//------------------------------------------------------------------------------------------------

Modifier *FindSkinModifier( INode *pNode )
{
	// Get object from node. Abort if no object.
	Object* ObjectPtr =((INode*)pNode)->GetObjectRef();
	
	if (!ObjectPtr) return NULL;

	// Is derived object ?
	if (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID) 
	{
		// Yes -> Cast.
		IDerivedObject* DerivedObjectPtr = static_cast<IDerivedObject*>(ObjectPtr);

		// Iterate over all entries of the modifier stack.
		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			// Get current modifier.
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			//DebugBox("Modifier Testing: %i \n",ModStackIndex);

			// Is this Physique ?
			if( ModifierPtr->ClassID() == Class_ID(SKIN_CLASSID) ) //PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
			{
				return ModifierPtr;
			}
			// Next modifier stack entry.
			ModStackIndex++;
		}
	}
	// Not found.
	return NULL;
}

int ActorEt::GetPhysiqueData( INode *pNode, std::vector< SMAXVertex > &VertexList, std::vector< std::string > &BoneList )
{
	int i, j;
	int nPhysiqueCount, nType;
	int numPhys = 0;
	float fSumWeight = 0.f;
	IPhyBlendedRigidVertex *pBlendVertex;
	IPhyRigidVertex *pRigidVertex;
	std::vector< SPhysiqueInfo > PhysiqueInfo;

	Modifier *pMod = FindPhysiqueModifier( pNode );
	if( pMod == NULL )
	{
		return 0;
	}
	if( pMod->ClassID() != Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) )
	{
		return 0;
	}
	IPhysiqueExport *pPhysiqueExport = ( IPhysiqueExport * )pMod->GetInterface( I_PHYEXPORT );
	IPhyContextExport *pContextExport = ( IPhyContextExport * )pPhysiqueExport->GetContextInterface( pNode );
	pContextExport->ConvertToRigid( true );
	pContextExport->AllowBlending( true );

	nPhysiqueCount = pContextExport->GetNumberVertices();
	for( i = 0; i < nPhysiqueCount; i++ )
	{
		SPhysiqueInfo Info;
		IPhyVertexExport *pVertexExport = pContextExport->GetVertexInterface( i );
		if( pVertexExport )
		{
			nType = pVertexExport->GetVertexType();
			switch( nType )
			{
				case RIGID_BLENDED_TYPE:
					pBlendVertex = ( IPhyBlendedRigidVertex * )pVertexExport;
					PhysiqueInfo.clear();
					for( j = 0; j < pBlendVertex->GetNumberNodes(); j++ )
					{
						 INode *pNode = pBlendVertex->GetNode( j );
						 if( pNode ) {
							Info.nBoneIndex = AddBoneNameList( BoneList, pNode->GetName() );
						}
						 else {
							 m_nNoInfluenceBoneError++;
						 }
						Info.fWeight = pBlendVertex->GetWeight( j );
						if( Info.fWeight <= 0.0f )
						{
							continue;
						}
						PhysiqueInfo.push_back( Info );
					}
					std::sort( PhysiqueInfo.begin(), PhysiqueInfo.end(), SortPhysiqueByWeight() );

					if( PhysiqueInfo.size() == 0 )
					{
						VertexList[ i ].nBone[ 0 ] = 0;
						VertexList[ i ].fWeight[ 0 ] = 1.0f;
						m_nNoInfluenceBoneError++;
					}
					if( PhysiqueInfo.size() > 4 )
					{
						m_nInfluenceBoneExceedError++;
					}

					numPhys = min( 4, PhysiqueInfo.size() );
					fSumWeight = 0.f;
					for( j = 0; j < numPhys; j++ )	// 4Weight 까지만 지원한다.
					{
						VertexList[ i ].nBone[ j ] = PhysiqueInfo[ j ].nBoneIndex;
						VertexList[ i ].fWeight[ j ] = PhysiqueInfo[ j ].fWeight;
						fSumWeight += PhysiqueInfo[ j ].fWeight;
					}
					if( numPhys != 0 && fSumWeight <= 0.f )	{	// 총합이 0 인 경우엔, 동등하게 나눠준다.
						m_nMaxWeightZeroError++;
						for( j = 0; j < numPhys; j++ ) {
							VertexList[ i ].fWeight[ j ] = 1.0f / numPhys;
						}
					}
					else {			// 영향받는 본이 4개 이상인 경우 총합이 1이 아닐 수 있므로 총합을 1로 만들어준다.
						for( j = 0; j < numPhys; j++ ) {
							VertexList[ i ].fWeight[ j ] /= fSumWeight;
						}
					}
					break;

				case RIGID_TYPE:
					pRigidVertex = ( IPhyRigidVertex * )pVertexExport;
					if( pRigidVertex->GetNode() ) {
						VertexList[ i ].nBone[ 0 ] = AddBoneNameList( BoneList, pRigidVertex->GetNode()->GetName() );
					}
					else {
						m_nNoInfluenceBoneError++;
					}
					VertexList[ i ].fWeight[ 0 ] = 1.0f;
					break;

				default:
					break;
			}
		}
	}

	pPhysiqueExport->ReleaseContextInterface( pContextExport );
	pMod->ReleaseInterface( I_PHYINTERFACE, pPhysiqueExport );

	return 1;
}

int ActorEt::GetSkinData( INode *pNode, std::vector< SMAXVertex > &VertexList, std::vector< std::string > &BoneList )
{
	int i, j;
	int nVertexCount, nAssignedBoneCount, nAssignedBone;
	Modifier *pMod = FindSkinModifier( pNode );
	INode *pBone;
	std::vector< SPhysiqueInfo > PhysiqueInfo;

	if( pMod == NULL )
	{
		return 0;
	}
	ISkin *pSkin = ( ISkin * )pMod->GetInterface( I_SKIN );
	if( pSkin == NULL )
	{
		return 0;
	}
	ISkinContextData *pSkinContext = pSkin->GetContextInterface( pNode );
	if( pSkinContext == NULL )
	{
		return 0;
	}

	nVertexCount = pSkinContext->GetNumPoints();
	for( i = 0; i < nVertexCount; i++ )
	{
		SPhysiqueInfo Info;
		nAssignedBoneCount = pSkinContext->GetNumAssignedBones( i );
		PhysiqueInfo.clear();
		for( j = 0; j < nAssignedBoneCount; j++ )
		{
			nAssignedBone = pSkinContext->GetAssignedBone( i, j );
			pBone = pSkin->GetBone( nAssignedBone );

			Info.nBoneIndex = AddBoneNameList( BoneList, pBone->GetName() );
			Info.fWeight = pSkinContext->GetBoneWeight( i, j );
			if( Info.fWeight <= 0.0f )
			{
				continue;
			}
			PhysiqueInfo.push_back( Info ); 
		}
		std::sort( PhysiqueInfo.begin(), PhysiqueInfo.end(), SortPhysiqueByWeight() );

		if( PhysiqueInfo.size() == 0 )
		{
			VertexList[ i ].nBone[ 0 ] = 0;
			VertexList[ i ].fWeight[ 0 ] = 1.0f;
			m_nNoInfluenceBoneError++;
		}
		if( PhysiqueInfo.size() > 4 )
		{
			m_nInfluenceBoneExceedError++;
		}

		int numPhys = min( 4, PhysiqueInfo.size() );
		
		float fSumWeight = 0.f;
		for( j = 0; j < numPhys; j++ )	// 4Weight 까지만 지원한다.
		{
			VertexList[ i ].nBone[ j ] = PhysiqueInfo[ j ].nBoneIndex;
			VertexList[ i ].fWeight[ j ] = PhysiqueInfo[ j ].fWeight;
			fSumWeight += PhysiqueInfo[ j ].fWeight;
		}
		if( numPhys != 0 && fSumWeight <= 0.f )	{	// 총합이 0 인 경우엔, 동등하게 나눠준다.
			m_nMaxWeightZeroError++;
			for( j = 0; j < numPhys; j++ ) {
				VertexList[ i ].fWeight[ j ] = 1.0f / numPhys;
			}
		}
		else {		// 영향받는 본이 4개 이상인 경우 총합이 1이 아닐 수 있므로 총합을 1로 만들어준다.	
			for( j = 0; j < numPhys; j++ ) {	
				VertexList[ i ].fWeight[ j ] /= fSumWeight;
			}
		}
	}
	pMod->ReleaseInterface(I_SKIN, pSkin);

	return 1;
}

int ActorEt::AddBoneNameList( std::vector< std::string > &BoneList, const char *pBoneName )
{
	int i;

	for( i = 0; i < BoneList.size(); i++ )
	{
		if( stricmp( pBoneName, BoneList[ i ].c_str() ) == 0 )
		{
			return i;
		}
	}

	BoneList.push_back( pBoneName );

	return BoneList.size() - 1;
}


void ActorEt::WriteAni( const char *pFileName )
{
	int i, nAniLength;
	FILE *fp;
	CSaveAni SaveAni;
	char szAniName[ 256 ], *pFindPtr;

	fp = fopen( pFileName, "wb" );
	if( fp == NULL )
	{
		char str[255];
		sprintf(str, "[%s] 파일이 읽기 전용이라\n 저장되지 않았습니다.", pFileName);
		MessageBox(0, str, "Error Message", MB_OK );
		return;
	}

	SAniFileHeader FileHeader;

	memset( &FileHeader, 0, sizeof( SAniFileHeader ) );
	strcpy( FileHeader.szHeaderString, ANI_FILE_STRING );
	FileHeader.nVersion = ANI_FILE_VERSION;
	FileHeader.nBoneCount = GetNodeCount( NODE_FLAG_BONE );
	FileHeader.nAniCount = 1;
	fwrite( &FileHeader, 1, sizeof( SAniFileHeader ), fp );

	GetDlgItemText( hPanel, IDC_EDIT_ANI_NAME, szAniName, 256 );
	if( strlen( szAniName ) == 0 )
	{
		pFindPtr = (char*)strrchr( pFileName, '\\' );
		memset( szAniName, 0, 256 );
		if( pFindPtr )
		{
			strcpy( szAniName, pFindPtr + 1 );
		}
		else
		{
			strcpy( szAniName, pFileName );
		}
		pFindPtr = ( char * )strrchr( szAniName, '.' );
		if( pFindPtr )
		{
			*pFindPtr = 0;
		}
	}
	fwrite( szAniName, 256, 1, fp );

	nAniLength = ( m_pMaxInterface->GetAnimRange().End() - m_pMaxInterface->GetAnimRange().Start() ) / GetTicksPerFrame();
	nAniLength++;
	fwrite( &nAniLength, sizeof( int ), 1, fp );

	m_pMaxInterface->ProgressStart( "Save Animation...", true, fn, NULL );
	for( i = 0; i < FileHeader.nBoneCount; i++ )
	{
		INode *pFindNode;

		pFindNode = FindNode( i, NODE_FLAG_BONE );
		SaveAni.ProcessBone( fp, pFindNode, m_pMaxInterface );
		m_pMaxInterface->ProgressUpdate( ( int )( i * 100.0f / FileHeader.nBoneCount ) ); 
		if( m_pMaxInterface->GetCancel() )
		{
			break;
		}
	}

	fclose( fp );
	m_pMaxInterface->ProgressEnd();
}

void ActorEt::WriteSkin( const char *pFileName, const char *pMeshName )
{
	int i;
	FILE *fp;
	CSaveSkin SaveSkin;

	fp = fopen( pFileName, "wb" );
	if( fp == NULL )
	{
		char str[255];
		sprintf(str, "[%s] 파일이 읽기 전용이라\n 저장되지 않았습니다.", pFileName);
		MessageBox(0, str, "Error Message", MB_OK );
		return;
	}

	SSkinFileHeader SkinHeader;

	memset( &SkinHeader, 0, sizeof( SSkinFileHeader ) );
	strcpy( SkinHeader.szHeaderString, SKIN_FILE_STRING );
	strcpy( SkinHeader.szMeshName, pMeshName );
	SkinHeader.nVersion = SKIN_FILE_VERSION;
	SkinHeader.nSubMeshCount = m_MaterialList.size();
	fwrite( &SkinHeader, sizeof( SSkinFileHeader ), 1, fp );
	for( i = 0; i < m_MaterialList.size(); i++ )
	{
		SaveSkin.SaveSkin( fp, m_MaterialList[ i ] );
	}
	fclose( fp );
}

void ActorEt::CalcBoundingBox( INode *pNode, Point3 &MaxVec, Point3 &MinVec )
{
	std::vector< SMAXVertex > VertexList;
	std::vector< std::string > BoneList;
	GetVertexList( pNode, VertexList, BoneList );
	GetBoundingBox( VertexList, MaxVec, MinVec );
}

void ActorEt::GetBoundingBox( std::vector< SMAXVertex > &SystemVertex, Point3 &MaxVec, Point3 &MinVec )
{
	int i;
	Point3 Position;

	for( i = 0; i < SystemVertex.size(); i++ )
	{
		Position = SystemVertex[ i ].Vertex;
		if( MaxVec.x < Position.x )
		{
			MaxVec.x = Position.x;
		}
		if( MaxVec.y < Position.y )
		{
			MaxVec.y = Position.y;
		}
		if( MaxVec.z < Position.z )
		{
			MaxVec.z = Position.z;
		}
		if( MinVec.x > Position.x )
		{
			MinVec.x = Position.x;
		}
		if( MinVec.y > Position.y )
		{
			MinVec.y = Position.y;
		}
		if( MinVec.z > Position.z )
		{
			MinVec.z = Position.z;
		}
	}
}

void ActorEt::WriteNavigation( const char *pFileName )
{
	int i;
	FILE *fp;

	fp = fopen( pFileName, "wb" );
	if( fp == NULL )
	{
		char str[255];
		sprintf(str, "[%s] 파일이 읽기 전용이라\n 저장되지 않았습니다.", pFileName);
		MessageBox(0, str, "Error Message", MB_OK );
		return;
	}

	struct SNavFileHeader
	{
		int nSignature;
		int nVersion;
		int nTriangleCount;
	};
	enum NAV_MESH_TYPE
	{
		NMT_TERRAIN = 0,
		NMT_PROP = 1,
	};

	SNavFileHeader Header;
	std::vector< int > vecEntrance;

	Header.nSignature = 0xefefefef;
	Header.nVersion = 11;
	Header.nTriangleCount = 0;
	fwrite( &Header, sizeof( SNavFileHeader ), 1, fp );
	m_pMaxInterface->ProgressStart( "Save Mesh...", true, fn, NULL );
	for( i = 0; i < ( int )m_vecNodeList.size(); i++ )
	{
		SNodeInfo *pNodeInfo;

		pNodeInfo = &m_vecNodeList[ i ];
		m_pMaxInterface->ProgressUpdate( ( int )( i * 100.0f / m_vecNodeList.size() ) ); 
		if( m_pMaxInterface->GetCancel() )
		{
			break;
		}
		if( ( pNodeInfo->nNodeFlag & NODE_FLAG_MESH ) == 0 )
		{
			continue;
		}
		std::vector< SMAXVertex > vecPoint;
		GetTriangleList( pNodeInfo->pNode, vecPoint );
		if( !vecPoint.empty() )
		{
			int j, nCount;

			nCount = ( int )vecPoint.size() / 3;
			for( j = 0; j < nCount; j++ )
			{
				NAV_MESH_TYPE Type = NMT_PROP;
				int nEvent = 0, nWallAttribute[ 3 ];

				fwrite( &vecPoint[ j * 3 ].Vertex, sizeof( float ), 3, fp );
				fwrite( &vecPoint[ j * 3 + 1 ].Vertex, sizeof( float ), 3, fp );
				fwrite( &vecPoint[ j * 3 + 2 ].Vertex, sizeof( float ), 3, fp );
				fwrite( &nEvent, sizeof( int ), 1, fp );
				fwrite( &Type, sizeof( NAV_MESH_TYPE ), 1, fp );
				memset( nWallAttribute, 0, sizeof( int ) * 3 );
				fwrite( nWallAttribute, sizeof( int ), 3, fp );
				// 버텍스 컬러가 흰색이 아닌 edge는 진출입점으로 본다.
				if( vecPoint[ j * 3 ].dwVertexColor != 0xffffffff )
				{
					if( vecPoint[ j * 3 + 1 ].dwVertexColor != 0xffffffff )
					{
						vecEntrance.push_back( Header.nTriangleCount + j );
						vecEntrance.push_back( 0 );
					}
					else if( vecPoint[ j * 3 + 2 ].dwVertexColor != 0xffffffff )
					{
						vecEntrance.push_back( Header.nTriangleCount + j );
						vecEntrance.push_back( 2 );
					}
				}
				else if( vecPoint[ j * 3 + 1 ].dwVertexColor != 0xffffffff )
				{
					if( vecPoint[ j * 3 + 2 ].dwVertexColor != 0xffffffff )
					{
						vecEntrance.push_back( Header.nTriangleCount + j );
						vecEntrance.push_back( 1 );
					}
				}

			}
			Header.nTriangleCount += ( int )vecPoint.size() / 3;
		}
	}
	int nSide = -1;
	for( i = 0; i < Header.nTriangleCount; i++ )
	{
		fwrite( &nSide, sizeof( int ), 1, fp );
		fwrite( &nSide, sizeof( int ), 1, fp );
		fwrite( &nSide, sizeof( int ), 1, fp );
	}
	if( !vecEntrance.empty() )
	{
		int nEntranceCount;
		nEntranceCount = vecEntrance.size() / 2;
		fwrite( &nEntranceCount, sizeof( int ), 1, fp );
		fwrite( &vecEntrance[ 0 ], sizeof( int ), nEntranceCount * 2, fp );
	}
	fseek( fp, 0, SEEK_SET );
	fwrite( &Header, sizeof( SNavFileHeader ), 1, fp );
	fclose( fp );
	m_pMaxInterface->ProgressEnd();
}

int ActorEt::GetTriangleList( INode *pNode, std::vector< SMAXVertex > &vecPoint )
{
	int i, j, nNeedDel, nFaceCount;
	Matrix3 NormalTM;
	bool bNegScale;
	TriObject *pTri = GetTriObjectFromNode( pNode, 0, nNeedDel );
	if( pTri == NULL )
	{
		return 0;
	}

	NormalTM = pNode->GetObjTMAfterWSM( 0 );
	bNegScale = TMNegParity( NormalTM );

	std::vector< SMAXVertex > VertexList;
	std::vector< std::string > BoneList;
	Mesh *pMesh = &pTri->GetMesh();
	GetVertexList( pNode, &pTri->GetMesh(), VertexList, BoneList );

	nFaceCount = pMesh->getNumFaces();
	for( i = 0; i < nFaceCount; i++ )
	{
		Face *pTriFace;

		pTriFace = pMesh->faces + i;
		for( j = 0; j < 3; j++ )
		{
			int nFaceIndex;

			if( bNegScale )
			{
				nFaceIndex = j;
			}
			else
			{
				nFaceIndex = 2 - j;
			}
			vecPoint.push_back( VertexList[ pTriFace->getVert( nFaceIndex ) ] );
		}
	}

	if( nNeedDel )
	{
		delete pTri;
	}

	return ( int )vecPoint.size();
}

int ActorEt::ProcessCollisionPrimitive( FILE *fp, INode *pNode, Point3 &MaxVec, Point3 &MinVec )
{
	int nParamIndex, nCount;
	CollisionType Type;
	Interval Inter;
	Object *pObject;
	Matrix3 NormalTM;
	IParamArray *iPrimitiveParams;
	char szParentName[ 1024 ];

	pObject = pNode->EvalWorldState( 0 ).obj;
	NormalTM = pNode->GetObjTMAfterWSM( 0 );

	if( pObject == NULL )
	{
		return 0;
	}
	iPrimitiveParams = pObject->GetParamBlock();

	CalcBoundingBox( pNode, MaxVec, MinVec );
	if( iPrimitiveParams == NULL )
	{
		std::vector< SMAXVertex > vecPoint;
		Point3 Edge;
		if( GetTriangleList( pNode, vecPoint ) <= 0 )
		{
			return 0;
		}

		Type = CT_TRIANGLES;

		fwrite( &Type, sizeof( CollisionType ), 1, fp );
		if( pNode->GetParentNode()->GetParentNode() )
		{
			strcpy( szParentName, pNode->GetParentNode()->GetName() );
			nCount = strlen( szParentName ) + 1;
		}
		else
		{
			nCount = 0;
		}
		fwrite( &nCount, sizeof( int ), 1, fp );
		if( nCount > 1 )
		{
			fwrite( szParentName, nCount, 1, fp );
		}

		int i;

		nCount = ( int )vecPoint.size() / 3;
		fwrite( &nCount, sizeof( int ), 1, fp );
		for( i = 0; i < nCount; i++ )
		{
			fwrite( &vecPoint[ i * 3 ], sizeof( Point3 ), 1, fp );
			Edge = vecPoint[ i * 3 + 1 ].Vertex - vecPoint[ i * 3 ].Vertex;
			fwrite( &Edge, sizeof( Point3 ), 1, fp );
			Edge = vecPoint[ i * 3 + 2 ].Vertex - vecPoint[ i * 3 ].Vertex;
			fwrite( &Edge, sizeof( Point3 ), 1, fp );
		}

		return 1;
	}
	else if( pObject->ClassID() == Class_ID( BOXOBJ_CLASS_ID, 0 ) )
	{
		SOBB Box;
		float fXSize, fYSize, fZSize;

		nParamIndex = pObject->GetParamBlockIndex( BOXOBJ_WIDTH );
		iPrimitiveParams->GetValue( nParamIndex, TimeValue( 0 ), fXSize, Inter );
		Box.Extent[ 0 ] = fXSize / 2;
		nParamIndex = pObject->GetParamBlockIndex( BOXOBJ_HEIGHT );
		iPrimitiveParams->GetValue( nParamIndex, TimeValue( 0 ), fYSize, Inter );
		Box.Extent[ 1 ] = fYSize / 2;
		nParamIndex = pObject->GetParamBlockIndex( BOXOBJ_LENGTH );
		iPrimitiveParams->GetValue( nParamIndex, TimeValue( 0 ), fZSize, Inter );
		Box.Extent[ 2 ] = fZSize / 2;

		Type = CT_BOX;
		Box.Center = Point3( 0.0f, 0.0f, fYSize / 2 ) * NormalTM;

		NormalTM.SetRow( 3, Point3( 0.0f, 0.0f, 0.0f ) );
		Box.Axis[ 0 ] = Point3( 1.0f, 0.0f, 0.0f );
		Box.Axis[ 0 ] = Normalize( NormalTM * Box.Axis[ 0 ] );

		Box.Axis[ 1 ] = Point3( 0.0f, 0.0f, 1.0f );
		Box.Axis[ 1 ] = Normalize( NormalTM * Box.Axis[ 1 ] );

		Box.Axis[ 2 ] = Point3( 0.0f, 1.0f, 0.0f );
		Box.Axis[ 2 ] = Normalize( NormalTM * Box.Axis[ 2 ] );

		fwrite( &Type, sizeof( CollisionType ), 1, fp );
		strcpy( szParentName, pNode->GetParentNode()->GetName() );
		nCount = strlen( szParentName ) + 1;
		fwrite( &nCount, sizeof( int ), 1, fp );
		if( nCount > 1 )
		{
			fwrite( szParentName, nCount, 1, fp );
		}
		DumpPoint3( fp, Box.Center );
		DumpPoint3( fp, Box.Axis[ 0 ] );
		DumpPoint3( fp, Box.Axis[ 1 ] );
		DumpPoint3( fp, Box.Axis[ 2 ] );
		fwrite( Box.Extent, sizeof( float ), 3, fp );

		return 1;
	}
	else if( pObject->ClassID() == Class_ID( SPHERE_CLASS_ID, 0 ) )
	{
		float fRadius;
		SSphere Sphere;

		nParamIndex = pObject->GetParamBlockIndex( CIRCLE_RADIUS );
		iPrimitiveParams->GetValue( nParamIndex, TimeValue( 0 ), fRadius, Inter );

		Type = CT_SPHERE;
		Sphere.Center = Point3( 0.0f, 0.0f, 0.0f ) * NormalTM;

		fwrite( &Type, sizeof( CollisionType ), 1, fp );
		if( pNode->GetParentNode()->GetParentNode() )
		{
			strcpy( szParentName, pNode->GetParentNode()->GetName() );
			nCount = strlen( szParentName ) + 1;
		}
		else
		{
			nCount = 0;
		}
		fwrite( &nCount, sizeof( int ), 1, fp );
		if( nCount > 1 )
		{
			fwrite( szParentName, nCount, 1, fp );
		}
		DumpPoint3( fp, Sphere.Center );
		fwrite( &fRadius, sizeof( float ), 1, fp );

		return 1;
	}
	else if( pObject->ClassID() == Class_ID( CYLINDER_CLASS_ID, 0 ) )
	{
		float fRadius, fHeight;
		SCylinder Cylinder;

		nParamIndex = pObject->GetParamBlockIndex( CYLINDER_RADIUS );
		iPrimitiveParams->GetValue( nParamIndex, TimeValue( 0 ), fRadius, Inter );

		nParamIndex = pObject->GetParamBlockIndex( CYLINDER_HEIGHT );
		iPrimitiveParams->GetValue( nParamIndex, TimeValue( 0 ), fHeight, Inter );

		Type = CT_CYLINDER;
		Cylinder.Origin = Point3( 0.0f, 0.0f, 0.0f ) * NormalTM;
		NormalTM.SetRow( 3, Point3( 0.0f, 0.0f, 0.0f ) );
		Cylinder.Direction = Point3( 0.0f, 0.0f, fHeight ) * NormalTM;

		fwrite( &Type, sizeof( CollisionType ), 1, fp );
		if( pNode->GetParentNode()->GetParentNode() )
		{
			strcpy( szParentName, pNode->GetParentNode()->GetName() );
			nCount = strlen( szParentName ) + 1;
		}
		else
		{
			nCount = 0;
		}
		fwrite( &nCount, sizeof( int ), 1, fp );
		if( nCount > 1 )
		{
			fwrite( szParentName, nCount, 1, fp );
		}
		DumpPoint3( fp, Cylinder.Origin );
		DumpPoint3( fp, Cylinder.Direction );
		fwrite( &fRadius, sizeof( float ), 1, fp );

		return 1;
	}
	else if( pObject->ClassID() == CAPS_CLASS_ID )
	{
		float fRadius, fHeight;
		SCylinder Cylinder;
		Point3 NormalDir;

		nParamIndex = pObject->GetParamBlockIndex( CAPS_RADIUS );
		iPrimitiveParams->GetValue( nParamIndex, TimeValue( 0 ), fRadius, Inter );

		nParamIndex = pObject->GetParamBlockIndex( CAPS_HEIGHT );
		iPrimitiveParams->GetValue( nParamIndex, TimeValue( 0 ), fHeight, Inter );

		fHeight -= fRadius * 2;
		if( fHeight < 0.0f )
		{
			fHeight = 0.0f;
		}

		Type = CT_CYLINDER;
		Cylinder.Origin = Point3( 0.0f, 0.0f, 0.0f ) * NormalTM;
		NormalTM.SetRow( 3, Point3( 0.0f, 0.0f, 0.0f ) );
		Cylinder.Direction = Point3( 0.0f, 0.0f, fHeight ) * NormalTM;
		NormalDir = Normalize( Cylinder.Direction );
		Cylinder.Origin = Cylinder.Origin + NormalDir * fRadius;

		fwrite( &Type, sizeof( CollisionType ), 1, fp );
		if( pNode->GetParentNode()->GetParentNode() )
		{
			strcpy( szParentName, pNode->GetParentNode()->GetName() );
			nCount = strlen( szParentName ) + 1;
		}
		else
		{
			nCount = 0;
		}
		fwrite( &nCount, sizeof( int ), 1, fp );
		if( nCount > 1 )
		{
			fwrite( szParentName, nCount, 1, fp );
		}
		DumpPoint3( fp, Cylinder.Origin );
		DumpPoint3( fp, Cylinder.Direction );
		fwrite( &fRadius, sizeof( float ), 1, fp );

		return 1;
	}

	return 0;
}

void ActorEt::WriteCamera( const char *pFileName )
{
	FILE *fp;
	CSaveCamera SaveCamera;

	fp = fopen( pFileName, "wb" );
	if( fp == NULL )
	{
		char str[255];
		sprintf(str, "[%s] 파일이 읽기 전용이라\n 저장되지 않았습니다.", pFileName);
		MessageBox(0, str, "Error Message", MB_OK );
		return;
	}

	INode *pFindNode;
	pFindNode = FindNode( 0, NODE_FLAG_CAMERA );
	if( pFindNode )
	{
		SaveCamera.ProcessCamera( fp, pFindNode, m_pMaxInterface, ( bool )(m_nExportTargetCamera != 0) );
	}

	fclose( fp );
}

Value *save_ani_cf( Value** arg_list, int count )
{
	theActorEt.m_pMaxInterface = MAXScript_interface;
	theActorEt.SaveAni( NULL, arg_list[ 0 ]->to_string() );

	return &true_value;
}

