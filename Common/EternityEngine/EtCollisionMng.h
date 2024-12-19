#pragma once

#include "KdTree.h"
#include "EtCollisionPrimitive.h"

class CEtCollisionMng : public CSingleton< CEtCollisionMng >
{
public:
	CEtCollisionMng(void);
	virtual ~CEtCollisionMng(void);
	void Clear();

protected:
	TKdTree< SKdTreeCollisionPrimitive > m_CollisionKdTree;
	std::vector< SCollisionPrimitive * > m_vecAllocPrimitive;

public:
	void AddCollisionMeshToKdTree( SCollisionPrimitive &Primitive, EtMatrix &WorldMat, const EtVector3 &vScale );
	void BuildCollisionTree();

	bool FindCollision( std::vector< SCollisionPrimitive * > &vecPrimitive, SAABox &BoundingBox, EtVector3 &vMove, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime);

	void Save( CStream *pStream ) { m_CollisionKdTree.SaveTree( pStream ); }
	void Load( CStream *pStream ) { m_CollisionKdTree.LoadTree( pStream ); }
};
