#include "StdAfx.h"
#include "ActionElement.h"
#include "ActionObject.h"
#include "ActionSignal.h"
#include "UserMessage.h"

#include "MainFrm.h"
#include "resource.h"
#include "PaneDefine.h"
#include "GlobalValue.h"

#include "ModifyLinkAniDlg.h"
#include "ModifyLengthDlg.h"

#include "SignalCustomRender.h"

CActionElement::CActionElement()
{
	SetType( CActionBase::ELEMENT );
	m_dwLength = 10;
	m_szLinkAniName.Empty();
	m_szNextActionName.Empty();
	m_dwBlendFrame = 0;
	m_dwNextActionFrame = 0;
}

CActionElement::~CActionElement()
{
}

void CActionElement::Process( LOCAL_TIME LocalTime )
{
	if( IsFocus() ) {
		CWnd *pWnd = GetPaneWnd( EVENT_PANE );
		if( pWnd ) pWnd->SendMessage( UM_EVENT_PANE_UPDATE_FRAME );

		CActionBase::Process( LocalTime );
	}
}

void CActionElement::InitPropertyInfo()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );

	PropertyGridBaseDefine Default[] = {
		{ "Common", "Link Animation", CUnionValueProperty::Integer_Combo, NULL, TRUE },
		{ "Common", "Length", CUnionValueProperty::Integer, "길이|1|2000|1", TRUE },
		{ "Common", "Next Action", CUnionValueProperty::Integer_Combo, NULL, TRUE },
		{ "Common", "Blend Length", CUnionValueProperty::Integer, "길이|0|60|1", TRUE },
		{ "Common", "Next Action Frame", CUnionValueProperty::Integer, "Frame|0|200|1", TRUE },
#ifdef _DIRECT_NEXT_ACTION_NAME
		{ "Common", "Next Action (Direct)", CUnionValueProperty::String, "Next Action 직접 지정 (값이 들어가면 콤보박스 값은 무시됩니다.)", TRUE },
#endif
		NULL,
	};

	CActionObject *pParent = (CActionObject *)GetParent();
	// Ani List Combo 박스에 넣어주기 
	m_szAniNameTemp.Empty();
	char szTemp[65535] = { 0, };
	CString szStr;
	sprintf_s( szTemp, "Select Animation|None" );
	for( int i=0; i<pParent->GetAniCount(); i++ ) {
		szStr.Format( "|%s", pParent->GetAniName(i) );
		strcat_s( szTemp, szStr );
	}
	m_szAniNameTemp = szTemp;
	Default[0].szDescription = m_szAniNameTemp.GetBuffer();

	// Action List Combo 박스에 넣어주기
	m_szActionNameTemp.Empty();
	sprintf_s( szTemp, "Select Action|None" );
	for( DWORD i=0; i<pParent->GetChildCount(); i++ ) {
		szStr.Format( "|%s", pParent->GetChild(i)->GetName() );
		strcat_s( szTemp, szStr );
	}
	m_szActionNameTemp = szTemp;

	Default[2].szDescription = m_szActionNameTemp.GetBuffer();

	AddPropertyInfo( Default );
}

#ifdef _DIRECT_NEXT_ACTION_NAME
void CActionElement::SetDirectActionName(bool bSet)
{
	if (bSet)
	{
		m_pVecPropertyList[2]->SetVariable(0);
		m_pVecPropertyList[5]->SetVariable(m_szNextActionName.GetBuffer());
	}
	else
	{
		m_pVecPropertyList[5]->SetVariable("");
	}
}
#endif

