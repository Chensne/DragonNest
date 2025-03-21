#include "Stdafx.h"

#ifdef PRE_ADD_CRAZYDUC_UI

#include "DnScoreMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnScoreMng::CDnScoreMng(void)
{	
	// 추후 애니파일을 교체하던지 이 애니파일을 사용하면서 애니키 값을 내부적으로 줄이던지 판단하자. 
	m_hUIAni = EternityEngine::LoadAni( CEtResourceMng::GetInstance().GetFullName("Damage_Number.ani").c_str() );
}
CDnScoreMng::~CDnScoreMng(void)
{
	SAFE_RELEASE_SPTR( m_hUIAni );
}
void CDnScoreMng::SetScore( EtVector3 vPos, int nScore, int nColorType )
{	
	CDnScoreDlg::COLOR_TYPE colorType;

	if(  nColorType <= 0 )
		return;

	switch(nColorType)
	{
	case EnumScoreColor::DARK_GRAY:
		colorType = CDnScoreDlg::COLOR_TYPE::CT_RED1;
		break;
	case EnumScoreColor::YELLOW:
		colorType = CDnScoreDlg::COLOR_TYPE::CT_WATER;
		break;
	case EnumScoreColor::BLUE:
		colorType = CDnScoreDlg::COLOR_TYPE::CT_WATER;
		break;
	case EnumScoreColor::RED:
		colorType = CDnScoreDlg::COLOR_TYPE::CT_WATER;
		break;
	case EnumScoreColor::BLACK:
		colorType = CDnScoreDlg::COLOR_TYPE::CT_RED1;
		break;
	}

	CDnScoreDlg *pDlg = m_CountDlgMemPool.Allocate();	

	float fFontSize = 1.0f;
	pDlg->SetColorType(colorType);
	pDlg->Initialize( true );
	pDlg->SetScore( vPos, nScore, m_hUIAni, fFontSize );
	pDlg->ShowCount( true );
	pDlg->Show( true );
	
	m_listCountDlg.push_back( pDlg );
}



#endif 