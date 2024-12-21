#include "StdAfx.h"
#include "DnStaffrollDlg.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "TaskManager.h"
#include "DnLoginDlg.h"
#include "DnStaffrollFont1Dlg.h"
#include "DnStaffrollFont2Dlg.h"
#include "DnStaffrollFont3Dlg.h"
#include "DnCustomControlCommon.h"
#include "DnFadeInOutDlg.h"

CDnStaffrollDlg::CDnStaffrollDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
{
	m_bLoaded = false;
	m_bPlay = false;
	m_fDelta = 0.f;
	m_fTotalDelta = 0.f;
	m_LastLoginState = CDnLoginTask::RequestIDPass;
	m_pButtonStaffroll = NULL;
}

CDnStaffrollDlg::~CDnStaffrollDlg(void)
{
	FreeStaffrollData();
}

void CDnStaffrollDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StaffRollBTDlg.ui" ).c_str(), bShow );
	LoadStaffrollData();
}

void CDnStaffrollDlg::InitialUpdate()
{
	m_pButtonStaffroll = GetControl<CEtUIButton>( "ID_BT_STAFFROLL" );
}

void CDnStaffrollDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_STAFFROLL" ) )
		{
			if( !m_bPlay ) Play();
			else Stop();
		}
	}
}

void CDnStaffrollDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	if( !m_bPlay ) return;
	m_fDelta += fElapsedTime * 0.1f;
	if( m_fDelta >= m_fTotalDelta ) {
		Stop();
		return;
	}

	for( DWORD i=0; i<m_pVecElementList.size(); i++ ) {
		ElementStruct *pStruct = m_pVecElementList[i];

		bool bUpdate = false;
		if( m_fDelta >= pStruct->fOffset && m_fDelta <= pStruct->fOffset + pStruct->fHeight + 1.f ) bUpdate = true;

		pStruct->Update( bUpdate, m_fDelta );
	}
}

void CDnStaffrollDlg::Process( float fElapsedTime )
{
	if( m_bShow ) {
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
		if( pTask ) {
			bool bEnable = true;
			if( pTask->IsRequestWait() ) bEnable = false;
			if( GetInterface().GetFadeDlg() && GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd ) bEnable = false;
			m_pButtonStaffroll->Enable( bEnable );
		}
	}
	CEtUIDialog::Process( fElapsedTime );
}

