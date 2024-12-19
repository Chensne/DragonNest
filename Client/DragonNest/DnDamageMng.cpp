#include "StdAfx.h"
#include "DnDamageMng.h"
#include "GameOption.h"
#include "DnState.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDamageMng::CDnDamageMng(void)
{
	m_hAni = EternityEngine::LoadAni( CEtResourceMng::GetInstance().GetFullName("Damage_Number.ani").c_str() );
	m_hDownAni = EternityEngine::LoadAni( CEtResourceMng::GetInstance().GetFullName("Damage_Number_2.ani").c_str() );
}

CDnDamageMng::~CDnDamageMng(void)
{
	SAFE_RELEASE_SPTR( m_hAni );
	SAFE_RELEASE_SPTR( m_hDownAni );
}

void CDnDamageMng::SetDamage( EtVector3 vPos, int nDamage, bool bHitOrDamge, bool bMeOrParty, bool bCritical, int hasElement, bool bStageLimit )
{
	if( bMeOrParty && !CGameOption::GetInstance().bMyCombatInfo ) {
		return;		
	}
	if( !bMeOrParty && !CGameOption::GetInstance().bPartyCombatInfo ) {
		return;		
	}

	CDnDamageDlg *pDlg = m_CountDlgMemPool.Allocate();		

	CDnDamageDlg::COLOR_TYPE arrColorType[2][2][2] = 
	{ 
		{{ CDnDamageDlg::CT_MYATK, CDnDamageDlg::CT_MYCRITICAL},		// bHitOrDamge == true, bMeOrParty == true -> 내가입힌 데미지
		 { CDnDamageDlg::CT_PARTYATK, CDnDamageDlg::CT_PARTYATK}},		// bHitOrDamge == true, bMeOrParty == false -> 파티원이 입힌 데미지
		{{ CDnDamageDlg::CT_MYDAMAGE, CDnDamageDlg::CT_MYDAMAGE},				// bHitOrDamge == false, bMeOrParty == true -> 내가 입은 데미지
		 { CDnDamageDlg::CT_PARTYDAMAGE, CDnDamageDlg::CT_PARTYDAMAGE}}			// bHitOrDamge == false, bMeOrParty == false -> 파티원이 입은 데미지
	};
	
	CDnDamageDlg::COLOR_TYPE colorType = arrColorType[ bHitOrDamge ? 0 : 1 ][ bMeOrParty ? 0 : 1 ][ bCritical ? 1 : 0 ];

	bool isHasElement = false;
	switch(hasElement)
	{
	case CDnState::Fire:
		colorType = CDnDamageDlg::CT_FIRE;
		isHasElement = true;
		break;
	case CDnState::Ice:
		colorType = CDnDamageDlg::CT_WATER;
		isHasElement = true;
		break;
	case CDnState::Light:
		colorType = CDnDamageDlg::CT_LIGHT;
		isHasElement = true;
		break;
	case CDnState::Dark:
		colorType = CDnDamageDlg::CT_DARK;
		isHasElement = true;
		break;
	default:
		colorType = arrColorType[ bHitOrDamge ? 0 : 1 ][ bMeOrParty ? 0 : 1 ][ bCritical ? 1 : 0 ];
		break;
	}

	//속성 공격이고 크리티컬일때 황금색 데미지 숫자 표시
	if (isHasElement && bCritical)
	{
		//내가 타격 한 데미지 일경우 황금색..
		if (bHitOrDamge)
			colorType = arrColorType[ bHitOrDamge ? 0 : 1 ][ bMeOrParty ? 0 : 1 ][ bCritical ? 1 : 0 ];
		//내가 피격 일때는 위에서 설정된 속성 색 그대로
	}

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	if( bStageLimit )
		colorType = CDnDamageDlg::CT_STAGELIMIT;
#endif
	
	pDlg->SetColorType(colorType);

	pDlg->Initialize( true );
	m_listCountDlg.push_back( pDlg );

	float nFontSize = bHitOrDamge ? ( bCritical ? 0.9f : 0.6f ) : (bCritical ? 1.2f : 0.8f);

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	if( bStageLimit )
	{
		pDlg->SetChangeFont();
		nFontSize = 1.f;
	}
#endif

	pDlg->SetDamage( vPos, nDamage, (!bHitOrDamge ? m_hDownAni : m_hAni), nFontSize );

	if (isHasElement)
		pDlg->SetHasElement(hasElement);

	pDlg->SetIsMyDamage(!bHitOrDamge);


	pDlg->ShowCount( true );
	pDlg->Show( true );
}

