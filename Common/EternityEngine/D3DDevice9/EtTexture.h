#pragma once

#include "EtResource.h"

class CEtTexture;
typedef CSmartPtr< CEtTexture >	EtTextureHandle;

#define PRE_MOD_IGNORE_MIPMAP

class CEtTexture : public CEtResource, public CLostDeviceProcess
{
public:
	CEtTexture();
	virtual ~CEtTexture();
	void Clear();

protected:
	EtBaseTexture *m_pTexture;
	EtSurface *m_pSurface;
	EtTextureType m_TextureType;
	EtFormat m_Format; 
	EtUsage m_Usage;
	EtPool m_Pool;
	int m_nLockLevel;

	int m_nWidth;
	int m_nHeight;
	int m_nOriginalWidth;
	int m_nOriginalHeight;
	int m_nFileSize;

	static EtBaseTexture *s_pEnvTexture;
	int	m_nAlphaTexture;
	// 텍스쳐를 2의 승수로 맞춰서 로딩할건지 말건지
	static bool s_bPow2;

	static EtTextureHandle m_hWhiteTexture;
	static EtTextureHandle m_hBlankTexture;

#ifdef PRE_MOD_IGNORE_MIPMAP
	static std::vector<std::string> s_vecIgnoreMipmapPath;
	static std::vector<std::string> s_vecMipmapPath;
#endif

protected:
	void CreateTexture( int nWidth, int nHeight, EtFormat Format, EtUsage Usage, EtPool Pool, int nLevel = 1 );
	void CreateRenderTarget( int nWidth, int nHeight, EtFormat Format, EtUsage Usage, EtPool Pool );

public:
	int LoadResource( CStream *pStream );

	void SetSize( int nWidth, int nHeight ) { m_nWidth = nWidth; m_nHeight = nHeight; }
	int Width();
	int Height();
	int OriginalWidth() { return m_nOriginalWidth; }
	int OriginalHeight() { return m_nOriginalHeight; }
	int GetFileSize() { return m_nFileSize; }
	void SetTextureType( EtTextureType Type ) { m_TextureType = Type; }
	EtTextureType GetTextureType() { return m_TextureType; }
	EtBaseTexture *GetTexturePtr() { return m_pTexture; }
	EtSurface *GetSurfaceLevel();

	void *Lock( int &nStride, bool bCanWrite = false );
	void Unlock();

	static EtTextureHandle CreateNormalTexture( int nWidth, int nHeight, EtFormat Format = FMT_X8R8G8B8, EtUsage Usage = USAGE_DEFAULT, EtPool Pool = POOL_DEFAULT, int nLevel = 1 );
	static EtTextureHandle CreateRenderTargetTexture( int nWidth, int nHeight, EtFormat Format = FMT_X8R8G8B8, EtUsage Usage = USAGE_DEFAULT, EtPool Pool = POOL_DEFAULT);

	bool ChangeFormat( EtFormat Format, EtUsage Usage = USAGE_DEFAULT, EtPool Pool = POOL_DEFAULT );
	bool ChangeFormatSize( EtFormat Format, EtUsage Usage, EtPool Pool  = POOL_DEFAULT, int nWidth = 0, int nHeight = 0 );
	void Copy( EtTextureHandle hSour, DWORD dwFilter = D3DX_DEFAULT, RECT *pDestRect = NULL, RECT *pSrcRect = NULL );
	EtTextureHandle CopyEx();
	void RemoveRenderTarget();
	void BuildMipmap();

	virtual void OnLostDevice();
	virtual void OnResetDevice();

	static void SetEnvTexture( EtBaseTexture *pEnvTexture ) { s_pEnvTexture = pEnvTexture; }
	static EtBaseTexture *GetEnvTexture() { return s_pEnvTexture; }

	static void SetPow2( bool bPow2 ) { s_bPow2 = bPow2; }
	static bool GetPow2() { return s_bPow2; }
	
	void CalcFittingArea( BYTE cAlphaThreshold, int *pX, int *pY, int *pWidth, int *pHeight );
	void FindAlphaBlock();
	bool IsAlphaTexture();

	static unsigned int CalcTextureSize(unsigned int nNonPow2Size );

	static EtTextureHandle GetWhiteTexture() { return m_hWhiteTexture; }
	static EtTextureHandle GetBlankTexture() { return m_hBlankTexture; }

	static void CreateDebugRes();
	static void ReleaseDebugRes();

#ifdef PRE_MOD_IGNORE_MIPMAP
	static void AddIgnoreMipmapPath( std::string strApplyPath ) { s_vecIgnoreMipmapPath.push_back( strApplyPath ); }
	static void AddMipmapPath( std::string strApplyPath ) { s_vecMipmapPath.push_back( strApplyPath ); }
#endif
};
