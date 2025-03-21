// ServiceManagerEx.h : ServiceManagerEx 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "SMConfig.h"
#include "GSMServer.h"
#include "Scheduler.h"

// CServiceManagerExApp:
// 이 클래스의 구현에 대해서는 ServiceManagerEx.cpp을 참조하십시오.
//

class CLogViewDisplayer;

class CServiceManagerExApp : public CWinApp
{
public:
	CServiceManagerExApp();
	virtual ~CServiceManagerExApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
// 구현입니다.
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