/*//===============================================================

	<< MEMORY POOL EX >>

	PRGM : B4nFter

	FILE : MEMPOOLEX.HPP
	DESC : �޸� Ǯ
	INIT BUILT DATE : 2004. 05. 17
	LAST BUILT DATE : 2007. 09. 16

	P.S.>
		- 
		- 

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "SYSBASEBASE.H"
#include <VECTOR>
#include <QUEUE>


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemPoolEx
//*---------------------------------------------------------------
// DESC : �޸� Ǯ Ŭ���� (����)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TDATA, INT TINITSIZE = 64, INT TINCRSIZE = 0>
class CMemPoolEx
{
public:
	CMemPoolEx();
	virtual ~CMemPoolEx();

	TDATA* GetMemory();
	VOID ReleaseMemory(TDATA* lpData);

protected:
	BOOL CreateMemory(INT iSize);

private:
	std::vector<TDATA*> m_vtMemManage;
	std::queue<TDATA*> m_queMemPool;
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemPoolEx::CMemPoolEx
//*---------------------------------------------------------------
// DESC : ������
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TDATA, INT TINITSIZE, INT TINCRSIZE>
CMemPoolEx<TDATA, TINITSIZE, TINCRSIZE>::CMemPoolEx()
{
#pragma warning (disable:4127)
	if (0 < TINITSIZE) {
		CreateMemory(TINITSIZE);
	}
#pragma warning (default:4127)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemPoolEx::~CMemPoolEx
//*---------------------------------------------------------------
// DESC : �Ҹ���
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TDATA, INT TINITSIZE, INT TINCRSIZE>
CMemPoolEx<TDATA, TINITSIZE, TINCRSIZE>::~CMemPoolEx()
{
	std::vector<TDATA*>::iterator it = m_vtMemManage.begin();
	for (;it != m_vtMemManage.end(); ++it) {
		delete [] *it;		// temporary !!! �� it have to be here		// 20100322 �����
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemPoolEx::CreateMemory
//*---------------------------------------------------------------
// DESC : �޸� Ǯ�� Ư�� ������ �޸� ����, �߰�
// PARM :	1 . iSize - ������ �޸��� ũ�� (����)
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TDATA, INT TINITSIZE, INT TINCRSIZE>
BOOL CMemPoolEx<TDATA, TINITSIZE, TINCRSIZE>::CreateMemory(INT iSize)
{
	if (iSize <= 0) 
		return FALSE;

	TDATA* lpData = new TDATA[iSize];	// 20100322 LNK2005
	if (lpData != NULL) 
	{
		m_vtMemManage.push_back(lpData);
		for (INT i = 0 ; i < iSize ; ++i) 
		{
			m_queMemPool.push(((TDATA*)lpData)+i);
		}
	}
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemPoolEx::GetMemory
//*---------------------------------------------------------------
// DESC : �޸� Ǯ���� �Ѱ��� �޸� ��Ʈ���� ������
// PARM : N/A
// RETV : �޸� ��Ʈ�� ������ (���� �޸𸮰� ���� ���� NULL)
// PRGM : B4nFter
//*---------------------------------------------------------------
#pragma warning (disable:4127)
template<class TDATA, INT TINITSIZE, INT TINCRSIZE>
TDATA* CMemPoolEx<TDATA, TINITSIZE, TINCRSIZE>::GetMemory()
{
	if (m_queMemPool.empty()) {
		if (TINCRSIZE > 0) {
			BOOL bRetVal = CreateMemory(TINCRSIZE);
			if (bRetVal == FALSE) {
				return NULL;
			}
		}
		else {
			return NULL;
		}
	}

	TDATA* lpRetMem = m_queMemPool.front();
	m_queMemPool.pop();

	return lpRetMem;
}
#pragma warning (default : 4127)


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemPoolEx::ReleaseMemory
//*---------------------------------------------------------------
// DESC : �޸� Ǯ�� �Ѱ��� �޸𸮸� ��ȯ
// PARM :	1 . lpData - ��ȯ�� �޸�
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TDATA, INT TINITSIZE, INT TINCRSIZE>
VOID CMemPoolEx<TDATA, TINITSIZE, TINCRSIZE>::ReleaseMemory(TDATA* lpData)
{
	m_queMemPool.push(lpData);	
}

