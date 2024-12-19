#pragma once
#include "EtUIType.h"

struct SUIControlInfo_01
{
	enum { VERSION = 0x101 };

	struct SUIButtonInfo{};
	struct SUICheckBoxInfo{};
	struct SUIRadioButtonInfo{};
	struct SUIEditBoxInfo{};
	struct SUIIMEEditBoxInfo{};
	struct SUIComboBoxInfo{};
	struct SUIListBoxInfo{};
	struct SUIProgressBarInfo{};
	struct SUITextBoxInfo{};
	struct SUIHtmlTextBoxInfo{};
	struct SUITreeInfo{};

	union
	{
		SUIButtonInfo ButtonInfo;
		SUICheckBoxInfo CheckBoxInfo;
		SUIRadioButtonInfo RadioButtonInfo;
		SUIEditBoxInfo EditBoxInfo;
		SUIIMEEditBoxInfo IMEEditBoxInfo;
		SUIComboBoxInfo ComboBoxInfo;
		SUIListBoxInfo ListBoxInfo;
		SUIProgressBarInfo ProgressBarInfo;
		SUITextBoxInfo TextBoxInfo;
		SUIHtmlTextBoxInfo HtmlTextBoxInfo;
		SUITreeInfo TreeInfo;
	};
};

struct SUIControlInfo_02
{
	enum { VERSION = 0x102 };

	struct SUIButtonInfo{};
	struct SUICheckBoxInfo{};
	struct SUIRadioButtonInfo{};
	struct SUIEditBoxInfo{};
	struct SUIIMEEditBoxInfo{};
	struct SUIComboBoxInfo{};
	struct SUIListBoxInfo{};
	struct SUIProgressBarInfo{};
	struct SUITextBoxInfo{};
	struct SUIHtmlTextBoxInfo{};
	struct SUITreeInfo{};
	struct SUIScrollBarInfo{};

	union
	{
		SUIButtonInfo ButtonInfo;
		SUICheckBoxInfo CheckBoxInfo;
		SUIRadioButtonInfo RadioButtonInfo;
		SUIEditBoxInfo EditBoxInfo;
		SUIIMEEditBoxInfo IMEEditBoxInfo;
		SUIComboBoxInfo ComboBoxInfo;
		SUIListBoxInfo ListBoxInfo;
		SUIProgressBarInfo ProgressBarInfo;
		SUITextBoxInfo TextBoxInfo;
		SUIHtmlTextBoxInfo HtmlTextBoxInfo;
		SUITreeInfo TreeInfo;
		SUIScrollBarInfo ScrollBarInfo;
	};
};

struct SUIControlInfo_03
{
	enum { VERSION = 0x103 };

	struct SUIStaticInfo{};
	struct SUIButtonInfo{};
	struct SUICheckBoxInfo{};
	struct SUIRadioButtonInfo{};
	struct SUIEditBoxInfo{};
	struct SUIIMEEditBoxInfo{};
	struct SUIComboBoxInfo{};
	struct SUIListBoxInfo{};
	struct SUIProgressBarInfo{};
	struct SUITextBoxInfo{};
	struct SUIHtmlTextBoxInfo{};
	struct SUITreeInfo{};
	struct SUIScrollBarInfo{};

	union
	{
		SUIStaticInfo StaticInfo;
		SUIButtonInfo ButtonInfo;
		SUICheckBoxInfo CheckBoxInfo;
		SUIRadioButtonInfo RadioButtonInfo;
		SUIEditBoxInfo EditBoxInfo;
		SUIIMEEditBoxInfo IMEEditBoxInfo;
		SUIComboBoxInfo ComboBoxInfo;
		SUIListBoxInfo ListBoxInfo;
		SUIProgressBarInfo ProgressBarInfo;
		SUITextBoxInfo TextBoxInfo;
		SUIHtmlTextBoxInfo HtmlTextBoxInfo;
		SUITreeInfo TreeInfo;
		SUIScrollBarInfo ScrollBarInfo;
	};
};

struct SUIControlInfo_04
{
	enum { VERSION = 0x104 };

	struct SUIStaticInfo{};
	struct SUIButtonInfo{};
	struct SUICheckBoxInfo{};
	struct SUIRadioButtonInfo{};
	struct SUIEditBoxInfo{};
	struct SUIIMEEditBoxInfo{};
	struct SUIComboBoxInfo{};
	struct SUIListBoxInfo{};
	struct SUIProgressBarInfo{};
	struct SUITextBoxInfo{};
	struct SUIHtmlTextBoxInfo{};
	struct SUITreeInfo{};
	struct SUIScrollBarInfo{};
	struct SUIQuestTreeInfo{};

