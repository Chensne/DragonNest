#pragma once


class ICommandListener;
class ICommand;


// ������ ����� ����ϴ� Ŭ����
class CObservable
{
private:
	vector<ICommandListener*>			m_vlpListeners;


public:
	CObservable(void);
	virtual ~CObservable(void);

	void AddCommandListener( ICommandListener* pEventListener );
	bool DelCommandListener( ICommandListener* pEventListener );

	bool NotifyToAllListeners( ICommand* pCommand );
};
