//==============================================================================

// 20091102 Add by Xiebin

// Please try to downsize reduntant and useless codings

//==============================================================================



var cFixedValue = 97;

var g_BartopbgHeight = 42; // the height of Quanquan icon is variable so as to add ads

var g_FixBtnCount = 0;

var g_FixBtnAry = null;

var g_FixBgLabelAry = null;

var g_Advertising = null;

var g_AdvertisingHeight = 0;

var g_MinSizeHT = 36;//  unnecessary

// more buttons

var g_AddMoreBtn = null;

// External button height for buttons such as  "More" or "Recomend" etc,which should be top-posted.

var g_BaseBtnHeight = 0;



var g_SpecialTopWidgetBtn = null;



var g_defHT = 0

var g_temptop = 0

var g_temptop2 = 0





function btnBarvFormCloseOnClick(Sender){

  System.Content.OpenIE("MessageHistory", "{config(IGP,MessageHistory)}", 0, 0, 535, 370, "xul=WebBrowserNoHelp.xml;ap=center;sf=true;cp=true;tg=true;ac=true");

 taskbarsvertical.FlashMessageIcon = false;

}



function OnTaskBarBefoClear(Sender)

{	//  it might update 3 times for the first startup

	// 1.loading default  \WidgetData\RecommendList\DefaultRecommend.xml

	// 2.NetReg update done successfully 

	// 3.new widget has been downloaded successfully 

	//System.Debug.outputString("OnTaskBarBefoClear() taskbarsvertical.barstop(" + taskbarsvertical.barstop + ")" + taskbarsvertical.BarStyle +  "\n");

	g_FixBgLabelAry = new Array(0);

	g_FixBtnAry = new Array(0);

	g_FixBtnCount = 0;

	g_Advertising = null;

	g_AddMoreBtn = null;

	g_SpecialTopWidgetBtn = null;

	g_BartopbgHeight = 42;

	lblBgAdvertising.visible = false;

	Bartopbg.height = g_BartopbgHeight;

}



