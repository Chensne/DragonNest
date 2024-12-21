#include "StdAfx.h"
#include "DnChangEJobCashItemDlg.h"
#include "DnItemTask.h"
#include "DnWorld.h"
#include "DnItem.h"
#include "DnTableDB.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_CHANGEJOB_CASHITEM

CDnChangeJobCashItemDlg::CDnChangeJobCashItemDlg( UI_DIALOG_TYPE dialogType /*= UI_TYPE_FOCUS*/, CEtUIDialog *pParentDialog /*= NULL*/, int nID /*= -1*/, CEtUICallback *pCallback /*= NULL*/ )
												: CEtUIDialog( dialogType, pParentDialog, nID, pCallback ),
												  m_pJobListBox( NULL ),
												  m_pJobExplainTextBox( NULL ),
												  m_pChangeJobOKButton( false ),
												  m_iSelectedJobID( 0 ),
												  m_pChangeJobCashItem( NULL )
{

}

CDnChangeJobCashItemDlg::~CDnChangeJobCashItemDlg( void )
{

}

void CDnChangeJobCashItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "JobReselectDlg.ui" ).c_str(), bShow );
}

void CDnChangeJobCashItemDlg::InitialUpdate( void )
{
	m_pJobListBox = GetControl<CEtUIListBox>( "ID_LISTBOX_CLASS" );
	m_pJobExplainTextBox = GetControl<CEtUITextBox>( "ID_TEXTBOX_CLASSINFO" );
	m_pChangeJobOKButton = GetControl<CEtUIButton>( "ID_BT_OK" );

	m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_BT_CLOSE") );
}

void CDnChangeJobCashItemDlg::OnResponseUseItemFromServer( void )
{
	
	
}

void CDnChangeJobCashItemDlg::_CalcChangableJobs( void )
{
	m_vlChangableJobIDs.clear();
	m_pJobListBox->RemoveAllItems();

	// 현재 내 캐릭터의 직업을 근간으로 바꿀 수 있는 직업을 리스팅 해본다.
	vector<int> vlJobHistory;
	CDnLocalPlayerActor* pLocalPlayerActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	pLocalPlayerActor->GetJobHistory( vlJobHistory );

	if( vlJobHistory.empty() )
		return;

	int iJobDeep = int(vlJobHistory.size() - 1);
	int iMyClass = (int)vlJobHistory.front();

	// 전직하지 않은 캐릭터는 이 아이템으로 아무것도 할 수 없다.
	if( 0 < iJobDeep )
	{
		DNTableFileFormat*  pJobTable = GetDNTable( CDnTableDB::TJOB );
		pJobTable->GetItemIDListFromField( "_JobNumber", iJobDeep, m_vlChangableJobIDs );

		// 부모직업이 다른 녀석은 뺀다.
		vector<int>::iterator iter = m_vlChangableJobIDs.begin();
		for( iter; iter != m_vlChangableJobIDs.end(); )
		{
			int iJobID = *iter;
			int iParentJobID = pJobTable->GetFieldFromLablePtr( iJobID, "_Class" )->GetInteger();
#ifdef PRE_ADD_JOBCHANGE_RESTRICT_JOB
			bool bAvailableData = true;
			CDNTableFile::Cell* pServiceCell = pJobTable->GetFieldFromLablePtr(iJobID, "_Service");
			CDNTableFile::Cell* pJobChangeAvailCell = pJobTable->GetFieldFromLablePtr(iJobID, "_JobChangeAble");

			if (pServiceCell == NULL || pJobChangeAvailCell == NULL)
			{
				bAvailableData = false;
			}
			else
			{
				if (pServiceCell->GetInteger() <= 0 || pJobChangeAvailCell->GetInteger() <= 0)
					bAvailableData = false;
			}
#else
			bool bAvailableData = (pJobTable->GetFieldFromLablePtr( iJobID, "_Service" )->GetInteger() == 1);
#endif
			
			// 캐릭터 직업만 비교해주면 된다. 현재 내 직업과 같거나 부모가 다른 직업은 빼준다.
			if( (iParentJobID != iMyClass) ||
				(iJobID == vlJobHistory.back()) || 
				(false == bAvailableData) )
			{
				iter = m_vlChangableJobIDs.erase( iter );
			}
			else
			{
				++iter;
			}
		}

		// 리스트 박스에 직업 이름 추가.
		for( int i = 0; i < (int)m_vlChangableJobIDs.size(); ++i )
		{
			int iJobID = m_vlChangableJobIDs.at( i );
			int iJobNameStringID = pJobTable->GetFieldFromLablePtr( iJobID, "_JobName" )->GetInteger();

			const wchar_t* pJobName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iJobNameStringID );
			m_pJobListBox->AddItem( pJobName, NULL, iJobID );
		}
	}

}

