#pragma once

#include "Timer.h"
#include "MAActorRenderBase.h"

class MASingleBody : virtual public MAActorRenderBase {
public:
	MASingleBody();
	virtual ~MASingleBody();

protected:
	std::string m_szSkinFileName;
	std::vector<int> m_vecFaceSubMeshIndex;

	std::string m_szCurrentEmissiveTexture;
	EtTextureHandle m_hCurrentEmissiveTexture;

protected:
	virtual bool ChangeSocialTexture( int nSocialIndex, int nFrameIndex ) override;
	bool ChangeEmissiveTexture( std::string &szTextureName );
	bool GetEmissiveConstants( float &fEmissivePower, float &fEmissivePowerRange, float &fEmissiveAniSpeed );
	bool SetEmissiveConstants( float fEmissivePower, float fEmissivePowerRange, float fEmissiveAniSpeed );

public:
	virtual bool LoadSkin( const char *szSkinName, const char *szAniName, bool bTwoPassAlpha = true );	
	virtual void FreeSkin();
};

