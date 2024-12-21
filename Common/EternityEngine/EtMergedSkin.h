#pragma once

#include "EtResource.h"
#include "EtMesh.h"
#include "EtSkin.h"

struct EmissiveParam {
	EtColor Color;
	float fPower;
	float fRange;
	float fAniSpeed;
	EmissiveParam() {
		Color = EtColor(1,0,1,1);
		fPower = 1.0f;
		fRange = 0.0f;
		fAniSpeed = 1.0f;
	}
};

class CEtMergedSkin : public CEtSkin
{
	friend class CEtSkinInstance;
	enum {		
		cMaxParts = 5,
	};	
public:
	CEtMergedSkin();
	virtual ~CEtMergedSkin(); 

protected:
	EmissiveParam m_EmissiveParam[ cMaxParts ];
	int		m_nTextureWidth;
	int		m_nTextureHeight;
	EtTextureHandle m_hCombineTexture;
	EtTextureHandle m_hCombineMaskTexture;
	
public:
	void SetTextureSize( int nWidth, int nHeight ) { m_nTextureWidth = nWidth; m_nTextureHeight = nHeight; }
	EtVector4 MergeTexture( EtTextureHandle hTexture, EtTextureHandle hMaskTexture, int nMergeOrder, RECT TextureRect );
	bool Assign( EtSkinHandle hSkin, RECT TextureRect );
#ifdef PRE_MOD_WARRIOR_RIGHTHAND
	void Merge( EtSkinHandle hSkin, RECT TextureRect, CEtSkinInstance *pSkinInstance = NULL );
#else
	void Merge( EtSkinHandle hSkin, RECT TextureRect );
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
	void BuildMipmap();

};

typedef CSmartPtr< CEtMergedSkin > EtMergedSkinHandle;
 