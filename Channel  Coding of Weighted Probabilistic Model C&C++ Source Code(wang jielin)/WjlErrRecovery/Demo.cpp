
// Demo.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Demo.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDemoApp

BEGIN_MESSAGE_MAP(CDemoApp, CWinAppEx)
END_MESSAGE_MAP()


// CDemoApp 构造
CDemoApp::CDemoApp()
{
}

// 唯一的一个 CDemoApp 对象

CDemoApp theApp;

// CDemoApp 初始化

BOOL CDemoApp::InitInstance()
{
	CWinAppEx::InitInstance();

	EnableTaskbarInteraction(FALSE);
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	
	CMainDlg dlgMain;
	dlgMain.DoModal();
	
	return TRUE;
}

int CDemoApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	return CWinAppEx::ExitInstance();
}
