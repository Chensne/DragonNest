#pragma once

#include "EtUIControl.h"

// 동영상 재생을 CustomControl로 구현하면서 이 클래스는 사용하지 않게 되었다.
class CEtUIMovieControl : public CEtUIControl
{
public:
	CEtUIMovieControl( CEtUIDialog *pParent );
	virtual ~CEtUIMovieControl(void);

protected:

	// 동영상 재생할때 텍스처 얻어와 할테니 텍스처 핸들은 그냥 놔둘게요.
	EtTextureHandle m_hTexture;

	SUICoord m_TextureCoord;
	SUICoord m_TextureUV;
	SUICoord m_ControlCoord;

public:
	//EtTextureHandle GetTextrue() { return m_hTexture; }
	//void SetTexture( EtTextureHandle hTextureHandle, int nX, int nY, int nWidth, int nHeight );
	//void DeleteTexture();

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual void UpdateRects();
	virtual void Process( float fElapsedTime );
};