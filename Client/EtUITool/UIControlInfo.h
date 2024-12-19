#pragma once
#include "EtUIType.h"
#include "DebugSet.h"
#include "UIControlInfoOld.h"
#include "EtUIControlProperty.h"

// ���ø� ������Ƽ�� ������ �����.
// �ٸ� ���� ���ڸ� �������� ���Ǵ� ���ø� �⺻ Ʋ.
//
// ���ø� ������Ƽ �� �� 4��,
// UI Type
// Template Name
// Template Width
// Template Height
// �Ʒ��� �ڽ��� ���� ����� �߰��ȴ�.
//
struct SUIControlInfo
{
	enum { VERSION = 0x107 };

	struct SUIStaticInfo
	{
		enum { VERSION = 0x103 };

		int nFrameLeft;
		int nFrameTop;
		int nFrameRight;
		int nFrameBottom;

		void Load_01( CStream &stream )
		{
			// SUIStaticInfo_01 ����ü���� bool bVariableWidth �ϳ��� ������ �־�����,
			// ���� �����ϹǷ� Seek�� �ѱ�� �ȴ�.
			stream.SeekCur<bool>();
		}

		void Load_02( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIStaticInfo_01::VERSION: Load_01(stream); break;
			case SUIStaticInfo_02::VERSION: Load_02(stream); break;
			case VERSION:
				{
					stream >> nFrameLeft;
					stream >> nFrameTop;
					stream >> nFrameRight;
					stream >> nFrameBottom;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIStaticInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << nFrameLeft;
			stream << nFrameTop;
			stream << nFrameRight;
			stream << nFrameBottom;
		}
	};

	struct SUIButtonInfo
	{
		enum { VERSION = 0x104 };

		float fMoveOffsetHori;
		float fMoveOffsetVert;
		UIButtonType ButtonType;
		bool bVariableWidth;
		float fVariableWidthLenth;

		void Load_03( CStream &stream )
		{
			stream >> fMoveOffsetHori;
			stream >> fMoveOffsetVert;
			stream >> ButtonType;
		}

		void Load_02( CStream &stream )
		{
			stream >> fMoveOffsetHori;
			stream >> fMoveOffsetVert;
			stream >> ButtonType;
			stream.Seek( sizeof(char)*_MAX_PATH, SEEK_CUR );
		}

		void Load_01( CStream &stream )
		{
			stream >> fMoveOffsetHori;
			stream >> fMoveOffsetVert;
			stream >> ButtonType;
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIButtonInfo_01::VERSION: Load_01(stream); break;
			case SUIButtonInfo_02::VERSION: Load_02(stream); break;
			case SUIButtonInfo_03::VERSION: Load_03(stream); break;
			case VERSION:
				{
					stream >> fMoveOffsetHori;
					stream >> fMoveOffsetVert;
					stream >> ButtonType;
					stream >> bVariableWidth;
					stream >> fVariableWidthLenth;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIButtonInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fMoveOffsetHori;
			stream << fMoveOffsetVert;
			stream << ButtonType;
			stream << bVariableWidth;
			stream << fVariableWidthLenth;
		}
	};

	struct SUICheckBoxInfo
	{
		enum { VERSION = 0x101 };

		float fSpace;
		int nAllign;

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case VERSION:
				{
					stream >> fSpace;
					stream >> nAllign;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUICheckBoxInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fSpace;
			stream << nAllign;
		}
	};

	struct SUIRadioButtonInfo
	{
		enum { VERSION = 0x106 };

		float fSpace;
		int nAllign;
		int nIconIndex;
		float fMoveOffsetHori;
		float fMoveOffsetVert;
		float fVariableWidthLength;

		void Load_01( CStream &stream )
		{
			stream >> fSpace;
			stream >> nAllign;
		}

		void Load_02( CStream &stream )
		{
			stream >> fSpace;
			stream >> nAllign;
			stream >> nIconIndex;
		}

		void Load_03( CStream &stream )
		{
			stream >> fSpace;
			stream >> nAllign;
			stream >> nIconIndex;
			stream.SeekCur<float>();
			stream.SeekCur<float>();
		}

		void Load_04( CStream &stream )
		{
			stream >> fSpace;
			stream >> nAllign;
			stream >> nIconIndex;
		}

