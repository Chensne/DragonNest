#include "stdafx.h"
#include <wx/wx.h>
#include "CmdProcessor.h"
#include "CommandSet.h"


const int CMD_HISTORY_CAPACITY = 50;


CCmdProcessor::CCmdProcessor(void) : m_pNowCommand( NULL ),
									 m_pLastDidCommand( NULL ),
									 m_iCmdHistoryCapacity( CMD_HISTORY_CAPACITY ),
									 m_iCmdIndex( -1 ),
									 m_bCmdUpdated( false ),
									 m_iLastCommandID( -1 )
									 //m_pLogReceiver( NULL )
{

}

CCmdProcessor::~CCmdProcessor(void)
{
	// 명령 큐에 쌓여있는거 메모리 전부 해제한다.
	for_each( m_dqCmdList.begin(), m_dqCmdList.end(), DeleteData<ICommand*>() );
}



void CCmdProcessor::SetCommand( ICommand* pCommand )
{
	assert( NULL == m_pNowCommand && "커맨드 실행중에 다시 커맨드를 날리면 안됩니다." );

	m_pNowCommand = pCommand;
}



void CCmdProcessor::DoCommand( void )
{
	if( m_pNowCommand->Recordable() )
	{
		// 일단 커맨드 포인터를 복사(커맨드 객체에 따라서는 깊은 복사)
		// 해서 그 포인터로 현재 커맨드를 설정
		m_pNowCommand = m_pNowCommand->CreateSame();

		++m_iCmdIndex;

		// 커맨드 인덱스가 최상위가 아니라면 기존에 있던 위의 커맨드들은 싹 날린다.
		while( m_iCmdIndex <= (int)m_dqCmdList.size()-1 )
		{
			delete m_dqCmdList.back();
			m_dqCmdList.pop_back();
		}

		m_dqCmdList.push_back( m_pNowCommand );

		m_bCmdUpdated = true;

		// 최대 갯수를 넘어가지 않도록.
		if( (int)m_dqCmdList.size() > m_iCmdHistoryCapacity )
		{
			delete m_dqCmdList.front();
			m_dqCmdList.pop_front();

			m_iCmdIndex = (int)m_dqCmdList.size() - 1;
		}
	}

	m_iLastCommandID = m_pNowCommand->GetTypeID();
	m_pNowCommand->Excute();

	m_pLastDidCommand = m_pNowCommand;

	const wxChar* pDesc = m_pNowCommand->GetDesc();
	if( pDesc )
	{
		m_strLastCmdMsg.assign( pDesc );
		//if( m_pLogReceiver )
		//	m_pLogReceiver->DataChanged();
	}

	m_pNowCommand = NULL;
	//m_bCmdUpdated = false;
}



bool CCmdProcessor::CanUndo( void )
{
	return -1 != m_iCmdIndex;
}



void CCmdProcessor::UndoCommand( void )
{
	// 안전한 코딩을 하자. 캬캬캬.
	if( CanUndo() )
	{
		// Undo 가 호출 되는 과정에서 커맨드 히스토리를 갱신해주는
		// 녀석들이 제대로 인식할 수 있도록 Undo 가 호출되기 전에 참조할 데이터들을 마련해준다.
		ICommand* pCommand = m_dqCmdList.at( m_iCmdIndex );

		// 실행취소 한 뒤의 커맨드 인덱스이다.
		--m_iCmdIndex;
		m_bCmdUpdated = true;

		m_iLastCommandID = pCommand->GetTypeID();
		pCommand->Undo();
		m_pLastDidCommand = pCommand;

		m_strLastCmdMsg.assign( wxT("*Undo : ") );
		const wxChar* pDesc = pCommand->GetDesc();
		if( pDesc )
		{
			m_strLastCmdMsg.append( pDesc );
			//if( m_pLogReceiver )
			//	m_pLogReceiver->DataChanged();
		}

		//m_bCmdUpdated = false;

	}
	else
	{
		m_bErrorOccured = true;
		m_strLastError.assign( wxT("[Command Invoker] UndoCommand() 함수 실패. 더 이상 실행 취소 할 수 없습니다.\n") );
	}
}



