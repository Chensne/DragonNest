/**********************************************************************
 *<
	FILE: ActorEt.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/
#pragma once 

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
//SIMPLE TYPE

#include <vector>

#include "utilapi.h"
#include "ExportUtil.h"
#include "SaveMesh.h"
#include "SaveSkin.h"
#include "SaveAni.h"

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;
extern int g_nAniQuality;


#define NODE_FLAG_MESH		0x00000001
#define NODE_FLAG_BONE		0x00000002
#define NODE_FLAG_COLLISION	0x00000004
#define NODE_FLAG_CAMERA	0x00000008
#define NODE_FLAG_DUMMY 0x00000010
struct SNodeInfo
{
	INode *pNode;
	int nNodeFlag;
};

class ActorEt : public UtilityObj 
{
public:
	ActorEt();
	~ActorEt();		

public:
	HWND			hPanel;
	HWND			m_hOptionPanel;
	IUtil			*iu;
	Interface		*m_pMaxInterface;
	
	WinRegistry m_RegUtil;

	INode *m_pBoundingBox;
	std::vector< SNodeInfo > m_vecNodeList;
	std::vector< Mtl * > m_MaterialList;

// Option Parameter
	int m_nExportSelected;
	int m_nUseStrip;
	int m_nGenerateLOD;
	int m_nGenerateUVAni;
	int m_nExportVertexColor;
	int m_nExportTargetCamera;

	int m_nNoInfluenceBoneError;
	int m_nInfluenceBoneExceedError;
	int m_nMaxWeightZeroError;
	
	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);

	void Init(HWND hWnd);
	void Destroy(HWND hWnd);

	void InitOption(HWND hWnd);
	void DestroyOption(HWND hWnd);

	void SaveMesh( HWND hWnd );
	void SaveAni( HWND hWnd, char *pFileName = NULL );
	void SaveCamera( HWND hWnd );
	void SaveNavigation( HWND hWnd );

	void DeleteThis() { }		

	void GetNodeList();
	void AddNodeList( INode *pNode );
	bool IsMeshObject( INode *pNode );
	bool IsBoneObject( INode *pNode );
	bool IsCameraObject( INode *pNode );
	bool IsDummyObject( INode *pNode );
	int GetNodeCount( int nNodeFlag );
	INode *FindNode( int nIndex, int nNodeFlag );
	SNodeInfo *FindNodeInfo( int nIndex, int nNodeFlag );

	void GetCollisionMeshList();
	void AddCollisionMeshList( INode *pNode );
	bool IsCollisionMeshObject( INode *pNode );

	void GetSelectedBone( std::vector< std::string > &vecSelectedBone );

//////////////////////////////////////////////////////
	void WriteMesh( const char *pFileName );
	int ProcessSubMesh( FILE *fp, INode *pNode, Point3 &MaxVec, Point3 &MinVec, bool bUseBone = false );
	TriObject *GetTriObjectFromNode( INode *pNode, TimeValue Time, int &deleteIt );
	void GetMaterialList( INode *pNode, std::vector< Mtl * > &MaterialList );
	bool GetVertexList( INode *pNode, std::vector< SMAXVertex > &VertexList, std::vector< std::string > &BoneList );
	bool GetVertexList( INode *pNode, Mesh *pMesh, std::vector< SMAXVertex > &VertexList, std::vector< std::string > &BoneList );
	Point3 GetNodeCenterPos( INode *pNode );
	int GetPhysiqueData( INode *pNode, std::vector< SMAXVertex > &VertexList, std::vector< std::string > &BoneList );
	int GetSkinData( INode *pNode, std::vector< SMAXVertex > &VertexList, std::vector< std::string > &BoneList );
	int AddBoneNameList( std::vector< std::string > &BoneList, const char *pBoneName );
////
/*	int GetTextureChannelCount( Mtl *pMtl );
	void WriteSubMesh( FILE *fp, INode *pNode, Mtl *pMtl, std::vector< SLocalVertex > &VertexList, std::vector< WORD > &FaceList );
	int GetPhysiqueData( INode *pNode, std::vector< SLocalVertex > &VertexList );*/
	void WriteAni( const char *pFileName );
	void WriteSkin( const char *pFileName, const char *pMeshName );
	void CalcBoundingBox( INode *pNode, Point3 &MaxVec, Point3 &MinVec );
	void GetBoundingBox( std::vector< SMAXVertex > &SystemVertex, Point3 &MaxVec, Point3 &MinVec );

	void WriteNavigation( const char *pFileName );

	int GetTriangleList( INode *pNode, std::vector< SMAXVertex > &vecPoint );
	int ProcessCollisionPrimitive( FILE *fp, INode *pNode, Point3 &MaxVec, Point3 &MinVec );

///////////////////////////////////////////////////////////////////////////
	void WriteCamera( const char *pFileName );
};

