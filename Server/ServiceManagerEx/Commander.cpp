// CommandLine.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "Commander.h"
#include "Log.h"
#include "ServiceManager.h"
#include "SMConfig.h"

extern CServiceManager* g_pServiceManager;
extern CLog g_Log;

// CCommander

IMPLEMENT_DYNAMIC(CCommander, CEdit)

BEGIN_MESSAGE_MAP(CCommander, CEdit)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CCommander::CCommander()
	: index(0)
{
}

CCommander::~CCommander()
{
}

void CCommander::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_RETURN:
		OnCommand();
		return;

	case VK_UP:
		OnPrevCommand();
		return;

	case VK_DOWN:
		OnNextCommand();
		return;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCommander::SetCommand(const wstring& command)
{
	SetWindowText(command.c_str());
	SetSel(GetWindowTextLength(), -1);
}

void CCommander::OnCommand()
{
	CString str;
	GetWindowText(str);
	if (str.IsEmpty())
		return;

	wchar_t msg[256];
	::wsprintf(msg, L"Are you sure to command '%s'?", str.GetBuffer());
	if (IDNO == ::AfxMessageBox(msg, MB_YESNO))
		return;

	char cmd[256];
	::WideCharToMultiByte(CP_ACP, 0, str.GetBuffer(), str.GetLength() + 1, cmd, 256, NULL, NULL);

	try
	{
		//PatchFull ���� ó��
		if( !stricmp(cmd,  "patchfull"))
		{
			if(!g_pServiceManager->IsRunLauncher())
			{
				AfxMessageBox(L"Launcher Not Found!");
				return;
			}
			if(!g_pServiceManager->IsStartAllLauncherAndContinue())
				return;

			g_pServiceManager->OnCommandPatch();
		}
		else
			g_pServiceManager->ParseCommand(cmd);
	}
	catch (...)
	{
		SetWindowText(L"");
		g_Log.Log(LogType::_ERROR, L"Command parsing error. \'%s\'\n", str.GetBuffer());
		return;
	}

	if (history.size() >= ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().cmdHistoryCount)
		history.erase(history.begin());

	history.push_back(str.GetBuffer());
	index = history.size();

	SetWindowText(L"");
}

void CCommander::OnPrevCommand()
{
	if (index <= 0)
		return;

	--index;
	wstring command = history[index];
	SetWindowText(command.c_str());
	SetSel(GetWindowTextLength(), -1);
}

void CCommander::OnNextCommand()
{
	if (index >= history.size() - 1)
		return;

	++index;
	wstring command = history[index];
	SetWindowText(command.c_str());
	SetSel(GetWindowTextLength(), -1);
}