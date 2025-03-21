#include "StdAfx.h"

#ifdef PRE_ADD_CRAZYDUC_UI

#include "DnMODCustom3Dlg.h"
#include "DnTrigger.h"
#include "DnPartyTask.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "Timeset.h"

CDnMODCustom3Dlg::CDnMODCustom3Dlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnMODDlgBase( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
	m_pStaticSecondTimer = NULL;
	m_pStaticMilliSecondTimer = NULL;
	m_bPlay = false;
	
	m_tStandardTimeSec = 0;

	m_fMilliSecond = 0.f;
	m_fExtraTimeSec = 0.0f;
	m_fOriginTimeSec = 0.0f;
	m_fCurTimeSec = 0.0f;

}

CDnMODCustom3Dlg::~CDnMODCustom3Dlg()
{
	SAFE_DELETE_VEC( m_pVecStaticCounter );
}

void CDnMODCustom3Dlg::InitialUpdate()
{
	char szStr[64];
	for( int i=0; ; i++ ) {
		bool bExist;
		CDnDamageCount *pStatic;
		sprintf_s( szStr, "ID_COUNTER%d",  i );
		pStatic = GetControl<CDnDamageCount>( szStr, &bExist );

		if( !bExist ) break;
		pStatic->Init();
		pStatic->SetFontSize( 1.f );
		pStatic->SetRightAlign( true );
		m_pVecStaticCounter.push_back( pStatic );
	}

	m_pStaticSecondTimer      = GetControl<CEtUIStatic>("ID_TEXT_SECOND");
	m_pStaticMilliSecondTimer = GetControl<CEtUIStatic>("ID_TEXT_MILISECOND");

	m_pStaticSecondTimer->SetText( L"00" );
	m_pStaticMilliSecondTimer->SetText( L"00" );

}

void CDnMODCustom3Dlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( m_szUIFileName ).c_str(), bShow );
}

void CDnMODCustom3Dlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CDnMODDlgBase::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMODCustom3Dlg::Show( bool bShow )
{
	CDnMODDlgBase::Show( bShow );
}

void CDnMODCustom3Dlg::Process( float fElapsedTime )
{
	CDnMODDlgBase::Process( fElapsedTime );
	
	TimeChecker();

	for( DWORD i=0; i<m_pVecStaticCounter.size(); i++ ) {
		LinkValueStruct *pStruct = GetLinkValue(i);
		if( pStruct ) {
			lua_tinker::table Table = lua_tinker::get<lua_tinker::table>( pStruct->pTrigger->GetLuaState(), "g_value" );
			if( Table.m_obj->validate() ) {
				int nValue = Table.get<int>( pStruct->nDefineValueIndex );
				m_pVecStaticCounter[i]->SetValue( nValue );
				m_pVecStaticCounter[i]->Show( true );
			}
		}
		else m_pVecStaticCounter[i]->Show( false );
	}

	if ( CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsSyncComplete() == false ) 
		m_bPlay = false ;

	if ( m_bPlay && m_fExtraTimeSec > 0.0f )
	{
		m_fExtraTimeSec = (m_fCurTimeSec - (float)((CTimeSet().GetTimeT64_GM() - (float)m_tStandardTimeSec)));
		RefreshTimer();
	}
	else if( ! m_bPlay )
	{
		m_tStandardTimeSec = CTimeSet().GetTimeT64_GM();
		m_fMilliSecond = (float)CTimeSet().GetMilliseconds()/1000;
	}

}


void CDnMODCustom3Dlg::Render( float fElapsedTime )
{
	CDnMODDlgBase::Render( fElapsedTime );
}

void CDnMODCustom3Dlg::SetTimer( int nExtraTime, int nOriginTime )
{	
	m_fExtraTimeSec = (float)nExtraTime;
	m_fOriginTimeSec = (float)nOriginTime;

	m_fCurTimeSec = m_fExtraTimeSec - (float)CTimeSet().GetMilliseconds() / 1000; 
	m_tStandardTimeSec = CTimeSet().GetTimeT64_GM();
	m_fMilliSecond = (float)CTimeSet().GetMilliseconds() / 1000;
	RefreshTimer();
}

void CDnMODCustom3Dlg::RefreshTimer()
{
	m_pStaticSecondTimer->ClearText();
	m_pStaticMilliSecondTimer->ClearText();

	int nSecond = static_cast<int>(ceil(m_fExtraTimeSec)) % 60;	
	int nMilliSecond = 100 - (CTimeSet().GetMilliseconds() / 10);


	nSecond = nSecond < 0 ? 0 : nSecond;
	wchar_t wszTimeValue[10] = {0};

	swprintf_s( wszTimeValue, _countof(wszTimeValue), L"%.2d",  nSecond );
	m_pStaticSecondTimer->SetText( wszTimeValue );

	swprintf_s( wszTimeValue, _countof(wszTimeValue), L"%.2d",  nMilliSecond );
	m_pStaticMilliSecondTimer->SetText( wszTimeValue );	

	if ( 0 >= m_fExtraTimeSec ) m_pStaticMilliSecondTimer->SetText( L"00" );

}

void CDnMODCustom3Dlg::ResetTimer()
{
	m_fExtraTimeSec = 0.0f;
	m_fOriginTimeSec = 0.0f;
	m_fCurTimeSec = 0.0f;
	m_tStandardTimeSec = 0;
	m_fMilliSecond = 0.0f;

	m_pStaticSecondTimer->SetText( L"00" );
	m_pStaticMilliSecondTimer->SetText( L"00" );

	m_bPlay = false;
}

void CDnMODCustom3Dlg::TimeChecker()
{
	CDnGameTask* pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" ) ;

	if( pGameTask && ! pGameTask->IsShowMODDialog() )
	{
		ResetTimer();
	}
}

#endif PRE_ADD_CRAZYDUC_UI
