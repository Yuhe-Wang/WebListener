
// WebListenerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <afxinet.h>

#include "WebListener.h"
#include "WebListenerDlg.h"

#include "string.h"
#include <iostream>
#include <fstream>
#include "ParserDom.h"
#include "Mmsystem.h" //����������

#define WM_TASKMANAGE WM_USER+300
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment( lib , "winmm.lib" )
#pragma comment( lib , "htmlcxx.lib" )  

CWebListenerDlg *g_dlg;//ȫ�ֶԻ���ָ��

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CWebListenerDlg �Ի���
//��̬��Ա��ʼ��
BOOL CWebListenerDlg::m_isFromFile=FALSE;
WebInf CWebListenerDlg::m_webinf={0,0,""};


CWebListenerDlg::CWebListenerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWebListenerDlg::IDD, pParent)
	, m_hour(0)
	, m_minute(5)
	, m_second(0)
	, m_autorun(FALSE)
	, m_normal_exit(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWebListenerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HOUR, m_hour);
	DDV_MinMaxUInt(pDX, m_hour, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_MUNITE, m_minute);
	DDV_MinMaxUInt(pDX, m_minute, 0, 59);
	DDX_Text(pDX, IDC_EDIT_SECOND, m_second);
	DDV_MinMaxUInt(pDX, m_second, 0, 59);
	DDX_Check(pDX, IDC_CHECK_FROMFILE, m_isFromFile);
	DDX_Check(pDX, IDC_CHECK_AUTORUN, m_autorun);
}

BEGIN_MESSAGE_MAP(CWebListenerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_TASKMANAGE,OnTaskManage)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_START, &CWebListenerDlg::OnBnClickedStart)
	ON_COMMAND(ID_MENU_EXIT, &CWebListenerDlg::OnCancel)
	ON_BN_CLICKED(IDC_EXIT, &CWebListenerDlg::OnCancel)
	ON_COMMAND(ID_STOP_LISTEN, &CWebListenerDlg::OnStopListen)
	ON_COMMAND(ID_ADJUST_FREQUENCY, &CWebListenerDlg::OnStopListen)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_AUTORUN, &CWebListenerDlg::OnBnClickedCheckAutorun)
END_MESSAGE_MAP()


// CWebListenerDlg ��Ϣ�������

BOOL CWebListenerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	g_dlg=this;
	m_listen_thread=NULL;
	//�������
	m_SkyNID.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_SkyNID.hWnd = this->m_hWnd;
	m_SkyNID.uID = IDR_TRAY_MENU;
	m_SkyNID.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	m_SkyNID.dwState = NIS_SHAREDICON;
	m_SkyNID.uCallbackMessage = WM_TASKMANAGE;
	m_SkyNID.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy_s(m_SkyNID.szTip, _T("׼������"));
	Shell_NotifyIcon(NIM_ADD, &m_SkyNID);

	if (State2File(FALSE)) //�ܴ������ļ���ȡ״̬
	{
		m_isFromFile=TRUE;
		UpdateData(FALSE);
		if (m_autorun && !m_normal_exit) OnBnClickedStart();//ֱ������
	}
	else //�����ļ��ָ�״̬����ѡ�򲻿���
	{
		GetDlgItem(IDC_CHECK_FROMFILE)->EnableWindow(FALSE);
	}
	//����ע���
	if(!setAutoRun(m_autorun))	OnCancel();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CWebListenerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWebListenerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CWebListenerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWebListenerDlg::OnBnClickedStart() //��ʼ����
{	
	if (m_listen_thread==NULL) //�����߳���δ����
	{
		m_listen_thread=AfxBeginThread(ListenThread,0,0,CREATE_SUSPENDED);
	}
	//����ʱ�䣬�趨��ʱ��,�����̨
	UpdateData(TRUE);
	if (!m_hour && !m_minute && !m_second)
	{
		MessageBox("ʱ�䲻��Ϊ��,���������ã�");
		return;
	}
	//State2File(TRUE);//���������˳����
	SetTimer(1,1000*(m_second+60*m_minute+3600*m_hour), NULL);
	//�޸�������ʾ
	CString tip,time;
	tip.Format("���ڼ���CSSA��̳��\n̽��ʱ����Ϊ��\n");
	if (m_hour != 0)
	{
		time.Format("%dСʱ",m_hour);
		tip+=time;
	}
	if (m_minute != 0)
	{
		time.Format("%d����", m_minute);
		tip+=time;
	}
	if (m_second != 0)
	{
		time.Format("%d��", m_second);
		tip+=time;
	}
	strcpy_s(m_SkyNID.szTip, tip);
	Shell_NotifyIcon(NIM_MODIFY, &m_SkyNID);
	ShowWindow(SW_HIDE);
}

