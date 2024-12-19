#include "StdAfx.h"
#include "EtUIControlProperty.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


bool SUIControlProperty::Load_01( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;

	switch( UIType )
	{
	case UI_CONTROL_SCROLLBAR:
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_MOVIECONTROL:										break;
	case UI_CONTROL_STATIC:			StaticProperty.Load(stream);		break;
	case UI_CONTROL_SLIDER:			SliderProperty.Load(stream);		break;
	case UI_CONTROL_TEXTURECONTROL:	TextureControlProperty.Load(stream);break;
	case UI_CONTROL_BUTTON:			ButtonProperty.Load(stream);		break;
	case UI_CONTROL_CHECKBOX:		CheckBoxProperty.Load(stream);		break;
	case UI_CONTROL_RADIOBUTTON:	RadioButtonProperty.Load(stream);	break;
	case UI_CONTROL_EDITBOX:		EditBoxProperty.Load(stream);		break;
	case UI_CONTROL_IMEEDITBOX:		IMEEditBoxProperty.Load(stream);	break;
	case UI_CONTROL_COMBOBOX:		ComboBoxProperty.Load(stream);		break;
	case UI_CONTROL_LISTBOX:		ListBoxProperty.Load(stream);		break;
	case UI_CONTROL_PROGRESSBAR:	ProgressBarProperty.Load(stream);	break;
	case UI_CONTROL_TEXTBOX:		TextBoxProperty.Load(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:	HtmlTextBoxProperty.Load(stream);	break;
	case UI_CONTROL_TREECONTROL:	TreeControlProperty.Load(stream);	break;
	default: 
		ASSERT(0&&"SUIControlProperty::Load_01");
		CDebugSet::ToLogFile( "SUIControlProperty::Load_01, Invalid Control Type!" );
		return false;
	}

	return true;
}

bool SUIControlProperty::Load_02( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;

	switch( UIType )
	{
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_MOVIECONTROL:										break;
	case UI_CONTROL_STATIC:			StaticProperty.Load(stream);		break;
	case UI_CONTROL_SLIDER:			SliderProperty.Load(stream);		break;
	case UI_CONTROL_TEXTURECONTROL:	TextureControlProperty.Load(stream);break;
	case UI_CONTROL_BUTTON:			ButtonProperty.Load(stream);		break;
	case UI_CONTROL_CHECKBOX:		CheckBoxProperty.Load(stream);		break;
	case UI_CONTROL_RADIOBUTTON:	RadioButtonProperty.Load(stream);	break;
	case UI_CONTROL_EDITBOX:		EditBoxProperty.Load(stream);		break;
	case UI_CONTROL_IMEEDITBOX:		IMEEditBoxProperty.Load(stream);	break;
	case UI_CONTROL_COMBOBOX:		ComboBoxProperty.Load(stream);		break;
	case UI_CONTROL_LISTBOX:		ListBoxProperty.Load(stream);		break;
	case UI_CONTROL_PROGRESSBAR:	ProgressBarProperty.Load(stream);	break;
	case UI_CONTROL_TEXTBOX:		TextBoxProperty.Load(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:	HtmlTextBoxProperty.Load(stream);	break;
	case UI_CONTROL_TREECONTROL:	TreeControlProperty.Load(stream);	break;
	case UI_CONTROL_SCROLLBAR:		ScrollBarProperty.Load(stream);		break;
	default: 
		ASSERT(0&&"SUIControlProperty::Load_02");
		CDebugSet::ToLogFile( "SUIControlProperty::Load_02, Invalid Control Type!" );
		return false;
	}

	return true;
}

bool SUIControlProperty::Load_03( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;
	stream >> nTooltipStringIndex;

	switch( UIType )
	{
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_MOVIECONTROL:										break;
	case UI_CONTROL_STATIC:			StaticProperty.Load(stream);		break;
	case UI_CONTROL_SLIDER:			SliderProperty.Load(stream);		break;
	case UI_CONTROL_TEXTURECONTROL:	TextureControlProperty.Load(stream);break;
	case UI_CONTROL_BUTTON:			ButtonProperty.Load(stream);		break;
	case UI_CONTROL_CHECKBOX:		CheckBoxProperty.Load(stream);		break;
	case UI_CONTROL_RADIOBUTTON:	RadioButtonProperty.Load(stream);	break;
	case UI_CONTROL_EDITBOX:		EditBoxProperty.Load(stream);		break;
	case UI_CONTROL_IMEEDITBOX:		IMEEditBoxProperty.Load(stream);	break;
	case UI_CONTROL_COMBOBOX:		ComboBoxProperty.Load(stream);		break;
	case UI_CONTROL_LISTBOX:		ListBoxProperty.Load(stream);		break;
	case UI_CONTROL_PROGRESSBAR:	ProgressBarProperty.Load(stream);	break;
	case UI_CONTROL_TEXTBOX:		TextBoxProperty.Load(stream);		break;
	case UI_CONTROL_HTMLTEXTBOX:	HtmlTextBoxProperty.Load(stream);	break;
	case UI_CONTROL_TREECONTROL:	TreeControlProperty.Load(stream);	break;
	case UI_CONTROL_SCROLLBAR:		ScrollBarProperty.Load(stream);		break;
	default: 
		ASSERT(0&&"SUIControlProperty::Load_03");
		CDebugSet::ToLogFile( "SUIControlProperty::Load_03, Invalid Control Type!" );
		return false;
	}

	return true;
}

bool SUIControlProperty::Load_04( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;
	stream >> nTooltipStringIndex;

	switch( UIType )
	{
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_MOVIECONTROL:												break;
	case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
	case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
	case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
	case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
	case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
	case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
	case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
	case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
	case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
	default: 
		ASSERT(0&&"SUIControlProperty::Load_04");
		CDebugSet::ToLogFile( "SUIControlProperty::Load_04, Invalid Control Type!" );
		return false;
	}

	return true;
}

bool SUIControlProperty::Load_05( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;
	stream >> nTooltipStringIndex;

	switch( UIType )
	{
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_MOVIECONTROL:												break;
	case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
	case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
	case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
	case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
	case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
	case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
	case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
	case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
	case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
	case UI_CONTROL_ANIMATION:			AnimationProperty.Load(stream);			break;
	default: 
		ASSERT(0&&"SUIControlProperty::Load_05");
		CDebugSet::ToLogFile( "SUIControlProperty::Load, Invalid Control Type!" );
		return false;
	}

	return true;
}

bool SUIControlProperty::Load_06( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;
	stream >> nTooltipStringIndex;

	switch( UIType )
	{
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_MOVIECONTROL:												break;
	case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
	case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
	case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
	case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
	case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
	case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
	case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
	case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
	case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
	case UI_CONTROL_ANIMATION:			AnimationProperty.Load(stream);			break;
	default:
		ASSERT(0&&"SUIControlProperty::Load_06");
		CDebugSet::ToLogFile( "SUIControlProperty::Load_06, Invalid Control Type!" );
		return false;
	}

	return true;
}

bool SUIControlProperty::Load_07( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;
	stream >> nTooltipStringIndex;

	switch( UIType )
	{
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_MOVIECONTROL:												break;
	case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
	case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
	case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
	case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
	case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
	case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
	case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
	case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
	case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
	case UI_CONTROL_ANIMATION:			AnimationProperty.Load(stream);			break;
	case UI_CONTROL_LINE_EDITBOX:		LineEditBoxProperty.Load(stream);		break;
	default:
		ASSERT(0&&"SUIControlProperty::Load_07");
		CDebugSet::ToLogFile( "SUIControlProperty::Load_07, Invalid Control Type!" );
		return false;
	}

	return true;
}

bool SUIControlProperty::Load_08( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;
	stream >> nTooltipStringIndex;

	switch( UIType )
	{
	case UI_CONTROL_CUSTOM:
	case UI_CONTROL_MOVIECONTROL:												break;
	case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
	case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
	case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
	case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
	case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
	case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
	case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
	case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
	case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
	case UI_CONTROL_ANIMATION:			AnimationProperty.Load(stream);			break;
	case UI_CONTROL_LINE_EDITBOX:		LineEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_LINE_IMEEDITBOX:	LineIMEEditBoxProperty.Load(stream);	break;
	default:
		ASSERT(0&&"SUIControlProperty::Load_08");
		CDebugSet::ToLogFile( "SUIControlProperty::Load, Invalid Control Type!" );
		return false;
	}

	return true;
}

bool SUIControlProperty::Load_09( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;
	stream >> nTooltipStringIndex;

	switch( UIType )
	{
	case UI_CONTROL_MOVIECONTROL:												break;
	case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
	case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
	case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
	case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
	case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
	case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
	case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
	case UI_CONTROL_CUSTOM:				CustomProperty.Load(stream);			break;
	case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
	case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
	case UI_CONTROL_ANIMATION:			AnimationProperty.Load(stream);			break;
	case UI_CONTROL_LINE_EDITBOX:		LineEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_LINE_IMEEDITBOX:	LineIMEEditBoxProperty.Load(stream);	break;
	case UI_CONTROL_LISTBOXEX:			ListBoxExProperty.Load(stream);			break;
	default:
		ASSERT(0&&"SUIControlProperty::Load_09");
		CDebugSet::ToLogFile( "SUIControlProperty::Load, Invalid Control Type!" );
		return false;
	}

	return true;
}

bool SUIControlProperty::Load_10( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;
	stream >> nTooltipStringIndex;
	stream >> nInitState;

	switch( UIType )
	{
	case UI_CONTROL_MOVIECONTROL:												break;
	case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
	case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
	case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
	case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
	case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
	case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
	case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
	case UI_CONTROL_CUSTOM:				CustomProperty.Load(stream);			break;
	case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
	case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
	case UI_CONTROL_ANIMATION:			AnimationProperty.Load(stream);			break;
	case UI_CONTROL_LINE_EDITBOX:		LineEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_LINE_IMEEDITBOX:	LineIMEEditBoxProperty.Load(stream);	break;
	case UI_CONTROL_LISTBOXEX:			ListBoxExProperty.Load(stream);			break;
	default:
		ASSERT(0&&"SUIControlProperty::Load");
		CDebugSet::ToLogFile( "SUIControlProperty::Load, Invalid Control Type!" );
		return false;
	}
	return true;
}

bool SUIControlProperty::Load_11( CStream &stream )
{
	stream >> UIType;
	stream >> nID;
	stream >> nHotKey;
	stream >> nTemplateIndex;
	stream >> bDefaultControl;
	stream >> UICoord;
	stream.ReadBuffer( szUIName, sizeof(szUIName) );
	stream >> nCustomControlID;
	stream >> nTooltipStringIndex;
	stream >> nInitState;

	switch( UIType )
	{
	case UI_CONTROL_MOVIECONTROL:		MovieControlPropery.Load(stream);		break;
	case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
	case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
	case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
	case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
	case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
	case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
	case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
	case UI_CONTROL_CUSTOM:				CustomProperty.Load(stream);			break;
	case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
	case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
	case UI_CONTROL_ANIMATION:			AnimationProperty.Load(stream);			break;
	case UI_CONTROL_LINE_EDITBOX:		LineEditBoxProperty.Load(stream);		break;
	case UI_CONTROL_LINE_IMEEDITBOX:	LineIMEEditBoxProperty.Load(stream);	break;
	case UI_CONTROL_LISTBOXEX:			ListBoxExProperty.Load(stream);			break;
	default:
		ASSERT(0&&"SUIControlProperty::Load");
		CDebugSet::ToLogFile( "SUIControlProperty::Load, Invalid Control Type!" );
		return false;
	}
	return true;
}

bool SUIControlProperty::Load_12( CStream &stream )
{
		stream >> UIType;
		stream >> nID;
		stream >> nHotKey;
		stream >> nTemplateIndex;
		stream >> bDefaultControl;
		stream >> UICoord;
		stream.ReadBuffer( szUIName, sizeof(szUIName) );
		stream >> nCustomControlID;
		stream >> nTooltipStringIndex;
		stream >> nInitState;

		switch( UIType )
		{
		case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
		case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
		case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
		case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
		case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
		case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
		case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
		case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
		case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
		case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
		case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
		case UI_CONTROL_CUSTOM:				CustomProperty.Load(stream);			break;
		case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
		case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
		case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
		case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
		case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
		case UI_CONTROL_ANIMATION:			AnimationProperty.Load(stream);			break;
		case UI_CONTROL_LINE_EDITBOX:		LineEditBoxProperty.Load(stream);		break;
		case UI_CONTROL_LINE_IMEEDITBOX:	LineIMEEditBoxProperty.Load(stream);	break;
		case UI_CONTROL_LISTBOXEX:			ListBoxExProperty.Load(stream);			break;
		case UI_CONTROL_MOVIECONTROL:		MovieControlPropery.Load(stream);		break;
		default:
			ASSERT(0&&"SUIControlProperty::Load12");
			CDebugSet::ToLogFile( "SUIControlProperty::Load12, Invalid Control Type! file: %s",stream.GetName());
			return false;
		}


	return true;
}

bool SUIControlProperty::Load( CStream &stream )
{
    auto n = stream.GetName();
    if (!strcmp(n, "d:\\projects\\cpp\\dragonnest\\build\\client\\resource\\ui\\charcreate\\charselectdlg.ui"))
    {
        //__debugbreak();
    }


	DWORD dwVersion;
	stream >> dwVersion;

	switch( dwVersion )
	{
	case SUIControlProperty_01::VERSION: Load_01(stream); break;
	case SUIControlProperty_02::VERSION: Load_02(stream); break;
	case SUIControlProperty_03::VERSION: Load_03(stream); break;
	case SUIControlProperty_04::VERSION: Load_04(stream); break;
	case SUIControlProperty_05::VERSION: Load_05(stream); break;
	case SUIControlProperty_06::VERSION: Load_06(stream); break;
	case SUIControlProperty_07::VERSION: Load_07(stream); break;
	case SUIControlProperty_08::VERSION: Load_08(stream); break;
	case SUIControlProperty_09::VERSION: Load_09(stream); break;
	case SUIControlProperty_10::VERSION: Load_10(stream); break;
	case SUIControlProperty_11::VERSION: Load_11(stream); break;
	case SUIControlProperty_12::VERSION: Load_12(stream); break;
	case VERSION:
		{
			stream >> UIType;
			stream >> nID;
			stream >> nHotKey;
			stream >> nTemplateIndex;
			stream >> bDefaultControl;
			stream >> UICoord;
			stream.ReadBuffer( szUIName, sizeof(szUIName) );
			stream >> nCustomControlID;
			stream >> nTooltipStringIndex;
			stream >> nInitState;

			switch( UIType )
			{
			case UI_CONTROL_STATIC:				StaticProperty.Load(stream);			break;
			case UI_CONTROL_SLIDER:				SliderProperty.Load(stream);			break;
			case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Load(stream);	break;
			case UI_CONTROL_BUTTON:				ButtonProperty.Load(stream);			break;
			case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Load(stream);			break;
			case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Load(stream);		break;
			case UI_CONTROL_EDITBOX:			EditBoxProperty.Load(stream);			break;
			case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Load(stream);		break;
			case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Load(stream);			break;
			case UI_CONTROL_LISTBOX:			ListBoxProperty.Load(stream);			break;
			case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Load(stream);		break;
			case UI_CONTROL_CUSTOM:				CustomProperty.Load(stream);			break;
			case UI_CONTROL_TEXTBOX:			TextBoxProperty.Load(stream);			break;
			case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Load(stream);		break;
			case UI_CONTROL_TREECONTROL:		TreeControlProperty.Load(stream);		break;
			case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Load(stream);			break;
			case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Load(stream);	break;
			case UI_CONTROL_ANIMATION:			AnimationProperty.Load(stream);			break;
			case UI_CONTROL_LINE_EDITBOX:		LineEditBoxProperty.Load(stream);		break;
			case UI_CONTROL_LINE_IMEEDITBOX:	LineIMEEditBoxProperty.Load(stream);	break;
			case UI_CONTROL_LISTBOXEX:			ListBoxExProperty.Load(stream);			break;
			case UI_CONTROL_MOVIECONTROL:		MovieControlPropery.Load(stream);		break;
			default:
				ASSERT(0&&"SUIControlProperty::Load");
				CDebugSet::ToLogFile( "SUIControlProperty::Load, Invalid Control Type! file: %s pos: %X UIType: %d",stream.GetName(),stream.Tell(),UIType);
				return false;
			}
		}
		break;
	default: 
		ASSERT(0&&"SUIControlProperty::Load");
		CDebugSet::ToLogFile( "SUIControlProperty::Load, Invalid Control Property Version!  %s pos: %X UIType: %d",stream.GetName(),stream.Tell(),UIType);
		return false;
	}

	return true;
}

bool SUIControlProperty::Save( CStream &stream )
{
	stream << VERSION;

	stream << UIType;
	stream << nID;
	stream << nHotKey;
	stream << nTemplateIndex;
	stream << bDefaultControl;
	stream << UICoord;
	stream.WriteBuffer( szUIName, sizeof(szUIName) );
	stream << nCustomControlID;
	stream << nTooltipStringIndex;
	stream << nInitState;

	switch( UIType )
	{
	case UI_CONTROL_STATIC:				StaticProperty.Save(stream);			break;
	case UI_CONTROL_SLIDER:				SliderProperty.Save(stream);			break;
	case UI_CONTROL_TEXTURECONTROL:		TextureControlProperty.Save(stream);	break;
	case UI_CONTROL_BUTTON:				ButtonProperty.Save(stream);			break;
	case UI_CONTROL_CHECKBOX:			CheckBoxProperty.Save(stream);			break;
	case UI_CONTROL_RADIOBUTTON:		RadioButtonProperty.Save(stream);		break;
	case UI_CONTROL_EDITBOX:			EditBoxProperty.Save(stream);			break;
	case UI_CONTROL_IMEEDITBOX:			IMEEditBoxProperty.Save(stream);		break;
	case UI_CONTROL_COMBOBOX:			ComboBoxProperty.Save(stream);			break;
	case UI_CONTROL_LISTBOX:			ListBoxProperty.Save(stream);			break;
	case UI_CONTROL_PROGRESSBAR:		ProgressBarProperty.Save(stream);		break;
	case UI_CONTROL_CUSTOM:				CustomProperty.Save(stream);			break;
	case UI_CONTROL_TEXTBOX:			TextBoxProperty.Save(stream);			break;
	case UI_CONTROL_HTMLTEXTBOX:		HtmlTextBoxProperty.Save(stream);		break;
	case UI_CONTROL_TREECONTROL:		TreeControlProperty.Save(stream);		break;
	case UI_CONTROL_SCROLLBAR:			ScrollBarProperty.Save(stream);			break;
	case UI_CONTROL_QUESTTREECONTROL:	QuestTreeControlProperty.Save(stream);	break;
	case UI_CONTROL_ANIMATION:			AnimationProperty.Save(stream);			break;
	case UI_CONTROL_LINE_EDITBOX:		LineEditBoxProperty.Save(stream);		break;
	case UI_CONTROL_LINE_IMEEDITBOX:	LineIMEEditBoxProperty.Save(stream);	break;
	case UI_CONTROL_LISTBOXEX:			ListBoxExProperty.Save(stream);			break;
	case UI_CONTROL_MOVIECONTROL:		MovieControlPropery.Save(stream);		break;
	default: 
		ASSERT(0&&"SUIControlProperty::Save, default");
		CDebugSet::ToLogFile( "SUIControlProperty::Save, Invalid Control Type!" );
		return false;
	}

	return true;
}

void SUIControlProperty::Assign( SUIControlProperty *pProperty )
{
	ASSERT( pProperty );

	UIType = pProperty->UIType;
	nID = pProperty->nID;
	nHotKey = pProperty->nHotKey;
	nTemplateIndex = pProperty->nTemplateIndex;
	bDefaultControl = pProperty->bDefaultControl;
	UICoord = pProperty->UICoord;
	strcpy_s( szUIName, 32, pProperty->szUIName );
	nCustomControlID = pProperty->nCustomControlID;
	nTooltipStringIndex = pProperty->nTooltipStringIndex;
	nInitState = pProperty->nInitState;

	switch( UIType )
	{
	case UI_CONTROL_STATIC:
		StaticProperty = pProperty->StaticProperty;
		break;
	case UI_CONTROL_SLIDER:
		SliderProperty = pProperty->SliderProperty;
		break;
	case UI_CONTROL_TEXTURECONTROL:
		TextureControlProperty = pProperty->TextureControlProperty;
		break;
	case UI_CONTROL_BUTTON:
		ButtonProperty = pProperty->ButtonProperty;
		break;
	case UI_CONTROL_CHECKBOX:
		CheckBoxProperty = pProperty->CheckBoxProperty;
		break;
	case UI_CONTROL_RADIOBUTTON:		
		RadioButtonProperty = pProperty->RadioButtonProperty;
		break;
	case UI_CONTROL_EDITBOX:			
		EditBoxProperty = pProperty->EditBoxProperty;	
		break;
	case UI_CONTROL_IMEEDITBOX:			
		IMEEditBoxProperty = pProperty->IMEEditBoxProperty;
		break;
	case UI_CONTROL_COMBOBOX:			
		ComboBoxProperty = pProperty->ComboBoxProperty;
		break;
	case UI_CONTROL_LISTBOX:			
		ListBoxProperty = pProperty->ListBoxProperty;
		break;
	case UI_CONTROL_PROGRESSBAR:		
		ProgressBarProperty = pProperty->ProgressBarProperty;
		break;
	case UI_CONTROL_CUSTOM:
		CustomProperty = pProperty->CustomProperty;
		break;
	case UI_CONTROL_TEXTBOX:			
		TextBoxProperty = pProperty->TextBoxProperty;
		break;
	case UI_CONTROL_HTMLTEXTBOX:
		{
			// Note : 상위 클래스인 TextBox에 속성을 넣어줘야 한다.
			//
			TextBoxProperty.bVerticalScrollBar = pProperty->HtmlTextBoxProperty.bVerticalScrollBar;
			TextBoxProperty.nScrollBarTemplate = pProperty->HtmlTextBoxProperty.nScrollBarTemplate;
			TextBoxProperty.fScrollBarSize = pProperty->HtmlTextBoxProperty.fScrollBarSize;
			TextBoxProperty.bLeftScrollBar = pProperty->HtmlTextBoxProperty.bLeftScrollBar;
			TextBoxProperty.fLineSpace = pProperty->HtmlTextBoxProperty.fLineSpace;
			TextBoxProperty.bRollOver = pProperty->HtmlTextBoxProperty.bRollOver;
		}
		HtmlTextBoxProperty = pProperty->HtmlTextBoxProperty;
		break;
	case UI_CONTROL_TREECONTROL:		
		TreeControlProperty = pProperty->TreeControlProperty;
		break;
	case UI_CONTROL_SCROLLBAR:			
		ScrollBarProperty = pProperty->ScrollBarProperty;
		break;
	case UI_CONTROL_QUESTTREECONTROL:	
		QuestTreeControlProperty = pProperty->QuestTreeControlProperty;
		break;
	case UI_CONTROL_ANIMATION:			
		AnimationProperty = pProperty->AnimationProperty;
		break;
	case UI_CONTROL_LINE_EDITBOX:
		LineEditBoxProperty = pProperty->LineEditBoxProperty;
		break;
	case UI_CONTROL_LINE_IMEEDITBOX:
		LineIMEEditBoxProperty = pProperty->LineIMEEditBoxProperty;
		break;
	case UI_CONTROL_MOVIECONTROL:
		MovieControlPropery = pProperty->MovieControlPropery;
		break;
	case UI_CONTROL_LISTBOXEX:
		ListBoxExProperty = pProperty->ListBoxExProperty;
		break;
	default: 
		ASSERT(0&&"SUIControlProperty::Assign, Invalid Control Type!");
		CDebugSet::ToLogFile( "SUIControlProperty::Assign, Invalid Control Type!" );
	}
}

void SUIControlProperty::SUIListBoxExProperty::Load(CStream& stream)
{
    DWORD dwVersion;
    stream >> dwVersion;

    switch (dwVersion)
    {
    case SUIListBoxExProperty_01::VERSION: Load_01(stream); break;

    case VERSION:
    {
        stream >> fScrollBarSize;
        stream >> nScrollBarTemplate;
        stream >> bMultiSelect;
        stream >> fBorder;
        stream >> fMargin;
        stream >> fLineSpace;
        stream.ReadBuffer(szListBoxItemUIFileName, _countof(szListBoxItemUIFileName));
        stream >> nSelectFrameLeft;
        stream >> nSelectFrameTop;
        stream >> nSelectFrameRight;
        stream >> nSelectFrameBottom;
        break;
    }

    default:
        CDebugSet::ToLogFile("SUIListBoxExProperty::Load, Invalid Control Version(%d)!", dwVersion);
        break;
    }
}

void SUIControlProperty::SUIListBoxProperty::Load(CStream& stream)
{
    DWORD dwVersion;
    stream >> dwVersion;

    switch (dwVersion)
    {
    case SUIListBoxProperty_01::VERSION: Load_01(stream); break;

    case VERSION:
    {
        stream >> fScrollBarSize;
        stream >> nScrollBarTemplate;
        stream >> bMultiSelect;
        stream >> fBorder;
        stream >> fMargin;
        stream >> fLineSpace;
        break;
    }

    default:
        CDebugSet::ToLogFile("SUIListBoxProperty::Load, Invalid Control Version(%d)!", dwVersion);
        break;
    }
}