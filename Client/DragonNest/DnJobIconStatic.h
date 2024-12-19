#pragma once

class CDnJobIconStatic : public CEtUIStatic
{
public:
	CDnJobIconStatic( CEtUIDialog *pParent );
	virtual ~CDnJobIconStatic(void);

protected:
	bool m_bUseSmallSizeIcon;
	int m_nJobIconID;

	EtTextureHandle m_hNormalSizeIcon;
	EtTextureHandle m_hSmallSizeIcon;
	SUICoord m_IconUV;

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );

	void SetIconID( int nJobID, bool bUseSmallSizeIcon = false );
};