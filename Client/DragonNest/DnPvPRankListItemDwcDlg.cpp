#include "Stdafx.h"

#ifdef PRE_ADD_DWC

#include "DnPvPRankListItemDwcDlg.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"
#include "DnSimpleTooltipDlg.h"

#ifdef _TH
#define INCREASE_MARKER L"+"
#define DECREASE_MARKER L"-"
#else
#define INCREASE_MARKER L"¡ã"
#define DECREASE_MARKER L"¡å"
#endif


CDnPvPRankListItemDwcDlg::CDnPvPRankListItemDwcDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_pJobIcon(NULL)
{
	for(int i = 0 ; i < e_Max ; ++i) 
		m_pCtrlStatic[i] = NULL;
}

CDnPvPRankListItemDwcDlg::~CDnPvPRankListItemDwcDlg()
{
}

void CDnPvPRankListItemDwcDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName("RankDWCListDlg.ui").c_str(), bShow );	
}

void CDnPvPRankListItemDwcDlg::InitialUpdate()
{
	const char* szCtrlName[] = { "ID_TEXT_RANK", "ID_TEXT_TEAMNAME", "ID_TEXT_TEAMCAPTAIN", "ID_TEXT_RESULT", "ID_TEXT_COUNT" };
	for(int i = 0 ; i < e_Max ; ++i)
	{
		m_pCtrlStatic[i] = GetControl<CEtUIStatic>(szCtrlName[i]);
	}

	m_pJobIcon = GetControl<CDnJobIconStatic>("ID_STATIC_CLASS");
}

void CDnPvPRankListItemDwcDlg::SetDWCRankInfo(TDWCRankData* pInfo)
{
	if(pInfo)
	{
		/*if( pInfo->wszTeamName == NULL || wcslen( pInfo->wszTeamName ) < 1 || pInfo->nRank == 0 )
			return;*/

		wchar_t str[256] = {0,};

		// PVP ¼øÀ§
		std::wstring sign( L"-" );	

		if( pInfo->nChangedRanking < 0 )
			sign.assign( DECREASE_MARKER );
		if( pInfo->nChangedRanking == 1 )
			sign.assign( INCREASE_MARKER );
		if( pInfo->nChangedRanking == 2 )
			sign.assign( L"N" );

		_itow( pInfo->nRank, str, 10 );
		std::wstring strRank( str );
		strRank.append( L"(" );
		strRank.append( sign );
		strRank.append( L")" );
		m_pCtrlStatic[e_Ranking]->SetText( strRank.c_str() );

		// ÆÀ¸í
		m_pCtrlStatic[e_TeamName]->SetText( pInfo->wszTeamName );

		// ¸®´õ Á÷¾÷ ¾ÆÀÌÄÜ.
		m_pJobIcon->SetIconID( pInfo->LeaderData.cJobCode );
		m_pJobIcon->SetTooltipText( DN_INTERFACE::STRING::GetJobString( pInfo->LeaderData.cJobCode ) );

		// ÆÀÀå ¾ÆÀÌµð
		m_pCtrlStatic[e_Captain]->SetText( pInfo->LeaderData.wszMemberName );

		// ÆÀ ½Â·ü
		std::wstring wTempStr = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 126039), pInfo->nWinCount, pInfo->nLoseCount, pInfo->nDrawCount);
		int TotalCount = (pInfo->nWinCount + pInfo->nLoseCount + pInfo->nDrawCount) ? (pInfo->nWinCount + pInfo->nLoseCount + pInfo->nDrawCount) : 1;

		WCHAR wszTemp[256] = { 0 , };
		wsprintf( wszTemp, L" %d%%", (int)((float)pInfo->nWinCount / (float)TotalCount*100) );
		wTempStr.append(wszTemp);
		m_pCtrlStatic[e_Result]->SetText(wTempStr.c_str());

		// ÆòÁ¡
		memset( str, 0, 256 );
		_itow( pInfo->nDWCPoint, str, 10 );	
		m_pCtrlStatic[e_TotalScore]->SetText( str );

		// ¸â¹öµé ¾ÆÀÌµð
		wTempStr.clear();
		wTempStr = FormatW(L"- ");
		wTempStr.append(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120265/*MID:ÆÀ¿ø*/));
		wTempStr.append(L" -");
		
		WCHAR wszTempString[256] = { 0 , };
		wsprintf(wszTempString, _T("%s\n%s\n%s\n%s"),	wTempStr.c_str(),	// -ÆÀ¿ø-
														pInfo->MemberData[0].wszMemberName,  // ÆÀ¿ø1
														pInfo->MemberData[1].wszMemberName,  // ÆÀ¿ø2
														pInfo->MemberData[2].wszMemberName );// ÆÀ¿ø3
		m_pCtrlStatic[e_TeamName]->SetTooltipText(wszTempString);
	}
}
#endif // PRE_ADD_DWC