#pragma once

#include "ActionPaneView.h"
#include "PropertiesPaneView.h"
#include "EventPaneView.h"
#include "LightPaneView.h"

enum PANE_DEFINE {
	ACTION_PANE = 200,
	PROPERTY_PANE,
	EVENT_PANE,
	LIGHT_PANE,
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

static struct PaneCreateStruct g_PaneList[] = {
	{ EVENT_PANE, -1, -1, RUNTIME_CLASS( CEventPaneView ), "Event Sequence", xtpPaneDockBottom, 0, 0, 300, 255, TRUE, NULL, TRUE },
	{ ACTION_PANE, -1, -1, RUNTIME_CLASS( CActionPaneView ), "ActionObject", xtpPaneDockLeft, 0, 0, 300, 540, TRUE, NULL, TRUE },
	{ PROPERTY_PANE, 1, -1, RUNTIME_CLASS( CPropertiesPaneView ), "Properties", xtpPaneDockBottom, 0, 0, 300, 250, TRUE, NULL, TRUE },
	{ LIGHT_PANE, -1, 1, RUNTIME_CLASS( CLightPaneView ), "Lights", xtpPaneDockLeft, 0, 0, 300, 540, TRUE, NULL, TRUE },
};

#define GetPaneWnd(id)	((CMainFrame*)AfxGetMainWnd())->GetPaneFrame(id);