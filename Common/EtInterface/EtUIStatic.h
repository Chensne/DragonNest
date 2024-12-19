#pragma once

#include "EtUIControl.h"

#define FIX_STATICCONTROL_FOCUS_PROCESS
#define PRE_UISTATIC_PARTTEXT_COLOR
#define PRE_MOD_UISTATIC_PARTTEXT_COLOR

class CEtUIStatic : public CEtUIControl
{
public:
	CEtUIStatic( CEtUIDialog *pParent );
	virtual ~CEtUIStatic(void);

protected:
	SUICoord m_BaseCoord[9];
	SUICoord m_BaseUVCoord[9];

	bool	m_bBlink;
	float	m_fBlinkTime;
	int		m_nBlinkAlpha;
	bool	m_bManualControlColor;
	float	m_fWidthScale;
	float	m_fHeightScale;
	SUIColor m_BgTextColor;
	float	m_fRotate;
	bool	m_bForceBlend;
#ifdef FIX_STATICCONTROL_FOCUS_PROCESS
#else
	bool  m_bCanHaveFocus;
#endif
	bool m_bButton;

	SUICoord m_FontCoord;
#ifdef PRE_UISTATIC_PARTTEXT_COLOR
	struct SPartTextInfo
	{
		std::wstring text;
		DWORD dwTextColor;
		DWORD dwBGColor;
		float fWidth;
		float fPartWidthOffset;
		DWORD dwFontFormat;
	};
	std::list<SPartTextInfo> m_PartTextColorInfoList;
#endif

public:
	// 몇몇 상태에서는 알파값의 변경이 필요없을때가 있다.(EnemyGaugeDlg) 이땐 true.
	void	SetShadowColor( DWORD dwColor, bool bSkipAlpha = false );
	void	SetTextColor( DWORD dwColor, bool bSkipAlpha = false );
	void	SetBgTextColor( DWORD dwColor, UI_CONTROL_STATE State = UI_STATE_NORMAL );

	void	SetTextureColor( DWORD dwColor );
	void	SetManualControlColor(bool bManual)		{ m_bManualControlColor = bManual; }
	void	SetTextureScale( float fScale )			{ m_fWidthScale = m_fHeightScale = fScale; }
	void	SetTextureWidthScale( float fScale )	{ m_fWidthScale = fScale; }
	void	SetTextureHeithScale( float fScale )	{ m_fHeightScale = fScale; }

	// Static컨트롤의 경우 툴에서 지정된 색상값을 변화시키고 싶을때가 필요할거다.
	void	SetTextureColor( DWORD dwColor, int nElementIndex, UI_CONTROL_STATE emState = UI_STATE_NORMAL );

	void SetRotate( float fRotate ) {m_fRotate = fRotate;}

	DWORD	GetTextColor();
	DWORD	GetShadowColor();
	DWORD	GetTextureColor();
	DWORD	GetCurTextureColor();
	int		GetBlinkAlpha()					{ return m_nBlinkAlpha; }

	void SetBlendRate( float fRate )	{ m_fBlendRate = fRate; }
	void ForceBlend() { m_bForceBlend = true; }
	// Note : Blink가 true가 되면 스태틱이 깜빡입니다.
	void SetBlink( bool bBlink );
	bool IsBlink() { return m_bBlink; }

	// Note : 스태틱의 공간이 부족할때 말줄임을 넣어준다. ex) "무궁화 꽃이 피..."
	bool SetTextWithEllipsis( const wchar_t *wszText, const wchar_t *wszSymbol );

#ifdef PRE_UISTATIC_PARTTEXT_COLOR
	virtual void ClearText();
#else
	virtual void ClearText() { m_szText.clear(); m_strRenderText.clear(); if( m_Property.StaticProperty.bAutoTooltip ) ClearTooltipText(); }
#endif

	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );

#ifdef FIX_STATICCONTROL_FOCUS_PROCESS
#else
	void SetCanHaveFocus( bool bFocus ) {m_bCanHaveFocus = bFocus;}
#endif
	SUICoord *GetFontCoord() { return &m_FontCoord; }

	const wchar_t* GetPropertyString();

	void SetButton( bool bButton ) { m_bButton = bButton;}

#ifdef PRE_UISTATIC_PARTTEXT_COLOR
	void SetPartColorText(const std::wstring& colorText, DWORD dwTextColor, DWORD dwBGColor);
	void ClearPartColorText();
#endif

public:	
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual void UpdateRects();
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );
	virtual void SetText( const std::wstring &strText , bool bFit = false, float fScaleX = 1.0f, float fScaleY = 1.0f) override;
#ifdef FIX_STATICCONTROL_FOCUS_PROCESS
#else
	virtual bool CanHaveFocus() { return m_bCanHaveFocus; }
#endif

private:
#ifdef PRE_UISTATIC_PARTTEXT_COLOR
	void MakePartColorText(std::list<SPartTextInfo>::iterator& iterToInsert, const std::wstring& orgText, const std::wstring colorText, SUIElement* pElement, const SPartTextInfo& partTextInfo);
#endif
};
