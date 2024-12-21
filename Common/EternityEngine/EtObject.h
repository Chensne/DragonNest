#pragma once

#include "EtSkin.h"
#include "EtSkinInstance.h"
#include "EtMesh.h"
#include "EtSubMesh.h"
#include "EtCollisionEntity.h"

#include "EtOctree.h"

class CEtCustomParam;
class CEtConvexVolume;
class CEtObject;
typedef CSmartPtr< CEtObject >	EtObjectHandle;

class CEtObject : public CSmartPtrBase< CEtObject >, public CEtCollisionEntity, public CBackgroundLoaderCallback
{
public:
	CEtObject();
	virtual ~CEtObject();
	void Clear();

// Parent Link 관련 멤버들
	enum LinkType
	{
		LT_BONE,
		LT_MESH_DUMMY,
		LT_OBJECT,			// #29199 애니 오브젝트의 위치기준 오프셋위치로 직접 링크.
	};

protected:
	bool m_bChild;
	bool m_bShow;
	bool m_bEnable;
	bool m_bEnableCull;
	bool m_bShadowCast;
	bool m_bShadowReceive;
	bool m_bLightMapCast;
	bool m_bWaterCast;
	bool m_bLightMapInfluence;
	bool m_bShowBoundingBox;

	RenderType m_RenderType;
	EtMatrix m_WorldMat;
	EtMatrix m_PrevWorldMat;
	EtSkinHandle m_hSkin;
	CEtSkinInstance *m_pSkinInstance;
	int m_nFrustumMask;
	int m_nSaveMatIndex;
	bool m_bValidSaveMatIndex;

	EtObjectHandle m_hParent;
	EtMatrix m_LinkOffsetMat;
	int m_nLinkBoneIndex;
	LinkType m_LinkType;
	std::vector<EtObjectHandle> m_vecChild;

	float m_fObjectAlpha;

// 충돌 처리 관련 Tree 구성..
	CEtOctreeNode< CEtObject * > *m_pCurOctreeNode;
	static CEtOctree< CEtObject * > *s_pDynamicOctree;

	SAABox m_OriginalBoundingBox;
	SAABox m_BoundingBox;
	SSphere m_OriginalBoundingSphere;
	SSphere m_BoundingSphere;

	bool m_bAniObject;

	DNVector(CEtLight *) m_vecInfluenceLight;

	static	bool	m_bSkipDrawCollision;

public:
	static void InitializeClass();
	static void FinalizeClass();

	int Initialize( EtSkinHandle hSkin );
	void CommonInitialize();
	void CreateSkinInstance();

	RenderType SetRenderType( RenderType Type );
	RenderType GetRenderType() { return m_RenderType; }

	virtual void SetParent( EtObjectHandle hObject, const char *pLinkBoneName, EtMatrix *pOffsetMat = NULL );
	void SetParent( EtObjectHandle hObject, int nBoneIndex, EtMatrix *pOffsetMat = NULL, LinkType Type = LT_BONE );
	void AddChild( EtObjectHandle hObject );
	void RemoveChild( EtObjectHandle hObject );
	EtObjectHandle GetParent() { return m_hParent;}
	int GetLinkBoneIndex() { return m_nLinkBoneIndex; }

