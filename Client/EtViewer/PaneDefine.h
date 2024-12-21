#pragma once

#include "WorkspacePaneView.h"
#include "PropShaderPaneView.h"
#include "AniListPaneView.h"

enum PANE_DEFINE {
	WORKSPACE_PANE = 200,
	PROP_SHADER_PANE,
	ANI_PANE,
};

struct PaneCreateStruct {
	int nPaneID;
	int nDockPaneID;
	int nAttackPaneID;
	CRuntimeClass *pClass;
	char *szName;
	XTPDockingPaneDirection Direction;
	int nLeft, nTop, nRight, nBottom;
	BOOL bShow;

	CXTPDockingPane *pThis;
};

static struct PaneCreateStruct g_PaneList[] = {
	{ WORKSPACE_PANE, -1, -1, RUNTIME_CLASS( CWorkspacePaneView ), "Workspace", xtpPaneDockLeft, 0, 0, 320, 400, TRUE, NULL },
	{ PROP_SHADER_PANE, 0, -1, RUNTIME_CLASS( CPropShaderPaneView ), "Properties", xtpPaneDockBottom, 0, 0, 320, 250, TRUE, NULL },
	{ ANI_PANE, -1, 1, RUNTIME_CLASS( CAniListPaneView ), "Animation", xtpPaneDockBottom, 0, 0, 320, 250, TRUE, NULL },
};

#define GetPaneWnd(id)	((CMainFrame*)AfxGetMainWnd())->GetPaneFrame(id);