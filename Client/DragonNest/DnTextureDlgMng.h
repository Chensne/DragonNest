#pragma once
#include "DnTextureDlg.h"
#include "DnCommDlgMng.h"

class CDnTextureDlgMng : public CDnCommDlgMng<CDnTextureDlg>
{
public:
	CDnTextureDlgMng(void);
	virtual ~CDnTextureDlgMng(void);

public:
	DWORD ShowTextureWindow( EtTextureHandle hTextureHandle, float fX, float fY, float fDelayTime, DWORD dwSetID = 0, CEtUICallback *pCall = NULL );
	DWORD ShowTextureWindow( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, float fDelayTime, DWORD dwSetID = 0, CEtUICallback *pCall = NULL );
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	DWORD ShowTextureWindow( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, float fDelayTime, DWORD dwSetID = 0, bool bAutoCloseDialog = true, CEtUICallback *pCall = NULL );
#endif
};