void CActionElement::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			{
				bool bReadOnly = false;
				if( m_szLinkAniName.IsEmpty() ) {
					pVariable->SetVariable(0);
					bReadOnly = false;
				}
				else {
					CActionObject *pParent = (CActionObject *)GetParent();
					int nIndex = pParent->GetAniIndex( m_szLinkAniName );
					pVariable->SetVariable( nIndex + 1 );
					bReadOnly = true; 
				}
				CActionObject *pParent = (CActionObject *)GetParent();
				pParent->SetTotalFrame( m_dwLength );

				m_pVecPropertyList[1]->SetDefaultEnable( !bReadOnly );
			}
			break;
		case 1:
			pVariable->SetVariable( (int)m_dwLength );
			break;
		case 2:
			{
				if( m_szNextActionName.IsEmpty() || strcmp( m_szNextActionName, "None" ) == NULL ) pVariable->SetVariable(0);
				else {
					CActionBase *pBase = GetParent()->GetChildFromName( m_szNextActionName );
					if( pBase == NULL ) {
						pVariable->SetVariable(0);
#ifdef _DIRECT_NEXT_ACTION_NAME
						SetDirectActionName(true);
#else
						assert(0);
#endif
						return;
					}
					for( int i=0;; i++ ) {
						const char *pStr = _GetSubStrByCount( i, m_szActionNameTemp.GetBuffer() );
						if( pStr == NULL ) break;
						if( strcmp( pStr, m_szNextActionName ) == NULL ) {
							pVariable->SetVariable( i - 1 );

							// Next Action Frame 최대치값 다시 설정
							char szTemp[64];
							sprintf_s( szTemp, "Frame|0|%d|1", ((CActionElement*)pBase)->GetLength() );
							m_pVecPropertyList[4]->SetSubDescription( szTemp, true );
							break;
						}
					}
				}
			}
			break;
		case 3:
			pVariable->SetVariable( (int)m_dwBlendFrame );
			break;
		case 4:
			pVariable->SetVariable( (int)m_dwNextActionFrame );
			break;
#ifdef _DIRECT_NEXT_ACTION_NAME
		case 5:
			{
				CActionBase *pBase = GetParent()->GetChildFromName( m_szNextActionName );
				SetDirectActionName(pBase == NULL);
			}
			break;
#endif
	}
}

void CActionElement::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			{
				bool bReadOnly = false;
				if( pVariable->GetVariableInt() == 0 ) {
					m_szLinkAniName.Empty();
					bReadOnly = false;
				}
				else {
					CActionObject *pParent = (CActionObject *)GetParent();
					DWORD dwOldLength = m_dwLength;
					CString szOldAniName = m_szLinkAniName;

					m_szLinkAniName = pParent->GetAniName( pVariable->GetVariableInt() - 1 );
					m_dwLength = pParent->GetAniLength( m_szLinkAniName ) - 1;
					m_pVecPropertyList[1]->SetVariable( (int)m_dwLength );
					bReadOnly = true;

					if( dwOldLength > m_dwLength )
						RequeueSignal( dwOldLength, m_dwLength, 1 );

					CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
					if( pWnd ) pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 1 );
				}

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				if( pWnd ) pWnd->SendMessage( UM_PROPERTY_PANE_SET_READONLY, 1, bReadOnly );

				pWnd = GetPaneWnd( EVENT_PANE );
				if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, (WPARAM)this );

				CGlobalValue::GetInstance().SetControlObject( this );

			}
			break;
		case 1:
			{
				m_dwLength = pVariable->GetVariableInt();

				DWORD dwMaxLength = 0;
				for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
					int nStartFrame = ((CActionSignal*)m_pVecChild[i])->GetStartFrame();
					int nEndFrame = ((CActionSignal*)m_pVecChild[i])->GetEndFrame();
					if( nStartFrame + ( nEndFrame - nStartFrame ) > (int)dwMaxLength ) 
						dwMaxLength = nStartFrame + ( nEndFrame - nStartFrame );
				}
				if( m_dwLength < dwMaxLength ) {
					m_dwLength = dwMaxLength;
					pVariable->SetVariable( (int)m_dwLength );

					CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
					if( pWnd ) pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 1 );
				}

				CWnd *pWnd = GetPaneWnd( EVENT_PANE );
				if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, (WPARAM)this );
			}
			break;
		case 2:
			{
				int nIndex = pVariable->GetVariableInt();

				for( int i=0;; i++ ) {
					const char *pStr = _GetSubStrByCount( i, m_szActionNameTemp.GetBuffer() );
					if( pStr == NULL ) break;
					if( i - 1 == nIndex ) {
						m_szNextActionName = pStr;
						if( strcmp( m_szNextActionName.GetBuffer(), "None" ) == NULL ) m_szNextActionName.Empty();

						// Next Action Frame 최대치값 다시 설정
						CActionBase *pBase = GetParent()->GetChildFromName( m_szNextActionName );
						if( pBase ) {
							char szTemp[64];
							sprintf_s( szTemp, "Frame|0|%d|1", ((CActionElement*)pBase)->GetLength() );
							m_pVecPropertyList[4]->SetSubDescription( szTemp, true );

							CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
							if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)this );
						}
					}
				}
			}
			break;
		case 3:
			m_dwBlendFrame = pVariable->GetVariableInt();
			break;
		case 4:
			m_dwNextActionFrame = pVariable->GetVariableInt();
			break;