function RefreshFixBtnPos(){

	// update fixed widget position

	var nBaseTop = 0;

	g_BaseBtnHeight = 0;

	if (g_AddMoreBtn)

	{// get more height for widget

		g_BaseBtnHeight = g_BaseBtnHeight + g_AddMoreBtn.height;

	}

	if (g_SpecialTopWidgetBtn)

	{// get the height of special Top widget button

		g_BaseBtnHeight = g_BaseBtnHeight + g_SpecialTopWidgetBtn.height;

	}



	if (taskbarsvertical.BarMode == 0)

	{

		lablebarline.top = 25 + g_FixBtnCount*23;

		taskbarsvertical.barsheight = taskbarsvertical.height - 60 - g_FixBtnCount*23 - g_AdvertisingHeight - g_BaseBtnHeight;

		Bartopbg.top = -g_BartopbgHeight;

		

        if (taskbarsvertical.BarsHideShow4Advertising)

        {

			switch(taskbarsvertical.BarStyle)

			{

				case 0: //  no space for ads.you can initialize some parameters here

					taskbarsvertical.barstop = 32 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;	

					break;

				case 1: // set the ads posotion

					if (g_Advertising)

					{	

		  				g_Advertising.top = 27 + g_FixBtnCount*23 + g_BaseBtnHeight;

					}

					taskbarsvertical.barstop = 32 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;	

					break;

				case 2:

					taskbarsvertical.barstop = 31 + g_FixBtnCount*23 + g_BaseBtnHeight;

					if (g_Advertising)

					{

						if (taskbarsvertical.barshide)

						{

							g_Advertising.top = 27 + g_FixBtnCount*23 + g_BaseBtnHeight;

						}

						else						

		   					g_Advertising.top = taskbarsvertical.barstop + taskbarsvertical.barsheight + 2;

					}

					break;

				default:

					taskbarsvertical.barstop = 32 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;	

			}// switch

			nBaseTop = 25;

        }

		else

		{

			switch(taskbarsvertical.BarStyle)

			{

				case 0: // no space for ads.you can initialize some parameters here

					taskbarsvertical.barstop = 32 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;	

				break;

				case 1: // set ads position

					if (g_Advertising)

					{

						if (!taskbarsvertical.barshide)

							g_Advertising.top = 27 + g_FixBtnCount*23 + g_BaseBtnHeight;			

					}

					taskbarsvertical.barstop = 32 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;	

					break;

				case 2:

					taskbarsvertical.barstop = 31 + g_FixBtnCount*23 + g_BaseBtnHeight;

					if (g_Advertising)

					{

						if (!taskbarsvertical.barshide)

							g_Advertising.top = taskbarsvertical.barstop + taskbarsvertical.barsheight + 2;

					}

					break;

				default:

					taskbarsvertical.barstop = 32 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;	

			}// switch		

			nBaseTop = 25;

		}

	}// if (taskbarsvertical.BarMode == 0)

	

	if (taskbarsvertical.BarMode == 1)

	{

		lablebarline.top = 12 + g_FixBtnCount*23;

		taskbarsvertical.barsheight = taskbarsvertical.height - 60 - g_FixBtnCount*23 - g_AdvertisingHeight - g_BaseBtnHeight;

		if (taskbarsvertical.BarsHideShow4Advertising)

		{

			switch(taskbarsvertical.BarStyle)

			{

				case 0: // no space for ads.you can initialize some parameters here

					taskbarsvertical.barstop = 19 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;	

					break;

				case 1: // set ads posotion

					if (g_Advertising)

					{

						//if (!taskbarsvertical.barshide)

						g_Advertising.top = 14 + g_FixBtnCount*23 + g_BaseBtnHeight;

					}

					taskbarsvertical.barstop = 20 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

					break;

				case 2: 

					taskbarsvertical.barstop = 17 + g_FixBtnCount*23 + g_BaseBtnHeight;

					if (g_Advertising)

					{

						if (taskbarsvertical.barshide)

						{

							g_Advertising.top = 14 + g_FixBtnCount*23 + g_BaseBtnHeight;

						}

						else

						g_Advertising.top = taskbarsvertical.barstop + taskbarsvertical.barsheight + 2;

					}

					break;

				default:

					taskbarsvertical.barstop = 19 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

			}// switch

			nBaseTop = 12;

		}

		else

		{ 

			switch(taskbarsvertical.BarStyle)

			{

				case 0: // no space for ads.you can initialize some parameters here

					taskbarsvertical.barstop = 19 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;	

					break;

				case 1: // set ads posotion

					if (g_Advertising)

					{

						if (!taskbarsvertical.barshide)

							g_Advertising.top = 14 + g_FixBtnCount*23 + g_BaseBtnHeight;

					}

					taskbarsvertical.barstop = 20 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

					break;

				case 2: 

					taskbarsvertical.barstop = 17 + g_FixBtnCount*23 + g_BaseBtnHeight;

					if (g_Advertising)

					{

						if (!taskbarsvertical.barshide)

							g_Advertising.top = taskbarsvertical.barstop + taskbarsvertical.barsheight + 2;

					}

					break;

				default:

					taskbarsvertical.barstop = 19 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

			}// switch

			nBaseTop = 12;

		}

	}// if (taskbarsvertical.BarMode == 1)

	

	//System.Debug.outputString("RefreshFixBtnPos() nBaseTop(" + nBaseTop + ") BarStyle=" + taskbarsvertical.BarStyle + " BarMode=" + taskbarsvertical.BarMode + "\n");

	for(i=0; i<g_FixBtnCount; i++)	

	{

		g_FixBtnAry[i].top = nBaseTop;

		g_FixBgLabelAry[i].top = nBaseTop;

		nBaseTop = nBaseTop + 23;

	}

	if (g_AddMoreBtn)

	{// display more buttons

		g_AddMoreBtn.top = nBaseTop;

		nBaseTop = nBaseTop + g_AddMoreBtn.height;

		g_AddMoreBtn.visible = true;

	}

	if (g_SpecialTopWidgetBtn)

	{//  display special top Widget Bottons

		g_SpecialTopWidgetBtn.top = nBaseTop; 

		g_SpecialTopWidgetBtn.visible = true;

	}

 	//System.Debug.outputString("RefreshFixBtnPos() taskbarsvertical.barstop(" + taskbarsvertical.barstop + ")" + taskbarsvertical.BarStyle +  "\n");

}



function OnAdvertisingClose(){

	//System.Debug.outputString("OnAdvertisingClose() BarStyle=" + taskbarsvertical.BarStyle + " \n");		

	g_Advertising = null;

	g_AdvertisingHeight = 0;

	g_BartopbgHeight = 42;

	Bartopbg.height = g_BartopbgHeight;

	lblBgAdvertising.visible = false;

	

    RefreshFixBtnPos();

    if (taskbarsvertical.BarsHideShow4Advertising && taskbarsvertical.barshide)

    {

		taskbarsverticalOnTaskBarsHideChanged(taskbarsvertical);

    }

}



function OnAddMoreBtnClick(Sender)

{

	taskbarsvertical.OpenQQToolsForm();

}



function OnTaskBarBeforeFixTopListUpdate(Sender)

