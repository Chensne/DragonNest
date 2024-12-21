#pragma once

class CEtEngine;
extern CEtEngine g_EtEngine;
inline CEtEngine *GetEtEngine() { return &g_EtEngine; }
extern CSyncLock *g_pEtRenderLock;
extern int g_nCurFrustumMask;

#define PRE_MOD_PRELOAD_SHADER

class CHighResolutionCapture;
class CEtEngine : public CSingleton< CEtEngine > 
{
public:
	CEtEngine();
	virtual ~CEtEngine();

public:
	bool Initialize( HWND hWnd, int nWidth, int nHeight, SGraphicOption &Option, 
		bool bWindow = false, bool bEnableMultiThread = false, bool bVSync = true, bool bShaderDebug = false );
	void Reinitialize( int nWidth, int nHeight );
	void Finalize();
	void RenderFrame( float fElapsedTime = 0.0166666f );
	void ShowFrame( RECT *pTargetRect = NULL );

	void CaptureScreen( const char *szFileName, int nWidth = INT_MAX, int nHeight = INT_MAX );
	void SetCaptureScreenClearColor( DWORD dwColor );

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	void FlushBuffer();
#endif
#ifdef PRE_MOD_PRELOAD_SHADER
public:
	bool PreLoadShader();
	void ReleasePreLoadShader();
private:
	std::vector< EtMaterialHandle > m_hPreLoadMaterials;
#endif

private:
	CHighResolutionCapture *m_pHRC;
};
