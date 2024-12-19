#pragma once

enum
{
#if defined( PRE_ADD_SKILLSLOT_EXTEND )
	_VERSION_KEYSETTING = 103,
#else
	_VERSION_KEYSETTING = 102,
#endif
};

enum
{
#if defined( PRE_ADD_SKILLSLOT_EXTEND )
	_VERSION_PADSETTING = 104,
#else
	_VERSION_PADSETTING = 103,
#endif
};

//키 컨트롤 조작 데이터
extern BYTE g_WrappingKeyData[WrappingKeyIndex_Amount];
extern BYTE g_DefaultWrappingKeyData[WrappingKeyIndex_Amount];

//실제 조이패드 컨트롤 조작 데이터
extern BYTE * g_WrappingJoypadData;
extern BYTE * g_WrappingJoypadAssistData;

//패드 별 컨트롤 조작 데이터
extern BYTE g_DefaultWrappingXBOX360Data[WrappingKeyIndex_Amount];
extern BYTE g_DefaultWrappingXBOX360AssistData[WrappingKeyIndex_Amount];
extern BYTE g_DefaultWrappingPSData[WrappingKeyIndex_Amount];
extern BYTE g_DefaultWrappingPSAssistData[WrappingKeyIndex_Amount];
extern BYTE g_DefaultWrappingHanData[WrappingKeyIndex_Amount];
extern BYTE g_DefaultWrappingHanAssistData[WrappingKeyIndex_Amount];
extern BYTE g_DefaultWrappingGAMMACData[WrappingKeyIndex_Amount];
extern BYTE g_DefaultWrappingGAMMACAssistData[WrappingKeyIndex_Amount];
extern BYTE g_WrappingCustomData[WrappingKeyIndex_Amount];
extern BYTE g_WrappingCustomAssistData[WrappingKeyIndex_Amount];

//키 UI 조작 데이터
extern BYTE g_UIWrappingKeyData[UIWrappingKeyIndex_Amount];
extern BYTE g_DefaultUIWrappingKeyData[UIWrappingKeyIndex_Amount];

//실제 조이패드 UI 조작 데이터
extern BYTE * g_UIWrappingJoypadData;
extern BYTE * g_UIWrappingJoypadAssistData;

//패드별 UI 조작 데이터
extern BYTE g_DefaultUIWrappingXBOX360Data[UIWrappingKeyIndex_Amount];
extern BYTE g_DefaultUIWrappingXBOX360AssistData[UIWrappingKeyIndex_Amount];
extern BYTE g_DefaultUIWrappingPSData[UIWrappingKeyIndex_Amount];
extern BYTE g_DefaultUIWrappingPSAssistData[UIWrappingKeyIndex_Amount];
extern BYTE g_DefaultUIWrappingHanData[UIWrappingKeyIndex_Amount];
extern BYTE g_DefaultUIWrappingHanAssistData[UIWrappingKeyIndex_Amount];
extern BYTE g_DefaultUIWrappingGAMMACData[UIWrappingKeyIndex_Amount];
extern BYTE g_DefaultUIWrappingGAMMACAssistData[UIWrappingKeyIndex_Amount];
extern BYTE g_UIWrappingCustomData[UIWrappingKeyIndex_Amount];
extern BYTE g_UIWrappingCustomAssistData[UIWrappingKeyIndex_Amount];

void Initialize_InputWrapper();
void _ResetDefault();

#define IW( KeyIndex)			g_WrappingKeyData[KeyIndex]
inline std::pair<BYTE, BYTE> IW_PAD(int KeyIndex)
{
	return std::make_pair(g_WrappingJoypadData[KeyIndex], g_WrappingJoypadAssistData[KeyIndex]);
}

extern WCHAR *g_szKeyString[256];
inline WCHAR * IW_STRING(BYTE KeyIndex, WCHAR ** nString = g_szKeyString)
{
	if( KeyIndex >= _countof( g_szKeyString ) )
		return nString[0];

	return nString[KeyIndex];
}

inline std::pair<BYTE, BYTE> IW_UI(int keyIndex)
{
	return std::make_pair(g_UIWrappingJoypadData[keyIndex], g_UIWrappingJoypadAssistData[keyIndex]);
}

extern WCHAR *g_szJoypadString[];
inline WCHAR * IW_PAD_STRING(int KeyIndex, WCHAR ** nString = g_szJoypadString)
{
	return KeyIndex != CInputJoyPad::NULL_VALUE ? nString[KeyIndex] : nString[CInputJoyPad::MAX_BUTTON];
}