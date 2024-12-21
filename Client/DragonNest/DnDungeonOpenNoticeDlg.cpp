#include "StdAfx.h"
#include "DnDungeonOpenNoticeDlg.h"
#include "TaskManager.h"
#include "DnCommonTask.h"
#include "DnTableDB.h"
#include "DnInterface.h"

#define SHOWSTART_DELAY 0.7f
#define SHOWEND_DELAY 5.f
#define SHOWELEMENT_DELTA 0.3f
#define SHOWELEMENT_NEXTDELTA 0.2f

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnDungeonOpenNoticeDlg::CDnDungeonOpenNoticeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	m_pStaticTitle = NULL;
	m_pStaticTitleName = NULL;
	memset( m_pStaticBack, 0, sizeof(m_pStaticBack) );
	memset( m_pStaticAbyssBack, 0, sizeof(m_pStaticAbyssBack) );
	memset( m_pStaticName, 0, sizeof(m_pStaticName) );
	memset( m_pStaticAbyssName, 0, sizeof(m_pStaticAbyssName) );
	memset( m_pStaticMapType, 0, sizeof(m_pStaticMapType) );
	memset( m_CoordBack, 0, sizeof(m_CoordBack) );
	memset( m_CoordAbyssBack, 0, sizeof(m_CoordAbyssBack) );
	memset( m_CoordName, 0, sizeof(m_CoordName) );
	memset( m_CoordAbyssName, 0, sizeof(m_CoordAbyssName) );
	memset( m_CoordMapType, 0, sizeof(m_CoordMapType) );

	m_fDelta = 0.f;
	m_fHideDeltaOffset = 0.f;
	m_fMaxDelta = 0.f;
	m_nShowElementCount = 0;
}

CDnDungeonOpenNoticeDlg::~CDnDungeonOpenNoticeDlg()
{
	SAFE_DELETE_PVEC( m_pVecDungeonOpenList );
}

void CDnDungeonOpenNoticeDlg::InitialUpdate()
{
	m_pStaticTitle = GetControl<CEtUIStatic>( "ID_STATIC_MAIN" );
	m_CoordTitle = m_pStaticTitle->GetUICoord();
	m_pStaticTitleName = GetControl<CEtUIStatic>( "ID_TEXT_MAIN" );
	m_CoordTitleName = m_pStaticTitleName->GetUICoord();

	char szStr[64];
	for( int i=0; i<6; i++ ) {
		sprintf_s( szStr, "ID_STATIC_SUB%d", i + 1 );
		m_pStaticBack[i] = GetControl<CEtUIStatic>( szStr );
		m_CoordBack[i] = m_pStaticBack[i]->GetUICoord();

		sprintf_s( szStr, "ID_STATIC_ABYSS%d", i + 1 );
		m_pStaticAbyssBack[i] = GetControl<CEtUIStatic>( szStr );
		m_CoordAbyssBack[i] = m_pStaticAbyssBack[i]->GetUICoord();

		sprintf_s( szStr, "ID_TEXT_SUB%d", i + 1 );
		m_pStaticName[i] = GetControl<CEtUIStatic>( szStr );
		m_CoordName[i] = m_pStaticName[i]->GetUICoord();

		sprintf_s( szStr, "ID_TEXT_ABYSS%d", i + 1 );
		m_pStaticAbyssName[i] = GetControl<CEtUIStatic>( szStr );
		m_CoordAbyssName[i] = m_pStaticAbyssName[i]->GetUICoord();

		for( int j=0; j<CDnWorld::LandEnvironment_Amount-1; j++ ) {
			sprintf_s( szStr, "ID_STATIC_MARK%d_%d", i+1, j+1 );
			m_pStaticMapType[i][j] = GetControl<CEtUIStatic>( szStr );
			m_CoordMapType[i][j] = m_pStaticMapType[i][j]->GetUICoord();
		}
	}
}

void CDnDungeonOpenNoticeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DungeonNoticeDlg.ui" ).c_str(), bShow );
}

void CDnDungeonOpenNoticeDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnDungeonOpenNoticeDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !m_bShow ) return;
	m_fDelta -= fElapsedTime;
	if( m_fDelta < 0.f ) {
		m_fDelta = 0.f;
		if( !m_pVecDungeonOpenList.empty() ) {
			UpdateInfo( m_pVecDungeonOpenList[0] );
			SAFE_DELETE( m_pVecDungeonOpenList[0] );
			m_pVecDungeonOpenList.erase( m_pVecDungeonOpenList.begin() );
		}
		else Show( false );
	}
	else {
		float fRatio;
		float fTemp;
		SUICoord Coord, DlgCoord;

		GetDlgCoord( DlgCoord );
		float fWidthEnd = ( DlgCoord.fWidth - DlgCoord.fX );

		if( ( m_fMaxDelta - m_fDelta ) <= m_fHideDeltaOffset ) {
			fRatio = 1.f - GetWeightValue( 0.f, SHOWELEMENT_NEXTDELTA, ( m_fMaxDelta - m_fDelta ) );
		}
		else {
			fRatio = GetWeightValue( m_fMaxDelta - SHOWELEMENT_NEXTDELTA, m_fMaxDelta, ( m_fMaxDelta - m_fDelta ) );
		}
		Coord = m_CoordTitle;
		Coord.fX = m_CoordTitle.fX + ( ( fWidthEnd - m_CoordTitle.fX ) * fRatio );
		m_pStaticTitle->SetUICoord( Coord );
		Coord = m_CoordTitleName;
		Coord.fX = m_CoordTitleName.fX + ( ( fWidthEnd - m_CoordTitleName.fX ) * fRatio );
		m_pStaticTitleName->SetUICoord( Coord );

		for( int i=0; i<m_nShowElementCount; i++ ) {
			if( ( m_fMaxDelta - m_fDelta ) <= m_fHideDeltaOffset ) {
				fTemp = SHOWSTART_DELAY + ( i * SHOWELEMENT_NEXTDELTA );
				fRatio = 1.f - GetWeightValue( fTemp, fTemp + SHOWELEMENT_DELTA, ( m_fMaxDelta - m_fDelta ) );
			}
			else {
				fTemp = m_fMaxDelta - SHOWELEMENT_DELTA - ( ( i + 1 ) * SHOWELEMENT_NEXTDELTA );
				fRatio = GetWeightValue( fTemp, fTemp + SHOWELEMENT_DELTA, ( m_fMaxDelta - m_fDelta ) );
			}

			Coord = m_CoordBack[i];
			Coord.fX = m_CoordBack[i].fX + ( ( fWidthEnd - m_CoordBack[i].fX ) * fRatio );
			m_pStaticBack[i]->SetUICoord( Coord );

			Coord = m_CoordAbyssBack[i];
			Coord.fX = m_CoordAbyssBack[i].fX + ( ( fWidthEnd - m_CoordAbyssBack[i].fX ) * fRatio );
			m_pStaticAbyssBack[i]->SetUICoord( Coord );

			Coord = m_CoordName[i];
			Coord.fX = m_CoordName[i].fX + ( ( fWidthEnd - m_CoordName[i].fX ) * fRatio );
			m_pStaticName[i]->SetUICoord( Coord );

			Coord = m_CoordAbyssName[i];
			Coord.fX = m_CoordAbyssName[i].fX + ( ( fWidthEnd - m_CoordAbyssName[i].fX ) * fRatio );
			m_pStaticAbyssName[i]->SetUICoord( Coord );

			for( int j=0; j<CDnWorld::LandEnvironment_Amount-1; j++ ) {
				Coord = m_CoordMapType[i][j];
				Coord.fX = m_CoordMapType[i][j].fX + ( ( fWidthEnd - m_CoordMapType[i][j].fX ) * fRatio );
				m_pStaticMapType[i][j]->SetUICoord( Coord );
			}
		}
	}
}

void CDnDungeonOpenNoticeDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	if( bShow ) {
		if( RefreshInfo() == false ) bShow = false;
	}

	CEtUIDialog::Show( bShow );
}


