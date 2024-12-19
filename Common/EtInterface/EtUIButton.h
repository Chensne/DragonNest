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

	int m_nButtonID;	// Note : ��ư�� �ε����� ������ �����ϱ� ���� �� ���� ������ �����Ͽ� ����� �� �ִ�.
	//blondy �¶��ΰ��ӿ��� �޼����� ���� �������� �ҷ��� ��ư ���̺� �ð��� ���� �д� �̱���� ���� �߰�
	bool  IsDisable;             //�𼼺� �ƴ°�  
	bool  IsEnableBackUp;       //�����¸� ����  	
	float m_ElapsedDisableTime;	// ���̺� �ǰ� ������ �ð� 		
	float m_DisableTime; //�̰��� ������ ���� �Ҽ��־�� �Ѵ�
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

	// SetTextWithEllipsis�Լ��� ����ϱ� ���� ������
	virtual void SetText( const std::wstring &strText , bool bFit = false, float fScaleX = 1.0f, float fScaleY = 1.0f) override;

	// EtUIStatic�� �ִ°� �״�� �����ɴϴ�. �ؿ� ������ ���� �Ѿ�� �ڵ����� ... ���ֱ� ���ؼ�.
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
