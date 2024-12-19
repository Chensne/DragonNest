#include <stdio.h>
#include <windows.h>

#define MAX_ITEMNAME_LEN			50
#define MAX_ITEM_BUY_COUNT			5

#pragma pack(push, 1)
typedef struct _tagItemInfoStructure
{
	int itemID;	
	WCHAR itemName[MAX_ITEMNAME_LEN+1];
	int itemPrice;
	int itemCount;		
} TITEM_INFO_UNICODE, *PTITEM_INFO_UNICODE;
#pragma pack(pop)

class CFuncID{ //�ܺ����α׷� ���Լ� ����ü.. �ܺ� ���α׷��� �Լ� �����ų�� �ִ�..
	public : 
		void (__stdcall *OnResError)(const __int64 _i64SeqNum, const int _iErrorCode);
		void (__stdcall *OnServerAuth)(const bool _bIsOK, const int _iErrorCode);
		void (__stdcall *OnUserBalance)(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiReMainCash, 
			const unsigned int _uiReMainMile, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);
		void (__stdcall *OnUseMoney)(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiReMainCash, 
			const unsigned int _uiReMainMile, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);		
		void (__stdcall *OnUseCoupon)(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const char _cItemType, 
			LPCWSTR _szItemDesc, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);		
		void (__stdcall *OnSetMile)(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiReMainMile, 
			const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);
		void (__stdcall *OnUseTrade)(const __int64 _i64SeqNum, const int _iBillType, const int _iUseCash, const int _iUseMile, 
			LPCWSTR _szPlayerID, LPCWSTR _szBillingKey, LPCWSTR _szRecvPlayerID, const int _iRecvCash, 
			const int _iRecvMile, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);
};
CFuncID funcID;//DLL���� Call�� class

//DLL�Լ�
typedef int (__stdcall *fnInit_SDK)(char *_szResult);
typedef int (__stdcall *fnSDKConnect)(CFuncID funcGetID, bool bDebugMode);
typedef int (__stdcall *fnOSP_CONNECTION_CHECK)(void);
typedef int  (__stdcall *fnUserBalance)(__int64 _i64SeqNum, LPCWSTR _szPlayerID, LPCWSTR _szIPAddr, int _iCSite);

typedef int (__stdcall *fnUseMoney)(__int64 _i64SeqNum, bool _bIsMine, LPWSTR _szPlayerID, LPWSTR _szPlayerCharName, int _iServerIndex
										, LPWSTR _szIPAddr, LPWSTR _szToPlayerCharName, LPWSTR _szReservedFields
										, unsigned int _uiSpentCash, unsigned int _uiSpentMile, unsigned int pGotMile
										, unsigned int _uiItemListNum, TITEM_INFO_UNICODE _stItemList[MAX_ITEM_BUY_COUNT], int _iCSite);

typedef int (__stdcall *fnUseCoupon)(__int64 _i64SeqNum, LPWSTR _szPlayerID, int _iServerIndex, LPWSTR _szPlayerCharName
                                        , LPWSTR _szIPAddr, LPWSTR _szCouponNum, int _iCSite);
typedef int (__stdcall *fnSetMile)(__int64 _i64SeqNum, LPWSTR _szPlayerID, int _iServerIndex, LPWSTR _szPlayerCharName, LPWSTR _szIPAddr
                                      , char _cSetType, unsigned int _uiAddMile, TITEM_INFO_UNICODE _stSellItemInfo, int _iCSite);
typedef int (__stdcall *fnUseTrade)(__int64 _i64SeqNum, int _iBillType, int _iUseCash, int _iUseMile, LPWSTR _szPlayerID, int _iFee, int _iPeriod
									   , LPWSTR _szBillingKey, LPWSTR _szIPAddr, int _iServerIndex, int _iGroupIndex, LPWSTR _szReservedFields
									   , int _iItemListNum, TITEM_INFO_UNICODE _stItemList[MAX_ITEM_BUY_COUNT], LPWSTR _szRecvPlayerID, LPWSTR _szRecvIPAddr 
									   , int _iRecvFee, int _iRecvitemListNum, TITEM_INFO_UNICODE stRecvitemList[MAX_ITEM_BUY_COUNT], int _iCSite);

//DLL���� Call�� �Լ�
void __stdcall OnResError(const __int64 _i64SeqNum, const  int _iErrorCode);
void __stdcall OnServerAuth(const bool _bIsOK, const int _iErrorCode);
void __stdcall OnUserBalance(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiReMainCash, 
	const unsigned int _uiReMainMile, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);
void __stdcall OnUseMoney(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiReMainCash, 
	const unsigned int _uiReMainMile, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);
void __stdcall OnUseCoupon(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const char _cItemType, 
	LPCWSTR _szItemDesc, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);
void __stdcall OnSetMile(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiReMainMile, 
	const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);
void __stdcall OnUseTrade(const __int64 _i64SeqNum, const int _iBillType, const int _iUseCash, const int _iUseMile, 
	LPCWSTR _szPlayerID, LPCWSTR _szBillingKey, LPCWSTR _szRecvPlayerID, const int _iRecvCash, 
	const int _iRecvMile, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite);

//DLL�Լ� ������
fnInit_SDK Init_SDK = NULL;
fnSDKConnect SDKConnect = NULL;
fnOSP_CONNECTION_CHECK OSP_CONNECTION_CHECK = NULL;
fnUserBalance UserBalance = NULL;
fnUseMoney UseMoney  = NULL;
fnUseCoupon UseCoupon  = NULL;
fnSetMile SetMile  = NULL;
fnUseTrade UseTrade  = NULL;