	union
	{
		SUIStaticInfo StaticInfo;
		SUIButtonInfo ButtonInfo;
		SUICheckBoxInfo CheckBoxInfo;
		SUIRadioButtonInfo RadioButtonInfo;
		SUIEditBoxInfo EditBoxInfo;
		SUIIMEEditBoxInfo IMEEditBoxInfo;
		SUIComboBoxInfo ComboBoxInfo;
		SUIListBoxInfo ListBoxInfo;
		SUIProgressBarInfo ProgressBarInfo;
		SUITextBoxInfo TextBoxInfo;
		SUIHtmlTextBoxInfo HtmlTextBoxInfo;
		SUITreeInfo TreeInfo;
		SUIScrollBarInfo ScrollBarInfo;
		SUIQuestTreeInfo QuestTreeInfo;
	};
};

struct SUIControlInfo_05
{
	enum { VERSION = 0x105 };

	struct SUIStaticInfo{};
	struct SUIButtonInfo{};
	struct SUICheckBoxInfo{};
	struct SUIRadioButtonInfo{};
	struct SUIEditBoxInfo{};
	struct SUIIMEEditBoxInfo{};
	struct SUIComboBoxInfo{};
	struct SUIListBoxInfo{};
	struct SUIProgressBarInfo{};
	struct SUITextBoxInfo{};
	struct SUIHtmlTextBoxInfo{};
	struct SUITreeInfo{};
	struct SUIScrollBarInfo{};
	struct SUIQuestTreeInfo{};
	struct SUIAnimationInfo{};

	union
	{
		SUIStaticInfo StaticInfo;
		SUIButtonInfo ButtonInfo;
		SUICheckBoxInfo CheckBoxInfo;
		SUIRadioButtonInfo RadioButtonInfo;
		SUIEditBoxInfo EditBoxInfo;
		SUIIMEEditBoxInfo IMEEditBoxInfo;
		SUIComboBoxInfo ComboBoxInfo;
		SUIListBoxInfo ListBoxInfo;
		SUIProgressBarInfo ProgressBarInfo;
		SUITextBoxInfo TextBoxInfo;
		SUIHtmlTextBoxInfo HtmlTextBoxInfo;
		SUITreeInfo TreeInfo;
		SUIScrollBarInfo ScrollBarInfo;
		SUIQuestTreeInfo QuestTreeInfo;
		SUIAnimationInfo AnimationInfo;
	};
};

struct SUIControlInfo_06
{
	enum { VERSION = 0x106 };

	struct SUIStaticInfo{};
	struct SUIButtonInfo{};
	struct SUICheckBoxInfo{};
	struct SUIRadioButtonInfo{};
	struct SUIEditBoxInfo{};
	struct SUIIMEEditBoxInfo{};
	struct SUIComboBoxInfo{};
	struct SUIListBoxInfo{};
	struct SUIProgressBarInfo{};
	struct SUITextBoxInfo{};
	struct SUIHtmlTextBoxInfo{};
	struct SUITreeInfo{};
	struct SUIScrollBarInfo{};
	struct SUIQuestTreeInfo{};
	struct SUIAnimationInfo{};
	struct SUILineEditBoxInfo{};

	union
	{
		SUIStaticInfo StaticInfo;
		SUIButtonInfo ButtonInfo;
		SUICheckBoxInfo CheckBoxInfo;
		SUIRadioButtonInfo RadioButtonInfo;
		SUIEditBoxInfo EditBoxInfo;
		SUIIMEEditBoxInfo IMEEditBoxInfo;
		SUIComboBoxInfo ComboBoxInfo;
		SUIListBoxInfo ListBoxInfo;
		SUIProgressBarInfo ProgressBarInfo;
		SUITextBoxInfo TextBoxInfo;
		SUIHtmlTextBoxInfo HtmlTextBoxInfo;
		SUITreeInfo TreeInfo;
		SUIScrollBarInfo ScrollBarInfo;
		SUIQuestTreeInfo QuestTreeInfo;
		SUIAnimationInfo AnimationInfo;
		SUILineEditBoxInfo LineEditBoxInfo;
	};
};

////////////////////////////////////////////////////////////////

struct SUIButtonInfo_01
{
	enum { VERSION = 0x101 };

	float fMoveOffsetHori;
	float fMoveOffsetVert;
	UIButtonType ButtonType;
};

struct SUIButtonInfo_02
{
	enum { VERSION = 0x102 };

	float fMoveOffsetHori;
	float fMoveOffsetVert;
	UIButtonType ButtonType;
	char szSoundFileName[_MAX_PATH];
};

struct SUIButtonInfo_03
{
	enum { VERSION = 0x103 };

	float fMoveOffsetHori;
	float fMoveOffsetVert;
	UIButtonType ButtonType;
};

struct SUIRadioButtonInfo_01
{
	enum { VERSION = 0x101 };

	float fSpace;
	int nAllign;
};

struct SUIRadioButtonInfo_02
{
	enum { VERSION = 0x102 };

