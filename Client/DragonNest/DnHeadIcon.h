#pragma once

class CDnPlayerActor;
class CDnHeadIcon
{
public:
	CDnHeadIcon();
	virtual ~CDnHeadIcon();

	enum
	{
		HEADICON_RENDERTIME = 5000,
	};


protected:
	EtTextureHandle m_hTex;
	DWORD m_dwHeadIconStartTime;
	DWORD m_dwRenderTime;
	bool m_bReleaseTexture;

public:
	static DWORD GetHeadIconAlpha( DWORD dwTime );
	DWORD GetHeadIconCustomAlpha( DWORD dwTime );

	void RenderHeadIcon( EtVector3 vHeadIconPos, float fElapsedTime );
	void SetHeadIcon( EtTextureHandle hTexture, DWORD dwStartTime, DWORD dwRenderTime, bool bReleaseTexture = false );

	// ChatBalloon�� ���θ� �𸣴� �ܺο��� Ȯ�� �� �۵�������Ų��.
	bool IsRenderHeadIcon();
	void StopRenderHeadIcon();
};