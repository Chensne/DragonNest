#include "StdAfx.h"
#include "DnPVPDetailedInfoDlg.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnPlayerActor.h"
#include "DnPartyTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPDetailedInfoDlg::CDnPVPDetailedInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
,m_pGrade(NULL)
,m_pGradeIcon(NULL)
,m_pXP(NULL)
,m_pLevelUpArrow(NULL)
,m_pNextLevleUp(NULL)
,m_pKillPoint(NULL)
,m_pSupportPoint(NULL)
,m_pAssistPoint(NULL)
,m_pDefeatedW(NULL)
,m_pDefeatedA(NULL)
,m_pDefeatedS(NULL)
,m_pDefeatedC(NULL)
,m_pKillW(NULL)
,m_pKillA(NULL)
,m_pKillS(NULL)
,m_pKillC(NULL)
{

}

CDnPVPDetailedInfoDlg::~CDnPVPDetailedInfoDlg(void)
{
}

void CDnPVPDetailedInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpRecord.ui" ).c_str(), bShow );	
}

void CDnPVPDetailedInfoDlg::InitialUpdate()
{
	m_pGrade = GetControl<CEtUIStatic>("ID_RESULT_GRADE");
	m_pGradeIcon= GetControl<CEtUITextureControl>("ID_TEXTURE_GRADE");

	m_pXP= GetControl<CEtUIStatic>("ID_RESULT_XP");
	m_pLevelUpArrow= GetControl<CEtUIStatic>("ID_RESULT_LEVELUP");
	
	m_pLevelUpArrow->Show(false);

	m_pNextLevleUp= GetControl<CEtUIStatic>("ID_RESULT_NEXT");
	m_pKillPoint= GetControl<CEtUIStatic>("ID_RESULT_SCORE1");
	m_pAssistPoint= GetControl<CEtUIStatic>("ID_RESULT_SCORE2");
	m_pSupportPoint= GetControl<CEtUIStatic>("ID_RESULT_SCORE3");

	m_pDefeatedW= GetControl<CEtUIStatic>("ID_DEFEATED_WAR");
	m_pDefeatedA= GetControl<CEtUIStatic>("ID_DEFEATED_ARC");
	m_pDefeatedS= GetControl<CEtUIStatic>("ID_DEFEATED_SOR");
	m_pDefeatedC= GetControl<CEtUIStatic>("ID_DEFEATED_CLE");

	m_pKillW= GetControl<CEtUIStatic>("ID_DEFEAT_WAR");
	m_pKillA= GetControl<CEtUIStatic>("ID_DEFEAT_ARC");
	m_pKillS= GetControl<CEtUIStatic>("ID_DEFEAT_SOR");
	m_pKillC= GetControl<CEtUIStatic>("ID_DEFEAT_CLE");
}

void CDnPVPDetailedInfoDlg::SetRecorfInfo( byte cGrade ,UINT uiTotalXP , UINT uiKillXP,UINT uiAssistXP,UINT uiSupportXP  )
{
	WCHAR wszTemp[100];

	SecureZeroMemory(wszTemp,sizeof(wszTemp));

	wsprintf(wszTemp,L"%d",uiTotalXP );
	m_pXP->SetText(wszTemp);	

	/*
	m_pGrade = GetControl<CEtUIStatic>("ID_RESULT_GRADE");
	m_pGradeIcon= GetControl<CEtUITextureControl>("ID_TEXTURE_GRADE");
*/	
	//m_pLevelUpArrow= GetControl<CEtUIStatic>("ID_RESULT_LEVELUP");	

	//m_pNextLevleUp= GetControl<CEtUIStatic>("ID_RESULT_NEXT");

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",uiKillXP );	
	m_pKillPoint->SetText(wszTemp);

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",uiAssistXP );
	m_pAssistPoint->SetText(wszTemp);

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",uiSupportXP );
	m_pSupportPoint->SetText(wszTemp);

	m_pNextLevleUp->SetText(L"");
	SecureZeroMemory(wszTemp,sizeof(wszTemp));

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
	if ( pSox && cGrade != 0 && cGrade < PvPCommon::Common::MaxRank  )
	{
		if( pSox->IsExistItem( cGrade ))
		{
			int iNeedXP = pSox->GetFieldFromLablePtr( cGrade, "PvPRankEXP" )->GetInteger();

			TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();

			if( pPvPInfo->uiXP + uiTotalXP > UINT(iNeedXP) )
			{
				cGrade++;
				m_pLevelUpArrow->Show(true);
				m_pLevelUpArrow->SetBlink(true);

			}else
				m_pLevelUpArrow->Show(false);

			if( cGrade < PvPCommon::Common::MaxRank )
			{
				iNeedXP = pSox->GetFieldFromLablePtr( cGrade, "PvPRankEXP" )->GetInteger();

				wsprintf(wszTemp,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120039 ), iNeedXP - (pPvPInfo->uiXP + uiTotalXP));
				m_pNextLevleUp->SetText(wszTemp);
			}				
		}
	}

	if( GetInterface().GetPVPIconTex() )
	{
		int iIconW,iIconH;
		int iU,iV;


		iIconW = GetInterface().GeticonWidth();
		iIconH = GetInterface().GeticonHeight();

		if( GetInterface().ConvertPVPGradeToUV( cGrade ,iU, iV ))
		{
			m_pGradeIcon->SetTexture(GetInterface().GetPVPIconTex(),iU, iV ,iIconW,iIconH );
			m_pGradeIcon->Show(true);
		}

	}
}

void CDnPVPDetailedInfoDlg::SetKillRecord( int KillW , int KillA , int KillS, int KillC ,int W , int A , int S, int C )
{
	//std::wstring wszTemp;

	WCHAR wszTemp[20];

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",W );
	m_pDefeatedW->SetText(wszTemp);

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",A );
	m_pDefeatedA->SetText(wszTemp);

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",S );
	m_pDefeatedS->SetText(wszTemp);

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",C );
	m_pDefeatedC->SetText(wszTemp);

	
	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",KillW );
	m_pKillW->SetText(wszTemp);

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",KillA );
	m_pKillA->SetText(wszTemp);

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",KillS );
	m_pKillS->SetText(wszTemp);

	SecureZeroMemory(wszTemp,sizeof(wszTemp));
	wsprintf(wszTemp,L"%d",KillC );
	m_pKillC->SetText(wszTemp);

}

