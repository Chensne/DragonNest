#include "StdAfx.h"
#include "ObjectAnimation.h"
#include "GlobalValue.h"
#include "EtViewer.h"

CObjectAnimation::CObjectAnimation()
{
	m_Type = CObjectBase::ANIMATION;
	m_bParentTypeSkin = false;

	m_nTotalAni = 0;
	m_nCurAni = 0;
	m_nBoneCount = 0;
}

CObjectAnimation::~CObjectAnimation()
{
	RemoveAnimation();
}

void CObjectAnimation::Process( LOCAL_TIME LocalTime )
{

}

void CObjectAnimation::SetTreeName( CString &szStr )
{
	if( !m_pTreeCtrl ) return;
	m_pTreeCtrl->SetItemText( m_ItemID, szStr );
	m_szName = szStr;
}

bool CObjectAnimation::LoadAnimation( CString &szFileName )
{
	if( !m_szFileName.IsEmpty() ) 
	{
		CObjectBase* pParent = GetParent();
		if( pParent == NULL ) return false;

		if( pParent->GetType() == CObjectBase::SKIN )
		{
			CObjectSkin *pSkin = (CObjectSkin*)pParent;
			if( !pSkin ) return false;
			pSkin->RemoveAnimation( m_szFileName );
		}
		else if( pParent->GetType() == CObjectBase::SCENE )
		{
			DWORD dwChildCount = pParent->GetChildCount();
			for( int i=0; i<dwChildCount; i++ )
			{
				CObjectBase* pChild = pParent->GetChild( i );
				if( pChild && pChild->GetType() == CObjectBase::SKIN )
				{
					CObjectSkin* pSkin = (CObjectSkin*)pChild;
					if( pSkin == NULL ) continue;
					if( pSkin->GetSkinType() == CObjectSkin::SKIN_EARRING || pSkin->GetSkinType() == CObjectSkin::SKIN_WING || pSkin->GetSkinType() == CObjectSkin::SKIN_TAIL )
						pSkin->LinkAccessoryParts();
					if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
					pSkin->RemoveAnimation( m_szFileName );
				}	
			}
		}
	}

	EtAniHandle AniHandle = EternityEngine::LoadAni( szFileName );
	if( !AniHandle ) return false;
	m_nBoneCount = AniHandle->GetBoneCount();
	SAFE_RELEASE_SPTR( AniHandle );

	m_szFileName = szFileName;

	return true;
}

void CObjectAnimation::RemoveAnimation()
{
	if( !m_szFileName.IsEmpty() ) 
	{
		CObjectBase* pParent = GetParent();
		if( pParent == NULL ) return;

		if( pParent->GetType() == CObjectBase::SKIN )
		{
			CObjectSkin *pSkin = (CObjectSkin*)pParent;
			if( !pSkin ) return;
			pSkin->RemoveAnimation( m_szFileName );
		}
		else if( pParent->GetType() == CObjectBase::SCENE )
		{
			DWORD dwChildCount = pParent->GetChildCount();
			for( int i=0; i<dwChildCount; i++ )
			{
				CObjectBase* pChild = pParent->GetChild( i );
				if( pChild && pChild->GetType() == CObjectBase::SKIN )
				{
					CObjectSkin* pSkin = (CObjectSkin*)pChild;
					if( pSkin == NULL ) continue;
					if( pSkin->GetSkinType() == CObjectSkin::SKIN_EARRING || pSkin->GetSkinType() == CObjectSkin::SKIN_WING || pSkin->GetSkinType() == CObjectSkin::SKIN_TAIL )
						pSkin->LinkAccessoryParts();
					if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
					pSkin->RemoveAnimation( m_szFileName );
				}	
			}
		}
	}
	m_szFileName.Empty();
}

