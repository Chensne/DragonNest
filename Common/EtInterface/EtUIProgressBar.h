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
	bool    m_bReverse; // bintitle - ProgressBar ���� : ExpGauge �ʹ� �޸� ProgressBar �� �⺻Ʋ ����� �ִ°��(Element�� ����) ���.
	int		m_nProgressIndex;		// �������� ���� ������ �����صа�� �����ؼ� ����Ѵ�.
	int		m_nProgressCount;
	int		m_nLightShadeElementIndex;
	int		m_nDarkShadeElementIndex;
	int		m_nGaugeEndElementIndex;	// 100% ��ä���������� ������ �����ٰ� ���� ����� ǥ���ҰŶ�� ���⿡ �ε����� �ִ´�.

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

	// ���α׷��� �ε��������� ������Ʈ�ε����� �������̶� ���ڷ� �޾Ƽ� �����Ѵ�. 0�̸� ������.
	void UseShadeProcessBar( int nLightElementIndex, int nDarkElementIndex );
	// ������ ���� �� ǥ���ϰ� ������, ������Ʈ �ֱ�. 0�̸� ������.
	void UseGaugeEndElement( int nGaugeEndElementIndex );

	void SetReverse( bool bR ){
		m_bReverse = bR;
	}
	bool IsReverse(){ return m_bReverse; }

};
