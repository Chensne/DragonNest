#include "StdAfx.h"
#include "DnCharGestureDlg.h"
#include "DnTableDB.h"
#include "DnLoginTask.h"
#include "DnGestureButton.h"
#include "TaskManager.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 


#ifdef PRE_MOD_SELECT_CHAR

CDnCharGestureDlg::CDnCharGestureDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_bCharCreate( false )
{
	memset( m_pGestureSlotButton, NULL, sizeof(CDnLifeSkillButton*) * MAX_GESTURE );
	memset( m_pGestureText, NULL, sizeof(CEtUIStatic*) * MAX_GESTURE );
	memset( m_pGestureButton, NULL, sizeof(CEtUIButton*) * MAX_GESTURE );
}

CDnCharGestureDlg::~CDnCharGestureDlg(void)
{
	m_vecStrGestureActionName.clear();
}

void CDnCharGestureDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreate_CharGestureDlg.ui" ).c_str(), bShow );
}

void CDnCharGestureDlg::InitialUpdate()
{
	char szControlName[32];
	for( int i=0; i<MAX_GESTURE; ++i )
	{
		sprintf_s( szControlName, _countof(szControlName), "ID_BT_GESTURE%d", i );
		m_pGestureSlotButton[i] = GetControl<CDnLifeSkillButton>( szControlName );
		sprintf_s( szControlName, _countof(szControlName), "ID_TEXT_GESTURE%d", i );
		m_pGestureText[i] = GetControl<CEtUIStatic>( szControlName );
		sprintf_s( szControlName, _countof(szControlName), "ID_BT_GT%d", i );
		m_pGestureButton[i] = GetControl<CEtUIButton>( szControlName );
		m_pGestureButton[i]->SetButtonID( i );
	}

	SetGestureInfo();
}

void CDnCharGestureDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pTask ) return;

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BT_GT" ) )
		{
			int nSelectIndex = static_cast<CEtUIButton*>( pControl )->GetButtonID();
			pTask->CharCreateGestureAction( m_vecStrGestureActionName[nSelectIndex].c_str(), m_bCharCreate );
		}
	}
}

void CDnCharGestureDlg::Show( bool bShow ) 
{ 
	CDnCustomDlg::Show( bShow );

	if( bShow )
	{
		SetGestureInfo();
	}
}

void CDnCharGestureDlg::SetGestureInfo()
{
	m_vecStrGestureActionName.clear();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGESTURE );
	int nGestureIndex1 = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Login_Gesture1 );
	int nGestureIndex2 = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Login_Gesture2 );
	int nGestureIndex3 = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Login_Gesture3 );
	int nGestureIndex4 = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Login_Gesture4 );
	int nGestureIndex5 = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Login_Gesture5 );

	m_vecStrGestureActionName.push_back( pSox->GetFieldFromLablePtr( nGestureIndex1, "_ActionID" )->GetString() );
	m_vecStrGestureActionName.push_back( pSox->GetFieldFromLablePtr( nGestureIndex2, "_ActionID" )->GetString() );
	m_vecStrGestureActionName.push_back( pSox->GetFieldFromLablePtr( nGestureIndex3, "_ActionID" )->GetString() );
	m_vecStrGestureActionName.push_back( pSox->GetFieldFromLablePtr( nGestureIndex4, "_ActionID" )->GetString() );
	m_vecStrGestureActionName.push_back( pSox->GetFieldFromLablePtr( nGestureIndex5, "_ActionID" )->GetString() );

	m_pGestureSlotButton[0]->SetGestureIcon( pSox->GetFieldFromLablePtr( nGestureIndex1, "_IconID" )->GetInteger() );
	m_pGestureSlotButton[1]->SetGestureIcon( pSox->GetFieldFromLablePtr( nGestureIndex2, "_IconID" )->GetInteger() );
	m_pGestureSlotButton[2]->SetGestureIcon( pSox->GetFieldFromLablePtr( nGestureIndex3, "_IconID" )->GetInteger() );
	m_pGestureSlotButton[3]->SetGestureIcon( pSox->GetFieldFromLablePtr( nGestureIndex4, "_IconID" )->GetInteger() );
	m_pGestureSlotButton[4]->SetGestureIcon( pSox->GetFieldFromLablePtr( nGestureIndex5, "_IconID" )->GetInteger() );

	m_pGestureText[0]->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nGestureIndex1, "_NameID" )->GetInteger() ) );
	m_pGestureText[1]->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nGestureIndex2, "_NameID" )->GetInteger() ) );
	m_pGestureText[2]->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nGestureIndex3, "_NameID" )->GetInteger() ) );
	m_pGestureText[3]->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nGestureIndex4, "_NameID" )->GetInteger() ) );
	m_pGestureText[4]->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nGestureIndex5, "_NameID" )->GetInteger() ) );
}

#endif // PRE_MOD_SELECT_CHAR