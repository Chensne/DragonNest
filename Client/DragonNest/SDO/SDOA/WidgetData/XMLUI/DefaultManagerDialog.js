var WidgetList_RowCount = 2;   // Nrow count per page

var WidgetList_ColCount = 5;   // Ncolumn count per page

var WidgetList_TotalNum = WidgetList_RowCount * WidgetList_ColCount;  // Nwidget count per page

var WidgetList_PageCount = 0;  // total page count

var WidgetList_CurPage = 0;    // current page ( page number starting from zero)



var WidgetCount = IGW.WidgetList.count, WidgetIndex = 0;

var PageCount = (WidgetCount%WidgetList_TotalNum==0)?(WidgetCount/WidgetList_TotalNum):(Math.floor(WidgetCount/WidgetList_TotalNum)+1);

var WidgetList = IGW.WidgetList;





var G_SwitchVersionTitleID = 0;

// store the initial opened list regardless of list update

var G_CacheWidgetList;



var G_IsWidgetItemClick = false;

var G_FocesWidgetNameSpace = "";

var G_FocesWidgetRow=-1;

var G_FocesWidgetCol=-1;





function ChangeListPage(PageIndex) {

	try

	{

		//  update widget count

		WidgetCount = IGW.WidgetList.count, WidgetIndex = 0;

		PageCount = (WidgetCount%WidgetList_TotalNum==0)?(WidgetCount/WidgetList_TotalNum):(Math.floor(WidgetCount/WidgetList_TotalNum)+1);

		EdtPageInfo.max = PageCount;

		

		if (PageIndex >= PageCount) PageIndex = PageCount - 1;

		if (PageIndex < 0) PageIndex = 0;



		WidgetList_PageCount = PageCount;

		WidgetList_CurPage   = PageIndex;

		if (PageCount == 0)

		{

			//PageInfo.caption     = "page0/0 ";

			EdtPageInfo.value = 0;

			LblPageInfo.caption = "/0page";
		}

		else

		{

			//PageInfo.caption     = (PageIndex + 1) + "/" + PageCount + "page";

			EdtPageInfo.value = (PageIndex + 1);

			LblPageInfo.caption = "/" + PageCount + "Page"

		}



		WidgetIndex = WidgetList_CurPage * WidgetList_TotalNum;

		CacheWidgetIndex = 0;

		//WidgetListTitleWWW.caption = "WidgetCount=" + WidgetCount + " WidgetIndex=" + WidgetIndex;

		for (Row = 1; Row <= WidgetList_RowCount; Row++) {

			for (Col = 1; Col <= WidgetList_ColCount; Col++) {

				if (WidgetIndex < WidgetCount) {

					

					wi= WidgetList.item(WidgetIndex);

					G_CacheWidgetList[CacheWidgetIndex] = "";

					

					//wi= WidgetList.item(G_CacheWidgetList[WidgetIndex]);

					if (wi)

					{

						try	{

							G_CacheWidgetList[CacheWidgetIndex] = wi.WidgetNameSpace;

							eval("WidgetIcon_" + Row + "_" + Col + ".icon = wi.IconFile;");

							// cache widget status 

							eval("WidgetIcon_" + Row + "_" + Col + ".HotStatusImage = 'btn_WidgetBigStatus';");

							eval("WidgetIcon_" + Row + "_" + Col + ".WidgetState = wi.state;");

						}

						catch(e){

						}

						

						try	{

							eval("WidgetBG_" + Row + "_" + Col + ".caption = wi.name;");

						}

						catch(e){

						}

						try	{					

							eval("WidgetBG_" + Row + "_" + Col + ".visible = true;");

							eval("Widget_ItemBg_" + Row + "_" + Col + ".visible = true;");

						}

						catch(e){

						}						

						if (G_FocesWidgetNameSpace == wi.WidgetNameSpace) 

						{

							try	{

							eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsiconSelect';");

							}

							catch(e){

							}

							

						}

						else

						{

							try	{

							eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsicon';");

							}

							catch(e){

							}							

						}

					}

				else

					{

					eval("WidgetBG_" + Row + "_" + Col + ".visible = false;");

					eval("Widget_ItemBg_" + Row + "_" + Col + ".visible = false;");

					eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsicon';");

					//WidgetListTitleWWW.caption = "WidgetBG_" + Row + "_" + Col + ".visible = false;";

						

						}

					WidgetIndex++;

					CacheWidgetIndex++;

				}

				else {

					eval("WidgetBG_" + Row + "_" + Col + ".visible = false;");

					eval("Widget_ItemBg_" + Row + "_" + Col + ".visible = false;");

					eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsicon';");

					//WidgetListTitleWWW.caption = "WidgetBG_" + Row + "_" + Col + ".visible = false;";

				}

			}

		}

	}

	catch(e) {

		WidgetName_1_1.caption=e;

	}

	

}



