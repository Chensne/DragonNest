#pragma once


class ICommand;

// 커맨드 히스토리 및 전반적인 부분을 관리하는 객체
// 지정된 커맨드를 실행하고 히스토리를 저장하는 객체입니다.
class CCmdProcessor
{
private:
	deque<ICommand*>		m_dqCmdList;
	ICommand*				m_pNowCommand;

	ICommand*				m_pLastDidCommand;

	int						m_iCmdHistoryCapacity;

	// 마지막으로 실행된 커맨드 큐에서의 커맨드 인덱스.
	int						m_iCmdIndex;

	bool					m_bCmdUpdated;
	tstring					m_strLastCmdMsg;

	// 마지막으로 실행된 커맨드의 enumeration 값을 저장해 둔다.
	int						m_iLastCommandID;

	bool					m_bErrorOccured;
	tstring					m_strLastError;

	/*IView*					m_pLogReceiver;*/

public:
	CCmdProcessor(void);
	~CCmdProcessor(void);

	void SetCommand( ICommand* pCommand );
	void DoCommand( void );

	bool CanUndo( void );
	void UndoCommand( void );

	bool CanRedo( void );
	void RedoCommand( void );

	bool IsCommandUpdate( void ) { return m_bCmdUpdated; };
	void OnUpdatedAllListeners( void );								// 모든 뷰들이 업데이트가 끝난 후 호출됨.

	int GetLastCmdTypeID( void ) { return m_iLastCommandID; };

	int GetNumCmdHistory( void );
	ICommand* GetCmdHistory( int iCmdIndex );
	void UpdateToThisHistory( int iCmdIndex );
	int GetNowCmdHistoryIndex( void ) { return m_iCmdIndex; };

	void SetHistoryCapacity( int iHistoryCapacity ) { m_iCmdHistoryCapacity = iHistoryCapacity; };
	int GetHistoryCapacity( void ) { return m_iCmdHistoryCapacity; };

	const wxChar* GetCommandMsg( void );
	const wxChar* GetLastError( void );

	ICommand* GetLastDidCommand( void ) { return m_pLastDidCommand; };

	//void SetLogReceiver( IView* pLogRecevier ) { m_pLogReceiver = pLogRecevier; };

	// 커맨드 모두 클리어.
	void Clear( void );
};
