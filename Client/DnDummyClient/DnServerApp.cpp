#include "Stdafx.h"
#include "DnServerApp.h"
#include "DnDummySession.h"

DnServerApp g_ServerApp;

DnServerApp::DnServerApp()
{

}

DnServerApp::~DnServerApp()
{

}


bool
DnServerApp::CreateSessionManager()
{
	m_pSessionManager = new DnDummySessionManager();

	return true;
}