function OnMsgDialogResize(Sender)
{
	btnFormClose.Left = Sender.Width - 36;
	labelcaption.Width = Sender.Width - 180;
	labelText.height = Sender.Height - 100;
	btnOK.top = Sender.Height - 46;
	btnCancel.top = btnOK.top;
}

function btnFormCloseOnClick(Sender){
	MyTestWidgetDialog.close();
}

MyTestWidgetDialog.onresize = OnMsgDialogResize;