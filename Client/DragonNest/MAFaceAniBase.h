#pragma once

class MAFaceAniBase 
{
public:
	MAFaceAniBase();
	virtual ~MAFaceAniBase();

protected:
	struct FrameStruct {
		float fFrame;
		EtTextureHandle hTexture;
		EtTextureHandle hMaskTexture;
	};
	struct SocialStruct {
		std::string szName;
		float fTotalFrame;
		std::vector<FrameStruct *> pVecList;
	};

	std::vector<SocialStruct*> m_pVecSocialList;
	std::map<std::string, int> m_szMapSearch;

	LOCAL_TIME m_LocalTime;
	std::string m_szSocialAction;
	std::string m_szPrevSocialAction;
	int m_nCurrentSocialActionIndex;
	LOCAL_TIME m_AniFrame;

	float m_fFps;

	bool m_bUseMaskTexture;

protected:
	void LoadFaceAni( int nTableID, SocialStruct *pSocial );
	int GetSocialIndex( const char *szAniName );

	virtual bool ChangeSocialTexture( int nSocialIndex, int nFrameIndex ) { return false;}

public:
	virtual bool Initialize( int nClassID );
	void Process( LOCAL_TIME LocalTime, float fDelta );

	void SetFaceAction( const char *szAniName );

	bool IsExistFaceAni( const char *szAniName );
	void FreeFaceAni();
};