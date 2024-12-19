/*//===============================================================

	<< SINGLETON >>

	PRGM : B4nFter

	FILE : SINGLETON.HPP
	DESC : 싱글턴 패턴 구현
	INIT BUILT DATE : 2006. 02. 18
	LAST BUILT DATE : 2007. 06. 19

	P.S.>
		- 방법1 (템플릿 클래스 사용) 과 방법2 (매크로 함수 사용) 두가지가 있음
		- 방법1 과 방법2 중 하나만 사용해야 함
		- 방법1 사용 시 객체를 하나 수동으로 생성해야 함

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "SYSBASEBASE.H"


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CSingleton
//*---------------------------------------------------------------
// DESC : Singleton 클래스
// PRGM : B4nFter
// P.S.>
//		- 방법1 : 템플릿 클래스
//*---------------------------------------------------------------
template<typename Derived>
class CSingleton
{
private:
	static Derived* m_pSingleton;

protected:
	CSingleton();
	virtual ~CSingleton();

public:
	inline static Derived& GetInstance();
	inline static Derived* GetInstancePtr();
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

template<typename Derived>
Derived* CSingleton<Derived>::m_pSingleton = NULL;


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSingleton::CSingleton
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename Derived>
CSingleton<Derived>::CSingleton()
{
	size_t nOffset = (size_t)(Derived*) 1 - (size_t)(CSingleton<Derived>*)(Derived*) 1;
	m_pSingleton = (Derived*)((size_t)this + nOffset);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSingleton::~CSingleton
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename Derived>
CSingleton<Derived>::~CSingleton()
{
    m_pSingleton = 0;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSingleton::GetInstance
//*---------------------------------------------------------------
// DESC : 싱글턴으로 관리하는 객체 참조 반환
// PARM : N/A
// RETV : 객체 참조
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename Derived>
inline Derived& CSingleton<Derived>::GetInstance()
{
    return (*m_pSingleton);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CSingleton::GetInstancePtr
//*---------------------------------------------------------------
// DESC : 싱글턴으로 관리하는 객체 포인터 반환
// PARM : N/A
// RETV : 객체 포인터
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename Derived>
inline Derived* CSingleton<Derived>::GetInstancePtr()
{
    return (m_pSingleton);
}


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
	static pCLASS&	GetInstance()		{ return m_Singleton; }		\
	static pCLASS*	GetInstancePtr()	{ return &m_Singleton; }

#define DEFINE_SINGLETON_CLASS(pCLASS)								\
	pCLASS pCLASS::m_Singleton;


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : DECLARE_SINGLETON_PTR_CLASS
//*---------------------------------------------------------------
// DESC : Singleton 매크로 함수 (포인터)
// PRGM : B4nFter
// P.S.>
//		- 방법3 : 매크로 함수 (포인터)
//		- 특정 클래스를 상속한 싱글턴 객체가 상속의 이점을 활용하기 위함
//		- More Effective C++ P199
//		- 기본 클래스는 DECLARE_SINGLETON_PTR_CLASS / DEFINE_SINGLETON_PTR_CLASS / INIT_SINGLETON_PTR_CLASS 로 싱글턴 포인터 선언
//		- 파생 클래스는 INSTANCE_SINGLETON_PTR_CLASS / DEFINE_SINGLETON_CLASS 로 싱글턴 정의
//		- 파생의 단계 마다 객체화가 필요하다면 INSTANCE_SINGLETON_CLASS / DEFINE_SINGLETON_CLASS 대신 단계별로 객체 선언해 줌
//*---------------------------------------------------------------
#define DECLARE_SINGLETON_PTR_CLASS(pCLASS)							\
protected:															\
	static pCLASS*	m_pSingleton;									\
public:																\
	static pCLASS*	GetInstancePtr()	{ return m_pSingleton; }

#define INSTANCE_SINGLETON_PTR_CLASS(pCLASS)						\
protected:															\
	static pCLASS	m_Singleton;

#define DEFINE_SINGLETON_PTR_CLASS(pCLASS)							\
	pCLASS* pCLASS::m_pSingleton = NULL;

#define INIT_SINGLETON_PTR_CLASS(pCLASS)							\
	pCLASS::m_pSingleton = this;


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : DECLARE_SINGLETON_PTR_BASE_CLASS / DECLARE_SINGLETON_PTR_DERV_CLASS
//*---------------------------------------------------------------
// DESC : Singleton 매크로 함수 (포인터)
// PRGM : B4nFter
// P.S.>
//		- 방법4 : 매크로 함수 (포인터)
//		- 특정 클래스를 상속한 싱글턴 객체가 상속의 이점을 활용하면서 각 단계 마다 싱글턴 객체를 활용하기 위함
//		- 기본 클래스는 DECLARE_SINGLETON_PTR_BASE_CLASS / DEFINE_SINGLETON_PTR_BASE_CLASS / INIT_SINGLETON_PTR_CLASS 로 싱글턴 포인터 선언
//		- 파생 클래스는 DECLARE_SINGLETON_PTR_DERV_CLASS / DEFINE_SINGLETON_PTR_DERV_CLASS / INIT_SINGLETON_PTR_CLASS 로 싱글턴 포인터 선언
//*---------------------------------------------------------------
#define DECLARE_SINGLETON_PTR_BASE_CLASS(pCLASS)					\
protected:															\
	static pCLASS*	m_pSingleton;									\
	static pCLASS	m_Singleton;									\
public:																\
	static pCLASS*	GetInstancePtr()	{ return static_cast<pCLASS*>(m_pSingleton); }

#define DECLARE_SINGLETON_PTR_DERV_CLASS(pCLASS)					\
protected:															\
	static pCLASS	m_Singleton;									\
public:																\
	static pCLASS*	GetInstancePtr()	{ return static_cast<pCLASS*>(m_pSingleton); }

#define DEFINE_SINGLETON_PTR_BASE_CLASS(pCLASS)						\
	pCLASS* pCLASS::m_pSingleton = NULL;							\
	pCLASS	pCLASS::m_Singleton;

#define DEFINE_SINGLETON_PTR_DERV_CLASS(pCLASS)						\
	pCLASS	pCLASS::m_Singleton;