{ // create necessary component according to PreBarStyle and set parameters before updating the interface

	//System.Debug.outputString("OnTaskBarBeforeFixTopListUpdate() PreBarStyle=" + taskbarsvertical.PreBarStyle + "\n");

	switch(taskbarsvertical.PreBarStyle)

	{

		case 0: // no space for ads.you can initialize some parameters here

			g_AdvertisingHeight = 0;

		break;

		case 1: // create ads position

		case 2:

			g_AdvertisingHeight = taskbarsvertical.BarAdvertisingHeight;

			if ( g_AdvertisingHeight < 0 )

			{ // security first

				g_AdvertisingHeight = 0;

			}

			var AdUrl = taskbarsvertical.BarAdvertisingURL;

			if ( (g_AdvertisingHeight > 0) && (AdUrl.length > 0) )

			{

				g_Advertising = taskbarsvertical.CreateControls("webbrowser");

				g_Advertising.height = g_AdvertisingHeight;

				g_Advertising.width = taskbarsvertical.BarsWidth + 1;				

				g_Advertising.left = 3;

				// it requires a special methord to set the link so as to prevent from being tampered  

				//g_Advertising.URL = 'D:\\MyTestAdvertising\\msgTip.html';

				g_Advertising.URL = AdUrl;

				g_Advertising.OnClose = OnAdvertisingClose;

				//  forbidding inputbox in ads

				g_Advertising.NoInput = true;

				g_Advertising.visible = false;

				//g_Advertising.BorderColor=0xFFFF0000;

			}

			else

			{ //  ads don't display without setting url

				g_AdvertisingHeight = 0;

			}

		break;

		default:// same with case 0

			g_AdvertisingHeight = 0;

	}// switch

	if (taskbarsvertical.HasMoreBtn)

	{//  "more"button displays

		g_AddMoreBtn = taskbarsvertical.CreateControls("ManagerButton");

		g_AddMoreBtn.left = 2;

		g_AddMoreBtn.width = 62;

		g_AddMoreBtn.height = 25;		

		g_AddMoreBtn.SkinName = "btn_barmorebtn";

		g_AddMoreBtn.visible = false;

		//g_AddMoreBtn.Link = ".\\DefaultManagerDialog.xml";

		g_AddMoreBtn.onclick = OnAddMoreBtnClick;

		

		g_AddMoreBtn.ShowHint = true;

		g_AddMoreBtn.Hint = "add tools";

		g_AddMoreBtn.name = "btnBarvFormSearch11";

	}

	else

	{// "more"button doesn't displays

		if (g_AddMoreBtn) 

		{

			g_AddMoreBtn.close;

			g_AddMoreBtn = null;

		}

	}// if (taskbarsvertical.HasMoreBtn)

	if (taskbarsvertical.HasNewTopOneApp)

	{//  special recommended widget displays	

		g_SpecialTopWidgetBtn = taskbarsvertical.CreateControls("widgetfaketaskbutton");

		// recommended widget settings 

		g_SpecialTopWidgetBtn.Icon = "btn_help_20x20_bar";

		g_SpecialTopWidgetBtn.SkinName = "bg_igwBarTopItem";

		g_SpecialTopWidgetBtn.Caption = "reserve";

		g_SpecialTopWidgetBtn.name = "SpecialTopWidgetBtn";

		

		g_SpecialTopWidgetBtn.width = 60;

		g_SpecialTopWidgetBtn.height = 25;

		g_SpecialTopWidgetBtn.left = 3;

		//g_SpecialTopWidgetBtn.BorderColor=0xffff0000;

		g_SpecialTopWidgetBtn.visible = false;

		taskbarsvertical.ProTopOneAppBtn(g_SpecialTopWidgetBtn);

	}

	else

	{// common recommended widget displays	

		if (g_SpecialTopWidgetBtn) 

			g_SpecialTopWidgetBtn.close;

		g_SpecialTopWidgetBtn = null;

		//taskbarsvertical.ProTopOneAppBtn(Empty);

	}

	

}



function OnTaskBarEndFixTopListUpdate(Sender)

{

	//System.Debug.outputString("OnTaskBarEndFixTopListUpdate() BarStyle=" + taskbarsvertical.BarStyle + "\n");

	switch(taskbarsvertical.BarStyle)

	{

		case 1: // create ads position

		case 2:

			g_AdvertisingHeight = taskbarsvertical.BarAdvertisingHeight;

			if ( g_AdvertisingHeight < 0 )

			{ // security first

				g_AdvertisingHeight = 0;

			}

			var AdUrl = taskbarsvertical.BarAdvertisingURL;

			if ( (g_AdvertisingHeight > 0) && (AdUrl.length > 0) )

			{

				if (g_Advertising)

				{

					g_Advertising.visible = true;

				}

			}

			else

			{ // ads don't display without ulr settings

				g_AdvertisingHeight = 0;

			}

		break;

		default:// same with case 0

			g_AdvertisingHeight = 0;

			if (g_Advertising)

			{

				g_Advertising.close;

				//System.Debug.outputString("OnTaskBarEndFixTopListUpdate() BarStyle=" + taskbarsvertical.BarStyle + "  g_Advertising.close\n");	

			}



	}// switch	

	RefreshFixBtnPos();



	if (taskbarsvertical.BarsHideShow4Advertising)

	{

		taskbarsvertical.MiniSize = g_MinSizeHT  + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

		if (!taskbarsvertical.barshide)

		{

			taskbarsvertical.MiniSize = taskbarsvertical.MiniSize + 17 + 30;

		}		

	}

	else

	{ 

		taskbarsvertical.MiniSize = 85+ g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

	}

       

	if (taskbarsvertical.height < taskbarsvertical.MiniSize)

	{// garantee enough height for adding more top widgets

		taskbarsvertical.height = taskbarsvertical.MiniSize;

	}

}



