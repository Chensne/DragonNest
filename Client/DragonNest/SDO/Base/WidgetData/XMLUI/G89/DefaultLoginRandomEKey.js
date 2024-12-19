function btnigwUserLoginEKeyFormCloseClick(Sender)
{
	igwUserLoginEKeyDialog.close;
}

function OnigwUserLoginEKeyResize(Sender)
{
//	btnigwUserLoginEKeyFormClose.left = Sender.width - 30;
	
//	btnOK.left =  (Sender.width / 2) - 90;
//	btnOK.top = Sender.height - 38;
	
//	btnCancel.left = (Sender.width / 2) + 20;
  //	btnCancel.top = Sender.height - 38;
  //  lblbgBlackBlock.top = Sender.height - 50;
  //	lblbgBlackBlock.width = Sender.width - 17;

//	lblbgBlackBlock.width = Sender.width - 17;
//	lblbgBlackBlock.height = Sender.height - 179;	
}

OnigwUserLoginEKeyResize(igwUserLoginEKeyDialog);
igwUserLoginEKeyDialog.onresize="OnigwUserLoginEKeyResize(Self);";