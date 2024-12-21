#pragma once


// ���̵� �� �ƿ� �����ϴ� Ŭ����
class CFade
{
private:
	enum
	{
		NONE,
		FADE_IN,
		FADE_OUT,
	};

	LPDIRECT3DDEVICE9			m_pd3dDevice;
	LPD3DXSPRITE				m_pSprite;
	LPDIRECT3DTEXTURE9			m_pBlackTexture;
	int							m_iFadeState;
	float						m_fNowFadeAmount;
	float						m_fFadeSpeed;
	DWORD						m_dwColor;
	

	CFade( void ) {};
public:
	CFade( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXSPRITE pSprite );
	virtual ~CFade(void);

	void Reset( void );
	void FadeIn( float fSpeed, DWORD dwColor );
	void FadeOut( float fSpeed, DWORD dwColor );

	void Render( float fDelta );
};
