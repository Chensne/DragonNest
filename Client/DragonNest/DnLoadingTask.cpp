#include "StdAfx.h"
#include "DnLoadingTask.h"
#include "DnFreeCamera.h"
#include "EtPrimitiveDraw.h"
#include "DnInterface.h"
#include <mmsystem.h>
#include "MessageListener.h"
#include "DnVillageTask.h"
#include "DnGameTask.h"
#include "DnTableDB.h"
#include "DnBridgeTask.h"
#include "EtUIDialog.h"
#include "DnActorClassDefine.h"
#include "DnPlayerActor.h"
#include "DnMainFrame.h"
#include "DnGameTipTask.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

/////  MapNameDlg  /////////////////////////////////////////////////////////////////////

void CDnMapNameDlg::InitialUpdate() 
{
	m_pMapName = GetControl<CEtUIStatic>("ID_MAPNAME");
	m_fMinHeight = m_pMapName->GetUICoord().fHeight;
}

void CDnMapNameDlg::Initialize( bool bShow ) 
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MapName.ui" ).c_str(), true );		
}

void CDnMapNameDlg::SetMapName( std::wstring wszMapName )
{
	m_pMapName->SetText( wszMapName, true, 1.2f, 0.8f );

	// 해상도 높은 와이드 4단계에서 로딩스태틱 가운데 픽셀이 어긋나는 현상이 있었다.
	// 상하 분할렌더링때문에 가운데 영역의 높이값이 0.5픽셀로 나오면서 픽셀이 어긋나보이는 현상이었는데,
	// 분할렌더링, bFit 빼버리고 가로가변 넣는게 제일 깔끔하나, 우선은 이렇게 임시로 크기 제한 걸어서 처리하기로 하겠다.
	if( wszMapName.size() && m_pMapName->GetUICoord().fHeight < m_fMinHeight ) {
		m_pMapName->SetSize( m_pMapName->GetUICoord().fWidth, m_fMinHeight );
		m_pMapName->GetFontCoord()->fY -= 2.0f / DEFAULT_UI_SCREEN_HEIGHT;
	}
}

void CDnMapNameDlg::Render( float fElapsedTime )
{
	if( wcslen(m_pMapName->GetText()) > 0 ) {
		CEtUIDialog::Render( fElapsedTime );
	}
}

/////  Loading Task  /////////////////////////////////////////////////////////////////////

CDnLoadingTask::CDnLoadingTask()
{
	m_pMachine = NULL;

	_srand( timeGetTime() );
	m_bEnableBackgroundLoad = false;
	m_bPauseRenderScreen = false;

	SCameraInfo Info;
	Info.fFogNear = 1000000.f;
	Info.fFogFar = 1000000.f;
	Info.fFar = 1000000.f;
	m_hCamera = EternityEngine::CreateCamera( &Info, false );
	m_strTodayTip.clear();

	m_nLoadStartTime = 0;

	m_TodayRect = SUICoord( 0.f, 0.f, 1.f, 1.f );
	m_bBusy = true;
	m_bPauseThreadLoad = false;
	m_pMapName = NULL;
	m_hWaitComplete = ::CreateEvent( NULL, FALSE, FALSE, NULL );	// Auto Reset

	for( int i=0; i<LoadThreadLayer_Amount; i++ )
		m_pVecLoadList[i].reserve( 50000 );

	m_bLastShowCursor = false;
	m_nLoadingAniDelay = 0;
	m_nLoadingAniSpeed = 0;
	m_nLoadingAniFrameCount = 0;
#ifdef PRE_FIX_LOADINGSCREEN
	m_bLoadingScreenRendered = false;
#endif // #ifdef PRE_FIX_LOADINGSCREEN
}

CDnLoadingTask::~CDnLoadingTask()
{
	for( int i=0; i<LoadThreadLayer_Amount; i++ ) {
		for( DWORD j=0; j<m_pVecLoadList[i].size(); j++ ) {
			for( DWORD k=0; k<m_pVecLoadList[i][j]->pVecPacketList.size(); k++ ) {
				SAFE_DELETEA( m_pVecLoadList[i][j]->pVecPacketList[k]->pData );
			}
			SAFE_DELETE_PVEC( m_pVecLoadList[i][j]->pVecPacketList );
		}
		SAFE_DELETE_PVEC( m_pVecLoadList[i] );
	}
	SAFE_RELEASE_SPTR( m_hCamera );
	SAFE_RELEASE_SPTR( m_hTexture );
	SAFE_RELEASE_SPTR( m_hLoadingAniTexture );
	SAFE_DELETE( m_pMapName );
	::CloseHandle( m_hWaitComplete );
}

