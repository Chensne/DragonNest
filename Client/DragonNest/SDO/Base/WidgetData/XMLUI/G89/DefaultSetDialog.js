
function btnSetFormCloseOnClick(Sender){
	igwsetdlg.close();
}

function btnCancelOnClick(Sender){
	igwsetdlg.close();
}

function btnThemeNameOnClick(Sender)
{
	PopupMenuUITheme.PopupByWin(btnThemeName);
}

function btnSkinNameOnClick(Sender)
{
	PopupMenuUISkin.PopupByWin(btnSkinName);
}

function VisibleTabControls(iTabIndex, bVisible)
{
	switch(iTabIndex)
	{
		case 0:
			blendcaption.Visible = bVisible;	
			blendvalue.Visible = bVisible;
			sliderBlendValue.Visible = bVisible;
			lblUITheme.Visible = bVisible;
	
			btnThemeName.Visible = bVisible;
			btnThemeDownCombox.Visible = bVisible;
	
			btnSkinName.Visible = false; //bVisible;
			lblUISkin.Visible = false;//bVisible;
			btnSkinDownCombox.Visible = false;//bVisible;
			
			checkboxDisablePopupMsg.Visible =bVisible;

      
			sliderHueValue.Visible = false; //bVisible;
			sliderLuminanceValue.Visible = false; //bVisible;
			sliderSaturationValue.Visible = false; //bVisible;
			lblHue.Visible = false; //bVisible;
			lblLuminance.Visible = false; //bVisible;
			lblSaturation.Visible = false; //bVisible;
			btnHLSColorNormal.Visible = false; //bVisible;
			btnHLSColor01.Visible = false; //bVisible;
			btnHLSColor02.Visible = false; //bVisible;
			btnHLSColor03.Visible = false; //bVisible;
			btnHLSColor04.Visible = false; //bVisible;
			btnHLSColor05.Visible = false; //bVisible;
			btnHLSColor06.Visible = false; //bVisible;
			btnHLSColor07.Visible = false; //bVisible;
			
			btnTabUISkin.Enabled = !bVisible;
		break;
		case 1:
		  bVisible = false;
			labelsoundRegion.Visible = bVisible;
			lblhintsoundvalueset.Visible = bVisible;
			btnTabAudio.Enabled = !bVisible;
		break;
		case 2:
			checkboxDisablePopupMsg.visible = bVisible;
			checkboxAllowPopupMsg.visible = bVisible;
			//labelSystemSetRegion.visible = bVisible;
			labelSystemSetCaption.visible = bVisible;
			checkboxAllowServerMsg.visible = bVisible;
                        //Ä£Ê½Ñ¡Ôñ
                        checkboxMinTypeSet.Visible= bVisible;
                      
			btnTabSystemSet.Enabled = !bVisible;			
		break;
		case 3:						
			//btnTabCapScreenSet.Enabled = !bVisible;
			//labelSystemSetScreenCap.visible = bVisible;
			
			btnTabCapScreenSet.visible = false;
			labelSystemSetScreenCap.visible = false;
		break;		
	}
}

function OnbtnTabUISkinClick(Sender)
{
	VisibleTabControls(0, true);
	VisibleTabControls(1, false);
	VisibleTabControls(2, false);
	VisibleTabControls(3, false);
}

function OnbtnTabAudioClick(Sender)
{
	VisibleTabControls(0, false);
	VisibleTabControls(1, true);
	VisibleTabControls(2, false);
	VisibleTabControls(3, false);
}

function OnbtnTabSystemSetClick(Sender)
{
	VisibleTabControls(0, false);
	VisibleTabControls(1, false);
	VisibleTabControls(2, true);
	VisibleTabControls(3, false);
}

function OnbtnTabCapScreenSetClick(Sender)
{
	VisibleTabControls(0, false);
	VisibleTabControls(1, false);
	VisibleTabControls(2, false);
	VisibleTabControls(3, true);	
}

function OnbtnHLSColorNormalClick(Sender){
	igwsetdlg.ProSkinColor(0, 0, 0);
}

function OnbtnChangeDirPathClick(Sender){
	MenuChangeDirList.PopupByWin(btnChangeDirPath);
}

function OnSetDialogResize(Sender)
{
	btnSetFormClose.Left = Sender.Width - 36;
	WidgetListTitleWWW.Width = Sender.Width - 180;
	
	btnOK.top = Sender.Height - 46;
	btnCancel.top = btnOK.top;
}

OnbtnTabUISkinClick(btnTabUISkin);
editHotKey.readonly=true;
editCapBmpPath.readonly=true;
editCapBmpPath.popupmenu=EditMenuChangeDir;
igwsetdlg.onresize = OnSetDialogResize;
//btnSkinName.Visible = false;	
//lblUISkin.Visible = false;
//btnSkinDownCombox.Visible = false;	
