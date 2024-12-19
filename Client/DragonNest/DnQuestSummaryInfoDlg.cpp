#include "StdAfx.h"
#include "DnQuestSummaryInfoDlg.h"
#include "DnQuestTask.h"
#include "DnActor.h"
#include "DnWorld.h"
#include "GlobalValue.h"
#include "DNNotifierCommon.h"
#include "DnMissionTask.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnWorldData.h"
#include "DnNPCActor.h"
#include "DnTableDB.h"
#include "DnQuestTabDlg.h"
#include "DnIntegrateQuestNReputationDlg.h"
#include "DnMissionDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnNotifierControls::CDnNotifierControls()
{

	m_bShow = false;
	m_bMission = false;

	m_nNotifierIndex = -1;
	m_nArrowState = ArrowType::Hide;

	m_PosY = 0.0f;	
	m_fPosY = 0.0f;
	m_fSmoothY = 0.0f;
	m_fDistance = 0.0f;

	m_pIcon = NULL;
	m_pTitle = NULL;
	m_pTextBox = NULL;
	m_pDistance = NULL;
	m_pArrowBase = NULL;
	m_pArrow = NULL;
	m_pArrowArrival = NULL;
	m_pArrowWrong = NULL;
	m_pArrowBattle = NULL;
	m_pArrowFind = NULL;
	m_pArrowArrivalDistance = NULL;
	m_pTraceArrow = NULL;		
	m_pTraceArrowArrival = NULL;
	m_pTraceArrowWrong = NULL;
	m_pTraceArrowBattle = NULL;
	m_pTraceArrowFind = NULL;
	m_pTraceArrowArrivalDistance = NULL;
	m_pDialog = NULL;
	m_pJournal = NULL;
	m_Quest = NULL;
	m_Board = NULL;
	m_OverBoard = NULL;

#ifdef PRE_FIX_48865
	m_pButtonClose = NULL;
#endif

}

void CDnNotifierControls::Show( bool bShow ) 
{
	m_bShow = bShow;
	if( bShow ) {
		if( m_nNotifierIndex < DNNotifier::RegisterCount::TotalQuest ) {
			m_pArrowBase->Show( true );
		}
	}
	else {
		m_pArrowBase->Show( false );				
		m_pArrow->Show( false );
		m_pArrowArrival->Show( false );
		m_pArrowWrong->Show( false );
		m_pArrowBattle->Show( false );
		m_pArrowFind->Show( false );
		m_pArrowArrivalDistance->Show( false );
		m_Board->Show( false );
		m_OverBoard->Show( false );
		m_pDistance->Show(false);

		if( m_bTraceQuest )
		{
			m_pTraceArrow->Show( false );
			m_pTraceArrowArrival->Show( false );
			m_pTraceArrowWrong->Show( false );
			m_pTraceArrowBattle->Show( false );
			m_pTraceArrowFind->Show( false );
			m_pTraceArrowArrivalDistance->Show( false );
		}
	}

	m_pTitle->Show( bShow );
	m_pTextBox->Show( bShow );
	m_pIcon->Show( bShow );
#ifdef PRE_FIX_48865
	m_pButtonClose->Show( bShow );
#endif

}

