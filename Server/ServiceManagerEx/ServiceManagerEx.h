// ServiceManagerEx.h : ServiceManagerEx ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "SMConfig.h"
#include "GSMServer.h"
#include "Scheduler.h"

// CServiceManagerExApp:
// �� Ŭ������ ������ ���ؼ��� ServiceManagerEx.cpp�� �����Ͻʽÿ�.
//

class CLogViewDisplayer;

class CServiceManagerExApp : public CWinApp
{
public:
	CServiceManagerExApp();
	virtual ~CServiceManagerExApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	bool LoadConfig();
	void ReloadConfigEx();

public:
	void BuildView();
	void RefreshView();
	void RefreshStateView();

public:
	const ConfigEx& GetConfigEx() const { return m_ConfigEx; }

public:
	void OnPatchStart();
	void OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax);
	void OnPatchEnd(bool succeeded);

	void OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax);

public:
	void OnWorldMaxUser(int id, int maxUser);
	void OnPatchFail(int id, const wchar_t* msg);
	void OnPatchCompleted(int id);
	void OnCommandPatch();

private:
	void LoadConfigEx();
	bool CreateDisplayer();
	bool InitServiceManager();

public:
	bool ReportExceptionToClipboard();
	bool ReportExceptionToBuffer(size_t days, OUT wstring& buffer);
	bool ReportExceptionToFile(const wstring& filename);

private:
	bool SaveToClipboard(const wstring& report);

private:
	void BuildSchedule();
	void ClearSchedule();

private:
	CGSMServer m_GSMServer;
	ConfigEx m_ConfigEx;
	HANDLE m_hMutex;
	CLogViewDisplayer* m_pLogDisplayer;
	SimpleScheduler::CScheduler m_Scheduler;
};

extern CServiceManagerExApp theApp;