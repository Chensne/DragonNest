function DefaultAdcolumnBarOnResize(Sender){
	// �ı��С���յ�֪ͨ
	btnPageDown.top = Sender.height - 14;
	
	//btnPageUp.visible = true;
	//btnPageUp.caption = DefaultAdcolumnBar.ActiveWidgetItem.WidgetNameSpace;
	
	// DefaultAdcolumnBar.ActiveWidgetItem �õ���ǰ��������WidgetItem
	// DefaultAdcolumnBar.ActiveWindows    �õ���ǰ��ʾ��widget�Ĵ���
	
}


DefaultAdcolumnBar.onresize="DefaultAdcolumnBarOnResize(Self);"; 
