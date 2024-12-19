#pragma once

#include "EtUIControl.h"

// ���� ó�� ��������� �ǵ� ��ü�� ���α׷����� ���� �ؽ�ó �ڵ��� �Ѱ��༭(SetTexture����Լ� ���)
// ���������δ� �ؽ�ó�� ���� ����ų� �ϴ� �� ���� ������� ����ϴ� ���̾���.
// �׷��� ������ ���� �ؽ�ó�� ������ ����ϴ� �� ���� �׳� Ʋ�� �����ΰ� ����ϴ� ���¿���.
//
// �� ��� Static�ؽ�ó���� �ʹ� �������� �ؽ�ó ������Ƽ�� ���ϸ� ���� �� ����Ϸ�����,
// �� ��쿣 �ؽ�ó�ڵ��� ���� �����ؾ��ϴ� ���̾���.
//
// ���� ���� ������ �ϱ� ���� bool���� �ϳ��� �����ΰ� ����ϱ�� �ϰڴ�.
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

	// Initialize�� ���� Property�� TextureFile���� �����ؼ� ������ ����.
	// �̶� ���ο� �ؽ�ó ������ �ö� ������ �ؽ�ó�� ����� �ڵ带 �����Ѵ�.
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