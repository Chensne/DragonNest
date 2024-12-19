#pragma once
#include "EtUIDialog.h"
#include "DnActor.h"

class CDnEnemyGaugeDlg : public CEtUIDialog
{
public:
	CDnEnemyGaugeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnEnemyGaugeDlg(void);

protected:
	CEtUIProgressBar *m_pHPBar;
	CEtUIProgressBar *m_pSuperArmorBar;

	CEtUIStatic *m_pName[3];

	CEtUIStatic *m_pBalloon;
	CEtUIStatic *m_pNameIcon[2][2];

	DnActorHandle m_hActor;
//	DWORD m_dwDefaultColor;
	float m_fDelayTime;

	EtVector3 m_vDamageOld;

protected:
	void UpdateHP();
	//void UpdateSP();
	void UpdateGaugePos();
//	void SetName( int nLevel, LPCWSTR pwszName );

	void SetColorProgressBar();

public:
	void SetEnemy( DnActorHandle hActor );
	DnActorHandle GetEnemy() const { return m_hActor; }

	float GetDelayTime() { return m_fDelayTime; }
	void ResetDelayTime();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
};
