// joystick_hidapiDlg.cpp : implementation file
//

//	This code will demonstrate how to use the HIDAPI library to interface with a generic usb joystick. 
//	The joystick used for testing uses the windows generic HID drivers.
//	This application may not be able to communicate with other joysticks as the command format may differ.
//	VID and PID values should be adjusted according to the used devices.
//
//	About usb specification refer to www.usb.org
//	Refer the HID class specification by the www.usb.org for class specific implementation guidlines.
//
//	Please note that error checking is not done inside the functions. Checking the return values, if needed
//	should be done manually during the debug. 
//
//	The code is self explanatory. For further inquiries visit www.UnboxnBeyond.com or
//	mail to info@unboxnbeyond.com

#include "stdafx.h"
#include "joystick_hidapi.h"
#include "joystick_hidapiDlg.h"
#include "hidapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// VID and PID values of the device

#define VID		0x0079
#define PID		0x0006

// Joystick outputs

#define NONE_PRESSED		0x7f7f0f00
#define UP_PRESSED			0x7f000f00
#define DOWN_PRESSED		0x7fff0f00
#define LEFT_PRESSED		0x007f0f00
#define RIGHT_PRESSED		0xff7f0f00
#define ONE_PRESSED			0x7f7f1f00
#define TWO_PRESSED			0x7f7f2f00
#define THREE_PRESSED		0x7f7f4f00
#define FOUR_PRESSED		0x7f7f8f00
#define LEFT_1_PRESSED		0x7f7f0f01
#define LEFT_2_PRESSED		0x7f7f0f04
#define RIGHT_1_PRESSED		0x7f7f0f02
#define RIGHT_2_PRESSED		0x7f7f0f08
#define SELECT_PRESSED		0x7f7f0f10
#define START_PRESSED		0x7f7f0f20	
#define LEFT_HAT_PRESSED	0x7f7f0f40
#define RIGHT_HAT_PRESSED	0x7f7f0f80

// Thread declarations

UINT JoystickPollingThread(LPVOID param);

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// Cjoystick_hidapiDlg dialog




Cjoystick_hidapiDlg::Cjoystick_hidapiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cjoystick_hidapiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cjoystick_hidapiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_READ_DATA, m_ceditData);
}

BEGIN_MESSAGE_MAP(Cjoystick_hidapiDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_READ_BTN, &Cjoystick_hidapiDlg::OnBnClickedReadBtn)
	ON_BN_CLICKED(IDOK, &Cjoystick_hidapiDlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_START_BTN, &Cjoystick_hidapiDlg::OnBnClickedStartBtn)
	ON_BN_CLICKED(IS_STOP_BTN, &Cjoystick_hidapiDlg::OnBnClickedStopBtn)
END_MESSAGE_MAP()


// Cjoystick_hidapiDlg message handlers

