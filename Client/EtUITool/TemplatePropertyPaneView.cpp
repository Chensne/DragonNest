// TemplatePropertyPaneView.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "TemplatePropertyPaneView.h"

#include "MainFrm.h"
#include "EtUIToolDoc.h"
#include "EtUIToolView.h"
#include "GlobalValue.h"
#include "PaneDefine.h"

#include "TemplateView.h"
#include "LayoutView.h"
#include "DummyView.h"
#include "UIToolTemplate.h"
#include "GenTexture.h"

#include "ScrollBarSelect.h"
#include "FontSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define UI_TYPE_STRING "UI ����|Static|Button|CheckBox|RadioButton|ComboBox|Slider|EditBox|IMEEditBox|ListBox"\
					"|ScrollBar|ProgressBar|Custom|TextBox|TextureControl|HtmlTextbox|Tree|QuestTree|Animation"\
					"|LineEditBox|LineIMEEditBox|MovieControl|ListBoxEx"

// CTemplatePropertyPaneView
PropertyGridBaseDefine g_StaticTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Frame Left", CUnionValueProperty::Integer, "�ܰ� ������ ���� ũ��", TRUE, false },
	{ "Property", "Frame Top", CUnionValueProperty::Integer, "�ܰ� ������ ���� ũ��", TRUE, false },
	{ "Property", "Frame Right", CUnionValueProperty::Integer, "�ܰ� ������ ������ ũ��", TRUE, false },
	{ "Property", "Frame Bottom", CUnionValueProperty::Integer, "�ܰ� ������ �Ʒ��� ũ��", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ButtonTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Move Offset Hori", CUnionValueProperty::Integer, "��ư �������� ��ư�� ���� ��ġ �̵�", TRUE, false },
	{ "Property", "Move Offset Vert", CUnionValueProperty::Integer, "��ư �������� ��ư�� ���� ��ġ �̵�", TRUE, false },
	{ "Property", "Button Type", CUnionValueProperty::Integer_Combo, "��ư Ÿ��|CLICK|TOGGLE", TRUE, false },
	{ "Property", "Variable Width", CUnionValueProperty::Boolean, "���α��� ����", TRUE, false },
	{ "Property", "Variable Width Length", CUnionValueProperty::Integer, "���� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_CheckBoxTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Space Size", CUnionValueProperty::Integer, "üũ��ư�� �ؽ�Ʈ������ ����", TRUE, false },
	{ "Property", "Allign", CUnionValueProperty::Integer_Combo, "��ư����|����|���|������", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_RadioButtonTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Space Size", CUnionValueProperty::Integer, "üũ��ư�� �ؽ�Ʈ������ ����", TRUE, false },
	{ "Property", "Allign", CUnionValueProperty::Integer_Combo, "��ư����|����|���|������", TRUE, false },
	{ "Property", "Icon Index", CUnionValueProperty::Integer, "������ �̹��� �ε���", TRUE, false },
	{ "Property", "Move Offset Hori", CUnionValueProperty::Integer, "��ư �������� ��ư�� ���� ��ġ �̵�", TRUE, false },
	{ "Property", "Move Offset Vert", CUnionValueProperty::Integer, "��ư �������� ��ư�� ���� ��ġ �̵�", TRUE, false },
	{ "Property", "Variable Width Length", CUnionValueProperty::Integer, "���� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ComboBoxTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Move Offset Hori", CUnionValueProperty::Integer, "��ư �������� ��ư�� ���� ��ġ �̵�", TRUE, false },
	{ "Property", "Move Offset Vert", CUnionValueProperty::Integer, "��ư �������� ��ư�� ���� ��ġ �̵�", TRUE, false },
	{ "Property", "Dropdown Size", CUnionValueProperty::Integer, "��Ӵٿ� �ڽ� ũ��", TRUE, false },
	{ "Property", "ScrollBar Name", CUnionValueProperty::String, "��ũ�ѹ� �̸�", TRUE, true },
	{ "Property", "Scrollbar Size", CUnionValueProperty::Integer, "��ũ�ѹ� �ڽ� ũ��", TRUE, false },
	{ "Property", "Variable Width Length", CUnionValueProperty::Integer, "���� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_SliderTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_EditBoxTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Border Size", CUnionValueProperty::Integer, "���� ũ��", TRUE, false },
	{ "Property", "Spacing Size", CUnionValueProperty::Integer, "�����̽� ũ��", TRUE, false },
	{ "Edit Color", "Text Color", CUnionValueProperty::Vector4Color, "���� ����", TRUE, false },
	{ "Edit Color", "Select Text Color", CUnionValueProperty::Vector4Color, "���ÿ��� ���� ����", TRUE, false },
	{ "Edit Color", "Select Back Color", CUnionValueProperty::Vector4Color, "���ÿ��� ��� ����", TRUE, false },
	{ "Edit Color", "Caret Color", CUnionValueProperty::Vector4Color, "Ŀ�� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_IMEEditBoxTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Border Size", CUnionValueProperty::Integer, "���� ũ��", TRUE, false },
	{ "Property", "Spacing Size", CUnionValueProperty::Integer, "�����̽� ũ��", TRUE, false },
	{ "Property", "Indicator Size", CUnionValueProperty::Integer, "�ε������� ũ��", TRUE, false },
	{ "Edit Color", "Text Color", CUnionValueProperty::Vector4Color, "���� ����", TRUE, false },
	{ "Edit Color", "Select Text Color", CUnionValueProperty::Vector4Color, "���ÿ��� ���� ����", TRUE, false },
	{ "Edit Color", "Select Back Color", CUnionValueProperty::Vector4Color, "���ÿ��� ��� ����", TRUE, false },
	{ "Edit Color", "Caret Color", CUnionValueProperty::Vector4Color, "Ŀ�� ����", TRUE, false },
	{ "Composition Color", "Composition Text Color", CUnionValueProperty::Vector4Color, "Composition ������ ���� ����", TRUE, false },
	{ "Composition Color", "Composition Back Color", CUnionValueProperty::Vector4Color, "Composition ������ ��� ����", TRUE, false },
	{ "Composition Color", "Composition Caret Color", CUnionValueProperty::Vector4Color, "Composition ������ Ŀ�� ����", TRUE, false },
	{ "Candidate Color", "Candidate Text Color", CUnionValueProperty::Vector4Color, "Candidate ������ ���� ����", TRUE, false },
	{ "Candidate Color", "Candidate Back Color", CUnionValueProperty::Vector4Color, "Candidate ������ ��� ����", TRUE, false },
	{ "Candidate Color", "Candidate Select Text Color", CUnionValueProperty::Vector4Color, "Candidate ������ ���ÿ��� ���� ����", TRUE, false },
	{ "Candidate Color", "Candidate Select Back Color", CUnionValueProperty::Vector4Color, "Candidate ������ ���ÿ��� ��� ����", TRUE, false },
	{ "Indicator Color", "Indicator Text Color", CUnionValueProperty::Vector4Color, "Indicator ������ ���� ����", TRUE, false },
	{ "Reading Color", "Reading Text Color", CUnionValueProperty::Vector4Color, "Reading ������ ���� ����", TRUE, false },
	{ "Reading Color", "Reading Back Color", CUnionValueProperty::Vector4Color, "Reading ������ ��� ����", TRUE, false },
	{ "Reading Color", "Reading Select Text Color", CUnionValueProperty::Vector4Color, "Reading ������ ���ÿ��� ���� ����", TRUE, false },
	{ "Reading Color", "Reading Select Back Color", CUnionValueProperty::Vector4Color, "Reading ������ ���ÿ��� ��� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ListBoxTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "ScrollBar Name", CUnionValueProperty::String, "��ũ�ѹ� �̸�", TRUE, true },
	{ "Property", "Scrollbar Size", CUnionValueProperty::Integer, "��ũ�ѹ� �ڽ� ũ��", TRUE, false },
	{ "Property", "Border Size", CUnionValueProperty::Integer, "���� ũ��", TRUE, false },
	{ "Property", "Margin Size", CUnionValueProperty::Integer, "���� ũ��", TRUE, false },
	{ "Property", "Line Space", CUnionValueProperty::Integer, "�ٰ���", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ScrollBarTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Thumb Size", CUnionValueProperty::Integer, "Thumb ���� ������", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ProgressBarTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Hori Border Percent", CUnionValueProperty::Float, "���� ���� �ۼ�Ʈ|0.0f|100.0f|0.1f", TRUE, false },
	{ "Property", "Vert Border Percent", CUnionValueProperty::Float, "���� ���� �ۼ�Ʈ|0.0f|100.0f|0.1f", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_CustomTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_TextBoxTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "ScrollBar Name", CUnionValueProperty::String, "��ũ�ѹ� �̸�", TRUE, true },
	{ "Property", "Scrollbar Size", CUnionValueProperty::Integer, "��ũ�ѹ� �ڽ� ũ��", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_HtmlTextBoxTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "ScrollBar Name", CUnionValueProperty::String, "��ũ�ѹ� �̸�", TRUE, true },
	{ "Property", "Scrollbar Size", CUnionValueProperty::Integer, "��ũ�ѹ� �ڽ� ũ��", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_TextureControlTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_TreeTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "ScrollBar Name", CUnionValueProperty::String, "��ũ�ѹ� �̸�", TRUE, true },
	{ "Property", "Scrollbar Size", CUnionValueProperty::Integer, "��ũ�ѹ� �ڽ� ũ��", TRUE, false },
	{ "Property", "Line Space", CUnionValueProperty::Integer, "�ٰ���", TRUE, false },
	{ "Property", "Indent Size", CUnionValueProperty::Integer, "���̱� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_QuestTreeTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "ScrollBar Name", CUnionValueProperty::String, "��ũ�ѹ� �̸�", TRUE, true },
	{ "Property", "Scrollbar Size", CUnionValueProperty::Integer, "��ũ�ѹ� �ڽ� ũ��", TRUE, false },
	{ "Property", "Line Space", CUnionValueProperty::Integer, "�ٰ���", TRUE, false },
	{ "Property", "Indent Size", CUnionValueProperty::Integer, "���̱� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_AnimationTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Loop", CUnionValueProperty::Boolean, "Loop", TRUE, false },
#ifdef _ANIMATION_PLAY_CONTROL
	{ "Property", "PlayTime Setting Type", CUnionValueProperty::Integer_Combo, "�÷��� �ð� ���� ���� Ÿ��|PerFrame|PerAction", TRUE, false },
	{ "Property", "PerAction Time", CUnionValueProperty::Integer, "�� ���ۿ� �ɸ��� �ð�(1/1000��). PlayTime Setting Type�� PerAction�� ���� ���.", TRUE, false },
#endif
	{ NULL },
};

PropertyGridBaseDefine g_LineEditBoxTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Line Space", CUnionValueProperty::Integer, "�ٰ���", TRUE, false },
	{ "Edit Color", "Text Color", CUnionValueProperty::Vector4Color, "���� ����", TRUE, false },
	{ "Edit Color", "Select Text Color", CUnionValueProperty::Vector4Color, "���ÿ��� ���� ����", TRUE, false },
	{ "Edit Color", "Select Back Color", CUnionValueProperty::Vector4Color, "���ÿ��� ��� ����", TRUE, false },
	{ "Edit Color", "Caret Color", CUnionValueProperty::Vector4Color, "Ŀ�� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_LineIMEEditBoxTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Line Space", CUnionValueProperty::Integer, "�ٰ���", TRUE, false },
	{ "Edit Color", "Text Color", CUnionValueProperty::Vector4Color, "���� ����", TRUE, false },
	{ "Edit Color", "Select Text Color", CUnionValueProperty::Vector4Color, "���ÿ��� ���� ����", TRUE, false },
	{ "Edit Color", "Select Back Color", CUnionValueProperty::Vector4Color, "���ÿ��� ��� ����", TRUE, false },
	{ "Edit Color", "Caret Color", CUnionValueProperty::Vector4Color, "Ŀ�� ����", TRUE, false },
	{ "Composition Color", "Composition Text Color", CUnionValueProperty::Vector4Color, "Composition ������ ���� ����", TRUE, false },
	{ "Composition Color", "Composition Back Color", CUnionValueProperty::Vector4Color, "Composition ������ ��� ����", TRUE, false },
	{ "Composition Color", "Composition Caret Color", CUnionValueProperty::Vector4Color, "Composition ������ Ŀ�� ����", TRUE, false },
	{ "Candidate Color", "Candidate Text Color", CUnionValueProperty::Vector4Color, "Candidate ������ ���� ����", TRUE, false },
	{ "Candidate Color", "Candidate Back Color", CUnionValueProperty::Vector4Color, "Candidate ������ ��� ����", TRUE, false },
	{ "Candidate Color", "Candidate Select Text Color", CUnionValueProperty::Vector4Color, "Candidate ������ ���ÿ��� ���� ����", TRUE, false },
	{ "Candidate Color", "Candidate Select Back Color", CUnionValueProperty::Vector4Color, "Candidate ������ ���ÿ��� ��� ����", TRUE, false },
	{ "Reading Color", "Reading Text Color", CUnionValueProperty::Vector4Color, "Reading ������ ���� ����", TRUE, false },
	{ "Reading Color", "Reading Back Color", CUnionValueProperty::Vector4Color, "Reading ������ ��� ����", TRUE, false },
	{ "Reading Color", "Reading Select Text Color", CUnionValueProperty::Vector4Color, "Reading ������ ���ÿ��� ���� ����", TRUE, false },
	{ "Reading Color", "Reading Select Back Color", CUnionValueProperty::Vector4Color, "Reading ������ ���ÿ��� ��� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_MovieControlTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ListBoxExTemplatePropertyDefine[] = 
{
	{ "Property", "UI Type", CUnionValueProperty::Integer_Combo, UI_TYPE_STRING, TRUE, false },
	{ "Property", "Template Name", CUnionValueProperty::String, "Template �̸�", TRUE, false },
	{ "Property", "Template Width", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "Template Height", CUnionValueProperty::Integer, "���ø��� ����ũ��", TRUE, false },
	{ "Property", "ScrollBar Name", CUnionValueProperty::String, "��ũ�ѹ� �̸�", TRUE, true },
	{ "Property", "Scrollbar Size", CUnionValueProperty::Integer, "��ũ�ѹ� �ڽ� ũ��", TRUE, false },
	{ "Property", "Border Size", CUnionValueProperty::Integer, "���� ũ��", TRUE, false },
	{ "Property", "Margin Size", CUnionValueProperty::Integer, "���� ũ��", TRUE, false },
	{ "Property", "Line Space", CUnionValueProperty::Integer, "�ٰ���", TRUE, false },
	{ "Property", "Select Frame Left", CUnionValueProperty::Integer, "�ʿ��� ��쿡�� �����ϼ���!", TRUE, false },
	{ "Property", "Select Frame Top", CUnionValueProperty::Integer, "���������� ���� ũ��", TRUE, false },
	{ "Property", "Select Frame Right", CUnionValueProperty::Integer, "���������� ������ ũ��", TRUE, false },
	{ "Property", "Select Frame Bottom", CUnionValueProperty::Integer, "���������� �Ʒ��� ũ��", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ElementPropertyDefine[] = 
{
	{ "Texture Color", "Normal", CUnionValueProperty::Vector4Color, "Normal State Texture Color", TRUE, false },
	{ "Texture Color", "Disabled", CUnionValueProperty::Vector4Color, "Disabled State Texture Color", TRUE, false },
	{ "Texture Color", "Hidden", CUnionValueProperty::Vector4Color, "Hidden State Texture Color", TRUE, false },
	{ "Texture Color", "Focus", CUnionValueProperty::Vector4Color, "Focus State Texture Color", TRUE, false },
	{ "Texture Color", "MouseEnter", CUnionValueProperty::Vector4Color, "MouseEnter State Texture Color", TRUE, false },
	{ "Texture Color", "Pressed", CUnionValueProperty::Vector4Color, "Pressed State Texture Color", TRUE, false },
	{ "Font Color", "Normal", CUnionValueProperty::Vector4Color, "Normal State Font Color", TRUE, false },
	{ "Font Color", "Disabled", CUnionValueProperty::Vector4Color, "Disabled State Font Color", TRUE, false },
	{ "Font Color", "Hidden", CUnionValueProperty::Vector4Color, "Hidden State Font Color", TRUE, false },
	{ "Font Color", "Focus", CUnionValueProperty::Vector4Color, "Focus State Font Color", TRUE, false },
	{ "Font Color", "MouseEnter", CUnionValueProperty::Vector4Color, "MouseEnter State Font Color", TRUE, false },
	{ "Font Color", "Pressed", CUnionValueProperty::Vector4Color, "Pressed State Font Color", TRUE, false },
	{ "Shadow Font Color", "Normal", CUnionValueProperty::Vector4Color, "Normal State Shadow Font Color", TRUE, false },
	{ "Shadow Font Color", "Disabled", CUnionValueProperty::Vector4Color, "Disabled State Shadow Font Color", TRUE, false },
	{ "Shadow Font Color", "Hidden", CUnionValueProperty::Vector4Color, "Hidden State Shadow Font Color", TRUE, false },
	{ "Shadow Font Color", "Focus", CUnionValueProperty::Vector4Color, "Focus State Shadow Font Color", TRUE, false },
	{ "Shadow Font Color", "MouseEnter", CUnionValueProperty::Vector4Color, "MouseEnter State Shadow Font Color", TRUE, false },
	{ "Shadow Font Color", "Pressed", CUnionValueProperty::Vector4Color, "Pressed State Shadow Font Color", TRUE, false },
	{ "Texture", "Texture File", CUnionValueProperty::String_FileOpen, "UI Texture File|Direct Draw Texture File|*.dds", TRUE, false },
	{ "Texture", "Texture Width", CUnionValueProperty::Integer, "UI Texture Width", TRUE, false },
	{ "Texture", "Texture Height", CUnionValueProperty::Integer, "UI Texture Height", TRUE, false },
	{ "Texture", "Texture Delay Time", CUnionValueProperty::Integer, "UI Texture Delay Time(1/100s)", TRUE, false },
	{ "Font Attribute", "Horizontal Allign", CUnionValueProperty::Integer_Combo, "�¿�����|Left|Center|Right", TRUE, false },
	{ "Font Attribute", "Vertical Allign", CUnionValueProperty::Integer_Combo, "��������|Top|VCenter|Bottom", TRUE, false },
	{ "Font Attribute", "FontSet Index", CUnionValueProperty::Integer, "FontSet Index", TRUE, true },
	{ "Font Attribute", "(���� Draw Type ����ϼ���)Shadow Font", CUnionValueProperty::Boolean, "Shadow Font", TRUE, false },
	{ "Font Attribute", "Font Hori Offset", CUnionValueProperty::Integer, "��Ʈ ���� ������", TRUE, false },
	{ "Font Attribute", "Font Vert Offset", CUnionValueProperty::Integer, "��Ʈ ���� ������", TRUE, false },
	{ "Font Attribute", "Font Draw Type", CUnionValueProperty::Integer_Combo, "��Ʈ ��ο� Ÿ��|Normal|Shadow|Stroke|Glow", TRUE, false },
	{ "Font Attribute", "Font Weight", CUnionValueProperty::Integer, "��Ʈ ��ο� Ÿ�Կ� ����Ǵ� ����", TRUE, false },
	{ "Font Attribute", "Alpha Weight", CUnionValueProperty::Float, "��Ʈ ȿ���� ����Ǵ� ������ ����ġ Stroke Ÿ�Կ��� ������ ��ġ�� �ʽ��ϴ�.|1.0f|2.0f|0.0001f", TRUE, false },
	{ "Font Attribute", "Blur Weight", CUnionValueProperty::Float, "��ü ��Ʈ�� ����Ǵ� ���� ����(�����ϱ� �����ϰ� ������ֽñ�)|0.0f|1.0f|0.0001f", TRUE, false },
	{ NULL },
};

IMPLEMENT_DYNCREATE(CTemplatePropertyPaneView, CFormView)

CTemplatePropertyPaneView::CTemplatePropertyPaneView()
	: CFormView(CTemplatePropertyPaneView::IDD)
{
	m_bActivate = false;
	m_pCurTemplate = NULL;
}

CTemplatePropertyPaneView::~CTemplatePropertyPaneView()
{
}

void CTemplatePropertyPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTemplatePropertyPaneView, CFormView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify )
	ON_MESSAGE( UM_PROPERTYGRID_ONCUSTOM_DIALOG, OnCustomDialogGrid )
END_MESSAGE_MAP()


// CTemplatePropertyPaneView diagnostics

#ifdef _DEBUG
void CTemplatePropertyPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTemplatePropertyPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTemplatePropertyPaneView message handlers


void CTemplatePropertyPaneView::RefreshProperty( CUIToolTemplate *pTemplate, std::vector< int > &vecItemIndex )
{
	//if( ( ( CMainFrame * )AfxGetMainWnd() )->IsPaneClosed( TEMPLATE_PROPERTY_PANE ) )
	//{
	//	return;
	//}

	m_vecUIElementIndex.clear();

	if( !vecItemIndex.empty() )
	{
		m_vecUIElementIndex.resize( vecItemIndex.size() );
		std::copy( vecItemIndex.begin(), vecItemIndex.end(), m_vecUIElementIndex.begin() );
	}

	PropertyGridBaseDefine *pPropertyDefine(NULL);

	if( m_vecUIElementIndex.empty() )
	{
		switch( pTemplate->UIType() )
		{
		case UI_CONTROL_STATIC:				pPropertyDefine = g_StaticTemplatePropertyDefine;			break;
		case UI_CONTROL_BUTTON:				pPropertyDefine = g_ButtonTemplatePropertyDefine;			break;
		case UI_CONTROL_CHECKBOX:			pPropertyDefine = g_CheckBoxTemplatePropertyDefine;			break;
		case UI_CONTROL_RADIOBUTTON:		pPropertyDefine = g_RadioButtonTemplatePropertyDefine;		break;
		case UI_CONTROL_COMBOBOX:			pPropertyDefine = g_ComboBoxTemplatePropertyDefine;			break;
		case UI_CONTROL_SLIDER:				pPropertyDefine = g_SliderTemplatePropertyDefine;			break;
		case UI_CONTROL_EDITBOX:			pPropertyDefine = g_EditBoxTemplatePropertyDefine;			break;
		case UI_CONTROL_IMEEDITBOX:			pPropertyDefine = g_IMEEditBoxTemplatePropertyDefine;		break;
		case UI_CONTROL_LISTBOX:			pPropertyDefine = g_ListBoxTemplatePropertyDefine;			break;
		case UI_CONTROL_SCROLLBAR:			pPropertyDefine = g_ScrollBarTemplatePropertyDefine;		break;
		case UI_CONTROL_PROGRESSBAR:		pPropertyDefine = g_ProgressBarTemplatePropertyDefine;		break;
		case UI_CONTROL_CUSTOM:				pPropertyDefine = g_CustomTemplatePropertyDefine;			break;
		case UI_CONTROL_TEXTBOX:			pPropertyDefine = g_TextBoxTemplatePropertyDefine;			break;
		case UI_CONTROL_HTMLTEXTBOX:		pPropertyDefine = g_HtmlTextBoxTemplatePropertyDefine;		break;
		case UI_CONTROL_TEXTURECONTROL:		pPropertyDefine = g_TextureControlTemplatePropertyDefine;	break;
		case UI_CONTROL_TREECONTROL:		pPropertyDefine = g_TreeTemplatePropertyDefine;				break;
		case UI_CONTROL_QUESTTREECONTROL:	pPropertyDefine = g_QuestTreeTemplatePropertyDefine;		break;
		case UI_CONTROL_ANIMATION:			pPropertyDefine = g_AnimationTemplatePropertyDefine;		break;
		case UI_CONTROL_LINE_EDITBOX:		pPropertyDefine = g_LineEditBoxTemplatePropertyDefine;		break;
		case UI_CONTROL_LINE_IMEEDITBOX:	pPropertyDefine = g_LineIMEEditBoxTemplatePropertyDefine;	break;
		case UI_CONTROL_MOVIECONTROL:		pPropertyDefine = g_MovieControlTemplatePropertyDefine;		break;
		case UI_CONTROL_LISTBOXEX:			pPropertyDefine = g_ListBoxExTemplatePropertyDefine;		break;
		default:
			ASSERT( 0 && "CTemplatePropertyPaneView::RefreshProperty, Invalid UI Type!!!" );
			break;
		}
	}
	else
	{
		pPropertyDefine = g_ElementPropertyDefine;
	}

	if( m_pCurTemplate != pTemplate )
	{
		CEtUIToolView *pView(NULL);
		CTemplateView *pTemplateView(NULL);

		pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
		pTemplateView = ( CTemplateView * )pView->GetTabView(CDummyView::TEMPLATE_VIEW);
		pTemplateView->AddTemplateControl( pTemplate );
	}
	m_pCurTemplate = pTemplate;
	RefreshPropertyGrid( pPropertyDefine );
}

void CTemplatePropertyPaneView::ApplyTemplateChange( bool bChangeElementTexture )
{
	m_pCurTemplate->m_bChanged = true;

	CEtUIToolView *pView(NULL);
	CTemplateView *pTemplateView(NULL);
	CLayoutView *pLayoutView(NULL);

	pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	pTemplateView = ( CTemplateView * )pView->GetTabView( CDummyView::TEMPLATE_VIEW );
	pTemplateView->AddTemplateControl( m_pCurTemplate );

	pLayoutView = ( CLayoutView * )pView->GetTabView( 0 );
	pLayoutView->RegenerateTexture();
	pLayoutView->ReinitAllTemplate();
	pLayoutView->ReinitAllControl();

	// �Ź� ���ø� �����Ҷ�����(AddTemplateControl) ȣ��ɶ����� ���׷��̼� �� �ʿ�� ���⶧����,
	// �������� �������� ���ø��ؽ�ó�� �����ϱ�� �Ѵ�.
	if( bChangeElementTexture )
	{
		// ���⼱ ���׷���Ʈ�� �ϰ�, �ؽ�ó �ڵ� ������ �ִ´�.
		std::vector< CUIToolTemplate * > vecTemplate;
		vecTemplate.push_back( m_pCurTemplate );
		EtTextureHandle hGenTexture = GenerateTexture( vecTemplate, true );
		if( hGenTexture ) m_pCurTemplate->m_Template.m_hTemplateTexture = hGenTexture;
	}
}

void CTemplatePropertyPaneView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	if( m_bActivate == true ) return;
	m_bActivate = true;

	CRect rcRect;
	GetClientRect( &rcRect );
	m_PropertyGrid.Create( rcRect, this, 0 );
	m_PropertyGrid.SetCustomColors( RGB(200, 200, 200), 0, RGB(182, 210, 189), RGB(247, 243, 233), 0 );

//	SendMessage( UM_REFRESH_PANE_VIEW );
}

BOOL CTemplatePropertyPaneView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CTemplatePropertyPaneView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( m_PropertyGrid ) {
		m_PropertyGrid.SetWindowPos( NULL, 0, 0, cx, cy, SWP_FRAMECHANGED );
	}
}

LRESULT CTemplatePropertyPaneView::OnGridNotify( WPARAM wParam, LPARAM lParam )
{
	return ProcessNotifyGrid( wParam, lParam );
}

DWORD ColorToDWORD( D3DXCOLOR Color )
{
	return D3DCOLOR_COLORVALUE(Color.r, Color.g, Color.b, Color.a);
}

D3DXCOLOR DWORDToColor( DWORD dwColor )
{
	return D3DXCOLOR( dwColor );
}

void CTemplatePropertyPaneView::OnSetValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( !m_pCurTemplate )
	{
		return;
	}

	if( m_vecUIElementIndex.empty() )
	{
		const char *pTemplateName(NULL);

		switch( dwIndex )
		{
		case 0:
			pVariable->SetVariable( m_pCurTemplate->UIType() );
			break;
		case 1:
			pTemplateName = m_pCurTemplate->GetTemplateName();
			pVariable->SetVariable ( pTemplateName ? pTemplateName : "" );
			break;
		case 2:
			pVariable->SetVariable( ( int )( m_pCurTemplate->m_fDefaultWidth * DEFAULT_UI_SCREEN_WIDTH ) );
			break;
		case 3:
			pVariable->SetVariable( ( int )( m_pCurTemplate->m_fDefaultHeight * DEFAULT_UI_SCREEN_HEIGHT ) );
			break;
		}

		switch( m_pCurTemplate->m_UIType )
		{
		case UI_CONTROL_STATIC:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.StaticInfo.nFrameLeft );
				break;
			case 5:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.StaticInfo.nFrameTop );
				break;
			case 6:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.StaticInfo.nFrameRight );
				break;
			case 7:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.StaticInfo.nFrameBottom );
				break;
			}
			break;
		case UI_CONTROL_BUTTON:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ButtonInfo.fMoveOffsetHori * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ButtonInfo.fMoveOffsetVert * DEFAULT_UI_SCREEN_HEIGHT ) );
				break;
			case 6:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.ButtonInfo.ButtonType );
				break;
			case 7:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.ButtonInfo.bVariableWidth );
				break;
			case 8:
				pVariable->SetVariable( (int)(m_pCurTemplate->m_ControlInfo.ButtonInfo.fVariableWidthLenth * DEFAULT_UI_SCREEN_WIDTH) );
				break;
			}
			break;
		case UI_CONTROL_CHECKBOX:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.CheckBoxInfo.fSpace * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 5:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.CheckBoxInfo.nAllign );
				break;
			}
			break;
		case UI_CONTROL_RADIOBUTTON:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.RadioButtonInfo.fSpace * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 5:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.RadioButtonInfo.nAllign );
				break;
			case 6:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.RadioButtonInfo.nIconIndex );
				break;
			case 7:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.RadioButtonInfo.fMoveOffsetHori * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 8:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.RadioButtonInfo.fMoveOffsetVert * DEFAULT_UI_SCREEN_HEIGHT ) );
				break;
			case 9:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.RadioButtonInfo.fVariableWidthLength * DEFAULT_UI_SCREEN_WIDTH) );
				break;
			}
			break;
		case UI_CONTROL_EDITBOX:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.EditBoxInfo.fBorder * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.EditBoxInfo.fSpace * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 6:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.EditBoxInfo.dwTextColor ) );
				break;
			case 7:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.EditBoxInfo.dwSelTextColor ) );
				break;
			case 8:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.EditBoxInfo.dwSelBackColor ) );
				break;
			case 9:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.EditBoxInfo.dwCaretColor ) );
				break;
			}
			break;
		case UI_CONTROL_IMEEDITBOX:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.fBorder * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.fSpace * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 6:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.fIndicatorSize * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 7:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwTextColor ) );
				break;
			case 8:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwSelTextColor ) );
				break;
			case 9:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwSelBackColor ) );
				break;
			case 10:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCaretColor ) );
				break;
			case 11:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompTextColor ) );
				break;
			case 12:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompBackColor ) );
				break;
			case 13:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompCaretColor ) );
				break;
			case 14:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiTextColor ) );
				break;
			case 15:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiBackColor ) );
				break;
			case 16:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiSelTextColor ) );
				break;
			case 17:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiSelBackColor ) );
				break;
			case 18:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwIndiTextColor ) );
				break;
			case 19:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingTextColor ) );
				break;
			case 20:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingBackColor ) );
				break;
			case 21:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingSelTextColor ) );
				break;
			case 22:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingSelBackColor ) );
				break;
			}
			break;
		case UI_CONTROL_COMBOBOX:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fMoveOffsetHori * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fMoveOffsetVert * DEFAULT_UI_SCREEN_HEIGHT ) );
				break;
			case 6:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fDropdownSize * DEFAULT_UI_SCREEN_HEIGHT ) );
				break;
			case 7:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					pVariable->SetVariable( ( char * )m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName.c_str() );
				}
				break;
			case 8:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fScrollBarSize * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 9:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fVariableWidthLenth * DEFAULT_UI_SCREEN_WIDTH) );
				break;
			}
			break;
		case UI_CONTROL_LISTBOX:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					pVariable->SetVariable( ( char * )m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName.c_str() );
				}
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ListBoxInfo.fScrollBarSize * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 6:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ListBoxInfo.fBorder * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 7:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ListBoxInfo.fMargin * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 8:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ListBoxInfo.fLineSpace * DEFAULT_UI_SCREEN_HEIGHT ) );
				break;
			}
			break;
		case UI_CONTROL_PROGRESSBAR:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.ProgressBarInfo.fHoriBorder * 100.0f );
				break;
			case 5:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.ProgressBarInfo.fVertBorder * 100.0f );
				break;
			}
			break;
		case UI_CONTROL_TEXTBOX:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					pVariable->SetVariable( ( char * )m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName.c_str() );
				}
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.TextBoxInfo.fScrollBarSize * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			}
			break;
		case UI_CONTROL_HTMLTEXTBOX:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					pVariable->SetVariable( ( char * )m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName.c_str() );
				}
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.HtmlTextBoxInfo.fScrollBarSize * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			}
			break;
		case UI_CONTROL_TREECONTROL:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					pVariable->SetVariable( ( char * )m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName.c_str() );
				}
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.TreeInfo.fScrollBarSize * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 6:
				pVariable->SetVariable( (int)(m_pCurTemplate->m_ControlInfo.TreeInfo.fLineSpace * DEFAULT_UI_SCREEN_HEIGHT) );
				break;
			case 7:
				pVariable->SetVariable( (int)(m_pCurTemplate->m_ControlInfo.TreeInfo.fIndentSize * DEFAULT_UI_SCREEN_WIDTH) );
				break;
			}
			break;
		case UI_CONTROL_QUESTTREECONTROL:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					pVariable->SetVariable( ( char * )m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName.c_str() );
				}
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.QuestTreeInfo.fScrollBarSize * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 6:
				pVariable->SetVariable( (int)(m_pCurTemplate->m_ControlInfo.QuestTreeInfo.fLineSpace * DEFAULT_UI_SCREEN_HEIGHT) );
				break;
			case 7:
				pVariable->SetVariable( (int)(m_pCurTemplate->m_ControlInfo.QuestTreeInfo.fIndentSize * DEFAULT_UI_SCREEN_WIDTH) );
				break;
			}
			break;
		case UI_CONTROL_SCROLLBAR:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( (int)(m_pCurTemplate->m_ControlInfo.ScrollBarInfo.fThumbVariableLenth * DEFAULT_UI_SCREEN_HEIGHT ) );
				break;
			}
			break;
		case UI_CONTROL_ANIMATION:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.AnimationInfo.bLoop );
				break;
