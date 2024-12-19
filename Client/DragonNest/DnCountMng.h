#pragma once
#include "SLMemPool.h"

template<typename TYPE>
class CDnCountMng
{
public:
	CDnCountMng(void);
	virtual ~CDnCountMng(void);

protected:
	std::list<TYPE*> m_listCountDlg;
	CSLMemPool<TYPE> m_CountDlgMemPool;

public:
	void SetDelayTime( float fDelayTime );
	void Process( float fElapsedTime );
	void Clear();
};

template<typename TYPE> 
CDnCountMng<TYPE>::CDnCountMng(void)
{
}

template<typename TYPE> 
CDnCountMng<TYPE>::~CDnCountMng(void)
{
	Clear();
}

template<typename TYPE>
void CDnCountMng<TYPE>::Clear()
{
	std::list<TYPE*>::iterator iter = m_listCountDlg.begin();
	for( ; iter != m_listCountDlg.end(); ++iter )
	{
		(*iter)->Show( false );
		m_CountDlgMemPool.Deallocate( (*iter) );
	}

	m_listCountDlg.clear();
}

template<typename TYPE> 
void CDnCountMng<TYPE>::Process( float fElapsedTime )
{
	std::list<TYPE*>::iterator iter = m_listCountDlg.begin();
	for( ; iter != m_listCountDlg.end(); )
	{
		if( (*iter)->GetDelayTime() <= 0.0f)
		{
			if( !((*iter)->IsShowCount()) )
			{
				(*iter)->Show( false );
				m_CountDlgMemPool.Deallocate( (*iter) );
				iter = m_listCountDlg.erase(iter);
				continue;
			}
			else
			{
				(*iter)->ShowCount( false );
			}
		}

		++iter;
	}
}

template<typename TYPE> 
void CDnCountMng<TYPE>::SetDelayTime( float fDelayTime )
{
	std::list<TYPE*>::iterator iter = m_listCountDlg.begin();
	for( ; iter != m_listCountDlg.end(); ++iter )
	{
		(*iter)->SetDelayTime( fDelayTime );
	}
}