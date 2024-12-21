#include "StdAfx.h"
#include "DnMissionAppellationTooltipDlg.h"
#include "DnItemTask.h"
#include "DnAppellationTask.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMissionAppellationTooltipDlg::CDnMissionAppellationTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
, m_pDescription(NULL)
, m_nAppellationIndex(-1)
{
}

CDnMissionAppellationTooltipDlg::~CDnMissionAppellationTooltipDlg(void)
{
}

void CDnMissionAppellationTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PopupAppellationDlg.ui" ).c_str(), bShow );
}

void CDnMissionAppellationTooltipDlg::InitialUpdate()
{
	m_pDescription = GetControl<CEtUITextBox>( "ID_STATIC1" );
}

void CDnMissionAppellationTooltipDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow ) return;
	if( bShow ) {
		m_pDescription->ClearText();
		if( m_nAppellationIndex != -1 && CDnAppellationTask::IsActive() ) {
			// nAppellationIndex 가 ItemID 로 받아놯기때문에 MaekDescription 은 nArrayIndex 로 받아서.. 다시 백해서 넣어준다.
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );
			int nItemID = pSox->GetArrayIndex( m_nAppellationIndex );
			tstring wszDesc = CDnAppellationTask::GetInstance().MakeDescription( nItemID );
			m_pDescription->SetText(wszDesc.c_str());
		}
	}
	CEtUIDialog::Show( bShow );
}

void CDnMissionAppellationTooltipDlg::SetAppellationInfo( int nItemID )
{
	m_nAppellationIndex = nItemID;
}