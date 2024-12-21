#pragma once
#include "EtUIControl.h"
#include "DnCustomControlCommon.h"

//#define	COUNT_YELLOW_RATIO	0.8f
//#define COUNT_RED_RATIO		1.0f

class CEtBVLEngine;
class CDnMovieControl : public CEtUIControl
{
public:
	CDnMovieControl( CEtUIDialog *pParent );
	virtual ~CDnMovieControl(void);

	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual void Show( bool bShow );

	bool Play( const char *szFileName, bool bRepeat = false );
	void Stop();

protected:
	EtTexture *m_pTexture;	// USE EtTextureHandle? by kalliste
	boost::shared_ptr<CEtBVLEngine> m_pMovieEngine;

	float m_fAlphaBorderHoriRate;
	float m_fAlphaBorderVertRate;
	EtTextureHandle m_hBorderAlphaTexture;
	void ProcessBorderAlpha();
	EtTexture *GetSafeTexture();
};
