/*//===============================================================

	<< BASE-SET >>

	PRGM : milkji, B4nFter

	FILE : BASESET.HPP
	DESC : ������Ʈ�� �⺻ ������ üũ�ϱ� ���� Ŭ����
	INIT BUILT DATE : 2007. 03. 13
	LAST BUILT DATE : 2007. 03. 13

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

// P.S.> SYSBASEBASE.H �� ���� ������ (���� �� ����� WINDOWS.H �� �⺻��� ���ܽ�Ŵ)


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CBaseSet
//*---------------------------------------------------------------
// DESC : ������Ʈ�� �⺻ ������ üũ�ϱ� ���� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------

class CBaseSet
{
public:
	static BOOL IsDebug();
	static BOOL IsUnicode();
	static BOOL IsMultiThread();
	static BOOL IsLittleEndian();
	static BOOL IsWindowStationVisible();
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::
//*---------------------------------------------------------------
// DESC : ���� ������Ʈ ������ _DEBUG ���� ���� ��ȯ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsDebug()
{
	return(MODE_CMP(TRUE,FALSE));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::IsUnicode
//*---------------------------------------------------------------
// DESC : ���� ������Ʈ�� ��� �ɼ��� _UNICODE ���� ���� ��ȯ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsUnicode()
{
	return(MODE_CHARSET(FALSE, TRUE));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::IsMultiThread
//*---------------------------------------------------------------
// DESC : ���� ������Ʈ�� �ڵ���� �ɼ��� _MT ���� ���� ��ȯ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsMultiThread()
{
	return(MODE_MT(FALSE,TRUE));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::IsLittleEndian
//*---------------------------------------------------------------
// DESC : ���� �÷����� little-endian ������� ���� ��ȯ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsLittleEndian()
{
	const unsigned long	ulHost = 0x12345678;
	return(::htonl(ulHost) != ulHost);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBaseSet::IsWindowStationVisible
//*---------------------------------------------------------------
// DESC : ���� ���μ����� ������ �����̼��� USER-INTERACTIVE ���� ���� ��ȯ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CBaseSet::IsWindowStationVisible()
{
	HWINSTA hWindowStation = ::GetProcessWindowStation();
	if (hWindowStation != NULL) {
		USEROBJECTFLAGS	stUOF;
		DWORD dwNeed = 0;
		if (::GetUserObjectInformation(hWindowStation, UOI_FLAGS, &stUOF, sizeof(USEROBJECTFLAGS), &dwNeed) == TRUE) {
			return ((WSF_VISIBLE & stUOF.dwFlags) == WSF_VISIBLE);
		}
	}
	return FALSE;
}

