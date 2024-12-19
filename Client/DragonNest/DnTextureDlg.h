#pragma once
#include "DnCommDlg.h"

class CDnTextureDlg : public CDnCommDlg
{
public:
	CDnTextureDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnTextureDlg(void);

protected:
	CEtUITextureControl *m_pTextureControl;
	EtTextureHandle m_hTextureHandle;
	bool m_bDeleteTexture;

	float m_fX;
	float m_fY;
	int m_nPos;


protected:
	void UpdateTextureBox();

public:
	void SetTexture( EtTextureHandle hTextureHandle, float fX, float fY, float fDelayTime );
	void SetTexture( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, float fDelayTime );
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	void SetTexture( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, bool bAutoCloseDialog, float fDelayTime);
#endif
	void DeleteTexture();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void OnChangeResolution();
};
