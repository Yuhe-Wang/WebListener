
// WebListener.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CWebListenerApp:
// 有关此类的实现，请参阅 WebListener.cpp
//

class CWebListenerApp : public CWinAppEx
{
public:
	CWebListenerApp();

// 重写
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
//数据
	HANDLE m_hMutex;//互斥量，用于检测实例
};

extern CWebListenerApp theApp;