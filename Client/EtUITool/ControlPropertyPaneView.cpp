// ControlPropertyPaneView.cpp : implementation file
//
#include "stdafx.h"
#include "EtUI.h"
#include "EtUITool.h"
#include "EtUIToolDoc.h"
#include "EtUIToolView.h"
#include "ControlPropertyPaneView.h"
#include "GlobalValue.h"
#include "EtUIType.h"
#include "EtUIMan.h"
#include "LayoutView.h"
#include "HotkeyDlg.h"
#include "CustomControlSelect.h"
#include "../../Common/EtStringManager/EtUIXML.h"
#include "EtUITextureControl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern std::vector< std::string > g_vecCustomControlName;

PropertyGridBaseDefine g_DialogPropertyDefine[] = 
{
	{ "Dialog", "Horizontal Allign", CUnionValueProperty::Integer_Combo, "Justified|None|Left|Center|Right|LeftFixed|RightFixed", TRUE, false },
	{ "Dialog", "Vertical Allign", CUnionValueProperty::Integer_Combo, "Vertical Alignment|None|Top|Center|Bottom", TRUE, false },
	{ "Dialog", "X", CUnionValueProperty::Integer, "���̾�α��� X ��ġ", TRUE, false },
	{ "Dialog", "Y", CUnionValueProperty::Integer, "���̾�α��� Y ��ġ", TRUE, false },
	{ "Dialog", "Width", CUnionValueProperty::Integer, "���̾�α��� ����ũ��", TRUE, false },
	{ "Dialog", "Height", CUnionValueProperty::Integer, "���̾�α��� ����ũ��", TRUE, false },
	{ "Dialog", "Color", CUnionValueProperty::Vector4Color, "���̾�α� ����", TRUE, false },
	{ "Dialog", "Texture", CUnionValueProperty::String_FileOpen, "���̾�α� �ؽ���|Direct Draw Texture File|*.dds", TRUE, false },
	{ "Dialog", "Frame Left", CUnionValueProperty::Integer, "���̾�α� �ܰ� ������ ���� ũ��", TRUE, false },
	{ "Dialog", "Frame Top", CUnionValueProperty::Integer, "���̾�α� �ܰ� ������ ���� ũ��", TRUE, false },
	{ "Dialog", "Frame Right", CUnionValueProperty::Integer, "���̾�α� �ܰ� ������ ������ ũ��", TRUE, false },
	{ "Dialog", "Frame Bottom", CUnionValueProperty::Integer, "���̾�α� �ܰ� ������ �Ʒ��� ũ��", TRUE, false },
	{ "Dialog", "Modal Dialog", CUnionValueProperty::Boolean, "��� ���̾�α�", TRUE, false },
	{ "Dialog", "Sound", CUnionValueProperty::Boolean, "���̾�α� ����", TRUE, false },
	{ "Dialog", "Lock Scaling", CUnionValueProperty::Boolean, "�ػ󵵿� ���� �����Ϻ�ȭ�� ���´�\n�⺻���� False", TRUE, false },
	{ NULL },
};

enum emDIALOG_PROPERTY_INDEX
{
	dp_Horizontal_Allign,
	dp_Vertical_Allign,
	dp_X,
	dp_Y,
	dp_Width,
	dp_Height,
	dp_Color,
	dp_Texture,
	dp_Frame_Left,
	dp_Frame_Top,
	dp_Frame_Right,
	dp_Frame_Bottom,
	dp_Modal_Dialog,
	dp_Sound,
	dp_Lock_Scaling,
};

