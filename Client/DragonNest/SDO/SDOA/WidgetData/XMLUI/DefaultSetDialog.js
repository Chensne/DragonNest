
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

                       
			
			sliderHueValue.Visible =bVisible;
			sliderLuminanceValue.Visible =bVisible;
			sliderSaturationValue.Visible =bVisible;
			lblHue.Visible =bVisible;
			lblLuminance.Visible =bVisible;
			lblSaturation.Visible =bVisible;
			btnHLSColorNormal.Visible =bVisible;
			btnHLSColor01.Visible =bVisible;
			btnHLSColor02.Visible =bVisible;
			btnHLSColor03.Visible =bVisible;
			btnHLSColor04.Visible =bVisible;
			btnHLSColor05.Visible =bVisible;
			btnHLSColor06.Visible =bVisible;
			btnHLSColor07.Visible =bVisible;
			
			btnTabUISkin.Enabled = !bVisible;
		break;
		case 1:
			labelsoundRegion.Visible = bVisible;
			lblhintsoundvalueset.Visible = bVisible;
			btnTabAudio.Enabled = !bVisible;
		break;
		case 2:
			checkboxDisablePopupMsg.visible = bVisible;
			checkboxAllowPopupMsg.visible = bVisible;
			labelSystemSetRegion.visible = bVisible;
			checkboxAllowServerMsg.visible = bVisible;
                        //choose model
                        checkboxMinTypeSet.Visible= bVisible;
                      
			btnTabSystemSet.Enabled = !bVisible;			
		break;
		case 3:						
			btnTabCapScreenSet.Enabled = !bVisible;
			labelSystemSetScreenCap.visible = bVisible;
			
			//btnTabCapScreenSet.visible = false;
			//labelSystemSetScreenCap.visible = false;
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

OnbtnTabUISkinClick(btnTabUISkin);
editHotKey.readonly=true;
editCapBmpPath.readonly=true;
editCapBmpPath.popupmenu=EditMenuChangeDir;
//btnSkinName.Visible = false;	
//lblUISkin.Visible = false;
//btnSkinDownCombox.Visible = false;	
