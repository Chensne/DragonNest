function OnBtnViewHelp(Sender)
{
	System.Content.OpenIE("", "{config(PT,ViewHelp)}", 0, 0, 0, 0, "fullscreen");	
}

function OnBtnGetDriverWiz(Sender)
{
	System.Content.OpenIE("", "{config(PT,GetDriverWiz)}", 0, 0, 0, 0, "fullscreen");
}
