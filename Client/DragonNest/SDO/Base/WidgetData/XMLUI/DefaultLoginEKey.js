function btnigwUserLoginEKeyFormCloseClick(Sender)
{
	igwUserLoginEKeyDialog.close;
}

function OnigwUserLoginEKeyResize(Sender)
{
	//btnigwUserLoginEKeyFormClose.left = Sender.width - 30;
	
	//btnOK.left =  (Sender.width / 2) - 90;
	//btnOK.top = Sender.height - 38;
	
	//btnCancel.left = (Sender.width / 2) + 20;
	//btnCancel.top = Sender.height - 38;
    //lblbgBlackBlock.top = Sender.height - 50;
	//lblbgBlackBlock.width = Sender.width - 17;

//	lblbgBlackBlock.width = Sender.width - 17;
//	lblbgBlackBlock.height = Sender.height - 179;	
}

function ChangStyle(iStyle)
{
	switch(iStyle)
	{
		case 1://  Sim卡密宝输入界面
			lblMBHintText001.Caption = "第一步：启动手机内置盛大密宝";
			//lblMBHintText001.top = 112;
			lblMBHintText002.Caption = "　　　　功能";
			lblMBHintText002.visible = true;
			//lblMBHintText002.top = 130;
			lblMBHintText003.Caption = "第二步：按手机提示输入挑战码";
			//lblMBHintText003.top = 150;
			lblMBHintText004.Caption = "第三步：将生成的密宝密码填入";
			//lblMBHintText004.top = 170;
			lblMBHintText005.visible = true;
			//lblMBHintText005.top = 188;
			lblMBHintText005.Caption = "　　　　输入框内";
			
			//lblbgMiBao.left = 68;
			//lblbgMiBao.top = 42;			
			//lblbgMiBao.width = 53;
			//lblbgMiBao.height = 66;
			//lblbgMiBao.Icon = ".\\Images\\SimMibo.png";
			labelMb_A8Hint.visible = false;
			//labelChallengeCaption.left = 197;
			labelChallengeCaption.Caption = "手机密宝挑战码：";

		break;
		default://  默认的密宝输入界面
			lblMBHintText001.Caption = "第一步：将挑战码输入密宝中";
			//lblMBHintText001.top = 122;
			lblMBHintText002.visible = false;
			//lblMBHintText002.top = 140;
			lblMBHintText003.Caption = "第二步：将生成的密宝密码填入";
			//lblMBHintText003.top = 166;
			lblMBHintText004.Caption = "　　　　输入框内";
			//lblMBHintText004.top = 184;
			lblMBHintText005.visible = false;
			//lblbgMiBao.left = 28;
			//lblbgMiBao.top = 52;			
			//lblbgMiBao.width = 131;
			//lblbgMiBao.height = 66;
			//lblbgMiBao.Icon = ".\\Images\\A8MiBo.png";
			labelMb_A8Hint.visible = true;
			//labelChallengeCaption.left = 197;
			labelChallengeCaption.Caption = "挑战码：";
	}
}

function MyTest(Sender)
{
	if (lblMBHintText002.visible)
	  ChangStyle(0);
	 else
	  ChangStyle(1);
}

function onstylechange(Sender)
{
	try
	{
		ChangStyle(igwUserLoginEKeyDialog.style);
	}
	catch(e){
			}
}

OnigwUserLoginEKeyResize(igwUserLoginEKeyDialog);
igwUserLoginEKeyDialog.onresize="OnigwUserLoginEKeyResize(Self);";
onstylechange(igwUserLoginEKeyDialog);