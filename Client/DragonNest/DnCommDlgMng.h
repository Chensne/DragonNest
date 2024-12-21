#pragma once
#include "SLMemPool.h"
#include "DnCommDlg.h"

struct SDlgSlotInfo
{
	DWORD dwID;
	DWORD dwSetID;
	CDnCommDlg *pDlg;

	SDlgSlotInfo()
		: dwID(0)
		, dwSetID(0)
		, pDlg(NULL)
	{
	}
};

template<typename TYPE>
class CDnCommDlgMng
{
protected:
	enum
	{
		USER_ID_OFFSET = 2000,
	};

public:
	CDnCommDlgMng(void);
	virtual ~CDnCommDlgMng(void);

protected:
	std::list<TYPE*> m_listDlg;
	CSLMemPool<TYPE> m_DlgMemPool;

	typedef std::deque<SDlgSlotInfo>	DLG_SLOT_DEQ;
	typedef DLG_SLOT_DEQ::iterator		DLG_SLOT_DEQ_ITER;

	DLG_SLOT_DEQ m_dequeEmptySlot;
	DLG_SLOT_DEQ m_dequeUseSlot;

public:
	void SetDelayTime( float fDelayTime );
	void CloseDlg( DWORD dwID, bool bSetID = false );

public:
	void Process( float fElapsedTime );
	void Clear();
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	void ClearNpcTalkTextureWindow();
#endif
};

template<typename TYPE> 
CDnCommDlgMng<TYPE>::CDnCommDlgMng(void)
{
	for( int i=1; i<100; i++ )
	{
		SDlgSlotInfo sDlgSlotInfo;
		sDlgSlotInfo.dwID = i;
		m_dequeEmptySlot.push_back( sDlgSlotInfo );
	}
}

template<typename TYPE> 
CDnCommDlgMng<TYPE>::~CDnCommDlgMng(void)
{
	Clear();

	m_dequeEmptySlot.clear();
	m_dequeUseSlot.clear();
}

template<typename TYPE>
void CDnCommDlgMng<TYPE>::Clear()
{
	std::list<TYPE*>::iterator iter = m_listDlg.begin();
	for( ; iter != m_listDlg.end(); ++iter )
	{
		(*iter)->Show( false );
		m_DlgMemPool.Deallocate( (*iter) );
	}

	m_listDlg.clear();
}

#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
template<typename TYPE>
void CDnCommDlgMng<TYPE>::ClearNpcTalkTextureWindow()
{
	std::list<TYPE*>::iterator iter = m_listDlg.begin();
	for( ; iter != m_listDlg.end(); ++iter )
	{
		if( (*iter)->GetDlgType() != CDnCommDlg::eCommDlgType::eCommDlg_TYPE_NPCTALK)
			continue;

		(*iter)->Show( false );
		m_DlgMemPool.Deallocate( (*iter) );
	}

	m_listDlg.clear();
}
#endif

template<typename TYPE>
void CDnCommDlgMng<TYPE>::Process( float fElapsedTime )
{
	std::list<TYPE*>::iterator iter = m_listDlg.begin();
	for( ; iter != m_listDlg.end(); )
	{
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
		CDnCommDlg::eCommDlgType DlgType = (*iter)->GetDlgType();
		bool bIsAutoCloseDialog = (*iter)->IsAutoCloseDialog();
		if( !bIsAutoCloseDialog && DlgType == CDnCommDlg::eCommDlgType::eCommDlg_TYPE_NPCTALK )
		{
			++iter;
			continue;
		}
#endif
		if( (*iter)->GetDelayTime() <= 0.0f )
		{
			(*iter)->Show( false );
			m_DlgMemPool.Deallocate( (*iter) );
			iter = m_listDlg.erase(iter);
			{
				SDlgSlotInfo sSlotInfo = m_dequeUseSlot.back();
				m_dequeUseSlot.pop_back();

				sSlotInfo.dwSetID = 0;
				sSlotInfo.pDlg->CallBackFunc( sSlotInfo.dwID, sSlotInfo.dwSetID );
				sSlotInfo.pDlg = NULL;

				m_dequeEmptySlot.push_back( sSlotInfo );
			}
			continue;
		}
		++iter;
	}
}

template<typename TYPE>
void CDnCommDlgMng<TYPE>::SetDelayTime( float fDelayTime )
{
	std::list<TYPE*>::iterator iter = m_listDlg.begin();
	for( ; iter != m_listDlg.end(); ++iter )
	{
		(*iter)->SetDelayTime( fDelayTime );
	}
}

template<typename TYPE>
void CDnCommDlgMng<TYPE>::CloseDlg( DWORD dwID, bool bSetID )
{
	CDnCommDlg *pDlg(NULL);

	DLG_SLOT_DEQ_ITER iter = m_dequeUseSlot.begin();

	for( ; iter != m_dequeUseSlot.end(); ++iter )
	{
		if( bSetID )
		{
			if( ((*iter).dwSetID - USER_ID_OFFSET) == dwID )
			{
				pDlg = (*iter).pDlg;
				break;
			}
		}
		else
		{
			if( (*iter).dwID == dwID )
			{
				pDlg = (*iter).pDlg;
				break;
			}
		}
	}

	if( pDlg )
	{
		pDlg->SetDelayTime( 0.0f );
	}
}