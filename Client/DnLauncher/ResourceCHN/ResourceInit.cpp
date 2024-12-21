#include "stdafx.h"
#include "ResourceInit.h"
#include "../DnControlData.h"
#include "../DnDefine_Launcher.h"

#define MAX_CTRL_NUM	29
CRect g_CtrlRect[MAX_CTRL_NUM];

void InitCtrlRect()
{
	DNCTRLDATA.SetControlDataFromResource(17001 /*IDR_XML_CONTROL_DATA*/, PATCH_CONTROL_NAME, g_CtrlRect, MAX_CTRL_NUM);

	/*
	g_CtrlRect[0].SetRect( 12, 44, 678, 410 );		// WebPageHtml Fit( 7, 39, 683, 414 )
	g_CtrlRect[1].SetRect( 514, 426, 672, 476 );	// BtnQuit
	g_CtrlRect[2].SetRect( 514, 426, 672, 476 );	// BtnStartGame
	g_CtrlRect[3].SetRect( 620, 549, 669, 573 );	// BtnOption
	g_CtrlRect[4].SetRect( 660, 10, 681, 31 );		// BtnWinClose
	g_CtrlRect[5].SetRect( 636, 10, 657, 31 );		// BtnWinMiniMum
	g_CtrlRect[6].SetRect( 86, 498, 460, 504 );		// ProgressDownload
	g_CtrlRect[7].SetRect( 86, 454, 460, 460 );		// ProgressCount
	g_CtrlRect[8].SetRect( 174, 15, 244, 30 );		// Text_LauncherVersion
	g_CtrlRect[9].SetRect( 0, 0, 0, 0 );			// Text_FileCount
	g_CtrlRect[10].SetRect( 80, 473, 300, 490 );	// Text_FileName
	g_CtrlRect[11].SetRect( 390, 427, 514, 444 );	// Text_TimeLeft
	g_CtrlRect[12].SetRect( 365, 473, 440, 490 );	// Text_FileSize
	g_CtrlRect[13].SetRect( 475, 447, 505, 464 );	// Text_DownloadPercentage
	g_CtrlRect[14].SetRect( 85, 427, 300, 444 );	// Text_DownloadState
	g_CtrlRect[15].SetRect( 531, 523, 672, 540 );	// Text_DirectStart
	g_CtrlRect[16].SetRect( 514, 523, 527, 536 );	// CheckBox_DIrectStart
	g_CtrlRect[17].SetRect( 514, 426, 672, 476 );	// BtnStartPatch
	g_CtrlRect[18].SetRect( 514, 484, 672, 517 );	// BtnSelectPartition
	g_CtrlRect[19].SetRect( 24, 522, 130, 540 );	// SelectPartition
	g_CtrlRect[20].SetRect( 130, 522, 239, 540 );	// SelectPartition Name
	g_CtrlRect[21].SetRect( 239, 522, 439, 540 );	// SelectPartition Guide
	g_CtrlRect[22].SetRect( 24, 427, 233, 444 );	// Static_Text_CurrentInfo
	g_CtrlRect[23].SetRect( 24, 473, 80, 490 );		// Static_Text_FileName
	g_CtrlRect[24].SetRect( 330, 473, 365, 490 );	// Static_Text_FileSize
	g_CtrlRect[25].SetRect( 300, 427, 390, 444 );	// Static_Text_TimeLeft
	g_CtrlRect[26].SetRect( 576, 15, 626, 36 );		// Static_Text_HomepageLink
	g_CtrlRect[27].SetRect( 24, 450, 88, 467 );		// Static_Text_TotalFile
	g_CtrlRect[28].SetRect( 24, 493, 88, 510 );		// Static_Text_CurrentFile
*/
}

void GetCtrlRect( CRect* pRect, int nMaxNum )
{
	if( pRect == NULL || nMaxNum != MAX_CTRL_NUM )
		return;

	for( int i=0; i<nMaxNum; i++ )
	{
		pRect[i] = g_CtrlRect[i];
	}
}
