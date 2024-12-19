function btnigwCapScreenDialogFormCloseClick(Sender)
{
	igwCapScreenDialog.close;
}

function OnigwCapScreenDialogResize(Sender)
{
//	btnigwCapScreenDialogFormClose.left = Sender.width - 20;
	
//	btnSetFullScreen.left = (Sender.width / 2) - 100;	
//	btnSmallScreen.left = (Sender.width / 2) + 30;
	//WidgetAtom.Info.
}

function OnbtnSmallScreenClick(Sender){
	igwCapScreenDialog.ScrCapMini();
}

function OnbtnSetFullScreenClick(Sender){
	if (igwCapScreenDialog.IsCanRestore() == true)
	{
		btnSetFullScreen.SkinName=igwCapScreenDialog.GetCacheSkins("btn_max12x12");
		btnSetFullScreen.hint="全屏";
		igwCapScreenDialog.ScrCapRestore();
	}
	else
	{
		btnSetFullScreen.SkinName=igwCapScreenDialog.GetCacheSkins("btn_restore12x12");
		btnSetFullScreen.hint="还原";
		igwCapScreenDialog.ScrCapMax();
	}
}

function OnbtnhelpClick(Sender){
  var WItem = IGW.WidgetList.item("sdoHelper");
  //var WName = WidgetInfo.WidgetNameSpace;
  var WName = "CapScreen";

  if (WItem)
  {
    WItem.SrcParam = WName;
    WItem.OpenWidget();
    WItem.SrcParam = ""; 
  }
  else
  {
    // 如果精灵不存在，直接OpenIE打开页面
    var WURL  = "http://chhelp.sdrs.sdo.com:8080/sdchweb/default.asp?co=1&tp=3&ut=1&id=&gd=" +
                "&gm=" + WidgetAtom.Info.GameId +
                "&ar=" + WidgetAtom.Info.GameAreaId +
                "&sv=" + WidgetAtom.Info.GameServerId +
                "&rl=" + EncodeUtf8(WidgetAtom.User.NickName) +
                "&ck=" + encodeURI(WName);

    System.Content.OpenIE("SYSTEM:HELP", WURL, 0, 0, 420, 420, "xul=WebBrowserNoHelp.xml;ap=center;sf=true;cp=true");
  }	
}

//OnigwCapScreenDialogResize(igwCapScreenDialog);
//igwCapScreenDialog.onresize="OnigwCapScreenDialogResize(Self);";

function LoadMain(){
	// 只有时不时不显示的skin资源需要cache，总是显示的可以不cache
	igwCapScreenDialog.AddCacheSkins("btn_max12x12");
	igwCapScreenDialog.AddCacheSkins("btn_restore12x12");
	
	igwCapScreenDialog.AddCacheSkins("btn_help12x12");
	igwCapScreenDialog.AddCacheSkins("btn_max12x12");
	igwCapScreenDialog.AddCacheSkins("btn_close12x12");
	igwCapScreenDialog.AddCacheSkins("btn_Capture35x35");	
}

LoadMain();