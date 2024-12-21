
#include "Stdafx.h"
#include "DNUserEventHandler.h"
#include "DNUserSession.h"

CDNUserEventHandler::CDNUserEventHandler( CDNUserSession* pSession ):m_pSession(pSession)
{

}

CDNUserEventHandler::~CDNUserEventHandler()
{

}

void CDNUserEventHandler::OnFinalize()
{
	m_pSession->WritePingLog();
}

