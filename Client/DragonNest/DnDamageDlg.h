#pragma once
#include "DnCountDlg.h"

class CDnDamageDlg : public CDnCountDlg
{
public:
	enum COLOR_TYPE {
		CT_NONE = -1,
		CT_PARTYATK = 0,
		CT_MYATK,
		CT_MYCRITICAL,
		CT_MYDAMAGE,
		CT_MYMANA,
		CT_MYHEAL,
		CT_PARTYHEAL,
		CT_PARTYDAMAGE,
		CT_BLUE1,
		CT_RED1,
		CT_FIRE,		//속성 공격 데미지 표시 추가
		CT_WATER,
		CT_LIGHT,
		CT_DARK,
		CT_STAGELIMIT,
	};
public:
	CDnDamageDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_BOTTOM, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDamageDlg(void);

protected:
	float m_fXGap;
	float m_fYGap;
	float m_fIncrese;
	float m_fVelocity;
	float m_fValue;
	float m_fTotalElapsedTime;

	EtVector3 m_vActorPos;
	EtVector3 m_vDamagePos;
	EtVector3 m_vAniPos;

	CDnDamageCount::eDamageType m_DamageType;

	EtAniHandle m_hAni;

	float	m_fDistanceFromCamera;

	COLOR_TYPE	m_ColorType;	

protected:
	virtual void UpdateCount();

	void UpdateDamagePos();
	void SetPosition( EtVector3 vPos );
	void HideDamage();

public:
	void SetDamage( EtVector3 vPos, int nDamage, EtAniHandle hAni, float fFontSize );
	void SetPlus();
	void SetChangeFont();
	void SetCritical( EtVector3 vPos );
	void SetResist( EtVector3 vPos );
	void SetCriticalRes( EtVector3 vPos );
	void SetColorType( COLOR_TYPE Type ) {m_ColorType = Type;}
	void SetDefaultShakeInfo();
#ifdef PRE_ADD_DECREASE_EFFECT
	void SetDecreaseEffect( EtVector3 vPos );
#endif // PRE_ADD_DECREASE_EFFECT

	// from CEtUIDialog
	virtual void Show( bool bShow );

	void SetHasElement(int hasElement);

	bool m_isMyDamage;
	void SetIsMyDamage(bool isMyDamage) { m_isMyDamage = isMyDamage; }

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
};
