/*//===============================================================

	<< LOG BASE >>

	PRGM : B4nFter

	FILE : LOGBASE.HPP
	DESC : 로그출력 기본 클래스로 모든 로그출력 클래스는 이 클래스를 파생하여 생성
	INIT BUILT DATE : 2005. 01. 12
	LAST BUILT DATE : 2007. 04. 10

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "SYSBASEBASE.H"


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CLogBase
//*---------------------------------------------------------------
// DESC : 로그를 파일에 출력하기 위한 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CLogBase
{
protected:
//	CLogBase* m_lpLogLink;	// !!! 주의 - 무한재귀, 교착상태 유발 ?

public:
	inline CLogBase();
	inline virtual ~CLogBase();			// P.S.> 명백한 오류, 차후 수정 (*.h, *.cpp 로 분리 필요)

	virtual BOOL Write(LPCTSTR lpszFormat, ...) = 0;
	virtual BOOL WriteDate(LPCTSTR lpszFormat, ...) = 0;
	virtual BOOL WriteDirect(LPCTSTR lpszText) = 0;

	// !!! 주의 - 무한재귀, 교착상태 유발 ?
/*
	inline VOID SetLogLink(CLogBase* lpLogLink) { m_lpLogLink = lpLogLink; }
	inline CLogBase* GetLogLink() { return m_lpLogLink; }
*/

private:
	inline virtual BOOL Initialize();	// P.S.> 명백한 오류, 차후 수정 (*.h, *.cpp 로 분리 필요)
	inline virtual VOID Finalize();		// P.S.> 명백한 오류, 차후 수정 (*.h, *.cpp 로 분리 필요)
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogBase::CLogBase
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CLogBase::CLogBase()
{
	Initialize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogBase::~CLogBase
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CLogBase::~CLogBase()
{
	Finalize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogBase::Initialize
//*---------------------------------------------------------------
// DESC : 객체 관련 자원들을 초기화
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CLogBase::Initialize()
{
//	m_lpLogLink = NULL;		// !!! 주의 - 무한재귀, 교착상태 유발 ?
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogBase::Finalize
//*---------------------------------------------------------------
// DESC : 객체 관련 자원들을 해제
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CLogBase::Finalize()
{

}