void CDnNotifierControls::SetPosY( float fY  )
{
	CDnQuestSummaryInfoDlg *pDialog = (CDnQuestSummaryInfoDlg*)m_pDialog;

	float fAddX = 0.f;
	float fAddY = 0.f;

	if( m_bClicked && m_bMouseOver ) {
		fAddX = 2.0f / DEFAULT_UI_SCREEN_WIDTH;
		fAddY = 2.0f / DEFAULT_UI_SCREEN_HEIGHT;
	}

	m_PosY = fY;

	m_Board->SetPosition( fAddX+m_BoardCoord.fX, fAddY+m_BoardCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_OverBoard->SetPosition( fAddX+m_BoardCoord.fX, fAddY+m_BoardCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );

	m_pIcon->SetPosition( fAddX+m_IconCoord.fX, fAddY+m_IconCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pTitle->SetPosition( fAddX+m_TitleCoord.fX, fAddY+m_TitleCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pTextBox->SetPosition( fAddX+m_TextBoxCoord.fX, fAddY+m_TextBoxCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pDistance->SetPosition( fAddX+m_DistanceCoord.fX, fAddY+m_DistanceCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pArrowBase->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pArrow->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pArrowArrival->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pArrowWrong->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pArrowBattle->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pArrowFind->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	m_pArrowArrivalDistance->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );

#ifdef PRE_FIX_48865
	m_pButtonClose->SetPosition( m_pButtonClose->GetUICoord().fX , fAddY + m_BoardCoord.fY + (5.0f / DEFAULT_UI_SCREEN_WIDTH) + m_fDistance * (fY-m_nNotifierIndex) );
#endif

	if( m_bTraceQuest )
	{
		m_pTraceArrow->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
		m_pTraceArrowArrival->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
		m_pTraceArrowWrong->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
		m_pTraceArrowBattle->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
		m_pTraceArrowFind->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
		m_pTraceArrowArrivalDistance->SetPosition( fAddX+m_ArrowCoord.fX, fAddY+m_ArrowCoord.fY + m_fDistance * (fY-m_nNotifierIndex) );
	}
}

void CDnNotifierControls::Process( float fElapsedTime )
{
	static float fMaxDist = 1.5f;
	float fDist = (m_fPosY - m_fSmoothY );
	if( fDist > fMaxDist ) {
		m_fSmoothY += (fDist - fMaxDist );
		fDist = fMaxDist;
	}
	else if( fDist < -fMaxDist ) {
		m_fSmoothY += (fDist + fMaxDist );
		fDist = -fMaxDist;
	}
	m_fSmoothY += fDist * EtMin(1.0f, ( fElapsedTime * 10.f ));
	SetPosY( m_fSmoothY );
}

EtVector2 CDnNotifierControls::CalcArrowDestination(int nQuestIndex , int nJurnalStep)
{
	EtVector2 PosVector(0,0);

	int nMapIndex = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
	const float fDummyPos = 123456.789f;

	if ( nQuestIndex <= 0 || nJurnalStep <= 0)
		return EtVector2(0,0);

	Journal* pJournal = g_DataManager.GetJournalData(nQuestIndex);
	if ( !pJournal )
		return EtVector2(0,0);

	JournalPage* pJournalPage = pJournal->FindJournalPage(nJurnalStep);
	if ( !pJournalPage )
		return EtVector2(0,0);

	if( pJournalPage->nDestinationNpc == -1 ) {
		PosVector = pJournalPage->vDestinationPos;
	}
	else {
		PosVector = EtVector2( fDummyPos, fDummyPos);
		CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( pJournalPage->nDestnationMapIndex );
		int nCount = 0;
		if( pWorldData ) nCount = pWorldData->GetNpcCount();
		int j;
		for( j=0; j<nCount; j++ )
		{
			CDnWorldData::NpcData *pNpcData = pWorldData->GetNpcData( j );
			if( pNpcData->nNpcID == pJournalPage->nDestinationNpc ) {
				PosVector = pNpcData->vPos;
				break;
			}
		}
		if( j == nCount && nMapIndex == pJournalPage->nDestnationMapIndex ) {
			CDnNPCActor *pNpcActor = CDnNPCActor::FindNpcActorFromID( pJournalPage->nDestinationNpc );
			if( !pNpcActor ) return EtVector2(0,0);
			PosVector = EtVector2( pNpcActor->GetPosition()->x, pNpcActor->GetPosition()->z);
		}
	}

	// 현재 위치와 목적지가 다르면
	if ( CGlobalInfo::GetInstance().m_nCurrentMapIndex != pJournalPage->nDestnationMapIndex  )
	{
		PathResult	pathResult;		
		GetQuestTask().GetPathFinder()->GetPathOneWay(CGlobalInfo::GetInstance().m_nCurrentMapIndex, pJournalPage->nDestnationMapIndex , pathResult );

		if ( pathResult.size() > 0 )
		{
			// 어느 게이트로 나가야 되는지 알아낸다음에
			int nGateIndex = pathResult[ 0 ].nGateIndex;
			DWORD dwCount = CDnWorld::GetInstance().GetGateCount();
			CDnWorld::GateStruct *pGateStruct( NULL );
			SOBB *pOBB( NULL );

			// 맵 링크 정보는 제대로 되어있지만 게이트 정보는 제대로 되어있지 않을때
			// 일단 타겟 위치는 이상한 값을 넣어놓는다.
			PosVector.x = PosVector.y = fDummyPos;
			if( nGateIndex == -1 ) {
				return EtVector2(0,0);
			}
			pGateStruct = CDnWorld::GetInstance().GetGateStruct( nGateIndex );
			if( !pGateStruct )
				return EtVector2(0,0);
			if( !(pGateStruct->pGateArea) )
				return EtVector2(0,0);

			pOBB = pGateStruct->pGateArea->GetOBB();

			if( !pOBB )
				return EtVector2(0,0);

			EtVector3 vGatePos = pOBB->Center;
			PosVector.x = vGatePos.x;
			PosVector.y = vGatePos.z;			
		}
	}

	return PosVector;
}

void CDnNotifierControls::DrawArrow()
{
	if( !CDnQuestTask::IsActive() )
		return;
	if( !m_bShow )
		return;
	if( m_nNotifierIndex >= DNNotifier::RegisterCount::TotalQuest ) 
		return;

	CDnQuestTask::QuestNotifierInfo Quest = GetQuestTask().GetNotifierInfo( m_nNotifierIndex );

	const EtVector3 *pPlayerPos(NULL);
	DnActorHandle hPlayer = CDnActor::s_hLocalActor;

	if( hPlayer )
		pPlayerPos = hPlayer->GetPosition();
	else 
		pPlayerPos = CDnWorld::GetInstance().GetUpdatePosition();

	EtVector3 vDestPos;

	if(m_Quest) 
	{
		EtVector2 vTargetPos  = CDnNotifierControls::CalcArrowDestination( m_Quest->nQuestID , m_Quest->cQuestJournal );
		vDestPos.x = vTargetPos.x;
		vDestPos.z = vTargetPos.y;
	}
	else
	{
		vDestPos.x = Quest.vTargetPos.x;
		vDestPos.z = Quest.vTargetPos.y;
	}

	vDestPos.y = 0.0f;

	const float fDummyPos = 123456.789f - 10.0f;

	bool bInvalidTarget = false;
	if( vDestPos.x > fDummyPos ) {
		bInvalidTarget = true;
	}

	EtVector3 vZDir, vCross, vTemp;
	vTemp = vDestPos - (*pPlayerPos);
	vTemp.y = 0.0f;
	EtVec3Normalize( &vTemp, &vTemp );

	EtVector3 vPlayerPos = *pPlayerPos;

	EtVector2 v1,v2;
	v1.x = vDestPos.x;
	v1.y = vDestPos.z;
	v2.x = vPlayerPos.x;
	v2.y = vPlayerPos.z;
	EtVector2 v = v1 - v2;

	float fDist = EtVec2Length(&v);
	int nDist = abs(int(fDist/100.0f));
	WCHAR wszBuff[256] = {0,};
	if ( fDist > 0.0f ) {
		if( bInvalidTarget ) {
			nDist = 0;
		}
		swprintf_s( wszBuff, _countof(wszBuff), L"%dM", nDist );
	}

	m_pDistance->SetText( wszBuff );

	CEtUIStatic *pArrow = NULL;

	if( m_bNowTraceQuest && m_bTraceQuest )
	{
		if ( m_nArrowState == ArrowType::Hide )			pArrow = m_pTraceArrowWrong;
		if ( m_nArrowState == ArrowType::NormalArrow )	{
			if( fDist < 100.f ) pArrow = m_pTraceArrowArrivalDistance;
			else pArrow = m_pTraceArrow;
		}
		if ( m_nArrowState == ArrowType::ArrivalMark )	pArrow = m_pTraceArrowArrival;
		if ( m_nArrowState == ArrowType::BattleMark )	pArrow = m_pTraceArrowBattle;
		if ( m_nArrowState == ArrowType::FindMark )		pArrow = m_pTraceArrowFind;
	}
	else
	{
		if ( m_nArrowState == ArrowType::Hide )			pArrow = m_pArrowWrong;
		if ( m_nArrowState == ArrowType::NormalArrow )	{
			if( fDist < 100.f ) pArrow = m_pArrowArrivalDistance;
			else pArrow = m_pArrow;
		}
		if ( m_nArrowState == ArrowType::ArrivalMark )	pArrow = m_pArrowArrival;
		if ( m_nArrowState == ArrowType::BattleMark )	pArrow = m_pArrowBattle;
		if ( m_nArrowState == ArrowType::FindMark )		pArrow = m_pArrowFind;
	}

	if ( !pArrow )
		return;

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera ) {
		vZDir = hCamera->GetMatEx()->m_vZAxis;
		vZDir.y = 0.0f;
	}

	EtVec3Normalize( &vZDir, &vZDir );
	float fDegree = EtAcos( EtVec3Dot( &vTemp, &vZDir ) );
	EtVec3Cross( &vCross, &vTemp, &vZDir );
	if( vCross.y > 0.0f ) {
		fDegree = -fDegree;
	}

	if( bInvalidTarget ) {
		fDegree = 0.0f;
	}

	SUICoord uiCoord;
	pArrow->GetUICoord(uiCoord);
	SUIElement *pElement = pArrow->GetElement(0);

	LOCAL_TIME time = CGlobalInfo::GetInstance().m_LocalTime;

	switch( m_nArrowState )
	{
	case ArrowType::NormalArrow:
		{
			static int nTerm = 1200;
			int nCurrMapIndex = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
			if( m_pathResult.size() == 1 &&  m_pathResult[0].nMapIndex == nCurrMapIndex ) {
				int nAlpha = EtClamp( (int)(sinf( ET_PI * 2* (GetTickCount() % nTerm) / (float)nTerm ) * 100 + (255-100) ), 0, 255);
				if( pArrow->GetTemplate().m_hTemplateTexture )
					m_pDialog->DrawSprite( pArrow->GetTemplate().m_hTemplateTexture, pElement->TemplateUVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), uiCoord, EtToDegree(fDegree) );
				else
					m_pDialog->DrawSprite( pElement->UVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), uiCoord, EtToDegree(fDegree) );
			}
			else {
				if( pArrow->GetTemplate().m_hTemplateTexture )
					m_pDialog->DrawSprite( pArrow->GetTemplate().m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, uiCoord, EtToDegree(fDegree) );
				else
					m_pDialog->DrawSprite( pElement->UVCoord, 0xffffffff, uiCoord, EtToDegree(fDegree) );
			}
			m_pDistance->Show(true);
		}
		break;
	case ArrowType::Hide:
		{
			if( pArrow->GetTemplate().m_hTemplateTexture )
				m_pDialog->DrawSprite( pArrow->GetTemplate().m_hTemplateTexture, pElement->TemplateUVCoord, D3DCOLOR_XRGB(255,255,255), uiCoord, 0.0f );
			else
				m_pDialog->DrawSprite( pElement->UVCoord, D3DCOLOR_XRGB(255,255,255), uiCoord, 0.0f );
			m_pDistance->Show(false);
		}
		break;
	case ArrowType::ArrivalMark:
	case ArrowType::BattleMark:
	case ArrowType::FindMark:
		{
			int nAlpha = 255;
			int BLEND_TIME = 500;
			float BLEND_RANGE  = 1.0f;
			int nTemp = (int)time%(BLEND_TIME+BLEND_TIME);
			if( nTemp < BLEND_TIME )  {
				nAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * nTemp * 255.0f);
			}
			else {
				nAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * ( (BLEND_TIME+BLEND_TIME) - nTemp ) *  255.0f);
			}
			if( pArrow->GetTemplate().m_hTemplateTexture )
				m_pDialog->DrawSprite( pArrow->GetTemplate().m_hTemplateTexture, pElement->TemplateUVCoord, D3DCOLOR_ARGB((DWORD)nAlpha,255,255,255), uiCoord, 0.0f );
			else
				m_pDialog->DrawSprite( pElement->UVCoord, D3DCOLOR_ARGB((DWORD)nAlpha,255,255,255), uiCoord, 0.0f );
			m_pDistance->Show(false);
		}
		break;
	}
}