bool CDnLoadingTask::Initialize( CDnLoadingStateMachine *pMachine, int nMapIndex, int nStageConstructionLevel )
{
	ScopeLock<CSyncLock> Lock(m_MachineLock);
	m_pMachine = NULL;
	if( pMachine == NULL ) return true;
	
	m_strTodayTip.clear();
	m_TodayRect = SUICoord( 0.f, 0.8f, 1.f, 0.2f );
	std::string szLoadingImage;
	int nTipIndex = 0;
	int nLoadingAniTableID = 0;
	DNTableFileFormat* pAniSox = GetDNTable( CDnTableDB::TLOADINGANI );

	if( nMapIndex > 0 ) {
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
#ifdef PRE_FIX_MEMOPT_EXT
		CommonUtil::GetFileNameFromFileEXT(szLoadingImage, pSox, nMapIndex, "_LoadingImage");
#else
		szLoadingImage = pSox->GetFieldFromLablePtr( nMapIndex, "_LoadingImage" )->GetString();
#endif // PRE_FIX_MEMOPT_EXT

		int nTodayTipStringID = GetGameTipTask().GetTodayTipStringID( nMapIndex, CDnBridgeTask::GetInstance().GetCurrentCharLevel() );
		if( nTodayTipStringID > -1 ) {	
			m_strTodayTip = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nTodayTipStringID );			
		}

		int nStringTableID = pSox->GetFieldFromLablePtr( nMapIndex, "_MapNameID" )->GetInteger();
		std::wstring strMapName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID );

		if( m_pMapName == NULL ) {
			m_pMapName = new CDnMapNameDlg;
			m_pMapName->Initialize( true );		
		}
		m_pMapName->SetMapName( strMapName );

		// Loading Ani 셋팅
		std::string szLoadingAniTable;
		if( nStageConstructionLevel == 4 )
			szLoadingAniTable = pSox->GetFieldFromLablePtr( nMapIndex, "_LoadingAniTableIDAbyss" )->GetString();
		else szLoadingAniTable = pSox->GetFieldFromLablePtr( nMapIndex, "_LoadingAniTableIDNormal" )->GetString();

		std::vector<int> nVecResult;
		ParseIndex( szLoadingAniTable, nVecResult );
		if( !nVecResult.empty() )
			nLoadingAniTableID = nVecResult[_rand()%nVecResult.size()];
	}
	else {
		nLoadingAniTableID = pAniSox->GetItemID( _rand()%pAniSox->GetItemCount() );
	}

	SAFE_RELEASE_SPTR( m_hTexture );

	if( !szLoadingImage.empty() ) {
		m_hTexture = EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName( szLoadingImage ).c_str() );
	}

	SAFE_RELEASE_SPTR( m_hLoadingAniTexture );
	if( nLoadingAniTableID > 0 ) {
		std::string szLoadingAni = pAniSox->GetFieldFromLablePtr( nLoadingAniTableID, "_FileName" )->GetString();
		m_nLoadingAniFrameCount = pAniSox->GetFieldFromLablePtr( nLoadingAniTableID, "_FrameCount" )->GetInteger();
		m_nLoadingAniDelay = pAniSox->GetFieldFromLablePtr( nLoadingAniTableID, "_FrameDelay" )->GetInteger();
		m_nLoadingAniSpeed = pAniSox->GetFieldFromLablePtr( nLoadingAniTableID, "_MoveSpeed" )->GetInteger();
		m_hLoadingAniTexture = EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName( szLoadingAni ).c_str() );
	}

	m_pMachine = pMachine;
	m_nLoadStartTime = timeGetTime();

#ifdef PRE_FIX_LOADINGSCREEN
	m_bLoadingScreenRendered = false;
