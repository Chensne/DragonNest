#pragma once

class CDnSkillUpButton : public CEtUIButton
{
protected:
	bool m_bBlink;
	float m_fBlinkTime;
	UI_CONTROL_STATE m_ctlState;
	int m_nTimeBack;
	//int m_iTooltipUIStringBackup;

public:
	CDnSkillUpButton( CEtUIDialog *pParent );
	virtual ~CDnSkillUpButton(void);

public:
	void Initialize( SUIControlProperty *pProperty );
	void SetBlink( bool bBlink );

public:
	virtual void Render( float fElapsedTime );
};