bool CCmdProcessor::CanRedo( void )
{
	return m_iCmdIndex < ((int)m_dqCmdList.size()-1);
}



void CCmdProcessor::RedoCommand( void )
{
	// 안전하고도 방어적인 코딩. -_-; 
	if( CanRedo() )
	{
		// Redo 도 Undo 와 마찬가지.
		++m_iCmdIndex;

		ICommand* pCommand = m_dqCmdList.at( m_iCmdIndex );

		m_bCmdUpdated = true;

		m_iLastCommandID = pCommand->GetTypeID();
		pCommand->Excute();
		m_pLastDidCommand = pCommand;

		m_strLastCmdMsg.assign( wxT("*Redo : ") );
		const wxChar* pDesc = pCommand->GetDesc();
		if( pDesc )
		{
			m_strLastCmdMsg.append( pDesc );
			//if( m_pLogReceiver )
			//	m_pLogReceiver->DataChanged();
		}

		//m_bCmdUpdated = false;
	}
	else
	{
		m_bErrorOccured = true;
		m_strLastError.assign( wxT("[Command Invoker] RedoCommand() 함수 실패. 더 이상 재실행 할 수 없습니다.\n") ); 
	}
}



void CCmdProcessor::OnUpdatedAllListeners( void )
{
	m_bCmdUpdated = false;
	m_iLastCommandID = CMD_NULL;
}




int CCmdProcessor::GetNumCmdHistory( void )
{
	return (int)m_dqCmdList.size();
}



ICommand* CCmdProcessor::GetCmdHistory( int iIndex )
{
	return m_dqCmdList.at( iIndex );
}




void CCmdProcessor::UpdateToThisHistory( int iCmdIndex )
{
	assert( iCmdIndex != -1 );

	if( iCmdIndex != -1 )
	{
		// 커맨드가 현재 커맨드보다 최근 커맨드라면 redo
		if( m_iCmdIndex < iCmdIndex )
		{
			while( m_iCmdIndex < iCmdIndex )
			{
				RedoCommand();
			}
		}
		else
		if( m_iCmdIndex > iCmdIndex )
		{
			// 커맨드가 현재 커맨드보다 이전 커맨드라면 undo
			while( m_iCmdIndex > iCmdIndex )
			{
				UndoCommand();
			}
		}
	}
}




const wxChar* CCmdProcessor::GetCommandMsg( void )
{
	//if( m_bCmdUpdated )
	//{
	return m_strLastCmdMsg.c_str();
	//}
	//else
	//	return NULL;
}



const wxChar* CCmdProcessor::GetLastError( void )
{
	m_bErrorOccured = false;

	return m_strLastError.c_str();
}


void CCmdProcessor::Clear( void )
{
	m_pNowCommand = NULL;
	m_iCmdIndex = -1;
	m_bCmdUpdated = true;			// 이래야 히스토리를 보여주는 뷰들이 업데이트 한다.
	m_iLastCommandID = CMD_NULL;

	// 명령 큐에 쌓여있는거 메모리 전부 해제한다.
	for_each( m_dqCmdList.begin(), m_dqCmdList.end(), DeleteData<ICommand*>() );

	m_dqCmdList.clear();
}

//
//void CCmdProcessor::RemoveCommand( int iCmdType )
//{
//	deque<ICommand*>::iterator iter = m_dqCmdList.begin();
//	for( iter; iter != m_dqCmdList.end(); ++iter )
//	{
//		if( iCmdType == (*iter)->GetTypeID() )
//		{
//			iter = m_dqCmdList.erase( iter );
//			
//			if( m_dqCmdList.empty())
//				break;
//		}
//	}
//}