function OnTaskBarfixtoplistupdate(Sender)

{ // update top-up widgets and their position according to top-up wedgets configurition

	g_FixBtnCount = taskbarsvertical.RecommendListCount;

	//System.Debug.outputString("OnTaskBarfixtoplistupdate()begin g_FixBtnCount=" + g_FixBtnCount + " BarStyle="+ taskbarsvertical.BarStyle +"\n");

	if ( g_FixBtnCount == 0)

	{ //  initialize without top-up widgets

		lablebarline.visible = false;	

		lablebarlineTop.visible = false;

	}

	else

	{ // create and congfig it according to the number of top-up wedgets

		lablebarline.visible = true;

		lablebarlineTop.visible = true;	

		g_FixBtnAry = new Array(g_FixBtnCount);

		g_FixBgLabelAry = new Array(g_FixBtnCount);

		

		// create background

		for(i=0; i<g_FixBtnCount; i++)	

		{

			g_FixBgLabelAry[i] = taskbarsvertical.CreateControls("label");

			// config

			g_FixBgLabelAry[i].SkinName = "bg_igwBarTopItemNormalbk"; 

			g_FixBgLabelAry[i].width = 60;

			g_FixBgLabelAry[i].height = 25;

			g_FixBgLabelAry[i].left = 3;			

		}

		// top-up widgets

		for(i=0; i<g_FixBtnCount; i++)	

		{

			g_FixBtnAry[i] = taskbarsvertical.CreateControls("widgetfaketaskbutton");

			// config

			g_FixBtnAry[i].Icon = "btn_help_20x20_bar";

			g_FixBtnAry[i].SkinName = "bg_igwBarTopItem";

			g_FixBtnAry[i].Caption = "reserve";

			

			g_FixBtnAry[i].width = 60;

			g_FixBtnAry[i].height = 25;

			g_FixBtnAry[i].left = 3;

			//g_FixBtnAry[i].BorderColor=0xffff0000;

			g_FixBtnAry[i].visible = true;

			taskbarsvertical.AddFixTopList(g_FixBtnAry[i]);			

		}		

	}

	

	switch(taskbarsvertical.BarStyle)

	{

		case 0: // no space for ads.you can initialize some parameters here

			g_AdvertisingHeight = 0;

		break;

		case 1: // create ads position

		case 2:

			g_AdvertisingHeight = taskbarsvertical.BarAdvertisingHeight;

			if ( g_AdvertisingHeight < 0 )

			{ // security first

				g_AdvertisingHeight = 0;

			}

			var AdUrl = taskbarsvertical.BarAdvertisingURL;

			if ( (g_AdvertisingHeight > 0) && (AdUrl.length > 0) )

			{

				//g_Advertising = taskbarsvertical.CreateControls("webbrowser");

				if (g_Advertising)

				{

					g_Advertising.visible = true;

				}

				/*

				g_Advertising.height = g_AdvertisingHeight;

				g_Advertising.width = taskbarsvertical.BarsWidth + 1;				

				g_Advertising.left = 3;

				// better to set the link in some special way so as to prevent from being prampered

				//g_Advertising.URL = 'D:\\MyTestAdvertising\\msgTip.html';

				g_Advertising.URL = AdUrl;

				g_Advertising.OnClose = OnAdvertisingClose;

				// forbiding inputing areas in ads

				g_Advertising.NoInput = true;

				g_Advertising.visible = false;

				*/

			}

			else

			{ // not display without setting

				g_AdvertisingHeight = 0;

			}

		break;

		default:// same with case 0

			g_AdvertisingHeight = 0;

	}

	

	RefreshFixBtnPos();



	if (taskbarsvertical.BarsHideShow4Advertising)

	{

		taskbarsvertical.MiniSize = g_MinSizeHT  + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

		if (!taskbarsvertical.barshide)

		{

			taskbarsvertical.MiniSize = taskbarsvertical.MiniSize + 17 + 30;

		}

	}

	else

	{ 

		taskbarsvertical.MiniSize = 85+ g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

	}



	if (taskbarsvertical.height < taskbarsvertical.MiniSize)

	{// garantee enough height to add more pop-up widgets

		taskbarsvertical.height = taskbarsvertical.MiniSize;

	}

	//System.Debug.outputString("OnTaskBarfixtoplistupdate()end g_FixBtnCount=" + g_FixBtnCount + " BarStyle="+ taskbarsvertical.BarStyle +"\n");

}



