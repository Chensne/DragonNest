#pragma once

#include <boost/shared_ptr.hpp>

class IDnObserverNotifyEvent;


//------------------------------------------------------------------------------
/**
	이벤트 통지를 받아서 처리
*/
//------------------------------------------------------------------------------
class CDnObserver
{
private:
	

public:
	virtual void OnEvent( boost::shared_ptr<IDnObserverNotifyEvent>& pEvent ) = 0;

};
