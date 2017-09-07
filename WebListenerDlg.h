
// WebListenerDlg.h : 头文件
//

#pragma once
struct WebInf
{
	int num_posts;
	int num_comments;
	CString link;
};

// CWebListenerDlg 对话框
class CWebListenerDlg : public CDialog
{
// 构造
public:
	CWebListenerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WEBLISTENER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedStart();
	static std::string GBToUTF8(const std::string& str);
	static void GetWebInf(WebInf& webinf);
	CWinThread *m_listen_thread;
	static UINT ListenThread(LPVOID p);
	NOTIFYICONDATA m_SkyNID;//托盘数据
	LRESULT OnTaskManage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStopListen();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void OnCancel();
	void OnOK(){};
	UINT m_hour;
	UINT m_minute;
	UINT m_second;
	static BOOL m_isFromFile;
	static WebInf m_webinf;
	afx_msg void OnBnClickedCheckAutorun();
	BOOL setAutoRun(BOOL yesno);
	BOOL m_autorun;//指示是否开机自动运行
	BOOL m_normal_exit;//指示是否正常退出
	BOOL State2File(BOOL yes, BOOL last=FALSE);
};
