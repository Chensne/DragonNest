function btnigwVirtualKeyboardDialogFormCloseClick(Sender)
{
	igwVirtualKeyboardDialog.close;
}

function OnigwInputMiBaoDialogResize(Sender)
{
	//btnigwInputMiBaoDialogFormClose.left = Sender.width - 20;
	//btnigwInputMiBaoDialogFormClose.top = Sender.height - 20;
	lblbgBlackBlock.width = Sender.width - 1;
	lblbgBlackBlock.height = Sender.height - 1;
		
}

OnigwInputMiBaoDialogResize(igwVirtualKeyboardDialog);
igwVirtualKeyboardDialog.onresize="OnigwInputMiBaoDialogResize(Self);";