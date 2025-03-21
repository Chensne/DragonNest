#pragma once
#include "EtUIControl.h"
#include "EtUISound.h"

class CEtUIButton : public CEtUIControl
{
public:
	CEtUIButton( CEtUIDialog *pParent );
	virtual ~CEtUIButton(void);

protected:	
	int m_nToggleButtonIndex;
	//CEtUISound::emUI_SOUND m_emSoundIndex;
	int m_nSoundIndex;

	SUICoord m_MoveButtonCoord;
	SUICoord m_BaseCoordLeft;
	SUICoord m_BaseCoordMid;
	SUICoord m_BaseCoordRight;
	SUICoord m_MoveBaseCoordLeft;
	SUICoord m_MoveBaseCoordMid;
	SUICoord m_MoveBaseCoordRight;

	SUICoord m_uvCoordLeft;
	SUICoord m_uvCoordMid;
	SUICoord m_uvCoordRight;
	SUICoord m_MouseCoord;

	int m_nButtonID;	// Note : 버튼을 인덱스나 값으로 구분하기 위해 이 값을 툴에서 셋팅하여 사용할 수 있다.
	//blondy 온라인게임에서 메세지를 자주 못보내게 할려고 버튼 디세이블 시간을 많이 둔다 이기능을 위해 추가
	bool  IsDisable;             //디세블 됐는가  
	bool  IsEnableBackUp;       //전상태를 저장  	
	float m_ElapsedDisableTime;	// 디세이블 되고 지나간 시간 		
	float m_DisableTime; //이값을 툴에서 세팅 할수있어야 한다
	//blondy end	
	bool m_bRightButton;
	bool m_bEnableKeyboard;
	bool	m_bBlink;
	float	m_fBlinkTime;
	int		m_nBlinkAlpha;

	// PRE_MOD_CASHSHOP_GIFTBOTTON_TWINKLE_SPEEDUP
	int		m_nBlendTime;

public:
	void SetButtonID( int nButtonID ) { m_nButtonID = nButtonID; }
	int GetButtonID() { return m_nButtonID; }	
	SUICoord GetMouseCoord() {return m_MouseCoord;}
	void SetBlink( bool bBlink );
	bool IsBlink() { return m_bBlink; }

	// PRE_MOD_CASHSHOP_GIFTBOTTON_TWINKLE_SPEEDUP
	inline void	SetBlendTime( int BlendTime = 1000 ) { m_nBlendTime = BlendTime; }

public:
	virtual void MouseEnter( bool bEnter );

	// SetTextWithEllipsis함수를 사용하기 위해 재정의
	virtual void SetText( const std::wstring &strText , bool bFit = false, float fScaleX = 1.0f, float fScaleY = 1.0f) override;

	// EtUIStatic에 있는거 그대로 가져옵니다. 해외 번역시 길이 넘어가면 자동으로 ... 해주기 위해서.
	bool SetTextWithEllipsis( const wchar_t *wszText, const wchar_t *wszSymbol );

	virtual void ClearText() { m_szText.clear(); m_strRenderText.clear(); if( m_Property.ButtonProperty.bAutoTooltip ) ClearTooltipText(); }

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual bool CanHaveFocus() { return true; }
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void PlayButtonSound();
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey();
	virtual void Render( float fElapsedTime );
	virtual void UpdateRects();
	virtual void Process( float fElapsedTime );
	virtual void Enable( bool bEnable ) ;

	void UpdateBlink( float fElapsedTime );
	void SetDisableTime( float fDisableTime){m_DisableTime = fDisableTime;};
	void EnableKeyboard( bool bEnable ) { m_bEnableKeyboard = bEnable; }
};
