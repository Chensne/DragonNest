#pragma once

#include "EtUIType.h"
#include "EtUITemplate.h"


class CEtUIDialog;
class CEtUITemplate;

class CEtUIControl
{
public:
	CEtUIControl( CEtUIDialog *pParent );
	virtual ~CEtUIControl(void);

	struct sTooltipInfo 
	{
		bool bRefresh;
		bool bBottomPosition;
		float fWidthSize;
		std::wstring strTooltipText;
	
		sTooltipInfo()
		{
			bRefresh = false;
			bBottomPosition = false;
			fWidthSize = 0.f;
			strTooltipText.clear();
		}
	};

protected:
	bool m_bEnable;
	bool m_bShow;
	bool m_bFocus;
	bool m_bMouseEnter;
	bool m_bPressed;
	bool m_bRightPressed;
	bool m_bKeyLock;

	std::wstring m_szText;
	std::wstring m_strRenderText;
	float m_fBlendRate;
	UI_CONTROL_STATE m_CurrentState;

	SUIControlProperty m_Property;

	CEtUITemplate m_Template;
	CEtUIDialog *m_pParent;
	bool m_bExistTemplateTexture;

	// Note : UIString.xml이 아닌 코드상에서 실시간으로 툴팁 내용을 보여주기 위한 변수
	//		예를 들면 존맵에 NPC 버튼의 NPC이름에 사용
	sTooltipInfo m_sTooltipInfo;
		
	float m_fZValue;

	static bool s_bFocusEditBox;
	SUICoord	m_BaseDlgCoord;

protected:
	void UpdateBlendRate();

public:
	CEtUIDialog *GetParent()				{ return m_pParent; }
	void SetParent( CEtUIDialog *pParent )	{ m_pParent = pParent; }

	virtual int GetTextToInt();
	virtual void SetIntToText( int nValue );
	virtual INT64 GetTextToInt64();
	virtual void SetInt64ToText( INT64 nValue );

	void SetZValue( float ZValue ) {m_fZValue = ZValue;}
		
	virtual void SetText( const std::wstring &strText , bool bFit = false, float fScaleX = 1.0f, float fScaleY = 1.0f );
	virtual LPCWSTR GetText() { return m_szText.c_str(); }
	const std::wstring& GetText() const { return m_szText; }
	virtual void ClearText() { m_szText.clear(); m_strRenderText.clear(); }
	const std::wstring& GetRenderText() const { return m_strRenderText; }

	void SetToolTipBottomPosition( bool bEnable ) { m_sTooltipInfo.bBottomPosition = bEnable; }
	void SetTooltipRefresh( bool bEnable ) { m_sTooltipInfo.bRefresh = bEnable; }
	void SetTooltipWidthLimit( float fWidth ) { m_sTooltipInfo.fWidthSize = fWidth; }

	sTooltipInfo GetTooltipInfo(){ return m_sTooltipInfo; }

	void SetTooltipText( const wchar_t *wszText ) { m_sTooltipInfo.strTooltipText = wszText; }
	std::wstring &GetTooltipText() { return m_sTooltipInfo.strTooltipText; }
	void ClearTooltipText() { m_sTooltipInfo.strTooltipText.clear(); }

	virtual void Enable( bool bEnable ) { m_bEnable = bEnable; }
	bool IsEnable() { return m_bEnable; }

	virtual void Show( bool bShow ) { m_bShow = bShow; }
	bool IsShow() { return m_bShow; }

	virtual void Focus( bool bFocus ) { m_bFocus = bFocus; }
	bool IsFocus() { return m_bFocus; }

	virtual void KeyLock(bool bKey){ m_bKeyLock = bKey;}
	bool IsKeyLock(){return m_bKeyLock;}

	virtual void MouseEnter( bool bEnter ) { m_bMouseEnter = bEnter; }
	bool IsMouseEnter() { return m_bMouseEnter; }

	virtual void SetPressed( bool bPressed ) { m_bPressed = bPressed; if( !m_bPressed ) OnReleasePressed(); }	// 강제로 Pressed값을 변경할때만 사용된다.
	bool IsPressed() {return m_bPressed;}

