// SendMailDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "SendMailDlg.h"
#include "HttpUpdater.h"
#include "Log.h"
#include "ServerReporter.h"
#include "ServiceManager.h"
#include <sstream>

// CSendMailDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSendMailDlg, CDialog)

CSendMailDlg::CSendMailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendMailDlg::IDD, pParent)
{

}

CSendMailDlg::~CSendMailDlg()
{
}

void CSendMailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSendMailDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSendMailDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CSendMailDlg::OnBnClickedOk()
{
	vector<RequestParam> parameters;

	CString data;
	GetDlgItemText(IDC_EDIT_EMAIL_SENDER, data);
	parameters.push_back(RequestParam(L"SenderAddress", data.GetBuffer(0)));

	GetDlgItemText(IDC_EDIT_EMAIL_RECIPIENTS, data);
	parameters.push_back(RequestParam(L"ReceiverAddress", data.GetBuffer(0)));

	GetDlgItemText(IDC_EDIT_EMAIL_SUBJECT, data);
	parameters.push_back(RequestParam(L"Subject", data.GetBuffer(0)));

	ServerReport::CServerReporter reporter;
	const ConfigEx& config = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx();
	vector<ServerReport::CReportView*> report;
	reporter.Report(config.serverReportDays, config.serverReportDetail, report);
	
	wstringstream ss;
	wchar_t partition_text [256];
	g_pServiceManager->GetPartitionText(partition_text);
	ss << partition_text << L"\r\n\r\n";
	for each (ServerReport::CReportView* pReport in report)
	{
		ss << L"<" << pReport->GetDay() << L">" << L"\r\n\r\n";

		for each (ServerReport::View* pView in pReport->GetViews())
		{
			ss << pView->overview.str();
		}

		ss << L"\r\n\r\n";
		delete pReport;
	}
	parameters.push_back(RequestParam(L"Contents", ss.str()));

	wstring buffer;
	reporter.Report(config.serverReportDays, config.serverReportDetail, buffer);
	if (buffer.size() >= 1024 * 1024 * 10)
		buffer.clear();

	vector<RequestParam> files;
	files.push_back(RequestParam(L"AttachFile", GenerateServerReportFileName(), buffer));

	const EMailConfig& email = config.GetEMailConfig();
	RequestParam::boundary = email.boundary;

	CHttpUpdater http;
	if (!http.QueryPostUpload(email.serverIp.c_str(), NULL, NULL, email.serverPage.c_str(), parameters, files))
		return;

	g_Log.Log(LogType::_NORMAL, L"Send Email Succeeded.\r\n");
	OnOK();
}

BOOL CSendMailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	const EMailConfig& email = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().GetEMailConfig();

	SetDlgItemText(IDC_EDIT_EMAIL_SENDER, email.sender.c_str());

	ServerReport::CServerReporter reporter;
	const ConfigEx& config = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx();
	vector<ServerReport::CReportView*> report;
	reporter.Report(config.serverReportDays, config.serverReportDetail, report);

	wstringstream ss;
	for (int i=0; i<(int)email.allowance.size(); i++)
	{
		if (!reporter.is_dbdelay && email.allowance[i].find(L"DB delayed") == 0)
			continue;

		ss << email.recipients[i] << L";";
	}
	SetDlgItemText(IDC_EDIT_EMAIL_RECIPIENTS, ss.str().c_str());

	ss.str(L"");
	CTime time = CTime::GetCurrentTime();
	ss << GenerateServerReportNation() << L" " << time.Format(L"%Y-%m-%d").GetBuffer() << L" " << email.subject;
	SetDlgItemText(IDC_EDIT_EMAIL_SUBJECT, ss.str().c_str());

	ss.str(L"");
	for each (wstring s in email.msg)
	{
		ss << s << L"\r\n";
	}
	SetDlgItemText(IDC_EDIT_EMAIL_MSG, ss.str().c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
