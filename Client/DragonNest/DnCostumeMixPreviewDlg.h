#pragma once
#include "DnCustomDlg.h"
#include "DnRenderAvatar.h"
#include "DnPlayerActor.h"
#include "DnCashShopDefine.h"

class CDnCostumeMixPreviewDlg : public CDnCustomDlg
{
public:
	enum PreviewType
	{
		eCOSMIX,
		eCOS_DMIX,
#ifdef PRE_ADD_COSRANDMIX
		eCOS_RANDOMMIX,
#endif
	};

	CDnCostumeMixPreviewDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCostumeMixPreviewDlg(void);

	virtual void		Initialize(bool bShow);
	virtual void		Show(bool bShow);

	void				ClearParts(int classId);
	int					GetCurrentClassId() const;
	void				MakeAvatarToRender(int classId);
	void				AttachParts(ITEMCLSID itemId);
	bool				IsInside(float fMouseX, float fMouseY) const;
	void				SetPreviewType(PreviewType type) { m_Type = type; }

protected:
	virtual void	InitialUpdate();
	virtual void	Render( float fElapsedTime );
	virtual void	Process( float fElapsedTime );
	virtual void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	CEtUIButton*	m_pReduceBtn;
	CEtUIButton*	m_pEnlargeBtn;
	CEtUIButton*	m_pRotateLeftBtn;
	CEtUIButton*	m_pRotateRightBtn;
	CEtUIButton*	m_pRotateFrontBtn;
	CEtUIButton*	m_pRotateBackBtn;

	CEtUIButton*	m_pDrawFrameBtn;

	CDnRenderAvatarNew		m_RenderAvatar;

	float					m_fMouseX;
	float					m_fMouseY;
	PreviewType				m_Type;
};