	void SetChild( bool bChild ) { m_bChild = bChild; }
	bool IsChild() { return m_bChild; }
	std::vector< EtObjectHandle >& GetChild() {return m_vecChild;}
	void ShowObject( bool bShow ) { m_bShow = bShow; }
	bool IsShow();
	void ShowSubmesh( int nSubmeshIndex, bool bShow );
	bool IsShowSubmesh( int nSubmeshIndex );
	void EnableObject( bool bEnable ) { m_bEnable = bEnable; }
	bool IsEnable() { return m_bEnable; }
	void EnableCull( bool bEnable ) { m_bEnableCull = bEnable; }
	bool IsEnableCull() { return m_bEnableCull; }
	void EnableShadowCast( bool bEnable );
	bool IsShadowCast() { return m_bShadowCast; }
	void EnableShadowReceive( bool bEnable );
	bool IsShadowReceive() { return m_bShadowReceive; }
	void EnableLightMapCast( bool bEnable ) { m_bLightMapCast = bEnable; }
	bool IsLightMapCast() { return m_bLightMapCast; }
	void EnableWaterCast( bool bEnable ) { m_bWaterCast = bEnable; }
	bool IsWaterCast() { return m_bWaterCast; }
	void EnableLightMapInfluence( bool bEnable );
	bool IsLightMapInfluence() { return m_bLightMapInfluence; }
	void ShowBoundingBox( bool bShow ) { m_bShowBoundingBox = bShow; }
	bool IsShowBoundingBox() { return m_bShowBoundingBox; }

	void SetObjectAlpha( float fAlpha );
	float GetObjectAlpha() { return m_fObjectAlpha; }

	EtMatrix *GetWorldMat() { return &m_WorldMat; }
	void SetWorldMat( EtMatrix &WorldMat ) { m_WorldMat = WorldMat; }
	virtual void Update( EtMatrix *pWorldMat );
	virtual EtMatrix *GetBoneMat( int nBoneIndex ) { return NULL; }
	virtual EtMatrix *GetBoneTransMat( int nBoneIndex ) { return NULL; }
	virtual EtMatrix *GetDummyTransMat( int nDummyIndex ) { return NULL; }
	virtual int GetAniFileIndex() { return -1; }

	EtSkinHandle GetSkin() { return m_hSkin; }
	const char *GetSkinFileName() { return m_hSkin->GetFileName(); }

	bool IsAniObject() { return m_bAniObject; }

	void SetFrustumMask( int nMask );
	void AddFrustumMask( int nMask );
	int GetFrustumMask() { return m_nFrustumMask; }

	EtMeshHandle GetMesh() { return ( !m_hSkin ) ? CEtMesh::Identity() : m_hSkin->GetMeshHandle(); }
	int GetSubMeshCount() { return ( !GetMesh() ) ? 0 : GetMesh()->GetSubMeshCount(); }
	CEtSubMesh *GetSubMesh( int nSubMeshIndex ) { return ( !GetMesh() ) ? NULL : GetMesh()->GetSubMesh( nSubMeshIndex ); }
	const char *GetSubMeshName(int nSubMeshIndex )	{ return GetSubMesh( nSubMeshIndex )->GetSubMeshName(); }
	int GetSubMeshIndex( const char *pSubMeshName ) { return ( !GetMesh() ) ? -1 : GetMesh()->GetSubMeshIndex( pSubMeshName ); }

	int GetVertexCount( int nSubMeshIndex ) { return GetSubMesh( nSubMeshIndex )->GetVertexCount(); }
	int GetVertexStream( int nSubMeshIndex, MeshStreamType Type, void **ppStream ) 
		{ return GetSubMesh( nSubMeshIndex )->GetVertexStream( Type, ppStream ); }
	void SetVertexStream( int nSubMeshIndex, MeshStreamType Type, void *pStream ) 
		{ GetSubMesh( nSubMeshIndex )->SetVertexStream( Type, pStream ); }

	EtMaterialHandle GetMaterial( int nSubMeshIndex ) { return m_hSkin->GetMaterialHandle( nSubMeshIndex ); }
	int GetMaterialCount() { return m_hSkin->GetMaterialCount(); }

	int GetEffectCount( int nSubMeshIndex ) { return m_hSkin->GetEffectCount( nSubMeshIndex ); }
	SCustomParam *GetCustomParam( int nSubMeshIndex, int nEffectIndex )
		{ return m_hSkin->GetCustomParam( nSubMeshIndex, nEffectIndex ); }
	void SetEffect( int nSubMeshIndex, int nEffectIndex, SCustomParam *pParam )
		{ m_hSkin->SetEffect( nSubMeshIndex, nEffectIndex, pParam ); }
	void SetTexture( int nSubMeshIndex, int nEffectIndex, const char *pTexName )
		{ return m_hSkin->SetTexture( nSubMeshIndex, nEffectIndex, pTexName ); }

