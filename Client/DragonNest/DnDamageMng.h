#pragma once
#include "DnCountMng.h"
#include "DnDamageDlg.h"

class CDnDamageMng : public CDnCountMng<CDnDamageDlg>
{
public:
	CDnDamageMng(void);
	virtual ~CDnDamageMng(void);

protected:
	EtAniHandle m_hAni;
	EtAniHandle m_hDownAni;

public:
	EtAniHandle GetDamageAni() { return m_hAni; }

public:
	void SetDamage( EtVector3 vPos, int nDamage, bool bEnemy, bool bMine, bool bCritical, int hasElement, bool bStageLimit );
	void SetRecovery( EtVector3 vPos, int nHP, int nSP, bool bEnemy, bool bMine );
	void SetCritical( EtVector3 vPos );
	void SetResist( EtVector3 vPos );
	void SetCriticalRes( EtVector3 vPos );
#ifdef PRE_ADD_DECREASE_EFFECT
	void SetDecreaseEffect( EtVector3 vPos );
#endif // PRE_ADD_DECREASE_EFFECT
};
