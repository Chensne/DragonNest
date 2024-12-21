#pragma once

#include "EtWorldWater.h"

class CEtWorldWaterRiver
{
public:
	CEtWorldWaterRiver( CEtWorldWater *pWater );
	virtual ~CEtWorldWaterRiver();

	enum InterpolationType {
		Linear,
		CatmullRom,
		Helmite,
	};

protected:
	CEtWorldWater *m_pWater;
	std::string m_szName;
	std::string m_szShaderName;
	std::vector<EtVector3> m_vVecPointList;

	float m_fWidth;
	InterpolationType m_InterpolationType;
	// 워터리버를 사막모래 흐르는 곳에도 사용하게 되면서 약간 문제가 발생했다.
	// 사막드래곤맵이 엄청 큰 맵인데, 띄엄띄엄 6개의 워터리버를 두니,
	// 각 워터리버에서 Bake하면서 오브젝트를 그리는 바람에 속도가 엄청 떨어지게 된 것이다.
	// 사막모래 흐르는 skn을 따로 만들 수도 있지만, 시간도 문제고, 지형에 맞춰서 스킨을 제작하는 것도 힘들어서,
	// 어차피 반사를 안하니 반사 루틴을 패스하는 속성을 하나 만들기로 했다. m_bIgnoreReflection 이런식으로.
	// 근데, 추가하려다보니 모든 Water.ini를 재저장해야했고,(버전이 없어서 일괄변환해야했다.)
	// 새파일을 만드는것도 보기 좀 그렇고, 해외빌드까지 관리하기엔 불편한 점이 많았다.
	// 그래서 내린 결론이 안쓰는 속성중에 하나를 반사 아님을 알리는 옵션으로 쓰자는 거였다.(정말 이상하지만..)
	// m_InterpolationType는 현재 리니어만 사용되며 다른 항목은 아예 안쓰인다.
	// 이걸 CatmullRom으로 설정할 경우 리니어처럼 똑같이 쓰되, Bake하지 않는거로 처리하겠다.

	BYTE *m_pAlphaTable;

	int m_nAlphaTableWidth;
	int m_nAlphaTableHeight;
	EtVector2 m_vUVRatio;
//	float m_fWaterLevel;

	int m_nBorderReduction;
	int m_nGaussianValue;
	float m_fGaussianEpsilon;
	int m_nMinAlpha;
	int m_nMaxAlpha;
	float m_fRevisionAlpha;

	std::vector<CEtWorldWater::CustomParamStruct *> m_pVecCustomParamList;
	CEtQuadtreeNode<CEtWorldWaterRiver *> *m_pCurrentNode;
	std::vector<EtVector3> m_vVecPrimitiveList;

	EtVector2 m_vSize;
	EtVector2 m_vOffset;
	EtVector2 m_vMin;

protected:
	void CalcSize();
	void GenerationPointList();

public:
	virtual bool Load( CStream *pStream );
	const char *GetName() { return m_szName.c_str(); }
	void SetName( char *szName ) { m_szName = szName; }

	const char *GetShaderName() { return m_szShaderName.c_str(); }
	void SetShaderName( char *szName ) { m_szShaderName = szName; }

	CEtWorldWater *GetWater() { return m_pWater; }

	void SetCurQuadtreeNode( CEtQuadtreeNode<CEtWorldWaterRiver *> *pNode ) { m_pCurrentNode = pNode; }
	CEtQuadtreeNode<CEtWorldWaterRiver *> *GetCurQuadtreeNode() { return m_pCurrentNode; }
	void GetBoundingCircle( SCircle &Circle );

	bool GetHeight( float fX, float fZ, float &fHeight );
	float Pick( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos );

	float GetRevisionAlpha() { return m_fRevisionAlpha; }
};