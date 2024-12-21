#include "stdafx.h"
#include <wx/wx.h>
#include "CommandSet.h"
#include "ToolData.h"


// 모든 뷰의 업데이트 커맨드
void CUpdateViewCmd::Excute( void )
{
	m_pCmdReceiver->SetModified( true );
}




// 리소스 등록 커맨드
CRegisterResCmd::CRegisterResCmd( ICmdReceiver* pCmdReceiver,
								  const wxChar* pResourceName, int iResourceKind, 
								  EtVector3& vPos, float fRotation ) : ICommand( pCmdReceiver ),
																	   m_strResName( pResourceName ),
																	   m_iResourceKind( iResourceKind ),
																	   m_vPos( vPos ),
																	   m_fRotation( fRotation )
{
	wxString strTemp;
	switch( iResourceKind )
	{
		case CDnCutSceneData::RT_RES_MAP:
			strTemp.assign( wxT("Map") );
			break;

		case CDnCutSceneData::RT_RES_ACTOR:
			strTemp.assign( wxT("Actor") );
			break;

		//case CDnCutSceneData::RT_RES_MAX_CAMERA:
		//	strTemp.assign( wxT("Camera") );
		//	break;

		case CDnCutSceneData::RT_RES_SOUND:
			strTemp.assign( wxT("Sound") );
			break;
	}

	m_strDesc.Printf( wxT("[Register resource] %s(%s)"), strTemp.c_str(), pResourceName );
}

CRegisterResCmd::~CRegisterResCmd(void)
{
}

// 깊은 복사 생성 함수.
ICommand* CRegisterResCmd::CreateSame( void )
{
	return new CRegisterResCmd( *this );
}

const wxChar* CRegisterResCmd::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


