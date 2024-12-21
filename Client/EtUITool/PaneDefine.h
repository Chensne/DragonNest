#pragma once

#include "TemplatePaneView.h"
#include "TemplatePropertyPaneView.h"
#include "ControlPropertyPaneView.h"
#include "LayerView.h"

enum PANE_DEFINE 
{
	LAYOUT_PANE = 200,
	TEMPLATE_PANE = 201,
	TEMPLATE_PROPERTY_PANE = 202,
	CONTROL_PROPERTY_PANE = 203,
	LAYER_PANE = 204,
};

struct PaneCreateStruct {
	int nPaneID;
	int nDockPaneID;
	int nAttachPaneID;
	CRuntimeClass *pClass;
	char *szName;
	XTPDockingPaneDirection Direction;
	int nLeft, nTop, nRight, nBottom;
	BOOL bShow;

	CXTPDockingPane *pThis;
	BOOL bAutoShowHide;
};

static struct PaneCreateStruct g_PaneList[] = 
{
	{ LAYER_PANE, -1, -1, RUNTIME_CLASS( CLayerView ), "Layer", xtpPaneDockBottom, 0, 0, 230, 130, TRUE, NULL, FALSE },
	{ TEMPLATE_PANE, -1, -1, RUNTIME_CLASS( CTemplatePaneView ), "Template", xtpPaneDockLeft, 0, 0, 230, 300, TRUE, NULL, FALSE },
	{ TEMPLATE_PROPERTY_PANE, 1, -1, RUNTIME_CLASS( CTemplatePropertyPaneView ), "Template Property", xtpPaneDockBottom, 0, 0, 230, 320, FALSE, NULL, TRUE },
	{ CONTROL_PROPERTY_PANE, 1, -1, RUNTIME_CLASS( CControlPropertyPaneView ), "Control Property", xtpPaneDockBottom, 0, 0, 230, 320, TRUE, NULL, TRUE },	
};

#define GetPaneWnd(id)	((CMainFrame*)AfxGetMainWnd())->GetPaneFrame(id);