	virtual void SetRightPressed( bool bPressed ) { m_bRightPressed = bPressed; } // 강제로 Pressed값을 변경할때만 사용된다.
	bool IsRightPressed() {return m_bRightPressed;}

	virtual bool IsHaveScrollBarControl() { return false; }
	virtual void OnReleasePressed() {}

	void DefaultControl( bool bDefault ) { m_Property.bDefaultControl = bDefault; }
	bool IsDefaultControl() { return m_Property.bDefaultControl ? true : false; }

	void SetUICoord( SUICoord &Coord ) { m_Property.UICoord = Coord; UpdateRects(); }
	virtual void GetUICoord( SUICoord &Coord ) { Coord = m_Property.UICoord; }
	SUICoord& GetUICoord() { return m_Property.UICoord; }
	void SetPosition( float fX, float fY ) { m_Property.UICoord.fX = fX; m_Property.UICoord.fY = fY; UpdateRects(); }
	void SetSize( float fWidth, float fHeight ) { m_Property.UICoord.fWidth = fWidth; m_Property.UICoord.fHeight = fHeight; UpdateRects();}

	const SUICoord& GetBaseUICoord() const { return m_BaseDlgCoord; }

	int GetTemplateIndex() { return m_Property.nTemplateIndex; }
	void SetTemplateIndex( int nTemplateIndex ) { m_Property.nTemplateIndex = nTemplateIndex; }

	virtual int GetScrollBarTemplate() { return -1; }
	virtual void SetScrollBarTemplate( int nIndex ) {}

	int GetID() { return m_Property.nID; }
	void SetID( int nID ) { m_Property.nID = nID; }

	void SetType( UI_CONTROL_TYPE Type ) { m_Property.UIType = Type; }
	UI_CONTROL_TYPE GetType() { return m_Property.UIType; }

	virtual void SetHotKey( int nKey ) { m_Property.nHotKey = nKey; }
	int GetHotKey() { return m_Property.nHotKey; }

	const char *GetControlName() const { return m_Property.szUIName; }

	float FloatXCoord( int nX );
	float FloatYCoord( int nY );
	int IntXCoord( float fX );
	int IntYCoord( float fY );

	void GetProperty( SUIControlProperty &Property ) { memcpy( &Property, &m_Property, sizeof( SUIControlProperty ) ); }
	SUIControlProperty *GetProperty() { return &m_Property; }

	CEtUITemplate &GetTemplate() { return m_Template; }
	SUIElement* GetElement( int nIndex );
	int GetElementCount() { return (int)m_Template.m_vecElement.size(); }

	void	SetTextColor( const DWORD TextColor, UI_CONTROL_STATE FocusType =  UI_STATE_NORMAL , int nElement = 0 );	// # 54411 - 서브 카테고리 폰트 색상 변경 - elkain03
	void	SetTextColor( const std::wstring StrColor, UI_CONTROL_STATE ControlStater = UI_STATE_NORMAL, int nElement = 0 );
	virtual void FindInputPos( std::vector<EtVector2> &vecPos );

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void SetTemplate( int nTemplateIndex );

	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam ) { return false; }
	virtual void OnHotkey() {}

	virtual void UpdateRects() {}
	virtual void UpdateUICoord( float fX, float fY, float fWidth, float fHeight );

	virtual void Render( float fElapsedTime );
	virtual void RenderDrag( float fElapsedTime ) {}

	virtual bool CanHaveFocus() { return false; }
	virtual bool IsInside( float fX, float fY );

	virtual void Process( float fElapsedTime ) {}

	virtual void OnChangeResolution();
	virtual void OnCmdDrag(UIDragCmdParam param) {}

	static bool IsFocusEditBox() { return s_bFocusEditBox; }
	static void SetFocusEditBox( bool bFocus ) { s_bFocusEditBox = bFocus; }

	void SetBlendRate( float fValue ) { m_fBlendRate = fValue; }
protected:
	bool m_bFirstRender;
};