std::string CWebListenerDlg::GBToUTF8(const std::string& str)
{
	std::string result;
	WCHAR *strSrc;
	TCHAR *szRes;
	//�����ʱ�����Ĵ�С
	int i = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	strSrc = new WCHAR[i+1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, strSrc, i);
	//�����ʱ�����Ĵ�С
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[i+1];
	int j=WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);
	result = szRes;
	delete []strSrc;
	delete []szRes;

	return result;
}

void CWebListenerDlg::GetWebInf(WebInf& webinf)
{
	using namespace std;
	///////////////Download Webpage//////////////////////
	CInternetSession httpSession;
	httpSession.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,6000);
	httpSession.SetOption(INTERNET_OPTION_SEND_TIMEOUT,60000);
	httpSession.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT,60000);
	/* ����������֮��ĵȴ�����ʱֵ�ں��뼶��*/
	httpSession.SetOption(INTERNET_OPTION_CONNECT_BACKOFF,500);
	/* ��������������ʱ�����Դ��������һ��������ͼ��ָ�������Դ�������ʧ�ܣ�������ȡ���� ȱʡֵΪ5��*/
	httpSession.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 3);

	CString url="http://gradpages.wustl.edu/cssa/forum";
	CInternetFile * htmlFile;
	try
	{
		htmlFile=(CInternetFile *)httpSession.OpenURL(url);
	}
	catch(CInternetException * m_pException)
	{
		m_pException->m_dwError;
		m_pException->Delete();
		httpSession.Close();  
		AfxMessageBox("���������쳣��\nWebListenner���˳���������ֱ�Ӵ���ҳ...");
		ShellExecute(0, NULL, url, NULL, NULL, 1);
		g_dlg->OnCancel();//�˳�����
		return;
	}
	CString Content;
	CString Result;
	if (htmlFile==NULL) return;
	while (htmlFile->ReadString(Content))
	{
		Result+=Content;
	}
	htmlFile->Close();
	httpSession.Close();
	string html=Result.GetBuffer(0);
	//////////////Parse the Webpage and get necessary data/////////////
	using namespace htmlcxx;
	HTML::ParserDom parser;
	parser.parse(html);
	tree<HTML::Node> tr=parser.getTree();
	tree<HTML::Node>::pre_order_iterator it = tr.begin();
	tree<HTML::Node>::pre_order_iterator end = tr.end();
	tree<HTML::Node>::pre_order_iterator pos;
	int num_posts=0;
	bool tag=false;
	string token1=GBToUTF8("���ֽ���");
	for(;it!=end;it++)
	{	
		if ((!it->isTag()) && (!it->isComment()) && it->text().find(token1.c_str())!=string::npos)
		{		
			++num_posts; //count the number of the posts
			if(!tag) {tag=true; pos=it;} //store the position of the first post
		}
	}
	pos=tr.parent(pos);
	pos->parseAttributes();
	CString link="http://gradpages.wustl.edu";
	link+=pos->attribute("href").second.c_str();
	//////////////////////////////////////////////////////////////////
	for (int i=0 ; i<5 ; i++, pos++){}
	int num_comments = _ttoi(pos->text().c_str());
	if (num_comments > 50) //ÿ��50ҳ������ҳ��
	{
		CString page_index;
		page_index.Format("?page=%d",num_comments/50);
		link+=page_index;
	}
	webinf.num_posts=num_posts;
	webinf.num_comments=num_comments;
	webinf.link=link;
}

UINT CWebListenerDlg::ListenThread(LPVOID p)
{
	bool initialized=false;
	while (true)
	{
		WebInf newinf;
		GetWebInf(newinf);
		if ( !m_isFromFile && !initialized )
		{
			m_webinf=newinf; //ֱ�Ӵ����ϸ���
			initialized=true;
			g_dlg->State2File(TRUE);
		}
		else 
		{
			if (newinf.num_posts!=m_webinf.num_posts) //��������
			{			
				//֪ͨ1
				g_dlg->State2File(TRUE);
				sndPlaySound( "Notify1.wav" , SND_ASYNC | SND_NODEFAULT );
				CString inf;
				inf.Format("CSSA������̳��%d��������!",newinf.num_posts-m_webinf.num_posts);
				m_webinf=newinf;
				AfxMessageBox(inf,MB_YESNO);
				ShellExecute(0, NULL,m_webinf.link, NULL, NULL, 1);
			}
			else if(newinf.num_comments!=m_webinf.num_comments) //û������������������
			{
				//֪ͨ2
				g_dlg->State2File(TRUE);
				sndPlaySound( "Notify2.wav" , SND_ASYNC | SND_NODEFAULT );
				CString inf;
				inf.Format("CSSA��̳��%d��������!",newinf.num_comments-m_webinf.num_comments);
				m_webinf=newinf;
				AfxMessageBox(inf,MB_YESNO);
				ShellExecute(0, NULL, m_webinf.link, NULL, NULL, 1);
			}
		}
		SuspendThread(GetCurrentThread()); //���������߳�
	}	
	return 0;
}

