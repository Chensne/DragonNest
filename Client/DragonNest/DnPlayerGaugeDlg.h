#pragma once
#include "EtUIDialog.h"
#include "DnEnemyGaugeDlg.h"
#include "DnActor.h"

class CDnPlayerGaugeDlg : public CDnEnemyGaugeDlg
{
public:
	CDnPlayerGaugeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPlayerGaugeDlg(void);

protected:
	CEtUIProgressBar *m_pSPBar;		

protected:
	void UpdateHP();
	void UpdateSP();
	
	//void SetName( int nLevel, LPCWSTR pwszName, DWORD dwColor = -1 );

public:

	void UpdateGaugePos();
	void SetActor( DnActorHandle hActor ) ;

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	virtual void Process( float fElapsedTime );
};
