#pragma once

#ifdef _CASHSHOP_UI

class CEtUIDialog;
class CDnPlayerActor;
class CDnActorPreview
{
public:
	CDnActorPreview();
	virtual ~CDnActorPreview();

	void Initialize(CEtUIDialog* pParentDlg, const SUICoord& charViewCoord, float screenWidth, float screenHeight);
	void SetActor(DnActorHandle handle);

	void BakeInspectView(CDnPlayerActor* pPlayerActor, float fElapsedTime);
	void RefreshItemSlots();

	void Show(bool bShow);

protected:
	void Render(float fElapsedTime);

private:
	void CloneActor(DnActorHandle hOriginal);

	int				m_nSessionID;

	DnActorHandle	m_hActor;

	CEtUIDialog*	m_pParent;
	SUICoord		m_CharViewCoord;

	EtTextureHandle m_hRTTexture;
	EtDepthHandle	m_hRTDepth;
	EtSurface*		m_pSurface;

	float			m_ScreenWidth;
	float			m_ScreenHeight;

	bool			m_bShow;
	float			m_fAniFrame;
	int				m_nAngle;
	float			m_fMouseX;
	float			m_fMouseY;
};

#endif // _CASHSHOP_UI