function taskbarsverticalOnResizeOne(Sender){

	//System.Debug.outputString("taskbarsverticalOnResizeOne() BarStyle="+ taskbarsvertical.BarStyle +"\n");

	// above icon

	btnBarvFormClose.top = Sender.height - 18;

	btnBarvFormSearch.top = btnBarvFormClose.top;

	btnBarvFormSet.top = btnBarvFormClose.top - 1;

	btnBarvFormCap.top = btnBarvFormClose.top;

	btnBarvFormHelp.top = btnBarvFormClose.top;



	btnBarvFormPageUp.top = Sender.height - 41;

	btnBarvFormPageDown.top = btnBarvFormPageUp.top;

	lablePageTotal.top = Sender.height - 38;

	

	//Sender.barsheight = Sender.height - 55 - cFixedValue;

	Sender.barsheight = Sender.height - 60 - g_FixBtnCount*23 - g_AdvertisingHeight - g_BaseBtnHeight;



	Bartopbg.top = Sender.height;



	lablePageTotal.caption = Sender.PageNum + "/" + Sender.PageCount();

	switch(taskbarsvertical.BarStyle)

	{

		case 2:	

			if (g_Advertising)

			{

				if (taskbarsvertical.BarsHideShow4Advertising)

				{

					if (taskbarsvertical.barshide)

					{

						g_Advertising.top = 14 + g_FixBtnCount*23 + g_BaseBtnHeight;

					}

					else

						g_Advertising.top = taskbarsvertical.barstop + taskbarsvertical.barsheight + 2;

				}

				else

				{ 

					if (!taskbarsvertical.barshide)

						g_Advertising.top = taskbarsvertical.barstop + taskbarsvertical.barsheight + 2;

				}

			}

		break;

	}// switch

}



function taskbarsverticalOnResizeTwo(Sender)

{

	//System.Debug.outputString("taskbarsverticalOnResizeTwo() BarStyle="+ taskbarsvertical.BarStyle +"\n");

	// below the icon

	btnBarvFormPageUp.top = Sender.height - 28;

	btnBarvFormPageDown.top = btnBarvFormPageUp.top;

	btnBarvFormBarResize.top = Sender.height - 6;

	lablePageTotal.top = Sender.height - 25;

	//Sender.barsheight = Sender.height - 55 - cFixedValue;

	Sender.barsheight = Sender.height - 60 - g_FixBtnCount*23 - g_AdvertisingHeight - g_BaseBtnHeight;

	

	lablePageTotal.caption = Sender.PageNum + "/" + Sender.PageCount();

	switch(taskbarsvertical.BarStyle)

	{

		case 2:	

			if (g_Advertising)

			{

				if (taskbarsvertical.BarsHideShow4Advertising)

				{

					if (taskbarsvertical.barshide)

					{// while BarsHideShow4Advertising and minimized

						g_Advertising.top = 27 + g_FixBtnCount*23 + g_BaseBtnHeight;						

					}

					else

						g_Advertising.top = taskbarsvertical.barstop + taskbarsvertical.barsheight + 2;

				}

				else

				{

					if (!taskbarsvertical.barshide)

						g_Advertising.top = taskbarsvertical.barstop + taskbarsvertical.barsheight + 2;

				}

			}

		break;

	}// switch

}



function btnBarvFormSetOnClick(Sender){

	CtrlDownWidgetItemMenu.PopupByWin(btnBarvFormSet);

}



function btnBarvFormBarResizeOnClick(Sender){

	if (taskbarsvertical.height == 550)

	  taskbarsvertical.height = 300;

	else

		taskbarsvertical.height = 550;	

}



function btnBarvFormSearchOnClick(Sender){

	taskbarsvertical.AllowChangedIndex = !taskbarsvertical.AllowChangedIndex;

}



function btnBarvFormPageUpOnClick(Sender){

	taskbarsvertical.PageUp();

	lablePageTotal.caption = taskbarsvertical.PageNum + "/" + taskbarsvertical.PageCount();

}



function btnBarvFormPageDownOnClick(Sender){

	taskbarsvertical.PageDown();

	lablePageTotal.caption = taskbarsvertical.PageNum + "/" + taskbarsvertical.PageCount();

}



function taskbarsverticalOnTaskChanged(Sender){

	lablePageTotal.caption = Sender.PageNum + "/" + Sender.PageCount();

}



function BartopLogoOnclick(Sender){

	taskbarsvertical.BarsHide = !taskbarsvertical.BarsHide;	

}



function taskbarsverticalOnBarModeChanged(Sender)

