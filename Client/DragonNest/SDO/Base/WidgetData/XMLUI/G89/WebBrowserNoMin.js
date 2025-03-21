function OnBrowserResize(Sender)
{
  formCaption.Width = Sender.Width - 180;
  btnClose.Left = Sender.Width - 36;
  btnHelp.Left = btnClose.Left - 24;
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

  System.Content.OpenIE("SYSTEM:HELP", WURL, 0, 0, 420, 420, "xul=WebBrowserNoHelp.xml;ap=center;sf=true;cp=true");
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
		formCaption.TextColor = 0xFFE8EBCB;
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