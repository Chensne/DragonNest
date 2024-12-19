#include "StdAfx.h"
#include "EtCustomRender.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

std::vector<CEtCustomRender *> CEtCustomRender::s_pVecStaticList;
bool CEtCustomRender::s_bEnableCustomRender = true;
CSyncLock CEtCustomRender::s_CustomLock;

bool CustomRenderSort( CEtCustomRender *pLeft, CEtCustomRender *pRight )
{
	return pLeft->GetCustomRenderDepth() > pRight->GetCustomRenderDepth();
}

CEtCustomRender::CEtCustomRender(void)
{
	m_bShow = true;
	m_fCustomRenderDepth  = 0.f;

	s_CustomLock.Lock();
	s_pVecStaticList.push_back( this );
	s_CustomLock.UnLock();	
}

CEtCustomRender::~CEtCustomRender(void)
{	
	s_CustomLock.Lock();
	std::vector<CEtCustomRender *>::iterator it;
	it = std::find( s_pVecStaticList.begin(), s_pVecStaticList.end(), this );
	if( it != s_pVecStaticList.end() ) {
		s_pVecStaticList.erase( it );
	}
	s_CustomLock.UnLock();
}

void CEtCustomRender::RenderCustomList( float fElapsedTime )
{
	if( !s_bEnableCustomRender )
		return;

	s_CustomLock.Lock();
	std::stable_sort( s_pVecStaticList.begin(), s_pVecStaticList.end(), CustomRenderSort );

	for( DWORD i=0; i<s_pVecStaticList.size(); i++ ) {
		if( !s_pVecStaticList[i]->IsEnable() ) continue;
		s_pVecStaticList[i]->RenderCustom( fElapsedTime );
	}
	s_CustomLock.UnLock();
}

void CEtCustomRender::EraseCustomRenderList()
{
	s_CustomLock.Lock();
	std::vector<CEtCustomRender *>::iterator it;
	it = std::find( s_pVecStaticList.begin(), s_pVecStaticList.end(), this );
	if( it != s_pVecStaticList.end() ) {
		s_pVecStaticList.erase( it );
	}
	s_CustomLock.UnLock();
}