#ifdef _DIRECT_NEXT_ACTION_NAME
		case 5:
			{
				const char* pStr = pVariable->GetVariableString();
				if (pStr == NULL || pStr[0] == '\0')
					break;

				ChangeNextActionName(m_szNextActionName.GetBuffer(), pStr);

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)this );
			}
			break;
#endif
	}
}

void CActionElement::OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
}


bool CActionElement::IsCanPlay()
{
	CActionObject *pObject = (CActionObject *)GetParent();

	if( NULL == pObject )
		return false;

	return pObject->IsCanPlay( m_szLinkAniName );
}

void CActionElement::Play( int nLoopCount, float fFrame )
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		pObject->Play( m_szLinkAniName, nLoopCount, fFrame );
		pObject->SetTotalFrame( (int)m_dwLength );
	}
}

void CActionElement::Stop()
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		pObject->Stop();
	}
}

void CActionElement::Pause()
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		pObject->Pause();
	}
}

void CActionElement::Resume()
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		pObject->Resume();
	}
}

void CActionElement::SetCurFrame( float fFrame )
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		pObject->SetCurFrame( fFrame );
	}
}

float CActionElement::GetCurFrame()
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		return pObject->GetCurFrame();
	}
	return 0.f;
}

float CActionElement::GetPrevFrame()
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		return pObject->GetPrevFrame();
	}
	return 0.f;
}

bool CActionElement::IsStop()
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		return pObject->IsStop();
	}
	return true;
}

bool CActionElement::IsPause()
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		return pObject->IsPause();
	}
	return true;
}


void CActionElement::SetPlay()
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		pObject->SetPlay( m_szLinkAniName );
	}
}

bool CActionElement::IsLoop()
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		if( pObject->GetLoopCount() > 1 ) return true;
		return false;
	}
	return false;
}

void CActionElement::SetLoop( bool bLoop )
{
	if( IsCanPlay() ) {
		CActionObject *pObject = (CActionObject *)GetParent();
		if( bLoop == true )
			pObject->SetLoopCount( INT_MAX );
		else pObject->SetLoopCount( 1 );
	}

}

bool CActionElement::ExportObject( FILE *fp, int &nCount )
{
	WriteCString( &m_szName, fp );

	WriteCString( &m_szLinkAniName, fp );
	fwrite( &m_dwLength, sizeof(DWORD), 1, fp );

	WriteCString( &m_szNextActionName, fp );
	fwrite( &m_dwBlendFrame, sizeof(DWORD), 1, fp );
	fwrite( &m_dwNextActionFrame, sizeof(DWORD), 1, fp );


	DWORD dwCount = (DWORD)m_pVecChild.size();
	fwrite( &dwCount, sizeof(int), 1, fp );

	return CActionBase::ExportObject( fp, nCount );


}

