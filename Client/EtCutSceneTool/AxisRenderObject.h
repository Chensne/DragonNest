#pragma once

#include "EtMatrixEx.h"
#include "EtObject.h"


// eternity engine �� �����Ͽ� 3d axis �� ������ ��.
class CEtWorldProp;
class CAxisRenderObject : public CEtCustomRender {
public:
	CAxisRenderObject( );//CEtWorldProp *pProp );
	virtual ~CAxisRenderObject();
	struct VERTEX_FORMAT {
		EtVector3 vPos;
		DWORD dwColor;
	};

	enum
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		AXIS_ALL,
		AXIS_NONE,
	};

protected:
	int m_nSelectAxis;
	VERTEX_FORMAT *m_pAxis[3];
	VERTEX_FORMAT *m_pAxisBadak[3];
	VERTEX_FORMAT *m_pSelect[3];
	int m_nPrimitiveCount[3];
	float m_fAxisLength;
	float m_fAxisPos;
	float m_fAxisRadius;
	EtVector3 m_vRotation;

	EtMatrix m_WorldMat;
	float m_fScale;
	bool m_bShow;
	//CEtWorldProp *m_pProp;

	int CreateCone( VERTEX_FORMAT **pVertex, DWORD dwColor, float fRadius, float fLength, int nSegment, int nAxis, float fAxisPos );
public:

	EtVector3 GetPosition() {return *(EtVector3*)&m_WorldMat._41;}
	void SetPosition( EtVector3 pos ) { *(EtVector3*)&m_WorldMat._41 = pos;}
	EtVector3 GetRotation() {return m_vRotation;}
	void SetRotation( EtVector3 rot );
	void SetWorld( EtMatrix& matWorld ) { m_WorldMat = matWorld; };
	void Initialize();
	void Destroy();
	void DrawAxis();

	int CheckAxis( const EtVector3& vCamPos, EtVector3 &vOrig, EtVector3 &vDir );
	void RotateAxis( int nX, int nY );
	//void MoveAxis( int nX, int nY, float fSpeed );
	void SetScale( float fValue ) { m_fScale = fValue; }

	virtual void RenderCustom( float fElapsedTime );
	void Update( EtMatrix *pMatrix );
	void Show( bool bShow ) { m_bShow = bShow; };

	int GetSelectedAxis( void ) { return m_nSelectAxis; };
//	virtual void Render();
//	virtual void GetExtent( EtVector3 &Origin, EtVector3 &Extent );

};