BOOL Cjoystick_hidapiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CFont* pfontFont = new CFont();
	pfontFont->CreatePointFont(400,_T("Arial"));
	GetDlgItem(ID_OUTPUT)->SetFont(pfontFont);
	GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("Output"));

	// Initializinf the return code member variable
	m_iReturnCode = 0;

	struct hid_device_info *strctDevInfo = NULL;
	m_pstrctDevice = NULL;

	// Initialize the HIDAPI library
	m_iReturnCode = hid_init();

	// Enumerate all the usb devices specified by the VID and PID
	strctDevInfo = hid_enumerate(VID,PID);

	// Get the handle to communicate with the required joystick defined by the VID and PID
	m_pstrctDevice = hid_open(VID,PID,NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cjoystick_hidapiDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cjoystick_hidapiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cjoystick_hidapiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cjoystick_hidapiDlg::OnBnClickedReadBtn()
{
	// TODO: Add your control notification handler code here
	unsigned char* pucData = (unsigned char*) malloc(sizeof(char) * 64);
	unsigned char *pucBinary = (unsigned char*) malloc(sizeof(char) * 8);
	unsigned char a,b,c,d,e,f,g,h;
	CString strValue = NULL;
	CString strTemp;
	
	// Read the input from the device. The function will read data coming through the interrupt IN endpoint.
	m_iReturnCode = hid_read(m_pstrctDevice,pucData,64);
	
	// Break the data received to 8 bytes for further processing.
	a = *pucData;
	b = *(pucData+1);
	c = *(pucData+2);
	d = *(pucData+3);
	e = *(pucData+4);
	f = *(pucData+5);
	g = *(pucData+6);
	h = *(pucData+7);

	// Generating a string to display in the Edit Box
	// Convert the unsigned char to binary
	UCharToBinary(pucBinary,a);

	// Convert the pointer of unsigned char to CString
	strTemp = PUCharToCString(pucBinary,8);
	strValue += strTemp;
	strValue += " ";

	UCharToBinary(pucBinary,b);
	strTemp = PUCharToCString(pucBinary,8);
	strValue += strTemp;
	strValue += " ";

	UCharToBinary(pucBinary,c);
	strTemp = PUCharToCString(pucBinary,8);
	strValue += strTemp;
	strValue += " ";

	UCharToBinary(pucBinary,d);
	strTemp = PUCharToCString(pucBinary,8);
	strValue += strTemp;
	strValue += " ";

	UCharToBinary(pucBinary,e);
	strTemp = PUCharToCString(pucBinary,8);
	strValue += strTemp;
	strValue += " ";

	UCharToBinary(pucBinary,f);
	strTemp = PUCharToCString(pucBinary,8);
	strValue += strTemp;
	strValue += " ";

	UCharToBinary(pucBinary,g);
	strTemp = PUCharToCString(pucBinary,8);
	strValue += strTemp;
	strValue += " ";

	UCharToBinary(pucBinary,h);
	strTemp = PUCharToCString(pucBinary,8);
	strValue += strTemp;
	strValue += " ";

	// Display the reply code in bunary format in the Edit Box for debug puposes.
	m_ceditData.SetWindowTextW(strValue);


	free(pucData);
	free(pucBinary);
}


// Parameters: pucBinary = pointer to an unsigned char which is malloced to store the neccessary data
void Cjoystick_hidapiDlg::UCharToBinary(unsigned char *pucBinary, unsigned char ucData)
{
	int i=7;
	for(i=7; i>=0; i--)
	{
		pucBinary[i] = ucData % 2;
		ucData /= 2;
	}
}

CString Cjoystick_hidapiDlg::PUCharToCString(unsigned char *pucBinary, unsigned int iLength)
{
	unsigned int i;
	CString strTemp;
	CString strAnswer = NULL;

	for(i=0; i<iLength; i++)
	{
		strTemp.Format(_T("%d"),pucBinary[i]);
		strAnswer += strTemp;
	}

	return strAnswer;
}

void Cjoystick_hidapiDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void Cjoystick_hidapiDlg::OnBnClickedStartBtn()
{
	// TODO: Add your control notification handler code here

	// Activate the inner loop inside the thread
	m_bThreadActive = TRUE;

	// Start the thread.
	m_pJoystickPoll = AfxBeginThread(AFX_THREADPROC(JoystickPollingThread),this);
}

void Cjoystick_hidapiDlg::OnBnClickedStopBtn()
{
	// TODO: Add your control notification handler code here

	// Deactivate the inner loop inside the thread
	m_bThreadActive = FALSE;
}

void Cjoystick_hidapiDlg::DecodeJoystickStatus()
{
	unsigned char* pucData = (unsigned char*) malloc(sizeof(char) * 64);
	unsigned char *pucBinary = (unsigned char*) malloc(sizeof(char) * 8);
	unsigned char a,b,c,d,e,f,g,h;
	unsigned long ulBytes = 0x00;
	CString strValue = NULL;
	CString strTemp;
	
	m_iReturnCode = hid_read(m_pstrctDevice,pucData,64);
	
	a = *pucData;
	b = *(pucData+1);
	c = *(pucData+2);
	d = *(pucData+3);
	e = *(pucData+4);
	f = *(pucData+5);
	g = *(pucData+6);
	h = *(pucData+7);

	// Generate an unisigned long value to represent all the events of the joystick.
	// This was generated using trial and error.
	ulBytes |= ((a<<24) | (b<<16) | (f<<8) | g); 

	// Switch according to the button press.
	switch(ulBytes)
	{
	case NONE_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("NONE_PRESSED"));
		break;
	case UP_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("UP_PRESSED"));
		break;
	case DOWN_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("DOWN_PRESSED"));
		break;
	case LEFT_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("LEFT_PRESSED"));
		break;
	case RIGHT_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("RIGHT_PRESSED"));
		break;
	case ONE_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("ONE_PRESSED"));
		break;
	case TWO_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("TWO_PRESSED"));
		break;
	case THREE_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("THREE_PRESSED"));
		break;
	case FOUR_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("FOUR_PRESSED"));
		break;
	case LEFT_1_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("LEFT_1_PRESSED"));
		break;
	case LEFT_2_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("LEFT_2_PRESSED"));
		break;
	case RIGHT_1_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("RIGHT_1_PRESSED"));
		break;
	case RIGHT_2_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("RIGHT_2_PRESSED"));
		break;
	case SELECT_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("SELECT_PRESSED"));
		break;
	case START_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("START_PRESSED"));
		break;
	case LEFT_HAT_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("LEFT_HAT_PRESSED"));
		break;
	case RIGHT_HAT_PRESSED:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("RIGHT_HAT_PRESSED"));
		break;
	default:
		GetDlgItem(ID_OUTPUT)->SetWindowTextW(_T("Not Defined"));
		break;

	}
	
	free(pucData);
	free(pucBinary);

}

// Thread to poll the joystick reply.
UINT JoystickPollingThread(LPVOID param)
{
	Cjoystick_hidapiDlg *pDlg = (Cjoystick_hidapiDlg *)param;

	// Inner loop
	while(pDlg->m_bThreadActive)
	{
		pDlg -> DecodeJoystickStatus();
	}

	AfxEndThread(0,TRUE);
	return 0;
}
