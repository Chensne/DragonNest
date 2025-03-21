#pragma once


class ICommandListener;
class ICommand;


// 옵저버 기능을 담당하는 클래스
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
