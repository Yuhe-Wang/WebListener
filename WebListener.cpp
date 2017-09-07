
// WebListener.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "WebListener.h"
#include "WebListenerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWebListenerApp

BEGIN_MESSAGE_MAP(CWebListenerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWebListenerApp ����

CWebListenerApp::CWebListenerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CWebListenerApp ����

CWebListenerApp theApp;


// CWebListenerApp ��ʼ��

BOOL CWebListenerApp::InitInstance()
{
	m_hMutex = CreateMutex(NULL, TRUE, "WebListener");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//ReleaseMutex(m_hMutex);
		MessageBox(NULL, "WebListener�Ѿ��ں�̨����!", "����", MB_OK);
		return FALSE;
	}
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	//SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CWebListenerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.Create(IDD_WEBLISTENER_DIALOG); 
	if(dlg.m_autorun && !dlg.m_normal_exit ) dlg.ShowWindow(SW_HIDE);
	dlg.RunModalLoop();
	//INT_PTR nResponse = dlg.DoModal();
	/*
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	*/

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}


int CWebListenerApp::ExitInstance()
{
	CloseHandle(m_hMutex);
	return CWinApp::ExitInstance();
}