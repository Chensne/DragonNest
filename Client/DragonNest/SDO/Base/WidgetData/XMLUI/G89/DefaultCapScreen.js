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
		btnSetFullScreen.hint="ȫ��";
		igwCapScreenDialog.ScrCapRestore();
	}
	else
	{
		btnSetFullScreen.SkinName=igwCapScreenDialog.GetCacheSkins("btn_restore12x12");
		btnSetFullScreen.hint="��ԭ";
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
    // ������鲻���ڣ�ֱ��OpenIE��ҳ��
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
	// ֻ��ʱ��ʱ����ʾ��skin��Դ��Ҫcache��������ʾ�Ŀ��Բ�cache
	igwCapScreenDialog.AddCacheSkins("btn_max12x12");
	igwCapScreenDialog.AddCacheSkins("btn_restore12x12");
	
	igwCapScreenDialog.AddCacheSkins("btn_help12x12");
	igwCapScreenDialog.AddCacheSkins("btn_max12x12");
	igwCapScreenDialog.AddCacheSkins("btn_close12x12");
	igwCapScreenDialog.AddCacheSkins("btn_Capture35x35");	
}

LoadMain();