void CDnDamageMng::SetRecovery( EtVector3 vPos, int nHP, int nSP, bool bEnemy, bool bMine )
{
	if( bMine && !CGameOption::GetInstance().bMyCombatInfo ) {
		return;		
	}

	bool bParty = !bMine && !bEnemy;
	if( bParty ) 
	{
		if( CGameOption::GetInstance().bPartyCombatInfo == false
#ifdef PRE_ADD_VIEW_OPITION_PARTY_HEAL
			&& CGameOption::GetInstance().m_bShowPartyHealInfo == false 
#endif
			)
		{
			return;
		}
	}

	if( nSP == 0 ) {	// HP 회복
		CDnDamageDlg *pDlg = m_CountDlgMemPool.Allocate();
		m_listCountDlg.push_back( pDlg );
		pDlg->SetColorType( bMine ? CDnDamageDlg::CT_MYHEAL : (bEnemy ? CDnDamageDlg::CT_MYHEAL : CDnDamageDlg::CT_PARTYHEAL ) );
		pDlg->Initialize( true );	
		pDlg->SetPlus();
#ifdef _RDEBUG
		g_Log.Log(LogType::_FILELOG, "[DMGERR] CDnDamageMng::SetRecovery() : nSP == 0 / vPos(%f, %f, %f) / nHP(%d)\n", vPos.x, vPos.y, vPos.z, nHP);
#endif
		pDlg->SetDamage( vPos, nHP, m_hAni, bMine ? 0.7f : 0.6f );
		pDlg->ShowCount( true );
		pDlg->Show( true );
	}
	else if( nHP == 0 ) {	// SP 회복
		if( bMine ) {
			CDnDamageDlg *pDlg = m_CountDlgMemPool.Allocate();
			m_listCountDlg.push_back( pDlg );
			pDlg->SetColorType( CDnDamageDlg::CT_MYMANA );
			pDlg->Initialize( true );	
			pDlg->SetPlus();
			pDlg->SetDamage( vPos, nSP, m_hAni, 0.7f);
			pDlg->ShowCount( true );
			pDlg->Show( true );
		}
	}
}

void CDnDamageMng::SetCritical( EtVector3 vPos )
{
	CDnDamageDlg *pDlg = m_CountDlgMemPool.Allocate();
	pDlg->SetColorType( CDnDamageDlg::CT_NONE );
	pDlg->Initialize( true );
	m_listCountDlg.push_back( pDlg );
	pDlg->SetCritical( vPos );
	pDlg->ShowCount( true );
	pDlg->Show( true );
}

void CDnDamageMng::SetResist( EtVector3 vPos )
{
	CDnDamageDlg *pDlg = m_CountDlgMemPool.Allocate();
	pDlg->SetColorType( CDnDamageDlg::CT_NONE );
	pDlg->Initialize( true );
	m_listCountDlg.push_back( pDlg );
	pDlg->SetResist( vPos );
	pDlg->ShowCount( true );
	pDlg->Show( true );
}

void CDnDamageMng::SetCriticalRes( EtVector3 vPos )
{
	CDnDamageDlg *pDlg = m_CountDlgMemPool.Allocate();
	pDlg->SetColorType( CDnDamageDlg::CT_NONE );
	pDlg->Initialize( true );
	m_listCountDlg.push_back( pDlg );
	pDlg->SetCriticalRes( vPos );
	pDlg->ShowCount( true );
	pDlg->Show( true );
}

#ifdef PRE_ADD_DECREASE_EFFECT
void CDnDamageMng::SetDecreaseEffect( EtVector3 vPos )
{
	CDnDamageDlg *pDlg = m_CountDlgMemPool.Allocate();
	pDlg->SetColorType( CDnDamageDlg::CT_NONE );
	pDlg->Initialize( true );
	m_listCountDlg.push_back( pDlg );
	pDlg->SetDecreaseEffect( vPos );
	pDlg->ShowCount( true );
	pDlg->Show( true );
}
#endif // PRE_ADD_DECREASE_EFFECT