#include "stdafx.h"
#include <wx/wx.h>
#include "ICommand.h"




ICommand::ICommand( ICmdReceiver* pCmdReceiver ) : m_pCmdReceiver( pCmdReceiver ), m_bIsUndo( false )
{

}

ICommand::~ICommand(void)
{

}