	float fSpace;
	int nAllign;
	int nIconIndex;
};

struct SUIRadioButtonInfo_03
{
	enum { VERSION = 0x103 };

	float fSpace;
	int nAllign;
	int nIconIndex;
	float fFontHoriOffset;
	float fFontVertOffset;
};

struct SUIRadioButtonInfo_04
{
	enum { VERSION = 0x104 };

	float fSpace;
	int nAllign;
	int nIconIndex;
};

struct SUIRadioButtonInfo_05
{
	enum { VERSION = 0x105 };

	float fSpace;
	int nAllign;
	int nIconIndex;
	float fMoveOffsetHori;
	float fMoveOffsetVert;
};

struct SUIEditBoxInfo_01
{
	enum { VERSION = 0x101 };

	float fSpace;
	float fBorder;
	DWORD dwTextColor;
	DWORD dwSelTextColor;
	DWORD dwSelBackColor;
	DWORD dwCaretColor;
	BOOL bPassword;
};

struct SUIEditBoxInfo_02
{
	enum { VERSION = 0x102 };

	float fSpace;
	float fBorder;
	DWORD dwTextColor;
	DWORD dwSelTextColor;
	DWORD dwSelBackColor;
	DWORD dwCaretColor;
	BOOL bPassword;
	DWORD dwMaxChars;
};

struct SUIIMEEditBoxInfo_01
{
	enum { VERSION = 0x101 };

	float fSpace;
	float fBorder;
	float fIndicatorSize;
	DWORD dwTextColor;
	DWORD dwSelTextColor;
	DWORD dwSelBackColor;
	DWORD dwCaretColor;
	DWORD dwCompTextColor;
	DWORD dwCompBackColor;
	DWORD dwCompCaretColor;
	DWORD dwCandiTextColor;
	DWORD dwCandiBackColor;
	DWORD dwCandiSelTextColor;
	DWORD dwCandiSelBackColor;
	DWORD dwIndiTextColor;
	DWORD dwReadingTextColor;
	DWORD dwReadingBackColor;
	DWORD dwReadingSelTextColor;
	DWORD dwReadingSelBackColor;
};

struct SUIIMEEditBoxInfo_02
{
	enum { VERSION = 0x102 };

	float fSpace;
	float fBorder;
	float fIndicatorSize;
	DWORD dwTextColor;
	DWORD dwSelTextColor;
	DWORD dwSelBackColor;
	DWORD dwCaretColor;
	DWORD dwCompTextColor;
	DWORD dwCompBackColor;
	DWORD dwCompCaretColor;
	DWORD dwCandiTextColor;
	DWORD dwCandiBackColor;
	DWORD dwCandiSelTextColor;
	DWORD dwCandiSelBackColor;
	DWORD dwIndiTextColor;
	DWORD dwReadingTextColor;
	DWORD dwReadingBackColor;
	DWORD dwReadingSelTextColor;
	DWORD dwReadingSelBackColor;
	DWORD dwMaxChars;
};

