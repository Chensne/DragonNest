#pragma once
#include "etuicontrol.h"

class CEtUISlider : public CEtUIControl
{
public:
	CEtUISlider( CEtUIDialog *pParent );
	virtual ~CEtUISlider(void);

protected:
	SUICoord m_ButtonCoord;
	int m_nValue;
	float m_fButtonX;
	float m_fDragX;
	float m_fDragOffset;
	bool m_bPressed;

public:
	virtual void Initialize( SUIControlProperty *pProperty );

	void SetValue( int nValue ) { SetValueInternal( nValue, false ); }
	int  GetValue() const { return m_nValue; };

	virtual bool IsInside( float fX, float fY );
	virtual bool CanHaveFocus() { return ( IsShow() && IsEnable() ); }
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );

	virtual void UpdateRects(); 
	virtual void Render( float fElapsedTime );

protected:
	void SetValueInternal( int nValue, bool bFromInput );
	int  ValueFromPos( float fX ); 
};