#ifdef _ANIMATION_PLAY_CONTROL
			case 5:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.AnimationInfo.PlayTimeType );
				break;
			case 6:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.AnimationInfo.timePerAction );
				break;
#endif
			}
			break;
		case UI_CONTROL_LINE_EDITBOX:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( (int)(m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.fLineSpace * DEFAULT_UI_SCREEN_HEIGHT) );
				break;
			case 5:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.dwTextColor ) );
				break;
			case 6:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.dwSelTextColor ) );
				break;
			case 7:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.dwSelBackColor ) );
				break;
			case 8:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.dwCaretColor ) );
				break;
			}
			break;
		case UI_CONTROL_LINE_IMEEDITBOX:
			switch( dwIndex )
			{
			case 4:
				pVariable->SetVariable( (int)(m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.fLineSpace * DEFAULT_UI_SCREEN_HEIGHT) );
				break;
			case 5:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwTextColor ) );
				break;
			case 6:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwSelTextColor ) );
				break;
			case 7:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwSelBackColor ) );
				break;
			case 8:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCaretColor ) );
				break;
			case 9:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompTextColor ) );
				break;
			case 10:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompBackColor ) );
				break;
			case 11:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompCaretColor ) );
				break;
			case 12:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiTextColor ) );
				break;
			case 13:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiBackColor ) );
				break;
			case 14:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiSelTextColor ) );
				break;
			case 15:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiSelBackColor ) );
				break;
			case 16:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingTextColor ) );
				break;
			case 17:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingBackColor ) );
				break;
			case 18:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingSelTextColor ) );
				break;
			case 19:
				pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingSelBackColor ) );
				break;
			}
			break;
		case UI_CONTROL_LISTBOXEX:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					pVariable->SetVariable( ( char * )m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName.c_str() );
				}
				break;
			case 5:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ListBoxExInfo.fScrollBarSize * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 6:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ListBoxExInfo.fBorder * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 7:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ListBoxExInfo.fMargin * DEFAULT_UI_SCREEN_WIDTH ) );
				break;
			case 8:
				pVariable->SetVariable( ( int )( m_pCurTemplate->m_ControlInfo.ListBoxExInfo.fLineSpace * DEFAULT_UI_SCREEN_HEIGHT ) );
				break;
			case 9:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.ListBoxExInfo.nSelectFrameLeft );
				break;
			case 10:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.ListBoxExInfo.nSelectFrameTop );
				break;
			case 11:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.ListBoxExInfo.nSelectFrameRight );
				break;
			case 12:
				pVariable->SetVariable( m_pCurTemplate->m_ControlInfo.ListBoxExInfo.nSelectFrameBottom );
				break;
			}
			break;
		}
	}
	else
	{
		int nCurElementIndex = m_vecUIElementIndex[ 0 ];
		const SUIElement &uiElement = m_pCurTemplate->m_Template.m_vecElement[ nCurElementIndex ];

		switch( dwIndex )
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( uiElement.TextureColor.dwColor[ dwIndex ] ) );
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( uiElement.FontColor.dwColor[ dwIndex - 6 ] ) );
			break;
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( uiElement.ShadowFontColor.dwColor[ dwIndex - 12 ] ) );
			break;
		case 18:
			pVariable->SetVariable( ( char * )m_pCurTemplate->m_vecTextureName[ nCurElementIndex ].c_str() );
			break;
		case 19:
			pVariable->SetVariable( (int)(uiElement.fTextureWidth * DEFAULT_UI_SCREEN_WIDTH) );
			break;
		case 20:
			pVariable->SetVariable( (int)(uiElement.fTextureHeight * DEFAULT_UI_SCREEN_HEIGHT) );
			break;
		case 21:
			pVariable->SetVariable( (int)(uiElement.fDelayTime * 100.0f) );
			break;
		case 22:
			pVariable->SetVariable( ( int )( uiElement.dwFontFormat & 0x03 ) );
			break;
		case 23:
			pVariable->SetVariable( ( int )( ( uiElement.dwFontFormat & 0xc ) >> 2 ) );
			break;
		case 24:
			pVariable->SetVariable( m_pCurTemplate->m_vecFontSetIndex[ nCurElementIndex ] );
			break;
		case 25:
			pVariable->SetVariable( uiElement.bShadowFont );
			break;
		case 26:
			pVariable->SetVariable( int(uiElement.fFontHoriOffset * DEFAULT_UI_SCREEN_WIDTH) );
			break;
		case 27:
			pVariable->SetVariable( int(uiElement.fFontVertOffset * DEFAULT_UI_SCREEN_HEIGHT) );
			break;

		case 28:
			pVariable->SetVariable( uiElement.nDrawType );
			break;

		case 29:
			pVariable->SetVariable( uiElement.nWeight );
			break;

		case 30:
			pVariable->SetVariable( uiElement.fAlphaWeight );
			break;

		case 31:
			pVariable->SetVariable( uiElement.fGlobalBlurAlphaWeight );
			break;
		}
	}
}

