#pragma once
#include "EtUIControl.h"
#include "DnCustomControlCommon.h"

class CDnCountCtl : public CEtUIControl
{
public:
	CDnCountCtl( CEtUIDialog *pParent );
	virtual ~CDnCountCtl(void);

protected:
	SUIColor m_Color;
	int m_nValue;
	std::vector<int> m_vecValueIndex;
	float m_fZValue;
	float m_fScale;
	float m_fFontSize;
	BYTE	m_cAlpha;
	bool	m_bUseCustomColor;
	bool	m_bPlus;
	bool	m_bMinus;
	bool    m_bChangeFont;
	bool m_bRightAlign;

public:
	void Init();
	void SetZValue( float fZValue ) {m_fZValue = fZValue;}
	void SetScale( float fScale ) {m_fScale = fScale;}
	void SetFontSize( float fSize ) {m_fFontSize = fSize;}
	void SetAlpha( BYTE cAlpha ) {m_cAlpha = cAlpha;}
	BYTE GetAlpha();

	void SetRightAlign( bool bValue ) { m_bRightAlign = bValue; }

	void UseCustomColor(bool bUseCustomColor ) { m_bUseCustomColor = bUseCustomColor;}
	void SetPlus(bool enable) {m_bPlus = enable;}
	void SetMinus(bool enable) {m_bMinus = enable;}
	void ChangeFont(bool enable) {m_bChangeFont = enable; }

public:
	virtual void SetValue( int nValue );
	virtual float GetControlWidth();
	virtual float GetControlHeight();

public:
	virtual void Render( float fElapsedTime );
};
