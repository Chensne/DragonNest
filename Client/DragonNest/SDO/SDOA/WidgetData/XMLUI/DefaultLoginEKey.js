function btnigwUserLoginEKeyFormCloseClick(Sender)

{

	igwUserLoginEKeyDialog.close;

}



function OnigwUserLoginEKeyResize(Sender)

{

	btnigwUserLoginEKeyFormClose.left = Sender.width - 30;

	

	btnOK.left =  (Sender.width / 2) - 90;

	btnOK.top = Sender.height - 38;

	

	btnCancel.left = (Sender.width / 2) + 20;

	btnCancel.top = Sender.height - 38;

    lblbgBlackBlock.top = Sender.height - 50;

	lblbgBlackBlock.width = Sender.width - 17;



//	lblbgBlackBlock.width = Sender.width - 17;

//	lblbgBlackBlock.height = Sender.height - 179;	

}



function ChangStyle(iStyle)

{

	switch(iStyle)

	{

		case 1://  SimaS∆[ù[ìèeQLub?
			lblMBHintText001.Caption = "Step1:initial the embeded SD.";

			lblMBHintText001.top = 112;

			lblMBHintText002.Caption = "      E-key in the cellphone";

			lblMBHintText002.visible = true;

			lblMBHintText002.top = 130;

			lblMBHintText003.Caption = "Step2:input the challengestring according to the guidance in the phone";

			lblMBHintText003.top = 150;

			lblMBHintText004.Caption = "Step3:input string in the input box";

			lblMBHintText004.top = 170;

			lblMBHintText005.visible = true;

			lblMBHintText005.top = 188;

			lblMBHintText005.Caption = "    from the E-key";

			

			lblbgMiBao.left = 68;

			lblbgMiBao.top = 42;			

			lblbgMiBao.width = 53;

			lblbgMiBao.height = 66;

			lblbgMiBao.Icon = ".\\Images\\SimMibo.png";

			labelMb_A8Hint.visible = false;

			//labelChallengeCaption.left = 197;

			labelChallengeCaption.Caption = "Cellphone SD E-key challengestring:";



		break;

		default://  ÿû§ãÑv∆[ù[ìèeQLub?
			lblMBHintText001.Caption = "Step1:input the challengestring in the SD E-key";

			lblMBHintText001.top = 122;

			lblMBHintText002.visible = false;

			lblMBHintText002.top = 140;

			lblMBHintText003.Caption = "Step2:input string in the input box";

			lblMBHintText003.top = 166;

			lblMBHintText004.Caption = "0000input box";

			lblMBHintText004.top = 184;

			lblMBHintText005.visible = false;

			lblbgMiBao.left = 28;

			lblbgMiBao.top = 52;			

			lblbgMiBao.width = 131;

			lblbgMiBao.height = 66;

			lblbgMiBao.Icon = ".\\Images\\A8MiBo.png";

			labelMb_A8Hint.visible = true;

			//labelChallengeCaption.left = 197;

			labelChallengeCaption.Caption = "challengestringˇ";

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