#pragma once

#include "SmartPtr.h"
#include "Timer.h"
#include "CriticalSection.h"
#include "EtEngine.h"
#include "EtResource.h"

template < class T >
class CDnUnknownRenderObject : public CSmartPtrBase< T >
{
public:
	CDnUnknownRenderObject( bool bProcess ) 
		: CSmartPtrBase<T>()
	{
		m_bDestroy = false;
		m_bProcess = bProcess;
		if( bProcess ) {
			ScopeLock<CSyncLock> Lock(s_Lock);
			s_pVecProcessList.push_back( static_cast< T *>( this ) );
		}
	}
	virtual ~CDnUnknownRenderObject()
	{
		SetProcess( false );
	}

	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}

	virtual void SetProcess( bool bFlag ) 
	{
		if( m_bProcess == bFlag ) return;
		ScopeLock<CSyncLock> Lock(s_Lock);
		m_bProcess = bFlag;

		if( m_bProcess == false ) {
			for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) {
				if( s_pVecProcessList[i] == this ) {
					s_pVecProcessList.erase( s_pVecProcessList.begin() + i );
					break;
				}
			}
		}
		else {
			s_pVecProcessList.push_back( static_cast< T *>( this ) );
		}
	}
	bool IsProcess() { return m_bProcess; }

	bool IsDestroy() { return m_bDestroy; }
	virtual void SetDestroy() { m_bDestroy = true; }
	static unsigned int GetProcessCount() { return (int)s_pVecProcessList.size(); }
	static void DeleteAllProcessObject() {
		for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) {
			s_pVecProcessList[i]->Release();
			i--;
		}
	};

protected:
	bool m_bProcess;
	bool m_bDestroy;

public:
	static std::vector<T *> s_pVecProcessList;
	static CSyncLock s_Lock;
};

#define DECL_DN_SMART_PTR_STATIC( name, count ) \
	std::vector<name *> CDnUnknownRenderObject<name>::s_pVecProcessList; \
	CSyncLock CDnUnknownRenderObject<name>::s_Lock; \
	DECL_SMART_PTR_STATIC( name, count ) 