		void Load_05( CStream &stream )
		{
			stream >> fSpace;
			stream >> nAllign;
			stream >> nIconIndex;
			stream >> fMoveOffsetHori;
			stream >> fMoveOffsetVert;
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIRadioButtonInfo_01::VERSION: Load_01(stream); break;
			case SUIRadioButtonInfo_02::VERSION: Load_02(stream); break;
			case SUIRadioButtonInfo_03::VERSION: Load_03(stream); break;
			case SUIRadioButtonInfo_04::VERSION: Load_04(stream); break;
			case SUIRadioButtonInfo_05::VERSION: Load_05(stream); break;
			case VERSION:
				{
					stream >> fSpace;
					stream >> nAllign;
					stream >> nIconIndex;
					stream >> fMoveOffsetHori;
					stream >> fMoveOffsetVert;
					stream >> fVariableWidthLength;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIRadioButtonInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fSpace;
			stream << nAllign;
			stream << nIconIndex;
			stream << fMoveOffsetHori;
			stream << fMoveOffsetVert;
			stream << fVariableWidthLength;
		}
	};

	struct SUIEditBoxInfo
	{
		enum { VERSION = 0x103 };

		float fSpace;
		float fBorder;
		DWORD dwTextColor;
		DWORD dwSelTextColor;
		DWORD dwSelBackColor;
		DWORD dwCaretColor;

		void Load_01( CStream &stream )
		{
			stream >> fSpace;
			stream >> fBorder;
			stream >> dwTextColor;
			stream >> dwTextColor;
			stream >> dwSelTextColor;
			stream >> dwSelBackColor;
			stream >> dwCaretColor;
			stream.SeekCur<BOOL>();
		}

		void Load_02( CStream &stream )
		{
			stream >> fSpace;
			stream >> fBorder;
			stream >> dwTextColor;
			stream >> dwTextColor;
			stream >> dwSelTextColor;
			stream >> dwSelBackColor;
			stream >> dwCaretColor;
			stream.SeekCur<BOOL>();
			stream.SeekCur<DWORD>();
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIEditBoxInfo_01::VERSION: Load_01(stream); break;
			case SUIEditBoxInfo_02::VERSION: Load_02(stream); break;
			case VERSION:
				{
					stream >> fSpace;
					stream >> fBorder;
					stream >> dwTextColor;
					stream >> dwSelTextColor;
					stream >> dwSelBackColor;
					stream >> dwCaretColor;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIEditBoxInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fSpace;
			stream << fBorder;
			stream << dwTextColor;
			stream << dwSelTextColor;
			stream << dwSelBackColor;
			stream << dwCaretColor;
		}
	};

	struct SUIIMEEditBoxInfo
	{
		enum { VERSION = 0x103 };

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

		void Load_01( CStream &stream )
		{
			stream >> fSpace;
			stream >> fBorder;
			stream >> fIndicatorSize;
			stream >> dwTextColor;
			stream >> dwSelTextColor;
			stream >> dwSelBackColor;
			stream >> dwCaretColor;
			stream >> dwCompTextColor;
			stream >> dwCompBackColor;
			stream >> dwCompCaretColor;
			stream >> dwCandiTextColor;
			stream >> dwCandiBackColor;
			stream >> dwCandiSelTextColor;
			stream >> dwCandiSelBackColor;
			stream >> dwIndiTextColor;
			stream >> dwReadingTextColor;
			stream >> dwReadingBackColor;
			stream >> dwReadingSelTextColor;
			stream >> dwReadingSelBackColor;
		}

