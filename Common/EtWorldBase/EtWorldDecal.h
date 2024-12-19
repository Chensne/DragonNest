#pragma once

#include "EtQuadtree.h"
class CEtWorldSector;
class CEtWorldDecal {
public:
	CEtWorldDecal( CEtWorldSector *pSector );
	virtual ~CEtWorldDecal();

	struct DecalStruct {
		EtVector2 vPos;
		float fRadius;
		float fRotate;
		EtColor vColor;
		float fAlpha;
		char szTextureName[64];
		char szDummy[128];
	};
protected:
	CEtWorldSector *m_pParentSector;
	EtVector2 m_vPos;
	float m_fRadius;
	float m_fRotate;
	EtColor m_vColor;
	float m_fAlpha;

	std::string m_szTextureName;
	CEtQuadtreeNode<CEtWorldDecal*> *m_pCurrentNode;

public:
	virtual bool Initialize( EtVector2 vPos, float fRadius, float fRotate, EtColor Color, float fAlpha, const char *szTextureName );

	void SetCurQuadtreeNode( CEtQuadtreeNode<CEtWorldDecal *> *pNode ) { m_pCurrentNode = pNode; }
	CEtQuadtreeNode<CEtWorldDecal *> *GetCurQuadtreeNode() { return m_pCurrentNode; }

	void GetBoundingCircle( SCircle &Circle );

	const char *GetTextureName() { return m_szTextureName.c_str(); }
	float GetRadius() { return m_fRadius; }
	float GetRotate() { return m_fRotate; }
	EtColor GetColor() { return m_vColor; }
	EtVector2 GetPosition() { return m_vPos; }
	float GetAlpha() { return m_fAlpha; }
};