void CDnStaffrollDlg::LoadStaffrollData()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSTAFFROLL );
	if( !pSox ) return;

	m_hVecFaceTextureList.resize(10);
	char szTemp[32];
	CFileNameString szFileName;
	bool bExistFile;
	for( int i=0; i<10; i++ ) {
		sprintf_s( szTemp, 32, "StaffRollFace%02d.dds", i+1 );
		szFileName = CEtResourceMng::GetInstance().GetFullName( szTemp, &bExistFile );
		if( !bExistFile ) continue;
		m_hVecFaceTextureList[i] = LoadResource( szFileName.c_str(), RT_TEXTURE );
	}

	float fHeight = 0.f;
	for( int i=0; i<pSox->GetItemCount(); i++ ) {
		int nItemID = pSox->GetItemID(i);

		ElementTypeEnum Type = (ElementTypeEnum)pSox->GetFieldFromLablePtr( nItemID, "_UIType" )->GetInteger();

		ElementStruct *pStruct = NULL;
		switch( Type ) {
			case ElementTypeEnum::Space: pStruct = new ElementStruct;	break;
			case ElementTypeEnum::Face:	 pStruct = new FaceElementStruct; break;
			case ElementTypeEnum::Font1: 
			case ElementTypeEnum::Font2:
			case ElementTypeEnum::Font3:
				pStruct = new FontElementStruct; 
				break;
		}
		if( !pStruct ) continue;

		pStruct->Type = Type;
		pStruct->szValue = pSox->GetFieldFromLablePtr( nItemID, "_Value" )->GetString();
		pStruct->fOffset = fHeight;
		pStruct->fHeight = 0.f;

		switch( Type ) {
			case ElementTypeEnum::Space:
				{
					int nSpace = atoi( pStruct->szValue.c_str() );
					pStruct->fHeight = nSpace / (float)GetEtDevice()->Height();
				}
				break;
			case ElementTypeEnum::Face:	 
				{
					std::vector<int> nVecList;
					for( int j=0; ; j++ ) {
						const char *pPtr = _GetSubStrByCount( j, (char*)pStruct->szValue.c_str(), ';' );
						if( !pPtr ) break;
						nVecList.push_back( atoi( pPtr ) );
					}
					std::vector<float> fVecWidthList;
					for( DWORD j=0; j<nVecList.size(); j++ ) {
						float fValue = ( FACE_TEXTURE_ICON_XSIZE + ( 20 * (int)( nVecList.size() - 1 ) ) ) / (float)GetEtDevice()->Width();
						fVecWidthList.push_back( 0.5f - ( fValue * (int)( nVecList.size() - 1 ) / 2.f ) + ( fValue * j ) );
					}

					FaceElementStruct *pFaceElement = (FaceElementStruct *)pStruct;

					SUICoord UVCoord;
					EtTextureHandle hTexture = GetFaceTexture( nVecList[0], UVCoord );
					pFaceElement->SetTexture( hTexture, UVCoord );

					pFaceElement->fHeight = FACE_TEXTURE_ICON_YSIZE / (float)GetEtDevice()->Height();

					pFaceElement->Coord.fWidth = FACE_TEXTURE_ICON_XSIZE / (float)GetEtDevice()->Width();
					pFaceElement->Coord.fHeight = FACE_TEXTURE_ICON_YSIZE / (float)GetEtDevice()->Height();
					pFaceElement->Coord.fX = fVecWidthList[0] - ( pFaceElement->Coord.fWidth / 2.f );
					pFaceElement->Coord.fY = 1.f;

					for( DWORD j=1; j<nVecList.size(); j++ ) {
						FaceElementStruct *pSubElement = new FaceElementStruct;
						pSubElement->Type = Type;
						pSubElement->fOffset = pStruct->fOffset;


						hTexture = GetFaceTexture( nVecList[j], UVCoord );
						pSubElement->SetTexture( hTexture, UVCoord );

						pSubElement->fHeight = FACE_TEXTURE_ICON_YSIZE / (float)GetEtDevice()->Height();

						pSubElement->Coord.fWidth = FACE_TEXTURE_ICON_XSIZE / (float)GetEtDevice()->Width();
						pSubElement->Coord.fHeight = FACE_TEXTURE_ICON_YSIZE / (float)GetEtDevice()->Height();
						pSubElement->Coord.fX = fVecWidthList[j] - ( pSubElement->Coord.fWidth / 2.f );
						pSubElement->Coord.fY = 1.f;
						m_pVecElementList.push_back( pSubElement );
					}
				}
				break;
			case ElementTypeEnum::Font1:
				{
					((FontElementStruct*)pStruct)->pFontDlg = new CDnStaffrollFont1Dlg;
					((FontElementStruct*)pStruct)->pFontDlg->Initialize( false );

					CalcFontElement( ((FontElementStruct*)pStruct)->pFontDlg, pStruct );
				}
				break;
			case ElementTypeEnum::Font2:
				{
					((FontElementStruct*)pStruct)->pFontDlg = new CDnStaffrollFont2Dlg;
					((FontElementStruct*)pStruct)->pFontDlg->Initialize( false );

					CalcFontElement( ((FontElementStruct*)pStruct)->pFontDlg, pStruct );
				}
				break;
			case ElementTypeEnum::Font3:
				{
					((FontElementStruct*)pStruct)->pFontDlg = new CDnStaffrollFont3Dlg;
					((FontElementStruct*)pStruct)->pFontDlg->Initialize( false );

					CalcFontElement( ((FontElementStruct*)pStruct)->pFontDlg, pStruct );
				}
				break;
		}
		m_pVecElementList.push_back( pStruct );
		fHeight += pStruct->fHeight;
	}
	m_fTotalDelta = fHeight + 1.f;
	
	m_bLoaded = true;
}