		void Load_02( CStream &stream )
		{
			stream >> fSpace;
			stream >> fBorder;
			stream >> fIndicatorSize;
			stream >> dwTextColor;
			stream >> dwSelTextColor;
			stream >> dwSelBackColor;
			stream >> dwCaretColor;
			stream >> dwCompTextColor;
			stream >> dwCompBackColor;
			stream >> dwCompCaretColor;
			stream >> dwCandiTextColor;
			stream >> dwCandiBackColor;
			stream >> dwCandiSelTextColor;
			stream >> dwCandiSelBackColor;
			stream >> dwIndiTextColor;
			stream >> dwReadingTextColor;
			stream >> dwReadingBackColor;
			stream >> dwReadingSelTextColor;
			stream >> dwReadingSelBackColor;
			stream.SeekCur<DWORD>();
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIIMEEditBoxInfo_01::VERSION: Load_01(stream); break;
			case SUIIMEEditBoxInfo_02::VERSION: Load_02(stream); break;
			case VERSION:
				{
					stream >> fSpace;
					stream >> fBorder;
					stream >> fIndicatorSize;
					stream >> dwTextColor;
					stream >> dwSelTextColor;
					stream >> dwSelBackColor;
					stream >> dwCaretColor;
					stream >> dwCompTextColor;
					stream >> dwCompBackColor;
					stream >> dwCompCaretColor;
					stream >> dwCandiTextColor;
					stream >> dwCandiBackColor;
					stream >> dwCandiSelTextColor;
					stream >> dwCandiSelBackColor;
					stream >> dwIndiTextColor;
					stream >> dwReadingTextColor;
					stream >> dwReadingBackColor;
					stream >> dwReadingSelTextColor;
					stream >> dwReadingSelBackColor;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIIMEEditBoxInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fSpace;
			stream << fBorder;
			stream << fIndicatorSize;
			stream << dwTextColor;
			stream << dwSelTextColor;
			stream << dwSelBackColor;
			stream << dwCaretColor;
			stream << dwCompTextColor;
			stream << dwCompBackColor;
			stream << dwCompCaretColor;
			stream << dwCandiTextColor;
			stream << dwCandiBackColor;
			stream << dwCandiSelTextColor;
			stream << dwCandiSelBackColor;
			stream << dwIndiTextColor;
			stream << dwReadingTextColor;
			stream << dwReadingBackColor;
			stream << dwReadingSelTextColor;
			stream << dwReadingSelBackColor;
		}
	};

	struct SUIComboBoxInfo
	{
		enum { VERSION = 0x103 };

		float fMoveOffsetHori;
		float fMoveOffsetVert;
		float fDropdownSize;
		float fScrollBarSize;
		float fVariableWidthLenth;

		void Load_01( CStream &stream )
		{
			stream >> fMoveOffsetHori;
			stream >> fMoveOffsetVert;
			stream >> fDropdownSize;
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
		}

		void Load_02( CStream &stream )
		{
			stream >> fMoveOffsetHori;
			stream >> fMoveOffsetVert;
			stream >> fDropdownSize;
			stream >> fScrollBarSize;
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIComboBoxInfo_01::VERSION: Load_01(stream); break;
			case SUIComboBoxInfo_02::VERSION: Load_02(stream); break;
			case VERSION:
				{
					stream >> fMoveOffsetHori;
					stream >> fMoveOffsetVert;
					stream >> fDropdownSize;
					stream >> fScrollBarSize;
					stream >> fVariableWidthLenth;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIComboBoxInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fMoveOffsetHori;
			stream << fMoveOffsetVert;
			stream << fDropdownSize;
			stream << fScrollBarSize;
			stream << fVariableWidthLenth;
		}
	};

	struct SUIListBoxInfo
	{
		enum { VERSION = 0x103 };

		float fScrollBarSize;
		float fBorder;
		float fMargin;
		float fLineSpace;

		void Load_01( CStream &stream )
		{
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream >> fBorder;
			stream >> fMargin;
		}

		void Load_02( CStream &stream )
		{
			stream >> fScrollBarSize;
			stream >> fBorder;
			stream >> fMargin;
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIListBoxInfo_01::VERSION: Load_01(stream); break;
			case SUIListBoxInfo_02::VERSION: Load_02(stream); break;
			case VERSION:
				{
					stream >> fScrollBarSize;
					stream >> fBorder;
					stream >> fMargin;
					stream >> fLineSpace;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIListBoxInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fScrollBarSize;
			stream << fBorder;
			stream << fMargin;
			stream << fLineSpace;
		}
	};

	struct SUIProgressBarInfo
	{
		enum { VERSION = 0x101 };

		float fHoriBorder;
		float fVertBorder;

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case VERSION:
				{
					stream >> fHoriBorder;
					stream >> fVertBorder;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIProgressBarInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fHoriBorder;
			stream << fVertBorder;
		}
	};

	struct SUITextBoxInfo
	{
		enum { VERSION = 0x107 };

		float fScrollBarSize;

		void Load_01( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
		}

