var g_MaxWidth=0;

function btnFormCloseOnClick(Sender){
	//IMEWidgetDialog.close();
	IMEWidgetDialog.visible = false;
}

function OnIMEWidgetDialogResize(Sender){
	//labelcaption.width = Sender.width - 20;
	//labelText.width = Sender.width - 20;
	btnFormClose.left = IMEWidgetDialog.width - 22;
}

function OnIMEWGDialogImestrchanged(Sender){
	g_MaxWidth = LblCompStr.width;
	if (g_MaxWidth < LblCandList.width)
	{
		g_MaxWidth = LblCandList.width;
	}
	if (g_MaxWidth < LblIMEName.width)
	{
		g_MaxWidth = LblIMEName.width;
	}
	IMEWidgetDialog.width = g_MaxWidth + 10;
}

IMEWidgetDialog.onresize="OnIMEWidgetDialogResize(Self);";