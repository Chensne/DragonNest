#include "StdAfx.h"
#include "ActionCommander.h"

CActionCommander s_BrushActionCommander;
CActionCommander s_PropActionCommander;
CActionCommander s_NaviActionCommander;
CActionCommander s_SoundActionCommander;
CActionCommander s_EventActionCommander;


CActionCommander::CActionCommander()
{
	m_nCurIndex = -1;
}

CActionCommander::~CActionCommander()
{
	Reset();
}

void CActionCommander::Reset()
{
	m_nCurIndex = 0;
	SAFE_DELETE_PVEC( m_pVecElement );
}

bool CActionCommander::IsCanUndo()
{
	if( m_pVecElement.size() == 0 ) return false;
	if( m_nCurIndex < 0 || m_nCurIndex > (int)m_pVecElement.size() - 1 ) return false;
	return true;
}

bool CActionCommander::IsCanRedo()
{
	if( m_pVecElement.size() == 0 ) return false;
	if( m_nCurIndex < -1 || m_nCurIndex >= (int)m_pVecElement.size() - 1 ) return false;

	return true;
}

bool CActionCommander::Redo()
{
	if( m_pVecElement.size() == 0 ) return true;
	if( m_nCurIndex < -1 || m_nCurIndex >= (int)m_pVecElement.size() - 1 ) return true;

	m_nCurIndex++;
	if( m_pVecElement[m_nCurIndex]->Redo() == false ) return false;

	return true;
}

bool CActionCommander::Undo()
{
	if( m_pVecElement.size() == 0 ) return true;
	if( m_nCurIndex < 0 || m_nCurIndex > (int)m_pVecElement.size() - 1 ) return true;

	if( m_pVecElement[m_nCurIndex]->Undo() == false ) return false;
	m_nCurIndex--;
	return true;
}

void CActionCommander::Add( CActionElement *pElement )
{
	if( m_nCurIndex < (int)m_pVecElement.size() - 1 ) {
		for( int i=(m_nCurIndex+1); i<(int)m_pVecElement.size(); i++ ) {
			SAFE_DELETE( m_pVecElement[i] );
			m_pVecElement.erase( m_pVecElement.begin() + i );
			i--;
		}
	}

	m_pVecElement.push_back( pElement );
	m_nCurIndex = (int)m_pVecElement.size() - 1;
}

int CActionCommander::GetUndoCount()
{
	return ( m_nCurIndex + 1 );
}

int CActionCommander::GetRedoCount()
{
	return (int)( ( m_pVecElement.size() - (m_nCurIndex+1) ) );
}

CString CActionCommander::GetUndoDesc( int nIndex )
{
	return m_pVecElement[ m_nCurIndex - nIndex ]->GetDesc();
}

CString CActionCommander::GetRedoDesc( int nIndex )
{
	return m_pVecElement[ (m_nCurIndex+1) + nIndex ]->GetDesc();
}


void CActionElement::AddAction()
{
	m_pCommander->Add( this );
}