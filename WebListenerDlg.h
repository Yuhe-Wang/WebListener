
// WebListenerDlg.h : ͷ�ļ�
//

#pragma once
struct WebInf
{
	int num_posts;
	int num_comments;
	CString link;
};

// CWebListenerDlg �Ի���
class CWebListenerDlg : public CDialog
{
// ����
public:
	CWebListenerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WEBLISTENER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	NOTIFYICONDATA m_SkyNID;//��������
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
	BOOL m_autorun;//ָʾ�Ƿ񿪻��Զ�����
	BOOL m_normal_exit;//ָʾ�Ƿ������˳�
	BOOL State2File(BOOL yes, BOOL last=FALSE);
};