void CTemplatePropertyPaneView::OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( !m_pCurTemplate )
	{
		return;
	}

	bool bChangeElementTexture = false;
	if( m_vecUIElementIndex.empty() )
	{
		switch( dwIndex )
		{
		case 0:
			m_pCurTemplate->SetUIType( ( UI_CONTROL_TYPE )pVariable->GetVariableInt() );
			break;
		case 1:
			m_pCurTemplate->SetTemplateName( pVariable->GetVariableString() );
			break;
		case 2:
			m_pCurTemplate->m_fDefaultWidth = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
			break;
		case 3:
			m_pCurTemplate->m_fDefaultHeight = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
			break;
		}

		switch( m_pCurTemplate->m_UIType )
		{
		case UI_CONTROL_STATIC:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.StaticInfo.nFrameLeft = pVariable->GetVariableInt();
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.StaticInfo.nFrameTop = pVariable->GetVariableInt();
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.StaticInfo.nFrameRight = pVariable->GetVariableInt();
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.StaticInfo.nFrameBottom = pVariable->GetVariableInt();
				break;
			}
			break;
		case UI_CONTROL_BUTTON:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.ButtonInfo.fMoveOffsetHori = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.ButtonInfo.fMoveOffsetVert = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.ButtonInfo.ButtonType = (UIButtonType)pVariable->GetVariableInt();
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.ButtonInfo.bVariableWidth = pVariable->GetVariableBool();
				break;
			case 8:
				m_pCurTemplate->m_ControlInfo.ButtonInfo.fVariableWidthLenth = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_WIDTH;
				break;
			}
			break;
		case UI_CONTROL_CHECKBOX:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.CheckBoxInfo.fSpace = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.CheckBoxInfo.nAllign = pVariable->GetVariableInt();
				break;
			}
			break;
		case UI_CONTROL_RADIOBUTTON:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.RadioButtonInfo.fSpace = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.RadioButtonInfo.nAllign = pVariable->GetVariableInt();
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.RadioButtonInfo.nIconIndex = pVariable->GetVariableInt();
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.RadioButtonInfo.fMoveOffsetHori = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 8:
				m_pCurTemplate->m_ControlInfo.RadioButtonInfo.fMoveOffsetVert = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 9:
				m_pCurTemplate->m_ControlInfo.RadioButtonInfo.fVariableWidthLength = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			}
			break;
		case UI_CONTROL_EDITBOX:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.EditBoxInfo.fBorder = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.EditBoxInfo.fSpace = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.EditBoxInfo.dwTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.EditBoxInfo.dwSelTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 8:
				m_pCurTemplate->m_ControlInfo.EditBoxInfo.dwSelBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 9:
				m_pCurTemplate->m_ControlInfo.EditBoxInfo.dwCaretColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			}
			break;
		case UI_CONTROL_IMEEDITBOX:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.fBorder = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.fSpace = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.fIndicatorSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 8:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwSelTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 9:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwSelBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 10:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCaretColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 11:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 12:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 13:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompCaretColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 14:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 15:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 16:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiSelTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 17:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiSelBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 18:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwIndiTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 19:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 20:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 21:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingSelTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 22:
				m_pCurTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingSelBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			}
			break;
		case UI_CONTROL_COMBOBOX:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fMoveOffsetHori = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fMoveOffsetVert = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fDropdownSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 7:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName = pVariable->GetVariableString();
				}
				break;
			case 8:
				m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fScrollBarSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 9:
				m_pCurTemplate->m_ControlInfo.ComboBoxInfo.fVariableWidthLenth = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			}
			break;
		case UI_CONTROL_LISTBOX:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName = pVariable->GetVariableString();
				}
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.ListBoxInfo.fScrollBarSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.ListBoxInfo.fBorder = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.ListBoxInfo.fMargin = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 8:
				m_pCurTemplate->m_ControlInfo.ListBoxInfo.fLineSpace = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
				break;
			}
			break;
		case UI_CONTROL_PROGRESSBAR:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.ProgressBarInfo.fHoriBorder = pVariable->GetVariableFloat() / 100.0f;
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.ProgressBarInfo.fVertBorder = pVariable->GetVariableFloat() / 100.0f;
				break;
			}
			break;
		case UI_CONTROL_TEXTBOX:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName = pVariable->GetVariableString();
				}
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.TextBoxInfo.fScrollBarSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			}
			break;
		case UI_CONTROL_HTMLTEXTBOX:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName = pVariable->GetVariableString();
				}
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.HtmlTextBoxInfo.fScrollBarSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			}
			break;
		case UI_CONTROL_TREECONTROL:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName = pVariable->GetVariableString();
				}
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.TreeInfo.fScrollBarSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.TreeInfo.fLineSpace = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.TreeInfo.fIndentSize = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_WIDTH;
				break;
			}
			break;
		case UI_CONTROL_QUESTTREECONTROL:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName = pVariable->GetVariableString();
				}
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.QuestTreeInfo.fScrollBarSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.QuestTreeInfo.fLineSpace = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.QuestTreeInfo.fIndentSize = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_WIDTH;
				break;
			}
			break;
		case UI_CONTROL_SCROLLBAR:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.ScrollBarInfo.fThumbVariableLenth = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_HEIGHT;
				break;
			}
			break;
		case UI_CONTROL_ANIMATION:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.AnimationInfo.bLoop = pVariable->GetVariableBool();
				break;