PropertyGridBaseDefine g_StaticPropertyDefine[] = 
{
	{ "Static", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "Static", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "Static", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "Static", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "Static", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Static", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Static", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "Static", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "Static", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "Static", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "Static", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "Static", "Text", CUnionValueProperty::String, "�ؽ�Ʈ", TRUE, true },
	{ "Static", "Horizontal Allign", CUnionValueProperty::Integer_Combo, "�¿�����|Left|Center|Right", TRUE, false },
	{ "Static", "Vertical Allign", CUnionValueProperty::Integer_Combo, "��������|Top|VCenter|Bottom", TRUE, false },
	{ "Static", "Shadow Font", CUnionValueProperty::Boolean, "Shadow Font", TRUE, false },
	{ "Static", "Font Color", CUnionValueProperty::Vector4Color, "��Ʈ ����", TRUE, false },
	{ "Static", "Shadow Font Color", CUnionValueProperty::Vector4Color, "��Ʈ �׸��ڻ���", TRUE, false },
	{ "Static", "Process Input", CUnionValueProperty::Boolean, "���콺���� ���� ��ǲ�� ó��", TRUE, false },
	{ "Static", "Texture Color", CUnionValueProperty::Vector4Color, "�ؽ�ó ���� ����", TRUE, false },
	{ "Static", "Auto Tooltip", CUnionValueProperty::Boolean, "�ؽ�Ʈ�� ������ ����� ...ǥ�ÿ� ���� �ڵ� ���", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ButtonPropertyDefine[] = 
{
	{ "Button", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "Button", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "Button", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "Button", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "Button", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Button", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Button", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "Button", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "Button", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "Button", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "Button", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "Button", "Text", CUnionValueProperty::String, "�ؽ�Ʈ", TRUE, true },
	//{ "Button", "Sound Type", CUnionValueProperty::Integer_COMBO, "����Ÿ��|NONE|OK|CANCEL|PAGE", TRUE, false },
	{ "Button", "Sound File", CUnionValueProperty::String_FileOpen, "���� ����|Wav File (*.wav)|*.wav", TRUE, false },
	{ "Button", "Button ID", CUnionValueProperty::Integer, "��ư ID", TRUE, false },
	{ "Button", "Auto Tooltip", CUnionValueProperty::Boolean, "�ؽ�Ʈ�� ������ ����� ...ǥ�ÿ� ���� �ڵ� ���", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_CheckBoxPropertyDefine[] = 
{
	{ "CheckBox", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "CheckBox", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "CheckBox", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "CheckBox", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "CheckBox", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "CheckBox", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "CheckBox", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "CheckBox", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "CheckBox", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "CheckBox", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "CheckBox", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "CheckBox", "Text", CUnionValueProperty::String, "�ؽ�Ʈ", TRUE, true },
	{ NULL },
};

PropertyGridBaseDefine g_RadioButtonPropertyDefine[] = 
{
	{ "RadioButton", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "RadioButton", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "RadioButton", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "RadioButton", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "RadioButton", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "RadioButton", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "RadioButton", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "RadioButton", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "RadioButton", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "RadioButton", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "RadioButton", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "RadioButton", "Text", CUnionValueProperty::String, "�ؽ�Ʈ", TRUE, true },
	{ "RadioButton", "Group", CUnionValueProperty::Integer, "������ư �׷�", TRUE, false },
	{ "RadioButton", "Tab ID", CUnionValueProperty::Integer, "�� ID", TRUE, false },
	{ "RadioButton", "Sound File", CUnionValueProperty::String_FileOpen, "���� ����|Wav File (*.wav)|*.wav", TRUE, false },
	{ "RadioButton", "Auto Tooltip", CUnionValueProperty::Boolean, "�ؽ�Ʈ�� ������ ����� ...ǥ�ÿ� ���� �ڵ� ���", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ComboBoxPropertyDefine[] = 
{
	{ "ComboBox", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "ComboBox", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "ComboBox", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "ComboBox", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "ComboBox", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ComboBox", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ComboBox", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "ComboBox", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "ComboBox", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "ComboBox", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "ComboBox", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "ComboBox", "Line Space", CUnionValueProperty::Integer, "�ٰ���", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_SliderPropertyDefine[] = 
{
	{ "Slider", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "Slider", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "Slider", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "Slider", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "Slider", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Slider", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Slider", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "Slider", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "Slider", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "Slider", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "Slider", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "Slider", "Min Value", CUnionValueProperty::Integer, "�����̴� �ּҰ�", TRUE, false },
	{ "Slider", "Max Value", CUnionValueProperty::Integer, "�����̴� �ִ밪", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_EditBoxPropertyDefine[] = 
{
	{ "EditBox", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "EditBox", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "EditBox", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "EditBox", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "EditBox", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "EditBox", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "EditBox", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "EditBox", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "EditBox", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "EditBox", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "EditBox", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "EditBox", "Password", CUnionValueProperty::Boolean, "Password", TRUE, false },
	{ "EditBox", "Max Chars", CUnionValueProperty::Integer, "Max Chars", TRUE, false },
	{ "EditBox", "Digit Only", CUnionValueProperty::Boolean, "Digit Only", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_IMEEditBoxPropertyDefine[] = 
{
	{ "IMEEditBox", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "IMEEditBox", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "IMEEditBox", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "IMEEditBox", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "IMEEditBox", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "IMEEditBox", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "IMEEditBox", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "IMEEditBox", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "IMEEditBox", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "IMEEditBox", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "IMEEditBox", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "IMEEditBox", "Indicator Size", CUnionValueProperty::Integer, "�ε������� ũ��", TRUE, false },
	{ "IMEEditBox", "Max Chars", CUnionValueProperty::Integer, "Max Chars", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ListBoxPropertyDefine[] = 
{
	{ "ListBox", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "ListBox", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "ListBox", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "ListBox", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "ListBox", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ListBox", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ListBox", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "ListBox", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "ListBox", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "ListBox", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "ListBox", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "ListBox", "Multi Select", CUnionValueProperty::Boolean, "��Ƽ����Ʈ ����?", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ScrollBarPropertyDefine[] = 
{
	{ "ScrollBar", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "ScrollBar", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "ScrollBar", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "ScrollBar", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "ScrollBar", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ScrollBar", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ScrollBar", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "ScrollBar", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "ScrollBar", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "ScrollBar", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "ScrollBar", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ NULL },
};

PropertyGridBaseDefine g_ProgressBarPropertyDefine[] = 
{
	{ "ProgressBar", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "ProgressBar", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "ProgressBar", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "ProgressBar", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "ProgressBar", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ProgressBar", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ProgressBar", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "ProgressBar", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "ProgressBar", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "ProgressBar", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "ProgressBar", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ NULL },
};

PropertyGridBaseDefine g_CustomPropertyDefine[] = 
{
	{ "Custom", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "Custom", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "Custom", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "Custom", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "Custom", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Custom", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Custom", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "Custom", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "Custom", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "Custom", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "Custom", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "Custom", "Color", CUnionValueProperty::Vector4Color, "���� ����", TRUE, true },
	{ NULL },
};

PropertyGridBaseDefine g_TextBoxPropertyDefine[] = 
{
	{ "TextBox", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "TextBox", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "TextBox", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "TextBox", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "TextBox", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "TextBox", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "TextBox", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "TextBox", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "TextBox", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "TextBox", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "TextBox", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "TextBox", "Vertical Scrollbar", CUnionValueProperty::Boolean, "��ũ�ѹ� ��� ����", TRUE, false },
	{ "TextBox", "Left Scrollbar", CUnionValueProperty::Boolean, "��ũ�ѹ� ���ʿ� ��ġ", TRUE, false },
	{ "TextBox", "Line Space", CUnionValueProperty::Integer, "�ٰ���", TRUE, false },
	{ "TextBox", "Horizontal Allign", CUnionValueProperty::Integer_Combo, "�¿�����|None|Left|Center|Right", TRUE, false },
	{ "TextBox", "Vertical Allign", CUnionValueProperty::Integer_Combo, "��������|None|Top|Center|Bottom", TRUE, false },
	{ "TextBox", "Variable Type", CUnionValueProperty::Integer_Combo, "����Ÿ��|None|Width|Height|Both", TRUE, false },
	{ "TextBox", "RollOver", CUnionValueProperty::Boolean, "�ѿ���", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_HtmlTextBoxPropertyDefine[] = 
{
	{ "HtmlTextBox", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "HtmlTextBox", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "HtmlTextBox", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "HtmlTextBox", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "HtmlTextBox", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "HtmlTextBox", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "HtmlTextBox", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "HtmlTextBox", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "HtmlTextBox", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "HtmlTextBox", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "HtmlTextBox", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "HtmlTextBox", "Vertical Scrollbar", CUnionValueProperty::Boolean, "��ũ�ѹ� ��� ����", TRUE, false },
	{ "HtmlTextBox", "Left Scrollbar", CUnionValueProperty::Boolean, "��ũ�ѹ� ���ʿ� ��ġ", TRUE, false },
	{ "HtmlTextBox", "Line Space", CUnionValueProperty::Integer, "�ٰ���", TRUE, false },
	{ "HtmlTextBox", "RollOver", CUnionValueProperty::Boolean, "�ѿ���", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_TextureControlPropertyDefine[] = 
{
	{ "TextureControl", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "TextureControl", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "TextureControl", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "TextureControl", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "TextureControl", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "TextureControl", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "TextureControl", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "TextureControl", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "TextureControl", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "TextureControl", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "TextureControl", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "TextureControl", "Text", CUnionValueProperty::String, "�ؽ�Ʈ", TRUE, true },
	{ "TextureControl", "Horizontal Allign", CUnionValueProperty::Integer_Combo, "�¿�����|Left|Center|Right", TRUE, false },
	{ "TextureControl", "Vertical Allign", CUnionValueProperty::Integer_Combo, "��������|Top|VCenter|Bottom", TRUE, false },
	{ "TextureControl", "Shadow Font", CUnionValueProperty::Boolean, "Shadow Font", TRUE, false },
	{ "TextureControl", "Font Color", CUnionValueProperty::Vector4Color, "��Ʈ ����", TRUE, false },
	{ "TextureControl", "Texture", CUnionValueProperty::String_FileOpen, "�ؽ���|Direct Draw Texture File|*.dds", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_TreePropertyDefine[] = 
{
	{ "Tree", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "Tree", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "Tree", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "Tree", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "Tree", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Tree", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Tree", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "Tree", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "Tree", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "Tree", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "Tree", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "Tree", "Vertical Scrollbar", CUnionValueProperty::Boolean, "��ũ�ѹ� ��� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_QuestTreePropertyDefine[] = 
{
	{ "QuestTree", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "QuestTree", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "QuestTree", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "QuestTree", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "QuestTree", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "QuestTree", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "QuestTree", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "QuestTree", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "QuestTree", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "QuestTree", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "QuestTree", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "QuestTree", "Vertical Scrollbar", CUnionValueProperty::Boolean, "��ũ�ѹ� ��� ����", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_AnimationPropertyDefine[] = 
{
	{ "Animation", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "Animation", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "Animation", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "Animation", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "Animation", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Animation", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "Animation", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "Animation", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "Animation", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "Animation", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "Animation", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ NULL },
};

PropertyGridBaseDefine g_LineEditBoxPropertyDefine[] = 
{
	{ "LineEidtBox", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "LineEidtBox", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "LineEidtBox", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "LineEidtBox", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "LineEidtBox", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "LineEidtBox", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "LineEidtBox", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "LineEidtBox", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "LineEidtBox", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "LineEidtBox", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "LineEidtBox", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ NULL },
};

PropertyGridBaseDefine g_LineIMEEditBoxPropertyDefine[] = 
{
	{ "IMEEditBox", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "IMEEditBox", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "IMEEditBox", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "IMEEditBox", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "IMEEditBox", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "IMEEditBox", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "IMEEditBox", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "IMEEditBox", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "IMEEditBox", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "IMEEditBox", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "IMEEditBox", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ NULL },
};

PropertyGridBaseDefine g_MovieControlPropertyDefine[] = 
{
	{ "MovieControl", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "MovieControl", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "MovieControl", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "MovieControl", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "MovieControl", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "MovieControl", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "MovieControl", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "MovieControl", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "MovieControl", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "MovieControl", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "MovieControl", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "MovieControl", "Alpha Border Size Hori", CUnionValueProperty::Integer, "���� �����׵θ�", TRUE, false },
	{ "MovieControl", "Alpha Border Size Vert", CUnionValueProperty::Integer, "���� �����׵θ�", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine g_ListBoxExPropertyDefine[] = 
{
	{ "ListBoxEx", "Template Name", CUnionValueProperty::String, "������� ���ø� �̸�", TRUE, false },
	{ "ListBoxEx", "Default Control", CUnionValueProperty::Boolean, "����Ʈ ��Ʈ��?", TRUE, false },
	{ "ListBoxEx", "X", CUnionValueProperty::Integer, "��Ʈ�� X ��ġ", TRUE, false },
	{ "ListBoxEx", "Y", CUnionValueProperty::Integer, "��Ʈ�� Y ��ġ", TRUE, false },
	{ "ListBoxEx", "Width", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ListBoxEx", "Height", CUnionValueProperty::Integer, "��Ʈ�� ����ũ��", TRUE, false },
	{ "ListBoxEx", "Control ID", CUnionValueProperty::String, "��Ʈ�� ID", TRUE, false },
	{ "ListBoxEx", "Init State", CUnionValueProperty::Integer, "�ʱ� ����\n0 : Normal, 1 : Hide, 2 : Disable", TRUE, false },
	{ "ListBoxEx", "Hotkey", CUnionValueProperty::String, "����Ű", TRUE, true },
	{ "ListBoxEx", "Custom Control ID", CUnionValueProperty::String, "Ŀ����", TRUE, true },
	{ "ListBoxEx", "Tooltip Index", CUnionValueProperty::Integer, "���� �ε���", TRUE, true },
	{ "ListBoxEx", "Multi Select", CUnionValueProperty::Boolean, "��Ƽ����Ʈ ����?", TRUE, false },
	{ "ListBoxEx", "UI File", CUnionValueProperty::String_FileOpen, "����Ʈ�ڽ� ������ UI|Eternity UI File (*.ui)|*.ui", TRUE, false },
	{ NULL },
};

PropertyGridBaseDefine *g_DefineList[] =
{
	g_StaticPropertyDefine,
	g_ButtonPropertyDefine,
	g_CheckBoxPropertyDefine,
	g_RadioButtonPropertyDefine,
	g_ComboBoxPropertyDefine, 
	g_SliderPropertyDefine,
	g_EditBoxPropertyDefine,
	g_IMEEditBoxPropertyDefine,
	g_ListBoxPropertyDefine,
	g_ScrollBarPropertyDefine,
	g_ProgressBarPropertyDefine,
	g_CustomPropertyDefine,
	g_TextBoxPropertyDefine,
	g_TextureControlPropertyDefine,
	g_HtmlTextBoxPropertyDefine,
	g_TreePropertyDefine,
	g_QuestTreePropertyDefine,
	g_AnimationPropertyDefine,
	g_LineEditBoxPropertyDefine,
	g_LineIMEEditBoxPropertyDefine,
	g_MovieControlPropertyDefine,
	g_ListBoxExPropertyDefine,
};

DWORD ColorToDWORD( D3DXCOLOR Color );
D3DXCOLOR DWORDToColor( DWORD dwColor );

// CControlPropertyPaneView

IMPLEMENT_DYNCREATE(CControlPropertyPaneView, CFormView)

CControlPropertyPaneView::CControlPropertyPaneView()
	: CFormView(CControlPropertyPaneView::IDD)
{
	m_bActivate = false;
	m_pDlgInfo = NULL;
	m_pCurControlProperty = NULL;
	m_nRefreshMode = -1;
}

CControlPropertyPaneView::~CControlPropertyPaneView()
{
	//SAFE_RELEASE_SPTR( m_hTextureControl );
}

void CControlPropertyPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CControlPropertyPaneView, CFormView)
	ON_WM_ERASEBKGND()
	ON_MESSAGE( XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify )
	ON_MESSAGE( UM_PROPERTYGRID_ONCUSTOM_DIALOG, OnCustomDialogGrid )
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CControlPropertyPaneView diagnostics

#ifdef _DEBUG
void CControlPropertyPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlPropertyPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlPropertyPaneView message handlers

BOOL CControlPropertyPaneView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

LRESULT CControlPropertyPaneView::OnGridNotify( WPARAM wParam, LPARAM lParam )
{
	return ProcessNotifyGrid( wParam, lParam );
}

LRESULT CControlPropertyPaneView::OnCustomDialogGrid( WPARAM wParam, LPARAM lParam )
{
	return ProcessCustomDialogGrid( wParam, lParam );
}

void CControlPropertyPaneView::RefreshDialogProperty()
{
	//if( m_nRefreshMode == 0 )
	//{
	//	return;
	//}

	m_pCurControlProperty = NULL;
	m_nRefreshMode = 0;
	RefreshPropertyGrid( g_DialogPropertyDefine );
}

void CControlPropertyPaneView::RefreshControlProperty( SUIControlProperty *pProperty )
{
	if( pProperty == NULL )
	{
		ResetPropertyGrid();
		return;
	}

	//if( ( m_nRefreshMode == 1 ) && ( m_pCurControlProperty == pProperty ) )
	//{
	//	return;
	//}

	//if( m_pCurControlProperty == pProperty )
	//{
	//	return;
	//}

	m_pCurControlProperty = pProperty;
	m_nRefreshMode = 1;

	RefreshPropertyGrid( g_DefineList[ m_pCurControlProperty->UIType ] );
}

void CControlPropertyPaneView::OnSetValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( !m_pDlgInfo || !pVariable )
	{
		return;
	}

	if( m_pCurControlProperty == NULL )
	{
		OnSetDialogValue( pVariable, dwIndex );
	}
	else
	{
		OnSetControlValue( pVariable, dwIndex );
	}
}

void CControlPropertyPaneView::OnSetDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	switch( dwIndex )
	{
	case dp_Horizontal_Allign:
		pVariable->SetVariable( m_pDlgInfo->AllignHori );
		break;
	case dp_Vertical_Allign:
		pVariable->SetVariable( m_pDlgInfo->AllignVert );
		break;
	case dp_X:
		pVariable->SetVariable( ( int )( m_pDlgInfo->DlgCoord.fX * DEFAULT_UI_SCREEN_WIDTH ) );
		break;
	case dp_Y:
		pVariable->SetVariable( ( int )( m_pDlgInfo->DlgCoord.fY * DEFAULT_UI_SCREEN_HEIGHT ) );
		break;
	case dp_Width:
		pVariable->SetVariable( ( int )( m_pDlgInfo->DlgCoord.fWidth * DEFAULT_UI_SCREEN_WIDTH ) );
		break;
	case dp_Height:
		pVariable->SetVariable( ( int )( m_pDlgInfo->DlgCoord.fHeight * DEFAULT_UI_SCREEN_HEIGHT ) );
		break;
	case dp_Color:
		pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pDlgInfo->dwDlgColor ) );
		break;
	case dp_Texture:
		pVariable->SetVariable( m_pDlgInfo->szDlgTextureName );
		break;
	case dp_Frame_Left:
		pVariable->SetVariable( m_pDlgInfo->nFrameLeft );
		break;
	case dp_Frame_Top:
		pVariable->SetVariable( m_pDlgInfo->nFrameTop );
		break;
	case dp_Frame_Right:
		pVariable->SetVariable( m_pDlgInfo->nFrameRight );
		break;
	case dp_Frame_Bottom:
		pVariable->SetVariable( m_pDlgInfo->nFrameBottom );
		break;
	case dp_Modal_Dialog:
		pVariable->SetVariable( m_pDlgInfo->bModalDialog );
		break;
	case dp_Sound:
		pVariable->SetVariable( m_pDlgInfo->bSound );
		break;
	case dp_Lock_Scaling:
		pVariable->SetVariable( m_pDlgInfo->bLockScalingByResolution );
		break;
	default:
		ASSERT(0&&"CControlPropertyPaneView::OnSetDialogValue");
		break;
	}
}

void CControlPropertyPaneView::OnSetControlValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	CEtUIToolView *pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	if( !pView ) return;

	CLayoutView *pLayoutView = ( CLayoutView * )pView->GetTabView( 0 );
	if( !pLayoutView ) return;

	const char *pTemplateName(NULL);

	switch( dwIndex )
	{
	case 0:
		pTemplateName = pLayoutView->GetTemplateName( m_pCurControlProperty );
		pVariable->SetVariable ( pTemplateName ? pTemplateName : "" );
		break;
	case 1:
		pVariable->SetVariable( m_pCurControlProperty->bDefaultControl ? true : false );
		break;
	case 2:
		pVariable->SetVariable( ( int )( m_pCurControlProperty->UICoord.fX * DEFAULT_UI_SCREEN_WIDTH ) );
		break;
	case 3:
		pVariable->SetVariable( ( int )( m_pCurControlProperty->UICoord.fY * DEFAULT_UI_SCREEN_HEIGHT ) );
		break;
	case 4:
		pVariable->SetVariable( ( int )( m_pCurControlProperty->UICoord.fWidth * DEFAULT_UI_SCREEN_WIDTH ) );
		break;
	case 5:
		pVariable->SetVariable( ( int )( m_pCurControlProperty->UICoord.fHeight * DEFAULT_UI_SCREEN_HEIGHT ) );
		break;
	case 6:
		pVariable->SetVariable( m_pCurControlProperty->szUIName );
		break;
	case 7:
		pVariable->SetVariable( m_pCurControlProperty->nInitState );
		break;
	case 8:
		pVariable->SetVariable( GetHotkeyString( m_pCurControlProperty->nHotKey ) );
		break;
	case 9:
		//pVariable->SetVariable( ( char * )g_vecCustomControlName[ m_pCurControlProperty->nCustomControlID ].c_str() );
		break;
	case 10:
		pVariable->SetVariable( m_pCurControlProperty->nTooltipStringIndex );
		break;
	}

	if( dwIndex == 11 )
	{
		int nStringIndex(0);
		bool bCase(true);

		switch( m_pCurControlProperty->UIType )
		{
		case UI_CONTROL_STATIC:			nStringIndex = m_pCurControlProperty->StaticProperty.nStringIndex;			break;
		case UI_CONTROL_BUTTON:			nStringIndex = m_pCurControlProperty->ButtonProperty.nStringIndex;			break;
		case UI_CONTROL_CHECKBOX:		nStringIndex = m_pCurControlProperty->CheckBoxProperty.nStringIndex;		break;
		case UI_CONTROL_RADIOBUTTON:	nStringIndex = m_pCurControlProperty->RadioButtonProperty.nStringIndex;		break;
		case UI_CONTROL_TEXTURECONTROL: nStringIndex = m_pCurControlProperty->TextureControlProperty.nStringIndex;	break;
		default: bCase = false; break;
		}

		if( EtInterface::xml::IsValid() && bCase )
		{
			if( nStringIndex > 0 )
			{
				char szTemp[2048]={0};
				WideCharToMultiByte( CP_ACP, 0, EtInterface::xml::GetString( CEtUIXML::idCategory1, nStringIndex ), -1, szTemp, 2048, NULL, NULL);
				pVariable->SetVariable( szTemp );
			}
			else
			{
				pVariable->SetVariable( "" );
			}
		}
	}

	switch( m_pCurControlProperty->UIType )
	{
	case UI_CONTROL_STATIC:
		switch( dwIndex )
		{
		case 12:
			pVariable->SetVariable( ( int )( m_pCurControlProperty->StaticProperty.dwFontFormat & 0x03 ) );
			break;
		case 13:
			pVariable->SetVariable( ( int )( ( m_pCurControlProperty->StaticProperty.dwFontFormat & 0xc ) >> 2 ) );
			break;
		case 14:
			pVariable->SetVariable( m_pCurControlProperty->StaticProperty.bShadowFont ? true : false );
			break;
		case 15:
			pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurControlProperty->StaticProperty.dwFontColor ) );
			break;
		case 16:
			pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurControlProperty->StaticProperty.dwShadowFontColor ) );
			break;
		case 17:
			pVariable->SetVariable( m_pCurControlProperty->StaticProperty.bProcessInput ? true : false );
			break;
		case 18:
			pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurControlProperty->StaticProperty.dwTextureColor ) );
			break;
		case 19:
			pVariable->SetVariable( m_pCurControlProperty->StaticProperty.bAutoTooltip ? true : false );
			break;
		}
		break;
	case UI_CONTROL_BUTTON:
		if( dwIndex == 12 )
		{
			pVariable->SetVariable( m_pCurControlProperty->ButtonProperty.szSoundFileName );
		}
		else if( dwIndex == 13 )
		{
			pVariable->SetVariable( m_pCurControlProperty->ButtonProperty.nButtonID );
		}
		else if( dwIndex == 14 )
		{
			pVariable->SetVariable( m_pCurControlProperty->ButtonProperty.bAutoTooltip ? true : false );
		}
		break;
	case UI_CONTROL_RADIOBUTTON:
		if( dwIndex == 12 )
		{
			pVariable->SetVariable( m_pCurControlProperty->RadioButtonProperty.nButtonGroup );
		}
		else if( dwIndex == 13 )
		{
			pVariable->SetVariable( m_pCurControlProperty->RadioButtonProperty.nTabID );
		}
		else if( dwIndex == 14 )
		{
			pVariable->SetVariable( m_pCurControlProperty->RadioButtonProperty.szSoundFileName );
		}
		else if( dwIndex == 15 )
		{
			pVariable->SetVariable( m_pCurControlProperty->RadioButtonProperty.bAutoTooltip ? true : false );
		}
		break;
	case UI_CONTROL_COMBOBOX:
		if( dwIndex == 11 )
		{
			pVariable->SetVariable( int(m_pCurControlProperty->ComboBoxProperty.fLineSpace * DEFAULT_UI_SCREEN_HEIGHT) );
		}
		break;
	case UI_CONTROL_SLIDER:
		if( dwIndex == 11 )
		{
			pVariable->SetVariable( m_pCurControlProperty->SliderProperty.nMinValue );
		}
		else if( dwIndex == 12 )
		{
			pVariable->SetVariable( m_pCurControlProperty->SliderProperty.nMaxValue );
		}
		break;
	case UI_CONTROL_LISTBOX:
		if( dwIndex == 11 )
		{
			pVariable->SetVariable( m_pCurControlProperty->ListBoxProperty.bMultiSelect ? true : false );
		}
		break;
	case UI_CONTROL_CUSTOM:
		if( dwIndex == 11 )
		{
			pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurControlProperty->CustomProperty.dwColor ) );
		}
		break;
	case UI_CONTROL_EDITBOX:
		switch( dwIndex )
		{
		case 11:
			pVariable->SetVariable( m_pCurControlProperty->EditBoxProperty.bPassword ? true : false );
			break;
		case 12:
			pVariable->SetVariable( (int)m_pCurControlProperty->EditBoxProperty.dwMaxChars-2 );
			break;
		case 13:
			pVariable->SetVariable( m_pCurControlProperty->EditBoxProperty.bOnlyDigit ? true : false );
		    break;
		}
		break;
	case UI_CONTROL_IMEEDITBOX:
		if( dwIndex == 11 )
		{
			pVariable->SetVariable( ( int )( m_pCurControlProperty->IMEEditBoxProperty.fIndicatorSize * DEFAULT_UI_SCREEN_WIDTH ) );
		}
		else if( dwIndex == 12 )
		{
			pVariable->SetVariable( (int)m_pCurControlProperty->IMEEditBoxProperty.dwMaxChars-2 );
		}
		break;
	case UI_CONTROL_TEXTBOX:
		switch( dwIndex )
		{
		case 11:
			pVariable->SetVariable( m_pCurControlProperty->TextBoxProperty.bVerticalScrollBar );
			break;
		case 12:
			pVariable->SetVariable( m_pCurControlProperty->TextBoxProperty.bLeftScrollBar );
			break;
		case 13:
			pVariable->SetVariable( (int)(m_pCurControlProperty->TextBoxProperty.fLineSpace * DEFAULT_UI_SCREEN_HEIGHT) );
			break;
		case 14:
			pVariable->SetVariable( m_pCurControlProperty->TextBoxProperty.AllignHori );
			break;
		case 15:
			pVariable->SetVariable( m_pCurControlProperty->TextBoxProperty.AllignVert );
			break;
		case 16:
			pVariable->SetVariable( m_pCurControlProperty->TextBoxProperty.VariableType );
			break;
		case 17:
			pVariable->SetVariable( m_pCurControlProperty->TextBoxProperty.bRollOver );
			break;
		}
		break;
	case UI_CONTROL_TEXTURECONTROL:
		switch( dwIndex )
		{
		case 12:
			pVariable->SetVariable( ( int )( m_pCurControlProperty->TextureControlProperty.dwFontFormat & 0x03 ) );
			break;
		case 13:
			pVariable->SetVariable( ( int )( ( m_pCurControlProperty->TextureControlProperty.dwFontFormat & 0xc ) >> 2 ) );
			break;
		case 14:
			pVariable->SetVariable( m_pCurControlProperty->TextureControlProperty.bShadowFont ? true : false );
			break;
		case 15:
			pVariable->SetVariable( ( D3DXVECTOR4 )DWORDToColor( m_pCurControlProperty->TextureControlProperty.dwFontColor ) );
			break;
		case 16:
			pVariable->SetVariable( m_pCurControlProperty->TextureControlProperty.szTextureFileName );
			break;
		}
		break;
	case UI_CONTROL_HTMLTEXTBOX:
		switch( dwIndex )
		{
		case 11:
			pVariable->SetVariable( m_pCurControlProperty->HtmlTextBoxProperty.bVerticalScrollBar );
			break;
		case 12:
			pVariable->SetVariable( m_pCurControlProperty->HtmlTextBoxProperty.bLeftScrollBar );
			break;
		case 13:
			pVariable->SetVariable( int(m_pCurControlProperty->HtmlTextBoxProperty.fLineSpace * DEFAULT_UI_SCREEN_HEIGHT) );
			break;
		case 14:
			pVariable->SetVariable( m_pCurControlProperty->HtmlTextBoxProperty.bRollOver );
			break;
		}
		break;
	case UI_CONTROL_TREECONTROL:
		switch( dwIndex )
		{
		case 11:
			pVariable->SetVariable( m_pCurControlProperty->TreeControlProperty.bVerticalScrollBar );
			break;
		}
		break;
	case UI_CONTROL_QUESTTREECONTROL:
		switch( dwIndex )
		{
		case 11:
			pVariable->SetVariable( m_pCurControlProperty->QuestTreeControlProperty.bVerticalScrollBar );
			break;
		}
		break;
	case UI_CONTROL_LISTBOXEX:
		switch( dwIndex )
		{
		case 11:
			pVariable->SetVariable( m_pCurControlProperty->ListBoxExProperty.bMultiSelect ? true : false );
			break;
		case 12:
			pVariable->SetVariable( m_pCurControlProperty->ListBoxExProperty.szListBoxItemUIFileName );
			break;
		}
		break;
	case UI_CONTROL_MOVIECONTROL:
		switch( dwIndex )
		{

		case 11:
			pVariable->SetVariable( ( int )( m_pCurControlProperty->MovieControlPropery.fAlphaBorderHori * DEFAULT_UI_SCREEN_WIDTH ) );
			break;
		case 12:
			pVariable->SetVariable( ( int )( m_pCurControlProperty->MovieControlPropery.fAlphaBorderVert * DEFAULT_UI_SCREEN_HEIGHT ) );
			break;
		}
		break;
	}
}

void CControlPropertyPaneView::OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( !m_pDlgInfo || !pVariable )
	{
		return;
	}

	if( m_pCurControlProperty == NULL )
	{
		OnChangeDialogValue( pVariable, dwIndex );
	}
	else
	{
		OnChangeControlValue( pVariable, dwIndex );
	}

	CEtUIToolView *pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	if( !pView ) return;

	CLayoutView *pLayoutView = ( CLayoutView * )pView->GetActiveTabView();
	if( !pLayoutView ) return;

	if( m_pCurControlProperty )
	{
		pLayoutView->ReinitAllControl();
	}
	else
	{
		pLayoutView->RefreshLayoutDlg();
	}
}

void CControlPropertyPaneView::OnChangeDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	char szFileName[ _MAX_PATH ]={0};
	
	switch( dwIndex )
	{
	case dp_Horizontal_Allign:
		m_pDlgInfo->AllignHori = ( UIAllignHoriType )pVariable->GetVariableInt();
		break;
	case dp_Vertical_Allign:
		m_pDlgInfo->AllignVert = ( UIAllignVertType )pVariable->GetVariableInt();
		break;
	case dp_X:
		m_pDlgInfo->DlgCoord.fX = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
		break;
	case dp_Y:
		m_pDlgInfo->DlgCoord.fY = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
		break;
	case dp_Width:
		m_pDlgInfo->DlgCoord.fWidth = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
		break;
	case dp_Height:
		m_pDlgInfo->DlgCoord.fHeight = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
		break;
	case dp_Color:
		m_pDlgInfo->dwDlgColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
		break;
	case dp_Texture:
		{
			_GetFullFileName( szFileName, _countof(szFileName), pVariable->GetVariableString() );
			strcpy_s( m_pDlgInfo->szDlgTextureName, _MAX_PATH, szFileName );
			pVariable->SetVariable( m_pDlgInfo->szDlgTextureName );
		}
		break;
	case dp_Frame_Left:
		m_pDlgInfo->nFrameLeft = pVariable->GetVariableInt();
		break;
	case dp_Frame_Top:
		m_pDlgInfo->nFrameTop = pVariable->GetVariableInt();
		break;
	case dp_Frame_Right:
		m_pDlgInfo->nFrameRight = pVariable->GetVariableInt();
		break;
	case dp_Frame_Bottom:
		m_pDlgInfo->nFrameBottom = pVariable->GetVariableInt();
		break;
	case dp_Modal_Dialog:
		m_pDlgInfo->bModalDialog = pVariable->GetVariableBool();
		break;
	case dp_Sound:
		m_pDlgInfo->bSound = pVariable->GetVariableBool();
		break;
	case dp_Lock_Scaling:
		m_pDlgInfo->bLockScalingByResolution = pVariable->GetVariableBool();
		break;
	default:
		ASSERT(0&&"CControlPropertyPaneView::OnChangeDialogValue");
		break;
	}
}

void CControlPropertyPaneView::OnChangeControlValue( CUnionValueProperty *pVariable, DWORD dwIndex, bool bNotifyChangeToLayoutView, SUIControlProperty *pProperty )
{
	CEtUIToolView *pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	if( !pView ) return;

	CLayoutView *pLayoutView = ( CLayoutView * )pView->GetActiveTabView();
	if( !pLayoutView ) return;

	// bNotifyChangeToLayoutView���� false��� ������ ��Ƽ����Ʈ �� ������ ��Ʈ�ѿ� ���� �Ӽ��� �����϶�� �̾߱��.
	SUIControlProperty *pOrigCurControlProperty = m_pCurControlProperty;
	if( bNotifyChangeToLayoutView == false )
	{
		m_pCurControlProperty = pProperty;
	}

	switch( dwIndex )
	{
	case 1:
		m_pCurControlProperty->bDefaultControl = pVariable->GetVariableBool();
		pLayoutView->SetDefaultControl( m_pCurControlProperty );
		break;
	case 2:
		m_pCurControlProperty->UICoord.fX = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
		break;
	case 3:
		m_pCurControlProperty->UICoord.fY = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
		break;
	case 4:
		m_pCurControlProperty->UICoord.fWidth = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
		break;
	case 5:
		m_pCurControlProperty->UICoord.fHeight = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
		break;
	case 6:
		strcpy_s( m_pCurControlProperty->szUIName, 32, pVariable->GetVariableString() );
		break;
	case 7:
		m_pCurControlProperty->nInitState = pVariable->GetVariableInt();
		break;
	}

	switch( m_pCurControlProperty->UIType )
	{
	case UI_CONTROL_STATIC:
		switch( dwIndex )
		{
		case 12:
			m_pCurControlProperty->StaticProperty.dwFontFormat &= 0xfffffffc;
			m_pCurControlProperty->StaticProperty.dwFontFormat |= pVariable->GetVariableInt();
			break;
		case 13:
			m_pCurControlProperty->StaticProperty.dwFontFormat &= 0xfffffff3;
			m_pCurControlProperty->StaticProperty.dwFontFormat |= pVariable->GetVariableInt() << 2;
			break;
		case 14:
			m_pCurControlProperty->StaticProperty.bShadowFont = pVariable->GetVariableBool();
			break;
		case 15:
			m_pCurControlProperty->StaticProperty.dwFontColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
			break;
		case 16:
			m_pCurControlProperty->StaticProperty.dwShadowFontColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
			break;
		case 17:
			m_pCurControlProperty->StaticProperty.bProcessInput = pVariable->GetVariableBool();
			break;
		case 18:
			m_pCurControlProperty->StaticProperty.dwTextureColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
			break;
		case 19:
			m_pCurControlProperty->StaticProperty.bAutoTooltip = pVariable->GetVariableBool();
			break;
		}
		break;
	case UI_CONTROL_BUTTON:
		if( dwIndex == 12 )
		{
			char szFileName[128];
			_GetFullFileName( szFileName, _countof(szFileName), pVariable->GetVariableString() );
			if( strlen(szFileName) > 31 )
			{
				MessageBox( "���� �����̸��� 31�ڸ� ���� �� �����ϴ�.\n���� ���� ������ ����մϴ�." );
				break;
			}
			sprintf_s( m_pCurControlProperty->ButtonProperty.szSoundFileName, _countof(m_pCurControlProperty->ButtonProperty.szSoundFileName), szFileName);
			pVariable->SetVariable( m_pCurControlProperty->ButtonProperty.szSoundFileName );
		}
		else if( dwIndex == 13 )
		{
			m_pCurControlProperty->ButtonProperty.nButtonID = pVariable->GetVariableInt();
		}
		else if( dwIndex == 14 )
		{
			m_pCurControlProperty->ButtonProperty.bAutoTooltip = pVariable->GetVariableBool();
		}
		break;
	case UI_CONTROL_RADIOBUTTON:
		if( dwIndex == 12 )
		{
			m_pCurControlProperty->RadioButtonProperty.nButtonGroup = pVariable->GetVariableInt();
		}
		else if( dwIndex == 13 )
		{
			m_pCurControlProperty->RadioButtonProperty.nTabID = pVariable->GetVariableInt();
		}
		else if( dwIndex == 14 )
		{
			char szFileName[128];
			_GetFullFileName( szFileName, _countof(szFileName), pVariable->GetVariableString() );
			if( strlen(szFileName) > 31 )
			{
				MessageBox( "���� �����̸��� 31�ڸ� ���� �� �����ϴ�.\n���� ���� ������ ����մϴ�." );
				break;
			}
			sprintf_s( m_pCurControlProperty->RadioButtonProperty.szSoundFileName, _countof(m_pCurControlProperty->RadioButtonProperty.szSoundFileName), szFileName);
			pVariable->SetVariable( m_pCurControlProperty->RadioButtonProperty.szSoundFileName );
		}
		else if( dwIndex == 15 )
		{
			m_pCurControlProperty->RadioButtonProperty.bAutoTooltip = pVariable->GetVariableBool();
		}
		break;
	case UI_CONTROL_COMBOBOX:
		if( dwIndex == 11 )
		{
			m_pCurControlProperty->ComboBoxProperty.fLineSpace = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_HEIGHT;
		}
		break;
	case UI_CONTROL_SLIDER:
		if( dwIndex == 11 )
		{
			m_pCurControlProperty->SliderProperty.nMinValue = pVariable->GetVariableInt();
		}
		else if( dwIndex == 12 )
		{
			m_pCurControlProperty->SliderProperty.nMaxValue = pVariable->GetVariableInt();
		}
		break;
	case UI_CONTROL_LISTBOX:
		if( dwIndex == 11 )
		{
			m_pCurControlProperty->ListBoxProperty.bMultiSelect = pVariable->GetVariableBool();
		}
		break;
	case UI_CONTROL_EDITBOX:
		switch( dwIndex )
		{
		case 11:
			m_pCurControlProperty->EditBoxProperty.bPassword = pVariable->GetVariableBool();
			break;
		case 12:
			{
				DWORD dwMaxChars = pVariable->GetVariableInt();
				dwMaxChars += 2;
				if( dwMaxChars > 256 ) dwMaxChars = 256;
				m_pCurControlProperty->EditBoxProperty.dwMaxChars = dwMaxChars;
			}
			break;
		case 13:
			m_pCurControlProperty->EditBoxProperty.bOnlyDigit = pVariable->GetVariableBool();
		    break;
		}
		break;
	case UI_CONTROL_IMEEDITBOX:
		if( dwIndex == 11 )
		{
			m_pCurControlProperty->IMEEditBoxProperty.fIndicatorSize = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
		}
		else if( dwIndex == 12 )
		{
			DWORD dwMaxChars = pVariable->GetVariableInt();
			dwMaxChars += 2;
			if( dwMaxChars > 256 ) dwMaxChars = 256;
			m_pCurControlProperty->IMEEditBoxProperty.dwMaxChars = dwMaxChars;
		}
		break;
	case UI_CONTROL_CUSTOM:
		if( dwIndex == 11 )
		{
			m_pCurControlProperty->CustomProperty.dwColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
		}
		break;
	case UI_CONTROL_TEXTBOX:
		switch( dwIndex )
		{
		case 11:
			m_pCurControlProperty->TextBoxProperty.bVerticalScrollBar = pVariable->GetVariableBool();
			break;
		case 12:
			m_pCurControlProperty->TextBoxProperty.bLeftScrollBar = pVariable->GetVariableBool();
			break;
		case 13:
			m_pCurControlProperty->TextBoxProperty.fLineSpace = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_HEIGHT;
			break;
		case 14:
			m_pCurControlProperty->TextBoxProperty.AllignHori = (UIAllignHoriType)pVariable->GetVariableInt();
			break;
		case 15:
			m_pCurControlProperty->TextBoxProperty.AllignVert = (UIAllignVertType)pVariable->GetVariableInt();
			break;
		case 16:
			m_pCurControlProperty->TextBoxProperty.VariableType = (UITextBoxVariableType)pVariable->GetVariableInt();
			break;
		case 17:
			m_pCurControlProperty->TextBoxProperty.bRollOver = pVariable->GetVariableBool();
			break;
		}
		break;
	case UI_CONTROL_TEXTURECONTROL:
		switch( dwIndex )
		{
		case 12:
			m_pCurControlProperty->TextureControlProperty.dwFontFormat &= 0xfffffffc;
			m_pCurControlProperty->TextureControlProperty.dwFontFormat |= pVariable->GetVariableInt();
			break;
		case 13:
			m_pCurControlProperty->TextureControlProperty.dwFontFormat &= 0xfffffff3;
			m_pCurControlProperty->TextureControlProperty.dwFontFormat |= pVariable->GetVariableInt() << 2;
			break;
		case 14:
			m_pCurControlProperty->TextureControlProperty.bShadowFont = pVariable->GetVariableBool();
			break;
		case 15:
			m_pCurControlProperty->TextureControlProperty.dwFontColor = ColorToDWORD( ( D3DXCOLOR )pVariable->GetVariableVector4() );
			break;
		case 16:
			{
				// �ڵ� �� ¥���� ���� ���̾�α� �������� �Ʒ�ó�� ���ϸ� ���ϴ� �� �˾Ҵ�. �̸��̸� �˾Ƶ���...
				//_GetFullFileName( szFileName, pVariable->GetVariableString() );

				// ������ ���⼭ �ؽ�ó ���� ����.
				TCHAR szDrive[256], szDir[256], szName[256], szExt[256];
				SPLITPATH_S(pVariable->GetVariableString(), szDrive, szDir, szName, szExt);
				if( strlen(szName) > 27 )
				{
					MessageBox( "�ؽ�ó�����̸��� 32�ڸ� ���� �� �����ϴ�.\n�ؽ�ó ������ ����մϴ�." );
					break;
				}
				strcat_s(szName, _countof(szName), szExt);
				sprintf_s(m_pCurControlProperty->TextureControlProperty.szTextureFileName, _countof(m_pCurControlProperty->TextureControlProperty.szTextureFileName), szName);
				pVariable->SetVariable(m_pCurControlProperty->TextureControlProperty.szTextureFileName);

				// ���� �̷� �ӽ� �ؽ�ó �ڵ� �Ⱦ���.
				//SAFE_RELEASE_SPTR( m_hTextureControl );
				//m_hTextureControl = LoadResource( szFileName, RT_TEXTURE );
				//CEtUITextureControl *pControl = (CEtUITextureControl*)pLayoutView->GetSelectedControl();
				//if( pControl )
				//	pControl->SetTexture( m_hTextureControl, 0, 0, m_hTextureControl->Width(), m_hTextureControl->Height() );
			}
			break;
		}
		break;
	case UI_CONTROL_HTMLTEXTBOX:
		switch( dwIndex )
		{
		case 11:
			m_pCurControlProperty->HtmlTextBoxProperty.bVerticalScrollBar = pVariable->GetVariableBool();
			break;
		case 12:
			m_pCurControlProperty->HtmlTextBoxProperty.bLeftScrollBar = pVariable->GetVariableBool();
			break;
		case 13:
			m_pCurControlProperty->HtmlTextBoxProperty.fLineSpace = pVariable->GetVariableInt() / (float)DEFAULT_UI_SCREEN_HEIGHT;
			break;
		case 14:
			m_pCurControlProperty->HtmlTextBoxProperty.bRollOver = pVariable->GetVariableBool();
			break;
		}
		break;
	case UI_CONTROL_TREECONTROL:
		switch( dwIndex )
		{
		case 11:
			m_pCurControlProperty->TreeControlProperty.bVerticalScrollBar = pVariable->GetVariableBool();
			break;
		}
		break;
	case UI_CONTROL_QUESTTREECONTROL:
		switch( dwIndex )
		{
		case 11:
			m_pCurControlProperty->QuestTreeControlProperty.bVerticalScrollBar = pVariable->GetVariableBool();
			break;
		}
		break;
	case UI_CONTROL_MOVIECONTROL:
		switch( dwIndex )
		{
		case 11:
			m_pCurControlProperty->MovieControlPropery.fAlphaBorderHori = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_WIDTH;
			break;
		case 12:
			m_pCurControlProperty->MovieControlPropery.fAlphaBorderVert = pVariable->GetVariableInt() / ( float )DEFAULT_UI_SCREEN_HEIGHT;
			break;
		}
		break;
	case UI_CONTROL_LISTBOXEX:
		switch( dwIndex )
		{
		case 11:
			m_pCurControlProperty->ListBoxExProperty.bMultiSelect = pVariable->GetVariableBool();
			break;
		case 12:
			char szFileName[128];
			_GetFullFileName( szFileName, _countof(szFileName), pVariable->GetVariableString() );
			if( strlen(szFileName) > 31 )
			{
				MessageBox( "UI�����̸��� 32�ڸ� ���� �� �����ϴ�.\nUI���� ������ ����մϴ�." );
				break;
			}
			sprintf_s( m_pCurControlProperty->ListBoxExProperty.szListBoxItemUIFileName, _countof(m_pCurControlProperty->ListBoxExProperty.szListBoxItemUIFileName), szFileName);
			pVariable->SetVariable( m_pCurControlProperty->ListBoxExProperty.szListBoxItemUIFileName );
			break;
		}
		break;
	}

	if( bNotifyChangeToLayoutView == false )
	{
		// Cur����.
		m_pCurControlProperty = pOrigCurControlProperty;
	}
	else
	{
		// ���̾ƿ��信 ������ �˸���.
		// ���̾ƿ���� ��Ƽ����Ʈ�� �Ǿ����� ��� �ٸ� ��Ʈ�ѿ��� �� ���� ������ ���̴�.
		pLayoutView->ChangeValueSelectedControl( pVariable, dwIndex );
	}
}

void CControlPropertyPaneView::OnSelectChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
}

void CControlPropertyPaneView::OnCustomDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	CEtUIToolView *pView(NULL);
	CLayoutView *pLayoutView(NULL);

	pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	ASSERT( pView&&"CControlPropertyPaneView::OnCustomDialogValue, pView is NULL!" );
	pLayoutView = ( CLayoutView * )pView->GetActiveTabView();
	ASSERT( pLayoutView&&"CControlPropertyPaneView::OnCustomDialogValue, pLayoutView is NULL!" );

	if( m_pCurControlProperty )
	{
		if( dwIndex == 8 )
		{
			CHotkeyDlg HotkeyDlg;

			HotkeyDlg.m_nHotkey = GetHotkeyIndex( m_pCurControlProperty->nHotKey );
			if( HotkeyDlg.DoModal() == IDOK )
			{
				m_pCurControlProperty->nHotKey = HotkeyDlg.m_nHotkey;
				RefreshPropertyGrid( g_DefineList[ m_pCurControlProperty->UIType ] );
			}
		}
		else if( dwIndex == 9 )
		{
			CCustomControlSelect ControlSelectDlg;

			if( ControlSelectDlg.DoModal() == IDOK )
			{
				m_pCurControlProperty->nCustomControlID = ControlSelectDlg.m_nCurSel;
				RefreshPropertyGrid( g_DefineList[ m_pCurControlProperty->UIType ] );
			}
		}
		else if( dwIndex == 10 )
		{
			if( m_StringSelectDlg.DoModal() == IDOK )
			{
				m_pCurControlProperty->nTooltipStringIndex = m_StringSelectDlg.GetStringIndex();
				RefreshPropertyGrid( g_DefineList[ m_pCurControlProperty->UIType ] );
			}
		}
		else if( dwIndex == 11 )
		{
			if( m_StringSelectDlg.DoModal() == IDOK )
			{
				switch( m_pCurControlProperty->UIType )
				{
				case UI_CONTROL_STATIC:
					m_pCurControlProperty->StaticProperty.nStringIndex = m_StringSelectDlg.GetStringIndex();
					RefreshPropertyGrid( g_StaticPropertyDefine );
					break;
				case UI_CONTROL_BUTTON:
					m_pCurControlProperty->ButtonProperty.nStringIndex = m_StringSelectDlg.GetStringIndex();
					RefreshPropertyGrid( g_ButtonPropertyDefine );
					break;
				case UI_CONTROL_CHECKBOX:
					m_pCurControlProperty->CheckBoxProperty.nStringIndex = m_StringSelectDlg.GetStringIndex();
					RefreshPropertyGrid( g_CheckBoxPropertyDefine );
					break;
				case UI_CONTROL_RADIOBUTTON:
					m_pCurControlProperty->RadioButtonProperty.nStringIndex = m_StringSelectDlg.GetStringIndex();
					RefreshPropertyGrid( g_RadioButtonPropertyDefine );
					break;
				case UI_CONTROL_TEXTURECONTROL:
					m_pCurControlProperty->TextureControlProperty.nStringIndex = m_StringSelectDlg.GetStringIndex();
					RefreshPropertyGrid( g_TextureControlPropertyDefine );
					break;
				}
			}
		}
	}

	if( m_pCurControlProperty )
	{
		pLayoutView->ReinitAllControl();
	}
	else
	{
		pLayoutView->RefreshLayoutDlg();
	}
}

void CControlPropertyPaneView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	if( m_bActivate == true ) return;
	m_bActivate = true;

	CRect rcRect;
	GetClientRect( &rcRect );
	m_PropertyGrid.Create( rcRect, this, 0 );
	m_PropertyGrid.SetCustomColors( RGB(200, 200, 200), 0, RGB(182, 210, 189), RGB(247, 243, 233), 0 );
}

void CControlPropertyPaneView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( m_PropertyGrid ) {
		m_PropertyGrid.SetWindowPos( NULL, 0, 0, cx, cy, SWP_FRAMECHANGED );
	}
}