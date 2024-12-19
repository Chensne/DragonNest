// HotkeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "HotkeyDlg.h"

SHotkey g_nHotkeyList[] =
{
	{ "NONE", 0 },
	{ "BACK", VK_BACK },
	{ "TAB", VK_TAB },
	{ "RETURN", VK_RETURN },
	{ "SHIFT", VK_SHIFT },
	{ "CONTROL", VK_CONTROL },
	{ "ALT", VK_MENU },
	{ "PAUSE", VK_PAUSE },
	{ "CAPITAL", VK_CAPITAL },
	{ "ESCAPE", VK_ESCAPE },
	{ "SPACE", VK_SPACE },
	{ "PRIOR", VK_PRIOR },
	{ "NEXT", VK_NEXT },
	{ "END", VK_END },
	{ "HOME", VK_HOME },
	{ "LEFT", VK_LEFT },
	{ "UP", VK_UP },
	{ "RIGHT", VK_RIGHT },
	{ "DOWN", VK_DOWN },
	{ "SNAPSHOT", VK_SNAPSHOT },
	{ "INSERT", VK_INSERT },
	{ "DELETE", VK_DELETE },
	{ "NUMPAD0", VK_NUMPAD0 },
	{ "NUMPAD1", VK_NUMPAD1 },
	{ "NUMPAD2", VK_NUMPAD2 },
	{ "NUMPAD3", VK_NUMPAD3 },
	{ "NUMPAD4", VK_NUMPAD4 },
	{ "NUMPAD5", VK_NUMPAD5 },
	{ "NUMPAD6", VK_NUMPAD6 },
	{ "NUMPAD7", VK_NUMPAD7 },
	{ "NUMPAD8", VK_NUMPAD8 },
	{ "NUMPAD9", VK_NUMPAD9 },
	{ "MULTIPLY", VK_MULTIPLY },
	{ "ADD", VK_ADD },
	{ "SEPARATOR", VK_SEPARATOR },
	{ "SUBTRACT", VK_SUBTRACT },
	{ "DECIMAL", VK_DECIMAL },
	{ "DIVIDE", VK_DIVIDE },
	{ "F1", VK_F1 },
	{ "F2", VK_F2 },
	{ "F3", VK_F3 },
	{ "F4", VK_F4 },  
	{ "F5", VK_F5 },  
	{ "F6", VK_F6 },  
	{ "F7", VK_F7 },  
	{ "F8", VK_F8 },  
	{ "F9", VK_F9 },  
	{ "F10", VK_F10 }, 
	{ "F11", VK_F11 }, 
	{ "F12", VK_F12 }, 
	{ "NUMLOCK", VK_NUMLOCK },
	{ "SCROLL", VK_SCROLL },
	{ "A", 0x41 },
	{ "B", 0x42 },
	{ "C", 0x43 },
	{ "D", 0x44 },
	{ "E", 0x45 },
	{ "F", 0x46 },
	{ "G", 0x47 },
	{ "H", 0x48 },
	{ "I", 0x49 },
	{ "J", 0x4A },
	{ "K", 0x4B },
	{ "L", 0x4C },
	{ "M", 0x4D },
	{ "N", 0x4E },
	{ "O", 0x4F },
	{ "P", 0x50 },
	{ "Q", 0x51 },
	{ "R", 0x52 },
	{ "S", 0x53 },
	{ "T", 0x54 },
	{ "U", 0x55 },
	{ "V", 0x56 },
	{ "W", 0x57 },
	{ "X", 0x58 },
	{ "Y", 0x59 },
	{ "Z", 0x5A },
	{ "0", 0x30 },
	{ "1", 0x31 },
	{ "2", 0x32 },
	{ "3", 0x33 },
	{ "4", 0x34 },
	{ "5", 0x35 },
	{ "6", 0x36 },
	{ "7", 0x37 },
	{ "8", 0x38 },
	{ "9", 0x39 },
	{ ";:", VK_OEM_1 },
	{ "=+", VK_OEM_PLUS },
	{ ",<", VK_OEM_COMMA },
	{ "-_", VK_OEM_MINUS },
	{ ".>", VK_OEM_PERIOD },
	{ "/?", VK_OEM_2 },
	{ "`~", VK_OEM_3 },
	{ "[{", VK_OEM_4 },
	{ "\\|", VK_OEM_5 },
	{ "]}", VK_OEM_6 },
	{ "'\"", VK_OEM_7 },
};


// CHotkeyDlg dialog

IMPLEMENT_DYNAMIC(CHotkeyDlg, CDialog)

CHotkeyDlg::CHotkeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHotkeyDlg::IDD, pParent)
{
	m_nHotkey = 0;
}

CHotkeyDlg::~CHotkeyDlg()
{
}

void CHotkeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOTKEY_COMBO, m_HotkeyList);
}


BEGIN_MESSAGE_MAP(CHotkeyDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_HOTKEY_COMBO, &CHotkeyDlg::OnCbnSelchangeHotkeyCombo)
	ON_BN_CLICKED(IDOK, &CHotkeyDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CHotkeyDlg message handlers

BOOL CHotkeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i, nCount;

	nCount = sizeof( g_nHotkeyList ) / sizeof( SHotkey );
	for( i = 0; i < nCount; i++ )
	{
		m_HotkeyList.AddString( g_nHotkeyList[ i ].szName );
	}

	m_HotkeyList.SetCurSel( m_nHotkey );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CHotkeyDlg::OnCbnSelchangeHotkeyCombo()
{
	// TODO: Add your control notification handler code here
}

void CHotkeyDlg::OnBnClickedOk()
{
	int nCurSel;

	nCurSel = m_HotkeyList.GetCurSel();
	if( nCurSel != -1 )
	{
		m_nHotkey = g_nHotkeyList[ nCurSel ].nCode;
	}
	// TODO: Add your control notification handler code here
	OnOK();
}

char *GetHotkeyString( int nHotkey )
{
	int i, nCount;

	nCount = sizeof( g_nHotkeyList ) / sizeof( SHotkey );
	for( i = 0; i < nCount; i++ )
	{
		if( nHotkey == g_nHotkeyList[ i ].nCode )
		{
			return g_nHotkeyList[ i ].szName;
		}
	}

	return NULL;
}

int GetHotkeyIndex( int nHotkey )
{
	int i, nCount;

	nCount = sizeof( g_nHotkeyList ) / sizeof( SHotkey );
	for( i = 0; i < nCount; i++ )
	{
		if( nHotkey == g_nHotkeyList[ i ].nCode )
		{
			return i;
		}
	}

	return -1;
}