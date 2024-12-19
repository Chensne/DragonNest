#pragma once

#ifdef PRE_ADD_CRAZYDUC_UI

#include "DnCountDlg.h"

class CDnScoreDlg : public CDnCountDlg
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

	CDnScoreDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_BOTTOM, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnScoreDlg(void);

protected:
	float m_fXGap;
	float m_fYGap;
	float m_fTotalElapsedTime;

	EtVector3 m_vActorPos;
	EtVector3 m_vScorePos;
	EtVector3 m_vAniPos;

	EtAniHandle m_hAni;

	float	m_fDistanceFromCamera;
	
	COLOR_TYPE	m_ColorType;	

protected:
	virtual void UpdateCount();

	void SetPosition( EtVector3 vPos );
	void UpdateScorePos();
	void HideScore();

public:
	void SetScore( EtVector3 vPos, int nScore, EtAniHandle hAni, float fFontSize );
	void SetSign( bool bPlus );		
	void SetColorType( COLOR_TYPE Type ) {m_ColorType = Type;}

	virtual void Show( bool bShow );
	
public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
};

#endif