#endif // #ifdef PRE_FIX_LOADINGSCREEN

	return true;
}

int CDnLoadingTask::GetTodayTipStringID( int nTableID, int nLevel )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TTODAYTIP );
	std::vector<int> nVecResult;
	std::string szStr;

	if( !pSox->IsExistItem( nTableID ) ) return -1;

	m_TodayRect.fX = pSox->GetFieldFromLablePtr( nTableID, "_Left" )->GetFloat();
	m_TodayRect.fY = pSox->GetFieldFromLablePtr( nTableID, "_Top" )->GetFloat();
	m_TodayRect.fWidth = pSox->GetFieldFromLablePtr( nTableID, "_Right" )->GetFloat();
	m_TodayRect.fHeight = pSox->GetFieldFromLablePtr( nTableID, "_Bottom" )->GetFloat();

	m_TodayRect.fWidth -= m_TodayRect.fX;
	m_TodayRect.fHeight -= m_TodayRect.fY;

	// 일단 글로벌 스트링 등록
	szStr = pSox->GetFieldFromLablePtr( nTableID, "_GlobalStringIndex" )->GetString();
	ParseIndex( szStr, nVecResult );

	// 레벨 찾아보구 맞는 레벨 넣어준다.
	char szTemp[32];
	int nLevelTableIndex = -1;
	if( nLevel > 0 ) {
		for( int i=0; i<10; i++ ) {
			sprintf_s( szTemp, "_Level%d", i + 1 );
			int nValue = pSox->GetFieldFromLablePtr( nTableID, szTemp )->GetInteger();
			if( nValue == 0 ) break;
			if( nLevel <= nValue ) {
				nLevelTableIndex = i;
				break;
			}
		}
	}
	if( nLevelTableIndex > -1 ) {
		sprintf_s( szTemp, "_Level%dStringIndex", nLevelTableIndex + 1 );
		szStr = pSox->GetFieldFromLablePtr( nTableID, szTemp )->GetString();
		ParseIndex( szStr, nVecResult );
	}

	int nVecCount = static_cast<int>( nVecResult.size() );

	if( nVecCount == 0 )
		return -1;

	return nVecResult[_rand() % nVecCount];
}

void CDnLoadingTask::ParseIndex( std::string &szStr, std::vector<int> &nVecResult )
{
	for( int i=0; ; i++ ) {
		//char *pPtr = _GetSubStrByCount( i, (char*)szStr.c_str(), ';' );
		std::string strValue = _GetSubStrByCountSafe( i, (char*)szStr.c_str(), ';' );
		if( strValue.size() == 0 ) break;
		std::string szTemp = strValue;
		if( szTemp.empty() ) break;

		if( std::find( szTemp.begin(), szTemp.end(), '~' ) == szTemp.end() ) {
			nVecResult.push_back( atoi(szTemp.c_str()) );
		}
		else {
			int nMin = 0, nMax = 0;
			for( int j=0; ; j++ ) {
				//char *pRange = _GetSubStrByCount( j, (char*)szTemp.c_str(), '~' );
				std::string strValue = _GetSubStrByCountSafe( j, (char*)szTemp.c_str(), '~' );
				if( strValue.size() == 0 ) break;
				if( j == 0 ) nMin = atoi(strValue.c_str());
				else if( j == 1 ) nMax = atoi(strValue.c_str() );
			}
			for( int j=nMin; j<=nMax; j++ ) {
				nVecResult.push_back(j);
			}
		}
	}
}

void CDnLoadingTask::WaitForComplete()
{
#if defined( PRE_FIX_CLIENT_FREEZING )
	CommonUtil::PrintFreezingLog( "CDnLoadingTask::WaitForComplete - Before" );
#endif

	::WaitForSingleObject( m_hWaitComplete, INFINITE );

#if defined( PRE_FIX_CLIENT_FREEZING )
	CommonUtil::PrintFreezingLog( "CDnLoadingTask::WaitForComplete - After\n" );
#endif
}

void CDnLoadingTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	UpdateHang();
	if( !m_bEnableBackgroundLoad ) {
		ScopeLock<CSyncLock> Lock( g_pEtRenderLock );
		if( !CDnMouseCursor::GetInstance().IsShowCursor() ) {
			CDnMouseCursor::GetInstance().ShowCursor( true, true );
		}
		CDnMouseCursor::GetInstance().UpdateCursor();
	}

	m_MachineLock.Lock();
	if( m_pMachine ) {
		if( m_pMachine->RenderScreen( LocalTime, fDelta ) == false ) {
			SAFE_RELEASE_SPTR( m_hTexture );
			SAFE_RELEASE_SPTR( m_hLoadingAniTexture );
			m_pMachine = NULL;
			OutputDebug("%d Loading Elapsed Time\n",  ( timeGetTime() - m_nLoadStartTime ) );
			if( m_pMapName ) {
				m_pMapName->SetMapName( std::wstring(L"") );
			}
			::SetEvent( m_hWaitComplete );
		}
		else
		{
			ScopeLock<CSyncLock> Lock( g_pEtRenderLock );
			EtDeviceCheck Check = GetEtDevice()->DeviceValidCheck();
#ifdef PRE_FIX_LOADINGSCREEN
			if( false == m_bLoadingScreenRendered && Check == DC_OK )
#else
			if( Check == DC_OK )
#endif // #ifdef PRE_FIX_LOADINGSCREEN
			{
				EternityEngine::ClearScreen( 0 );
				GetEtDevice()->BeginScene();

				if( m_hCamera ) {
					EtVector4 Vertices[ 4 ];
					EtVector2 TexCoord[ 4 ];
					int nWidth, nHeight;

					nWidth = CEtDevice::GetInstance().Width();
					nHeight = CEtDevice::GetInstance().Height();

					if( m_hTexture ) {
						float fTemp = ( nWidth - ( nHeight / 9.f ) * 16.f ) / 2.f;					
						Vertices[0] = EtVector4( fTemp, 0.f, 0.0f, 0.0f );
						Vertices[1] = EtVector4( nWidth - fTemp, 0.f, 0.0f, 0.0f );
						Vertices[2] = EtVector4( nWidth - fTemp, (float)nHeight, 0.0f, 0.f );
						Vertices[3] = EtVector4( fTemp, (float)nHeight, 0.0f, 0.0f );

						TexCoord[0] = EtVector2( 0.0f, 0.0f );
						TexCoord[1] = EtVector2( 1.0f, 0.0f );
						TexCoord[2] = EtVector2( 1.0f, 1.0f );
						TexCoord[3] = EtVector2( 0.0f, 1.0f );
						EternityEngine::DrawQuad2DWithTex( Vertices, TexCoord, m_hTexture->GetTexturePtr() );
					}

#if !defined(PRE_FIX_LOADINGSCREEN)
					if( m_hLoadingAniTexture ) {
						int nCycleMove = m_nLoadingAniSpeed;
						int nCurTime = timeGetTime();
						int nTime = ( nCurTime - m_nLoadStartTime ) % nCycleMove;
						float fWidthPos = ( ( nWidth + 80.f ) / (float)nCycleMove ) * (float)nTime;

						int nAniFrame = m_nLoadingAniFrameCount;
						int nCycleFrame = m_nLoadingAniDelay;

						nTime = ( nCurTime - m_nLoadStartTime ) % ( nCycleFrame + 1 );
						int nCurFrame = (int)( ( nAniFrame / (float)nCycleFrame ) * (float)nTime );
						float fMin = ( 1.f / nAniFrame ) * nCurFrame;
						float fMax = ( 1.f / nAniFrame ) * ( nCurFrame + 1 );

						Vertices[ 0 ] = EtVector4( nWidth - fWidthPos, nHeight - 80.f, 0.f, 0.f );
						Vertices[ 1 ] = EtVector4( (float)nWidth - fWidthPos + 80.f, nHeight - 80.f, 0.f, 0.f );
						Vertices[ 2 ] = EtVector4( (float)nWidth - fWidthPos + 80.f, (float)nHeight, 0.f, 0.f );
						Vertices[ 3 ] = EtVector4( nWidth - fWidthPos, (float)nHeight, 0.f, 0.f );

						TexCoord[ 0 ] = EtVector2( 0.0f, fMin );
						TexCoord[ 1 ] = EtVector2( 1.0f, fMin );
						TexCoord[ 2 ] = EtVector2( 1.0f, fMax );
						TexCoord[ 3 ] = EtVector2( 0.0f, fMax );

						EternityEngine::DrawQuad2DWithTex( Vertices, TexCoord, m_hLoadingAniTexture->GetTexturePtr() );
					}
#endif // #if defined(!PRE_FIX_LOADINGSCREEN)
					CEtPrimitiveDraw::GetInstance().Render( m_hCamera );
				}
				if( CEtFontMng::IsActive() ) {
					GetEtDevice()->EnableAlphaBlend( true );
					if( !m_strTodayTip.empty() ) {
						int nFontIndex = INT_MAX;						
						SFontDrawEffectInfo Info;
						Info.dwFontColor = 0xFFFFFFFF;
						CEtFontMng::GetInstance().DrawTextW(nFontIndex, 16, m_strTodayTip.c_str(), DT_VCENTER | DT_CENTER | DT_WORDBREAK, m_TodayRect, -1, Info, false);
					}

					if( m_pMapName ) {
						m_pMapName->Process( fDelta );
						m_pMapName->Render( fDelta );
					}

					GetEtDevice()->EnableAlphaBlend( false );
				}

				GetEtDevice()->EndScene();
				GetEtDevice()->ShowFrame( NULL );

#ifdef PRE_FIX_LOADINGSCREEN
				m_bLoadingScreenRendered = true;
#endif // #ifdef PRE_FIX_LOADINGSCREEN
			}
		}
	}
	m_MachineLock.UnLock();

	if( !m_bPauseThreadLoad ) {
		bool bProcess = true;
		if( CDnBridgeTask::IsActive() ) {
			switch( CDnBridgeTask::GetInstance().GetBridgeState() ) {
				case eBridgeState::LoginToVillage:
				case eBridgeState::VillageToVillage:
				case eBridgeState::GameToVillage:
					{
						if( !CDnVillageTask::s_bCompleteFirstRender ) {
							bProcess = false;
						}
					}
					break;
				case eBridgeState::VillageToGame:
				case eBridgeState::GameToGame:
				case eBridgeState::LoginToGame:
					{
						if( !CDnGameTask::s_bCompleteFirstRender ) {
							bProcess = false;
						}
					}
					break;
			}
		}
		if( bProcess ) {
			if( !m_pVecLoadList[0].empty() || !m_pVecLoadList[1].empty() || !m_pVecLoadList[2].empty() ) {
				m_bBusy = true;
			}

			LoadStruct *pLoad;
			for( int l=0; l<LoadThreadLayer_Amount; l++ ) {
				for( DWORD i=0; i<m_pVecLoadList[l].size(); i++ ) {
					pLoad = m_pVecLoadList[l][i];
					bool bSkip = false;
					if( pLoad->pSkipFunc && !pLoad->pVecPacketList.empty()) {
						m_Lock.Lock();
						LoadPacketStruct *pLastPacket = pLoad->pVecPacketList[pLoad->pVecPacketList.size()-1];
						if( pLoad->pSkipFunc( pLastPacket->nMainCmd, pLastPacket->nSubCmd ) == true ) bSkip = true;
						m_Lock.UnLock();
					}
					if( !bSkip )  {

#if defined( PRE_FIX_CLIENT_FREEZING )
						CommonUtil::PrintFreezingLog( "CDnLoadingTask::Process - Load - Before" );
#endif

   						if( pLoad->pFunc( pLoad->pThis, pLoad->pParam, pLoad->nSize, pLoad->LocalTime ) == false ) {
							OutputDebug( "Thread Load Failed!!\n" );
						}

						bool bNeedLock = m_bEnableBackgroundLoad;
						if( bNeedLock && CClientSessionManager::IsActive() ) CClientSessionManager::GetInstance().LockProcessDispatchMessage( true );
						m_Lock.Lock();

						if( !pLoad->pVecPacketList.empty() ) {
							for( int j=(int)pLoad->pVecPacketList.size() - 1; j>=0; j-- ) {
								LoadPacketStruct *pPacket = pLoad->pVecPacketList[j];
								CClientSessionManager::GetInstance().RecvPacket( pPacket->nMainCmd, pPacket->nSubCmd, (char*)pPacket->pData, pPacket->nSize, 0 );
								SAFE_DELETEA( pPacket->pData );
							}
						}
						if( bNeedLock && CClientSessionManager::IsActive() ) CClientSessionManager::GetInstance().LockProcessDispatchMessage( false );
						SAFE_DELETE_PVEC( pLoad->pVecPacketList );

						SAFE_DELETEA( pLoad->pParam );
						SAFE_DELETE( pLoad );

						m_pVecLoadList[l].erase( m_pVecLoadList[l].begin() + i );
						m_Lock.UnLock();

#if defined( PRE_FIX_CLIENT_FREEZING )
						CommonUtil::PrintFreezingLog( "CDnLoadingTask::Process - Load - After\n" );
#endif
					}
					else {
#if defined( PRE_FIX_CLIENT_FREEZING )
						CommonUtil::PrintFreezingLog( "CDnLoadingTask::Process - Skip - Before" );
#endif

						bool bNeedLock = m_bEnableBackgroundLoad;
						if( bNeedLock && CClientSessionManager::IsActive() ) CClientSessionManager::GetInstance().LockProcessDispatchMessage( true );

						m_Lock.Lock();
						if( !pLoad->pVecPacketList.empty() ) {
							for( int j=(int)pLoad->pVecPacketList.size() - 1; j>=0; j-- ) {
								LoadPacketStruct *pPacket = pLoad->pVecPacketList[j];
								SAFE_DELETEA( pPacket->pData );
							}
						}
						if( bNeedLock && CClientSessionManager::IsActive() ) CClientSessionManager::GetInstance().LockProcessDispatchMessage( false );
						SAFE_DELETE_PVEC( pLoad->pVecPacketList );

						SAFE_DELETEA( pLoad->pParam );
						SAFE_DELETE( pLoad );

						m_pVecLoadList[l].erase( m_pVecLoadList[l].begin() + i );
						m_Lock.UnLock();

#if defined( PRE_FIX_CLIENT_FREEZING )
						CommonUtil::PrintFreezingLog( "CDnLoadingTask::Process - Load - After\n" );
#endif
					}
					break;
				}
			}
	    }
	}

	m_bBusy = false;

	return;
}

