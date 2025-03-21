#pragma once

class CDnMenuButton : public CEtUIButton
{
public:
	CDnMenuButton( CEtUIDialog *pParent );
	virtual ~CDnMenuButton(void);

protected:
	bool m_bOpenDlg;
	bool m_bBlink;

	SUICoord m_ShortCutUV;
	SUICoord m_ShortCutCoord;
	EtTextureHandle m_hShortCutIcon;

public:
	void SetOpen( bool bOpen );
	void SetBlink() { m_bBlink = true; }
	void ResetBlink() { m_bBlink = false; }

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual void SetHotKey( int nKey );
};