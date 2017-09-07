
// WebListenerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <afxinet.h>

#include "WebListener.h"
#include "WebListenerDlg.h"

#include "string.h"
#include <iostream>
#include <fstream>
#include "ParserDom.h"
#include "Mmsystem.h" //播放声音库

#define WM_TASKMANAGE WM_USER+300
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment( lib , "winmm.lib" )
#pragma comment( lib , "htmlcxx.lib" )  

CWebListenerDlg *g_dlg;//全局对话框指针

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CWebListenerDlg 对话框
//静态成员初始化
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


// CWebListenerDlg 消息处理程序

BOOL CWebListenerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	g_dlg=this;
	m_listen_thread=NULL;
	//添加托盘
	m_SkyNID.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_SkyNID.hWnd = this->m_hWnd;
	m_SkyNID.uID = IDR_TRAY_MENU;
	m_SkyNID.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	m_SkyNID.dwState = NIS_SHAREDICON;
	m_SkyNID.uCallbackMessage = WM_TASKMANAGE;
	m_SkyNID.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy_s(m_SkyNID.szTip, _T("准备就绪"));
	Shell_NotifyIcon(NIM_ADD, &m_SkyNID);

	if (State2File(FALSE)) //能从配置文件读取状态
	{
		m_isFromFile=TRUE;
		UpdateData(FALSE);
		if (m_autorun && !m_normal_exit) OnBnClickedStart();//直接运行
	}
	else //“从文件恢复状态”复选框不可用
	{
		GetDlgItem(IDC_CHECK_FROMFILE)->EnableWindow(FALSE);
	}
	//设置注册表
	if(!setAutoRun(m_autorun))	OnCancel();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWebListenerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWebListenerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWebListenerDlg::OnBnClickedStart() //开始监听
{	
	if (m_listen_thread==NULL) //监听线程尚未创建
	{
		m_listen_thread=AfxBeginThread(ListenThread,0,0,CREATE_SUSPENDED);
	}
	//更新时间，设定定时器,进入后台
	UpdateData(TRUE);
	if (!m_hour && !m_minute && !m_second)
	{
		MessageBox("时间不能为零,请重新设置！");
		return;
	}
	//State2File(TRUE);//修正程序退出标记
	SetTimer(1,1000*(m_second+60*m_minute+3600*m_hour), NULL);
	//修改托盘提示
	CString tip,time;
	tip.Format("正在监听CSSA论坛，\n探测时间间隔为：\n");
	if (m_hour != 0)
	{
		time.Format("%d小时",m_hour);
		tip+=time;
	}
	if (m_minute != 0)
	{
		time.Format("%d分钟", m_minute);
		tip+=time;
	}
	if (m_second != 0)
	{
		time.Format("%d秒", m_second);
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
	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	strSrc = new WCHAR[i+1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, strSrc, i);
	//获得临时变量的大小
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
	/* 在重试连接之间的等待的延时值在毫秒级。*/
	httpSession.SetOption(INTERNET_OPTION_CONNECT_BACKOFF,500);
	/* 在网络连接请求时的重试次数。如果一个连接企图在指定的重试次数后仍失败，则请求被取消。 缺省值为5。*/
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
		AfxMessageBox("网络连接异常！\nWebListenner将退出，并尝试直接打开网页...");
		ShellExecute(0, NULL, url, NULL, NULL, 1);
		g_dlg->OnCancel();//退出程序
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
	string token1=GBToUTF8("二手交易");
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
	if (num_comments > 50) //每满50页产生新页面
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
			m_webinf=newinf; //直接从网上更新
			initialized=true;
			g_dlg->State2File(TRUE);
		}
		else 
		{
			if (newinf.num_posts!=m_webinf.num_posts) //有新帖子
			{			
				//通知1
				g_dlg->State2File(TRUE);
				sndPlaySound( "Notify1.wav" , SND_ASYNC | SND_NODEFAULT );
				CString inf;
				inf.Format("CSSA二手论坛有%d条新帖子!",newinf.num_posts-m_webinf.num_posts);
				m_webinf=newinf;
				AfxMessageBox(inf,MB_YESNO);
				ShellExecute(0, NULL,m_webinf.link, NULL, NULL, 1);
			}
			else if(newinf.num_comments!=m_webinf.num_comments) //没开新帖，但有新评论
			{
				//通知2
				g_dlg->State2File(TRUE);
				sndPlaySound( "Notify2.wav" , SND_ASYNC | SND_NODEFAULT );
				CString inf;
				inf.Format("CSSA论坛有%d条新评论!",newinf.num_comments-m_webinf.num_comments);
				m_webinf=newinf;
				AfxMessageBox(inf,MB_YESNO);
				ShellExecute(0, NULL, m_webinf.link, NULL, NULL, 1);
			}
		}
		SuspendThread(GetCurrentThread()); //挂起自身线程
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

void CWebListenerDlg::OnStopListen() //停止监测
{
	ShowWindow(SW_SHOW);
	KillTimer(1);//停止更新命令
	strcpy_s(m_SkyNID.szTip, _T("准备就绪"));
	Shell_NotifyIcon(NIM_MODIFY, &m_SkyNID);
}

void CWebListenerDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_listen_thread->ResumeThread(); //打开监听线程，进行一次监测
	CDialog::OnTimer(nIDEvent);
}

void CWebListenerDlg::OnCancel()
{
	Shell_NotifyIcon(NIM_DELETE, &m_SkyNID);//清除托盘
	State2File(TRUE, TRUE); //写入文件，指示正常关闭
	CDialog::OnCancel();
}
void CWebListenerDlg::OnBnClickedCheckAutorun()
{
	UpdateData(TRUE);
	if(!setAutoRun(m_autorun)) 
	{
		MessageBox("开机自动运行设置失败！");
		m_autorun=!m_autorun;
		UpdateData(FALSE);
	}
}

BOOL CWebListenerDlg::setAutoRun(BOOL yes) //设置注册表自动运行
{
	CRegKey myKey;
	if(myKey.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run") != ERROR_SUCCESS)
	{
		MessageBox("错误：无法打开HKEY_LOCAL_MACHINE\\Software\\Microsof\\tWindows\\CurrentVersion\\Run注册表键!");
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

BOOL CWebListenerDlg::State2File(BOOL yes, BOOL last) //文件读写状态变量
{
	using namespace std;
	if(yes)
	{
		//将状态存入文档	
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
			ostream << endl << "说明-----------------------------\n参数1:二手交易帖子的数量\n参数2:最新二手交易帖子的回复数\n参数3:监测频率-小时\n参数4:监测频率-分钟\n参数5:监测频率-秒\n参数6:是否自动运行\n参数7:程序是否正常退出\n" <<endl;
			ostream.close();
		}
		else return FALSE;		
	}
	else //从文档读取
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