bool CDnLoadingTask::IsLoading()
{
	if( m_pMachine ) return true;
	for( int i=0; i<LoadThreadLayer_Amount; i++ )
		if( !m_pVecLoadList[i].empty() ) return true;

	return false;
}

#include "DnMainFrame.h"
void CDnLoadingTask::EnableBackgroundLoading( bool bEnable, bool bForceClear )
{
	m_Lock.Lock();	
	m_bEnableBackgroundLoad = bEnable;
	m_bPauseThreadLoad = bForceClear;
	m_Lock.UnLock();

	if( !bEnable ) {
		if( bForceClear ) {
			// 마지막 돌던건 끝날때까지 대기.
			while(m_bBusy) { Sleep(1); }
			for( int iLoadLayer=0; iLoadLayer<LoadThreadLayer_Amount; iLoadLayer++ ) {
				for( DWORD i=0; i<m_pVecLoadList[iLoadLayer].size(); i++ ) {
					LoadStruct *pLoad = m_pVecLoadList[iLoadLayer][i];

					for( DWORD iPacket=0; iPacket<pLoad->pVecPacketList.size(); iPacket++ ) {
						SAFE_DELETEA( pLoad->pVecPacketList[iPacket]->pData );
					}
					SAFE_DELETE_PVEC( pLoad->pVecPacketList );

					SAFE_DELETEA( pLoad->pParam );
					SAFE_DELETE( pLoad );
				}
				m_pVecLoadList[iLoadLayer].clear();
			}
		}
		else {
			for( int iLoadLayer=0; iLoadLayer<LoadThreadLayer_Amount; iLoadLayer++ ) {
				while( !m_pVecLoadList[iLoadLayer].empty() ) {
					if( m_bDestroyTask || !m_bUseThread ) break;
					Sleep(1);
				}
			}
		}
	}

	if( GetFocus() != CDnMainFrame::GetInstance().GetHWnd() ) {
		CInputDevice::GetInstance().ResetAllInput();
		if( CDnMouseCursor::GetInstancePtr() ) {
			CDnMouseCursor::GetInstance().ShowCursor( true, true );
		}
	}
	else {
		CDnMouseCursor::GetInstance().ShowCursor( false, true );
	}
}

void CDnLoadingTask::InsertLoadObject( bool (__stdcall *Func)( void *pThis, void *pParam, int nParamSize, LOCAL_TIME LocalTime ), int (__stdcall *GetFunc)( void *pParam, int nParamSize ), bool (__stdcall *CheckFunc)( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nParamSize ), bool (__stdcall *SkipFunc)( int nMainCmd, int nSubCmd ), void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime, int nLayerIndex )
{
	ScopeLock<CSyncLock> Lock(m_Lock);
	if( m_bEnableBackgroundLoad ) {
		LoadStruct *pStruct = new LoadStruct;
		pStruct->pThis = pThis;
		pStruct->pFunc = Func;
		pStruct->pCheckFunc = CheckFunc;
		pStruct->pGetFunc = GetFunc;
		pStruct->pSkipFunc = SkipFunc;
		pStruct->nSize = nSize;
		
		pStruct->LocalTime = LocalTime;
		if(nSize > 0) {
			pStruct->pParam =  new char[nSize];
			memcpy( pStruct->pParam, pParam, nSize );
		}
		else {
			pStruct->pParam = NULL;
		}		
		m_pVecLoadList[nLayerIndex].push_back( pStruct );
	}
	else {
		Func( pThis, pParam, nSize, LocalTime );
	}
}

bool CDnLoadingTask::InsertLoadPacket( void *pThis, int nMainCmd, int nSubCmd, void *pParam, int nSize )
{
	if( !m_bEnableBackgroundLoad ) return false;

	ScopeLock<CSyncLock> Lock(m_Lock);
	for( int l=0; l<LoadThreadLayer_Amount; l++ ) {
		for( DWORD i=0; i<m_pVecLoadList[l].size(); i++ ) {
			LoadStruct *pStruct = m_pVecLoadList[l][i];
			if( !pStruct->pCheckFunc ) continue;
			if( pStruct->pCheckFunc( pThis, pStruct->pGetFunc( pStruct->pParam, pStruct->nSize ), nMainCmd, nSubCmd, pParam, nSize ) == true ) {
				LoadPacketStruct *pLoadPacket = new LoadPacketStruct;
				pLoadPacket->nMainCmd = nMainCmd;
				pLoadPacket->nSubCmd = nSubCmd;			
				pLoadPacket->nSize = nSize;
				if( nSize > 0 ) {
					pLoadPacket->pData = new char[nSize];
					memcpy( pLoadPacket->pData, pParam, nSize );
				}
				else {
					pLoadPacket->pData = NULL;
				}
				pStruct->pVecPacketList.push_back( pLoadPacket );
				return true;
			}
		}
	}

	return false;
}

bool CDnLoadingTask::CheckSameLoadObject( void *pParam, int nSize, int nLayerIndex )
{
	if( pParam == NULL || nSize == 0 ) return false;

	ScopeLock<CSyncLock> Lock(m_Lock);
	LoadStruct *pLoad;
	for( DWORD i=1; i<m_pVecLoadList[nLayerIndex].size(); i++ ) {	// 0번은 지금 처리중인 녀석일거다.
		pLoad = m_pVecLoadList[nLayerIndex][i];
		if( pLoad->nSize != nSize ) continue;
		if( nSize == sizeof(DWORD) ) {	// 우선 DWORD만 처리해둔다. 그 외 경우에 SameLoadObject를 호출할 이유가 아직은 없다.
			DWORD dwValue1 = *(DWORD*)pLoad->pParam;
			DWORD dwValue2 = *(DWORD*)pParam;

			if( dwValue1 == dwValue2 ) return true;
		}
	}
	return false;
}

void CDnLoadingTask::PauseRenderScreen( bool bPause )
{
	ScopeLock<CSyncLock> Lock(m_RenderLock);
	m_bPauseRenderScreen = bPause;
}
