// joystick_hidapiDlg.h : header file
//

#pragma once
#include "afxwin.h"


// Cjoystick_hidapiDlg dialog
class Cjoystick_hidapiDlg : public CDialog
{
// Construction
public:
	Cjoystick_hidapiDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_JOYSTICK_HIDAPI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// PS: User variables
private:
	int m_iReturnCode;
	struct hid_device_ *m_pstrctDevice;
	CWinThread *m_pJoystickPoll;
public:
	bool m_bThreadActive;

// PS: User functions
private:
	void UCharToBinary(unsigned char *pucBinary, unsigned char ucData);
	CString PUCharToCString(unsigned char *pucBinary, unsigned int iLength);
public:
	void DecodeJoystickStatus();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedReadBtn();
	CEdit m_ceditData;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedStartBtn();
	afx_msg void OnBnClickedStopBtn();
};
