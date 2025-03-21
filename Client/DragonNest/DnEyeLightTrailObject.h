#pragma once

#include "EtObject.h"
#include "DnUnknownRenderObject.h"

class CDnEyeLightTrailObject : public CDnUnknownRenderObject<CDnEyeLightTrailObject>
{
public:
	CDnEyeLightTrailObject( CDnRenderBase *pRenderBase );
	virtual ~CDnEyeLightTrailObject();

protected:
	struct PointStruct {
		EtObjectHandle hObject;
		EtVector3 vPos;
	};
	std::vector<PointStruct> m_VecPointList;
	int m_nPointIndex;

	std::string m_szLinkBoneName;
	CDnRenderBase *m_pRenderBase;
	float m_fScaleMin;
	float m_fScaleMax;
	bool m_bShow;

protected:
	void Finalize();

public:
	static EtMatrix GetBillboardMatrix( EtVector3 vPos );
	
	bool Initialize( const char *szSkinName, const char *szLinkBoneName, int nCount, float fScaleMin, float fScaleMax );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	bool IsShow() { return m_bShow; }
	void Show( bool bShow );
	void SetScale( float fMin, float fMax ) { m_fScaleMin = fMin; m_fScaleMax = fMax; }
};