void CObjectAnimation::Activate()
{
	CObjectBase::Activate();
	if( !m_ItemID ) return;

	EtAniHandle AniHandle = EternityEngine::LoadAni( m_szFileName );
	if( !AniHandle ) return;

	m_szVecAniName.clear();

	m_nTotalAni = AniHandle->GetAniCount();
	m_nCurAni = ( m_nTotalAni == 0 ) ? -1 : 0;
	for( int i=0; i<m_nTotalAni; i++ ) {
		m_nVecTotalFrame.push_back( AniHandle->GetAniLength(i) );
	}

	for( int i=0; i<m_nTotalAni; i++ ) {
		m_szVecAniName.push_back( AniHandle->GetAniName(i) );
	}
	SAFE_RELEASE_SPTR( AniHandle );

	ChangeTreeName();

	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return;
		if( pSkin->LoadAnimation( m_szFileName ) == false ) 
		{
			m_szFileName.Empty();
			CGlobalValue::GetInstance().RemoveObjectFromTreeID( m_ItemID );
		}
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
				if( pSkin->GetSkinType() == CObjectSkin::SKIN_EARRING || pSkin->GetSkinType() == CObjectSkin::SKIN_WING || pSkin->GetSkinType() == CObjectSkin::SKIN_TAIL )
					pSkin->LinkAccessoryParts();
				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
				if( pSkin->LoadAnimation( m_szFileName ) == false ) 
				{
					m_szFileName.Empty();
					CGlobalValue::GetInstance().RemoveObjectFromTreeID( m_ItemID );
				}
			}	
		}
	}
}

void CObjectAnimation::Play( int nIndex, int nLoopCount, float fFrame )
{
	m_nCurAni = nIndex;

	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return;
		pSkin->Play( m_szFileName, m_nCurAni, nLoopCount, fFrame );
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) 
				{
					DWORD dwChildCount = pSkin->GetChildCount();
					for( int j=0; j<dwChildCount; j++ )
					{
						CObjectBase* pChild = pSkin->GetChild( j );
						if( pChild && pChild->GetType() == CObjectBase::ANIMATION )
						{
							CObjectAnimation* pAnimation = (CObjectAnimation*)pChild;
							if( pAnimation )
								pAnimation->Play( pAnimation->GetCurAniIndex(), nLoopCount, fFrame );
						}
					}
					continue;
				}
				pSkin->Play( m_szFileName, m_nCurAni, nLoopCount, fFrame );
			}	
		}
	}

	ChangeTreeName();
}

void CObjectAnimation::Stop()
{
	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return;
		pSkin->Stop();
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS )
				{
					DWORD dwChildCount = pSkin->GetChildCount();
					for( int j=0; j<dwChildCount; j++ )
					{
						CObjectBase* pChild = pSkin->GetChild( j );
						if( pChild && pChild->GetType() == CObjectBase::ANIMATION )
						{
							CObjectAnimation* pAnimation = (CObjectAnimation*)pChild;
							if( pAnimation )
								pAnimation->Stop();
						}
					}
					continue;
				}

				pSkin->Stop();
			}	
		}
	}
}

void CObjectAnimation::Pause()
{
	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return;
		pSkin->Pause();
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS )
				{
					DWORD dwChildCount = pSkin->GetChildCount();
					for( int j=0; j<dwChildCount; j++ )
					{
						CObjectBase* pChild = pSkin->GetChild( j );
						if( pChild && pChild->GetType() == CObjectBase::ANIMATION )
						{
							CObjectAnimation* pAnimation = (CObjectAnimation*)pChild;
							if( pAnimation )
								pAnimation->Pause();
						}
					}
					continue;
				}

				pSkin->Pause();
			}	
		}
	}
}

void CObjectAnimation::Resume()
{
	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return;
		pSkin->Resume();
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS )
				{
					DWORD dwChildCount = pSkin->GetChildCount();
					for( int j=0; j<dwChildCount; j++ )
					{
						CObjectBase* pChild = pSkin->GetChild( j );
						if( pChild && pChild->GetType() == CObjectBase::ANIMATION )
						{
							CObjectAnimation* pAnimation = (CObjectAnimation*)pChild;
							if( pAnimation )
								pAnimation->Resume();
						}
					}
				}

				pSkin->Resume();
			}	
		}
	}
}

bool CObjectAnimation::IsPlay()
{
	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return false;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return false;
		return pSkin->IsPlay();
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
				return pSkin->IsPlay();
			}	
		}
	}

	return false;
}

bool CObjectAnimation::IsPause()
{
	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return false;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return false;
		return pSkin->IsPause();
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
				return pSkin->IsPause();
			}	
		}
	}

	return false;
}