		void Load_02( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream.SeekCur<UIAllignHoriType>();
			stream.SeekCur<UIAllignVertType>();
		}

		void Load_03( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream.SeekCur<UIAllignHoriType>();
			stream.SeekCur<UIAllignVertType>();
			stream.SeekCur<UITextBoxVariableType>();
		}

		void Load_04( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream.SeekCur<UIAllignHoriType>();
			stream.SeekCur<UIAllignVertType>();
			stream.SeekCur<UITextBoxVariableType>();
			stream.SeekCur<bool>();
		}

		void Load_05( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream.SeekCur<float>();
			stream.SeekCur<UIAllignHoriType>();
			stream.SeekCur<UIAllignVertType>();
			stream.SeekCur<UITextBoxVariableType>();
			stream.SeekCur<bool>();
		}

		void Load_06( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<float>();
			stream.SeekCur<UIAllignHoriType>();
			stream.SeekCur<UIAllignVertType>();
			stream.SeekCur<UITextBoxVariableType>();
			stream.SeekCur<bool>();
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUITextBoxInfo_01::VERSION: Load_01(stream); break;
			case SUITextBoxInfo_02::VERSION: Load_02(stream); break;
			case SUITextBoxInfo_03::VERSION: Load_03(stream); break;
			case SUITextBoxInfo_04::VERSION: Load_04(stream); break;
			case SUITextBoxInfo_05::VERSION: Load_05(stream); break;
			case SUITextBoxInfo_06::VERSION: Load_06(stream); break;
			case VERSION:
				{
					stream >> fScrollBarSize;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUITextBoxInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fScrollBarSize;
		}
	};

	struct SUIHtmlTextBoxInfo
	{
		enum { VERSION = 0x104 };

		float fScrollBarSize;

		void Load_01( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream.SeekCur<UIAllignHoriType>();
			stream.SeekCur<UIAllignVertType>();
		}

		void Load_02( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream.SeekCur<UIAllignHoriType>();
			stream.SeekCur<UIAllignVertType>();
		}

		void Load_03( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream.SeekCur<UIAllignHoriType>();
			stream.SeekCur<UIAllignVertType>();
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIHtmlTextBoxInfo_01::VERSION: Load_01(stream); break;
			case SUIHtmlTextBoxInfo_02::VERSION: Load_02(stream); break;
			case SUIHtmlTextBoxInfo_03::VERSION: Load_03(stream); break;
			case VERSION:
				{
					stream >> fScrollBarSize;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIHtmlTextBoxInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fScrollBarSize;
		}
	};

	struct SUITreeInfo
	{
		enum { VERSION = 0x108 };

		float fScrollBarSize;
		float fLineSpace;
		float fIndentSize;

		void Load_01( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream >> fLineSpace;
		}

		void Load_02( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream >> fLineSpace;
			stream.SeekCur<float>();
			stream.SeekCur<float>();
			stream >> fIndentSize;
		}

		void Load_03( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream >> fLineSpace;
			stream.SeekCur<float>();
			stream.SeekCur<float>();
			stream >> fIndentSize;
			stream.SeekCur<float>();
		}

		void Load_04( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream >> fLineSpace;
			stream.SeekCur<float>();
			stream.SeekCur<float>();
			stream >> fIndentSize;
			stream.SeekCur<float>();
		}

		void Load_05( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream.SeekCur<bool>();
			stream.SeekCur<float>();
			stream >> fLineSpace;
			stream.SeekCur<float>();
			stream.SeekCur<float>();
			stream >> fIndentSize;
			stream.SeekCur<float>();
			stream.SeekCur<float>();
		}

		void Load_06( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream >> fLineSpace;
			stream.SeekCur<float>();
			stream.SeekCur<float>();
			stream >> fIndentSize;
			stream.SeekCur<float>();
			stream.SeekCur<float>();
		}

		void Load_07( CStream &stream )
		{
			stream.SeekCur<bool>();
			stream >> fScrollBarSize;
			stream.SeekCur<int>();
			stream >> fLineSpace;
			stream >> fIndentSize;
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUITreeInfo_01::VERSION: Load_01(stream); break;
			case SUITreeInfo_02::VERSION: Load_02(stream); break;
			case SUITreeInfo_03::VERSION: Load_03(stream); break;
			case SUITreeInfo_04::VERSION: Load_04(stream); break;
			case SUITreeInfo_05::VERSION: Load_05(stream); break;
			case SUITreeInfo_06::VERSION: Load_06(stream); break;
			case SUITreeInfo_07::VERSION: Load_07(stream); break;
			case VERSION:
				{
					stream >> fScrollBarSize;
					stream >> fLineSpace;
					stream >> fIndentSize;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUITreeInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fScrollBarSize;
			stream << fLineSpace;
			stream << fIndentSize;
		}
	};

