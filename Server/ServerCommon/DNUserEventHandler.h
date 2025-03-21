
#pragma once

class CDNUserSession;

class CDNUserEventHandler
{
protected:
	CDNUserEventHandler( CDNUserSession* pSession );

public:
	virtual ~CDNUserEventHandler();
	virtual void OnFinalize();

private:
	CDNUserSession* m_pSession;
};