struct SUITextBoxInfo_01
{
	enum { VERSION = 0x101 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bUseMinThumbSize;
};

struct SUITextBoxInfo_02
{
	enum { VERSION = 0x102 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bUseMinThumbSize;
	bool bVariableTextBox;
	float fLineSpace;
	UIAllignHoriType AllignHori;
	UIAllignVertType AllignVert;
};

struct SUITextBoxInfo_03
{
	enum { VERSION = 0x103 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bUseMinThumbSize;
	float fLineSpace;
	UIAllignHoriType AllignHori;
	UIAllignVertType AllignVert;
	UITextBoxVariableType VariableType;
};

struct SUITextBoxInfo_04
{
	enum { VERSION = 0x104 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bUseMinThumbSize;
	float fLineSpace;
	UIAllignHoriType AllignHori;
	UIAllignVertType AllignVert;
	UITextBoxVariableType VariableType;
	bool bRollOver;
};

struct SUITextBoxInfo_05
{
	enum { VERSION = 0x105 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bFixThumb;
	float fThumbSize;
	float fLineSpace;
	UIAllignHoriType AllignHori;
	UIAllignVertType AllignVert;
	UITextBoxVariableType VariableType;
	bool bRollOver;
};

struct SUITextBoxInfo_06
{
	enum { VERSION = 0x106 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	float fLineSpace;
	UIAllignHoriType AllignHori;
	UIAllignVertType AllignVert;
	UITextBoxVariableType VariableType;
	bool bRollOver;
};

struct SUIHtmlTextBoxInfo_01
{
	enum { VERSION = 0x101 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bUseMinThumbSize;
	bool bVariableTextBox;
	float fLineSpace;
	UIAllignHoriType AllignHori;
	UIAllignVertType AllignVert;
};

struct SUIHtmlTextBoxInfo_02
{
	enum { VERSION = 0x102 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bFixThumb;
	float fThumbSize;
	bool bVariableTextBox;
	float fLineSpace;
	UIAllignHoriType AllignHori;
	UIAllignVertType AllignVert;
};

struct SUIHtmlTextBoxInfo_03
{
	enum { VERSION = 0x103 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bVariableTextBox;
	float fLineSpace;
	UIAllignHoriType AllignHori;
	UIAllignVertType AllignVert;
};

struct SUITreeInfo_01
{
	enum { VERSION = 0x101 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bUseMinThumbSize;
	float fLineSpace;
};

struct SUITreeInfo_02
{
	enum { VERSION = 0x102 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bUseMinThumbSize;
	float fLineSpace;
	float fMainButtonSize;
	float fSubButtonSize;
	float fIndentSize;
};

struct SUITreeInfo_03
{
	enum { VERSION = 0x103 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bUseMinThumbSize;
	float fLineSpace;
	float fMainButtonSize;
	float fSubButtonSize;
	float fIndentSize;
	float fBorderSize;
};

struct SUITreeInfo_04
{
	enum { VERSION = 0x104 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bFixThumb;
	float fThumbSize;
	float fLineSpace;
	float fMainButtonSize;
	float fSubButtonSize;
	float fIndentSize;
	float fBorderSize;
};

struct SUITreeInfo_05
{
	enum { VERSION = 0x105 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	bool bFixThumb;
	float fThumbSize;
	float fLineSpace;
	float fMainButtonSize;
	float fSubButtonSize;
	float fIndentSize;
	float fBorderSize;
	float fStateSize;
};

struct SUITreeInfo_06
{
	enum { VERSION = 0x106 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	float fLineSpace;
	float fMainButtonSize;
	float fSubButtonSize;
	float fIndentSize;
	float fBorderSize;
	float fStateSize;
};

struct SUITreeInfo_07
{
	enum { VERSION = 0x107 };

	bool bVerticalScrollBar;
	float fScrollBarSize;
	int nScrollBarTemplate;
	float fLineSpace;
	float fIndentSize;
};

struct SUIScrollBarInfo_01
{
	enum { VERSION = 0x101 };

	float fThumbSize;
	bool bfixableThumb;
};

struct SUIScrollBarInfo_02
{
	enum { VERSION = 0x102 };

	bool bfixableThumb;
};

struct SUIComboBoxInfo_01
{
	enum { VERSION = 0x101 };

	float fMoveOffsetHori;
	float fMoveOffsetVert;
	float fDropdownSize;
	float fScrollBarSize;
	int nScrollBarTemplate;
};

struct SUIComboBoxInfo_02
{
	enum { VERSION = 0x102 };

	float fMoveOffsetHori;
	float fMoveOffsetVert;
	float fDropdownSize;
	float fScrollBarSize;
};

struct SUIListBoxInfo_01
{
	enum { VERSION = 0x101 };

	float fScrollBarSize;
	int nScrollBarTemplate;
	float fBorder;
	float fMargin;
};

struct SUIListBoxInfo_02
{
	enum { VERSION = 0x102 };

	float fScrollBarSize;
	float fBorder;
	float fMargin;
};

struct SUIStaticInfo_01
{
	enum { VERSION = 0x101 };

	bool bVariableWidth;
};

struct SUIStaticInfo_02
{
	enum { VERSION = 0x102 };

	bool bVariableWidth;
	float fVariableWidthLenth;
};

struct SUILineEditBoxInfo_01
{
	enum { VERSION = 0x101 };

	DWORD dwTextColor;
	DWORD dwSelTextColor;
	DWORD dwSelBackColor;
	DWORD dwCaretColor;
};

struct SUILineIMEEditBoxInfo_01
{
	enum { VERSION = 0x101 };

	DWORD dwTextColor;
	DWORD dwSelTextColor;
	DWORD dwSelBackColor;
	DWORD dwCaretColor;
	DWORD dwCompTextColor;
	DWORD dwCompBackColor;
	DWORD dwCompCaretColor;
	DWORD dwCandiTextColor;
	DWORD dwCandiBackColor;
	DWORD dwCandiSelTextColor;
	DWORD dwCandiSelBackColor;
	DWORD dwReadingTextColor;
	DWORD dwReadingBackColor;
	DWORD dwReadingSelTextColor;
	DWORD dwReadingSelBackColor;
};

struct SUIListBoxExInfo_01
{
	enum { VERSION = 0x101 };

	float fScrollBarSize;
	float fBorder;
	float fMargin;
	float fLineSpace;
};

#define _ANIMATION_FRAME_CONTROL
#ifdef _ANIMATION_FRAME_CONTROL
struct SUIAnimationInfo_01
{
	enum { VERSION = 0x101 };

	bool  bLoop;
};
#endif