function PrePageClick() {	

	ChangeListPage(WidgetList_CurPage - 1);

	//igwmanagerdlg.setfocus(true);

}



function NextPageClick() {

	ChangeListPage(WidgetList_CurPage + 1);

}



function FirstPageClick(){

	ChangeListPage(0);

}

function LastPageClick(){

	ChangeListPage(PageCount - 1);

}





function CommonSelectWidget(Sender, Row, Col) {	

	var WidgetIndex = ((Row - 1) * WidgetList_ColCount + (Col - 1));



	if (WidgetIndex < WidgetList_TotalNum) {

		//wi = WidgetList.item(WidgetIndex);

		//WidgetListTitleWWW.caption = G_CacheWidgetList[WidgetIndex];

		//WidgetListTitleWWW.caption = Sender.name;

		wi = WidgetList.item(G_CacheWidgetList[WidgetIndex]);

		if (wi)

		{

			WidgetDesc.caption = wi.name + "\n\n" + wi.description;

		

			btnauthorLogo.icon = wi.LogoFile;

			if (btnauthorLogo.icon == "")btnauthorLogo.icon = wi.IconFile;

			lblCopyright.caption = wi.Copyright;

			lblauthorName.caption = wi.authorName;

			lblauthorInfoURL.caption = wi.authorInfoURL;

			

			// added widgets displayed or not (2:not display in the toolbar;8:not display in the text version toolbar)

			try	{					

				if ((wi.AtBarID >= 0) || ((wi.invisible&(2|8)) != 0))

				{ 

					eval("WidgetAdd_" + Row + "_" + Col + ".caption = 'Display';");

					//eval("WidgetAdd_" + Row + "_" + Col + ".skinname = 'btn_managernormal_66x24';");

				}

				else

				{ 

					eval("WidgetAdd_" + Row + "_" + Col + ".caption = 'Add';");

					//eval("WidgetAdd_" + Row + "_" + Col + ".skinname = 'btn_managernormal_66x24';");

				}

			} catch(e) {};



			if (G_IsWidgetItemClick==true)

			{

				if ((G_FocesWidgetCol!=-1) && (G_FocesWidgetRow!=-1))

				{

					try	{

						eval("WidgetBG_" + G_FocesWidgetRow + "_" + G_FocesWidgetCol + ".skinname = 'bg_igwselectbarsicon';");

					}

					catch(e){

					}					

				}

				G_FocesWidgetNameSpace=wi.WidgetNameSpace;

				G_FocesWidgetRow=Row;

				G_FocesWidgetCol=Col;

				try	{

					eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsiconSelect';");

				}

				catch(e){

				}

			}

			

			

		}

		else

		{

			WidgetDesc.caption = G_CacheWidgetList[WidgetIndex] + "Non-existingÿ";

		

			btnauthorLogo.icon = "";			

			lblCopyright.caption = "";

			lblauthorName.caption = "";

			lblauthorInfoURL.caption = "";

		}

	

		

	}

}





function SelectWidget(Sender, Row, Col) {

  CommonSelectWidget(Sender, Row, Col);

}



function OpenWidget(Row, Col) {	

	var WidgetIndex = ((Row - 1) * WidgetList_ColCount + (Col - 1));

	if (WidgetIndex < WidgetList_TotalNum) {

		SelectWidget(Row, Col);

		wi = WidgetList.item(G_CacheWidgetList[WidgetIndex])

		if (wi)

		{

			wi.OpenWidget();

			// added widgets displayed or not (2:not display in the toolbar;8:not display in the text version toolbar)

			try	{					

				if ((wi.AtBarID >= 0) || ((wi.invisible&(2|8)) != 0))

				{ 

					eval("WidgetAdd_" + Row + "_" + Col + ".caption = 'Display';");

					//eval("WidgetAdd_" + Row + "_" + Col + ".skinname = 'btn_managernormal_66x24';");

				}

				else

				{ 

					eval("WidgetAdd_" + Row + "_" + Col + ".caption = 'Add';");

					//eval("WidgetAdd_" + Row + "_" + Col + ".skinname = 'btn_managernormal_66x24';");

				}

			} catch(e) {};

		}

	}

}