void CDnNotifierControls::Refresh( const wchar_t *szTitle, const wchar_t *szDest, std::vector< boost::tuple<std::wstring, int, int> > &vecGetList, std::wstring szProgress )
{
	m_pTitle->ClearText();
#ifdef PRE_MOD_MISSION_HELPER
	m_pTitle->SetTooltipText( L"" );
#endif
	m_pTextBox->ClearText();
	m_pTextBox->SetTooltipText( L"" );

	wchar_t szTemp[ 256 ] = {0, };
	swprintf_s( szTemp, _countof(szTemp), L"%s", szTitle );
	m_pTitle->SetTextWithEllipsis( szTemp, L"..." );

#ifdef PRE_MOD_MISSION_HELPER
	if( !m_bMission )
		m_pTextBox->AppendText( szDest );
#else
	m_pTextBox->AppendText( szDest );
#endif
	int nSize = m_pTextBox->GetLineSize();

#if !defined(_US)
	if(nSize > 1) // 한줄이상이면 한줄만 남기고 ".." 추가
		m_pTextBox->ResizeLineTextWithSymbol(1,L"..");
#endif

#ifdef PRE_MOD_MISSION_HELPER
	if( !m_bMission ) {
		if( vecGetList.size() == 1 ) {
			int nCnt = vecGetList[0].get<1>();
			int nTotalCnt = vecGetList[0].get<2>();
			if( nCnt < 0 && nTotalCnt < 0 ) {
				swprintf_s( szTemp, L"%s", vecGetList[0].get<0>().c_str() );			
			}
			else if( nTotalCnt < 0 ) {
				swprintf_s( szTemp, L"- %s [%d]", vecGetList[0].get<0>().c_str(), vecGetList[0].get<1>()  );
			}
			else {

				int WORD_MAXSIZE = 12;

				if((int)vecGetList[0].get<0>().size() > WORD_MAXSIZE)
				{
					std::wstring wszTemp;
					wszTemp = vecGetList[0].get<0>().substr( 0, WORD_MAXSIZE );

					swprintf_s( szTemp, L"- %s.. [%d/%d]", wszTemp.c_str() , vecGetList[0].get<1>(), vecGetList[0].get<2>() );	
				}
				else
				{
					swprintf_s( szTemp, L"- %s [%d/%d]", vecGetList[0].get<0>().c_str(), vecGetList[0].get<1>(), vecGetList[0].get<2>() );	
				}

			}				

			m_pTextBox->AddText( szTemp );
		}
		else if( !vecGetList.empty() ) {
			int nItemCnt = 0;
			int nItemTotalCnt = 0;
			std::wstring szGetString;
			int nMaxLen = 0;
			for each( boost::tuple<std::wstring, int, int> e in vecGetList ) {
				if( e.get<1>() < 0 ) continue;
				int nTotalCount = e.get<2>();
				nItemCnt += e.get<1>();
				nItemTotalCnt += nTotalCount < 0 ? 1 : nTotalCount;
				if( nTotalCount < 0 ) {
					std::wstring szAdd = FormatW( L"  %s [%d]    \n",  e.get<0>().c_str(), e.get<1>() );
					nMaxLen = EtMax( nMaxLen, (int)szAdd.length());
					szGetString += szAdd;
				}
				else {
					std::wstring szAdd = FormatW( L"  %s [%d/%d]    \n",  e.get<0>().c_str(), e.get<1>(), e.get<2>() );
					nMaxLen = EtMax( nMaxLen, (int)szAdd.length());
					szGetString += szAdd;
				}
			}

			for( int i = 0; i < 5*nMaxLen/2; i++)
				szGetString += L" ";
			szGetString += L"\n";
			if( nItemTotalCnt != 0 &&  nItemCnt <= nItemTotalCnt) { // #22983 관련 아이템 카운트 맥스치 초과하는 경우 없음.
				m_pTextBox->AddText( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 422 ), 100 * nItemCnt / nItemTotalCnt ).c_str(), textcolor::YELLOW );
				m_pTextBox->SetTooltipText( szGetString.c_str() );
			}
		}
	}
#else
	if( vecGetList.size() == 1 ) {
		int nCnt = vecGetList[0].get<1>();
		int nTotalCnt = vecGetList[0].get<2>();
		if( nCnt < 0 && nTotalCnt < 0 ) {
			swprintf_s( szTemp, L"%s", vecGetList[0].get<0>().c_str() );			
		}
		else if( nTotalCnt < 0 ) {
			swprintf_s( szTemp, L"- %s [%d]", vecGetList[0].get<0>().c_str(), vecGetList[0].get<1>()  );
		}
		else {

			int WORD_MAXSIZE = 12;

			if((int)vecGetList[0].get<0>().size() > WORD_MAXSIZE)
			{
				std::wstring wszTemp;
				wszTemp = vecGetList[0].get<0>().substr( 0, WORD_MAXSIZE );

				swprintf_s( szTemp, L"- %s.. [%d/%d]", wszTemp.c_str() , vecGetList[0].get<1>(), vecGetList[0].get<2>() );	
			}
			else
			{
				swprintf_s( szTemp, L"- %s [%d/%d]", vecGetList[0].get<0>().c_str(), vecGetList[0].get<1>(), vecGetList[0].get<2>() );	
			}

		}				

		m_pTextBox->AddText( szTemp );
	}
	else if( !vecGetList.empty() ) {
		int nItemCnt = 0;
		int nItemTotalCnt = 0;
		std::wstring szGetString;
		int nMaxLen = 0;
		for each( boost::tuple<std::wstring, int, int> e in vecGetList ) {
			if( e.get<1>() < 0 ) continue;
			int nTotalCount = e.get<2>();
			nItemCnt += e.get<1>();
			nItemTotalCnt += nTotalCount < 0 ? 1 : nTotalCount;
			if( nTotalCount < 0 ) {
				std::wstring szAdd = FormatW( L"  %s [%d]    \n",  e.get<0>().c_str(), e.get<1>() );
				nMaxLen = EtMax( nMaxLen, (int)szAdd.length());
				szGetString += szAdd;
			}
			else {
				std::wstring szAdd = FormatW( L"  %s [%d/%d]    \n",  e.get<0>().c_str(), e.get<1>(), e.get<2>() );
				nMaxLen = EtMax( nMaxLen, (int)szAdd.length());
				szGetString += szAdd;
			}
		}

		for( int i = 0; i < 5*nMaxLen/2; i++)
			szGetString += L" ";
		szGetString += L"\n";
		if( nItemTotalCnt != 0 &&  nItemCnt <= nItemTotalCnt) { // #22983 관련 아이템 카운트 맥스치 초과하는 경우 없음.
			m_pTextBox->AddText( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 422 ), 100 * nItemCnt / nItemTotalCnt ).c_str(), textcolor::YELLOW );
			m_pTextBox->SetTooltipText( szGetString.c_str() );
		}
	}
#endif

	if( m_bMission ) {
		m_pTextBox->AppendText( szProgress.c_str(), textcolor::WHITE );
#ifdef PRE_MOD_MISSION_HELPER
		std::wstring wszTooltip;
		wszTooltip += szTitle;
		if( !szProgress.empty() ) {
			wszTooltip += L"     ";
			wszTooltip += szProgress;
		}
		wszTooltip += L"\n\n";
		wszTooltip += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7877 );
		wszTooltip += L" ";
		wszTooltip += szDest;
		if( vecGetList.size() == 2 ) {
			wszTooltip += L"\n";
			wszTooltip += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7878 );
			wszTooltip += L" ";
			wszTooltip += vecGetList[0].get<0>();
			wszTooltip += L"\n";
			wszTooltip += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7879 );
			wszTooltip += L" ";
			wszTooltip += vecGetList[1].get<0>();
		}
		m_pTitle->SetTooltipText( wszTooltip.c_str() );
#endif
	}

	m_pIcon->Show( true );
	if( m_nNotifierIndex < DNNotifier::RegisterCount::TotalQuest ) {
		m_pArrowBase->Show( true );
	}
}

