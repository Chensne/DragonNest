#pragma once



class ICmdReceiver;


// 명령어를 표현하는 하나의 단위
class ICommand
{
protected:
	ICmdReceiver*			m_pCmdReceiver;
	wxString				m_strDesc;
	bool					m_bIsUndo;


public:
	ICommand( ICmdReceiver* pCmdReceiver );
	virtual ~ICommand(void);

	// 깊은 복사 생성 함수.
	virtual ICommand* CreateSame( void ) = 0;

	virtual const wxChar* GetDesc( void ) const = 0;
	virtual int GetTypeID( void ) = 0;

	virtual void Excute( void ) = 0;
	virtual void Undo( void ) = 0;

	// 기록이 되는 커맨드인지 아닌지.
	virtual bool Recordable( void ) { return false; };

	bool IsUndo( void ) { return m_bIsUndo; };
};