{

	//System.Debug.outputString("taskbarsverticalOnBarModeChanged begin(:" + taskbarsvertical.BarMode + ") barshide=" + taskbarsvertical.barshide + "\n");

	

	

	// while bar model switch

	if (taskbarsvertical.BarMode == 0)

	{// below icon

		if (Bartopbg.top != -g_BartopbgHeight)

		{

		        var t;

			t = taskbarsvertical.top + taskbarsvertical.height + Bartopbg.height;



			btnBarvFormBarResize.ResizeType = "gwrstBottom";

			taskbarsvertical.SkinName = "bg_igwbarsvertical";

			btnBarvFormBarResize.SkinName = "btn_barvresize_60x6";

			taskbarsvertical.onresize="taskbarsverticalOnResizeTwo(Self);";

			btnBarvFormSet.SkinName = "btn_barDownMenu_15x20";

				

			taskbarsverticalOnResizeTwo(taskbarsvertical);		

			taskbarsvertical.barstop = 28 + cFixedValue;

		

			taskbarsvertical.top = t;		

			Bartopbg.top = -g_BartopbgHeight;

			btnBarvFormClose.top = 2;

			btnBarvFormSearch.top = 2;

			btnBarvFormSet.top = 0;

			btnBarvFormCap.top = 2;

			btnBarvFormHelp.top = 2;

			// last ones make it better while up size turns down

			lablebarlineTop.top = 23;

		}

	}

	

	if (taskbarsvertical.BarMode == 1)

	{// above icon

		if (Bartopbg.top = -g_BartopbgHeight)

		{

			var k;

 

			k = taskbarsvertical.top - Bartopbg.height - taskbarsvertical.height;

			btnBarvFormBarResize.ResizeType = "gwrstTop";

			taskbarsvertical.SkinName = "bg_igwbarsvertical_mirror";

			btnBarvFormBarResize.SkinName = "btn_barvresize_mirror_60x6";

			taskbarsvertical.onresize="taskbarsverticalOnResizeOne(Self);";

			btnBarvFormSet.SkinName = "btn_barDownMenu_mirror_15x20";

			taskbarsverticalOnResizeOne(taskbarsvertical);			



			taskbarsvertical.barstop = 14 + cFixedValue;

			taskbarsvertical.top = k;

			btnBarvFormBarResize.top = 0;

		

			//  last ones make it better while up side turns down

			lablebarlineTop.top = 10;

		}// if (Bartopbg.top = -g_BartopbgHeight)

	}// if (taskbarsvertical.BarMode == 1)	

	RefreshFixBtnPos();

	//System.Debug.outputString("taskbarsverticalOnBarModeChanged end(:" + taskbarsvertical.BarMode + ")\n");

}



function OnTaskBarStyleChanged(Sender)

{ // responding while alting style

	//System.Debug.outputString("OnTaskBarStyleChanged(" + taskbarsvertical.BarStyle + ")\n");

}



function taskbarsverticalOnTaskBarsHideChanged(Sender)

