#include "Stdafx.h"
#include "DnMasterJoinDlg.h"
#include "DnMasterJoinListDlg.h"
#include "DnMasterTask.h"
#include "DnInterfaceString.h"
#include "DnLocalPlayerActor.h"
#include "DnTableDB.h"

CDnMasterJoinDlg::CDnMasterJoinDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pListBoxEx( NULL )
, m_nCurSelect( 0 )
{
}

CDnMasterJoinDlg::~CDnMasterJoinDlg()
{
}

void CDnMasterJoinDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MasterJoinDlg.ui" ).c_str(), bShow );
}

void CDnMasterJoinDlg::InitialUpdate()
{
	m_pListBoxEx = GetControl<CEtUIListBoxEx>( "ID_LISTBOXEX_LIST" );
}

void CDnMasterJoinDlg::Show(bool bShow)
{
	CEtUIDialog::Show( bShow );
}

void CDnMasterJoinDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnMasterJoinDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_CLOSE" ) ) {
			Show( false );
		}
		else if( IsCmdControl( "ID_BUTTON_CANCEL" ) ) {
			Show( false );
		}
		else if( IsCmdControl( "ID_BUTTON_OK" ) ) {
			const CDnMasterTask::SMasterList & MasterList = GetMasterTask().GetMasterList();
			GetMasterTask().RequestMasterJoin( MasterList.MasterInfoList[m_nCurSelect].biCharacterDBID, MasterList.MasterInfoList[m_nCurSelect].wszCharName.c_str() );
			Show( false );
		}
	}
}

void CDnMasterJoinDlg::RefreshInfo( int nCurSelect )
{
	m_pListBoxEx->RemoveAllItems();

	//CDnMasterJoinListDlg *pItem = m_pListBoxEx->InsertItem<CDnMasterJoinListDlg>(0);

	const CDnMasterTask::SMasterList & MasterList = GetMasterTask().GetMasterList();

	if( MasterList.cCount < nCurSelect )
		return;

	m_nCurSelect = nCurSelect;

	CONTROL( Static, ID_TEXT_TITLENAME )->SetText( MasterList.MasterInfoList[nCurSelect].wszSelfIntroduction );
	CONTROL( Static, ID_TEXT_NAME )->SetText( MasterList.MasterInfoList[nCurSelect].wszCharName );

	CONTROL( Static, ID_TEXT_CLASS )->SetText( DN_INTERFACE::STRING::GetJobString( MasterList.MasterInfoList[nCurSelect].cJobCode ) );

	CONTROL( Static, ID_TEXT_LEVEL )->SetIntToText( (int)MasterList.MasterInfoList[nCurSelect].cLevel );

	CONTROL( Static, ID_TEXT_RESPECT )->SetIntToText( MasterList.MasterInfoList[nCurSelect].iRespectPoint );

	CONTROL( Static, ID_TEXT_ENTER )->SetIntToText( (int)MasterList.MasterInfoList[nCurSelect].iGraduateCount );

#if !defined(PRE_MOD_MASTER_GENDER)
	if( 1 == MasterList.MasterInfoList[nCurSelect].cGenderCode )
	{
		CONTROL( Static, ID_TEXT_SEX )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7203 ) );
		CONTROL( Static, ID_TEXT_SEX )->Show( true );
	}
	else if( 2 == MasterList.MasterInfoList[nCurSelect].cGenderCode )
	{
		CONTROL( Static, ID_TEXT_SEX )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7204 ) );
		CONTROL( Static, ID_TEXT_SEX )->Show( true );
	}
	else
		CONTROL( Static, ID_TEXT_SEX )->Show( false );
#endif	// #if !defined(PRE_MOD_MASTER_GENDER)

	const SCPupilList * pPupilList = GetMasterTask().GetPupilList();

	if( NULL == pPupilList )
		return;

	CDnMasterJoinListDlg * pItem = NULL; 

	for( int itr = 0; itr < pPupilList->cCount; ++itr )
	{
		pItem = m_pListBoxEx->InsertItem<CDnMasterJoinListDlg>(0);

		pItem->SetInfo( pPupilList->PupilInfoList[itr].wszCharName, pPupilList->PupilInfoList[itr].cLevel, pPupilList->PupilInfoList[itr].cJob );
	}

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	int levelDelta = MasterList.MasterInfoList[nCurSelect].cLevel - localActor->GetLevel();

	if( levelDelta < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MasterSystem_MasterAndPupilAllowLevelGap)) ||
		GetMasterTask().IsMasterPlayer( MasterList.MasterInfoList[nCurSelect].wszCharName.c_str() ) != 0 )
		CONTROL( Button, ID_BUTTON_OK )->Enable( false );
	else
		CONTROL( Button, ID_BUTTON_OK )->Enable( true );
}
