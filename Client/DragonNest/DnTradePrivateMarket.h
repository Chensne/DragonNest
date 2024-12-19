#pragma once
#include "DNPacket.h"

class CDnItem;
class CDnPrivateMarketDlg;

class CDnTradePrivateMarket	: public CEtUICallback
{
	struct STradeUserInfo
	{
		bool m_bSelfToTarget;	// 자신이 거래를 건거면 true, 거래요청을 받은거면 false다.
		DWORD m_dwSessionID;
		std::wstring m_strName;
		float m_fAcceptTime;

		STradeUserInfo()
			: m_dwSessionID(0)
			, m_fAcceptTime(10.0f)
		{
			m_bSelfToTarget = false;
		}
	};

public:
	CDnTradePrivateMarket(void);
	virtual ~CDnTradePrivateMarket(void);

protected:
	DWORD m_dwCurTradeUserID;		// 거래중인 상대유저의 ID
	DWORD m_dwReadyTradeUserID;		// 현재 띄워진 창의 대상 ID(수락이던, 요청이던)
	std::wstring m_wszTradeUserName;

	// 상대방의 거래아이템 리스트
	std::vector<CDnItem*> m_vecTradePlayerItem;

	CDnPrivateMarketDlg *m_pPrivateMarketDialog;

	typedef std::list<STradeUserInfo>		LIST_TRADEUSERINFO;
	typedef LIST_TRADEUSERINFO::iterator	LIST_TRADEUSERINFO_ITER;

	LIST_TRADEUSERINFO m_listTradeUserInfo;

	int m_nChatMode;				// 교환창 뜰때의 채팅모드. 기억해두었다가 되돌리는데 사용.

public:
	void Process( float fElapsedTime );

	float GetShortestAcceptTime();
	void OpenAcceptRequestDialog();

protected:
	void AddTradePlayerItem( int nIndex, TItemInfo &Item );
	void RemoveTradePlayerItem( int nIndex );
	void ClearTradePlayerItem();

	void AddTradeUserInfo( bool bSelfToTarget, DWORD nSessionID );
	bool RemoveTradeUserInfo( bool bSelfToTarget, DWORD nSessionID );

	bool GetTradeUserName( DWORD nSessionID, std::wstring &wszName );

public:
	// 요청취소는 외부에서도 호출할 수 있다.
#ifdef PRE_FIX_CANCELTRADE
	void ClearTradeUserInfoList(bool bClearOnly = false);
#else
	void ClearTradeUserInfoList();
#endif

	// 현재 요청, 수락창이 떠 있는 상대방 ID
	void SetTradePlayerSessionID( DWORD dwSessionID )	{ m_dwReadyTradeUserID = dwSessionID; }
	DWORD GetTradePlayerSessionID()						{ return m_dwReadyTradeUserID; }

#ifdef PRE_FIX_49403
	// 거래중인 상대유저의 ID.
	DWORD GetCurTradeUserID(){
		return m_dwCurTradeUserID;
	}

	void SendExchangeRequestCancel( DWORD nSessionID ); // 거래요청취소.

#endif

	void SetPrivateMarketDialog( CDnPrivateMarketDlg *pDialog ) { m_pPrivateMarketDialog = pDialog; }
	CDnPrivateMarketDlg *GetPrivateMarketDlg() { return m_pPrivateMarketDialog; }

	// 이미 요청한 유저인가. - 전에는 거래를 요청한/요청받은 유저에 한해서만 거래를 다시 걸 수 없었는데,
	// 기획이 바뀌어서 아래 두 함수만 사용하게 되었다.
	bool IsRequestUser( DWORD nSessionID );

	// 내가 누군가에게 요청중인가,
	bool IsRequesting();

	// 누군가 나에게 요청중인가.
	bool IsAccepting();


	// 교환 내역에 있는 아이템들에다 new표시해두기.
	// RefreshInven으로 들어오는 아이템에 대해 일괄처리하므로, 현재 사용하지 않는다.
	//void SetNewGainTradeList();

public:
	void RequestPrivateMarket( UINT nSessionID );
	void RequestPrivateMarketAccept( bool bAccept, UINT nSessionID );
	void RequestPrivateMarketAddItem( int nSlotIndex, int nInvenIndex, int nCount, INT64 biSerial );
	void RequestPrivateMarketDeleteItem( int nIndex );
	void RequestPrivateMarketCoin( INT64 nCoin );
	void RequestPrivateMarketConfirm( char cType );
	void RequestPrivateMarketCancel();
	void RequestPrivateMarketComplete();

public:
	void OnRecvPrivateMarket( SCExchangeRequest *pPacket );
	void OnRecvPrivateMarketStart( SCExchangeStart *pPacket );
	void OnRecvPrivateMarketAddItem( SCExchangeAddItem *pPacket );
	void OnRecvPrivateMarketDeleteItem( SCExchangeDeleteItem *pPacket );
	void OnRecvPrivateMarketCoin( SCExchangeAddCoin *pPacket );
	void OnRecvPrivateMarketConfirm( SCExchangeConfirm *pPacket );
	void OnRecvPrivateMarketComplete( SCExchangeComplete *pPacket );
	void OnRecvPrivateMarketCancel();
	void OnRecvPrivateMarketReject( SCExchangeReject *pPacket );

public:
	// CEtUICallback
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

};