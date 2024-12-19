function OnFormResize(Sender)
{
	btnFormClose.left = Sender.width - 36;
	labelcaption.width = Sender.width - 180;
	labelText.width = Sender.width - 14;
	labelText.height = Sender.height - 42;
}

function btnFormCloseOnClick(Sender){
	PopupWidgetMessageDialog.close();
}

OnFormResize(PopupWidgetMessageDialog);
PopupWidgetMessageDialog.onresize="OnFormResize(Self);";