int CDnNotifierControls::GetNotifyArrowState(int nQuestID , int nJurnalStep)
{
	int nArrowState = ArrowType::Hide;
	int nMapIndex = CGlobalInfo::GetInstance().m_nCurrentMapIndex;

	Journal* pJournal = g_DataManager.GetJournalData( nQuestID );
	if( !pJournal ) return false;	

	JournalPage* pJournalPage = pJournal->FindJournalPage( nJurnalStep );
	if ( !pJournalPage ) return false;

	switch( pJournalPage->nDestnationMode )
	{
	case JournalPage::none:
		nArrowState = ArrowType::NormalArrow;
		break;
	case JournalPage::hide:
		nArrowState = ArrowType::Hide;
		break;
	case JournalPage::arrival:
		nArrowState = ArrowType::ArrivalMark;
		break;
	case JournalPage::stage_hide:
		{
			if ( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon )
				nArrowState = ArrowType::Hide;
			else
				nArrowState = ArrowType::NormalArrow;
		}
		break;
	case JournalPage::stage_arrival:
		{
			if ( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon )
				nArrowState = ArrowType::ArrivalMark;
			else
				nArrowState = ArrowType::NormalArrow;
		}
		break;
	case JournalPage::select_hide:
		{
			if ( std::find( pJournalPage->DestnationModeValue.begin(), pJournalPage->DestnationModeValue.end(), nMapIndex ) != pJournalPage->DestnationModeValue.end() )
				nArrowState = ArrowType::Hide;
			else
				nArrowState = ArrowType::NormalArrow;
		}
		break;
	case JournalPage::select_arrival:
		{
			if ( std::find( pJournalPage->DestnationModeValue.begin(), pJournalPage->DestnationModeValue.end(), nMapIndex ) != pJournalPage->DestnationModeValue.end() )
				nArrowState = ArrowType::ArrivalMark;
			else
				nArrowState = ArrowType::Hide;
		}
		break;
	case JournalPage::stage_hide_except:
		{
			if ( std::find( pJournalPage->DestnationModeValue.begin(), pJournalPage->DestnationModeValue.end(), nMapIndex ) == pJournalPage->DestnationModeValue.end() )
				nArrowState = ArrowType::NormalArrow;
			else
				nArrowState = ArrowType::Hide;
		}
		break;
	case JournalPage::custom:
		{			
			if ( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon )
				nArrowState = ArrowType::Hide;
			else
				nArrowState = ArrowType::NormalArrow;

			for ( size_t i = 0 ; i < pJournalPage->CustomDestnationMark.size() ; ++i )
			{
				if ( pJournalPage->CustomDestnationMark[i].nMapIdx == nMapIndex && 
					pJournalPage->CustomDestnationMark[i].nMarkIdx > -1	  && 
					pJournalPage->CustomDestnationMark[i].nMarkIdx < ArrowType::MaxCount )
				{
					nArrowState = pJournalPage->CustomDestnationMark[i].nMarkIdx;
				}
			}

			nArrowState = CheckNotifyArrowStateByMap( nArrowState , pJournalPage->nDestnationMapIndex );
		}
		break;
	}

	return nArrowState;
}

int CDnNotifierControls::CheckNotifyArrowStateByMap( int nArrowState , int nDestinationMapIndex )
{
	DNTableFileFormat* pMapSox = GetDNTable( CDnTableDB::TMAP );
	if( !pMapSox )
		return nArrowState;

	if( CDnWorld::GetInstance().GetMapSubType() ==  CDnWorld::MapSubTypePvPVillage )
	{
		if( pMapSox->IsExistItem(nDestinationMapIndex) )
		{
			int nDestinationMapSubType = pMapSox->GetFieldFromLablePtr( nDestinationMapIndex , "_MapSubType" )->GetInteger();

			if(nDestinationMapSubType == CDnWorld::MapSubTypePvPVillage)
				return nArrowState;
		}
		return ArrowType::Hide;
	}

	int nOrignalArrowState = nArrowState;
	int nCurrentMapIndex = CGlobalInfo::GetInstance().m_nCurrentMapIndex;

	bool bIsolatedMap = false;
	bool bIsolatedDestination = false;


	if( pMapSox->IsExistItem(nCurrentMapIndex) )
	{
		int nSubMapType = pMapSox->GetFieldFromLablePtr( nCurrentMapIndex , "_MapSubType" )->GetInteger();
		if( nSubMapType == CDnWorld::MapSubTypeDarkLairWorldZone )
			bIsolatedMap = true;
	}

	if( pMapSox->IsExistItem(nDestinationMapIndex) )
	{
		int nSubMapType = pMapSox->GetFieldFromLablePtr( nDestinationMapIndex , "_MapSubType" )->GetInteger();
		if( nSubMapType == CDnWorld::MapSubTypeDarkLairWorldZone || nSubMapType == CDnWorld::MapSubTypeDarkLair	)
			bIsolatedDestination = true;
	}

	if( bIsolatedMap != bIsolatedDestination ) 
	{
		if( nOrignalArrowState == ArrowType::NormalArrow )
			nArrowState = ArrowType::Hide;
	}
	else
	{
		nArrowState = nOrignalArrowState;
	}

	return nArrowState;
}

void CDnNotifierControls::UpdateDestinationMode()
{
	int nQuestIndex = 0;
	int nJournalStep = 0;

	if(m_Quest)
	{
		nQuestIndex = m_Quest->nQuestID;
		nJournalStep = m_Quest->cQuestJournal;
	}
	else
	{
		CDnQuestTask::QuestNotifierInfo questNotifierInfo = GetQuestTask().GetNotifierInfo( m_nNotifierIndex );
		nQuestIndex = questNotifierInfo.nQuestIndex;
		nJournalStep = questNotifierInfo.nJournalStep;
	}

	m_nArrowState = CDnNotifierControls::GetNotifyArrowState(nQuestIndex,nJournalStep);
}

void CDnNotifierControls::ChangeArrowStateByPathCheck()
{
	if( m_nArrowState == ArrowType::NormalArrow && m_pathResult.empty() )
		m_nArrowState = ArrowType::Hide;
}

void CDnNotifierControls::SetNotifier( int nNotifier )
{
	m_nNotifierIndex = nNotifier;
	m_IconCoord = m_pIcon->GetUICoord();
	m_BoardCoord = m_Board->GetUICoord();
	m_TitleCoord = m_pTitle->GetUICoord();
	m_TextBoxCoord = m_pTextBox->GetUICoord();
	m_DistanceCoord = m_pDistance->GetUICoord();
	m_ArrowCoord = m_pArrowBase->GetUICoord();
	m_SelectCoord = m_BoardCoord;	
	m_SelectCoord.fHeight *= 0.97f;
}

void CDnNotifierControls::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{	
	if( uMsg == WM_MOUSEMOVE ) {
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		m_pDialog->PointToFloat( MousePoint, fMouseX, fMouseY );
		SUICoord Coord = m_SelectCoord;
		Coord.fY += m_fDistance * (m_PosY - m_nNotifierIndex);
		m_bMouseOver = Coord.IsInside( fMouseX, fMouseY ) && m_bShow;
	}
}

void CDnNotifierControls::PostRender()
{
	DrawArrow();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDnQuestSummaryInfoDlg::CDnQuestSummaryInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
	m_fSelectAlpha = 0.f;
	m_nOverIndex = -1;
	m_nClickIndex = -1;
	m_lastHeight = 0.f;
	m_lastOverIndex = 0;
	m_bPriorityMode = false;
	m_fControlDistance = 0.0f;
	m_fOverIndexSmooth = 0.0f;
}

CDnQuestSummaryInfoDlg::~CDnQuestSummaryInfoDlg(void)
{
}

void CDnQuestSummaryInfoDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "QuestSumInfoDlg.ui" ).c_str(), bShow );
}

