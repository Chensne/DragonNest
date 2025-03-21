// 最底部的广告
var g_BottomAd = null;
// 最顶部的广告
var g_TopAd = null;
// 是否显示广告
var g_IsShowLogo = false;
// 是否显示导航条
var g_IsShowNavigation = false;

// 广告模式常量定义
var cAdStyle_Bottom16 = 1;
var cAdStyle_Top24    = 2;
var cAdStyle_Bottom75 = 4;

function OnBrowserResize(Sender)
{  
  formCaption.Width = Sender.Width - 180;

  btnClose.Left = Sender.Width - 36;
  if (btnMin.visible)  
  {
  	btnMin.Left = btnClose.Left - 24; 
  }
    
  if (btnHelp.visible)
  {
   	if (btnMin.visible)
  		btnHelp.Left = btnMin.Left - 25; 
  	else
  		btnHelp.Left = btnClose.Left - 24;
  }
    
  /*  在龙之谷主题中去掉广告相关 
	switch(WebBrowser.AdcolumnStyle){
		case 0: // 无广告位，可以在处初始化些参数
		break;
		
		case 1:// 仅有最底部有广告
			if (g_BottomAd)
			{
				g_BottomAd.left = 5;
				g_BottomAd.top = WebBrowser.height - 18;
				g_BottomAd.width = WebBrowser.width - 12;
			}
		break;
		case 2:
		  if (g_TopAd)
		  {
		  	g_TopAd.width = WebBrowser.width - 12;
		  }
		break;
		case 3:
		  	if (g_TopAd)
		  	{
		  		g_TopAd.width = WebBrowser.width - 12;
		  	}
			if (g_BottomAd)
			{
				g_BottomAd.left = 5;
				g_BottomAd.top = WebBrowser.height - 18;
				g_BottomAd.width = WebBrowser.width - 12;
			}		
		break;
		case 4:
			if (g_BottomAd)
			{
				g_BottomAd.left = 5;
				g_BottomAd.top = WebBrowser.height - 78;
				g_BottomAd.width = WebBrowser.width - 12;
			}		
		break;
		case 6:
		  	if (g_TopAd)
		  	{
		  		g_TopAd.width = WebBrowser.width - 12;
		  	}
			if (g_BottomAd)
			{
				g_BottomAd.left = 5;
				g_BottomAd.top = WebBrowser.height - 78;
				g_BottomAd.width = WebBrowser.width - 12;
			}				
		break;
		
		default:// 同case 0 也是无广告
	}  
	*/
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

//--------把中文字符转换成Utf8编码------------------------//
function EncodeUtf8(s1)
{
  var s = encodeURIComponent(s1);
  var sa = s.split("%");
  var retV ="";

  if(sa[0] != "")
  {
    retV = sa[0];
  }
  for(var i = 1; i < sa.length; i ++)
  {
    if(sa[i].substring(0,1) == "u")
    {
      retV += Hex2Utf8(Str2Hex(sa[i].substring(1,5)));
    }
    else
      retV += "%" + sa[i];
  }
      
  return retV;
}

function Str2Hex(s)
{
  var c = "";
  var n;
  var ss = "0123456789ABCDEF";
  var digS = "";

  for(var i = 0; i < s.length; i ++)
  {
    c = s.charAt(i);
    n = ss.indexOf(c);
    digS += Dec2Dig(eval(n));
  }

  //return value;
  return digS;
}

function Dec2Dig(n1)
{
  var s = "";
  var n2 = 0;

  for(var i = 0; i < 4; i++)
  {
    n2 = Math.pow(2,3 - i);
    if(n1 >= n2)
    {
      s += '1';
      n1 = n1 - n2;
    }
    else
      s += '0';
  }
 
  return s;
}

function Dig2Dec(s)
{
  var retV = 0;

  if(s.length == 4)
  {
    for(var i = 0; i < 4; i ++)
    {
      retV += eval(s.charAt(i)) * Math.pow(2, 3 - i);
    }
    return retV;
  }
  
  return -1;
} 

function Hex2Utf8(s)
{
  var retS = "";
  var tempS = "";
  var ss = "";
 
  if(s.length == 16)
  {
    tempS = "1110" + s.substring(0, 4);
    tempS += "10" +  s.substring(4, 10); 
    tempS += "10" + s.substring(10,16); 
    var sss = "0123456789ABCDEF";

    for(var i = 0; i < 3; i ++)
    {
      retS += "%";
      ss = tempS.substring(i * 8, (eval(i)+1)*8);
            
      retS += sss.charAt(Dig2Dec(ss.substring(0,4)));
      retS += sss.charAt(Dig2Dec(ss.substring(4,8)));
    }
    return retS;
  }
  
  return "";
} 

function OnFormBtnHelp(Sender)
{
  // 如果窗口属于Widget，取WidgetNameSpace，否则取窗口名称
  var WName;
  if (typeof(WidgetInfo) == "object")
    WName = WidgetInfo.WidgetNameSpace;
  else
    WName = WebBrowser.Name;
  if (WName == "") return;

  // 如果存在“精灵”的应用，直接带参数打开精灵
  var WItem = IGW.WidgetList.item("sdoHelper");
  if (WItem) 
  {
    WItem.open(WName);
    return;
  }

  // 如果精灵不存在，用OpenIE打开页面
  var WURL  = "http://chhelp.sdrs.sdo.com:8080/sdchweb/default.asp?co=1&tp=3&ut=1&id=&gd=" +
              "&gm=" + WidgetAtom.Info.GameId +
              "&ar=" + WidgetAtom.Info.GameAreaId +
              "&sv=" + WidgetAtom.Info.GameServerId +
              "&rl=" + EncodeUtf8(WidgetAtom.User.NickName) +
              "&ck=" + encodeURI(WName);

  System.Content.OpenIE("SYSTEM:HELP", WURL, 0, 0, 420, 420, "xul=WebBrowser.xml;ap=center;sf=true;cp=true");
}

function CheckButtonStatus()
{ 
	var isWidget = typeof(WidgetInfo) == "object";
	var isBarVisible = isWidget && ((WidgetInfo.invisible&(2|8)) == 0);

	// 最小化按钮可见，要求必须是一个Widget且Widget不能是不出现在Bar上的(2: 不出现在Bar上; 8: 不出现在文字版Bar上)
	btnMin.visible = isWidget && isBarVisible;
	btnHelp.visible = isWidget;
	if (!btnHelp.visible) 
	{ 
		try {
			btnHelp.visible = (WebBrowser.IsFlyoutWindow) && (WebBrowser.Name != "");
		}
		catch(e){}
	}
	
	var nServicesFlag = 0;
	if (isWidget)
	{
		try{
			nServicesFlag = WidgetInfo.ServicesFlag;
		}
		catch(e){
			// 在旧版本的客户端中是没有该属性的，从兼容性考虑仍然显示
			g_IsShowLogo = true;
			//System.Debug.outputString(" WidgetInfo.ServicesFlag Error\n");
			}
	}
	else
	{
		if (WebBrowser.name != "WebBrowser")
		{// 不是widget但是推荐的页面
			try{
				var wi=IGW.WidgetList.item(WebBrowser.name);
				if (wi)
				{
					nServicesFlag = wi.ServicesFlag;
				}
			}
			catch(e){}				
		}
	}
	
	switch(nServicesFlag)
	{// 为方便扩展
		case 1:
			g_IsShowLogo = true;
			break;
		default:
			g_IsShowLogo = false;
			break;	
	}
	
  //System.Debug.outputString(" WidgetInfo.ServicesFlag == " + nServicesFlag + "  winname=" + WebBrowser.name + " \n");
  //g_IsShowLogo = true;
  if (g_IsShowLogo)
  { // 不显示logo时的caption情况
  	formCaption.left = 94;
  	formLogo.visible = true;
  }
  else
  {
  	formCaption.left = 94;
  	formLogo.visible = false;
  }
  
  	
  	try
  	{// 为兼容旧版本考虑，取不到则默认不显示
  		g_IsShowNavigation = WebBrowser.IsShowNavigation;
  		if (g_IsShowNavigation == undefined)
  		{
  			g_IsShowNavigation = false;
  		}
  	}
  	catch(e)
  	{
  		g_IsShowNavigation = false;
  	}
  	
  /*	关闭导航功能
	btnPrior.visible = g_IsShowNavigation;
	btnNext.visible = g_IsShowNavigation;
	btnRefresh.visible = g_IsShowNavigation;
  
	if (g_IsShowNavigation)
	{
		formCaption.left = formCaption.left + 54;
		if (g_IsShowLogo)
		{
			btnPrior.left = 74;
			btnNext.left = 92;
			btnRefresh.left = 110;
		}
		else
		{
			btnPrior.left = 5;
			btnNext.left = 23;
			btnRefresh.left = 41;
		}
	}
	*/ 
}
function OnWebBrowserSetFocus(Sender)
{
	if (eventArgs.Focus)
	{// 有焦点状态
		//btnClose.SkinName="btn_igwieclose_focus_15x15";
		//btnHelp.SkinName="btn_igwieWenHao_focus_15x15";	
		//btnMin.SkinName="btn_igwieMini_focus_15x15";
		//formCaption.ShadowReplay = 0;
		formCaption.TextColor = 0xFFE8EBCB;
		//formCaption.ShadowEnabled = true;
		//System.Debug.outputString("OnWebBrowserSetFocus true\n");
	}
	else
	{// 无焦点状态
		//btnClose.SkinName="btn_igwieclose_Normal_15x15";
		//btnHelp.SkinName="btn_igwieWenHao_normal_15x15";
		//btnMin.SkinName="btn_igwieMini_normal_15x15";
		//formCaption.ShadowEnabled = false;
		//formCaption.ShadowReplay = 0;
		formCaption.TextColor = 0x80000000;
		//System.Debug.outputString("OnWebBrowserSetFocus false\n");
	}
}

function OnBottomAdClose(Sender)
{
	var t = 0;
	if (g_BottomAd)
	{
		t = g_BottomAd.height;
		//System.Debug.outputString("OnBottomAdClose t=" + t + "\n");		
		g_BottomAd.OnClose = "";
	}
	g_BottomAd = null;	
	
	// 在这里不能做逻辑操作，只能做些清除变量引用等
	
	if (t == 16)
	{
		WebBrowser.AdcolumnStyle = (WebBrowser.AdcolumnStyle & (~cAdStyle_Bottom16));
		OnBrowserResize(WebBrowser);
	}
	if (t == 75)
	{
		//System.Debug.outputString("OnBottomAdClose AdcolumnStyle75 begin=" + WebBrowser.AdcolumnStyle + "\n");
		WebBrowser.AdcolumnStyle = (WebBrowser.AdcolumnStyle & (~cAdStyle_Bottom75));
		//System.Debug.outputString("OnBottomAdClose AdcolumnStyle75   end=" + WebBrowser.AdcolumnStyle + "\n");
		OnBrowserResize(WebBrowser);
	}	
}

function OnTopAdClose(Sender)
{
	var t = 0;
	if (g_TopAd)
	{
		t = g_TopAd.height;
		//System.Debug.outputString("OnTopAdClose t=" + t + "\n");	
		g_TopAd.OnClose = "";
	}	
	g_TopAd = null;
	
	if (t==24)
	{
		//System.Debug.outputString("OnTopAdClose AdcolumnStyle75 begin=" + WebBrowser.AdcolumnStyle + "\n");
		WebBrowser.AdcolumnStyle = (WebBrowser.AdcolumnStyle & (~cAdStyle_Top24));
		//System.Debug.outputString("OnTopAdClose AdcolumnStyle75   end=" + WebBrowser.AdcolumnStyle + "\n");
		OnBrowserResize(WebBrowser);
	}
}

function OnAdWebBrowserStyleUpdate(Sender)
{
	//System.Debug.outputString("OnAdWebBrowserStyleUpdate" + WebBrowser.AdcolumnStyle + "\n");
	/* 在龙之谷主题中去掉边框上的广告功能
	switch(WebBrowser.AdcolumnStyle){
		case 0: // 无广告位，可以在处初始化些参数
			WebBrowser.Margin = "(6, 33, 6, 6)";
			WebBrowser.SkinName = "bg_widgetform";
		break;
		
		case 1:// 仅有最底部有广告 16像素
			WebBrowser.Margin = "(6, 33, 6, 19)";
			WebBrowser.SkinName = "bg_widgetform_ad01";		
			// 底部广告
			g_BottomAd = WebBrowser.CreateControls("adwebbrowsercontrol");
			//g_BottomAd.BorderColor = 0xFFFF0000;
			g_BottomAd.height = 16;
			g_BottomAd.name = "g_BottomAd";
			// 测试用
			g_BottomAd.URL = WebBrowser.AdBottomURL;
			g_BottomAd.OnClose = OnBottomAdClose;
			g_BottomAd.NoInput = true;
		break;
		case 2:
			WebBrowser.Margin = "(6, 59, 6, 6)";
			WebBrowser.SkinName = "bg_widgetform_ad02";		
			// 顶部广告 24像素
			g_TopAd = WebBrowser.CreateControls("adwebbrowsercontrol"); 
			//g_TopAd.BorderColor = 0xFFFF0000;
			g_TopAd.height = 24;
	  		g_TopAd.left = 5;
	  		g_TopAd.top = 34;
	  		g_TopAd.name = "g_TopAd";
			
			// 测试用
			g_TopAd.URL = WebBrowser.AdTopURL;
			g_TopAd.OnClose = OnTopAdClose;
			g_TopAd.NoInput = true;
			
		break;
		case 3:
			WebBrowser.Margin = "(6, 59, 6, 19)";
			WebBrowser.SkinName = "bg_widgetform_ad03";
			// 底部16，顶部24都有广告
			
			// 顶部广告
			g_TopAd = WebBrowser.CreateControls("adwebbrowsercontrol"); 
			//g_TopAd.BorderColor = 0xFFFF0000;
			g_TopAd.height = 24;
	  		g_TopAd.left = 5;
	  		g_TopAd.top = 34;
	  		g_TopAd.NoInput = true;
			
			// 测试用
			g_TopAd.URL = WebBrowser.AdTopURL;
			g_TopAd.OnClose = OnTopAdClose;
			g_TopAd.NoInput = true;
			g_TopAd.name = "g_TopAd";

			// 底部广告
			g_BottomAd = WebBrowser.CreateControls("adwebbrowsercontrol");
			//g_BottomAd.BorderColor = 0xFFFF0000;
			g_BottomAd.name = "g_BottomAd";
			g_BottomAd.height = 16;
			// 测试用
			g_BottomAd.URL = WebBrowser.AdBottomURL;
			g_BottomAd.OnClose = OnBottomAdClose;
			g_BottomAd.NoInput = true;
		break;
		
		case 4:
			// 底部广告75像素高
			WebBrowser.Margin = "(6, 33, 6, 80)";
			WebBrowser.SkinName = "bg_widgetform_ad04";		
			// 底部广告
			g_BottomAd = WebBrowser.CreateControls("adwebbrowsercontrol");
			//g_BottomAd.BorderColor = 0xFFFF0000;
			g_BottomAd.height = 75;
			g_BottomAd.name = "g_BottomAd";
			// 测试用			
			g_BottomAd.URL = WebBrowser.AdBottomURL;
			g_BottomAd.OnClose = OnBottomAdClose;
			g_BottomAd.NoInput = true;		
		break;
		
		case 6:
			// 顶24 底部广告75像素高
			WebBrowser.Margin = "(6, 59, 6, 80)";
			WebBrowser.SkinName = "bg_widgetform_ad06";		
			// 顶部广告
			g_TopAd = WebBrowser.CreateControls("adwebbrowsercontrol"); 
			//g_TopAd.BorderColor = 0xFFFF0000;
			g_TopAd.height = 24;
	  		g_TopAd.left = 5;
	  		g_TopAd.top = 34;
	  		g_TopAd.NoInput = true;
						
			g_TopAd.URL = WebBrowser.AdTopURL;
			g_TopAd.OnClose = OnTopAdClose;
			g_TopAd.NoInput = true;
			g_TopAd.name = "g_TopAd";

			// 底部广告
			g_BottomAd = WebBrowser.CreateControls("adwebbrowsercontrol");
			//g_BottomAd.BorderColor = 0xFFFF0000;
			g_BottomAd.name = "g_BottomAd";
			g_BottomAd.height = 75;
			// 测试用
			g_BottomAd.URL = WebBrowser.AdBottomURL;
			g_BottomAd.OnClose = OnBottomAdClose;
			g_BottomAd.NoInput = true;
		
		break;
		
		default:// 同case 0 也是无广告
			WebBrowser.Margin = "(6, 33, 6, 6)";
			WebBrowser.SkinName = "bg_widgetform";
	}
	*/
  CheckButtonStatus();
  OnBrowserResize(WebBrowser);
}

function OnAdWebBrowserBeforClear(Sender)
{
	// 最底部的广告
	g_BottomAd = null;
	// 最顶部的广告
	g_TopAd = null;
	
	//System.Debug.outputString("OnAdWebBrowserBeforClear\n");
}

function loadMain(browser)
{
  WebBrowser.onresize = OnBrowserResize;
  WebBrowser.oncontenttitlechange = OnBrowserTitleChange;
//  WebBrowser.oncontentstatusTextchange = OnBrowserStatusTextChange;
  WebBrowser.oncontentprogresschange = OnBrowserTitleChange;

  // 初次根据状态检测
  OnAdWebBrowserBeforClear(WebBrowser);
  OnAdWebBrowserStyleUpdate(WebBrowser);
  
  //CheckButtonStatus();
  //OnBrowserResize(WebBrowser);
}
loadMain(WebBrowser);