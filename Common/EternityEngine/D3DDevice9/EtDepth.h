#pragma once

class CEtDepth;
typedef CSmartPtr< CEtDepth >	EtDepthHandle;

class CEtDepth : public CEtResource, public CLostDeviceProcess
{
public:
	CEtDepth();
	virtual ~CEtDepth();

protected:
	EtSurface *m_pSurface;
	EtFormat m_Format;
	int m_nWidth;
	int m_nHeight;
	EtMultiSampleType m_MultiSampleType;

public:
	int LoadResource( CStream *pStream ) { return 1; }

	void CreateDepthBuffer( int nWidth, int nHeight, EtFormat Format = FMT_D24S8, EtMultiSampleType MultiSampleType = MULTISAMPLE_NONE );
	void SetSize( int nWidth, int nHeight ) {m_nWidth = nWidth; m_nHeight = nHeight;}
	EtSurface *GetDepthBuffer() { return m_pSurface; }

	virtual void OnLostDevice();
	virtual void OnResetDevice();

	static EtDepthHandle CreateDepthStencil( int nWidth, int nHeight, EtFormat Format = FMT_D24S8, EtMultiSampleType MultiSampleType = MULTISAMPLE_NONE );
};