void CDnQuestSummaryInfoDlg::InitialUpdate()
{
	for( int i = 0; i < DNNotifier::RegisterCount::Total; i++) 
	{
		char szType[32]={0,};
		if( i < DNNotifier::RegisterCount::MainQuest ) {
			_strcpy( szType, _countof(szType), "MAIN", (int)strlen("MAIN"));
		}
		else if( i < DNNotifier::RegisterCount::TotalQuest ) {
			sprintf_s( szType, "SUB%d", i - DNNotifier::RegisterCount::MainQuest );
		}
		else {
			sprintf_s( szType, "MIS%d", i - DNNotifier::RegisterCount::TotalQuest );
		}

		m_NotifierControls[ i ].m_OverBoard = GetControl<CEtUIStatic>( FormatA("ID_STATIC_OVER%d", i ).c_str() );
		m_NotifierControls[ i ].m_Board = GetControl<CEtUIStatic>( FormatA("ID_STATIC_BOARD%d", i ).c_str() );

		m_NotifierControls[ i ].m_OverBoard->Show( false );
		m_NotifierControls[ i ].m_Board->Show( false );

		m_NotifierControls[ i ].m_pIcon = GetControl<CEtUIStatic>( FormatA("ID_%s_ICON", szType ).c_str() );
		m_NotifierControls[ i ].m_pTitle = GetControl<CEtUIStatic>( FormatA("ID_%s_TITLE", szType ).c_str() );
		m_NotifierControls[ i ].m_pTextBox = GetControl<CEtUITextBox>( FormatA("ID_%s_TEXTBOX", szType ).c_str() );
		m_NotifierControls[ i ].m_pDistance = GetControl<CEtUIStatic>( FormatA("ID_%s_DISTANCE", szType ).c_str() );

		m_NotifierControls[ i ].m_pArrowBase = GetControl<CEtUIStatic>( FormatA("ID_%s_ARROW_BASE", szType ).c_str() );

		m_NotifierControls[ i ].m_pArrow = GetControl<CEtUIStatic>( FormatA("ID_%s_ARROW", szType ).c_str() );
		m_NotifierControls[ i ].m_pArrowArrival = GetControl<CEtUIStatic>( FormatA("ID_%s_ARRIVAL", szType ).c_str() );
		m_NotifierControls[ i ].m_pArrowWrong = GetControl<CEtUIStatic>( FormatA("ID_%s_WRONG", szType ).c_str() );
		m_NotifierControls[ i ].m_pArrowBattle = GetControl<CEtUIStatic>( FormatA("ID_%s_BATTLE", szType ).c_str() );
		m_NotifierControls[ i ].m_pArrowFind = GetControl<CEtUIStatic>( FormatA("ID_%s_FIND", szType ).c_str() );
		m_NotifierControls[ i ].m_pArrowArrivalDistance = GetControl<CEtUIStatic>( FormatA("ID_%s_HERE", szType ).c_str() );

#ifdef PRE_FIX_48865
		m_NotifierControls[ i ].m_pButtonClose = GetControl<CEtUIButton>( FormatA("ID_BT_%s_CLOSE", szType ).c_str() );
#endif

		m_NotifierControls[ i ].m_bTraceQuest = false;
		m_NotifierControls[ i ].m_bNowTraceQuest = false;

		if( i < DNNotifier::RegisterCount::TotalQuest )
		{
			m_NotifierControls[ i ].m_bTraceQuest = true;

			if( i < DNNotifier::RegisterCount::MainQuest )
				_strcpy( szType, _countof(szType), "TRACK", (int)strlen("TRACK"));
			else
				sprintf_s( szType, "TRACK%d", i );

			m_NotifierControls[ i ].m_pTraceArrow = GetControl<CEtUIStatic>( FormatA("ID_%s_ARROW", szType ).c_str() );
			m_NotifierControls[ i ].m_pTraceArrowArrival = GetControl<CEtUIStatic>( FormatA("ID_%s_ARRIVAL", szType ).c_str() );
			m_NotifierControls[ i ].m_pTraceArrowWrong = GetControl<CEtUIStatic>( FormatA("ID_%s_WRONG", szType ).c_str() );
			m_NotifierControls[ i ].m_pTraceArrowBattle = GetControl<CEtUIStatic>( FormatA("ID_%s_BATTLE", szType ).c_str() );
			m_NotifierControls[ i ].m_pTraceArrowFind = GetControl<CEtUIStatic>( FormatA("ID_%s_FIND", szType ).c_str() );
			m_NotifierControls[ i ].m_pTraceArrowArrivalDistance = GetControl<CEtUIStatic>( FormatA("ID_%s_HERE", szType ).c_str() );
		}

		m_NotifierControls[ i ].SetDialog( this );
		m_NotifierControls[ i ].SetNotifier( i );
		m_NotifierControls[ i ].Show( false );	
		m_NotifierControls[ i ].MoveY( float(i) );
	}

	m_SelectCoord = m_NotifierControls[0].m_pArrowBase->GetUICoord();
	m_SelectCoord.Merge( m_NotifierControls[ 0 ].m_pTextBox->GetUICoord() );
	m_SelectCoord.fHeight *= 0.95f;

	m_BlurCoord = DlgCoordToScreenCoord( m_SelectCoord );
	m_BlurCoord.fX -= 0.005f;
	m_BlurCoord.fY -= 0.005f;
	m_BlurCoord.fWidth -= 0.01f;
	m_BlurCoord.fHeight += 0.01f;


	m_fControlDistance = m_NotifierControls[ 1 ].m_pIcon->GetProperty()->UICoord.fY - m_NotifierControls[ 0 ].m_pIcon->GetProperty()->UICoord.fY;
	for( int i = 0; i < DNNotifier::RegisterCount::Total; i++) {
		m_NotifierControls[ i ].SetDistance( m_fControlDistance );
	}
}

bool CDnQuestSummaryInfoDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() ) {
		return BaseClass::MsgProc( hWnd, uMsg, wParam, lParam );
	}

	int nOverIndex = -1;
	for( int i = 0; i < DNNotifier::RegisterCount::Total; i++) {
		m_NotifierControls[ i ].MsgProc( hWnd, uMsg, wParam, lParam );		// MouseOver 처리
		if( m_NotifierControls[ i ].IsMouseOver() ) {
			nOverIndex = i;
		}
	}	

	if( nOverIndex == -1 ) {
		for( int i = 0; i < DNNotifier::RegisterCount::Total; i++)  {
			m_NotifierControls[ i ].m_OverBoard->Show( false );
			m_NotifierControls[ i ].m_Board->Show( false );			
		}
	}
	else {
		for( int i = 0; i < DNNotifier::RegisterCount::Total; i++)  {
			if( m_NotifierControls[ i ].IsShow() ) {

				m_NotifierControls[ i ].m_OverBoard->Show( i == nOverIndex );
				m_NotifierControls[ i ].m_Board->Show( true );
			} 
			else {
				m_NotifierControls[ i ].m_Board->Show( false );
				m_NotifierControls[ i ].m_OverBoard->Show( false );
			}
		}
	
		switch( uMsg )
		{
		case WM_LBUTTONDOWN:
			{
				m_nClickIndex = nOverIndex;	

#ifdef PRE_FIX_48865
				if( m_NotifierControls[ nOverIndex ].m_pButtonClose )
				{
					POINT MousePoint;
					MousePoint.x = LONG( LOWORD( lParam ) );
					MousePoint.y = LONG( HIWORD( lParam ) );

					float fMouseX, fMouseY;
					m_NotifierControls[ nOverIndex ].m_pDialog->PointToFloat( MousePoint, fMouseX, fMouseY );

					if( m_NotifierControls[ nOverIndex ].m_pButtonClose->IsInside(fMouseX,fMouseY) )
					{
						m_NotifierControls[ nOverIndex ].Show(false);

						if( m_NotifierControls[ nOverIndex ].IsMission() ) {
#ifdef PRE_MOD_MISSION_HELPER
							int nIndex = nOverIndex - DNNotifier::RegisterCount::TotalQuest;
							CDnMissionTask::MissionInfoStruct *pMissionStruct = GetMissionTask().GetMissionInfoForNotifier( nIndex );
							if( pMissionStruct )
								GetMissionTask().RemoveNotifier( GetMissionTask().GetNotifierType( pMissionStruct ), pMissionStruct->nNotifierID );

							CDnMissionDlg *pDlg = (CDnMissionDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::MISSION_DIALOG );
							if( pDlg ) pDlg->RefreshInfo();
#else
							bool bDaily = (nOverIndex == DNNotifier::RegisterCount::TotalQuest);
							CDnMissionTask::MissionInfoStruct *pMissionStruct = GetMissionTask().GetMissionInfoForNotifier( CDnMissionTask::DailyMissionTypeEnum(bDaily ? 0 : 1) );
							if( pMissionStruct )
								GetMissionTask().RemoveNotifier(  pMissionStruct->nNotifierID );
#endif
						}
						else
						{
							Journal *pJournal = m_NotifierControls[ nOverIndex ].GetJournal();
							if( pJournal ) 
							{
								if( nOverIndex < DNNotifier::RegisterCount::MainQuest ) 
								{
									if( GetQuestTask().RemoveQuestNotifier( DNNotifier::Type::MainQuest, pJournal->nQuestIndex ) )
										GetQuestTask().OnUpdateNotifier( true );
								}
								else if( nOverIndex < DNNotifier::RegisterCount::TotalQuest ) 
								{
									if( GetQuestTask().RemoveQuestNotifier( DNNotifier::Type::SubQuest, pJournal->nQuestIndex ) )
										GetQuestTask().OnUpdateNotifier( true );
								}
							}
						}

						m_nClickIndex = -1;
					}
					if( GetQuestTask().GetQuestDlg() )
						GetQuestTask().GetQuestDlg()->RefreshQuestNotifierCheck();
				}
#endif
			}
			break;
		case WM_LBUTTONUP:
			{
				// Open Dialog
				if( m_nClickIndex == nOverIndex) {
					if( m_NotifierControls[ nOverIndex ].IsMission() ) {
#ifdef PRE_MOD_MISSION_HELPER
						int nIndex = nOverIndex - DNNotifier::RegisterCount::TotalQuest;
						CDnMissionTask::MissionInfoStruct *pMissionStruct = GetMissionTask().GetMissionInfoForNotifier( nIndex );
						if( pMissionStruct )
							GetInterface().GetMainMenuDialog()->ShowMissionDialog( (MissionInfoStruct *)pMissionStruct );
#else
						bool bDaily = (nOverIndex == DNNotifier::RegisterCount::TotalQuest);
						int nItemID = -1;
						CDnMissionTask::MissionInfoStruct *pMissionStruct = GetMissionTask().GetMissionInfoForNotifier( CDnMissionTask::DailyMissionTypeEnum(bDaily ? 0 : 1) );
						if( pMissionStruct ) nItemID = pMissionStruct->nNotifierID;
						GetInterface().GetMainMenuDialog()->ShowMissionDialog( bDaily, nItemID );
#endif
					}
					else {
						Journal *pJournal = m_NotifierControls[ nOverIndex ].GetJournal();
						if( pJournal ) {
							GetInterface().GetMainMenuDialog()->ShowQuestDialog( true, pJournal->nChapterIndex, pJournal->nQuestIndex, pJournal->IsMainQuest() );
							GetInterface().CloseInspectPlayerInfoDialog();
						}
					}
				}

				m_nClickIndex = -1;
			}
			return true;
		}
	}


	return BaseClass::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnQuestSummaryInfoDlg::Process( float fElapsedTime )
{
	for( int i = 0; i < DNNotifier::RegisterCount::Total; i++) {
		m_NotifierControls[ i ].SetClicked( ( m_nClickIndex == i ) );
		m_NotifierControls[ i ].Process( fElapsedTime );
	}
	BaseClass::Process( fElapsedTime );
}

