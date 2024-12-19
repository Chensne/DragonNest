#pragma once

#include "LostDeviceProcess.h"

class CEtBaseSprite
{
public:
	CEtBaseSprite() {}
	virtual ~CEtBaseSprite() {}

public:
	virtual void Release() {}
	virtual void SetTransform( EtMatrix *pTransform) {}
	virtual void SetWorldViewLH( EtMatrix *pWorld, EtMatrix *pView){}
	virtual void Begin( DWORD Flags) {}
	virtual void Draw( LPDIRECT3DTEXTURE9 pTexture, CONST RECT *pSrcRect, CONST D3DXVECTOR3 *pCenter, CONST D3DXVECTOR3 *pPosition, D3DCOLOR Color ) {}
	virtual void Flush() {}
	virtual void End() {}
	virtual void OnLostDevice() {}
	virtual void OnResetDevice() {}
};

class CEtDXSprite : public CEtBaseSprite
{
public:
	CEtDXSprite() {
		m_pSprite = NULL;
		D3DXCreateSprite( ( LPDIRECT3DDEVICE9 )GetEtDevice()->GetDevicePtr(), &m_pSprite );
		ADD_D3D_RES( m_pSprite );	
	}
	virtual ~CEtDXSprite() {
		Release();
	}

private:
	ID3DXSprite *m_pSprite;	

public:
	virtual void Release() {
		SAFE_RELEASE( m_pSprite );		
	}
	virtual void SetTransform( EtMatrix *pTransform) {
		if(m_pSprite)m_pSprite->SetTransform( pTransform );
	}
	virtual void SetWorldViewLH( EtMatrix *pWorld, EtMatrix *pView){
		if(m_pSprite)m_pSprite->SetWorldViewLH( pWorld, pView );
	}
	virtual void Begin( DWORD Flags) {
		if(m_pSprite)m_pSprite->Begin( Flags );
	}
	virtual void Draw( LPDIRECT3DTEXTURE9 pTexture, CONST RECT *pSrcRect, CONST D3DXVECTOR3 *pCenter, CONST D3DXVECTOR3 *pPosition, D3DCOLOR Color) {
		if(m_pSprite)m_pSprite->Draw( pTexture, pSrcRect, pCenter, pPosition, Color );
	}
	virtual void Flush() {
		if(m_pSprite)m_pSprite->Flush();
	}
	virtual void End() {
		if(m_pSprite)m_pSprite->End();
	}
	virtual void OnLostDevice() {
		if(m_pSprite)m_pSprite->OnLostDevice();
	}
	virtual void OnResetDevice() {
		if(m_pSprite)m_pSprite->OnResetDevice();
	}
};

class CEtCustomSprite : public CEtBaseSprite
{
public:
	CEtCustomSprite();

private:
	EtMatrix m_WorldMat;
	EtMatrix m_ViewMat;
	EtMatrix m_ProjMat;

	int m_nVertexDecl;

	DWORD	m_dwFlag;

public:
	virtual void Begin( DWORD Flags);
	virtual void SetTransform( EtMatrix *pTransform);
	virtual void SetWorldViewLH( EtMatrix *pWorld, EtMatrix *pView);
	virtual void Draw( LPDIRECT3DTEXTURE9 pTexture, CONST RECT *pSrcRect, CONST D3DXVECTOR3 *pCenter, CONST D3DXVECTOR3 *pPosition, D3DCOLOR Color);	
};

class CEtSprite : public CSingleton< CEtSprite >, public CLostDeviceProcess
{
public:
	CEtSprite(void);
	virtual ~CEtSprite(void);

protected:
	CEtBaseSprite *m_pCustomSprite;
	CEtBaseSprite *m_pDXSprite;
	CEtBaseSprite *m_pSprite;
	bool m_bBegin;

public:
	void Initialize();

	bool IsBegin() { return m_bBegin; }
	void Begin( DWORD dwFlags );
	void End();
	void Flush();
	void SetTransform( EtMatrix &TransMat );
	void SetWorldViewMat( EtMatrix &WorldMat, EtMatrix &ViewMat );
	void Draw( LPDIRECT3DTEXTURE9 pTexture, CONST RECT *pSrcRect, CONST D3DXVECTOR3 *pCenter, CONST D3DXVECTOR3 *pPosition, D3DCOLOR Color);
	void DrawSprite( EtTexture *pTexture, int nTexWidth, int nTexHeight, SUICoord &UVCoord, DWORD dwColor, SUICoord &ScreenCoord, float fRotate, float fZValue = 0.0f, float fShear = 0.0f );
	void DrawSprite3D( EtTexture *pTexture, int nTexWidth, int nTexHeight, DWORD dwColor, EtVector2 &vSize, EtVector3 &vPosition, float fRotate, SUICoord *pCoord = NULL );
	void DrawRect( SUICoord &ScreenCoord, DWORD dwColor, float fZValue = 0.f);

	virtual void OnLostDevice();
	virtual void OnResetDevice();
};
