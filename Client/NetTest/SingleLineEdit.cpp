// SingleLineEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NetTest.h"
#include "SingleLineEdit.h"


// CSingleLineEdit

IMPLEMENT_DYNAMIC(CSingleLineEdit, CEdit)

CSingleLineEdit::CSingleLineEdit()
{

}

CSingleLineEdit::~CSingleLineEdit()
{
}


BEGIN_MESSAGE_MAP(CSingleLineEdit, CEdit)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CSingleLineEdit message handlers

void CSingleLineEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_RETURN )
	{
		CString szText;
		GetWindowText( szText );
		szText += _T( "\r\n" );
		SetWindowText( _T( "" ) );
		GetParent()->SendMessage( EN_EDIT_CHANGE, ( LPARAM )szText.GetBuffer() );
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