bool CActionElement::ImportObject( FILE *fp, int nVersion )
{
	ReadCString( &m_szName, fp );

	ReadCString( &m_szLinkAniName, fp );
	fread( &m_dwLength, sizeof(DWORD), 1, fp );

	ReadCString( &m_szNextActionName, fp );
	fread( &m_dwBlendFrame, sizeof(DWORD), 1, fp );
	fread( &m_dwNextActionFrame, sizeof(DWORD), 1, fp );
//	if( strcmp( m_szNextActionName.GetBuffer(), "None" ) == NULL ) m_szNextActionName.Empty();

		if (nVersion >= 2)
		{
			fread( &dwUnkAct2, sizeof(DWORD), 1, fp );
		}
		if (nVersion >= 3)
		{
			fread( &dwUnkAct3, sizeof(DWORD), 1, fp );
		}
		//rlkt_act4
		if (nVersion >= 4)
		{
			fread(&bUnkAct4, sizeof(bool), 1, fp);
		}

		//rlkt_act5
		if (nVersion >= 5)
		{
			fread(&dwUnkAct5, sizeof(DWORD), 1, fp);
		}

	DWORD dwCount;
	fread( &dwCount, sizeof(int), 1, fp );

	for( DWORD i=0; i<dwCount; i++ ) {
		CActionSignal *pSignal = new CActionSignal;
		pSignal->SetParent( this );
		AddChild( pSignal );
		if( pSignal->ImportObject( fp ) == false ) {
			RemoveChild( pSignal );
			continue;
		}
	}

	// 에니메이션 길이가 틀려졌거나 있던 에니메이션이 없어졌을 경우 처리해준다.
	if( !m_szLinkAniName.IsEmpty() ) {
		if( ((CActionObject*)GetParent())->GetAniIndex( m_szLinkAniName ) == -1 ) {
			CModifyLinkAniDlg Dlg;
			Dlg.SetActionName( m_szName );
			Dlg.SetOriginalName( m_szLinkAniName );
			if( Dlg.DoModal() == IDOK ) {
				m_szLinkAniName = Dlg.m_szResultString;
				if( strcmp( m_szLinkAniName, " None" ) == NULL ) {
					m_szLinkAniName.Empty();
				}
			}
			else {
				return false;
			}
		}

		if( !m_szLinkAniName.IsEmpty() ) {
			DWORD dwLength = ((CActionObject*)GetParent())->GetAniLength( m_szLinkAniName ) - 1;
			if( m_dwLength > dwLength ) {
				if( CModifyLengthDlg::s_bYesAll == false ) {
					CModifyLengthDlg Dlg;
					Dlg.SetActionName( m_szName );
					Dlg.DoModal();
				}
				m_dwLength = dwLength;
				RequeueSignal( m_dwLength, dwLength, CModifyLengthDlg::s_nType );
			}
			else if( m_dwLength < dwLength ) {
				CString szString;
				szString.Format( "다음 액션의 에니메이션 길이가 길어졌습니다.\n\nAction Name : %s\nAnimation Name : %s\n\n기존 : %d Frame -> 현제 : %d Frame\n액션의 길이를 에니메이션 길이에 마춰 늘릴까요?\n", m_szName, m_szLinkAniName, m_dwLength, dwLength );
				if( MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, szString, "Question", MB_YESNO ) == IDYES ) {
					m_dwLength = dwLength;
				}
			}
		}
	}

	return true;
}

bool CActionElement::CheckImportLinkAni( FILE *fp , int nVersion)
{
#ifdef _CHECK_WALKFRONT
	if (ImportForCheck(fp, nVersion) == false)
		return false;
#else
	ReadCString( &m_szName, fp );

	ReadCString( &m_szLinkAniName, fp );
	fread( &m_dwLength, sizeof(DWORD), 1, fp );

	ReadCString( &m_szNextActionName, fp );
	fread( &m_dwBlendFrame, sizeof(DWORD), 1, fp );
	fread( &m_dwNextActionFrame, sizeof(DWORD), 1, fp );

	DWORD dwCount;
	fread( &dwCount, sizeof(int), 1, fp );

	for( DWORD i=0; i<dwCount; i++ ) {
		CActionSignal *pSignal = new CActionSignal;
		pSignal->SetParent( this );
		AddChild( pSignal );
		if( pSignal->ImportObject( fp ) == false ) {
			RemoveChild( pSignal );
			continue;
		}
	}
#endif
	if( m_szLinkAniName.IsEmpty() ) return false;
	return true;
}

