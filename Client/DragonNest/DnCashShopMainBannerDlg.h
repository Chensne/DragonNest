#pragma once

#ifdef PRE_ADD_CASHSHOP_RENEWAL

#include "EtUIDialog.h"

class CDnCashShopMainBanner : public CEtUIDialog
{

private:

	CEtUITextureControl * m_pTextureCtr;
	
	std::vector<EtTextureHandle> m_vTexture;

	float m_time;
	int m_crrIdx;



public:
	CDnCashShopMainBanner(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopMainBanner(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float dTime );
	virtual void Show( bool bShow );	

	// 해상도변경에 따른 위치조정.
	virtual void OnChangeResolution();

private:


public:
	


};

#endif // PRE_ADD_CASHSHOP_RENEWAL