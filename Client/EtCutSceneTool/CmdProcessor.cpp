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
	// ��� ť�� �׿��ִ°� �޸� ���� �����Ѵ�.
	for_each( m_dqCmdList.begin(), m_dqCmdList.end(), DeleteData<ICommand*>() );
}



void CCmdProcessor::SetCommand( ICommand* pCommand )
{
	assert( NULL == m_pNowCommand && "Ŀ�ǵ� �����߿� �ٽ� Ŀ�ǵ带 ������ �ȵ˴ϴ�." );

	m_pNowCommand = pCommand;
}



void CCmdProcessor::DoCommand( void )
{
	if( m_pNowCommand->Recordable() )
	{
		// �ϴ� Ŀ�ǵ� �����͸� ����(Ŀ�ǵ� ��ü�� ���󼭴� ���� ����)
		// �ؼ� �� �����ͷ� ���� Ŀ�ǵ带 ����
		m_pNowCommand = m_pNowCommand->CreateSame();

		++m_iCmdIndex;

		// Ŀ�ǵ� �ε����� �ֻ����� �ƴ϶�� ������ �ִ� ���� Ŀ�ǵ���� �� ������.
		while( m_iCmdIndex <= (int)m_dqCmdList.size()-1 )
		{
			delete m_dqCmdList.back();
			m_dqCmdList.pop_back();
		}

		m_dqCmdList.push_back( m_pNowCommand );

		m_bCmdUpdated = true;

		// �ִ� ������ �Ѿ�� �ʵ���.
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
	// ������ �ڵ��� ����. ļļļ.
	if( CanUndo() )
	{
		// Undo �� ȣ�� �Ǵ� �������� Ŀ�ǵ� �����丮�� �������ִ�
		// �༮���� ����� �ν��� �� �ֵ��� Undo �� ȣ��Ǳ� ���� ������ �����͵��� �������ش�.
		ICommand* pCommand = m_dqCmdList.at( m_iCmdIndex );

		// ������� �� ���� Ŀ�ǵ� �ε����̴�.
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
		m_strLastError.assign( wxT("[Command Invoker] UndoCommand() �Լ� ����. �� �̻� ���� ��� �� �� �����ϴ�.\n") );
	}
}



bool CCmdProcessor::CanRedo( void )
{
	return m_iCmdIndex < ((int)m_dqCmdList.size()-1);
}



void CCmdProcessor::RedoCommand( void )
{
	// �����ϰ� ������� �ڵ�. -_-; 
	if( CanRedo() )
	{
		// Redo �� Undo �� ��������.
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
		m_strLastError.assign( wxT("[Command Invoker] RedoCommand() �Լ� ����. �� �̻� ����� �� �� �����ϴ�.\n") ); 
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
		// Ŀ�ǵ尡 ���� Ŀ�ǵ庸�� �ֱ� Ŀ�ǵ��� redo
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
			// Ŀ�ǵ尡 ���� Ŀ�ǵ庸�� ���� Ŀ�ǵ��� undo
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
	m_bCmdUpdated = true;			// �̷��� �����丮�� �����ִ� ����� ������Ʈ �Ѵ�.
	m_iLastCommandID = CMD_NULL;

	// ��� ť�� �׿��ִ°� �޸� ���� �����Ѵ�.
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
