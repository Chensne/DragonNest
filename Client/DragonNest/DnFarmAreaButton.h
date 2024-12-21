#pragma once

class CDnFarmAreaButton : public CEtUIButton
{
public:
	CDnFarmAreaButton( CEtUIDialog *pParent );
	virtual ~CDnFarmAreaButton(void);

protected:
	bool m_bPushPin;
	EtVector3 m_vWorldPos;

public:
	bool IsPushPin() { return m_bPushPin; }
	void SetPushPin( bool bPin ) { m_bPushPin = bPin; }

	void SetWorldPos( EtVector3 &vPos ) { m_vWorldPos = vPos; }
	EtVector3 *GetWorldPos() { return &m_vWorldPos; }
public:
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void Render( float fElapsedTime );
};
