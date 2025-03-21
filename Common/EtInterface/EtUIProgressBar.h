#pragma once
#include "etuicontrol.h"

class CEtUIProgressBar : public CEtUIControl
{
	enum PROGRESSBAR_TYPE
	{
		PBT_HORI,
		PBT_VERT,
	};

public:
	CEtUIProgressBar( CEtUIDialog *pParent );
	virtual ~CEtUIProgressBar(void);

protected:

	SUICoord m_ProgressCoord;	
	DWORD	m_dwTick;

	bool	m_bSmooth;
	bool	m_bUseLazy;
	bool	m_bUseLayer;
	bool    m_bUseProgressIndex;
	bool    m_bImmediateChangeNeeded;
	bool    m_bReverse; // bintitle - ProgressBar 반전 : ExpGauge 와는 달리 ProgressBar 에 기본틀 모양이 있는경우(Element가 복수) 사용.
	int		m_nProgressIndex;		// 게이지바 색을 여러개 지정해둔경우 선택해서 사용한다.
	int		m_nProgressCount;
	int		m_nLightShadeElementIndex;
	int		m_nDarkShadeElementIndex;
	int		m_nGaugeEndElementIndex;	// 100% 안채워져있을때 게이지 끝에다가 뭔가 모양을 표시할거라면 여기에 인덱스를 넣는다.

	float	m_fProgress;
	float	m_fProgressSmooth;

	std::vector<int> m_nVecLayerElementList;

public:

	virtual void Initialize( SUIControlProperty *pProperty );

	virtual bool CanHaveFocus() { return false; }
	virtual void UpdateRects();
	virtual void Render( float fElapsedTime );

public:

	bool IsUseLayer() { return m_bUseLayer; }
	
	int GetLayerCount() { return (int)m_nVecLayerElementList.size(); }
	int GetProgressIndex() { return m_nProgressIndex; }
	float GetProgress() { return m_fProgress; }
	
	void SetProgress( float fProgress );
	void SetSmooth( bool bSmooth ) { m_bSmooth = bSmooth; }
	void SetImmediateChange() {m_bImmediateChangeNeeded = true;}
	void SetProgressIndex( int nIndex ) { m_nProgressIndex = nIndex; }
	void UseProgressIndex( bool bUse ) { m_bUseProgressIndex = bUse; }
	void UseLayer(bool bLayer, int nLayerElementCount, ... );

	// 프로그래스 인덱스때문에 엘리멘트인덱스가 유동적이라 인자로 받아서 설정한다. 0이면 사용안함.
	void UseShadeProcessBar( int nLightElementIndex, int nDarkElementIndex );
	// 게이지 끝에 뭐 표시하고 싶으면, 엘리먼트 넣기. 0이면 사용안함.
	void UseGaugeEndElement( int nGaugeEndElementIndex );

	void SetReverse( bool bR ){
		m_bReverse = bR;
	}
	bool IsReverse(){ return m_bReverse; }

};
