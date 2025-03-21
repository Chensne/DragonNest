#pragma once
#include "EtUIDialog.h"
#include "DnActor.h"

/********************************************************************
	created:	2010/11/24
	author:		semozz
	
	purpose:	StateBlow의 내구도 게이지 표시를 위한 다이얼로그(EnemyGagueDlg 구조 그대로 사용)
*********************************************************************/
class CDnDurabilityDlg : public CEtUIDialog
{
public:
	CDnDurabilityDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDurabilityDlg(void);

protected:
	CEtUIProgressBar *m_pDurabilityBar;
	CEtUIStatic* m_pIcon;

	float m_fDelayTime;
	float m_fRate;

	DnActorHandle m_hActor;
	int m_nStateBlowID;

protected:
	void UpdateDurability();
	void UpdateGaugePos();

	EtVector3 GetGaugePos();
public:
	void SetDurability(float fDurability);
	void SetActorInfo( DnActorHandle hActor, int nStateBlowID );
	
	DnActorHandle GetActor() { return m_hActor; }
	
	bool IsSameInfo( DnActorHandle hActor, int nStateBlowID )
	{
		return (hActor == m_hActor && nStateBlowID == m_nStateBlowID);
	}

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
};