void CDnDungeonOpenNoticeDlg::UpdateInfo( DungeonOpenStruct *pInfo )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	m_nShowElementCount = (int)pInfo->nVecMapID.size();

	SUICoord Coord;
	std::wstring wszStr = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 930 );
	int nChatStringID;
	switch( pInfo->Type ) {
		case CDnCommonTask::OpenWorldMap: wszStr += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 932 ); nChatStringID = 934; break;
		case CDnCommonTask::OpenDungeon: wszStr += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 933 ); nChatStringID = 935; break;
		case CDnCommonTask::OpenAbyss: wszStr += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 931 ); nChatStringID = 936; break;
	}
	m_pStaticTitleName->SetText( wszStr );

	WCHAR wszTemp[256];
	for( DWORD i=0; i<pInfo->nVecMapID.size(); i++ ) {
		int nMapNameID = pSox->GetFieldFromLablePtr( pInfo->nVecMapID[i], "_MapNameID" )->GetInteger();
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nChatStringID ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMapNameID ) );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
	}

	for( int i=0; i<6; i++ ) {
		m_pStaticBack[i]->Show( false );
		m_pStaticAbyssBack[i]->Show( false );
		m_pStaticName[i]->Show( false );
		m_pStaticAbyssName[i]->Show( false );
		for( int j=0; j<CDnWorld::LandEnvironment_Amount-1; j++ ) {
			m_pStaticMapType[i][j]->Show( false );
		}

		Coord = m_CoordBack[i];
		Coord.fX = 1.f;
		m_pStaticBack[i]->SetUICoord( Coord );

		Coord = m_CoordAbyssBack[i];
		Coord.fX = 1.f;
		m_pStaticAbyssBack[i]->SetUICoord( Coord );

		Coord = m_CoordName[i];
		Coord.fX = 1.f;
		m_pStaticName[i]->SetUICoord( Coord );

		Coord = m_CoordAbyssName[i];
		Coord.fX = 1.f;
		m_pStaticAbyssName[i]->SetUICoord( Coord );

		for( int j=0; j<CDnWorld::LandEnvironment_Amount-1; j++ ) {
			Coord = m_CoordMapType[i][j];
			Coord.fX = 1.f;
			m_pStaticMapType[i][j]->SetUICoord( Coord );
		}
	}

	int nOffset = 0;
	if( m_nShowElementCount > 6 ) m_nShowElementCount = 6;
	for( int i=0; i<m_nShowElementCount; i++ ) {
		int nMapID = pInfo->nVecMapID[i];
		int nEnvironment = pSox->GetFieldFromLablePtr( nMapID, "_Environment" )->GetInteger();

		CEtUIStatic *pShowBack = NULL;
		CEtUIStatic *pShowName = NULL;
		CEtUIStatic *pShowIcon = NULL;
		std::wstring wszName;
		int nMapNameID = pSox->GetFieldFromLablePtr( nMapID, "_MapNameID" )->GetInteger();
		wszName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMapNameID );
		switch( pInfo->Type ) {
			case CDnCommonTask::OpenWorldMap:
				pShowBack = m_pStaticBack[nOffset];
				pShowName = m_pStaticName[nOffset];
				break;
			case CDnCommonTask::OpenDungeon:
				pShowBack = m_pStaticBack[nOffset];
				pShowName = m_pStaticName[nOffset];
				if( nEnvironment > 0 ) {
					int nIcon = nEnvironment-1;
					if( nIcon >= CDnWorld::LandEnvironment_Amount - 1 ) nIcon = CDnWorld::LandEnvironment_Amount - 2;
					pShowIcon = m_pStaticMapType[nOffset][nIcon];
				}
				break;
			case CDnCommonTask::OpenAbyss:
				pShowBack = m_pStaticAbyssBack[nOffset];
				pShowName = m_pStaticAbyssName[nOffset];
//				wszName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000 );
				break;
		}
		nOffset++;

		pShowBack->Show( true );
		pShowName->Show( true );
		pShowName->SetText( wszName );
		if( pShowIcon ) pShowIcon->Show( true );
	}
	m_nShowElementCount = nOffset;
	m_fHideDeltaOffset = SHOWSTART_DELAY + SHOWEND_DELAY + ( m_nShowElementCount * SHOWELEMENT_NEXTDELTA );
	m_fDelta = m_fHideDeltaOffset + ( ( m_nShowElementCount + 1 ) * SHOWELEMENT_NEXTDELTA ) + SHOWELEMENT_DELTA;
	m_fMaxDelta = m_fDelta;
}

bool CDnDungeonOpenNoticeDlg::RefreshInfo()
{
	if( !CTaskManager::IsActive() ) return false;
	CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
	if( !pTask ) return false;
	DWORD dwCount = pTask->GetDungeonOpenNoticeCount();
	if( dwCount == 0 ) return false;

	SAFE_DELETE_PVEC( m_pVecDungeonOpenList );

	DungeonOpenStruct *pStruct[3];
	for( int i=0; i<3; i++ ) {
		pStruct[i] = new DungeonOpenStruct;
		pStruct[i]->Type = (CDnCommonTask::DungeonOpenNoticeTypeEnum)i;
	}

	for( DWORD i=0; i<dwCount; i++ ) {
		CDnCommonTask::DungeonOpenNoticeStruct *pData = pTask->GetDungeonOpenNoticeData(i);
		pStruct[pData->Type]->nVecMapID.push_back( pData->nMapID );
	}
	for( int i=0; i<3; i++ ) {
		if( !pStruct[i]->nVecMapID.empty() ) {
			m_pVecDungeonOpenList.push_back( pStruct[i] );
		}
		else SAFE_DELETE( pStruct[i] );
	}
	if( m_pVecDungeonOpenList.empty() ) return false;

	UpdateInfo( m_pVecDungeonOpenList[0] );
	SAFE_DELETE( m_pVecDungeonOpenList[0] );
	m_pVecDungeonOpenList.erase( m_pVecDungeonOpenList.begin() );
	return true;
}

float CDnDungeonOpenNoticeDlg::GetWeightValue( float fStartDelta, float fEndDelta, float fCurDelta )
{
	if( fCurDelta <= fStartDelta ) return 0.f;
	if( fCurDelta >= fEndDelta ) return 1.f;

	float fWeight = ( 1.f / ( fEndDelta - fStartDelta ) ) * ( fCurDelta - fStartDelta );
	if( fWeight < 0.5f ) {
		fWeight += fWeight;
		fWeight = abs( cos( EtToRadian( 90.f * fWeight ) ) - 1.f ) * 0.5f;
	}
	else {
		fWeight -= 0.5f;
		fWeight += fWeight;

		fWeight = 0.5f + ( abs( cos( EtToRadian( 90.f + ( 90.f * fWeight ) ) ) ) * 0.5f );
	}
	return fWeight;
}