void CDnChangeJobCashItemDlg::SetItem( CDnItem* pChangeJobCashItem )
{
	m_pChangeJobCashItem = pChangeJobCashItem;
}

void CDnChangeJobCashItemDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_iSelectedJobID = 0;

		// 현재 캐릭터의 직업에 따라 변경할 수 있는 모든 직업들을 찾아낸 뒤 
		// 직업 리스트 박스에 넣어준다.
		_CalcChangableJobs();

		m_pChangeJobOKButton->Enable( false );
		m_pJobExplainTextBox->ClearText();

		m_SmartMove.MoveCursor();
	}
	else
	{
		m_SmartMove.ReturnCursor();
	}

	CEtUIDialog::Show( bShow );
}

void CDnChangeJobCashItemDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand )
	{
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl("ID_BT_OK") )
			{
				// 마을에서만 사용 가능.
				if( CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType() )
				{
					// ~~으로 클래스가 변경되며, 스킬은 초기화 됩니다. 정말 사용하시겠습니까?
					DNTableFileFormat*  pJobTable = GetDNTable( CDnTableDB::TJOB ); 
					int iJobStringID = pJobTable->GetFieldFromLablePtr( m_iSelectedJobID, "_JobName" )->GetInteger();

					WCHAR wcBuffer[ 256 ] = { 0 };
					swprintf_s( wcBuffer, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4847), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iJobStringID ) );

					GetInterface().MessageBox( wcBuffer, MB_YESNO, CHANGEJOB_CONFIRM_MB, this );
				}
			}
			else
			if( IsCmdControl( "ID_BT_CLOSE" ) )
			{
				Show( false );
			}
		}
		else
		if( nCommand == EVENT_LISTBOX_SELECTION )
		{
			if( IsCmdControl("ID_LISTBOX_CLASS") )
			{
				// 선택된 인덱스로 직업 인덱스를 갖고 옴.
				m_pJobListBox->GetSelectedValue( m_iSelectedJobID );

				// 텍스트 박스에 직업 설명 셋팅.
				DNTableFileFormat*  pJobTable = GetDNTable( CDnTableDB::TJOB ); 
				int iJobExplainStringID = pJobTable->GetFieldFromLablePtr( m_iSelectedJobID, "_JobDescriptionID" )->GetInteger();

				m_pJobExplainTextBox->ClearText();
				m_pJobExplainTextBox->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iJobExplainStringID) );

				m_pChangeJobOKButton->Enable( true );
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnChangeJobCashItemDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID )
	{
		case CHANGEJOB_CONFIRM_MB:
			{
				if( nCommand == EVENT_BUTTON_CLICKED )
				{
					if( IsCmdControl( "ID_YES" ) )
					{
						// 서버로 아이템 전직 아이템 사용 패킷을 보낸다.
						GetItemTask().RequestUseChangeJobCashItem( m_pChangeJobCashItem->GetSerialID(), m_iSelectedJobID );

						// 다이얼로그 닫음
						Show( false );
					}
				}
			}
			break;
	}
}

#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM