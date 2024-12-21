#include "StdAfx.h"
#include "DnTradePrivateMarket.h"
#include "DnInterface.h"
#include "DnPrivateMarketDlg.h"
#include "DnItemTask.h"
#include "TradeSendPacket.h"
#include "DnBridgeTask.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnChatTabDlg.h"
#include "DnChatOption.h"
#include "DnInventory.h"
#include "DnCharInventory.h"
#include "DNIsolate.h"
#include "DnRestraintTask.h"
#include "DnChatRoomDlg.h"
#include "DnAcceptRequestDlg.h"
#ifdef PRE_ADD_SECONDARY_SKILL
#include "DnLifeSkillCookingTask.h"
#endif // PRE_ADD_SECONDARY_SKILL



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTradePrivateMarket::CDnTradePrivateMarket(void)
	: m_dwCurTradeUserID(0)
	, m_dwReadyTradeUserID(0)
	, m_pPrivateMarketDialog(NULL)
	, m_nChatMode(CHAT_NORMAL)
{
	m_vecTradePlayerItem.resize(10);
}

CDnTradePrivateMarket::~CDnTradePrivateMarket(void)
{
	ClearTradePlayerItem();
}

void CDnTradePrivateMarket::AddTradePlayerItem( int nIndex, TItemInfo &Item )
{
	if( nIndex < 0 ) return;
	if( nIndex >= (int)m_vecTradePlayerItem.size() ) return;
	CDnItem *pItem = GetItemTask().CreateItem( Item );
	if( pItem )
	{
		m_vecTradePlayerItem[nIndex] = pItem;
		CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
	}
}

void CDnTradePrivateMarket::RemoveTradePlayerItem( int nIndex )
{
	if( nIndex < 0 ) return;
	if( nIndex >= (int)m_vecTradePlayerItem.size() ) return;
	SAFE_DELETE( m_vecTradePlayerItem[nIndex] );
}

void CDnTradePrivateMarket::ClearTradePlayerItem()
{
	SAFE_DELETE_PVEC( m_vecTradePlayerItem );
}

void CDnTradePrivateMarket::RequestPrivateMarket( UINT nSessionID )
{
	if( nSessionID <= 0 )
	{
		CDebugSet::ToLogFile( "CDnTradePrivateMarket::RequestPrivateMarket, Invalid SessionID(%d)!", nSessionID );
		return;
	}

	// 거래 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true ) )
		return;
	
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
	if( !hActor )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3601 ), MB_OK );	// UISTRING : 유저가 존재하지 않습니다.
		return;
	}

	if (GetIsolateTask().IsBlackList(hActor->GetName()))
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3623 ), MB_OK);	// UISTRING : 차단한 캐릭터에게는 거래를 요청할 수 없습니다.
		return;
	}

	AddTradeUserInfo( true, nSessionID );

	//wchar_t wszTemp[256] = {0};
	//swprintf_s( wszTemp, _countof(wszTemp), L"%s %s", hActor->GetName(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3602 ) );
	//if( !GetInterface().OpenPrivateMarketReqDialog( wszTemp, 10.0f, PRIVATE_MARKET_REQ_DIALOG, this ) )
	//	return;

	SendExchangeRequest( nSessionID, false );
}

void CDnTradePrivateMarket::RequestPrivateMarketAccept( bool bAccept, UINT nSessionID )
{
#ifdef PRE_FIX_49403
	// #49403 특수 순서에서, 돈을 소모했음에도 불구하고, 비행선을 탈 수 없다. 
	// 블라인드창이 열려있는 경우 거래수락을 막는다.
	if( GetInterface().IsOpenBlind() )
		bAccept = false;
#endif	

	// Note : 거래수락
	//
	SendExchangeAccept( bAccept, nSessionID );
}

void CDnTradePrivateMarket::RequestPrivateMarketAddItem( int nSlotIndex, int nInvenIndex, int nCount, INT64 biSerial )
{
	// Note : 아이템 추가
	//
	ASSERT( nCount>0 );
	SendExchangeAddItem( nSlotIndex, nInvenIndex, nCount, biSerial );
}

void CDnTradePrivateMarket::RequestPrivateMarketDeleteItem( int nIndex )
{
	// Note : 아이템 삭제
	//
	ASSERT( nIndex>=0 );
	SendExchangeDeleteItem( nIndex );
}

void CDnTradePrivateMarket::RequestPrivateMarketCoin( INT64 nCoin )
{
	// Note : 동전입력
	//
	ASSERT( nCoin>=0 );
	SendExchangeAddCoin( nCoin );
}

void CDnTradePrivateMarket::RequestPrivateMarketConfirm( char cType )
{
	SendExchangeConfirm( cType );
}

void CDnTradePrivateMarket::RequestPrivateMarketCancel()
{
	SendExchangeCancel();
}

void CDnTradePrivateMarket::OnRecvPrivateMarket( SCExchangeRequest *pPacket )
{
	// Note : 거래 신청이 들어왔다.
	//
	if( pPacket->nRet == ERROR_NONE )
	{
		bool bAccept(true);

		if( m_dwCurTradeUserID > 0 )
		{
			// Note : 거래 중이다. 거래중이라는 메세지를 보내야 한다.
			//
			bAccept = false;
		}
		else if( m_dwReadyTradeUserID > 0 )
		{
			// Note : 거래 요청 중이거나, 거래 수락-거절 확인창이 떠있는 중이라면,
			//
			bAccept = false;
		}
		else if( GetInterface().GetChatRoomDlg()->IsShow() )
		{
			// 채팅룸이 보이는 상태라면,
			// 현재 채팅룸에 속한 사람인지 판단해서 확인한다.
			if( !GetInterface().GetChatRoomDlg()->IsMyChatRoomMember( pPacket->nSenderSessionID ) )
				bAccept = false;
		}
		else if( GetInterface().IsShowMapMoveCashItemDlg() )
		{
			// 마을 이동 아이템 다이얼로그가 떠 있는 상태라면 거래 안됨.
			bAccept = false;
		}
		else if( GetInterface().IsShowGateReady() )
		{
			bAccept = false;
		}
		else if( CDnLocalPlayerActor::IsLockInput() )
		{
			bAccept = false;
		}
		else if( GetInterface().IsOpenBlind() )
		{
			bAccept = false;
		}
#ifdef PRE_ADD_CHANGEJOB_CASHITEM
		else if (GetInterface().IsShowChangeJobDialog())
		{
			bAccept = false;
		}
#endif
		else if( GetItemTask().IsRequestWait() )
		{
			bAccept = false;
		}
#ifdef PRE_ADD_SECONDARY_SKILL
		else if( CDnLifeSkillCookingTask::IsActive() && ( GetLifeSkillCookingTask().IsNowCooking() || GetLifeSkillCookingTask().IsRequestCooking() ) )
		{
			bAccept = false;
		}
#endif // PRE_ADD_SECONDARY_SKILL
		if( bAccept == false )
		{
			RequestPrivateMarketAccept( false, pPacket->nSenderSessionID );
			return;
		}
#ifdef PRE_ADD_SECONDARY_SKILL
		GetInterface().CloseCookingDialog();
#endif // PRE_ADD_SECONDARY_SKILL
		AddTradeUserInfo( false, pPacket->nSenderSessionID );
	}
	else if( pPacket->nRet == ERROR_EXCHANGE_SENDERCANCEL )
	{
		// #19668 이슈에서처럼 A->B 신청한 상태에서 B수락 직후 A가 응답받기전 취소를 눌렀을 경우
		// 거래 성립 후 B에게 ERROR_EXCHANGE_SENDERCANCEL이 날아온다.
		// 만약 이런 상태라면, 거래를 취소시킨다.
		if( m_dwCurTradeUserID > 0 )
		{
			RequestPrivateMarketCancel();
			return;
		}

		// Note : 거래요청을 취소했다.
		//
		std::wstring wszTradeUserName;
		GetTradeUserName( pPacket->nSenderSessionID, wszTradeUserName );
		bool bRet = RemoveTradeUserInfo( false, pPacket->nSenderSessionID );
		m_dwReadyTradeUserID = 0;

		// 혹시 상대의 요청취소 패킷을 받기전에 수락을 눌러 거래창이 열렸다면,
		// 창을 닫고 아래 변수를 초기화해야한다.
		m_dwCurTradeUserID = 0;

		// 정상적으로 리스트에서 삭제되었을때만 메세지를 화면에 보여준다.
		if( bRet )
		{
			wchar_t wszTemp[256] = {0};
			swprintf_s( wszTemp, _countof(wszTemp), L"%s %s", wszTradeUserName.c_str(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3604 ) );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
		}
	}
	else
	{
		GetInterface().ServerMessageBox(pPacket->nRet);

		// 유령에게 /거래로 걸었을 경우 이쪽으로 들어오게 된다.
		RemoveTradeUserInfo( true, pPacket->nSenderSessionID );
	}
}

void CDnTradePrivateMarket::OnRecvPrivateMarketStart( SCExchangeStart *pPacket )
{
	std::wstring wszTradeUserName;
	GetTradeUserName( pPacket->nTargetSessionID, wszTradeUserName );
	RemoveTradeUserInfo( true, pPacket->nTargetSessionID );

	if( wszTradeUserName.empty() )
		wszTradeUserName = m_wszTradeUserName;

	// Note : 거래가 시작되었다. 창을 띄우자.
	//
	// 채팅룸 보이지고 있는 상황이라면, 할 필요 없을거다.
	bool bStopAll = true;
	if( GetInterface().GetChatRoomDlg() && GetInterface().GetChatRoomDlg()->IsShow() )
		bStopAll = false;
	if( bStopAll )
		CDnLocalPlayerActor::StopAllPartyPlayer();

	if( pPacket->nRet == ERROR_NONE )
	{
		m_dwCurTradeUserID = pPacket->nTargetSessionID;

		// 거래가 성립되었으니 리스트 초기화.
		ClearTradeUserInfoList();

		// 교환창 띄우고,
		CDnMainMenuDlg *pMenuDlg = (CDnMainMenuDlg*)GetInterface().GetMainMenuDialog();
		if( pMenuDlg ) pMenuDlg->ShowPrivateMarketDialog( true );

		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg )
			pInvenDlg->SetPrivateMarketName( CDnActor::s_hLocalActor->GetName(), wszTradeUserName.c_str() );

		// 거래가 시작되면 현재 거래모드를 기억해두었다가(귓속말까지는 기억하지 않는다.)
		m_nChatMode = GetInterface().GetChatDialog()->GetChatMode();
		
		// 기본 채팅을 상대방 귓속말로 전환한다.
		GetInterface().GetChatDialog()->SetPrivateName( wszTradeUserName );
	}
	else
	{
#ifdef PRE_FIX_CANCELTRADE
		ClearTradeUserInfoList(true);
#endif
		GetInterface().ServerMessageBox(pPacket->nRet);
		m_dwCurTradeUserID = 0;
	}

	//GetInterface().ClosePrivateMarketReqDialog();
	m_dwReadyTradeUserID = 0;
}

void CDnTradePrivateMarket::OnRecvPrivateMarketAddItem( SCExchangeAddItem *pPacket )
{
	// Note : 상대가 아이템 추가했다.
	//
	if( pPacket->nRet == ERROR_NONE )
	{
		if( pPacket->nSessionID == m_dwCurTradeUserID )
		{
			RemoveTradePlayerItem( pPacket->cExchangeIndex );
			AddTradePlayerItem( pPacket->cExchangeIndex, pPacket->ItemInfo );
			m_pPrivateMarketDialog->OnAddItem_Other( pPacket->cExchangeIndex, m_vecTradePlayerItem[pPacket->cExchangeIndex] );
		}
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRet);
}

void CDnTradePrivateMarket::OnRecvPrivateMarketDeleteItem( SCExchangeDeleteItem *pPacket )
{
	// Note : 상대가 아이템을 삭제했다.
	//
	if( pPacket->nRet == ERROR_NONE )
	{
		if( pPacket->nSessionID == m_dwCurTradeUserID )
		{
			RemoveTradePlayerItem( pPacket->cExchangeIndex );
			m_pPrivateMarketDialog->OnDelItem_Other( pPacket->cExchangeIndex );
		}
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRet);
}

void CDnTradePrivateMarket::OnRecvPrivateMarketCoin( SCExchangeAddCoin *pPacket )
{
	// Note : 상대가 코인을 변경했다.
	//
	if( pPacket->nRet == ERROR_NONE )
	{
		if( pPacket->nSessionID == m_dwCurTradeUserID )
		{
			m_pPrivateMarketDialog->OnUpdateMoney_Other( pPacket->nCoin );
		}
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRet);
}

void CDnTradePrivateMarket::OnRecvPrivateMarketConfirm( SCExchangeConfirm *pPacket )
{
	// Note : 상대방의 거래 확인/취소
	//
	if( pPacket->nSessionID == m_dwCurTradeUserID )
	{
		switch( pPacket->cType )
		{
		case EXCHANGE_REGIST:
			m_pPrivateMarketDialog->OnClickButtonRegist_Other();
			break;
		case EXCHANGE_CONFIRM:
			m_pPrivateMarketDialog->OnClickButtonConfirm_Other();
			break;
		default:
			CDebugSet::ToLogFile( "CDnTradePrivateMarket::OnRecvPrivateMarketConfirm, default case!" );
			break;
		}
	}
}

void CDnTradePrivateMarket::OnRecvPrivateMarketComplete( SCExchangeComplete *pPacket )
{
	if( pPacket->nRet != ERROR_NONE )
	{
		GetInterface().ServerMessageBox(pPacket->nRet);
	}
	else
	{
		wchar_t wszTemp[256] = {0};
		swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4111 ) );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
	}

	// Note : 거래가 완전히 종료되었으니 창을 닫자.
	//
	CDnMainMenuDlg *pMenuDlg = (CDnMainMenuDlg*)GetInterface().GetMainMenuDialog();
	if( pMenuDlg )
	{
		pMenuDlg->ShowPrivateMarketDialog( false, pPacket->nRet != ERROR_NONE );
	}

	// 거래 성공 후 교환창에 올려졌던 아이템들에게 new표시를 해두자.
	// PrivateMarketDlg에 있는 아이템은 단지 교환창에만 있는 아이템일 뿐이다.
	//CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
	//if( pInvenDlg ) pInvenDlg->SetPrivateMarketNewGain();
	//SetNewGainTradeList();

	// 거래가 끝나면 이전 채팅모드로 변환시키되, 만약 거래 도중 변경했다면 그냥 내버려둔다.
	DnActorHandle hTarget = CDnActor::FindActorFromUniqueID( m_dwCurTradeUserID );
	if( hTarget )
	{
		if( GetInterface().GetChatDialog()->GetChatMode() == CHAT_PRIVATE &&
			GetInterface().GetChatDialog()->GetPrivateName() == hTarget->GetName() )
		{
			GetInterface().GetChatDialog()->SetChatMode( m_nChatMode );
		}
	}

	m_dwCurTradeUserID = 0;
	m_dwReadyTradeUserID = 0;
}

void CDnTradePrivateMarket::OnRecvPrivateMarketCancel()
{
	// Note : 거래가 취소되었다.
	//

	// 한가지 검사를 해야하는게 있다.
	// 거래중이던 유저 둘이 동시에 Cancel을 누를 경우 어쨌든 먼저 처리되는 취소요청은 제대로 처리 될테지만,
	// 다음으로 처리되는 요청에 대해선 아래 액터 얻는 부분에서 유저를 찾을 수 없다는 메세지박스가 뜨게된다.
	// 이것을 막기 위해 현재 거래중인 유저가 있는지부터 검사하겠다.
	if( !m_dwCurTradeUserID )
		return;
	
	// 창을 닫고 메세지 박스 출력.
	//
	CDnMainMenuDlg *pMenuDlg = (CDnMainMenuDlg*)GetInterface().GetMainMenuDialog();
	if( pMenuDlg )
	{
		pMenuDlg->ShowPrivateMarketDialog( false );
		pMenuDlg->CloseInvenDialog();
	}

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( m_dwCurTradeUserID );
	if( !hActor )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3601 ), MB_OK );
	}
	else {
		wchar_t wszTemp[256] = {0};
		swprintf_s( wszTemp, _countof(wszTemp), L"%s %s", hActor->GetName(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3605 ) );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );

		// 거래가 끝나면 이전 채팅모드로 변환시키되, 만약 거래 도중 변경했다면 그냥 내버려둔다.
		DnActorHandle hTarget = CDnActor::FindActorFromUniqueID( m_dwCurTradeUserID );
		if( hTarget )
		{
			if( GetInterface().GetChatDialog()->GetChatMode() == CHAT_PRIVATE &&
				GetInterface().GetChatDialog()->GetPrivateName() == hTarget->GetName() )
			{
				GetInterface().GetChatDialog()->SetChatMode( m_nChatMode );
			}
		}
	}

	m_dwCurTradeUserID = 0;
	m_dwReadyTradeUserID = 0;
}

void CDnTradePrivateMarket::OnRecvPrivateMarketReject( SCExchangeReject *pPacket )
{
	GetInterface().ClosePrivateMarketReqDialog();

	if( pPacket->nRetCode != ERROR_NONE )
	{
		// 리턴코드가 제재때문이라면 리턴코드에 에러코드가 들어있을 것이다.

		// 요청창 없애야하니 리무브 후
		std::wstring wszTradeUserName;
		GetTradeUserName( pPacket->nSessionID, wszTradeUserName );
		RemoveTradeUserInfo( true, pPacket->nSessionID );

		if( ERROR_GENERIC_LEVELLIMIT == pPacket->nRetCode )
		{
			WCHAR wszMessage[512];
			int iWeight = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::LimitLevel_Exchange);
			swprintf_s( wszMessage, _countof(wszMessage), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4168 ), iWeight );	// 거래 요청 기능은 %d 레벨 이후 사용 가능합니다.

			GetInterface().MessageBox( wszMessage );

			return;
		}
		// 서버 에러코드 메세지박스
		GetInterface().ServerMessageBox(pPacket->nRetCode);
	}
	else
	{
		// 그 외 경우라면 일반적인 거절이다.

		// 자신이 시도한 거래 요청은 시간이 끝났을 경우 Process부분에서 직접 삭제하게 되어있다.
		// 그래서 RemoveTradeUserInfo 리턴값 검사 후 메세지를 출력하는 루틴이 없다.
		// 그런데, 실제 거래거절인지, 로그아웃하면서 거절인지 서버는 특별히 알 방법이 없어서 두번 보내게 되어있다.
		// 검사 후 메세지 띄우는 걸로 하겠다.
		std::wstring wszTradeUserName;
		GetTradeUserName( pPacket->nSessionID, wszTradeUserName );
		bool bRet = RemoveTradeUserInfo( true, pPacket->nSessionID );

		if( bRet )
		{
			wchar_t wszTemp[256] = {0};
			swprintf_s( wszTemp, _countof(wszTemp), L"%s %s", wszTradeUserName.c_str(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3606 ) );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
		}
	}
}

void CDnTradePrivateMarket::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	switch( nID ) 
	{
	case ACCEPT_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
				{
					ClearTradeUserInfoList();
					RequestPrivateMarketAccept( true, m_dwReadyTradeUserID );

					m_dwCurTradeUserID = m_dwReadyTradeUserID;
				}
				else if( (strcmp( pControl->GetControlName(), "ID_CANCEL" ) == 0) )
				{
					RequestPrivateMarketAccept( false, m_dwReadyTradeUserID );
				}

				m_dwReadyTradeUserID = 0;
				GetInterface().CloseAcceptDialog();
			}
		}
		break;
	case PRIVATE_MARKET_REQ_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if( (strcmp( pControl->GetControlName(), "ID_CANCEL" ) == 0) )
				{
					// Note : 여기서 서버에 요청 취소를 보내야 한다.
					//
					SendExchangeRequest( m_dwReadyTradeUserID, true );
					m_dwReadyTradeUserID = 0;
				}

				GetInterface().ClosePrivateMarketReqDialog();
			}
		}
		break;
	case ACCEPT_REQUEST_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
				{
					// 우선 이 요청에 대한 것부터 리스트에서 뺀 후
					GetTradeUserName( m_dwReadyTradeUserID, m_wszTradeUserName );	// 임시 기억
					RemoveTradeUserInfo( false, m_dwReadyTradeUserID );

					// 해당 유저가 주위에 존재하는지 확인한다.
					DnActorHandle hTarget = CDnActor::FindActorFromUniqueID( m_dwReadyTradeUserID );
					if( hTarget )
					{
						// 다른 모든 요청에 대해 취소처리를 후 수락패킷 전송
						ClearTradeUserInfoList();
						RequestPrivateMarketAccept( true, m_dwReadyTradeUserID );
						m_dwCurTradeUserID = m_dwReadyTradeUserID;
					}
					else
					{
						// 거절패킷을 보내야 서버에서 두 클라의 상태를 같이 풀 수 있다.
						RequestPrivateMarketAccept( false, m_dwReadyTradeUserID );
						m_dwReadyTradeUserID = 0;
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3601 ), MB_OK );
					}
				}
				else if( (strcmp( pControl->GetControlName(), "ID_REJECT" ) == 0) )
				{
					// 타겟이 주위에 있는지 여부와 상관없이 거절패킷 보내면 된다.
					RemoveTradeUserInfo( false, m_dwReadyTradeUserID );
					RequestPrivateMarketAccept( false, m_dwReadyTradeUserID );
					m_dwReadyTradeUserID = 0;
				}
				else if( (strcmp( pControl->GetControlName(), "ID_CANCEL" ) == 0) )
				{
					// 타겟이 주위에 있는지 여부와 상관없이 서버에 요청 취소를 보내면 된다.
					RemoveTradeUserInfo( true, m_dwReadyTradeUserID );
					SendExchangeRequest( m_dwReadyTradeUserID, true );
					m_dwReadyTradeUserID = 0;
				}

				GetInterface().CloseAcceptRequestDialog();
			}
		}
		break;
}
}

#ifdef PRE_FIX_CANCELTRADE
void CDnTradePrivateMarket::ClearTradeUserInfoList(bool bClearOnly)
#else
void CDnTradePrivateMarket::ClearTradeUserInfoList()
#endif
{
#ifdef PRE_FIX_CANCELTRADE
	if (bClearOnly)
{
		m_listTradeUserInfo.clear();
		return;
	}
#endif
	LIST_TRADEUSERINFO_ITER iter = m_listTradeUserInfo.begin();
	for( ; iter != m_listTradeUserInfo.end(); ++iter )
	{
		//RequestPrivateMarketAccept( false, iter->m_dwSessionID );

		if( iter->m_bSelfToTarget == true )
		{
			// 강제로 제거될땐 보내는게 원칙이다.
			SendExchangeRequest( iter->m_dwSessionID, true );
		}
		else
		{
			RequestPrivateMarketAccept( false, iter->m_dwSessionID );
		}
	}

	m_listTradeUserInfo.clear();
}

void CDnTradePrivateMarket::AddTradeUserInfo( bool bSelfToTarget, DWORD nSessionID )
{
	STradeUserInfo sTradeUserInfo;
	sTradeUserInfo.m_bSelfToTarget = bSelfToTarget;
	sTradeUserInfo.m_dwSessionID = nSessionID;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
	if( !hActor ) return;
	sTradeUserInfo.m_strName = hActor->GetName();
	sTradeUserInfo.m_fAcceptTime = 10.0f;

	m_listTradeUserInfo.push_back( sTradeUserInfo );
}

bool CDnTradePrivateMarket::RemoveTradeUserInfo( bool bSelfToTarget, DWORD nSessionID )
{
	LIST_TRADEUSERINFO_ITER iter = m_listTradeUserInfo.begin();
	for( ; iter != m_listTradeUserInfo.end(); ++iter )
	{
		if( iter->m_bSelfToTarget == bSelfToTarget && iter->m_dwSessionID == nSessionID )
		{
			//m_listTradeUserInfo.pop_front();
			m_listTradeUserInfo.erase( iter );
			return true;
		}
	}

	CDebugSet::ToLogFile( "CDnTradePrivateMarket::RemoveTradeUserInfo, Invalid Session ID(%d)", nSessionID );
	return false;
}

bool CDnTradePrivateMarket::GetTradeUserName( DWORD nSessionID, std::wstring &wszName )
{
	LIST_TRADEUSERINFO_ITER iter = m_listTradeUserInfo.begin();
	for( ; iter != m_listTradeUserInfo.end(); ++iter )
	{
		if( iter->m_dwSessionID == nSessionID )
		{
			wszName = iter->m_strName;
			return true;
		}
	}
	return false;
}

bool CDnTradePrivateMarket::IsRequestUser( DWORD nSessionID )
{
	LIST_TRADEUSERINFO_ITER iter = m_listTradeUserInfo.begin();
	for( ; iter != m_listTradeUserInfo.end(); ++iter )
	{
		// 내가 걸었던지, 나한테 요청이 들어왔던지는 상관하지 않는다.
		if( iter->m_dwSessionID == nSessionID )
		{
			return true;
		}
	}

	return false;
}

bool CDnTradePrivateMarket::IsRequesting()
{
	LIST_TRADEUSERINFO_ITER iter = m_listTradeUserInfo.begin();
	for( ; iter != m_listTradeUserInfo.end(); ++iter )
	{
		// 누구에게 걸었느냐는 상관하지 않는다.
		if( iter->m_bSelfToTarget == true )
		{
			return true;
		}
	}

	return false;
}

bool CDnTradePrivateMarket::IsAccepting()
{
	LIST_TRADEUSERINFO_ITER iter = m_listTradeUserInfo.begin();
	for( ; iter != m_listTradeUserInfo.end(); ++iter )
	{
		// 누가 나에게 걸었느냐는 상관하지 않는다.
		if( iter->m_bSelfToTarget == false )
		{
			return true;
		}
	}

	return false;
}

