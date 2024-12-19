// JavaScript Document

function CloseIGW()
{
	external.stateEnable = false;
	external.hide();
}

function OpenIGW()
{
	external.stateEnable = true;
	external.hide();
}

function GetIGWState()
{
	return external.stateEnable;
}

function PostCloseIGWReason(aReason)
{
	external.PostLog("~"+aReason);
}


function loadMain()
{
//	alert("����IGW״̬");
	openIGW.checked = external.stateEnable;
	closeIGW.checked = !external.stateEnable;
}