#ifdef _ANIMATION_PLAY_CONTROL
			case 5:
				m_pCurTemplate->m_ControlInfo.AnimationInfo.PlayTimeType = (UIAnimationPlayTimeType)pVariable->GetVariableInt();
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.AnimationInfo.timePerAction = pVariable->GetVariableInt();
				break;
#endif
			}
			break;
		case UI_CONTROL_LINE_EDITBOX:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.fLineSpace = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.dwTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.dwSelTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.dwSelBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 8:
				m_pCurTemplate->m_ControlInfo.LineEditBoxInfo.dwCaretColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			}
			break;
		case UI_CONTROL_LINE_IMEEDITBOX:
			switch( dwIndex )
			{
			case 4:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.fLineSpace = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwSelTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwSelBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 8:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCaretColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 9:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 10:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 11:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompCaretColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 12:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 13:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 14:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiSelTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 15:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiSelBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 16:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 17:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 18:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingSelTextColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 19:
				m_pCurTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingSelBackColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			}
			break;
		case UI_CONTROL_LISTBOXEX:
			switch( dwIndex )
			{
			case 4:
				if( m_pCurTemplate->m_pExternControlTemplate )
				{
					m_pCurTemplate->m_pExternControlTemplate->m_Template.m_szTemplateName = pVariable->GetVariableString();
				}
				break;
			case 5:
				m_pCurTemplate->m_ControlInfo.ListBoxExInfo.fScrollBarSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 6:
				m_pCurTemplate->m_ControlInfo.ListBoxExInfo.fBorder = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 7:
				m_pCurTemplate->m_ControlInfo.ListBoxExInfo.fMargin = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 8:
				m_pCurTemplate->m_ControlInfo.ListBoxExInfo.fLineSpace = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 9:
				m_pCurTemplate->m_ControlInfo.ListBoxExInfo.nSelectFrameLeft = pVariable->GetVariableInt();
				break;
			case 10:
				m_pCurTemplate->m_ControlInfo.ListBoxExInfo.nSelectFrameTop = pVariable->GetVariableInt();
				break;
			case 11:
				m_pCurTemplate->m_ControlInfo.ListBoxExInfo.nSelectFrameRight = pVariable->GetVariableInt();
				break;
			case 12:
				m_pCurTemplate->m_ControlInfo.ListBoxExInfo.nSelectFrameBottom = pVariable->GetVariableInt();
				break;
			}
			break;
		}
	}
	else
	{
		int nCurElementIndex;
		SUIElement *pElement;

		EtTextureHandle hTexture;
		char szFileName[128];

		for( int i = 0; i < ( int )m_vecUIElementIndex.size(); i++ )
		{
			nCurElementIndex = m_vecUIElementIndex[ i ];
			pElement = &m_pCurTemplate->m_Template.m_vecElement[ nCurElementIndex ];

			switch( dwIndex )
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				pElement->TextureColor.dwColor[ dwIndex ] = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				pElement->FontColor.dwColor[ dwIndex - 6 ] = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
				pElement->ShadowFontColor.dwColor[ dwIndex - 12 ] = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
				break;
			case 18:

				// ���ø� �ؽ�ó �����ϱ�.
				// �Ʒ�ó�� ���ϸ� �����ؼ� �ȵȴ�.
				// �巡��׽�Ʈ ������Ʈ�� �޸� ���ҽ��Ŵ����� ������� �ʱ� ������, ������ ��ü�� �����ؾ��Ѵ�.
				//
				// ���� ���߿� ����� ���·� �ٲ۴ٸ�, ���⼭ ����α��� ������ �� LoadResouce�ʿ��� ������ ������ ������ ���ļ� �ε��ϸ� �ɰŴ�.
				//
				//TCHAR szDrive[256], szDir[256], szName[256], szExt[256];
				//SPLITPATH_S(pVariable->GetVariableString(), szDrive, szDir, szName, szExt);
				//strcat(szName, szExt);
				//m_pCurTemplate->m_vecTextureName[ nCurElementIndex ] = szName;

				m_pCurTemplate->m_vecTextureName[ nCurElementIndex ] = pVariable->GetVariableString();

				// �ؽ�ó�� �����ϸ� �ؽ�ó ũ���� �ؽ�ó ���� ���ΰ� �ڵ����� �Էµȴ�.
				_GetFullFileName( szFileName, _countof(szFileName), pVariable->GetVariableString() );
				hTexture = LoadResource( szFileName, RT_TEXTURE );
				if( hTexture )
				{
					pElement->fTextureWidth = hTexture->Width() / (float)DEFAULT_UI_SCREEN_WIDTH;
					pElement->fTextureHeight = hTexture->Height() / (float)DEFAULT_UI_SCREEN_HEIGHT;
					SAFE_RELEASE_SPTR( hTexture );
					RefreshPropertyGridVariable();
					bChangeElementTexture = true;
				}
				break;
			case 19:
				pElement->fTextureWidth = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 20:
				pElement->fTextureHeight = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_HEIGHT;
				break;
			case 21:
				pElement->fDelayTime = pVariable->GetVariableInt() / 100.0f;
				break;
			case 22:
				pElement->dwFontFormat &= 0xfffffffc;
				pElement->dwFontFormat |= pVariable->GetVariableInt();
				break;
			case 23:
				pElement->dwFontFormat &= 0xfffffff3;
				pElement->dwFontFormat |= pVariable->GetVariableInt() << 2;
				break;
			case 24:
				m_pCurTemplate->m_vecFontSetIndex[ nCurElementIndex ] = pVariable->GetVariableInt();
				break;
			case 25:
				pElement->bShadowFont = pVariable->GetVariableBool();
				break;
			case 26:
				pElement->fFontHoriOffset = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_WIDTH;
				break;
			case 27:
				pElement->fFontVertOffset = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_HEIGHT;
				break;

			case 28:
				pElement->nDrawType = pVariable->GetVariableInt();
				break;

			case 29:
				pElement->nWeight = pVariable->GetVariableInt();
				break;

			case 30:
				//float* pFloatMinMax = pVariable->GetVariableFloatMinMax();
				//if( pFloatMinMax[ 0 ] < pFloatMinMax[ 1 ] )
				//	pElement->fAlphaWeight = pFloatMinMax[ 1 ];
				//else
				//if( pFloatMinMax[ 0 ] > pFloatMinMax[ 2 ] )
				//	pElement->fAlphaWeight = pFloatMinMax[ 2 ];
				//else
				//	pElement->fAlphaWeight = pFloatMinMax[ 0 ];
				pElement->fAlphaWeight = pVariable->GetVariableFloat();
				break;

			case 31:
				pElement->fGlobalBlurAlphaWeight = pVariable->GetVariableFloat();
				break;
			}
		}
	}

	ApplyTemplateChange( bChangeElementTexture );
}

