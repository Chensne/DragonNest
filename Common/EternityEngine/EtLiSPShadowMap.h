#pragma once

#include "EtShadowMap.h"
#include "EtPrimitive.h"

struct SPolygon
{
	std::vector< EtVector3 > Polygon;
};

class CEtLiSPShadowMap : public CEtShadowMap
{
public:
	CEtLiSPShadowMap(void);
	virtual ~CEtLiSPShadowMap(void);
	void Clear();

protected:
	SAABox m_SceneBox;
	std::vector< EtVector3 > m_vecFocusRegion;
	EtVector3 m_NearCameraPoint;

	EtVector3 m_CamPos;
	EtVector3 m_CamDir;
	EtVector3 m_LightDir;

public:
	void Initialize( ShadowQuality Quality );

	void CalcFrustumPoly( std::vector< SPolygon > &vecPolygon );
	void ClipFrustumBySceneBox(  std::vector< SPolygon > &vecPolygon );
	void ClipPolygonByPlane( std::vector< EtVector3 > &Polygon, EtVector4 &Plane, 
		std::vector< EtVector3 > &PolyOut, std::vector< EtVector3 > &PolyIntersect );
	void ClosePolygon( std::vector< EtVector3 > &PolyOut, std::vector< SPolygon > &IntersectEdges, EtVector4 &Plane );
	void ClipFrustumByPlane( std::vector< SPolygon > &vecPolygon, EtVector4 &Plane, std::vector< SPolygon > &vecOutput );
	void CalcNearCameraPoint();
	void GetSCameraInfo();
	void CalcFocusRegion();
	void CalcLightSpaceViewProjDir( EtVector3 &LightSpaceViewProjDir );
	void CalcZ0LS( EtVector3 &Output, EtMatrix &LightSpaceMat, float fBoxMaxZ );
	float CalcNParam( EtMatrix &LightSpaceMat, SAABox &Box );
	void CalcLiSPSMat();
	void RescaleShadowMat();
	void CalcUpVec( EtVector3 &Output );
	void CalcShadowMat();

	void BeginShadow();
	void EndShadow();
};

//void CreateLiSPSahdowMap( ShadowQuality Quality );
