#include "StdAfx.h"
#include "UIControlInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Note : TemplateProperty에서 설정한 속성값들을 복사한다.
//
void SUIControlInfo::CopyControlInfo( SUIControlProperty *pProperty )
{
	if( pProperty == NULL )
	{
		CDebugSet::ToLogFile( "SUIControlInfo::CopyControlInfo, pProperty is NULL!" );
		return;
	}

	switch( pProperty->UIType )
	{
	case UI_CONTROL_STATIC:
		pProperty->StaticProperty.nFrameLeft = StaticInfo.nFrameLeft;
		pProperty->StaticProperty.nFrameTop = StaticInfo.nFrameTop;
		pProperty->StaticProperty.nFrameRight = StaticInfo.nFrameRight;
		pProperty->StaticProperty.nFrameBottom = StaticInfo.nFrameBottom;
		break;
	case UI_CONTROL_BUTTON:
		pProperty->ButtonProperty.fMoveOffsetHori = ButtonInfo.fMoveOffsetHori;
		pProperty->ButtonProperty.fMoveOffsetVert = ButtonInfo.fMoveOffsetVert;
		pProperty->ButtonProperty.ButtonType = ButtonInfo.ButtonType;
		pProperty->ButtonProperty.bVariableWidth = ButtonInfo.bVariableWidth;
		pProperty->ButtonProperty.fVariableWidthLength = ButtonInfo.fVariableWidthLenth;
		break;
	case UI_CONTROL_CHECKBOX:
		pProperty->CheckBoxProperty.fSpace = CheckBoxInfo.fSpace;
		pProperty->CheckBoxProperty.nAllign = CheckBoxInfo.nAllign;
		break;
	case UI_CONTROL_RADIOBUTTON:
		pProperty->RadioButtonProperty.fSpace = RadioButtonInfo.fSpace;
		pProperty->RadioButtonProperty.nAllign = RadioButtonInfo.nAllign;
		pProperty->RadioButtonProperty.nIconIndex = RadioButtonInfo.nIconIndex;
		pProperty->RadioButtonProperty.fMoveOffsetHori = RadioButtonInfo.fMoveOffsetHori;
		pProperty->RadioButtonProperty.fMoveOffsetVert = RadioButtonInfo.fMoveOffsetVert;
		pProperty->RadioButtonProperty.fVariableWidthLength = RadioButtonInfo.fVariableWidthLength;
		break;
	case UI_CONTROL_COMBOBOX:
		pProperty->ComboBoxProperty.fMoveOffsetHori = ComboBoxInfo.fMoveOffsetHori;
		pProperty->ComboBoxProperty.fMoveOffsetVert = ComboBoxInfo.fMoveOffsetVert;
		pProperty->ComboBoxProperty.fDropdownSize = ComboBoxInfo.fDropdownSize;
		pProperty->ComboBoxProperty.fScrollBarSize = ComboBoxInfo.fScrollBarSize;
		pProperty->ComboBoxProperty.fVariableWidthLength = ComboBoxInfo.fVariableWidthLenth;
		break;
	case UI_CONTROL_EDITBOX:
		pProperty->EditBoxProperty.fSpace = EditBoxInfo.fSpace;
		pProperty->EditBoxProperty.fBorder = EditBoxInfo.fBorder;
		pProperty->EditBoxProperty.dwTextColor = EditBoxInfo.dwTextColor;
		pProperty->EditBoxProperty.dwSelTextColor = EditBoxInfo.dwSelTextColor;
		pProperty->EditBoxProperty.dwSelBackColor = EditBoxInfo.dwSelBackColor;
		pProperty->EditBoxProperty.dwCaretColor = EditBoxInfo.dwCaretColor;
		break;
	case UI_CONTROL_IMEEDITBOX:
		pProperty->IMEEditBoxProperty.fSpace = IMEEditBoxInfo.fSpace;
		pProperty->IMEEditBoxProperty.fBorder = IMEEditBoxInfo.fBorder;
		pProperty->IMEEditBoxProperty.dwTextColor = IMEEditBoxInfo.dwTextColor;
		pProperty->IMEEditBoxProperty.dwSelTextColor = IMEEditBoxInfo.dwSelTextColor;
		pProperty->IMEEditBoxProperty.dwSelBackColor = IMEEditBoxInfo.dwSelBackColor;
		pProperty->IMEEditBoxProperty.dwCaretColor = IMEEditBoxInfo.dwCaretColor;
		pProperty->IMEEditBoxProperty.dwCompTextColor = IMEEditBoxInfo.dwCompTextColor;
		pProperty->IMEEditBoxProperty.dwCompBackColor = IMEEditBoxInfo.dwCompBackColor;
		pProperty->IMEEditBoxProperty.dwCompCaretColor = IMEEditBoxInfo.dwCompCaretColor;
		pProperty->IMEEditBoxProperty.dwCandiTextColor = IMEEditBoxInfo.dwCandiTextColor;
		pProperty->IMEEditBoxProperty.dwCandiBackColor = IMEEditBoxInfo.dwCandiBackColor;
		pProperty->IMEEditBoxProperty.dwCandiSelTextColor = IMEEditBoxInfo.dwCandiSelTextColor;
		pProperty->IMEEditBoxProperty.dwCandiSelBackColor = IMEEditBoxInfo.dwCandiSelBackColor;
		pProperty->IMEEditBoxProperty.dwIndiTextColor = IMEEditBoxInfo.dwIndiTextColor;
		pProperty->IMEEditBoxProperty.dwReadingTextColor = IMEEditBoxInfo.dwReadingTextColor;
		pProperty->IMEEditBoxProperty.dwReadingBackColor = IMEEditBoxInfo.dwReadingBackColor;
		pProperty->IMEEditBoxProperty.dwReadingSelTextColor = IMEEditBoxInfo.dwReadingSelTextColor;
		pProperty->IMEEditBoxProperty.dwReadingSelBackColor = IMEEditBoxInfo.dwReadingSelBackColor;
		break;
	case UI_CONTROL_LISTBOX:
		pProperty->ListBoxProperty.fScrollBarSize = ListBoxInfo.fScrollBarSize;
		pProperty->ListBoxProperty.fBorder = ListBoxInfo.fBorder;
		pProperty->ListBoxProperty.fMargin = ListBoxInfo.fMargin;
		pProperty->ListBoxProperty.fLineSpace = ListBoxInfo.fLineSpace;
		break;
	case UI_CONTROL_PROGRESSBAR:
		pProperty->ProgressBarProperty.fHoriBorder = ProgressBarInfo.fHoriBorder;
		pProperty->ProgressBarProperty.fVertBorder = ProgressBarInfo.fVertBorder;
		break;
	case UI_CONTROL_TEXTBOX:
		pProperty->TextBoxProperty.fScrollBarSize = TextBoxInfo.fScrollBarSize;
		break;
	case UI_CONTROL_HTMLTEXTBOX:
		pProperty->HtmlTextBoxProperty.fScrollBarSize = HtmlTextBoxInfo.fScrollBarSize;
		break;
	case UI_CONTROL_TREECONTROL:
		pProperty->TreeControlProperty.fScrollBarSize = TreeInfo.fScrollBarSize;
		pProperty->TreeControlProperty.fLineSpace = TreeInfo.fLineSpace;
		pProperty->TreeControlProperty.fIndentSize = TreeInfo.fIndentSize;
		break;
	case UI_CONTROL_SCROLLBAR:
		pProperty->ScrollBarProperty.fThumbVariableLenth = ScrollBarInfo.fThumbVariableLenth;
		break;
	case UI_CONTROL_QUESTTREECONTROL:
		pProperty->QuestTreeControlProperty.fScrollBarSize = QuestTreeInfo.fScrollBarSize;
		pProperty->QuestTreeControlProperty.fLineSpace = QuestTreeInfo.fLineSpace;
		pProperty->QuestTreeControlProperty.fIndentSize = QuestTreeInfo.fIndentSize;
		break;
	case UI_CONTROL_ANIMATION:
		pProperty->AnimationProperty.bLoop = AnimationInfo.bLoop;
#ifdef _ANIMATION_PLAY_CONTROL
		pProperty->AnimationProperty.PlayTimeType = AnimationInfo.PlayTimeType;
		pProperty->AnimationProperty.timePerAction = AnimationInfo.timePerAction;
#endif
		break;
	case UI_CONTROL_LINE_EDITBOX:
		pProperty->LineEditBoxProperty.dwTextColor = LineEditBoxInfo.dwTextColor;
		pProperty->LineEditBoxProperty.dwSelTextColor = LineEditBoxInfo.dwSelTextColor;
		pProperty->LineEditBoxProperty.dwSelBackColor = LineEditBoxInfo.dwSelBackColor;
		pProperty->LineEditBoxProperty.dwCaretColor = LineEditBoxInfo.dwCaretColor;
		pProperty->LineEditBoxProperty.fLineSpace = LineEditBoxInfo.fLineSpace;
		break;
	case UI_CONTROL_LINE_IMEEDITBOX:
		pProperty->LineIMEEditBoxProperty.dwTextColor = LineIMEEditBoxInfo.dwTextColor;
		pProperty->LineIMEEditBoxProperty.dwSelTextColor = LineIMEEditBoxInfo.dwSelTextColor;
		pProperty->LineIMEEditBoxProperty.dwSelBackColor = LineIMEEditBoxInfo.dwSelBackColor;
		pProperty->LineIMEEditBoxProperty.dwCaretColor = LineIMEEditBoxInfo.dwCaretColor;
		pProperty->LineIMEEditBoxProperty.dwCompTextColor = LineIMEEditBoxInfo.dwCompTextColor;
		pProperty->LineIMEEditBoxProperty.dwCompBackColor = LineIMEEditBoxInfo.dwCompBackColor;
		pProperty->LineIMEEditBoxProperty.dwCompCaretColor = LineIMEEditBoxInfo.dwCompCaretColor;
		pProperty->LineIMEEditBoxProperty.dwCandiTextColor = LineIMEEditBoxInfo.dwCandiTextColor;
		pProperty->LineIMEEditBoxProperty.dwCandiBackColor = LineIMEEditBoxInfo.dwCandiBackColor;
		pProperty->LineIMEEditBoxProperty.dwCandiSelTextColor = LineIMEEditBoxInfo.dwCandiSelTextColor;
		pProperty->LineIMEEditBoxProperty.dwCandiSelBackColor = LineIMEEditBoxInfo.dwCandiSelBackColor;
		pProperty->LineIMEEditBoxProperty.dwReadingTextColor = LineIMEEditBoxInfo.dwReadingTextColor;
		pProperty->LineIMEEditBoxProperty.dwReadingBackColor = LineIMEEditBoxInfo.dwReadingBackColor;
		pProperty->LineIMEEditBoxProperty.dwReadingSelTextColor = LineIMEEditBoxInfo.dwReadingSelTextColor;
		pProperty->LineIMEEditBoxProperty.dwReadingSelBackColor = LineIMEEditBoxInfo.dwReadingSelBackColor;
		pProperty->LineIMEEditBoxProperty.fLineSpace = LineIMEEditBoxInfo.fLineSpace;
		break;
	case UI_CONTROL_SLIDER:
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_TEXTURECONTROL:
	case UI_CONTROL_MOVIECONTROL:
		break;
	case UI_CONTROL_LISTBOXEX:
		pProperty->ListBoxExProperty.fScrollBarSize = ListBoxExInfo.fScrollBarSize;
		pProperty->ListBoxExProperty.fBorder = ListBoxExInfo.fBorder;
		pProperty->ListBoxExProperty.fMargin = ListBoxExInfo.fMargin;
		pProperty->ListBoxExProperty.fLineSpace = ListBoxExInfo.fLineSpace;
		pProperty->ListBoxExProperty.nSelectFrameLeft = ListBoxExInfo.nSelectFrameLeft;
		pProperty->ListBoxExProperty.nSelectFrameTop = ListBoxExInfo.nSelectFrameTop;
		pProperty->ListBoxExProperty.nSelectFrameRight = ListBoxExInfo.nSelectFrameRight;
		pProperty->ListBoxExProperty.nSelectFrameBottom = ListBoxExInfo.nSelectFrameBottom;
		break;
	default:
		CDebugSet::ToLogFile( "SUIControlInfo::CopyControlInfo, default case(%d)!", pProperty->UIType );
		break;
	}
}

