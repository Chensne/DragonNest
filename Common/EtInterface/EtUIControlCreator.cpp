#include "StdAfx.h"
#include "EtUIControlCreator.h"
#include "DebugSet.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUIControlCreator::CEtUIControlCreator(void)
{
}

CEtUIControlCreator::~CEtUIControlCreator(void)
{
}

CEtUIControl* CEtUIControlCreator::CreateControl( UI_CONTROL_TYPE Type, CEtUIDialog *pParent )
{
	CEtUIControl *pControl(NULL);

	switch( Type )
	{
	case UI_CONTROL_STATIC:				pControl = new CEtUIStatic( pParent );			break;
	case UI_CONTROL_BUTTON:				pControl = new CEtUIButton( pParent );			break;
	case UI_CONTROL_CHECKBOX:			pControl = new CEtUICheckBox( pParent );		break;
	case UI_CONTROL_RADIOBUTTON:		pControl = new CEtUIRadioButton( pParent );		break;
	case UI_CONTROL_COMBOBOX:			pControl = new CEtUIComboBox( pParent );		break;
	case UI_CONTROL_SLIDER:				pControl = new CEtUISlider( pParent );			break;
	case UI_CONTROL_EDITBOX:			pControl = new CEtUIEditBox( pParent );			break;
	case UI_CONTROL_IMEEDITBOX:			pControl = new CEtUIIMEEditBox( pParent );		break;
	case UI_CONTROL_LISTBOX:			pControl = new CEtUIListBox( pParent );			break;
	case UI_CONTROL_SCROLLBAR:			pControl = new CEtUIScrollBar( pParent );		break;
	case UI_CONTROL_PROGRESSBAR:		pControl = new CEtUIProgressBar( pParent );		break;
	case UI_CONTROL_TEXTBOX:			pControl = new CEtUITextBox( pParent );			break;
	case UI_CONTROL_HTMLTEXTBOX:		pControl = new CEtUIHtmlTextBox( pParent );		break;
	case UI_CONTROL_TEXTURECONTROL:		pControl = new CEtUITextureControl( pParent );	break;
	case UI_CONTROL_TREECONTROL:		pControl = new CEtUITreeCtl( pParent );			break;
	case UI_CONTROL_CUSTOM:				pControl = new CEtUIControl( pParent );			break;
	case UI_CONTROL_QUESTTREECONTROL:	pControl = new CEtUIQuestTreeCtl( pParent );	break;
	case UI_CONTROL_ANIMATION:			pControl = new CEtUIAnimation( pParent );		break;
	case UI_CONTROL_LINE_EDITBOX:		pControl = new CEtUILineEditBox( pParent );		break;
	case UI_CONTROL_LINE_IMEEDITBOX:	pControl = new CEtUILineIMEEditBox( pParent );	break;
	case UI_CONTROL_MOVIECONTROL:		pControl = new CEtUIMovieControl( pParent );	break;
	case UI_CONTROL_LISTBOXEX:			pControl = new CEtUIListBoxEx( pParent );		break;
	default:
		ASSERT( 0 && "CEtUIControlCreator::CreateControl, Invalid UI Code!!!" );
		CDebugSet::ToLogFile( "CEtUIControlCreator::CreateControl, %d Invalid control type!", Type );
		pControl = NULL;
		break;
	}

	return pControl;
}
