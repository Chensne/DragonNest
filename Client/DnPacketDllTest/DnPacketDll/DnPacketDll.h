
#pragma once

#include <windows.h>
#include <string>

#ifdef  __cplusplus
extern "C" {
#endif

namespace DNTest
{
	#ifdef DLLEXPORT
		#define DNDeclSpec __declspec(dllexport)
	#else
		#define DNDeclSpec __declspec(dllimport)
	#endif

	class DNCrypt;

	class DNDeclSpec DNPacketDll
	{
		public:

			DNPacketDll();
			virtual ~DNPacketDll();

			int TestFunc(){ return S_OK; }

			void OnDisconnect();
			void OnConnect();
			UINT OnReceive( const char* pBuf, const UINT uiSize );
			virtual void OnSend( const char* pBuf, const UINT uiSize ) = 0;
			virtual void OnFinishLoginProcess() = 0;

			void SetAccountInfo( const WCHAR* pwszAccount, const WCHAR* pwszPW )
			{
				m_strAccount	= pwszAccount;
				m_strPW			= pwszPW;
			}
		private:

			void _DoEncrypt( int iMainCmd, int iSubCmd, char* pData, int iLen );
			void _DoDecrypt( int iMainCmd, int iSubCmd, char* pData, int iLen );
			void _DoParse( void* pEnPacket );

			DNCrypt* m_pCrypt;

			std::wstring m_strAccount;
			std::wstring m_strPW;
			std::wstring m_strIP;
	};
} // namespace DNTest

#ifdef  __cplusplus
}
#endif
