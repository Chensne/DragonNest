#pragma once
#include "DnSlotButton.h"

class CDnSkillSlotButton : public CDnSlotButton
{
public:
	CDnSkillSlotButton( CEtUIDialog *pParent );
	virtual ~CDnSkillSlotButton(void);

protected:
	SKILLSLOT_STATE m_ButtonState;

public:
	SKILLSLOT_STATE GetSlotState()						{ return m_ButtonState; }
	void SetSlotState( SKILLSLOT_STATE ButtonState )	{ m_ButtonState = ButtonState; }

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	//virtual void UpdateRects();
	virtual void Render( float fElapsedTime );
	virtual void RenderDrag( float fElapsedTime );
};
