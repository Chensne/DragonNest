#pragma once

#include "EtMatrixEx.h"
#include "EtObject.h"

class CEtWorldProp;
class CAxisRenderObject : public CEtCustomRender {
public:
	CAxisRenderObject( );//CEtWorldProp *pProp );
	virtual ~CAxisRenderObject();
	struct VERTEX_FORMAT {
		EtVector3 vPos;
		DWORD dwColor;
	};

public:
	static int m_nSelectAxis;
	static bool m_bInitialize;
	static VERTEX_FORMAT *m_pAxis[3];
	static VERTEX_FORMAT *m_pAxisBadak[3];
	static VERTEX_FORMAT *m_pSelect[3];
	static int m_nPrimitiveCount[3];
	static float s_fAxisLength;
	static float s_fAxisPos;
	static float s_fAxisRadius;
	static EtVector3 s_vRotation;

	EtMatrix m_WorldMat;
	float m_fScale;
	static int s_nRefCount;
	//CEtWorldProp *m_pProp;

	int CreateCone( VERTEX_FORMAT **pVertex, DWORD dwColor, float fRadius, float fLength, int nSegment, int nAxis, float fAxisPos );
public:

	EtVector3 GetPosition() {return *(EtVector3*)&m_WorldMat._41;}
	void SetPosition( EtVector3 pos ) { *(EtVector3*)&m_WorldMat._41 = pos;}
	EtVector3 GetRotation() {return s_vRotation;}
	void SetRotation( EtVector3 rot );
	void Initialize();
	void Destroy();
	void DrawAxis();

	int CheckAxis( EtVector3 &vOrig, EtVector3 &vDir );
	void RotateAxis( int nX, int nY );
	void MoveAxis( int nX, int nY, float fSpeed );
	void SetScale( float fValue ) { m_fScale = fValue; }

	virtual void RenderCustom( float fElapsedTime );
	void Update( EtMatrix *pMatrix );
//	virtual void Render();
//	virtual void GetExtent( EtVector3 &Origin, EtVector3 &Extent );

};