LRESULT CWebListenerDlg::OnTaskManage(WPARAM wParam, LPARAM lParam)
{
	if(wParam != IDR_TRAY_MENU) return 1;

	switch(lParam)
	{
	case WM_RBUTTONUP:
		{
			CMenu * pTaskMenu, taskMenu;
			CPoint pMousePosition;
			taskMenu.LoadMenu(IDR_TRAY_MENU);
			pTaskMenu = taskMenu.GetSubMenu(0);
			GetCursorPos(&pMousePosition);
			SetForegroundWindow();
			pTaskMenu->TrackPopupMenu(TPM_RIGHTALIGN,pMousePosition.x,pMousePosition.y,this);
			break;
		}
	case WM_LBUTTONDOWN:
		{
			SetForegroundWindow();
			break;
		}
	}
	return 0;
}

void CWebListenerDlg::OnStopListen() //ֹͣ���
{
	ShowWindow(SW_SHOW);
	KillTimer(1);//ֹͣ��������
	strcpy_s(m_SkyNID.szTip, _T("׼������"));
	Shell_NotifyIcon(NIM_MODIFY, &m_SkyNID);
}

void CWebListenerDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_listen_thread->ResumeThread(); //�򿪼����̣߳�����һ�μ��
	CDialog::OnTimer(nIDEvent);
}

void CWebListenerDlg::OnCancel()
{
	Shell_NotifyIcon(NIM_DELETE, &m_SkyNID);//�������
	State2File(TRUE, TRUE); //д���ļ���ָʾ�����ر�
	CDialog::OnCancel();
}
void CWebListenerDlg::OnBnClickedCheckAutorun()
{
	UpdateData(TRUE);
	if(!setAutoRun(m_autorun)) 
	{
		MessageBox("�����Զ���������ʧ�ܣ�");
		m_autorun=!m_autorun;
		UpdateData(FALSE);
	}
}

BOOL CWebListenerDlg::setAutoRun(BOOL yes) //����ע����Զ�����
{
	CRegKey myKey;
	if(myKey.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run") != ERROR_SUCCESS)
	{
		MessageBox("�����޷���HKEY_LOCAL_MACHINE\\Software\\Microsof\\tWindows\\CurrentVersion\\Runע����!");
		return FALSE;
	}	
	DWORD szBuff=MAX_PATH;
	char exeFullPath[MAX_PATH];
	LONG queryState=myKey.QueryValue(exeFullPath,"CSSA_Listener",&szBuff);
	if (yes)
	{
		CString regPath=exeFullPath;	
		GetModuleFileName(NULL,exeFullPath,MAX_PATH);
		if (queryState!=ERROR_SUCCESS || regPath!=exeFullPath) myKey.SetValue(exeFullPath, "CSSA_Listener");
	}
	else 
	{
		if (queryState==ERROR_SUCCESS) myKey.DeleteValue("CSSA_Listener");
	}
	myKey.Close();
	return TRUE;
}

BOOL CWebListenerDlg::State2File(BOOL yes, BOOL last) //�ļ���д״̬����
{
	using namespace std;
	if(yes)
	{
		//��״̬�����ĵ�	
		ofstream ostream;
		ostream.open("conf.dat");
		if(ostream.is_open())
		{			
			ostream<< m_webinf.num_posts << endl;
			ostream<< m_webinf.num_comments << endl;
			ostream<< m_hour <<endl;
			ostream<< m_minute <<endl;
			ostream<< m_second <<endl;
			ostream<< m_autorun << endl;
			if(last) ostream<< TRUE << endl;
			else ostream<< FALSE << endl;
			ostream << endl << "˵��-----------------------------\n����1:���ֽ������ӵ�����\n����2:���¶��ֽ������ӵĻظ���\n����3:���Ƶ��-Сʱ\n����4:���Ƶ��-����\n����5:���Ƶ��-��\n����6:�Ƿ��Զ�����\n����7:�����Ƿ������˳�\n" <<endl;
			ostream.close();
		}
		else return FALSE;		
	}
	else //���ĵ���ȡ
	{
		ifstream istream;
		istream.open("conf.dat");
		if(istream.is_open())
		{
			istream>> m_webinf.num_posts >> m_webinf.num_comments >> m_hour >> m_minute >> m_second >> m_autorun >> m_normal_exit;
			istream.close();
		}
		else return FALSE;
	}
	return TRUE;
}