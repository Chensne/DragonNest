function btnigwUserLoginECardDialogFormCloseClick(Sender)
{
	igwUserLoginECardDialog.close;
}

function OnigwUserLoginECardDialogResize(Sender)
{
	btnigwUserLoginECardFormClose.left = Sender.width - 30;
	
	btnOK.left = (Sender.width / 2) - 90;
	btnOK.top = Sender.height - 38;
	
	btnCancel.left = (Sender.width / 2) + 20;
	btnCancel.top = Sender.height - 38;	
	lblbgBlackBlock1.width = Sender.width - 17;
	lblbgBlackBlock1.top = Sender.height - 50;

}

OnigwUserLoginECardDialogResize(igwUserLoginECardDialog);
igwUserLoginECardDialog.onresize="OnigwUserLoginECardDialogResize(Self);";
igwUserLoginECardDialog.AddCacheSkins("bg_igwMibo001_208x305");
igwUserLoginECardDialog.AddCacheSkins("bg_igwMibo002_208x305");
igwUserLoginECardDialog.AddCacheSkins("bg_SkGridHorizontal_185x20");
igwUserLoginECardDialog.AddCacheSkins("bg_SkGridVertical_24x286");