#pragma once

#include "EtSkin.h"
#include "EtMesh.h"
#include "EtCollisionEntity.h"

#include "EtOctree.h"

#include "MultiSmartPtr.h"
#include "MultiCommon.h"

class CEtObject;
typedef CMultiSmartPtr< CEtObject > EtObjectHandle;

class CEtObject : public CMultiSmartPtrBase< CEtObject, MAX_SESSION_COUNT >, public CEtCollisionEntity
{
public:
	CEtObject( CMultiRoom *pRoom );
	virtual ~CEtObject();
	void Clear();

protected:
	EtMatrix m_WorldMat;
	EtSkinHandle m_hSkin;

	CEtOctreeNode< CEtObject * > *m_pCurOctreeNode;
	STATIC_DECL( CEtOctree< CEtObject * > s_DynamicOctree );

	SAABox m_OriginalBoundingBox;
	SAABox m_BoundingBox;
	SSphere m_OriginalBoundingSphere;
	SSphere m_BoundingSphere;

public:
	int Initialize( EtSkinHandle hSkin );
	void CommonInitialize();

	EtMatrix *GetWorldMat() { return &m_WorldMat; }
	void SetWorldMat( EtMatrix &WorldMat ) { m_WorldMat = WorldMat; }
	virtual void Update( EtMatrix *pWorldMat );
	virtual EtMatrix *GetBoneMat( int nBoneIndex ) { return NULL; }
	virtual EtMatrix *GetBoneTransMat( int nBoneIndex ) { return NULL; }
	virtual int GetAniFileIndex() { return -1; }

	EtSkinHandle GetSkin() { return m_hSkin; }
	EtMeshHandle GetMesh() { return m_hSkin->GetMeshHandle(); }
	const char *GetSkinFileName() { return m_hSkin->GetFileName(); }

	void GetMeshBoundingBox( SAABox &AABox );
	void RecalcBoundingBox();
	void RecalcBoundingSphere();
	void GetBoundingBox( SOBB &OBBox ) { OBBox.Init( m_OriginalBoundingBox, m_WorldMat ); }
	void GetBoundingBox( SAABox &AABox ) { AABox = m_BoundingBox; }
	void GetOriginalBoundingBox( SAABox &AABox ) { AABox = m_OriginalBoundingBox; }
	void GetMeshBoundingSphere( SSphere &Sphere );
	void GetBoundingSphere( SSphere &Sphere, bool bActorSize = false ) { Sphere = m_BoundingSphere; }
	virtual void GetExtent( EtVector3 &Origin, EtVector3 &Extent );
	void UpdateBoundingPrimitive();

	virtual void UpdateCollisionPrimitive( EtMatrix &WorldMat );
	bool FindCollision( EtMatrix &WorldMat, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime = true, bool bUpdatePrimitive = true );
	bool FindDynamicCollision( EtMatrix &WorldMat, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime = true, bool bUpdatePrimitive = true );
	bool _FindDynamicCollision( SAABox &BoundingBox, EtVector3 &vMove, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime );
	bool FindCollision( EtObjectHandle hObject, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime = true );

	static void SetWorldSize( CMultiRoom *pRoom, EtVector3 &WorldCenter, float fRadius );
};

class IBoostPoolEtObject:public CEtObject, public TBoostMemoryPool< IBoostPoolEtObject >
{
public:
	IBoostPoolEtObject( CMultiRoom *pRoom ):CEtObject(pRoom){}
	virtual ~IBoostPoolEtObject(){}
};
