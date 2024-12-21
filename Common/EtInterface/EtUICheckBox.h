#pragma once
#include "EtUIButton.h"

class CEtUICheckBox : public CEtUIButton
{
public:
	CEtUICheckBox( CEtUIDialog *pParent );
	virtual ~CEtUICheckBox(void);

protected:
	bool m_bChecked;
	SUICoord m_ButtonCoord;
	SUICoord m_TextCoord;

protected:
	void SetCheckedInternal( bool bChecked, bool bFromInput, bool bProcessCommand = true );

public:
	bool IsChecked() const { return m_bChecked; }
	void SetChecked( bool bChecked, bool bProcessCommand = true ) { SetCheckedInternal( bChecked, false, bProcessCommand ); }

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey();
	virtual void UpdateRects(); 
	virtual void Render( float fElapsedTime );
	virtual void Process( float fElapsedTime );

	virtual void SetButtonRects( int nAllign );
};
