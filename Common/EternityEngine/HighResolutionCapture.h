#pragma once

class CHighResolutionCapture
{
public:
	CHighResolutionCapture();
	virtual ~CHighResolutionCapture();

	void Capture( const char *szFilePrefix, int nWidth = INT_MAX, int nHeight = INT_MAX );

	void BeginCapture();
	void EndCapture();

	void SetClearColor( DWORD dwColor ) { m_dwClearColor = dwColor; }

protected:
	CEtColorAdjustTexFilter *m_pColorAdjFilter;
	DWORD m_dwClearColor;

	bool m_bCapture;
	std::string m_szFilePrefix;

	EtTextureHandle m_hRenderTarget;
	EtDepthHandle m_hDepthTarget;
	EtTextureHandle m_hTempTarget;

	float m_fTempAspectRatio;
};