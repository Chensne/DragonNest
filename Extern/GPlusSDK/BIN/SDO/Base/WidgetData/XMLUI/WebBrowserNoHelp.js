function OnBrowserResize(Sender)
{
  formCaption.Width = Sender.Width - 100;
  btnClose.Left = Sender.Width - 25;
}

function OnBrowserTitleChange(Sender)
{
  var BrowserCaption = Sender.ContentTitle;
  
  if (BrowserCaption =="" || BrowserCaption == "about:blank")
  {
  	BrowserCaption = System.Widget.Name;

  	if ((BrowserCaption == "Application") || (BrowserCaption == "<Application>"))
		BrowserCaption = "圈圈";
  }

  if ((Sender.ContentProgress>=0) && (Sender.ContentProgressMax>0) && (Sender.ContentProgress < Sender.ContentProgressMax))
  {
	  BrowserCaption = BrowserCaption + " (Loading..." + Math.floor(Sender.ContentProgress*100/Sender.ContentProgressMax) + "%)";
  }
  // 这样做的目的是使在做文字特效时周围能空余出足够的空白，为快速达到目的先这样做，内部文字输出系统要调整好需要时间
  formCaption.Caption = BrowserCaption;
}

function OnBrowserStatusTextChange(Sender)
{
  formCaption.Caption = Sender.ContentStatusText;
}

function OnFormBtnClose(Sender)
{
	WebBrowser.close;
}

function OnFormBtnMin(Sender){
	System.Gadget.visible=false;
}

function loadMain(browser)
{
  WebBrowser.onresize = OnBrowserResize;
  WebBrowser.oncontenttitlechange = OnBrowserTitleChange;
//  WebBrowser.oncontentstatusTextchange = OnBrowserStatusTextChange;
  WebBrowser.oncontentprogresschange = OnBrowserTitleChange;

  OnBrowserResize(WebBrowser);
}

function OnWebBrowserSetFocus(Sender)
{
	if (eventArgs.Focus)
	{// 有焦点状态
		//btnClose.SkinName="btn_igwieclose_focus_15x15";
		//btnHelp.SkinName="btn_igwieWenHao_focus_15x15";	
		//btnMin.SkinName="btn_igwieMini_focus_15x15";
		//formCaption.ShadowReplay = 3;
		formCaption.TextColor = 0xFF000000;
		//formCaption.ShadowEnabled = true;
	}
	else
	{// 无焦点状态
		//btnClose.SkinName="btn_igwieclose_Normal_15x15";
		//btnHelp.SkinName="btn_igwieWenHao_normal_15x15";
		//btnMin.SkinName="btn_igwieMini_normal_15x15";
		//formCaption.ShadowEnabled = false;
		//formCaption.ShadowReplay = 0;
		formCaption.TextColor = 0x80000000;
	}
}

loadMain(WebBrowser);