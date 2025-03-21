function OnBrowserResize(Sender)
{
  formCaption.Width = Sender.Width;
  btnFormClose.left = WebBrowser.Width - btnFormClose.Width - 2;
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
  formCaption.Caption = BrowserCaption;
}

function OnBrowserStatusTextChange(Sender)
{
  formCaption.Caption = Sender.ContentStatusText;
}

function btnFormCloseOnClick(Sender)
{
	//formCaption.Caption = "asdfkjalksdjf";
	WebBrowser.close();
}

function loadMain(browser)
{
  WebBrowser.onresize = OnBrowserResize;
  WebBrowser.oncontenttitlechange = OnBrowserTitleChange;
//  WebBrowser.oncontentstatusTextchange = OnBrowserStatusTextChange;
  WebBrowser.oncontentprogresschange = OnBrowserTitleChange;

  OnBrowserResize(WebBrowser);
}

loadMain();