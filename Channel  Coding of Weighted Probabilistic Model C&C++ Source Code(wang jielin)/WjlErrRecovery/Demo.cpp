
// Demo.cpp : ����Ӧ�ó��������Ϊ��
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


// CDemoApp ����
CDemoApp::CDemoApp()
{
}

// Ψһ��һ�� CDemoApp ����

CDemoApp theApp;

// CDemoApp ��ʼ��

BOOL CDemoApp::InitInstance()
{
	CWinAppEx::InitInstance();

	EnableTaskbarInteraction(FALSE);
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	
	CMainDlg dlgMain;
	dlgMain.DoModal();
	
	return TRUE;
}

int CDemoApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	return CWinAppEx::ExitInstance();
}