	struct SUIScrollBarInfo
	{
		enum { VERSION = 0x103 };

		float fThumbVariableLenth;

		void Load_01( CStream &stream )
		{
			stream.SeekCur<float>();
			stream.SeekCur<bool>();
		}

		void Load_02( CStream &stream )
		{
			stream.SeekCur<bool>();
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIScrollBarInfo_01::VERSION: Load_01(stream); break;
			case SUIScrollBarInfo_02::VERSION: Load_02(stream); break;
			case VERSION:
				{
					stream >> fThumbVariableLenth;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIScrollBarInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fThumbVariableLenth;
		}
	};

	struct SUIQuestTreeInfo
	{
		enum { VERSION = 0x101 };

		float fScrollBarSize;
		float fLineSpace;
		float fIndentSize;

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case VERSION:
				{
					stream >> fScrollBarSize;
					stream >> fLineSpace;
					stream >> fIndentSize;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIQuestTreeInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fScrollBarSize;
			stream << fLineSpace;
			stream << fIndentSize;
		}
	};

	struct SUIAnimationInfo
	{
#define _ANIMATION_PLAY_CONTROL
#ifdef _ANIMATION_PLAY_CONTROL
		enum { VERSION = 0x102 };

		bool  bLoop;
		UIAnimationPlayTimeType PlayTimeType;
		int timePerAction;

		void Load_01( CStream& stream )
		{
			stream >> bLoop;
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIAnimationInfo_01::VERSION: Load_01( stream ); break;
			case VERSION:
				{
					stream >> bLoop;
					stream >> PlayTimeType;
					stream >> timePerAction;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIAnimation::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << bLoop;
			stream << PlayTimeType;
			stream << timePerAction;
		}
#else
		enum { VERSION = 0x101 };

		bool bLoop;

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case VERSION:
				{
					stream >> bLoop;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIAnimation::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << bLoop;
		}
#endif // #ifdef _ANIMATION_PLAY_CONTROL
	};

	struct SUILineEditBoxInfo
	{
		enum { VERSION = 0x102 };

		DWORD dwTextColor;
		DWORD dwSelTextColor;
		DWORD dwSelBackColor;
		DWORD dwCaretColor;
		float fLineSpace;

		void Load_01( CStream &stream )
		{
			stream >> dwTextColor;
			stream >> dwSelTextColor;
			stream >> dwSelBackColor;
			stream >> dwCaretColor;
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUILineEditBoxInfo_01::VERSION: Load_01( stream ); break;
			case VERSION:
				{
					stream >> dwTextColor;
					stream >> dwSelTextColor;
					stream >> dwSelBackColor;
					stream >> dwCaretColor;
					stream >> fLineSpace;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUILineEditBoxInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << dwTextColor;
			stream << dwSelTextColor;
			stream << dwSelBackColor;
			stream << dwCaretColor;
			stream << fLineSpace;
		}
	};

	struct SUILineIMEEditBoxInfo
	{
		enum { VERSION = 0x102 };

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
		float fLineSpace;

