function DefaultAdcolumnBarOnResize(Sender){
	//  informing the user after resizing
	btnPageDown.top = Sender.height - 14;
	
	//btnPageUp.visible = true;
	//btnPageUp.caption = DefaultAdcolumnBar.ActiveWidgetItem.WidgetNameSpace;
	
	// DefaultAdcolumnBar.ActiveWidgetItem  activate current WidgetItem of AdcolumnBar
	// DefaultAdcolumnBar.ActiveWindows     activate current widget window
	
}


DefaultAdcolumnBar.onresize="DefaultAdcolumnBarOnResize(Self);"; 