void CDnQuestSummaryInfoDlg::Render( float fElapsedTime )
{
	if( !IsShow() )
		return;
#if defined(PRE_ADD_INTEGERATE_QUEST_REPUT)
	CDnIntegrateQuestNReputationDlg* pIntegrateQuestNReputationDlg = (CDnIntegrateQuestNReputationDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::InTEGRATE_QUESTNREPUTATION_DIALOG );
	bool isShow = pIntegrateQuestNReputationDlg ? pIntegrateQuestNReputationDlg->IsEventTabShow(CDnIntegrateQuestNReputationDlg::QUEST_TAB) : false;
	if (isShow == false)
#else
	CEtUIDialog *pQuestDlg = (CEtUIDialog*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::QUEST_DIALOG );
	if( pQuestDlg && !pQuestDlg->IsShow() )
#endif // PRE_ADD_INTEGERATE_QUEST_REPUT
	{
		bool bClicked = false;

		int i;
		for( i = 0; i < DNNotifier::RegisterCount::Total; i++) {
			if( m_NotifierControls[ i ].IsMouseOver() ) {
				if( m_NotifierControls[ i ].IsClicked() ) {
					bClicked = true;
				}
				break;
			}
		}
		int nOverIndex = i;

	}

	BaseClass::Render( fElapsedTime );	

	for( int i = 0; i < DNNotifier::RegisterCount::Total; i++) {
		m_NotifierControls[ i ].PostRender();
	}
}

void CDnQuestSummaryInfoDlg::ClearAll()
{
	for( int i = 0; i < DNNotifier::RegisterCount::Total; i++) {
		m_NotifierControls[ i ].Show( false );
	}
}

static bool CompareQuestLevel( int QuestID_1, int QuestID_2 )
{
	Journal *pJournal_First = g_DataManager.GetJournalData( QuestID_1 );
	Journal *pJournal_Second = g_DataManager.GetJournalData( QuestID_2 );

	if(pJournal_First && pJournal_Second)
		if( pJournal_First->nQuestLevel < pJournal_Second->nQuestLevel ) return true;

	return false;
}

bool CDnQuestSummaryInfoDlg::IsCanProgressQuest(int nQuestIndex)
{
	int nArrowState = ArrowType::Hide;
	int nMapIndex = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
	TQuest *QuestInfo = NULL;

	std::vector<TQuest*> vecProgSubQuest;
	GetQuestTask().GetPlaySubQuest( vecProgSubQuest ); // 현재 진행중인 퀘스트 리스트.

	bool bExistQuest = false;
	for(DWORD i=0;i<vecProgSubQuest.size();i++)
	{
		if(vecProgSubQuest[i]->nQuestID == nQuestIndex)
		{
			QuestInfo = vecProgSubQuest[i];
			bExistQuest = true;
		}
	}

	if(!bExistQuest || !QuestInfo)
		return false;

	Journal* pJournal = g_DataManager.GetJournalData( QuestInfo->nQuestID );
	if( !pJournal ) return false;	

	JournalPage* pJournalPage = pJournal->FindJournalPage( QuestInfo->cQuestJournal );
	if ( !pJournalPage ) return false;

	nArrowState = CDnNotifierControls::GetNotifyArrowState(QuestInfo->nQuestID,QuestInfo->cQuestJournal);

	PathResult	pathResult;	
	GetQuestTask().GetPathFinder()->GetPathOneWay(nMapIndex, pJournalPage->nDestnationMapIndex , pathResult );

	if( nMapIndex != pJournalPage->nDestnationMapIndex  )
	{
		if( nArrowState == ArrowType::NormalArrow && pathResult.empty() )
			nArrowState = ArrowType::Hide;
	}

	return (nArrowState == ArrowType::Hide) ? false : true;
}

