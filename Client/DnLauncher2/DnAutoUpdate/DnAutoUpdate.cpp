// DnAutoUpdate.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "DnAutoUpdate.h"
#include "DnAutoUpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern CString			g_strHttpPath;
extern CString			g_strLocalPath;
extern vector<int>		g_vecUpdateVersion;


// CDnAutoUpdateApp

BEGIN_MESSAGE_MAP(CDnAutoUpdateApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

int __cdecl DefautAllocHook( int, void *, size_t, int, long, const unsigned char *, int )
{
	return 1;
}

int __cdecl MyAllocHook(
						int nAllocType,
						void * pvData,
						size_t nSize,
						int nBlockUse,
						long lRequest,
						const unsigned char * szFileName,
						int nLine
						)
{
	if( nAllocType == _HOOK_ALLOC && nSize == 196 )
	{
		int a = 0;
	}
	return 1;
}

// CDnAutoUpdateApp ����

CDnAutoUpdateApp::CDnAutoUpdateApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
//	_CrtSetAllocHook( MyAllocHook );
//	_CrtSetBreakAlloc(59925);
}


// ������ CDnAutoUpdateApp ��ü�Դϴ�.

CDnAutoUpdateApp theApp;


// CDnAutoUpdateApp �ʱ�ȭ

BOOL CDnAutoUpdateApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	CString strCmdLine = m_lpCmdLine;

	int nIndex = strCmdLine.Find( '|' );
	g_strHttpPath = strCmdLine.Left( nIndex );
	strCmdLine.Delete( 0, nIndex + 1 );

	nIndex = strCmdLine.Find( '|' );
	g_strLocalPath = strCmdLine.Left( nIndex );
	strCmdLine.Delete( 0, nIndex + 1 );

	nIndex = strCmdLine.Find( '-' );
	char szTemp[256] = { 0, };
	WideCharToMultiByte( CP_ACP, 0, strCmdLine.Left( nIndex ).GetBuffer(), -1, szTemp, sizeof(szTemp), NULL, NULL );
	int nStartVersion = atoi( szTemp );
	strCmdLine.Delete( 0, nIndex + 1 );
	WideCharToMultiByte( CP_ACP, 0, strCmdLine.GetBuffer(), -1, szTemp, sizeof(szTemp), NULL, NULL );
	int nTargetVersion = atoi( szTemp );

	if( g_strHttpPath.GetLength() == 0 || g_strLocalPath.GetLength() == 0 )
		return FALSE;
	if( nStartVersion == 0 || nTargetVersion == 0 || nStartVersion > nTargetVersion )
		return FALSE;

	for( int i=nStartVersion; i<=nTargetVersion; i++ )
		g_vecUpdateVersion.push_back( i );

	CDnAutoUpdateDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}
