
function btnigwEnterGameDialogFormCloseClick(Sender)
{
	igwEnterGameDialog.close;
}

function OnigwEnterGameDialogResize(Sender)
{
	btnigwEnterGameDialogFormClose.left = Sender.width - 30;
}

function OnigwEnterGameDialogHide(Sender)
{
}

function OnigwEnterGameDialogShow(Sender)
{
}

function OnbtnKFClick(Sender)
{
	var WItem = IGW.WidgetList.item("sdoRecommendKF");
	if (WItem) 
		WItem.OpenWidget();
	else 
		System.Content.OpenIE("", "{config(PT,RecommendKF)}", 0, 0, 0, 0, "fullscreen");
}

function OnbtnCZClick(Sender)
{
	var WItem = IGW.WidgetList.item("sdoRecommendCZ");
	if (WItem) 
		WItem.OpenWidget();
	else 
		System.Content.OpenIE("", "{config(PT,RecommendCZ)}", 0, 0, 0, 0, "fullscreen");
}

function OnbtnJFClick(Sender)
{
	var WItem = IGW.WidgetList.item("sdoRecommendJF");
	if (WItem) 
		WItem.OpenWidget();
	else 
		System.Content.OpenIE("", "{config(PT,RecommendJF)}", 0, 0, 0, 0, "fullscreen");
}

function OnbtnFHClick(Sender)
{
	var WItem = IGW.WidgetList.item("sdoRecommendFH");
	if (WItem) 
		WItem.OpenWidget();
	else 
		System.Content.OpenIE("", "{config(PT,RecommendFH)}", 0, 0, 0, 0, "fullscreen");
}

OnigwEnterGameDialogResize(igwEnterGameDialog);
igwEnterGameDialog.onresize="OnigwEnterGameDialogResize(Self);";