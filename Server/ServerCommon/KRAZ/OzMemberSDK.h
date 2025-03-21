#include <stdio.h>
#include <windows.h>

class CFuncID{ 	
	private: 
	public:
		void (__stdcall *OnResError)(const unsigned __int64 _ui64SeqNum,const int _iErrorCode);
		void (__stdcall *OnServerAuth)(const bool nIsOK, const int _iErrorCode);
			  
		void (__stdcall *OnLoginResult)(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiPcRoomID, LPCWSTR _szToken, LPCWSTR _szReservedFields, const char _cResultValue, const int _iCSite);		
		void (__stdcall *OnEKEY_User)(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiTransID, const int _iEkeyType, LPCWSTR _szChallenge);
		void (__stdcall *OnSNDA_LoginResult)(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, LPCWSTR _szDigitID, const int _iResultValue, const int _iProtectFlag, const int _iLimitFlag, const unsigned int _uiTransID);
		void (__stdcall *OnSNDA_EzLoginResult)(const __int64 _i64SeqNum, LPCWSTR _szSessionID, LPCWSTR _szNumID, LPCWSTR _szPTID, const char _cResultValue);
};
CFuncID funcID;
//DLL함수 포인터

//DLL함수
typedef int (__stdcall *fnInit_SDK)(char *_szResult);
typedef int (__stdcall *fnConSock)(CFuncID x, bool debugMode);
typedef int (__stdcall *fnOSP_CONNECTION_CHECK)(void);
//UniCode 지원 함수
typedef int (__stdcall *fnNormalLogin)(unsigned __int64 _ui64SeqNum, LPCWSTR _szPID, LPCWSTR _szPWD, LPCWSTR _szIPAddr, int nCSite);
typedef int (__stdcall *fnWebLogin)(unsigned __int64 _ui64SeqNum, LPCWSTR _szPKeyID, LPCWSTR _szRequestTime, LPCWSTR _szIPAddr);
typedef int (__stdcall *fnKeyLogin)(unsigned __int64 _ui64SeqNum, LPCWSTR _szPID, LPCWSTR _szToken, LPCWSTR _szIPAddr, int nCSite);
typedef int (__stdcall *fnSNDA_PTLogin)(unsigned __int64 _ui64SeqNum, LPCWSTR _szPTID, LPCWSTR _szPWD, LPCWSTR _szIPAddr);
typedef int (__stdcall *fnEkeyLogin)(int nEkeyType,unsigned __int64 _ui64SeqNum, LPCWSTR _szEKey, LPCWSTR _szIPAddr, unsigned int nTransID);
typedef int (__stdcall *fnSNDA_EzLogin)(unsigned __int64 _ui64SeqNum, LPCWSTR _szSessionID, LPCWSTR _szResID, LPCWSTR _szActionType, LPCWSTR _szIPAddr);

fnInit_SDK Init_SDK = NULL;
fnConSock ConSock = NULL;
fnOSP_CONNECTION_CHECK OSP_CONNECTION_CHECK = NULL;
fnNormalLogin NormalLogin = NULL;
fnWebLogin WebLogin  = NULL;
fnKeyLogin KeyLogin  = NULL;
fnSNDA_PTLogin SNDA_PTLogin  = NULL;
fnEkeyLogin nEkeyLogin  = NULL;
fnSNDA_EzLogin SNDA_EzLogin  = NULL;

//UniCode CallBack 함수
void __stdcall OnResError(const unsigned __int64 _ui64SeqNum, const int _iErrorCode);
void __stdcall OnServerAuth(const bool _bIsOK, const int _iErrorCode);
void __stdcall OnLoginResult			(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiPcRoomID, LPCWSTR _szToken, LPCWSTR _szReservedFields, const char _cResultValue, const int _iCSite);
void __stdcall OnEKEY_User			(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiTransID, const int _iEkeyType, LPCWSTR _szChallenge);
void __stdcall OnSNDA_LoginResult		(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, LPCWSTR _szDigitID, const int _iResultValue, const int _iProtectFlag, const int _iLimitFlag, const unsigned int _uiTransID);
void __stdcall OnSNDA_EzLoginResult	(const __int64 _i64SeqNum, LPCWSTR _szSessionID, LPCWSTR _szNumID, LPCWSTR _szPTID, const char _cResultValue);