void CDnTradePrivateMarket::Process( float fElapsedTime )
{
	// 이벤트 및 상점 사용시 모든 처리에 대해 거절 보낸다.
	if( GetInterface().IsOpenBlind() )
		ClearTradeUserInfoList();

	// 채널이동도 포함해야할텐데.

	// 각각의 요청에 대한 시간 처리등은 모두 여기서 하고, 다이얼로그에선 처리된 내용(시간값 등)을 보여주기만 한다.
	LIST_TRADEUSERINFO_ITER iter = m_listTradeUserInfo.begin();
	for( ; iter != m_listTradeUserInfo.end(); )
	{
		iter->m_fAcceptTime -= fElapsedTime;

		if( iter->m_fAcceptTime <= 0.0f )
		{
			if( iter->m_bSelfToTarget == true )
			{
				// 시간이 지나 내 요청타임이 끝났을땐 요청취소를 보내지 않는다.
				//SendExchangeRequest( iter->m_dwSessionID, true );

				// 이건 이미 여러번 요청을 보낼때부터 다른 값이 들어있다.
				//m_dwRequestTradeUserID = 0;

				// 알아서 닫히게 되어있다. 시간 끝나면.
				//GetInterface().ClosePrivateMarketReqDialog();

				// 거절로 올거기 때문에, 직접 지우지도 않는다.
				//iter = m_listTradeUserInfo.erase( iter );
				//continue;
			}
			else
			{
				RequestPrivateMarketAccept( false, iter->m_dwSessionID );
				iter = m_listTradeUserInfo.erase( iter );
				continue;
			}
		}

		++iter;
	}

	// 확인용
	if( m_listTradeUserInfo.empty() )
		m_dwReadyTradeUserID = 0;
}

float CDnTradePrivateMarket::GetShortestAcceptTime()
{
	if( !m_listTradeUserInfo.empty() )
		return m_listTradeUserInfo.begin()->m_fAcceptTime;
	else
		return _INVALID_TIME_VALUE;
}

void CDnTradePrivateMarket::OpenAcceptRequestDialog()
{
	LIST_TRADEUSERINFO_ITER iterInner = m_listTradeUserInfo.begin();

	wchar_t wszTemp[256] = {0};
	if( iterInner->m_bSelfToTarget == true )
	{
		swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3602), iterInner->m_strName.c_str());	// UISTRING : <html><br><div align=\"center\">[%s] 님께 거래를 요청중입니다.</div></html>
		GetInterface().OpenAcceptRequestDialog( wszTemp, _REQUEST_ACCEPT_TOTALTIME, iterInner->m_fAcceptTime, false, CDnAcceptRequestDlg::eType01, ACCEPT_REQUEST_DIALOG, this );
		m_dwReadyTradeUserID = iterInner->m_dwSessionID;
	}
	else
	{
		swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3607), iterInner->m_strName.c_str());	// UISTRING : <html><br><div align=\"center\">[%s] 님이 거래를 요청하셨습니다.</div></html>
		GetInterface().OpenAcceptRequestDialog( wszTemp, _REQUEST_ACCEPT_TOTALTIME, iterInner->m_fAcceptTime, true, CDnAcceptRequestDlg::eType01, ACCEPT_REQUEST_DIALOG, this );
		m_dwReadyTradeUserID = iterInner->m_dwSessionID;
	}
}

//void CDnTradePrivateMarket::SetNewGainTradeList()
//{
//	DWORD dwVecSize = (DWORD)m_vecTradePlayerItem.size();
//	for( DWORD i=0; i<dwVecSize; i++ )
//	{
//		if( m_vecTradePlayerItem[i] )
//		{
//			// 인벤토리 안에 있는 아이템 중에 같은 것에다가 New표시를 해둔다.
//			CDnItem *pItem = GetItemTask().GetCharInventory().GetItemFromSerialID( m_vecTradePlayerItem[i]->GetSerialID() );
//			if( pItem )
//				pItem->SetNewGain( true );
//		}
//	}
//}


#ifdef PRE_FIX_49403
void CDnTradePrivateMarket::SendExchangeRequestCancel( DWORD nSessionID )
{
	SendExchangeRequest( nSessionID, true );
}
#endif
