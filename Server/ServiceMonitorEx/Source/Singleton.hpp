/*//===============================================================

	<< SINGLETON >>

	PRGM : B4nFter

	FILE : SINGLETON.HPP
	DESC : �̱��� ���� ����
	INIT BUILT DATE : 2006. 02. 18
	LAST BUILT DATE : 2007. 06. 19

	P.S.>
		- ���1 (���ø� Ŭ���� ���) �� ���2 (��ũ�� �Լ� ���) �ΰ����� ����
		- ���1 �� ���2 �� �ϳ��� ����ؾ� ��
		- ���1 ��� �� ��ü�� �ϳ� �������� �����ؾ� ��

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

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
// DESC : Singleton Ŭ����
// PRGM : B4nFter
// P.S.>
//		- ���1 : ���ø� Ŭ����
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
// DESC : ������
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
// DESC : �Ҹ���
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
// DESC : �̱������� �����ϴ� ��ü ���� ��ȯ
// PARM : N/A
// RETV : ��ü ����
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
// DESC : �̱������� �����ϴ� ��ü ������ ��ȯ
// PARM : N/A
// RETV : ��ü ������
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
// DESC : Singleton ��ũ�� �Լ�
// PRGM : B4nFter
// P.S.>
//		- ���2 : ��ũ�� �Լ�
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
// DESC : Singleton ��ũ�� �Լ� (������)
// PRGM : B4nFter
// P.S.>
//		- ���3 : ��ũ�� �Լ� (������)
//		- Ư�� Ŭ������ ����� �̱��� ��ü�� ����� ������ Ȱ���ϱ� ����
//		- More Effective C++ P199
//		- �⺻ Ŭ������ DECLARE_SINGLETON_PTR_CLASS / DEFINE_SINGLETON_PTR_CLASS / INIT_SINGLETON_PTR_CLASS �� �̱��� ������ ����
//		- �Ļ� Ŭ������ INSTANCE_SINGLETON_PTR_CLASS / DEFINE_SINGLETON_CLASS �� �̱��� ����
//		- �Ļ��� �ܰ� ���� ��üȭ�� �ʿ��ϴٸ� INSTANCE_SINGLETON_CLASS / DEFINE_SINGLETON_CLASS ��� �ܰ躰�� ��ü ������ ��
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
// DESC : Singleton ��ũ�� �Լ� (������)
// PRGM : B4nFter
// P.S.>
//		- ���4 : ��ũ�� �Լ� (������)
//		- Ư�� Ŭ������ ����� �̱��� ��ü�� ����� ������ Ȱ���ϸ鼭 �� �ܰ� ���� �̱��� ��ü�� Ȱ���ϱ� ����
//		- �⺻ Ŭ������ DECLARE_SINGLETON_PTR_BASE_CLASS / DEFINE_SINGLETON_PTR_BASE_CLASS / INIT_SINGLETON_PTR_CLASS �� �̱��� ������ ����
//		- �Ļ� Ŭ������ DECLARE_SINGLETON_PTR_DERV_CLASS / DEFINE_SINGLETON_PTR_DERV_CLASS / INIT_SINGLETON_PTR_CLASS �� �̱��� ������ ����
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