void CDnQuestSummaryInfoDlg::RefreshPriortyNotifier()
{
	DNTableFileFormat*  pMapTable = GetDNTable( CDnTableDB::TMAP );
	if(!pMapTable)
		return;

	UINT nTraceQuestID = GetQuestTask().GetTraceQuestID();

	std::vector< boost::tuple<std::wstring, int, int> > vecGetList;
	std::wstring szProgress;

	std::vector<int> vec_PriorityQuestList; // 그중에 좀더 우선순위를 가지는것
	vec_PriorityQuestList.clear();

	std::vector<int> vec_AddedQuestList;
	vec_AddedQuestList.clear();


	std::vector<TQuest*> vecProgSubQuest;
	GetQuestTask().GetPlaySubQuest( vecProgSubQuest ); // 현재 진행중인 퀘스트 리스트.

	for( int i = DNNotifier::RegisterCount::MainQuest; i < DNNotifier::RegisterCount::TotalQuest; i++ ) 
	{
		TQuest *QuestInfo = NULL;
		Journal *pJournal = GetQuestTask().GetNotifierJournal( i );
		JournalPage *pJournalPage = GetQuestTask().GetNotifierJournalPage( i );

		if( pJournal && pJournalPage && IsCanProgressQuest(pJournal->nQuestIndex))
		{
			if( pMapTable->GetFieldFromLablePtr( pJournalPage->nDestnationMapIndex , "_MapType" )->GetInteger() == CDnWorld::MapTypeDungeon )
				vec_PriorityQuestList.push_back(pJournal->nQuestIndex);
			else
				vec_AddedQuestList.push_back(pJournal->nQuestIndex);
		}
	}

	int nRemainSize = (int)(vec_PriorityQuestList.size() + vec_AddedQuestList.size());
	if( nRemainSize < DNNotifier::RegisterCount::SubQuest)
	{
		Journal* pJournal = NULL;
		JournalPage* pJournalPage = NULL;

		for(DWORD j=0;j<vecProgSubQuest.size();j++) // 지금 현재 받은 퀘스트 전부를 검색
		{
			bool bPassIndex = true;

			// 이미 리스트에 등록된 경우는 스킵
			for(DWORD k=0; k<vec_PriorityQuestList.size(); k++) 
			{
				if(vecProgSubQuest[j]->nQuestID == vec_PriorityQuestList[k])
				{
					bPassIndex = false;
					break;
				}
			}
			for(DWORD k=0; k<vec_AddedQuestList.size(); k++)
			{
				if(vecProgSubQuest[j]->nQuestID == vec_AddedQuestList[k])
				{
					bPassIndex = false;
					break;
				}
			}
			//

			if(bPassIndex && vecProgSubQuest[j])
			{
				pJournal = g_DataManager.GetJournalData( vecProgSubQuest[j]->nQuestID );
				if(pJournal)
					pJournalPage = pJournal->FindJournalPage( vecProgSubQuest[j]->cQuestJournal );

				if(pJournal && pJournalPage && IsCanProgressQuest(pJournal->nQuestIndex))
				{
					if( pMapTable->GetFieldFromLablePtr( pJournalPage->nDestnationMapIndex , "_MapType" )->GetInteger() == CDnWorld::MapTypeDungeon )
						vec_PriorityQuestList.push_back(pJournal->nQuestIndex);
					else
						vec_AddedQuestList.push_back(pJournal->nQuestIndex);
				}
			}
		}
	}

	// 최우선 리스트를 작성한뒤 그것을 레벨별로 소팅해준다.
	if( vec_PriorityQuestList.size() > 1) 	std::sort( vec_PriorityQuestList.begin() , vec_PriorityQuestList.end() , CompareQuestLevel );

	// 최우선 리스트를 제외한 추가적인 리스트를 작성한후 소팅해주고 // 최우선 리스트의 마지막에 푸쉬해준다.
	if(!vec_AddedQuestList.empty())
	{
		if( vec_AddedQuestList.size() > 1 ) std::sort( vec_AddedQuestList.begin() , vec_AddedQuestList.end() , CompareQuestLevel );

		for(DWORD i=0; i<vec_AddedQuestList.size(); i++)
		{
			vec_PriorityQuestList.push_back(vec_AddedQuestList[i]); // 통합 우선순위가 큰게 먼저 정렬됨.
			if(vec_PriorityQuestList.size() >= DNNotifier::RegisterCount::SubQuest)
				break;
		}
	}

	for( DWORD i = DNNotifier::RegisterCount::MainQuest; i < DNNotifier::RegisterCount::TotalQuest ; i++ ) // 마지막으로 정렬된 리스트를 뿌려줌.
	{ 
		TQuest *QuestInfo = NULL;
		Journal* pJournal = NULL;
		JournalPage* pJournalPage = NULL;
		int nArrayIndex = i - DNNotifier::RegisterCount::MainQuest;

		if( (int)vec_PriorityQuestList.size() <= nArrayIndex)
			break;

		for(DWORD j=0;j<vecProgSubQuest.size();j++)
		{
			if(vecProgSubQuest[j]->nQuestID == vec_PriorityQuestList[nArrayIndex])
			{
				QuestInfo = vecProgSubQuest[j];
				break;
			}
		}

		if(QuestInfo)
			pJournal = g_DataManager.GetJournalData( QuestInfo->nQuestID );

		if(pJournal )
			pJournalPage = pJournal->FindJournalPage( QuestInfo->cQuestJournal );

		if( pJournal && pJournalPage )
		{
			vecGetList.clear();
			szProgress.clear();
			PathResult	pathResult;		
			GetQuestTask().GetPathFinder()->GetPathOneWay(CGlobalInfo::GetInstance().m_nCurrentMapIndex, pJournalPage->nDestnationMapIndex , pathResult );

			GetQuestTask().GetCurJournalCountingInfoString( pJournal->nQuestIndex, pJournalPage->nJournalPageIndex, vecGetList, szProgress );
			m_NotifierControls[ i ].ClearQuestInfo();
			m_NotifierControls[ i ].SetQuestInfo(QuestInfo);
			m_NotifierControls[ i ].SetJournal( pJournal );
			m_NotifierControls[ i ].Show( true );
			m_NotifierControls[ i ].Refresh( pJournal->wszQuestTitle.c_str(), pJournalPage->szDestination.c_str(), vecGetList, szProgress );
			m_NotifierControls[ i ].UpdateDestinationMode();
			m_NotifierControls[ i ].SetMission( false );
			m_NotifierControls[ i ].SetPath(pathResult);
			if(pJournalPage->nDestnationMapIndex != CGlobalInfo::GetInstance().m_nCurrentMapIndex)
				m_NotifierControls[ i ].ChangeArrowStateByPathCheck();

			if( pJournal->nQuestIndex == nTraceQuestID )
				m_NotifierControls[ i ].SetTraceQuest( true );
			else
				m_NotifierControls[ i ].SetTraceQuest( false );

			m_renderIndices.push_back( i );
		}
	}
}

void CDnQuestSummaryInfoDlg::RefreshMainNotifier()
{
	std::vector< boost::tuple<std::wstring, int, int> > vecGetList;
	std::wstring szProgress;

	UINT nTraceQuestID = GetQuestTask().GetTraceQuestID();

	if( GetQuestTask().IsMainQuestNotifierExist() ) {
		Journal *pJournal = GetQuestTask().GetNotifierJournal( 0 );
		JournalPage *pJournalPage = GetQuestTask().GetNotifierJournalPage( 0 );
		if( pJournal && pJournalPage ) 
		{
			GetQuestTask().GetCurJournalCountingInfoString( pJournal->nQuestIndex, pJournalPage->nJournalPageIndex, vecGetList, szProgress );
			m_NotifierControls[ 0 ].ClearQuestInfo();
			m_NotifierControls[ 0 ].SetJournal( pJournal );
			m_NotifierControls[ 0 ].Show( true );
			m_NotifierControls[ 0 ].Refresh( pJournal->wszQuestTitle.c_str(), pJournalPage->szDestination.c_str(), vecGetList, szProgress );
			m_NotifierControls[ 0 ].UpdateDestinationMode();
			m_NotifierControls[ 0 ].SetMission( false );
			m_NotifierControls[ 0 ].SetPath( GetQuestTask().GetNotifierInfo(0).pathResult );
			if(pJournalPage->nDestnationMapIndex != CGlobalInfo::GetInstance().m_nCurrentMapIndex)
				m_NotifierControls[ 0 ].ChangeArrowStateByPathCheck();

			m_renderIndices.push_back( 0 );

			if( pJournal->nQuestIndex == nTraceQuestID )
				m_NotifierControls[ 0 ].SetTraceQuest( true );
			else
				m_NotifierControls[ 0 ].SetTraceQuest( false );
		}
		else
		{
			const TQuest* pQuest = GetQuestTask().GetQuestIndex( GetQuestTask().GetNotifierInfo(0).nQuestIndex );
			if( !(pQuest && pQuest->cQuestJournal == 0) )
				GetQuestTask().ClearNotifierData(0);
		}
	}

}
void CDnQuestSummaryInfoDlg::RefreshSubNotifier()
{
	std::vector< boost::tuple<std::wstring, int, int> > vecGetList;
	std::wstring szProgress;

	UINT nTraceQuestID = GetQuestTask().GetTraceQuestID();

	for( int i = DNNotifier::RegisterCount::MainQuest; i < DNNotifier::RegisterCount::TotalQuest; i++ ) {
		Journal *pJournal = GetQuestTask().GetNotifierJournal( i );
		JournalPage *pJournalPage = GetQuestTask().GetNotifierJournalPage( i );

		if( pJournal && pJournalPage )
		{
			vecGetList.clear();
			szProgress.clear();
			GetQuestTask().GetCurJournalCountingInfoString( pJournal->nQuestIndex, pJournalPage->nJournalPageIndex, vecGetList, szProgress );
			m_NotifierControls[ i ].ClearQuestInfo();
			m_NotifierControls[ i ].SetJournal( pJournal );
			m_NotifierControls[ i ].Show( true );
			m_NotifierControls[ i ].Refresh( pJournal->wszQuestTitle.c_str(), pJournalPage->szDestination.c_str(), vecGetList, szProgress );
			m_NotifierControls[ i ].UpdateDestinationMode();
			m_NotifierControls[ i ].SetMission( false );
			m_NotifierControls[ i ].SetPath( GetQuestTask().GetNotifierInfo( i ).pathResult );
			if(pJournalPage->nDestnationMapIndex != CGlobalInfo::GetInstance().m_nCurrentMapIndex)
				m_NotifierControls[ i ].ChangeArrowStateByPathCheck();

			m_renderIndices.push_back( i );

			if( pJournal->nQuestIndex == nTraceQuestID )
				m_NotifierControls[ i ].SetTraceQuest( true );
			else
				m_NotifierControls[ i ].SetTraceQuest( false );
		}
		else
		{
			const TQuest* pQuest = GetQuestTask().GetQuestIndex( GetQuestTask().GetNotifierInfo(i).nQuestIndex );
			if( pQuest && pQuest->cQuestJournal == 0 )
				continue;
			GetQuestTask().ClearNotifierData( i );
		}
	}
}

