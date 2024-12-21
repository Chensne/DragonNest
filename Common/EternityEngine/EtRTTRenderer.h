#pragma once

class CEtRTTRenderer;
typedef CSmartPtr< CEtRTTRenderer >	EtRTTHandle;

struct SRTTObject
{
	EtObjectHandle hHandle;
	BOOL bDelete;
};
class CEtRTTRenderer : public CSmartPtrBase< CEtRTTRenderer >
{
public:
	CEtRTTRenderer(void);
	virtual ~CEtRTTRenderer(void);

protected:
	int m_nWidth;
	int m_nHeight;
	EtFormat m_Format;
	EtCameraHandle m_hCamera;
	EtTextureHandle m_hRenderTarget;
	EtDepthHandle m_hDepthTarget;

	bool m_bRenderChild;
	int m_nRenderFrameCount;

	std::vector< SRTTObject > m_vecObject;
	std::vector< EtEffectObjectHandle > m_vecEffectObject;
	std::vector< EtBillboardEffectHandle > m_vecParticle;

public:
	virtual EtTextureHandle GetRTT() { return m_hRenderTarget; }

	void Initialize( SCameraInfo &CameraInfo, int nWidth, int nHeight, int nViewPortWidth = -1, int nViewPortHeight = -1, EtFormat Format = FMT_A8B8G8R8 );

	bool IsRenderChild() { return m_bRenderChild; }
	bool SetRenderChild( bool bRender )
	{
		bool bOld = m_bRenderChild;
		m_bRenderChild = bRender;
		return bOld;
	}

	void ClearRenderObject();
	EtObjectHandle CreateStaticObject( const char *pSkinName );
	EtAniObjectHandle CreateAniObject( const char *pSkinName, const char *pAniName );
	EtAniObjectHandle CreateAniObject( EtSkinHandle hSkin, const char *pAniName );
	void AddObject( EtObjectHandle hObject, bool bAutoDelete = false );
	EtBillboardEffectHandle CreateBillboardEffect( int nParticleDataIndex, EtMatrix *pWorldMat, bool bReuseParticleInstance );
	EtEffectObjectHandle CreateEffectObject( int nEffectDataIndex, EtMatrix *pWorldMat );

	void InitCamera( SCameraInfo &CameraInfo );
	void UpdateCamera( EtMatrix &WorldMat ) { m_hCamera->Update( &WorldMat ); }
	void SetRenderFrameCount( int nCount ) { m_nRenderFrameCount = nCount; }

	void InitRenderObject();
	virtual void BeginRTT();
	void Render( float fElapsedTime );
	virtual void EndRTT();
	virtual void RenderRTT( float fElapsedTime );

	static void RenderRTTList( float fElapsedTime );
};