	void CalcParentBoneMat();
	virtual void InitRender( int nSaveMatIndex = -1 );
	virtual void Render();
	void RenderImmediate( bool bRenderChild );
	void RenderAlphaImmediate( bool bRenderChild );
	virtual void RenderShadow();
	virtual void RenderWater( int index);

	int GetSaveMatIndex() {return m_nSaveMatIndex;}
	void SetSaveMatIndex( int nIndex );

	void GetMeshBoundingBox( SAABox &AABox );
	void RecalcBoundingBox();
	void RecalcBoundingSphere();
	void GetBoundingBox( SOBB &OBBox ) { OBBox.Init( m_OriginalBoundingBox, m_WorldMat ); }
	void GetBoundingBox( SAABox &AABox ) { AABox = m_BoundingBox; }
	void GetMeshBoundingSphere( SSphere &Sphere );
	void GetBoundingSphere( SSphere &Sphere, bool bActorSize = false ) { Sphere = m_BoundingSphere; }
	virtual void GetExtent( EtVector3 &Origin, EtVector3 &Extent );
	void DrawBoundingBox( DWORD dwColor = 0xffffffff );
	void UpdateBoundingPrimitive();

	virtual void UpdateCollisionPrimitive( EtMatrix &WorldMat, bool bUpdate = true );
	bool FindDynamicCollision( EtMatrix &WorldMat, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime = true, bool bUpdaetPrimitive = true);
	bool _FindDynamicCollision( SAABox &BoundingBox, EtVector3 &vMove, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime);
	bool FindCollision( EtMatrix &WorldMat, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime = true, bool bUpdaetPrimitive = true);
	bool FindCollision( EtObjectHandle hObject, std::vector< SCollisionResponse > &vecResponse, bool bCalcContactTime = true );

	int AddCustomParam( const char *pParamName );
	bool SetCustomParam( int nParamIndex, void *pValue, int nSubMeshIndex = -1 );
	void RestoreCustomParam( int nParamIndex, int nSubMeshIndex = -1 );

	static void ClearFrustumMask();
	static void MaskFrustumObjectList( CEtConvexVolume *pFrustum, int nFrustumMask );
	static void InitRenderObjectList();
	static void RenderObjectList( int nMask );
	static void RenderShadowObjectList( int nMask, float fShadowRange );
	static void DrawCollisionList( int nMask );
	static void SkipDrawCollisionList( bool bSkip ) { m_bSkipDrawCollision = bSkip; }
	void CalcLightInfluence();
	static void CalcLightInfluenceList( bool bForce = false );
	void CalcLightMapInfluence();
	static void RecalcLightMapInfluence();

	static void SetWorldSize( EtVector3 &WorldCenter, float fRadius );
	static EtObjectHandle Pick( int nX, int nY, int nCameraIndex = 0 );

	virtual void OnLoadComplete( CBackgroundLoader *pLoader );
	CEtSkinInstance *GetSkinInstance() { return m_pSkinInstance; }
	//void SetBlendParam( int blendOp, int srcBlend, int destBlend ) { m_nBlendOP = blendOp; m_nSrcBlend = srcBlend; m_nDestBlend = destBlend;}

	void AddCollisionMeshToKdTree();
	static void BuildKdTree();

//	void AddToListByLock();
//	void DeleteFromListByLock();

	// 이펙트툴 스킨 리로드 기능 위한 함수
	void ForceSetSkin( EtSkinHandle hSkin ) { m_hSkin = hSkin; m_pSkinInstance->CopySkinInfo( m_hSkin ); }
};