void CDnQuestSummaryInfoDlg::RefreshMissionNotifier()
{
	for( int i = 0; i < DNNotifier::RegisterCount::TotalMission; i++ ) {
#ifdef PRE_MOD_MISSION_HELPER
		CDnMissionTask::MissionInfoStruct *pMissionStruct = GetMissionTask().GetMissionInfoForNotifier( i );
#else
		CDnMissionTask::MissionInfoStruct *pMissionStruct = GetMissionTask().GetMissionInfoForNotifier( CDnMissionTask::DailyMissionTypeEnum(i) );
#endif
		int nIndex = DNNotifier::RegisterCount::TotalQuest + i;
		if( pMissionStruct )
		{
			m_NotifierControls[ nIndex ].Show( true );

#ifdef PRE_MOD_MISSION_HELPER
			std::wstring szProgress;
			if( pMissionStruct->nArrayIndex == -1 &&
				( eDailyMissionType::DAILYMISSION_DAY == pMissionStruct->nType || eDailyMissionType::DAILYMISSION_WEEK == pMissionStruct->nType) )
				szProgress = FormatW( L"(%d/%d)", pMissionStruct->nCounter, pMissionStruct->nRewardCounter );

			std::vector< boost::tuple<std::wstring, int, int> > vecGetList;
			std::wstring wszCategory;
			if( pMissionStruct->nArrayIndex != -1 )
			{
				switch( pMissionStruct->MainCategory )
				{
				case CDnMissionTask::Normal: wszCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7109 ); break;
				case CDnMissionTask::Dungeon: wszCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7110 ); break;
				case CDnMissionTask::Battle: wszCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9745 ); break;
				}
			}
			else if( pMissionStruct->nType == eDailyMissionType::DAILYMISSION_GUILDCOMMON )
			{
				wszCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000029520 );
			}
			else
			{
				wszCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9739 );
			}
			vecGetList.push_back( boost::make_tuple(wszCategory, -1, -1) );

			std::wstring wszSubCategory;
			if( pMissionStruct->nArrayIndex != -1 )
			{
				for( int j = 0;; j++ )
				{
					std::string strValue = _GetSubStrByCountSafe( j, (char*)pMissionStruct->szSubCategory.c_str(), '/' );
					if( strValue.size() == 0 ) break;
					if( j > 0 ) wszSubCategory += L"/";
					int nCategoryID = atoi( strValue.c_str() );
					wszSubCategory += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nCategoryID );
				}
			}
			else
			{
				switch( pMissionStruct->nType )
				{
				case eDailyMissionType::DAILYMISSION_DAY: wszSubCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9740 ); break;
				case eDailyMissionType::DAILYMISSION_WEEK: wszSubCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9741 ); break;
				case eDailyMissionType::DAILYMISSION_GUILDWAR: wszSubCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126187 ); break;
				case eDailyMissionType::DAILYMISSION_GUILDCOMMON: wszSubCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000029520 ); break;
				case eDailyMissionType::DAILYMISSION_WEEKENDEVENT: wszSubCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 10077 ); break;
				case eDailyMissionType::DAILYMISSION_WEEKENDREPEAT: wszSubCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 10079 ); break;
#ifdef PRE_ADD_MONTHLY_MISSION
				case eDailyMissionType::DAILYMISSION_MONTH: wszSubCategory = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 10167 ); break;
#endif 
				}
			}
			vecGetList.push_back( boost::make_tuple(wszSubCategory, -1, -1) );

			m_NotifierControls[ nIndex ].SetMission( true );
			m_NotifierControls[ nIndex ].Refresh( pMissionStruct->szTitle.c_str(), pMissionStruct->szSubTitle.c_str(), vecGetList, szProgress );
			m_renderIndices.push_back( nIndex );
#else
			std::wstring szProgress = FormatW( L"(%d/%d)", pMissionStruct->nCounter, pMissionStruct->nRewardCounter );

			m_NotifierControls[ nIndex ].SetMission( true );
			m_NotifierControls[ nIndex ].Refresh( pMissionStruct->szTitle.c_str(), L"", std::vector< boost::tuple<std::wstring, int, int> >(), szProgress );			
			m_renderIndices.push_back( nIndex );
#endif
		}
	}
}

void CDnQuestSummaryInfoDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();
	RefreshNotifier();
}

void CDnQuestSummaryInfoDlg::RefreshNotifier()
{
	if( !CDnQuestTask::IsActive() ) return;

#ifdef PRE_OPT_CLIENT_QUEST_SCRIPT
	if(g_DataManager.GetJournalList().size() == 0)
		return;
#endif

	ClearAll();
	m_renderIndices.clear();

	RefreshMainNotifier();

	if( m_bPriorityMode )
		RefreshPriortyNotifier();
	else
		RefreshSubNotifier();

	RefreshMissionNotifier();

	bool bBeforeMission = false;
	float fPosY = 0.f;
	std::vector< int >::iterator it = m_renderIndices.begin();
	for( ; it != m_renderIndices.end(); ++it ) {
		if( m_NotifierControls[ *it ].m_bMission ) {
			if( !bBeforeMission && *it == 5 ) {
				fPosY += 0.5f;
			}
			bBeforeMission = true;
		}
		m_NotifierControls[ *it ].MoveY( fPosY  );	
		fPosY += 1.0f;
	}
}

bool CDnQuestSummaryInfoDlg::FindControl( std::vector< CEtUIControl* > &vecControl, int nTypeCount, UI_CONTROL_TYPE *pType, bool bCheckCoveredControl, std::vector<SUICoord> &vecDlgCoord )
{
	// 퀘스트 알림이의 경우 버튼으로 이뤄져있지 않기때문에, 직접 추가해야한다.
	CEtUIControl *pControl(NULL);
	for( int i = 0; i < DNNotifier::RegisterCount::Total; i++) {
		if( m_NotifierControls[ i ].IsShow() ) {
			pControl = m_NotifierControls[ i ].m_Board;

			bool bPushControl = true;
			if( bCheckCoveredControl ) {
				// 일반적인 컨트롤로 생각해 가운데 지점이 다른 창에 가려져있는지 확인한다.
				SUICoord ControlCoord;
				pControl->GetUICoord( ControlCoord );
				ControlCoord.fX = ControlCoord.fX + ControlCoord.fWidth/2.0f;
				ControlCoord.fY = ControlCoord.fY + ControlCoord.fHeight/2.0f;
				ControlCoord = DlgCoordToScreenCoord( ControlCoord );

				for( int k = 0; k < (int)vecDlgCoord.size(); ++k )
				{
					if( ControlCoord.fX >= vecDlgCoord[k].fX && ControlCoord.fY >= vecDlgCoord[k].fY &&
						ControlCoord.fX <= vecDlgCoord[k].fX+vecDlgCoord[k].fWidth && ControlCoord.fY <= vecDlgCoord[k].fY+vecDlgCoord[k].fHeight )
					{
						bPushControl = false;
						break;
					}
				}
			}

			if( bPushControl )
				vecControl.push_back( pControl );
		}
	}

	return CEtUIDialog::FindControl( vecControl, nTypeCount, pType, bCheckCoveredControl, vecDlgCoord );
}

void CDnQuestSummaryInfoDlg::Show(bool bShow)
{
	CEtUIDialog::Show(bShow);
}