{	

	//System.Debug.outputString("taskbarsverticalOnTaskBarsHideChanged begin(" + taskbarsvertical.BarStyle + ") MiniSize=" + taskbarsvertical.MiniSize + "\n");

	if (taskbarsvertical.BarsHideShow4Advertising)

	{

		g_BartopbgHeight = 42;

		taskbarsvertical.MiniSize = g_MinSizeHT  + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

		if (!taskbarsvertical.barshide)

		{

			taskbarsvertical.MiniSize = taskbarsvertical.MiniSize + 17 + 30;

		}

	}

	else

	{ 

		btnBarvFormPageUp.visible = true;

		btnBarvFormPageDown.visible = true;

		btnBarvFormBarResize.visible = true;

		lablePageTotal.visible = true;

		

		taskbarsvertical.MiniSize = 85 + g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

		if (taskbarsvertical.BarMode ==1)  

		{

			taskbarsverticalOnResizeOne(taskbarsvertical);

		}

		else

		{

			taskbarsverticalOnResizeTwo(taskbarsvertical);    

		}

	}// if (taskbarsvertical.BarsHideShow4Advertising)



	//  move

	if (taskbarsvertical.BarsHideShow4Advertising)

	{

		if (taskbarsvertical.barshide)

		{

			BartopLogo.hint = "open Quanquan";

			BartopLogo.showhint = true;

			

			g_defHT=taskbarsvertical.Height;

			g_temptop=taskbarsvertical.Height;

			g_temptop2=Bartopbg.top;



			btnBarvFormPageUp.visible = false;

			btnBarvFormPageDown.visible = false;

			btnBarvFormBarResize.visible = false;

			lablePageTotal.visible = false;			



			//System.Debug.outputString("taskbarsverticalOnTaskBarsHideChanged  1(:" + taskbarsvertical.BarMode + ")\n");



			if (taskbarsvertical.BarMode ==1) 

			{

				g_temptop2=taskbarsvertical.top;

				if (g_temptop2+taskbarsvertical.MiniSize<taskbarsvertical.BarModeSnapSize) 

				{// not allow changing model while Bartopbg.top00

					taskbarsvertical.Top=0;

					taskbarsvertical.Height= taskbarsvertical.MiniSize;  

					//System.Debug.outputString("taskbarsverticalOnTaskBarsHideChanged   g_temptop2 1(:" +   g_temptop2 + ")\n");

					return;

				}  

			}

			taskbarsvertical.Height= taskbarsvertical.MiniSize;

			if (taskbarsvertical.BarMode ==1)  

			{

				taskbarsverticalOnResizeOne(taskbarsvertical);

				taskbarsvertical.Top=taskbarsvertical.top+g_defHT-taskbarsvertical.Height; 

			}

			else

			{

				taskbarsverticalOnResizeTwo(taskbarsvertical);    

			}

			RefreshFixBtnPos();

		}

		else

		{

			BartopLogo.hint = "Minimize Quanquan";		

			BartopLogo.showhint = true; 	



			btnBarvFormPageUp.visible = true;

			btnBarvFormPageDown.visible = true;

			btnBarvFormBarResize.visible = true;

			lablePageTotal.visible = true;

			lablePageTotal.visible = true; 



			if (g_defHT<=(85+g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight)) 

			{

				//System.Debug.outputString("taskbarsverticalOnTaskBarsHideChanged   Rbc†Ng\Sÿ" + g_defHT + "\n");

				g_defHT=85+ g_AdvertisingHeight + g_FixBtnCount*23 + g_BaseBtnHeight;

			}

			

			g_temptop=taskbarsvertical.Height;

			taskbarsvertical.height=g_defHT;

			if (taskbarsvertical.BarMode ==1)  

			{

				taskbarsverticalOnResizeOne(taskbarsvertical);

				taskbarsvertical.Top=taskbarsvertical.top-g_defHT+g_temptop;

			}

			else

			{

				taskbarsverticalOnResizeTwo(taskbarsvertical);    

			} 

			RefreshFixBtnPos();	              

		}		

         return;

	}// if (taskbarsvertical.BarsHideShow4Advertising)



	if (taskbarsvertical.barshide)

	{

		BartopLogo.hint = "Open Quanquan";

		BartopLogo.showhint = true;

		if (taskbarsvertical.Height<=taskbarsvertical.MiniSize) 

			taskbarsvertical.Height=taskbarsvertical.MiniSize+5;

			

		g_defHT=taskbarsvertical.Height; 

		

		if (g_Advertising)

		{ //  minimizing ads can save resource

			

			if (taskbarsvertical.BarHideShowAdvertising)

			{

				taskbarsvertical.moveTo(g_Advertising, Bartopbg);

				g_BartopbgHeight = 42 + g_Advertising.height + 8;

				Bartopbg.height = g_BartopbgHeight;

				lblBgAdvertising.visible = true;

				lblBgAdvertising.height = g_Advertising.height + 8;

				g_Advertising.top = 46;

				g_Advertising.left = 3;

			}

			else

			{

				g_Advertising.visible = false;

				lblBgAdvertising.visible = false;

			}

			

		}

	}

	else

	{

		BartopLogo.hint = "minimize Quanquan";		

		BartopLogo.showhint = true;

		if (taskbarsvertical.Height<=taskbarsvertical.MiniSize)

			taskbarsvertical.Height=taskbarsvertical.MiniSize+5;

		g_defHT=taskbarsvertical.Height;

		

		if (g_Advertising)

		{

			g_Advertising.visible = true;

			lblBgAdvertising.visible = false;

			if (taskbarsvertical.BarHideShowAdvertising)

			{

				g_BartopbgHeight = 42;

				Bartopbg.height = g_BartopbgHeight;

				

				taskbarsvertical.moveTo(g_Advertising, taskbarsvertical);

				RefreshFixBtnPos();

				g_Advertising.left = 3;

			}

		}// if (g_Advertising)		

	}

	//System.Debug.outputString("taskbarsverticalOnTaskBarsHideChanged end(" + taskbarsvertical.BarStyle + ")\n");

}





function OnMenuItemBarItemAddClick(Sender)

{

	btnBarvFormSearch.NextLinkClose = false;

	btnBarvFormSearch.click();

	btnBarvFormSearch.NextLinkClose = true;

}



//--------transfer chinese characters to utf8 code-----------------------//

function EncodeUtf8(s1)

{

  var s = encodeURIComponent(s1);

  var sa = s.split("%");

  var retV ="";



  if(sa[0] != "")

  {

    retV = sa[0];

  }

  for(var i = 1; i < sa.length; i ++)

  {

    if(sa[i].substring(0,1) == "u")

    {

      retV += Hex2Utf8(Str2Hex(sa[i].substring(1,5)));

    }

    else

      retV += "%" + sa[i];

  }

      

  return retV;

}



function Str2Hex(s)

