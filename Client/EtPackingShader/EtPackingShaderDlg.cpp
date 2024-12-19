// EtPackingShaderDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "EtPackingShader.h"
#include "EtPackingShaderDlg.h"
#include "EtPackingShaderUtility.h"

#define REG_SUBKEY "SOFTWARE\\EyedentityGames\\EtPackingShader"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CEtPackingShaderDlg 대화 상자




CEtPackingShaderDlg::CEtPackingShaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEtPackingShaderDlg::IDD, pParent)
	, m_szCurrentDefine(_T(""))
	, m_szProgress(_T(""))
	, m_szShaderFolder(_T(""))
	, m_szOutputFolder(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nPrevComboSel = 0;
}

void CEtPackingShaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szCurrentDefine);
	DDX_Text(pDX, IDC_EDIT2, m_szProgress);
	DDX_Control(pDX, IDC_COMBO2, m_ComboBox);
	DDX_Text(pDX, IDC_EDIT3, m_szShaderFolder);	
	DDX_Control(pDX, IDC_EDIT2, m_progressEdit);
	DDX_Text(pDX, IDC_EDIT4, m_szOutputFolder);
}

BEGIN_MESSAGE_MAP(CEtPackingShaderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CEtPackingShaderDlg::OnBnClickedButtonCompile)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CEtPackingShaderDlg::OnCbnSelchangeCombo)
	ON_BN_CLICKED(IDC_BUTTON2, &CEtPackingShaderDlg::OnBnClickedButtonFolder)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON3, &CEtPackingShaderDlg::OnBnClickedButtonOutput)
END_MESSAGE_MAP()


// CEtPackingShaderDlg 메시지 처리기

