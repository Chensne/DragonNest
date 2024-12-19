var g_VirtualKeyboard = null;


function btnigwUserLoginDialogFormCloseClick(Sender)
{
	igwUserLoginDialog.close;
}

function OnigwUserLoginDialogResize(Sender)
{

//	btnigwUserLoginDialogFormClose.left = Sender.width - 30;
//	btnOK.left = (Sender.width / 2) - 100;
//	btnOK.top = Sender.height - 40;
	
//	btnCancel.left = (Sender.width / 2) + 30;
//	btnCancel.top = Sender.height - 40;
//      lblbgAdvertisingBlock.top = Sender.height - 80;
//	lblbgAdvertisingBlock.width = Sender.width - 17;
//      lblbgBlackBlock.height = Sender.height - 179;
}


function OnBtnMyComplaint(Sender)
{
	var WItem = IGW.WidgetList.item("sdoComplaint");
	if (WItem)
		WItem.OpenWidget();
	else 
		System.Content.OpenIE("", "{config(PT,Complaint)}", 0, 0, 0, 0, "fullscreen");	
}

function OnBtnlAccountProtection(Sender)
{
	var WItem = IGW.WidgetList.item("sdoSafeHelper");
	if (WItem)
		WItem.OpenWidget();
}


function OnBtnFindPwd(Sender)
{
	var WItem = IGW.WidgetList.item("sdoFindPwd");
	if (WItem)
		WItem.OpenWidget();
	else 
		System.Content.OpenIE("", "{config(PT,FindPwd)}", 0, 0, 0, 0, "fullscreen");	
}

function OnBtnChangePwd(Sender)
{
	var WItem = IGW.WidgetList.item("sdoChangePwd");
	if (WItem) 
		WItem.OpenWidget();
	else 
		System.Content.OpenIE("", "{config(PT,ChangePwd)}", 0, 0, 0, 0, "fullscreen");
}

function OnBtnRegisterPt(Sender)
{

	var WItem = IGW.WidgetList.item("sdoReg");
	if (WItem) 
		WItem.OpenWidget();
	else
		System.Content.OpenIE("", "{config(PT,RegPt)}", 0, 0, 0, 0, "fullscreen");	
}

function OnbtnKeyboardLinked(Sender)
{
	try{
		g_VirtualKeyboard = btnKeyboard.LinkWin;
		if (g_VirtualKeyboard) 
		{
			g_VirtualKeyboard.OnClose = OnVirtualKeyboardClose;
		}
		//System.Debug.OutputString("OnbtnKeyboardLinked OK");
	}
	catch(e) {
		g_VirtualKeyboard = null;
		//System.Debug.OutputString("OnbtnKeyboardLinked Error");
	}	
	edtPassword.onsetfocus="";	
	edtPassword.focus = true;
	edtPassword.onsetfocus="OnedtPasswordSetFocus(Self);";	
}

function OnbtnKeyboardClick(Sender)
{		
	edtPassword.onsetfocus="";	
	edtPassword.focus = true;
	edtPassword.onsetfocus="OnedtPasswordSetFocus(Self);";		
	//System.Debug.OutputString("OnbtnKeyboardClick ");
}

function OnVirtualKeyboardClose(){
	g_VirtualKeyboard = null;
}

function OnigwUserLoginDialogHide(Sender)
{
	//System.Debug.OutputString("OnigwUserLoginDialogHide");
	if (g_VirtualKeyboard)
	{
		try{
			g_VirtualKeyboard.visible = false;
		}
		catch(e) {
			g_VirtualKeyboard = null;
		}
	}
}

function OnigwUserLoginDialogShow(Sender)
{
	//System.Debug.OutputString("OnigwUserLoginDialogShow");
	if (g_VirtualKeyboard)
	{
		try{
			g_VirtualKeyboard.visible = true;
		}
		catch(e) {
			g_VirtualKeyboard = null;
		}
	}	
}


function OnedtPasswordSetFocus(Sender){
	
	//System.Debug.OutputString("OnedtPasswordSetFocus");
	edtPassword.onsetfocus="";
	
	if (edtPassword.focus)
	{
		
		if (g_VirtualKeyboard)
		{
			g_VirtualKeyboard.visible = true;
			//System.Debug.OutputString("OnedtPasswordSetFocus True visible=true");
		}
		else
		{
			btnKeyboard.GoLink();
			//System.Debug.OutputString("OnedtPasswordSetFocus True visible=true GoLink");
		}
	}
	else
	{
		//System.Debug.OutputString("OnedtPasswordSetFocus False");
		if (g_VirtualKeyboard)
		{
			//System.Debug.OutputString("OnedtPasswordSetFocus False aaa");			
			g_VirtualKeyboard.close;
			g_VirtualKeyboard = null;
		}		
	}
	edtPassword.onsetfocus="OnedtPasswordSetFocus(Self);";
}


OnigwUserLoginDialogResize(igwUserLoginDialog);
igwUserLoginDialog.onresize="OnigwUserLoginDialogResize(Self);";