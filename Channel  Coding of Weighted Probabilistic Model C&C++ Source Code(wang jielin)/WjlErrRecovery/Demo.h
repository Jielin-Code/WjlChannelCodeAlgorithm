
// Demo.h : Demo Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CDemoApp:
// �йش����ʵ�֣������ Demo.cpp
//

class CDemoApp : public CWinAppEx
{
public:
	CDemoApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
// ʵ��

public:
	DECLARE_MESSAGE_MAP()
};

extern CDemoApp theApp;
