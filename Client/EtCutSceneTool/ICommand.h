#pragma once



class ICmdReceiver;


// ��ɾ ǥ���ϴ� �ϳ��� ����
class ICommand
{
protected:
	ICmdReceiver*			m_pCmdReceiver;
	wxString				m_strDesc;
	bool					m_bIsUndo;


public:
	ICommand( ICmdReceiver* pCmdReceiver );
	virtual ~ICommand(void);

	// ���� ���� ���� �Լ�.
	virtual ICommand* CreateSame( void ) = 0;

	virtual const wxChar* GetDesc( void ) const = 0;
	virtual int GetTypeID( void ) = 0;

	virtual void Excute( void ) = 0;
	virtual void Undo( void ) = 0;

	// ����� �Ǵ� Ŀ�ǵ����� �ƴ���.
	virtual bool Recordable( void ) { return false; };

	bool IsUndo( void ) { return m_bIsUndo; };
};