BOOL CEtPackingShaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.	
	char szStrFolder[2048] = { 0, };
	char szStrOutput[2048] = { 0, };
	char szStrDefine1[2048] = {0,};
	char szStrDefine2[2048] = {0,};
	char szStrDefine3[2048] = {0,};
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ShaderFolder", szStrFolder, 2048 );
	if( strlen(szStrFolder) == 0 )
		m_szShaderFolder = "\\\\eye-ad\\ToolData\\SharedEffect";
	else m_szShaderFolder = szStrFolder;

	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "OutputFolder", szStrOutput, 2048 );
	if( strlen(szStrOutput) == 0 )
		m_szOutputFolder = "\\\\eye-ad\\ToolData\\Resource\\SharedEffect";
	else m_szOutputFolder = szStrOutput;
	
	GetModuleFileName(GetModuleHandle(0), m_szFileName, MAX_PATH );
	char *ps = strrchr(m_szFileName, '\\');
	if( ps ) {
		strcpy(ps, "\\shader.info");
	}

	FILE *fp = fopen(m_szFileName, "rt");

	if( fp == NULL ) {
		m_szDefineList[0] = "ETERNITY_ENGINE Eternity\r\nUSE_TERRAIN_LIGHTMAP 1\r\nDEPTH_SHADOWMAP 1\r\nBAKE_DEPTHMAP 1\r\nBAKE_VELOCITY 0\r\n";
		m_szDefineList[1] = "ETERNITY_ENGINE Eternity\r\nUSE_TERRAIN_LIGHTMAP 1\r\nDEPTH_SHADOWMAP 1\r\nBAKE_DEPTHMAP 1\r\nBAKE_VELOCITY 0\r\n";
		m_szDefineList[2] = "ETERNITY_ENGINE Eternity\r\nUSE_TERRAIN_LIGHTMAP 1\r\nDEPTH_SHADOWMAP 1\r\nBAKE_DEPTHMAP 1\r\nBAKE_VELOCITY 0\r\n";
	}
	else {
		char buffer[255];
		int nIndex = -1;
		for( ;; ) {
			fgets(buffer, 255, fp);
			if( feof(fp) || buffer[0] == '\0' )break;
			if( buffer[0] == '[' ) {				
				if( strcmp(buffer, "[DefineHigh]\n") == 0 ) {
					nIndex = 0;
				}
				if( strcmp(buffer, "[DefineNormal]\n") == 0 ) {
					nIndex = 1;
				}
				if( strcmp(buffer, "[DefineLow]\n") == 0 ) {
					nIndex = 2;
				}
			}
			else {
				m_szDefineList[nIndex] += buffer;				
			}
		}
		fclose(fp);
	}
	
	m_szCurrentDefine = m_szDefineList[0];
	UpdateData( FALSE );

	m_ComboBox.SetCurSel( 0 );	
	//OnCbnSelchangeCombo();
 
	if( strstr(GetCommandLine(), "-autostart") != NULL ) {
		OnBnClickedButtonCompile();
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CEtPackingShaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CEtPackingShaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CEtPackingShaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

struct SHADER_COMPILE_INFO
{
	char szFileName[64];
	int nQualityLevel;
	DWORD dwBufferSize;
	BYTE *pCompiledBuffer;	
};

DWORD WINAPI CEtPackingShaderDlg::CompileProc( LPVOID pThisPointer )
{
	CEtPackingShaderDlg *pThis = (CEtPackingShaderDlg*)pThisPointer;

	std::vector< std::string > &shaderFileNameList = GetShaderFileNameList( pThis->m_szShaderFolder );

	std::vector< SHADER_COMPILE_INFO > CompiledList;

	CString str;
	for( int nQualityLevel = 0; nQualityLevel < 3; nQualityLevel++) 
	{		
		int nErrors=0;
		int nSuccess=0;

		std::vector<D3DXMACRO> DefineList = GetStringToMacro( pThis->m_szDefineList[ nQualityLevel ] );

		for each( std::string szFileName in shaderFileNameList ) 
		{
			char szName[255];		
			char *ps = strrchr((char*)szFileName.c_str(), '\\');
			strcpy(szName, ps+1);		
			str += szName;			

			const char *fullName = szFileName.c_str();
			LPD3DXEFFECTCOMPILER pCompiler = NULL;
			LPD3DXBUFFER	pParseErrors = NULL;
			DWORD dwFlags = 0;
			D3DXCreateEffectCompilerFromFile(fullName, &DefineList[0], NULL, dwFlags, &pCompiler, &pParseErrors);
			if(!pCompiler) {
				char *szErrorMsg = (char*)pParseErrors->GetBufferPointer();
				// compile error
				str += " . . . Compile Error - ";		
				str += szErrorMsg;
				str += "\r\n";
				pThis->m_progressEdit.SetWindowText( str );
				pThis->m_progressEdit.LineScroll( INT_MAX );
				nErrors++;
				break;
			}

			LPD3DXBUFFER		pCompiledEffect = NULL;
			LPD3DXBUFFER		pErrorMsg = NULL;
			pCompiler->CompileEffect(dwFlags, &pCompiledEffect, &pErrorMsg);
			if(!pCompiledEffect) {
				char *szErrorMsg = (char*)pErrorMsg->GetBufferPointer();
				str += " . . . Compile Error - ";		
				str += szErrorMsg;
				str += "\r\n";
				pThis->m_progressEdit.SetWindowText( str );
				pThis->m_progressEdit.LineScroll( INT_MAX );	
				nErrors++;
				break;
			}
			pCompiler->Release();

			SHADER_COMPILE_INFO scInfo;
			strcpy(scInfo.szFileName, szName);
			scInfo.nQualityLevel = nQualityLevel;
			scInfo.dwBufferSize = pCompiledEffect->GetBufferSize();
			scInfo.pCompiledBuffer = new BYTE[ pCompiledEffect->GetBufferSize() ];
			memcpy(scInfo.pCompiledBuffer, pCompiledEffect->GetBufferPointer(), pCompiledEffect->GetBufferSize());
			CompiledList.push_back( scInfo );

			pCompiledEffect->Release();

			str += " . . . Compiling Ok.\r\n";		
			pThis->m_progressEdit.SetWindowText( str );
			pThis->m_progressEdit.LineScroll( INT_MAX );
			nSuccess++;
		}

		for each( D3DXMACRO e in DefineList ) {
			delete [] e.Name;
			delete [] e.Definition;
		}
		
		if( nErrors == 0 ) {
			char szText[255];
			sprintf(szText, " %d succeeded, %d failed\r\n\r\n", nSuccess, nErrors );
			str += szText;
			pThis->m_progressEdit.SetWindowText( str );
			pThis->m_progressEdit.LineScroll( INT_MAX );
		}
		else {
			break;
		}


	}


	char szOutName[255];
	strcpy(szOutName, pThis->m_szOutputFolder.GetBuffer());
	strcat(szOutName, "\\DnShaders.dat");
	FILE *fp = fopen(szOutName, "wb");
	if( fp == NULL ) {
		AfxMessageBox("파일을 쓸 수 없습니다.");
	}
	else {
		DWORD dwHead = *(DWORD*)"SHD0";
		int dwCount = (int)CompiledList.size();	
		fwrite(&dwHead, sizeof(DWORD), 1, fp);
		fwrite(&dwCount, sizeof(int), 1, fp);
		for( int i = 0; i < dwCount; i++) {
			fwrite(CompiledList[i].szFileName, 64, 1, fp);
			fwrite(&CompiledList[i].nQualityLevel, sizeof(DWORD), 1, fp);
			fwrite(&CompiledList[i].dwBufferSize, sizeof(DWORD), 1, fp);
			fwrite(CompiledList[i].pCompiledBuffer, CompiledList[i].dwBufferSize, 1, fp);
			delete [] CompiledList[i].pCompiledBuffer;
		}
		fclose(fp);

		char szText[255];
		sprintf(szText, " %d shaders save success. \r\n\r\n", dwCount );
		str += szText;
		pThis->m_progressEdit.SetWindowText( str );
		pThis->m_progressEdit.LineScroll( INT_MAX );
	}


	if( strstr(GetCommandLine(), "-autostart") != NULL ) {
		pThis->EndDialog(IDOK);
	}

	return 0;
}

void CEtPackingShaderDlg::OnBnClickedButtonCompile()
{	
	OnCbnSelchangeCombo();
	HANDLE packingThread = ::CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)&CEtPackingShaderDlg::CompileProc, this, 0, NULL );
}

