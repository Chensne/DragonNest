#pragma once

class CDnColorButton : public CEtUIButton
{
public:
	CDnColorButton( CEtUIDialog *pParent );
	virtual ~CDnColorButton(void);

public:
	virtual void MouseEnter( bool bEnter );
	virtual void Render( float fElapsedTime );
};