{

  var c = "";

  var n;

  var ss = "0123456789ABCDEF";

  var digS = "";



  for(var i = 0; i < s.length; i ++)

  {

    c = s.charAt(i);

    n = ss.indexOf(c);

    digS += Dec2Dig(eval(n));

  }



  //return value;

  return digS;

}



function Dec2Dig(n1)

{

  var s = "";

  var n2 = 0;



  for(var i = 0; i < 4; i++)

  {

    n2 = Math.pow(2,3 - i);

    if(n1 >= n2)

    {

      s += '1';

      n1 = n1 - n2;

    }

    else

      s += '0';

  }

 

  return s;

}



function Dig2Dec(s)

{

  var retV = 0;



  if(s.length == 4)

  {

    for(var i = 0; i < 4; i ++)

    {

      retV += eval(s.charAt(i)) * Math.pow(2, 3 - i);

    }

    return retV;

  }

  

  return -1;

} 



function Hex2Utf8(s)

{

  var retS = "";

  var tempS = "";

  var ss = "";

 

  if(s.length == 16)

  {

    tempS = "1110" + s.substring(0, 4);

    tempS += "10" +  s.substring(4, 10); 

    tempS += "10" + s.substring(10,16); 

    var sss = "0123456789ABCDEF";



    for(var i = 0; i < 3; i ++)

    {

      retS += "%";

      ss = tempS.substring(i * 8, (eval(i)+1)*8);

            

      retS += sss.charAt(Dig2Dec(ss.substring(0,4)));

      retS += sss.charAt(Dig2Dec(ss.substring(4,8)));

    }

    return retS;

  }

  

  return "";

} 



function OnbtnBarvFormHelpClick(Sender)

{

  var WName = "IGW";

  var WItem = IGW.WidgetList.item("sdoHelper");

  if (WItem)

  {

   	WItem.open(WName);

   	return;

  } 

   

  // OpenIE()to get sdohelper without widget

  var WURL  = "http://chhelp.sdrs.sdo.com:8080/sdchweb/default.asp?co=1&tp=3&ut=1&id=&gd=" +

              "&gm=" + WidgetAtom.Info.GameId +

              "&ar=" + WidgetAtom.Info.GameAreaId +

              "&sv=" + WidgetAtom.Info.GameServerId +

              "&rl=" + EncodeUtf8(WidgetAtom.User.NickName) +

              "&ck=" + encodeURI(WName);



  System.Content.OpenIE("SYSTEM:HELP", WURL, 0, 0, 420, 420, "xul=WebBrowser.xml;ap=center;sf=true;cp=true");

}



function OnlblUnReadMsgCaptionChange(Sender)

{

	var tmpCaption = "";

	tmpCaption = lblUnReadMsg.caption;

	////System.Debug.outputString("testtest display the countÿ(" + tmpCaption.length + ")" + tmpCaption + "\n");

	switch(tmpCaption.length)

	{

		case 1:

			lblUnReadMsg.left = 46;

			lblUnReadMsg.width = 27;

		break;

		case 2:

			lblUnReadMsg.left = 41;

			lblUnReadMsg.width = 32;

		break;

		case 3:

			lblUnReadMsg.left = 36;

			lblUnReadMsg.width = 37;

		break;

		case 4:

			lblUnReadMsg.left = 31;

			lblUnReadMsg.width = 42;

		break;

		case 5:

			lblUnReadMsg.left = 26;

			lblUnReadMsg.width = 47;

		break;

		default:

		  lblUnReadMsg.visible = false;		  

		break;

	}

}



// initializing

taskbarsverticalOnResizeTwo(taskbarsvertical);

// assign component which Resize sent message to

btnBarvFormBarResize.NotifyResizeControl = taskbarsvertical;

taskbarsvertical.onresize="taskbarsverticalOnResizeTwo(Self);";

taskbarsvertical.onbarmodechanged="taskbarsverticalOnBarModeChanged(Self);"

taskbarsvertical.onbarshidechanged="taskbarsverticalOnTaskBarsHideChanged(Self);";



taskbarsvertical.PopupMenu = CtrlAddWidgetItemMenu;

Bartopbg.PopupMenu = CtrlAddWidgetItemMenu;



//taskbarsvertical.barstop = 28 + cFixedValue;

Bartopbg.top = -g_BartopbgHeight;

Bartopbg.height = g_BartopbgHeight;

btnBarvFormClose.top = 2;

btnBarvFormSearch.top = 2;

btnBarvFormSet.top = 0;

btnBarvFormCap.top = 2;

btnBarvFormHelp.top = 2;

btnBarvFormSet.SkinName = "btn_barDownMenu_15x20";

// last ones make it better while up size turns down

//lablebarline.top = 117;

lablebarlineTop.top = 23;



//taskbarsvertical.BarMode = 0;

taskbarsvertical.BarModeSnapSize = 60;

//System.Debug.outputString("taskbarsvertical.barstop(" + taskbarsvertical.barstop + ")\n");