function DefaultAdcolumnBarOnResize(Sender){
	// 改变大小后收到通知
	btnPageDown.top = Sender.height - 14;
	
	//btnPageUp.visible = true;
	//btnPageUp.caption = DefaultAdcolumnBar.ActiveWidgetItem.WidgetNameSpace;
	
	// DefaultAdcolumnBar.ActiveWidgetItem 得到当前广告边栏的WidgetItem
	// DefaultAdcolumnBar.ActiveWindows    得到当前显示的widget的窗口
	
}


DefaultAdcolumnBar.onresize="DefaultAdcolumnBarOnResize(Self);"; 
