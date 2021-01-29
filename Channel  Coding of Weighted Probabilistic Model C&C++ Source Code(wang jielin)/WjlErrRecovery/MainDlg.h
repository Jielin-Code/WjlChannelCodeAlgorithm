#pragma once

#include "afxcmn.h"
#include "afxwin.h"

// CMainDlg 对话框

class CMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainDlg();

// 对话框数据
	enum { IDD = IDD_MAINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	double Coeff(char n);
	unsigned long GetMicroTimeStamp(void);
	CString	GetTime();
	void InitRandom(void);
	unsigned int Random();
	void ErrCode(unsigned char *ucInBuffer,const unsigned int *unInbufferLen,const double *dErrbitRate);
	BOOL OpenPath(CString strFilePath);
	void DisplayFileInfo(CString strPath,UINT stcControlID);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnKillfocusEdInputpath();

	afx_msg void OnEnChangeEdInputpath();
	afx_msg void OnBnClickedBtnOpen();

	afx_msg void OnEnChangeEdOutpath();
	afx_msg void OnBnClickedBtnGo();

	afx_msg void OnEnChangeEdErrrate();

	afx_msg void OnBnClickedBtnCompress();
	afx_msg void OnBnClickedBtnUnzip();
	afx_msg void OnBnClickedBtnOpenzip();
	afx_msg void OnBnClickedBtnOpenunzip();
	afx_msg void OnEnChangeEdZippath();
	afx_msg void OnEnKillfocusEdZippath();
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);

private:
	CString	m_strInputPath;	//	
	CString	m_strOutputPath;

	CString	m_strZipdataPath;
	CString m_strUnzipDataPath;

	CString	m_strErrbitRate;

	CButton	m_btnOpenGo;
	CButton m_btnCompress;
	CButton	m_btnOpen;
	CButton	m_btnUnzip;

	CEdit	m_edInputPath;
	CEdit	m_edOutputPath;

	CEdit	m_edZipDataPath;
	CEdit	m_edUnzipOutPath;

	CEdit	m_edErrbitRate;

	CButton m_btnOpenZipdata;
	CButton m_btnOpenUnzipdata;
	
	CProgressCtrl m_proCompress;
	CProgressCtrl m_proUnzip;
	CButton m_btnChkUnzipedOpen;
	CComboBox m_comCodeRate;
};