bool SUIControlInfo::Load_01( CStream &stream, UI_CONTROL_TYPE uiControlType )
{
	switch( uiControlType )
	{
	case UI_CONTROL_STATIC:
	case UI_CONTROL_SLIDER:
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_SCROLLBAR:
	case UI_CONTROL_TEXTURECONTROL:
	case UI_CONTROL_MOVIECONTROL:									break;

	case UI_CONTROL_BUTTON:			ButtonInfo.Load(stream);		break;
	case UI_CONTROL_CHECKBOX:		CheckBoxInfo.Load(stream);		break;
	case UI_CONTROL_RADIOBUTTON:	RadioButtonInfo.Load(stream);	break;
	case UI_CONTROL_EDITBOX:		EditBoxInfo.Load(stream);		break;
	case UI_CONTROL_IMEEDITBOX:		IMEEditBoxInfo.Load(stream);	break;
	case UI_CONTROL_COMBOBOX:		ComboBoxInfo.Load(stream);		break;
	case UI_CONTROL_LISTBOX:		ListBoxInfo.Load(stream);		break;
	case UI_CONTROL_PROGRESSBAR:	ProgressBarInfo.Load(stream);	break;
	case UI_CONTROL_TEXTBOX:		TextBoxInfo.Load(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:	HtmlTextBoxInfo.Load(stream);	break;
	case UI_CONTROL_TREECONTROL:	TreeInfo.Load(stream);			break;
	default: 
		CDebugSet::ToLogFile( "SUIControlInfo::Load_01, Invalid Control Type(%d)!", uiControlType );
		break;
	}

	return true;
}

bool SUIControlInfo::Load_02( CStream &stream, UI_CONTROL_TYPE uiControlType )
{
	switch( uiControlType )
	{
	case UI_CONTROL_STATIC:
	case UI_CONTROL_SLIDER:
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_TEXTURECONTROL:
	case UI_CONTROL_MOVIECONTROL:									break;
	case UI_CONTROL_BUTTON:			ButtonInfo.Load(stream);		break;
	case UI_CONTROL_CHECKBOX:		CheckBoxInfo.Load(stream);		break;
	case UI_CONTROL_RADIOBUTTON:	RadioButtonInfo.Load(stream);	break;
	case UI_CONTROL_EDITBOX:		EditBoxInfo.Load(stream);		break;
	case UI_CONTROL_IMEEDITBOX:		IMEEditBoxInfo.Load(stream);	break;
	case UI_CONTROL_COMBOBOX:		ComboBoxInfo.Load(stream);		break;
	case UI_CONTROL_LISTBOX:		ListBoxInfo.Load(stream);		break;
	case UI_CONTROL_PROGRESSBAR:	ProgressBarInfo.Load(stream);	break;
	case UI_CONTROL_TEXTBOX:		TextBoxInfo.Load(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:	HtmlTextBoxInfo.Load(stream);	break;
	case UI_CONTROL_TREECONTROL:	TreeInfo.Load(stream);			break;
	case UI_CONTROL_SCROLLBAR:		ScrollBarInfo.Load(stream);		break;
	default: 
		CDebugSet::ToLogFile( "SUIControlInfo::Load_02, Invalid Control Type(%d)!", uiControlType );
		break;
	}

	return true;
}

bool SUIControlInfo::Load_03( CStream &stream, UI_CONTROL_TYPE uiControlType )
{
	switch( uiControlType )
	{
	case UI_CONTROL_SLIDER:
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_TEXTURECONTROL:
	case UI_CONTROL_MOVIECONTROL:									break;
	case UI_CONTROL_STATIC:			StaticInfo.Load(stream);		break;
	case UI_CONTROL_BUTTON:			ButtonInfo.Load(stream);		break;
	case UI_CONTROL_CHECKBOX:		CheckBoxInfo.Load(stream);		break;
	case UI_CONTROL_RADIOBUTTON:	RadioButtonInfo.Load(stream);	break;
	case UI_CONTROL_EDITBOX:		EditBoxInfo.Load(stream);		break;
	case UI_CONTROL_IMEEDITBOX:		IMEEditBoxInfo.Load(stream);	break;
	case UI_CONTROL_COMBOBOX:		ComboBoxInfo.Load(stream);		break;
	case UI_CONTROL_LISTBOX:		ListBoxInfo.Load(stream);		break;
	case UI_CONTROL_PROGRESSBAR:	ProgressBarInfo.Load(stream);	break;
	case UI_CONTROL_TEXTBOX:		TextBoxInfo.Load(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:	HtmlTextBoxInfo.Load(stream);	break;
	case UI_CONTROL_TREECONTROL:	TreeInfo.Load(stream);			break;
	case UI_CONTROL_SCROLLBAR:		ScrollBarInfo.Load(stream);		break;
	default: 
		CDebugSet::ToLogFile( "SUIControlInfo::Load_03, Invalid Control Type(%d)!", uiControlType );
		break;
	}

	return true;
}

bool SUIControlInfo::Load_04( CStream &stream, UI_CONTROL_TYPE uiControlType )
{
	switch( uiControlType )
	{
	case UI_CONTROL_SLIDER:
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_TEXTURECONTROL:
	case UI_CONTROL_MOVIECONTROL:										break;
	case UI_CONTROL_STATIC:				StaticInfo.Load(stream);		break;
	case UI_CONTROL_BUTTON:				ButtonInfo.Load(stream);		break;
	case UI_CONTROL_CHECKBOX:			CheckBoxInfo.Load(stream);		break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonInfo.Load(stream);	break;
	case UI_CONTROL_EDITBOX:			EditBoxInfo.Load(stream);		break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxInfo.Load(stream);	break;
	case UI_CONTROL_COMBOBOX:			ComboBoxInfo.Load(stream);		break;
	case UI_CONTROL_LISTBOX:			ListBoxInfo.Load(stream);		break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarInfo.Load(stream);	break;
	case UI_CONTROL_TEXTBOX:			TextBoxInfo.Load(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxInfo.Load(stream);	break;
	case UI_CONTROL_TREECONTROL:		TreeInfo.Load(stream);			break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarInfo.Load(stream);		break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeInfo.Load(stream);		break;
	default: 
		CDebugSet::ToLogFile( "SUIControlInfo::Load_04, Invalid Control Type(%d)!", uiControlType );
		break;
	}

	return true;
}

bool SUIControlInfo::Load_05( CStream &stream, UI_CONTROL_TYPE uiControlType )
{
	switch( uiControlType )
	{
	case UI_CONTROL_SLIDER:
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_TEXTURECONTROL:
	case UI_CONTROL_MOVIECONTROL:										break;
	case UI_CONTROL_STATIC:				StaticInfo.Load(stream);		break;
	case UI_CONTROL_BUTTON:				ButtonInfo.Load(stream);		break;
	case UI_CONTROL_CHECKBOX:			CheckBoxInfo.Load(stream);		break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonInfo.Load(stream);	break;
	case UI_CONTROL_EDITBOX:			EditBoxInfo.Load(stream);		break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxInfo.Load(stream);	break;
	case UI_CONTROL_COMBOBOX:			ComboBoxInfo.Load(stream);		break;
	case UI_CONTROL_LISTBOX:			ListBoxInfo.Load(stream);		break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarInfo.Load(stream);	break;
	case UI_CONTROL_TEXTBOX:			TextBoxInfo.Load(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxInfo.Load(stream);	break;
	case UI_CONTROL_TREECONTROL:		TreeInfo.Load(stream);			break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarInfo.Load(stream);		break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeInfo.Load(stream);		break;
	case UI_CONTROL_ANIMATION:			AnimationInfo.Load(stream);		break;
	default: 
		CDebugSet::ToLogFile( "SUIControlInfo::Load_05, Invalid Control Type(%d)!", uiControlType );
		break;
	}

	return true;
}

bool SUIControlInfo::Load_06( CStream &stream, UI_CONTROL_TYPE uiControlType )
{
	switch( uiControlType )
	{
	case UI_CONTROL_SLIDER:
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_TEXTURECONTROL:
	case UI_CONTROL_MOVIECONTROL:										break;
	case UI_CONTROL_STATIC:				StaticInfo.Load(stream);		break;
	case UI_CONTROL_BUTTON:				ButtonInfo.Load(stream);		break;
	case UI_CONTROL_CHECKBOX:			CheckBoxInfo.Load(stream);		break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonInfo.Load(stream);	break;
	case UI_CONTROL_EDITBOX:			EditBoxInfo.Load(stream);		break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxInfo.Load(stream);	break;
	case UI_CONTROL_COMBOBOX:			ComboBoxInfo.Load(stream);		break;
	case UI_CONTROL_LISTBOX:			ListBoxInfo.Load(stream);		break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarInfo.Load(stream);	break;
	case UI_CONTROL_TEXTBOX:			TextBoxInfo.Load(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxInfo.Load(stream);	break;
	case UI_CONTROL_TREECONTROL:		TreeInfo.Load(stream);			break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarInfo.Load(stream);		break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeInfo.Load(stream);		break;
	case UI_CONTROL_ANIMATION:			AnimationInfo.Load(stream);		break;
	case UI_CONTROL_LINE_EDITBOX:		LineEditBoxInfo.Load(stream);	break;
	default: 
		CDebugSet::ToLogFile( "SUIControlInfo::Load_06, Invalid Control Type(%d)!", uiControlType );
		break;
	}

	return true;
}

bool SUIControlInfo::Load( CStream &stream, UI_CONTROL_TYPE uiControlType )
{
	DWORD dwVersion;
	stream >> dwVersion;

	switch( dwVersion )
	{
	case SUIControlInfo_01::VERSION: Load_01( stream, uiControlType ); break;
	case SUIControlInfo_02::VERSION: Load_02( stream, uiControlType ); break;
	case SUIControlInfo_03::VERSION: Load_03( stream, uiControlType ); break;
	case SUIControlInfo_04::VERSION: Load_04( stream, uiControlType ); break;
	case SUIControlInfo_05::VERSION: Load_05( stream, uiControlType ); break;
	case SUIControlInfo_06::VERSION: Load_06( stream, uiControlType ); break;
	case VERSION:
		{
			switch( uiControlType )
			{
			case UI_CONTROL_SLIDER:
			case UI_CONTROL_CUSTOM:
			case UI_CONTROL_TEXTURECONTROL:
			case UI_CONTROL_MOVIECONTROL:										break;
			case UI_CONTROL_STATIC:				StaticInfo.Load(stream);		break;
			case UI_CONTROL_BUTTON:				ButtonInfo.Load(stream);		break;
			case UI_CONTROL_CHECKBOX:			CheckBoxInfo.Load(stream);		break;
			case UI_CONTROL_RADIOBUTTON:		RadioButtonInfo.Load(stream);	break;
			case UI_CONTROL_EDITBOX:			EditBoxInfo.Load(stream);		break;
			case UI_CONTROL_IMEEDITBOX:			IMEEditBoxInfo.Load(stream);	break;
			case UI_CONTROL_COMBOBOX:			ComboBoxInfo.Load(stream);		break;
			case UI_CONTROL_LISTBOX:			ListBoxInfo.Load(stream);		break;
			case UI_CONTROL_PROGRESSBAR:		ProgressBarInfo.Load(stream);	break;
			case UI_CONTROL_TEXTBOX:			TextBoxInfo.Load(stream);		break;
			case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxInfo.Load(stream);	break;
			case UI_CONTROL_TREECONTROL:		TreeInfo.Load(stream);			break;
			case UI_CONTROL_SCROLLBAR:			ScrollBarInfo.Load(stream);		break;
			case UI_CONTROL_QUESTTREECONTROL:	QuestTreeInfo.Load(stream);		break;
			case UI_CONTROL_ANIMATION:			AnimationInfo.Load(stream);		break;
			case UI_CONTROL_LINE_EDITBOX:		LineEditBoxInfo.Load(stream);	break;
			case UI_CONTROL_LINE_IMEEDITBOX:	LineIMEEditBoxInfo.Load(stream);break;
			case UI_CONTROL_LISTBOXEX:			ListBoxExInfo.Load(stream);		break;
			default: 
				CDebugSet::ToLogFile( "SUIControlInfo::Load, Invalid Control Type(%d)!", uiControlType );
				break;
			}
		}
		break;

	default: 
		CDebugSet::ToLogFile( "SUIControlInfo::Load, Invalid Control Version(%d)!", dwVersion );
		break;
	}

	return true;
}

bool SUIControlInfo::Save( CStream &stream, UI_CONTROL_TYPE uiControlType )
{
	stream << VERSION;

	switch( uiControlType )
	{
	case UI_CONTROL_SLIDER:
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_TEXTURECONTROL:
	case UI_CONTROL_MOVIECONTROL:										break;
	case UI_CONTROL_STATIC:				StaticInfo.Save(stream);		break;
	case UI_CONTROL_BUTTON:				ButtonInfo.Save(stream);		break;
	case UI_CONTROL_CHECKBOX:			CheckBoxInfo.Save(stream);		break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonInfo.Save(stream);	break;
	case UI_CONTROL_EDITBOX:			EditBoxInfo.Save(stream);		break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxInfo.Save(stream);	break;
	case UI_CONTROL_COMBOBOX:			ComboBoxInfo.Save(stream);		break;
	case UI_CONTROL_LISTBOX:			ListBoxInfo.Save(stream);		break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarInfo.Save(stream);	break;
	case UI_CONTROL_TEXTBOX:			TextBoxInfo.Save(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxInfo.Save(stream);	break;
	case UI_CONTROL_TREECONTROL:		TreeInfo.Save(stream);			break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarInfo.Save(stream);		break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeInfo.Save(stream);		break;
	case UI_CONTROL_ANIMATION:			AnimationInfo.Save(stream);		break;
	case UI_CONTROL_LINE_EDITBOX:		LineEditBoxInfo.Save(stream);	break;
	case UI_CONTROL_LINE_IMEEDITBOX:	LineIMEEditBoxInfo.Save(stream);break;
	case UI_CONTROL_LISTBOXEX:			ListBoxExInfo.Save(stream);		break;
	default: 
		CDebugSet::ToLogFile( "SUIControlInfo::Save, Invalid Control Type(%d)!", uiControlType );
		break;
	}

	return true;
}