#pragma once

// Note : UI ��Ʈ�ѵ��� �߻���Ű�� �޼����Դϴ�.
//		�޼����� �ʿ信 ���� ���� ��ġġ �ʵ��� �߰��Ͻø� �˴ϴ�.
//
#define DEFAULT_UI_WIDESCREEN_WIDTH			1366
#define DEFAULT_UI_SCREEN_WIDTH				1024
#define DEFAULT_UI_SCREEN_HEIGHT			768
#define DEFAULT_UI_SCREEN_RATIO				1.3333333f

#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_BUTTON_DOUBLE_CLICKED                0x0102
#define EVENT_BUTTON_RCLICKED                0x0103
#define EVENT_BUTTON_RDOUBLE_CLICKED                0x0104
#define EVENT_BUTTON_ONCLICK                0x0105
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_COMBOBOX_DROPDOWN_OPENED	    0x0202
#define EVENT_COMBOBOX_DROPDOWN_CLOSED	    0x0203
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_EDITBOX_STRING                0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_EDITBOX_ESCAPE                0x0603
#define EVENT_EDITBOX_KEYUP	                0x0604
#define EVENT_EDITBOX_KEYDOWN               0x0605
#define EVENT_EDITBOX_SHIFT_KEYUP           0x0606
#define EVENT_EDITBOX_SHIFT_KEYDOWN         0x0607
#define EVENT_EDITBOX_LCTRL_KEYUP			0x0608
#define EVENT_EDITBOX_LCTRL_KEYDOWN			0x0609
#define EVENT_EDITBOX_FOCUS					0x0610
#define EVENT_EDITBOX_RELEASEFOCUS			0x0611
#define EVENT_EDITBOX_IME_CHANGE			0x0612
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.
#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_SELECTION_END         0x0703

#define EVENT_TEXTBOX_SELECTION				0x0801
#define EVENT_TREECONTROL_SELECTION			0x0901
#define EVENT_TREECONTROL_SELECTION_END		0x0902

#define EVENT_FADE_COMPLETE					0x0A01
#define EVENT_DIALOG_SHOW					0x0A02
#define EVENT_DIALOG_HIDE					0x0A03

//#ifdef PRE_ADD_UICONTROL_MENULIST
#define EVENT_MENULIST_SELECTION			0x0B01
// #endif // PRE_ADD_UICONTROL_MENULIST

// Note : �������� Ÿ���Դϴ�.
//		�����찡 ��µǴ� ������ �޼��� ó�� ������ ������ �ݴϴ�.
//		�׸��� CHILDŸ���� �θ� ������ �����찡 ���ϴ� Ÿ�Դϴ�.
//		�׷��ٰ� �ݵ�� CHILD�� �ؾ��ϴ°� �ƴմϴ�. �ʿ信 ���� �����Ͻø� �˴ϴ�.
//
enum UI_DIALOG_TYPE
{
	// Note : �ֻ��� ������ Ÿ��
	//
	UI_TYPE_BOTTOM_MSG,
	UI_TYPE_BOTTOM,
	UI_TYPE_TOP_MSG,
	UI_TYPE_TOP,
	UI_TYPE_FOCUS,
	UI_TYPE_MODAL,
	UI_TYPE_MOST_TOP,

	// Note : �ڽ� ������ Ÿ��
	//
	UI_TYPE_CHILD,
	UI_TYPE_CHILD_MODAL,

	UI_TYPE_SELF,
};

// Note : ��Ʈ�ѵ��� �����Դϴ�.
//		���� �Ҷ� ��Ʈ���� ����� ���İ��� ������ �ݴϴ�.
//
enum UI_CONTROL_STATE
{
	UI_STATE_NORMAL,
	UI_STATE_DISABLED,
	UI_STATE_HIDDEN,
	UI_STATE_FOCUS,
	UI_STATE_MOUSEENTER,
	UI_STATE_PRESSED,
	UI_STATE_COUNT,
};

enum UI_CONTROL_TYPE
{ 
	// Note : �Ʒ� ��Ʈ�� Ÿ���� ����Ǹ� �ȵȴ�.
	//		����� ���̹Ƿ� �о���̱� ���ؼ� �׻� ��ġ�ؾ� �Ѵ�.
	//		���ο� ����Ʈ ��Ʈ���� ����� ���⿡ Ÿ���� �߰��� �ֽø� �˴ϴ�.
	//
	UI_CONTROL_STATIC = 0, 
	UI_CONTROL_BUTTON = 1, 
	UI_CONTROL_CHECKBOX = 2,
	UI_CONTROL_RADIOBUTTON = 3,
	UI_CONTROL_COMBOBOX = 4,
	UI_CONTROL_SLIDER = 5,
	UI_CONTROL_EDITBOX = 6,
	UI_CONTROL_IMEEDITBOX = 7,
	UI_CONTROL_LISTBOX = 8,
	UI_CONTROL_SCROLLBAR = 9,
	UI_CONTROL_PROGRESSBAR = 10,
	UI_CONTROL_CUSTOM = 11,
	UI_CONTROL_TEXTBOX = 12,
	UI_CONTROL_TEXTURECONTROL = 13,
	UI_CONTROL_HTMLTEXTBOX = 14,
	UI_CONTROL_TREECONTROL = 15,
	UI_CONTROL_QUESTTREECONTROL = 16,
	UI_CONTROL_ANIMATION = 17,
	UI_CONTROL_LINE_EDITBOX = 18,
	UI_CONTROL_LINE_IMEEDITBOX = 19,
	UI_CONTROL_MOVIECONTROL = 20,
	UI_CONTROL_LISTBOXEX = 21,

//#ifdef PRE_ADD_UICONTROL_MENULIST
	UI_CONTROL_MENULIST = 22,
//#endif // PRE_ADD_UICONTROL_MENULIST
	
	UI_CONTROL_SIZE = 23,

};

enum UIAllignHoriType
{
	AT_HORI_NONE,
	AT_HORI_LEFT,
	AT_HORI_CENTER,
	AT_HORI_RIGHT,

	// �Ʒ� �� ���ڴ� ���̾�α� �������Ŀ����� ���ȴ�.
	AT_HORI_LEFT_FIXED,
	AT_HORI_RIGHT_FIXED,
};

enum UIAllignVertType
{
	AT_VERT_NONE,
	AT_VERT_TOP,
	AT_VERT_CENTER,
	AT_VERT_BOTTOM,
};

enum UIButtonType
{
	UI_BUTTON_CLICK,
	UI_BUTTON_TOGGLE,
};

enum UIButtonSound
{
	UI_BUTTON_NONE,
	UI_BUTTON_OK,
	UI_BUTTON_CANCEL,
	UI_BUTTON_PAGE,
};

enum UITextBoxVariableType
{
	UI_TEXTBOX_NONE,
	UI_TEXTBOX_WIDTH,
	UI_TEXTBOX_HEIGHT,
	UI_TEXTBOX_BOTH,
};

enum UIDragCmdParam
{
	UI_DRAG_CMD_NONE,
	UI_DRAG_CMD_CANCEL,
};

#define _ANIMATION_FRAME_CONTROL
#ifdef _ANIMATION_FRAME_CONTROL
enum UIAnimationPlayTimeType
{
	UI_ANIMATION_PLAYTIME_PERFRAME,
	UI_ANIMATION_PLAYTIME_PERACTION,
};
#endif

// Note : UI ProcessCommand Message
//
#define UI_MSG_OK			0x00000000L
#define UI_MSG_CANCEL		0x00000001L
#define UI_MSG_YES			0x00000002L
#define UI_MSG_NO			0x00000003L
