
// WebListener.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWebListenerApp:
// �йش����ʵ�֣������ WebListener.cpp
//

class CWebListenerApp : public CWinAppEx
{
public:
	CWebListenerApp();

// ��д
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
//����
	HANDLE m_hMutex;//�����������ڼ��ʵ��
};

extern CWebListenerApp theApp;