// excute 및 undo 가 끝아면 CObservable 객체가 알아서 등록된 리스너들의 업데이트 함수를 호출해줍니다.
void CRegisterResCmd::Excute( void )
{
	bool bResult = m_pCmdReceiver->RegisterResource( m_strResName.c_str(), m_iResourceKind );
	m_pCmdReceiver->SetRegResPos( m_strResName.c_str(), m_vPos );
	m_pCmdReceiver->SetRegResRot( m_strResName.c_str(), m_fRotation ); 
	
	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


void CRegisterResCmd::Undo( void )
{
	bool bResult = m_pCmdReceiver->UnRegisterResource( m_strResName.c_str(), m_iResourceKind );

	if( bResult )
		m_pCmdReceiver->SetModified( true );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// 리소스 삭제 커맨드
CRemoveRegResCmd::CRemoveRegResCmd( ICmdReceiver* pCmdReceiver,
									const wxChar* pResourceName, int iResourceKind ) : ICommand( pCmdReceiver ),
																					   m_strResName( pResourceName ),
																					   m_iResourceKind( iResourceKind )
{
	wxString strTemp;
	switch( iResourceKind )
	{
		case CDnCutSceneData::RT_RES_MAP:
			strTemp.assign( wxT("Map") );
			break;

		case CDnCutSceneData::RT_RES_ACTOR:
			strTemp.assign( wxT("Actor") );
			break;

		//case CDnCutSceneData::RT_RES_MAX_CAMERA:
		//	strTemp.assign( wxT("Camera") );
		//	break;

		case CDnCutSceneData::RT_RES_SOUND:
			strTemp.assign( wxT("Sound") );
			break;
	}

	m_strDesc.Printf( wxT("[Unregister resource] %s(%s)"), strTemp.c_str(), pResourceName );
}

CRemoveRegResCmd::~CRemoveRegResCmd(void)
{
	for_each( m_vlpActionInfo.begin(), m_vlpActionInfo.end(), DeleteData<ActionInfo*>() );
	for_each( m_vlpKeyInfo.begin(), m_vlpKeyInfo.end(), DeleteData<KeyInfo*>() );
}


ICommand* CRemoveRegResCmd::CreateSame( void )
{
	CRemoveRegResCmd* pNewCmd = new CRemoveRegResCmd( *this );
	pNewCmd->m_vlpActionInfo.clear();
	for( int iAction = 0; iAction < (int)m_vlpActionInfo.size(); ++iAction )
	{
		pNewCmd->m_vlpActionInfo.push_back( new ActionInfo(*m_vlpActionInfo.at(iAction)) );
	}

	return pNewCmd;
}

const wxChar* CRemoveRegResCmd::GetDesc( void ) const
{
	return m_strDesc.c_str();
}

void CRemoveRegResCmd::Excute( void )
{
	m_vPos =m_pCmdReceiver->GetRegResPos( m_strResName.c_str() );
	m_fRotation = m_pCmdReceiver->GetRegResRot( m_strResName.c_str() );

	// 처음에만 리스트를 받아놓으면 된다.
	if( m_vlpActionInfo.empty() )
	{
		int iNumAction = m_pCmdReceiver->GetThisActorsActionNum( m_strResName.c_str() );
		for( int iAction = 0; iAction < iNumAction; ++iAction )
		{
			const ActionInfo* pActionInfo = m_pCmdReceiver->GetThisActorsActionInfoByIndex( m_strResName.c_str(), iAction );
			m_vlpActionInfo.push_back( new ActionInfo( *pActionInfo ) );
		}
	}

	if( m_vlpKeyInfo.empty() )
	{
		int iNumKey = m_pCmdReceiver->GetThisActorsKeyNum( m_strResName.c_str() );
		for( int iKey = 0; iKey < iNumKey; ++iKey )
		{
			const KeyInfo* pKeyInfo = m_pCmdReceiver->GetThisActorsKeyInfoByIndex( m_strResName.c_str(), iKey );
			m_vlpKeyInfo.push_back( new KeyInfo( *pKeyInfo ) );
		}
	}

	bool bResult = m_pCmdReceiver->UnRegisterResource( m_strResName.c_str(), m_iResourceKind );
	
	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


void CRemoveRegResCmd::Undo( void )
{
	bool bResult = m_pCmdReceiver->RegisterResource( m_strResName.c_str(), m_iResourceKind );
	m_pCmdReceiver->SetRegResPos( m_strResName.c_str(), m_vPos );
	m_pCmdReceiver->SetRegResRot( m_strResName.c_str(), m_fRotation );

	int iNumAction = (int)m_vlpActionInfo.size();
	for( int iAction = 0; iAction < iNumAction; ++iAction )
		m_pCmdReceiver->InsertAction( m_vlpActionInfo.at(iAction) );

	int iNumKey = (int)m_vlpKeyInfo.size();
	for( int iKey = 0; iKey < iNumKey; ++iKey )
		m_pCmdReceiver->InsertKey( m_vlpKeyInfo.at(iKey) );
	
	if( bResult ) 
		m_pCmdReceiver->SetModified( true );
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 장면 속성 변경 커맨드
CSceneInfoModify::CSceneInfoModify( ICmdReceiver* pCmdReceiver, SceneInfo* pSceneInfo ) : ICommand( pCmdReceiver ),
																						  m_pSceneInfo( new SceneInfo(*pSceneInfo) ),
																						  m_pPrevSceneInfo( NULL )
{
	//wxCSConv MBConv( wxFONTENCODING_CP949 );

	//wxChar caWActorName[ 256 ];
	//wxChar caWActionName[ 256 ];
	//ZeroMemory( caWActorName, sizeof(caWActorName) );
	//ZeroMemory( caWActionName, sizeof(caWActionName) );

	//MBConv.MB2WC( caWActorName, m_pActionInfo->strActorName.c_str(), 256 );
	//MBConv.MB2WC( caWActionName, m_pActionInfo->strActionName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Scene Info Modify]") );
}

CSceneInfoModify::~CSceneInfoModify( void )
{
	SAFE_DELETE( m_pSceneInfo );
	SAFE_DELETE( m_pPrevSceneInfo );
}



ICommand* CSceneInfoModify::CreateSame( void )
{
	CSceneInfoModify* pCopied = new CSceneInfoModify( *this );
	pCopied->m_pSceneInfo = new SceneInfo( *m_pSceneInfo );

	if( m_pPrevSceneInfo )
		pCopied->m_pPrevSceneInfo = new SceneInfo( *m_pPrevSceneInfo );
	else
		pCopied->m_pPrevSceneInfo = new SceneInfo;

	return pCopied;
}


const wxChar* CSceneInfoModify::GetDesc( void ) const
{
	return m_strDesc.c_str();
}

void CSceneInfoModify::Excute( void )
{
	*m_pPrevSceneInfo = *m_pCmdReceiver->GetSceneInfo();
	bool bResult = m_pCmdReceiver->ModifySceneInfo( m_pSceneInfo );

	if( bResult ) 
		m_pCmdReceiver->SetModified( true );
}


void CSceneInfoModify::Undo( void )
{
	bool bResult = m_pCmdReceiver->ModifySceneInfo( m_pPrevSceneInfo );

	if( bResult )
		m_pCmdReceiver->SetModified( true );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 액션 추가 커맨드
CActionInsertCmd::CActionInsertCmd( ICmdReceiver* pCmdReceiver, 
								    ActionInfo* pAction ) : ICommand( pCmdReceiver ),
									m_pActionInfo( new ActionInfo( *pAction ) )
																		
																													
			
{
	wxCSConv MBConv( wxFONTENCODING_CP949 );
	
	wxChar caWActorName[ 256 ];
	wxChar caWActionName[ 256 ];
	ZeroMemory( caWActorName, sizeof(caWActorName) );
	ZeroMemory( caWActionName, sizeof(caWActionName) );

	MBConv.MB2WC( caWActorName, m_pActionInfo->strActorName.c_str(), 256 );
	MBConv.MB2WC( caWActionName, m_pActionInfo->strActionName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Insert action] %s(%s)"), caWActorName,
													 caWActionName );
}

CActionInsertCmd::~CActionInsertCmd(void)
{
	SAFE_DELETE( m_pActionInfo );
}


ICommand* CActionInsertCmd::CreateSame( void )
{
	CActionInsertCmd* pCopied = new CActionInsertCmd( *this );
	pCopied->m_pActionInfo = new ActionInfo( *m_pActionInfo );
	
	return pCopied;
}

const wxChar* CActionInsertCmd::GetDesc( void ) const
{
	return m_strDesc.c_str();
}

void CActionInsertCmd::Excute( void )
{
	bool bResult = m_pCmdReceiver->InsertAction( m_pActionInfo );

	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


void CActionInsertCmd::Undo( void )
{
	bool bResult = m_pCmdReceiver->RemoveAction( m_pActionInfo->iID );

	if( bResult ) 
		m_pCmdReceiver->SetModified( true );
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 액터 속성 변경 커맨드
CActorResPropChange::CActorResPropChange( ICmdReceiver* pCmdReceiver, const wxChar* pOldActorName, const wxChar* pNewActorName,
										  const EtVector3& vPos, float fRotation, bool bFitYPosYoMap, 
										  bool bInfluenceLightmap, int iMonsterTableIDAsBoss, bool bScaleLock ) : ICommand( pCmdReceiver ),
																												m_strOldActorName( pOldActorName ),
																												m_strNewActorName( pNewActorName ),
																												m_vPos( vPos ),
																												m_fRotation( fRotation ),
																												m_bFitYPosToMap( bFitYPosYoMap ),
																												m_bInfluenceLightmap( bInfluenceLightmap ),
																												m_iMonsterTableIDAsBoss( iMonsterTableIDAsBoss ),
																												m_iOldMonsterTableIDAsBoss( 0 ),
																												m_bScaleLock( bScaleLock )
																													
{
	m_strDesc.Printf( wxT("[Actor prop change] %s"), m_strNewActorName.c_str() );
}


CActorResPropChange::~CActorResPropChange( void )
{

}


ICommand* CActorResPropChange::CreateSame( void )
{
	return new CActorResPropChange( *this );
}


const wxChar* CActorResPropChange::GetDesc( void ) const
{
	return m_strDesc.c_str();
}



void CActorResPropChange::Excute( void )
{
	bool bResult = false;

	m_vOldPos = m_pCmdReceiver->GetRegResPos( m_strOldActorName.c_str() );
	m_fOldRotation = m_pCmdReceiver->GetRegResRot( m_strOldActorName.c_str() );
	m_bOldFitYPosToMap = m_pCmdReceiver->GetThisActorsFitYPosToMap( m_strOldActorName.c_str() );
	m_bOldInfluenceLightmap = m_pCmdReceiver->GetThisActorsInfluenceLightmap( m_strOldActorName.c_str() );
	m_iOldMonsterTableIDAsBoss = m_pCmdReceiver->GetMonsterTableID( m_strOldActorName.c_str() );
	m_bOldScaleLock = m_pCmdReceiver->GetThisActorsScaleLock( m_strOldActorName.c_str() );
	
	if( m_strOldActorName != m_strNewActorName )
	{
		bResult = m_pCmdReceiver->ChangeRegResName( m_strOldActorName.c_str(), m_strNewActorName.c_str() );
	}
	else
		bResult = true;
	
	if( bResult )
		bResult = m_pCmdReceiver->SetRegResPos( m_strNewActorName.c_str(), m_vPos );

	if( bResult )
		bResult = m_pCmdReceiver->SetRegResRot( m_strNewActorName.c_str(), m_fRotation );

	if( bResult )
		bResult = m_pCmdReceiver->SetActorsFitYPosToMap( m_strNewActorName.c_str(), m_bFitYPosToMap );

	if( bResult )
		bResult = m_pCmdReceiver->SetActorsInfluenceLightmap( m_strNewActorName.c_str(), m_bInfluenceLightmap );

	if( bResult )
		bResult = m_pCmdReceiver->SetMonsterTableID( m_strNewActorName.c_str(), m_iMonsterTableIDAsBoss );

	if( bResult )
		bResult = m_pCmdReceiver->SetActorsScaleLock( m_strNewActorName.c_str(), m_bScaleLock );

	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


void CActorResPropChange::Undo( void )
{
	bool bResult = false;

	if( m_strOldActorName != m_strNewActorName )
	{
		bResult = m_pCmdReceiver->ChangeRegResName( m_strNewActorName.c_str(), m_strOldActorName.c_str() );
	}
	else
		bResult = true;

	if( bResult )
		bResult = m_pCmdReceiver->SetRegResPos( m_strOldActorName.c_str(), m_vOldPos );

	if( bResult )
		bResult = m_pCmdReceiver->SetRegResRot( m_strOldActorName.c_str(), m_fOldRotation );

	if( bResult )
		bResult = m_pCmdReceiver->SetActorsFitYPosToMap( m_strOldActorName.c_str(), m_bOldFitYPosToMap );

	if( bResult )
		bResult = m_pCmdReceiver->SetActorsInfluenceLightmap( m_strOldActorName.c_str(), m_bOldInfluenceLightmap );

	if( bResult )
		bResult = m_pCmdReceiver->SetMonsterTableID( m_strOldActorName.c_str(), m_iOldMonsterTableIDAsBoss );

	if( bResult ) 
		bResult = m_pCmdReceiver->SetActorsScaleLock( m_strOldActorName.c_str(), m_bOldScaleLock );

	if( bResult )
		m_pCmdReceiver->SetModified( true );
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 액션 속성 변경 커맨드
CActionPropChange::CActionPropChange( ICmdReceiver* pCmdReceiver, ActionInfo* pNewActionInfo ) : ICommand( pCmdReceiver ),
																							     m_NewActionInfo( *pNewActionInfo ),
																								 m_bUndo( false )
{
	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar wcaActorName[ 256 ];
	wxChar wcaActionName[ 256 ];
	ZeroMemory( wcaActorName, sizeof(wcaActorName) );
	ZeroMemory( wcaActionName, sizeof(wcaActionName) );
	MBConv.MB2WC( wcaActorName, m_NewActionInfo.strActorName.c_str(), 256 );
	MBConv.MB2WC( wcaActionName, m_NewActionInfo.strActionName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Action prop changed] %s(%s)"), wcaActionName, wcaActorName );
}


CActionPropChange::~CActionPropChange( void )
{

}

// 깊은 복사 생성 함수.
ICommand* CActionPropChange::CreateSame( void )
{
	return new CActionPropChange( *this );
}


const wxChar* CActionPropChange::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


void CActionPropChange::Excute( void )
{
	const ActionInfo* pActionInfo = m_pCmdReceiver->GetActionInfoByID( m_NewActionInfo.iID );

	assert( pActionInfo );
	if( pActionInfo )
	{
		m_PrevActionInfo = *pActionInfo;
		m_pCmdReceiver->ModifyActionInfo( m_NewActionInfo );

		m_bUndo = false;

		m_pCmdReceiver->SetModified( true );
	}
}


void CActionPropChange::Undo( void )
{
	m_pCmdReceiver->ModifyActionInfo( m_PrevActionInfo );

	m_bUndo = true;

	m_pCmdReceiver->SetModified( true );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 액션 삭제 커맨드
CActionRemoveCmd::CActionRemoveCmd( ICmdReceiver* pCmdReceiver, int iActionID ) : ICommand( pCmdReceiver ), 
																				  m_iActionIDToRemove( iActionID )
																								
{
	const ActionInfo* pActionInfo = TOOL_DATA.GetActionInfoByID( iActionID );
	m_RemoveActionInfo = *pActionInfo;

	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar wcaActorName[ 256 ];
	wxChar wcaActionName[ 256 ];
	ZeroMemory( wcaActorName, sizeof(wcaActorName) );
	ZeroMemory( wcaActionName, sizeof(wcaActionName) );
	MBConv.MB2WC( wcaActorName, pActionInfo->strActorName.c_str(), 256 );
	MBConv.MB2WC( wcaActionName, pActionInfo->strActionName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Action prop removed] %s(%s)"), wcaActionName, wcaActorName );
}


CActionRemoveCmd::~CActionRemoveCmd( void )
{

}

// 깊은 복사 생성 함수.
ICommand* CActionRemoveCmd::CreateSame( void )
{
	return new CActionRemoveCmd( *this );
}


const wxChar* CActionRemoveCmd::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


void CActionRemoveCmd::Excute( void )
{
	bool bResult = m_pCmdReceiver->RemoveAction( m_RemoveActionInfo.iID );
	
	assert( bResult );
	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


void CActionRemoveCmd::Undo( void )
{
	bool bResult = m_pCmdReceiver->InsertAction( &m_RemoveActionInfo );

	assert( bResult );
	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 키 추가 커맨드
CKeyInsertCmd::CKeyInsertCmd( ICmdReceiver* pCmdReceiver, 
							  KeyInfo* pKey ) : ICommand( pCmdReceiver ),
												m_pKeyInfo( new KeyInfo( *pKey ) )



{
	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar caWActorName[ 256 ];
	wxChar caWKeyName[ 256 ];
	ZeroMemory( caWActorName, sizeof(caWActorName) );
	ZeroMemory( caWKeyName, sizeof(caWKeyName) );

	MBConv.MB2WC( caWActorName, m_pKeyInfo->strActorName.c_str(), 256 );
	MBConv.MB2WC( caWKeyName, m_pKeyInfo->strKeyName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Insert Key] %s(%s)"), caWActorName,
					  caWKeyName );
}


CKeyInsertCmd::~CKeyInsertCmd(void)
{
	SAFE_DELETE( m_pKeyInfo );
}


ICommand* CKeyInsertCmd::CreateSame( void )
{
	CKeyInsertCmd* pCopied = new CKeyInsertCmd( *this );
	pCopied->m_pKeyInfo = new KeyInfo( *m_pKeyInfo );

	return pCopied;
}


const wxChar* CKeyInsertCmd::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


void CKeyInsertCmd::Excute( void )
{
	bool bResult = m_pCmdReceiver->InsertKey( m_pKeyInfo );

	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


void CKeyInsertCmd::Undo( void )
{
	bool bResult = m_pCmdReceiver->RemoveKey( m_pKeyInfo->iID );

	if( bResult ) 
		m_pCmdReceiver->SetModified( true );
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 액션 삭제 커맨드
CKeyRemoveCmd::CKeyRemoveCmd( ICmdReceiver* pCmdReceiver, int iKeyID ) : ICommand( pCmdReceiver ), 
																		 m_iKeyIDToRemove( iKeyID )

{
	const KeyInfo* pKeyInfo = TOOL_DATA.GetKeyInfoByID( iKeyID );
	m_RemoveKeyInfo = *pKeyInfo;

	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar wcaActorName[ 256 ];
	wxChar wcaKeyName[ 256 ];
	ZeroMemory( wcaActorName, sizeof(wcaActorName) );
	ZeroMemory( wcaKeyName, sizeof(wcaKeyName) );
	MBConv.MB2WC( wcaActorName, pKeyInfo->strActorName.c_str(), 256 );
	MBConv.MB2WC( wcaKeyName, pKeyInfo->strKeyName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Key prop removed] %s(%s)"), wcaKeyName, wcaActorName );
}


CKeyRemoveCmd::~CKeyRemoveCmd( void )
{

}

// 깊은 복사 생성 함수.
ICommand* CKeyRemoveCmd::CreateSame( void )
{
	return new CKeyRemoveCmd( *this );
}


const wxChar* CKeyRemoveCmd::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


void CKeyRemoveCmd::Excute( void )
{
	bool bResult = m_pCmdReceiver->RemoveKey( m_RemoveKeyInfo.iID );

	assert( bResult );
	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


void CKeyRemoveCmd::Undo( void )
{
	bool bResult = m_pCmdReceiver->InsertKey( &m_RemoveKeyInfo );

	assert( bResult );
	if( bResult )
		m_pCmdReceiver->SetModified( true );
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 키 속성 변경 커맨드
CKeyPropChange::CKeyPropChange( ICmdReceiver* pCmdReceiver, KeyInfo* pNewKeyInfo ) : ICommand( pCmdReceiver ),
																					 m_NewKeyInfo( *pNewKeyInfo ),
																					 m_bUndo( false )
{
	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar wcaActorName[ 256 ];
	wxChar wcaKeyName[ 256 ];
	ZeroMemory( wcaActorName, sizeof(wcaActorName) );
	ZeroMemory( wcaKeyName, sizeof(wcaKeyName) );
	MBConv.MB2WC( wcaActorName, m_NewKeyInfo.strActorName.c_str(), 256 );
	MBConv.MB2WC( wcaKeyName, m_NewKeyInfo.strKeyName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Key prop changed] %s(%s)"), wcaKeyName, wcaActorName );
}


CKeyPropChange::~CKeyPropChange( void )
{

}

// 깊은 복사 생성 함수.
ICommand* CKeyPropChange::CreateSame( void )
{
	return new CKeyPropChange( *this );
}


const wxChar* CKeyPropChange::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


void CKeyPropChange::Excute( void )
{
	const KeyInfo* pKeyInfo = m_pCmdReceiver->GetKeyInfoByID( m_NewKeyInfo.iID );

	assert( pKeyInfo );
	if( pKeyInfo )
	{
		m_PrevKeyInfo = *pKeyInfo;
		if( KeyInfo::MOVE == m_PrevKeyInfo.iKeyType &&
			KeyInfo::MULTI_MOVE == m_NewKeyInfo.iKeyType )
		{
			wxCSConv MBConv( wxFONTENCODING_CP949 );
			wxChar wcaActorName[ 256 ];
			ZeroMemory( wcaActorName, sizeof(wcaActorName) );
			MBConv.MB2WC( wcaActorName, m_NewKeyInfo.strActorName.c_str(), 256 );
			m_NewKeyInfo.vlMoveKeys.front().vPos = m_pCmdReceiver->GetRegResPos( wcaActorName );
		}
			
		m_pCmdReceiver->ModifyKeyInfo( m_NewKeyInfo );
		m_bUndo = false;

		m_pCmdReceiver->SetModified( true );
	}
}


void CKeyPropChange::Undo( void )
{
	m_pCmdReceiver->ModifyKeyInfo( m_PrevKeyInfo );
	m_bUndo = true;

	m_pCmdReceiver->SetModified( true );
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 이벤트 추가 커맨드
CEventInsertCmd::CEventInsertCmd( ICmdReceiver* pCmdReceiver,
								  EventInfo* pEvent ) : ICommand( pCmdReceiver ),
														m_pEventInfo( pEvent->clone() )



{
	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar caWEventName[ 256 ];
	ZeroMemory( caWEventName, sizeof(caWEventName) );

	MBConv.MB2WC( caWEventName, m_pEventInfo->strEventName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Insert Event] %s"), caWEventName );
}



CEventInsertCmd::~CEventInsertCmd(void)
{
	SAFE_DELETE( m_pEventInfo );
}


ICommand* CEventInsertCmd::CreateSame( void )
{
	CEventInsertCmd* pCopied = new CEventInsertCmd( *this );
	pCopied->m_pEventInfo = m_pEventInfo->clone();

	return pCopied;
}


const wxChar* CEventInsertCmd::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


void CEventInsertCmd::Excute( void )
{
	bool bResult = m_pCmdReceiver->InsertEvent( m_pEventInfo );

	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


void CEventInsertCmd::Undo( void )
{
	bool bResult = m_pCmdReceiver->RemoveEvent( m_pEventInfo->iID );

	if( bResult ) 
		m_pCmdReceiver->SetModified( true );
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 이벤트 삭제 커맨드
CEventRemoveCmd::CEventRemoveCmd( ICmdReceiver* pCmdReceiver, int iEventID ) : ICommand( pCmdReceiver ), 
																				m_iEventIDToRemove( iEventID )

{
	const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( iEventID );
	m_pRemoveEventInfo = pEventInfo->clone();

	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar wcaEventName[ 256 ];
	ZeroMemory( wcaEventName, sizeof(wcaEventName) );
	MBConv.MB2WC( wcaEventName, pEventInfo->strEventName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Event prop removed] %s"), wcaEventName );
}


CEventRemoveCmd::~CEventRemoveCmd( void )
{
	SAFE_DELETE( m_pRemoveEventInfo );
}

// 깊은 복사 생성 함수.
ICommand* CEventRemoveCmd::CreateSame( void )
{
  	CEventRemoveCmd* pNewEvent = new CEventRemoveCmd( *this );
	pNewEvent->m_pRemoveEventInfo = m_pRemoveEventInfo->clone();
	
	return pNewEvent;
}


const wxChar* CEventRemoveCmd::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


void CEventRemoveCmd::Excute( void )
{
	bool bResult = m_pCmdReceiver->RemoveEvent( m_pRemoveEventInfo->iID );

	assert( bResult );
	if( bResult )
		m_pCmdReceiver->SetModified( true );
}


void CEventRemoveCmd::Undo( void )
{
	bool bResult = m_pCmdReceiver->InsertEvent( m_pRemoveEventInfo );

	assert( bResult );
	if( bResult )
		m_pCmdReceiver->SetModified( true );
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 이벤트 속성 변경 커맨드
CEventPropChange::CEventPropChange( ICmdReceiver* pCmdReceiver, EventInfo* pNewEventInfo ) : ICommand( pCmdReceiver ),
																							 m_pNewEventInfo( pNewEventInfo->clone() ),
																							 m_pPrevEventInfo( NULL ),
																							 m_bUndo( false )
{
	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar wcaEventName[ 256 ];
	ZeroMemory( wcaEventName, sizeof(wcaEventName) );
	MBConv.MB2WC( wcaEventName, m_pNewEventInfo->strEventName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Event prop changed] %s"), wcaEventName );
}


CEventPropChange::~CEventPropChange( void )
{
	SAFE_DELETE( m_pNewEventInfo );
	SAFE_DELETE( m_pPrevEventInfo );
}

// 깊은 복사 생성 함수.
ICommand* CEventPropChange::CreateSame( void )
{
	CEventPropChange* pNewEvent = new CEventPropChange( *this );
	
	pNewEvent->m_pNewEventInfo = m_pNewEventInfo->clone();

	if( m_pPrevEventInfo )
		pNewEvent->m_pPrevEventInfo = m_pPrevEventInfo->clone();

	return pNewEvent;
}


const wxChar* CEventPropChange::GetDesc( void ) const
{
	return m_strDesc.c_str();
}



void CEventPropChange::Excute( void )
{
	const EventInfo* pEventInfo = m_pCmdReceiver->GetEventInfoByID( m_pNewEventInfo->iID );

	assert( pEventInfo );
	if( pEventInfo )
	{
		SAFE_DELETE( m_pPrevEventInfo );
		m_pPrevEventInfo = pEventInfo->clone();
		m_pCmdReceiver->ModifyEventInfo( m_pNewEventInfo );

		m_pCmdReceiver->SetModified( true );

		m_bUndo = false;
	}
}



void CEventPropChange::Undo( void )
{
	m_pCmdReceiver->ModifyEventInfo( m_pPrevEventInfo );

	m_pCmdReceiver->SetModified( true );

	m_bUndo = true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 일괄 데이터 변경 커맨드
CBatchEdit::CBatchEdit( ICmdReceiver* pCmdReceiver, float fStartTimeDelta,
													const vector<const ActionInfo*> vlpActionInfo, 
													const vector<const KeyInfo*> vlpKeyInfo,
													const vector<const EventInfo*> vlpEventInfo ) : ICommand( pCmdReceiver ),
																									 m_fStartTimeDelta( fStartTimeDelta ),
																									 m_vlpActionInfo( vlpActionInfo ),
																									 m_vlpKeyInfo( vlpKeyInfo ),
																									 m_vlpEventInfo( vlpEventInfo )
		
{
	wxCSConv MBConv( wxFONTENCODING_CP949 );

	wxChar wcaEventName[ 256 ];
	ZeroMemory( wcaEventName, sizeof(wcaEventName) );
	//MBConv.MB2WC( wcaEventName, m_pNewEventInfo->strEventName.c_str(), 256 );

	m_strDesc.Printf( wxT("[Batch data changed]") );
}


CBatchEdit::~CBatchEdit()
{

}


const wxChar* CBatchEdit::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


// 깊은 복사 생성 함수.
ICommand* CBatchEdit::CreateSame( void )
{
	CBatchEdit* pNewCommand = new CBatchEdit( *this );
	return pNewCommand;
}




void CBatchEdit::Excute( void )
{
	int iNumActions = (int)m_vlpActionInfo.size();
	for( int iAction = 0; iAction < iNumActions; ++iAction )
	{
		const ActionInfo* pActionInfo = m_vlpActionInfo.at( iAction );
		ActionInfo Copy = (*pActionInfo);
		
		Copy.fStartTime += m_fStartTimeDelta;
		if( Copy.fStartTime < 0.0f )
			Copy.fStartTime = 0.0f;

		m_pCmdReceiver->ModifyActionInfo( Copy );
	}

	int iNumKeys = (int)m_vlpKeyInfo.size();
	for( int iKey = 0; iKey < iNumKeys; ++iKey )
	{
		const KeyInfo* pKeyInfo = m_vlpKeyInfo.at( iKey );
		KeyInfo Copy = *pKeyInfo;
		
		Copy.fStartTime += m_fStartTimeDelta;
		if( Copy.fStartTime < 0.0f )
			Copy.fStartTime = 0.0f;

		m_pCmdReceiver->ModifyKeyInfo( Copy );
	}

	int iNumEvents = (int)m_vlpEventInfo.size();
	for( int iEvent = 0; iEvent < iNumEvents; ++iEvent )
	{
		const EventInfo* pEventInfo = m_vlpEventInfo.at( iEvent );
		EventInfo* pCopy = pEventInfo->clone();

		pCopy->fStartTime += m_fStartTimeDelta;
		if( pCopy->fStartTime < 0.0f )
			pCopy->fStartTime = 0.0f;

		m_pCmdReceiver->ModifyEventInfo( pCopy );

		delete pCopy;
	}

	m_pCmdReceiver->SetModified( true );
}


void CBatchEdit::Undo( void )
{
	int iNumActions = (int)m_vlpActionInfo.size();
	for( int iAction = 0; iAction < iNumActions; ++iAction )
	{
		const ActionInfo* pActionInfo = m_vlpActionInfo.at( iAction );
		ActionInfo Copy = (*pActionInfo);

		Copy.fStartTime -= m_fStartTimeDelta;			// 아.. 이런 매직넘버들 다 없애야 하는데. -_-
		if( Copy.fStartTime < 0.0f )
			Copy.fStartTime = 0.0f;

		m_pCmdReceiver->ModifyActionInfo( Copy );
	}

	int iNumKeys = (int)m_vlpKeyInfo.size();
	for( int iKey = 0; iKey < iNumKeys; ++iKey )
	{
		const KeyInfo* pKeyInfo = m_vlpKeyInfo.at( iKey );
		KeyInfo Copy = *pKeyInfo;

		Copy.fStartTime -= m_fStartTimeDelta;
		if( Copy.fStartTime < 0.0f )
			Copy.fStartTime = 0.0f;

		m_pCmdReceiver->ModifyKeyInfo( Copy );
	}

	int iNumEvents = (int)m_vlpEventInfo.size();
	for( int iEvent = 0; iEvent < iNumEvents; ++iEvent )
	{
		const EventInfo* pEventInfo = m_vlpEventInfo.at( iEvent );
		EventInfo* pCopy = pEventInfo->clone();

		pCopy->fStartTime -= m_fStartTimeDelta;
		if( pCopy->fStartTime < 0.0f )
			pCopy->fStartTime = 0.0f;

		m_pCmdReceiver->ModifyEventInfo( pCopy );

		delete pCopy;
	}

	m_pCmdReceiver->SetModified( true );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 클립보드에 넣는 커맨드.
CCopyToClipboard::CCopyToClipboard( ICmdReceiver* pCmdReceiver, const ActionInfo* pActionInfo ) : ICommand( pCmdReceiver ), 
																							m_pCopiedActionInfo( new ActionInfo ),
																							m_pCopiedKeyInfo( NULL ),
																							m_pCopiedEventInfo( NULL ),
																							m_iCopyType( COPY_ACTION )
{
	*m_pCopiedActionInfo = *pActionInfo;

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar wcaName[ 256 ];
	ZeroMemory( wcaName, sizeof(wcaName) );
	MBConv.MB2WC( wcaName, m_pCopiedActionInfo->strActionName.c_str(), 256 );
	m_strDesc.Printf( wxT("[Copy to Clipboard] ActionID: %d Name:%s"), m_pCopiedActionInfo->iID, wcaName );
}


CCopyToClipboard::CCopyToClipboard( ICmdReceiver* pCmdReceiver, const KeyInfo* pKeyInfo ) : ICommand( pCmdReceiver ), 
																					  m_pCopiedActionInfo( NULL ),
																					  m_pCopiedKeyInfo( new KeyInfo ),
																					  m_pCopiedEventInfo( NULL ),
																					  m_iCopyType( COPY_KEY )
{
	*m_pCopiedKeyInfo = *pKeyInfo;

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar wcaName[ 256 ];
	ZeroMemory( wcaName, sizeof(wcaName) );
	MBConv.MB2WC( wcaName, m_pCopiedKeyInfo->strKeyName.c_str(), 256 );
	m_strDesc.Printf( wxT("[Copy to Clipboard] KeyID: %d Name:%s"), m_pCopiedKeyInfo->iID, wcaName );
}


CCopyToClipboard::CCopyToClipboard( ICmdReceiver* pCmdReceiver, const EventInfo* pEventInfo ) : ICommand( pCmdReceiver ),
																						  m_pCopiedActionInfo( NULL ),
																						  m_pCopiedKeyInfo( NULL ),
																						  m_pCopiedEventInfo( NULL ),
																						  m_iCopyType( COPY_EVENT )
{
	m_pCopiedEventInfo = pEventInfo->clone();

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar wcaName[ 256 ];
	ZeroMemory( wcaName, sizeof(wcaName) );
	MBConv.MB2WC( wcaName, m_pCopiedEventInfo->strEventName.c_str(), 256 );
	m_strDesc.Printf( wxT("[Copy to Clipboard] EventID: %d Name:%s"), m_pCopiedEventInfo->iID, wcaName );
}

CCopyToClipboard::~CCopyToClipboard( void )
{
	SAFE_DELETE( m_pCopiedActionInfo );
	SAFE_DELETE( m_pCopiedKeyInfo );
	SAFE_DELETE( m_pCopiedEventInfo );
}

ICommand* CCopyToClipboard::CreateSame( void )
{
	CCopyToClipboard* pNewCommand = new CCopyToClipboard( *this );
	
	switch( pNewCommand->GetCopyType() )
	{
		case COPY_ACTION:
			pNewCommand->m_pCopiedActionInfo = new ActionInfo( *m_pCopiedActionInfo );
			break;

		case COPY_KEY:
			pNewCommand->m_pCopiedKeyInfo = new KeyInfo( *m_pCopiedKeyInfo );
			break;

		case COPY_EVENT:
			pNewCommand->m_pCopiedEventInfo = m_pCopiedEventInfo->clone();
			break;
	}

	return pNewCommand;
}

const wxChar* CCopyToClipboard::GetDesc( void ) const
{
	return m_strDesc.c_str();
}

void CCopyToClipboard::Excute( void )
{
	switch( m_iCopyType )
	{
		case COPY_ACTION:
			TOOL_DATA.CopyToClipboard( m_pCopiedActionInfo );
			break;

		case COPY_KEY:
			TOOL_DATA.CopyToClipboard( m_pCopiedKeyInfo );
			break;

		case COPY_EVENT:
			TOOL_DATA.CopyToClipboard( m_pCopiedEventInfo );
			break;
	}
}


void CCopyToClipboard::Undo( void )
{
	// 클립보드 비움
	switch( m_iCopyType )
	{
		case COPY_ACTION:
			TOOL_DATA.ResetActionClipboard();
			break;

		case COPY_KEY:
			TOOL_DATA.ResetKeyClipboard();
			break;

		case COPY_EVENT:
			TOOL_DATA.ResetEventClipboard();
			break;
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 클립보드에서 붙여 넣는 커맨드.
// 실제 붙여넣기가 될 땐 새롭게 Info 데이터가 하나 추가되는 형식으로 된다. id 는 유니크해야하며 
// 데이터도 붙여넣음 당하는 곳에서 실제 사용할 수 없는 데이터는 빼도록 해야한다.. 
CPasteFromClipboard::CPasteFromClipboard( ICmdReceiver* pCmdReceiver, int iCopyType, const char* pActorToPaste, float fTimeToPaste ) : 
																						ICommand( pCmdReceiver ), 
																						m_iCopyType( iCopyType ),
																						m_pCopiedActionInfo( NULL ),
																						m_pCopiedKeyInfo( NULL ),
																						m_pCopiedEventInfo( NULL )
{
	int iID = -1;
	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar wcaName[ 256 ];
	ZeroMemory( wcaName, sizeof(wcaName) );
	
	switch( m_iCopyType )
	{
		case CCopyToClipboard::COPY_ACTION:
			{
				wxASSERT( TOOL_DATA.IsActionCopied() );
				const ActionInfo* pClipboard = TOOL_DATA.GetActionClipboard();
				m_pCopiedActionInfo = new ActionInfo( *pClipboard );
				m_pCopiedActionInfo->strActorName.assign( pActorToPaste );
				m_pCopiedActionInfo->fStartTime = fTimeToPaste;
				iID = m_pCopiedActionInfo->iID = m_pCmdReceiver->GenerateID();
				MBConv.MB2WC( wcaName, pClipboard->strActionName.c_str(), 256 );
			}
			break;

		case CCopyToClipboard::COPY_KEY:
			{
				wxASSERT( TOOL_DATA.IsKeyCopied() );
				const KeyInfo* pClipboard = TOOL_DATA.GetKeyClipboard();
				m_pCopiedKeyInfo = new KeyInfo( *pClipboard );
				m_pCopiedKeyInfo->strActorName.assign( pActorToPaste );
				m_pCopiedKeyInfo->fStartTime = fTimeToPaste;
				iID = m_pCopiedKeyInfo->iID = m_pCmdReceiver->GenerateID();

				if( !m_pCopiedKeyInfo->vlMoveKeys.empty() )
				{
					for( int i = 0; i < (int)m_pCopiedKeyInfo->vlMoveKeys.size(); ++i )
					{
						m_pCopiedKeyInfo->vlMoveKeys.at( i ).iID = TOOL_DATA.GenerateID();
					}
				}

				MBConv.MB2WC( wcaName, pClipboard->strKeyName.c_str(), 256 );
			}
			break;

		case CCopyToClipboard::COPY_EVENT:
			{
				wxASSERT( TOOL_DATA.IsEventCopied() );
				const EventInfo* pClipboard = TOOL_DATA.GetEventClipboard();
				m_pCopiedEventInfo = pClipboard->clone();
				iID = pClipboard->iID;
				m_pCopiedEventInfo->strActorName.assign( pActorToPaste );
				m_pCopiedEventInfo->fStartTime = fTimeToPaste;
				iID = m_pCopiedEventInfo->iID = m_pCmdReceiver->GenerateID();
				MBConv.MB2WC( wcaName, pClipboard->strEventName.c_str(), 256 );
			}
			break;

		default:
			wxASSERT( !wxT("Invalid Copy Type!!") );
			break;
	}

	m_strDesc.Printf( wxT("[Paste from Clipboard] EventID: %d Name:%s"), iID, wcaName );
}


CPasteFromClipboard::~CPasteFromClipboard( void )
{
	SAFE_DELETE( m_pCopiedActionInfo );
	SAFE_DELETE( m_pCopiedKeyInfo );
	SAFE_DELETE( m_pCopiedKeyInfo );
}

ICommand* CPasteFromClipboard::CreateSame( void )
{
	CPasteFromClipboard* pNewCommand = new CPasteFromClipboard( *this );

	switch( pNewCommand->GetCopyType() )
	{
		case CCopyToClipboard::COPY_ACTION:
			pNewCommand->m_pCopiedActionInfo = new ActionInfo( *m_pCopiedActionInfo );
			break;

		case CCopyToClipboard::COPY_KEY:
			pNewCommand->m_pCopiedKeyInfo = new KeyInfo( *m_pCopiedKeyInfo );
			break;

		case CCopyToClipboard::COPY_EVENT:
			pNewCommand->m_pCopiedEventInfo = m_pCopiedEventInfo->clone();
			break;
	}

	return pNewCommand;
}

const wxChar* CPasteFromClipboard::GetDesc( void ) const
{
	return m_strDesc.c_str();
}

void CPasteFromClipboard::Excute( void )
{
	// 붙여넣기 수행
	//TOOL_DATA.Reset
	switch( m_iCopyType )
	{
		case CCopyToClipboard::COPY_ACTION:
			m_pCmdReceiver->InsertAction( m_pCopiedActionInfo );
			break;

		case CCopyToClipboard::COPY_KEY:
			m_pCmdReceiver->InsertKey( m_pCopiedKeyInfo );
			break;

		case CCopyToClipboard::COPY_EVENT:
			m_pCmdReceiver->InsertEvent( m_pCopiedEventInfo );
			break;
	}
	
	TOOL_DATA.SetModified( true );
}


void CPasteFromClipboard::Undo( void )
{
	// 붙여넣기 취소.
	switch( m_iCopyType )
	{
		case CCopyToClipboard::COPY_ACTION:
			m_pCmdReceiver->RemoveAction( m_pCopiedActionInfo->iID );
			break;

		case CCopyToClipboard::COPY_KEY:
			m_pCmdReceiver->RemoveKey( m_pCopiedKeyInfo->iID );
			break;

		case CCopyToClipboard::COPY_EVENT:
			m_pCmdReceiver->RemoveEvent( m_pCopiedEventInfo->iID );
			break;
	}

	TOOL_DATA.SetModified( true );
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 액터를 바꾸는 커맨드. 이 커맨드 오기 전에 바꿀 수 있는지 먼저 검증부터 됩니다.
// 따라서 커맨드 생성이 됐다는 것은 무조건 리소스를 바꿀 수 있다는 것. (애니메이션 호환됨)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 일괄 데이터 변경 커맨드
CChangeActorRes::CChangeActorRes( ICmdReceiver* pCmdReceiver, const wxChar* pNewActorResName, const wxChar* pOldActorName ) : ICommand( pCmdReceiver ),
																															  m_strNewActorResName( pNewActorResName ),
																															  m_strNewActorName( pNewActorResName ),
																															  m_strOldActorName( pOldActorName )
{
	// 나머지 문자열 만들어 둠.
	int iOffset = (int)m_strOldActorName.find_last_of( wxT("_") );
	wxString strUserPostFix = m_strOldActorName.substr( iOffset, m_strOldActorName.length() );
	m_strNewActorName.Append( strUserPostFix );

	m_strOldActorResName = m_strOldActorName.substr( 0, iOffset );

	m_strDesc.Printf( wxT("[%s Actor has changed to %s actor resource]"), m_strOldActorName.c_str(), m_strNewActorResName.c_str() );
}


CChangeActorRes::~CChangeActorRes()
{

}


const wxChar* CChangeActorRes::GetDesc( void ) const
{
	return m_strDesc.c_str();
}


void CChangeActorRes::Excute( void )
{
	m_bIsUndo = false;

	TOOL_DATA.ChangeRegResName( m_strOldActorName.c_str(), m_strNewActorName.c_str() );

	TOOL_DATA.SetModified( true );
}


void CChangeActorRes::Undo( void )
{
	m_bIsUndo = true;

	TOOL_DATA.ChangeRegResName( m_strNewActorName.c_str(), m_strOldActorName.c_str() );

	TOOL_DATA.SetModified( true );
}