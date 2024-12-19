#pragma once

#include <stdio.h>
#include "AssertX.h"

template < class T > class CSingleton
{
public:
	CSingleton()
	{
#pragma warning( disable:4311 4312 )
		ASSERT( !s_pObj && "중복 생성 되었습니다." );

#if defined( WIN64 )
		s_pObj = static_cast<T*>(this);
#else // #if defined( WIN64 )
		int offset = ( int )( T * )1 - ( int )( CSingleton < T > * )( T * )1;
		s_pObj = ( T * )( ( int )this + offset );
#endif // #if defined( WIN64 )
#pragma warning( default:4311 4312 )
	}
	virtual ~CSingleton()
	{
		ASSERT( s_pObj );
		s_pObj = NULL;
	}

protected:
	static T* s_pObj;

public:
	static T& GetInstance()
	{
		ASSERT( s_pObj );
		return ( *s_pObj );
	}
	static T *GetInstancePtr()
	{
		return s_pObj;
	}
	static bool CreateInstance()
	{
		ASSERT( !s_pObj );
		T *pTemp = new T;
		ASSERT( pTemp );
		return true;
	}
	static void DestroyInstance()
	{
		ASSERT( s_pObj );
		delete s_pObj;
		s_pObj = NULL;
	}
	static bool IsActive()
	{
		return ( s_pObj ) ? true : false;
	}
};

template < class T > T *CSingleton < T >::s_pObj = 0;


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : DECLARE_SINGLETON_CLASS
//*---------------------------------------------------------------
// DESC : Singleton 매크로 함수
// PRGM : B4nFter
// P.S.>
//		- 방법2 : 매크로 함수
//*---------------------------------------------------------------
#define DECLARE_SINGLETON_CLASS(pCLASS)								\
private:															\
	static pCLASS	m_Singleton;									\
public:																\
	static pCLASS&	GetInstance();									\
	static pCLASS*	GetInstancePtr();

#define DEFINE_SINGLETON_CLASS(pCLASS)								\
	pCLASS pCLASS::m_Singleton;										\
	pCLASS&	pCLASS::GetInstance()		{ return m_Singleton; }		\
	pCLASS*	pCLASS::GetInstancePtr()	{ return &m_Singleton; }

// /GL 옵션 적용 시 C4744 경고가 발생 (_tmain()) 하여 inline 되지 않도록 별도 구현
/*
#define DECLARE_SINGLETON_CLASS(pCLASS)								\
private:															\
	static pCLASS	m_Singleton;									\
public:																\
	static pCLASS&	GetInstance()		{ return m_Singleton; }		\
	static pCLASS*	GetInstancePtr()	{ return &m_Singleton; }

#define DEFINE_SINGLETON_CLASS(pCLASS)								\
	pCLASS pCLASS::m_Singleton;
*/

