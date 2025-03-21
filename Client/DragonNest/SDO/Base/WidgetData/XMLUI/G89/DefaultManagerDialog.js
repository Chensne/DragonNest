var WidgetList_RowCount = 2;   // 一页显示的Widget行数
var WidgetList_ColCount = 5;   // 一页显示的Widget列数
var WidgetList_TotalNum = WidgetList_RowCount * WidgetList_ColCount;  // 一页内显示的Widget数量
var WidgetList_PageCount = 0;  // 总页数
var WidgetList_CurPage = 0;    // 当前页(0开始)

var WidgetCount = IGW.WidgetList.count, WidgetIndex = 0;
var PageCount = (WidgetCount%WidgetList_TotalNum==0)?(WidgetCount/WidgetList_TotalNum):(Math.floor(WidgetCount/WidgetList_TotalNum)+1);
var WidgetList = IGW.WidgetList;


var G_SwitchVersionTitleID = 0;
// 为使不列表更新影响这里保存初次打开时的列表
var G_CacheWidgetList;

var G_IsWidgetItemClick = false;
var G_FocesWidgetNameSpace = "";
var G_FocesWidgetRow=-1;
var G_FocesWidgetCol=-1;


function ChangeListPage(PageIndex) {
	try
	{
		// 刷新数量
		WidgetCount = IGW.WidgetList.count, WidgetIndex = 0;
		PageCount = (WidgetCount%WidgetList_TotalNum==0)?(WidgetCount/WidgetList_TotalNum):(Math.floor(WidgetCount/WidgetList_TotalNum)+1);
		EdtPageInfo.max = PageCount;
		
		if (PageIndex >= PageCount) PageIndex = PageCount - 1;
		if (PageIndex < 0) PageIndex = 0;

		WidgetList_PageCount = PageCount;
		WidgetList_CurPage   = PageIndex;
		if (PageCount == 0)
		{
			//PageInfo.caption     = "0/0页";
			EdtPageInfo.value = 0;
			LblPageInfo.caption = "/0页";
		}
		else
		{
			//PageInfo.caption     = (PageIndex + 1) + "/" + PageCount + "页";
			EdtPageInfo.value = (PageIndex + 1);
			LblPageInfo.caption = "/" + PageCount + "页"
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
							// 获取对应的Widget状态
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
							//eval("Widget_ItemBg_" + Row + "_" + Col + ".visible = true;");
						}
						catch(e){
						}						
						if (G_FocesWidgetNameSpace == wi.WidgetNameSpace) 
						{
							try	{
							eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsiconSelect_91x140';");
							}
							catch(e){
							}
							
						}
						else
						{
							try	{
							eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsicon_91x140';");
							}
							catch(e){
							}							
						}
					}
				else
					{
					eval("WidgetBG_" + Row + "_" + Col + ".visible = false;");
					//eval("Widget_ItemBg_" + Row + "_" + Col + ".visible = false;");
					eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsicon_91x140';");
					//WidgetListTitleWWW.caption = "WidgetBG_" + Row + "_" + Col + ".visible = false;";
						
						}
					WidgetIndex++;
					CacheWidgetIndex++;
				}
				else {
					eval("WidgetBG_" + Row + "_" + Col + ".visible = false;");
					//eval("Widget_ItemBg_" + Row + "_" + Col + ".visible = false;");
					eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsicon_91x140';");
					//WidgetListTitleWWW.caption = "WidgetBG_" + Row + "_" + Col + ".visible = false;";
				}
			}
		}
	}
	catch(e) {
		//WidgetName_1_1.caption=e;
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
			
			// 已经在Bar上的 或者 在Bar上不可见的(2: 不出现在Bar上; 8: 不出现在文字版Bar上)
			try	{					
				if ((wi.AtBarID >= 0) || ((wi.invisible&(2|8)) != 0))
				{ 
					eval("WidgetAdd_" + Row + "_" + Col + ".caption = '显 示';");
					//eval("WidgetAdd_" + Row + "_" + Col + ".skinname = 'btn_managernormal_66x24';");
				}
				else
				{ 
					eval("WidgetAdd_" + Row + "_" + Col + ".caption = '添 加';");
					//eval("WidgetAdd_" + Row + "_" + Col + ".skinname = 'btn_managernormal_66x24';");
				}
			} catch(e) {};

			if (G_IsWidgetItemClick==true)
			{
				if ((G_FocesWidgetCol!=-1) && (G_FocesWidgetRow!=-1))
				{
					try	{
						eval("WidgetBG_" + G_FocesWidgetRow + "_" + G_FocesWidgetCol + ".skinname = 'bg_igwselectbarsicon_91x140';");
					}
					catch(e){
					}					
				}
				G_FocesWidgetNameSpace=wi.WidgetNameSpace;
				G_FocesWidgetRow=Row;
				G_FocesWidgetCol=Col;
				try	{
					eval("WidgetBG_" + Row + "_" + Col + ".skinname = 'bg_igwselectbarsiconSelect_91x140';");
				}
				catch(e){
				}
			}
			
			
		}
		else
		{
			WidgetDesc.caption = G_CacheWidgetList[WidgetIndex] + "不存在！";
		
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
			// 已经在Bar上的 或者 在Bar上不可见的(2: 不出现在Bar上; 8: 不出现在文字版Bar上)
			try	{					
				if ((wi.AtBarID >= 0) || ((wi.invisible&(2|8)) != 0))
				{ 
					eval("WidgetAdd_" + Row + "_" + Col + ".caption = '显 示';");
					//eval("WidgetAdd_" + Row + "_" + Col + ".skinname = 'btn_managernormal_66x24';");
				}
				else
				{ 
					eval("WidgetAdd_" + Row + "_" + Col + ".caption = '添 加';");
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
		//WidgetListTitleWWW.caption = "鼠标旋停" + Sender.name + " " + HoverCount + IGW.Version;
	}
	else
	{
		//SwitchVersionTitle(0);
		HoverCount--;
		//WidgetListTitleWWW.caption = "鼠标离开" + Sender.name + " " + HoverCount;
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
	// 用于切换版本信息显示
	if (vtID == 0)
	{// 显示IGW版本信息
		WidgetVersionTitle.caption = "圈圈版本信息";
		WidgetVersionTitle.TextColor = 0xFFFFFFFF;
		btnauthorLogo.visible = false;
		lblauthorName.left = 330;
		lblauthorName.width = 145;
		lblauthorName.caption = "圈圈";
		lblCopyright.left = 330;
		lblCopyright.width = 145;
		lblCopyright.caption = "版本：" + IGW.Version;
		lblauthorInfoURL.left = 330;
		lblauthorInfoURL.width = 210;
		lblauthorInfoURL.caption = "SNDA Corporation保留所有权利 ";
		WidgetDescTitle.caption = "圈圈工具集";
		WidgetDescTitle.TextColor = 0xFFFFFFFF;
		WidgetDesc.caption = "\n您可以通过圈圈工具集添加应用，成功添加的应用将直接显示在圈圈工具条上。";
		G_SwitchVersionTitleID = 0;
	}
	else
	{// 显示Widget版本信息
		WidgetVersionTitle.caption = "Widget版本信息";
		WidgetVersionTitle.TextColor = 0xFFFFFFFF;
		btnauthorLogo.visible = true;
		lblauthorName.left = 390;
		lblauthorName.width = 145;
		//lblauthorName.captiion = "圈圈";
		lblCopyright.left = 390;
		lblCopyright.width = 145;
		//lblCopyright.caption = "版本：" + IGW.Version;
		lblauthorInfoURL.left = 390;
		lblauthorInfoURL.width = 145;
		//lblauthorInfoURL.caption = "SNDA Corporation保留所有权";
		WidgetDescTitle.caption = "描述信息";
		WidgetDescTitle.TextColor = 0xFFFFFFFF;
		G_SwitchVersionTitleID = 1;
	}
}

function InitWidgetList()
{
	// 得到WidgetList的NameSpace
	G_CacheWidgetList = new Array(WidgetList_TotalNum);
	for (i = 0; i < WidgetList_TotalNum; i++)
   	{
   		G_CacheWidgetList[i] = "";
   	}
	
}

function OnDlgSetChanged(Sender)
{// 目前仅用于刷新状态图标
	ChangeListPage(WidgetList_CurPage);
}

function OnEdtPageInfoEnterDown(){
	ChangeListPage(EdtPageInfo.value - 1);
}

function OnManagerDialogResize(Sender)
{
	btnManagerFormClose.Left = Sender.Width - 36;
	WidgetListTitleWWW.Width = Sender.Width - 180;
}


EdtPageInfo.OnEnterDown=OnEdtPageInfoEnterDown;
EdtPageInfo.min = 0;

InitWidgetList();
ChangeListPage(0);
SwitchVersionTitle(0);

igwmanagerdlg.OnSetChanged = OnDlgSetChanged;
igwmanagerdlg.onresize = OnManagerDialogResize;