		void Load_01( CStream &stream )
		{
			stream >> dwTextColor;
			stream >> dwSelTextColor;
			stream >> dwSelBackColor;
			stream >> dwCaretColor;
			stream >> dwCompTextColor;
			stream >> dwCompBackColor;
			stream >> dwCompCaretColor;
			stream >> dwCandiTextColor;
			stream >> dwCandiBackColor;
			stream >> dwCandiSelTextColor;
			stream >> dwCandiSelBackColor;
			stream >> dwReadingTextColor;
			stream >> dwReadingBackColor;
			stream >> dwReadingSelTextColor;
			stream >> dwReadingSelBackColor;
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUILineIMEEditBoxInfo_01::VERSION: Load_01( stream ); break;
			case VERSION:
				{
					stream >> dwTextColor;
					stream >> dwSelTextColor;
					stream >> dwSelBackColor;
					stream >> dwCaretColor;
					stream >> dwCompTextColor;
					stream >> dwCompBackColor;
					stream >> dwCompCaretColor;
					stream >> dwCandiTextColor;
					stream >> dwCandiBackColor;
					stream >> dwCandiSelTextColor;
					stream >> dwCandiSelBackColor;
					stream >> dwReadingTextColor;
					stream >> dwReadingBackColor;
					stream >> dwReadingSelTextColor;
					stream >> dwReadingSelBackColor;
					stream >> fLineSpace;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUILineIMEEditBoxInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << dwTextColor;
			stream << dwSelTextColor;
			stream << dwSelBackColor;
			stream << dwCaretColor;
			stream << dwCompTextColor;
			stream << dwCompBackColor;
			stream << dwCompCaretColor;
			stream << dwCandiTextColor;
			stream << dwCandiBackColor;
			stream << dwCandiSelTextColor;
			stream << dwCandiSelBackColor;
			stream << dwReadingTextColor;
			stream << dwReadingBackColor;
			stream << dwReadingSelTextColor;
			stream << dwReadingSelBackColor;
			stream << fLineSpace;
		}
	};

	struct SUIListBoxExInfo
	{
		enum { VERSION = 0x102 };

		float fScrollBarSize;
		float fBorder;
		float fMargin;
		float fLineSpace;
		int nSelectFrameLeft;
		int nSelectFrameTop;
		int nSelectFrameRight;
		int nSelectFrameBottom;

		void Load_01( CStream &stream )
		{
			stream >> fScrollBarSize;
			stream >> fBorder;
			stream >> fMargin;
			stream >> fLineSpace;
		}

		void Load( CStream &stream )
		{
			DWORD dwVersion;
			stream >> dwVersion;

			switch( dwVersion )
			{
			case SUIListBoxExInfo_01::VERSION: Load_01( stream ); break;
			case VERSION:
				{
					stream >> fScrollBarSize;
					stream >> fBorder;
					stream >> fMargin;
					stream >> fLineSpace;
					stream >> nSelectFrameLeft;
					stream >> nSelectFrameTop;
					stream >> nSelectFrameRight;
					stream >> nSelectFrameBottom;
				}
				break;
			default:
				CDebugSet::ToLogFile( "SUIListBoxExInfo::Load, Invalid Version(%d)!", dwVersion );
				break;
			}
		}

		void Save( CStream &stream )
		{
			stream << VERSION;
			stream << fScrollBarSize;
			stream << fBorder;
			stream << fMargin;
			stream << fLineSpace;
			stream << nSelectFrameLeft;
			stream << nSelectFrameTop;
			stream << nSelectFrameRight;
			stream << nSelectFrameBottom;
		}
	};

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
		//SUICustomInfo ProgressBarInfo;
		SUITextBoxInfo TextBoxInfo;
		SUIHtmlTextBoxInfo HtmlTextBoxInfo;
		SUITreeInfo TreeInfo;
		SUIScrollBarInfo ScrollBarInfo;
		SUIQuestTreeInfo QuestTreeInfo;
		SUIAnimationInfo AnimationInfo;
		SUILineEditBoxInfo LineEditBoxInfo;
		SUILineIMEEditBoxInfo LineIMEEditBoxInfo;
		SUIListBoxExInfo ListBoxExInfo;
	};

	SUIControlInfo()
	{
		SecureZeroMemory( this, sizeof(SUIControlInfo) );
	}

public:
	bool Load_01( CStream &stream, UI_CONTROL_TYPE uiControlType );
	bool Load_02( CStream &stream, UI_CONTROL_TYPE uiControlType );
	bool Load_03( CStream &stream, UI_CONTROL_TYPE uiControlType );
	bool Load_04( CStream &stream, UI_CONTROL_TYPE uiControlType );
	bool Load_05( CStream &stream, UI_CONTROL_TYPE uiControlType );
	bool Load_06( CStream &stream, UI_CONTROL_TYPE uiControlType );

	bool Load( CStream &stream, UI_CONTROL_TYPE uiControlType );
	bool Save( CStream &stream, UI_CONTROL_TYPE uiControlType );

public:
	void CopyControlInfo( SUIControlProperty *pProperty );
};