#ifdef _CHECK_WALKFRONT
bool CActionElement::ImportForCheck(FILE *fp , int nVersion)
{
	ReadCString( &m_szName, fp );

	ReadCString( &m_szLinkAniName, fp );
	fread( &m_dwLength, sizeof(DWORD), 1, fp );

	ReadCString( &m_szNextActionName, fp );
	fread( &m_dwBlendFrame, sizeof(DWORD), 1, fp );
	fread( &m_dwNextActionFrame, sizeof(DWORD), 1, fp );
	
		if (nVersion >= 2)
		{
			fread( &dwUnkAct2, sizeof(DWORD), 1, fp );
		}
		if (nVersion >= 3)
		{
			fread( &dwUnkAct3, sizeof(DWORD), 1, fp );
		}
		//rlkt_act4
		if (nVersion >= 4)
		{
			fread(&bUnkAct4, sizeof(bool), 1, fp);
		}

		//rlkt_act5
		if (nVersion >= 5)
		{
			fread(&dwUnkAct5, sizeof(DWORD), 1, fp);
		}

	DWORD dwCount;
	fread( &dwCount, sizeof(int), 1, fp );

	for( DWORD i=0; i<dwCount; i++ ) {
		CActionSignal *pSignal = new CActionSignal;
		pSignal->SetParent( this );
		AddChild( pSignal );
		if( pSignal->ImportObject( fp ) == false ) {
			RemoveChild( pSignal );
			continue;
		}
	}

	return true;
};
#endif

void CActionElement::RequeueSignal( int nSourceLength, int nModifyLength, int nType )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		CActionSignal *pSignal = (CActionSignal *)m_pVecChild[i];

		switch( nType ) {
			case 0:
				if( pSignal->GetStartFrame() + ( pSignal->GetEndFrame() - pSignal->GetStartFrame() ) <= nModifyLength ) continue;
				RemoveChild( pSignal );
				i--;
				break;
			case 1:
				if( pSignal->GetStartFrame() + ( pSignal->GetEndFrame() - pSignal->GetStartFrame() ) <= nModifyLength ) continue;
				if( pSignal->GetStartFrame() > nModifyLength ) 
					pSignal->SetStartFrame( nModifyLength - 1 );
				pSignal->SetEndFrame( nModifyLength );
				while(1) {
					if( IsOverlapOtherChild(i) == false ) break;
					pSignal->SetYOrder( pSignal->GetYOrder() + 1 );
				}
				break;
			case 2:
				break;
		}
	}
}

bool CActionElement::IsOverlapOtherChild( DWORD dwChildIndex )
{
	CActionSignal *pSignal = (CActionSignal *)m_pVecChild[dwChildIndex];

	bool bOverlap = false;
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( dwChildIndex == i ) continue;
		CActionSignal *pChild = (CActionSignal *)m_pVecChild[i];
		if( pSignal->GetStartFrame() >= pChild->GetStartFrame() && pSignal->GetStartFrame() < pChild->GetEndFrame() && pSignal->GetYOrder() == pChild->GetYOrder() ) {
			bOverlap = true;
			break;
		}
		if( pChild->GetEndFrame() >= pSignal->GetStartFrame() && pChild->GetStartFrame() < pSignal->GetEndFrame() && pSignal->GetYOrder() == pChild->GetYOrder() ) {
			bOverlap = true;
			break;
		}
	}
	return bOverlap;
}

void CActionElement::ChangeNextActionName( const char *szPrevName, const char *szNewName )
{
	if( strcmp( szPrevName, m_szNextActionName ) != NULL ) return;
	m_szNextActionName = szNewName;
}


CActionElement &CActionElement::operator = ( CActionElement &e )
{
	m_szLinkAniName = e.m_szLinkAniName;
	m_dwLength = e.m_dwLength;
	m_szNextActionName = e.m_szNextActionName;
	m_dwBlendFrame = e.m_dwBlendFrame;
	m_dwNextActionFrame = e.m_dwNextActionFrame;

	m_szAniNameTemp = e.m_szAniNameTemp;
	m_szActionNameTemp = e.m_szActionNameTemp;

	*(CActionBase*)this = *(CActionBase*)&e;

	return *this;
}

void CActionElement::ReloadSCR()
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		CActionSignal *pSignal = (CActionSignal *)m_pVecChild[i];
		if( pSignal->GetCustomRender() ) {
			pSignal->GetCustomRender()->Release();
		}
	}
	CEtResource::FlushWaitDelete();
	//g_ActionCoreMng.FlushWaitDelete();
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		CActionSignal *pSignal = (CActionSignal *)m_pVecChild[i];
		if( pSignal->GetCustomRender() ) {
			pSignal->GetCustomRender()->OnModify();
		}
	}
}