void CDnStaffrollDlg::FreeStaffrollData()
{
	SAFE_RELEASE_SPTRVEC( m_hVecFaceTextureList );
	SAFE_DELETE_PVEC( m_pVecElementList );
	m_bLoaded = false;
}

wstring CDnStaffrollDlg::GetParseString( const char *szStr )
{
	wstring wszStr;

	for( int i=0; ; i++ ) {
		const char *pPtr = _GetSubStrByCount( i, (char*)szStr, ';' );
		if( pPtr == NULL ) break;
		int nUIStringID = atoi( pPtr );
		if( i != 0 ) wszStr += L"  ";
		wszStr += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringID );
	}
	return wszStr;
}

void CDnStaffrollDlg::CalcFontElement( CEtUIDialog *pDlg, ElementStruct *pStruct )
{
	SUICoord Coord;

	CDnStaffrollFontBase *pFontBase = dynamic_cast<CDnStaffrollFontBase *>(pDlg);
	if( pFontBase ) pFontBase->SetText( GetParseString( pStruct->szValue.c_str() ).c_str() );
	pDlg->GetDlgCoord( Coord );
	pStruct->fHeight = Coord.fHeight;

	Coord.fX = 0.f;
	Coord.fY = 1.f;
	pDlg->SetDlgCoord( Coord );
}

EtTextureHandle CDnStaffrollDlg::GetFaceTexture( int nIconIndex, SUICoord &UVCoord )
{
	EtTextureHandle hIconTexture;
	int nIndex = nIconIndex / FACE_TEXTURE_ICON_COUNT;
	if( nIndex < (int)m_hVecFaceTextureList.size() && m_hVecFaceTextureList[ nIndex ] )
		hIconTexture = m_hVecFaceTextureList[ nIndex ];
	else
		hIconTexture = m_hVecFaceTextureList[0];
	nIconIndex %= FACE_TEXTURE_ICON_COUNT;
	CalcButtonUV( nIconIndex, hIconTexture, UVCoord, FACE_TEXTURE_ICON_XSIZE, FACE_TEXTURE_ICON_YSIZE );

	return hIconTexture;
}


void CDnStaffrollDlg::Play()
{
	CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	m_LastLoginState = pTask->GetState();
	switch( m_LastLoginState ) {
		case CDnLoginTask::Logo:
		case CDnLoginTask::RequestIDPass:
			GetInterface().GetLoginDlg()->Show( false );
			break;
		case CDnLoginTask::ServerList:
			GetInterface().ShowServerList( false );
			break;
	}

	m_bPlay = true;
	m_fDelta = 0.f;
}

void CDnStaffrollDlg::Stop()
{
	CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	switch( m_LastLoginState ) {
		case CDnLoginTask::Logo:
		case CDnLoginTask::RequestIDPass:
			GetInterface().GetLoginDlg()->Show( true );
			break;
		case CDnLoginTask::ServerList:
			GetInterface().ShowServerList( true );
			break;
	}

	ResetStaffrollDelta();
}

void CDnStaffrollDlg::ResetStaffrollDelta()
{
	m_bPlay = false;
	m_fDelta = 0.f;

	for( DWORD i=0; i<m_pVecElementList.size(); i++ ) {
		m_pVecElementList[i]->Update( false, 0.f );
	}
}

void CDnStaffrollDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	if( bShow ) {
		if( m_bPlay ) ResetStaffrollDelta();
	}
	else {
		if( m_bPlay ) ResetStaffrollDelta();
	}
	CEtUIDialog::Show( bShow );
}