void CTemplatePropertyPaneView::OnSelectChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
}

LRESULT CTemplatePropertyPaneView::OnCustomDialogGrid( WPARAM wParam, LPARAM lParam )
{
	return ProcessCustomDialogGrid( wParam, lParam );
}

extern std::vector< CUIToolTemplate * > g_vecScrollBar;

void CTemplatePropertyPaneView::OnCustomDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( m_vecUIElementIndex.empty() )
	{
		if( dwIndex == 7 )
		{
			// Note : �޺��ڽ� ��ũ��
			//
			CScrollBarSelect ScrollBarDlg;

			if( ScrollBarDlg.DoModal() == IDOK )
			{
				if( ScrollBarDlg.m_nSelectIndex != -1 )
				{
					m_pCurTemplate->m_pExternControlTemplate = g_vecScrollBar[ ScrollBarDlg.m_nSelectIndex ];
					m_pCurTemplate->m_szExternalControlName = g_vecScrollBar[ ScrollBarDlg.m_nSelectIndex ]->m_Template.m_szTemplateName;
				}
				else
				{
					m_pCurTemplate->m_pExternControlTemplate = NULL;
					m_pCurTemplate->m_szExternalControlName = "";
				}

				RefreshPropertyGrid( g_ComboBoxTemplatePropertyDefine );
			}
		}
		else if( dwIndex == 4 )
		{
			CScrollBarSelect ScrollBarDlg;

			if( ScrollBarDlg.DoModal() == IDOK )
			{
				if( ScrollBarDlg.m_nSelectIndex != -1 )
				{
					m_pCurTemplate->m_pExternControlTemplate = g_vecScrollBar[ ScrollBarDlg.m_nSelectIndex ];
					m_pCurTemplate->m_szExternalControlName = g_vecScrollBar[ ScrollBarDlg.m_nSelectIndex ]->m_Template.m_szTemplateName;
				}
				else
				{
					m_pCurTemplate->m_pExternControlTemplate = NULL;
					m_pCurTemplate->m_szExternalControlName = "";
				}

				if( m_pCurTemplate->m_UIType == UI_CONTROL_LISTBOX )
				{
					RefreshPropertyGrid( g_ListBoxTemplatePropertyDefine );
				}
				else if( m_pCurTemplate->m_UIType == UI_CONTROL_TEXTBOX )
				{
					RefreshPropertyGrid( g_TextBoxTemplatePropertyDefine );
				}
				else if( m_pCurTemplate->m_UIType == UI_CONTROL_HTMLTEXTBOX )
				{
					RefreshPropertyGrid( g_HtmlTextBoxTemplatePropertyDefine );
				}
				else if( m_pCurTemplate->m_UIType == UI_CONTROL_TREECONTROL )
				{
					RefreshPropertyGrid( g_TreeTemplatePropertyDefine );
				}
				else if( m_pCurTemplate->m_UIType == UI_CONTROL_QUESTTREECONTROL )
				{
					RefreshPropertyGrid( g_QuestTreeTemplatePropertyDefine );
				}
				else if( m_pCurTemplate->m_UIType == UI_CONTROL_LISTBOXEX )
				{
					RefreshPropertyGrid( g_ListBoxExTemplatePropertyDefine );
				}
			}
		}
	}
	else
	{
		if( dwIndex == 24 )
		{
			CFontSetDlg FontSetDlg;

			if( FontSetDlg.DoModal() == IDOK )
			{
				for( int i = 0; i < ( int )m_vecUIElementIndex.size(); i++ ) 
				{
					m_pCurTemplate->m_vecFontSetIndex[ m_vecUIElementIndex[ i ] ] = FontSetDlg.m_nSelectIndex;
				}
				RefreshPropertyGrid( g_ElementPropertyDefine );
			}
		}
	}

	ApplyTemplateChange();
}