function btnManagerFormCloseOnClick(Sender){

	igwmanagerdlg.close();

}



var HoverCount = 0;



function onItemHover(Sender, Row, Col)

{

	if (eventArgs.Hover)

	{

		if (G_SwitchVersionTitleID != 1 )

		  SwitchVersionTitle(1);

		G_IsWidgetItemClick = false;  

		//eval(Sender.onclick);		

		CommonSelectWidget(Sender, Row, Col);

		G_IsWidgetItemClick = true;

		

		HoverCount++;

		//WidgetListTitleWWW.caption = "mouse hover" + Sender.name + " " + HoverCount + IGW.Version;

	}

	else

	{

		//SwitchVersionTitle(0);

		HoverCount--;

		//WidgetListTitleWWW.caption = "mouse out " + Sender.name + " " + HoverCount;

		if (G_FocesWidgetNameSpace != "")

		{

			wi = WidgetList.item(G_FocesWidgetNameSpace);

			if (wi)

			{

				WidgetDesc.caption = wi.name + "\n\n" + wi.description;

		

				btnauthorLogo.icon = wi.LogoFile;

				if (btnauthorLogo.icon == "")btnauthorLogo.icon = wi.IconFile;

				lblCopyright.caption = wi.Copyright;

				lblauthorName.caption = wi.authorName;

				lblauthorInfoURL.caption = wi.authorInfoURL;				

			}

		}

		

	}

}



function SwitchVersionTitle(vtID)

{

	// Switch Version information display

	if (vtID == 0)

	{// display IGW Version information

		WidgetVersionTitle.caption = "version information for Quanquan";

		WidgetVersionTitle.TextColor = 0xFF000000;

		btnauthorLogo.visible = false;

		lblauthorName.left = 330;

		lblauthorName.width = 145;

		lblauthorName.caption = "Quanquan";

		lblCopyright.left = 330;

		lblCopyright.width = 145;

		lblCopyright.caption = "versionÿ" + IGW.Version;

		lblauthorInfoURL.left = 330;

		lblauthorInfoURL.width = 210;

		lblauthorInfoURL.caption = "SNDA proprietary rights reserved ";

		WidgetDescTitle.caption = "Quanquan kit";

		WidgetDescTitle.TextColor = 0xFF000000;

		WidgetDesc.caption = "\n You can add widgets in Quanquan by the Quanquan kit,and the added widget would display in the toolbar.";

		G_SwitchVersionTitleID = 0;

	}

	else

	{//  display Widget Version information

		WidgetVersionTitle.caption = "version information for Quanquan Widget ";

		WidgetVersionTitle.TextColor = 0xFF000000;

		btnauthorLogo.visible = true;

		lblauthorName.left = 390;

		lblauthorName.width = 145;

		//lblauthorName.captiion = "Quanquan";

		lblCopyright.left = 390;

		lblCopyright.width = 145;

		//lblCopyright.caption = "versionÿ" + IGW.Version;

		lblauthorInfoURL.left = 390;

		lblauthorInfoURL.width = 145;

		//lblauthorInfoURL.caption = "SNDA proprietary rights reserved";

		WidgetDescTitle.caption = "notification";

		WidgetDescTitle.TextColor = 0xFF000000;

		G_SwitchVersionTitleID = 1;

	}

}



function InitWidgetList()

{

	// cache NameSpace of WidgetList

	G_CacheWidgetList = new Array(WidgetList_TotalNum);

	for (i = 0; i < WidgetList_TotalNum; i++)

   	{

   		G_CacheWidgetList[i] = "";

   	}

	

}



function OnDlgSetChanged(Sender)

{// only for updating status icon

	ChangeListPage(WidgetList_CurPage);

}



function OnEdtPageInfoEnterDown(){

	ChangeListPage(EdtPageInfo.value - 1);

}



EdtPageInfo.OnEnterDown=OnEdtPageInfoEnterDown;

EdtPageInfo.min = 0;



InitWidgetList();

ChangeListPage(0);

SwitchVersionTitle(0);



igwmanagerdlg.OnSetChanged = OnDlgSetChanged;