void CEtPackingShaderDlg::OnCbnSelchangeCombo()
{
	UpdateData( TRUE );
	m_szDefineList[ m_nPrevComboSel ] = m_szCurrentDefine;
	int nCurSel = m_ComboBox.GetCurSel();
	m_szCurrentDefine = m_szDefineList[ nCurSel ];
	UpdateData( FALSE );
	m_nPrevComboSel = nCurSel;
}

void CEtPackingShaderDlg::OnBnClickedButtonFolder()
{
	// TODO: Add your control notification handler code here
	CXTBrowseDialog Dlg;
	UpdateData(TRUE);
	Dlg.SetTitle(_T("Select Shader Directory"));
	if( !m_szShaderFolder.IsEmpty() ) {
		TCHAR path[_MAX_PATH];
		STRCPY_S(path, _MAX_PATH, m_szShaderFolder);
		Dlg.SetSelPath(path);
	}
	if( Dlg.DoModal() == IDOK ) {
		m_szShaderFolder = Dlg.GetSelPath();
		UpdateData(FALSE);
	}
}

void CEtPackingShaderDlg::OnBnClickedButtonOutput()
{
	// TODO: Add your control notification handler code here
	CXTBrowseDialog Dlg;
	UpdateData(TRUE);
	Dlg.SetTitle(_T("Select Output Directory"));
	if( !m_szOutputFolder.IsEmpty() ) {
		TCHAR path[_MAX_PATH];
		STRCPY_S(path, _MAX_PATH, m_szOutputFolder);
		Dlg.SetSelPath(path);
	}
	if( Dlg.DoModal() == IDOK ) {
		m_szOutputFolder = Dlg.GetSelPath();
		UpdateData(FALSE);
	}
}

void CEtPackingShaderDlg::OnDestroy()
{
	CDialog::OnDestroy();
	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ShaderFolder", m_szShaderFolder.GetBuffer() );
	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "OutputFolder", m_szOutputFolder.GetBuffer() );

	OnCbnSelchangeCombo();

	/*SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "DefineHigh", m_szDefineList[0].GetBuffer() );
	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "DefineNormal", m_szDefineList[1].GetBuffer() );
	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "DefineLow", m_szDefineList[2].GetBuffer() );	*/

	FILE *fp = fopen(m_szFileName, "wt");
	fputs("[DefineHigh]\n", fp);
	fputs(m_szDefineList[0].GetBuffer(), fp);
	fputs("[DefineNormal]\n", fp);
	fputs(m_szDefineList[1].GetBuffer(), fp);
	fputs("[DefineLow]\n", fp);
	fputs(m_szDefineList[2].GetBuffer(), fp);
	fclose(fp);
	
	// TODO: Add your message handler code here
}
