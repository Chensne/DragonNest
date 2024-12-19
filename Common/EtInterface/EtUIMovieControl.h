#pragma once

#include "EtUIControl.h"

// ������ ����� CustomControl�� �����ϸ鼭 �� Ŭ������ ������� �ʰ� �Ǿ���.
class CEtUIMovieControl : public CEtUIControl
{
public:
	CEtUIMovieControl( CEtUIDialog *pParent );
	virtual ~CEtUIMovieControl(void);

protected:

	// ������ ����Ҷ� �ؽ�ó ���� ���״� �ؽ�ó �ڵ��� �׳� ���ѰԿ�.
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