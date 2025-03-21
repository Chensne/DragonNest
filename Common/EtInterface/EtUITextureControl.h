#pragma once

#include "EtUIControl.h"

// 원래 처음 만들어질때 의도 자체가 프로그램에서 직접 텍스처 핸들을 넘겨줘서(SetTexture멤버함수 사용)
// 내부적으로는 텍스처를 직접 지우거나 하는 것 없이 받은대로 사용하는 것이었다.
// 그래서 툴에서 직접 텍스처를 설정해 사용하는 것 없이 그냥 틀만 만들어두고 사용하는 형태였다.
//
// 이 결과 Static텍스처들이 너무 많아져서 텍스처 프로퍼티에 파일명 저장 후 사용하려고보니,
// 이 경우엔 텍스처핸들을 직접 관리해야하는 것이었다.
//
// 위의 경우와 구분을 하기 위해 bool변수 하나를 만들어두고 사용하기로 하겠다.
class CEtUITextureControl : public CEtUIControl
{
public:
	CEtUITextureControl( CEtUIDialog *pParent );
	virtual ~CEtUITextureControl(void);

protected:
	EtTextureHandle m_hTexture;

	/*SUICoord	m_TextureCoord;*/
	SUICoord	m_TextureUV;
	SUICoord	m_ControlCoord;
	
	SUICoord	m_NewUICoord;

	float		m_Scale;
	bool		m_bManualControlColor;

	float		m_fAddX;
	float		m_fAddY;

	// Initialize로 들어온 Property에 TextureFile명이 존재해서 생성된 경우다.
	// 이땐 새로운 텍스처 설정이 올때 기존의 텍스처를 지우는 코드를 수행한다.
	bool m_bCreatedFromPropertyTextureFile;

public:
	EtTextureHandle GetTextrue()	{ return m_hTexture; }
	void	SetTexture( EtTextureHandle hTextureHandle );
	void	SetTexture( EtTextureHandle hTextureHandle, int nX, int nY, int nWidth, int nHeight );
	void	SetFlipHTexture( EtTextureHandle hTextureHandle, int nX, int nY, int nWidth, int nHeight, bool bFlipH = false);
	void	SetScale(float scale)		{ m_Scale = scale; }
	void	SetTexturePosition(float fX, float fY);
	void	SetManualControlColor(bool bManual)		{ m_bManualControlColor = bManual; }
	void	DeleteTexture();
	DWORD	GetTextureColor();
	float	GetScale() const						{ return m_Scale; }
	void	SetTextureColor(DWORD dwColor);
	void GetUVCoord( SUICoord &Coord ) {Coord = m_TextureUV;}

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual void UpdateRects();
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );

	virtual void GetUICoord( SUICoord &Coord ) { Coord = m_NewUICoord; }
};