void CObjectAnimation::ChangeTreeName()
{
	char szTemp[512];
	char szFileName[512];
	_GetFullFileName( szFileName, _countof(szFileName), m_szFileName );
	sprintf_s( szTemp, "%s [%3d/%3d] - %s(Bone:%d)", szFileName, m_nCurAni == -1 ? 0 : ( m_nCurAni + 1 ), m_nTotalAni, m_nTotalAni == 0 ? 0 : m_szVecAniName[m_nCurAni], m_nBoneCount );
	SetTreeName( CString(szTemp) );
}

void CObjectAnimation::LinkAniInfo( bool bEnable )
{
	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return;
		pSkin->LinkAniInfo( bEnable, this );
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
				pSkin->LinkAniInfo( bEnable, this );
			}	
		}
	}
}

float CObjectAnimation::GetCurFrame()
{
	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return 0;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return 0;
		int nAniIndex = pSkin->GetChildAniIndex( m_szFileName, m_nCurAni );
		if( nAniIndex == -1 || nAniIndex != pSkin->GetCurAniIndex() ) return 0.f;
		return pSkin->GetCurFrame();
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
				int nAniIndex = pSkin->GetChildAniIndex( m_szFileName, m_nCurAni );
//				if( nAniIndex == -1 || nAniIndex != pSkin->GetCurAniIndex() ) continue;
				return pSkin->GetCurFrame();
			}	
		}
	}
}

int CObjectAnimation::GetTotalFrame()
{
	if( m_nCurAni == -1 ) return 0;
	return m_nVecTotalFrame[m_nCurAni];
}

void CObjectAnimation::SetCurFrame( float fFrame )
{
	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return;
		pSkin->SetCurFrame( fFrame );
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
//				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
				pSkin->SetCurFrame( fFrame );
			}	
		}
	}
}

void CObjectAnimation::SetLoopCount( int nLoopCount )
{
	CObjectBase* pParent = GetParent();
	if( pParent == NULL ) return;

	if( pParent->GetType() == CObjectBase::SKIN )
	{
		CObjectSkin *pSkin = (CObjectSkin*)pParent;
		if( !pSkin ) return;
		pSkin->SetLoopCount( nLoopCount );
	}
	else if( pParent->GetType() == CObjectBase::SCENE )
	{
		DWORD dwChildCount = pParent->GetChildCount();
		for( int i=0; i<dwChildCount; i++ )
		{
			CObjectBase* pChild = pParent->GetChild( i );
			if( pChild && pChild->GetType() == CObjectBase::SKIN )
			{
				CObjectSkin* pSkin = (CObjectSkin*)pChild;
				if( pSkin == NULL ) continue;
//				if( pSkin->GetSkinType() != CObjectSkin::SKIN_PARTS ) continue;
				pSkin->SetLoopCount( nLoopCount );
			}	
		}
	}
}

void CObjectAnimation::ExportObject( FILE *fp, int &nCount )
{
	fwrite( &m_Type, sizeof(int), 1, fp );
	CObjectBase* pParent = GetParent();
	if( pParent )
	{
		if( pParent->GetType() == CObjectBase::SKIN )
			m_bParentTypeSkin = true;
		else if( pParent->GetType() == CObjectBase::SCENE )
			m_bParentTypeSkin = false;
	}
	fwrite( &m_bParentTypeSkin , sizeof(bool), 1, fp );
	WriteCString( &m_szFileName, fp );
	fwrite( &m_nCurAni, sizeof(int), 1, fp );
	float fCurFrame = GetCurFrame();
	fwrite( &fCurFrame, sizeof(float), 1, fp );

	nCount++;

	CObjectBase::ExportObject( fp, nCount );
}

void CObjectAnimation::ImportObject( FILE *fp )
{
	fread( &m_bParentTypeSkin, sizeof(bool), 1, fp );
	ReadCString( &m_szFileName, fp );
	fread( &m_nLoadedCurAni, sizeof(int), 1, fp );
	fread( &m_fLoadedCurFrame, sizeof(float), 1, fp );
	LoadAnimation( m_szFileName );
}

void CObjectAnimation::GetChildFileName( std::vector<CString> &szVecResult )
{
	if( !m_szFileName.IsEmpty() ) szVecResult.push_back( m_szFileName );

	CObjectBase::GetChildFileName( szVecResult );
}
