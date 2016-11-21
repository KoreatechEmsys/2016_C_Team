
// JolJakDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "JolJak.h"
#include "JolJakDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEF_PROCESS_NAME "Microsoft Surface Globe"
// CJolJakDlg 대화 상자



CJolJakDlg::CJolJakDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JOLJAK_DIALOG, pParent)
	, m_staticGesture(_T(""))
	, m_staticX(_T(""))
	, m_staticY(_T(""))
	, m_editLog(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJolJakDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
	DDX_Text(pDX, IDC_STATIC_GESTURE, m_staticGesture);
	DDX_Text(pDX, IDC_STATIC_X, m_staticX);
	DDX_Text(pDX, IDC_STATIC_Y, m_staticY);
	//  DDX_Control(pDX, IDC_STATIC_Y2, m_staticDist);
	//  DDX_Control(pDX, IDC_BTN_TEST, m_btnManual);
	DDX_Text(pDX, IDC_EDIT2, m_editLog);
	DDX_Control(pDX, IDC_EDIT2, m_editLogCtrl);
	DDX_Control(pDX, IDC_BTN_TEST, m_btnManual);
}

BEGIN_MESSAGE_MAP(CJolJakDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_START, &CJolJakDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CJolJakDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_TEST, &CJolJakDlg::OnBnClickedBtnTest)
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


// CJolJakDlg 메시지 처리기

BOOL CJolJakDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	/*Window 반투명화 Procedure*/
	typedef BOOL(WINAPI *SLWA)(HWND, COLORREF, BYTE, DWORD);
	SLWA pSetLayeredWindowAttributes = NULL;
	HINSTANCE hmodUSER32 = LoadLibrary(_T("USER32.DLL"));
	pSetLayeredWindowAttributes =
		(SLWA)GetProcAddress(hmodUSER32, "SetLayeredWindowAttributes");

	HWND hwnd = this->m_hWnd;
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	pSetLayeredWindowAttributes(hwnd, 0, 100, LWA_ALPHA);

	SetBackgroundColor(RGB(30, 30, 30)); //배경색 조정

	/*Touch Input Initialize*/
	InitializeTouchInjection(1, TOUCH_FEEDBACK_DEFAULT);
	memset(&contact, 0, sizeof(POINTER_TOUCH_INFO));

	m_btnStop.EnableWindow(false);
	m_staticGesture = _T("");
	m_staticX = _T("");
	m_staticY = _T("");
	
	
	SetWindowPos(&wndTopMost, 2150, 850, 0, 0, SWP_NOSIZE);
	initProc();
	m_editLog = "Program is executed\r\n\r\n\r\n\r\n\r\n\r\n";
	UpdateData(FALSE);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CJolJakDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CJolJakDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CJolJakDlg::initProc(void)
{
	gestureChangeCount = 0;
}

void CJolJakDlg::OnBnClickedBtnStart()
{
	hWndTarget = NULL;
	hWndTarget = FindWindowA(NULL, DEF_PROCESS_NAME);
	
	if (hWndTarget == NULL)
	{
		MessageBox(_T("Check Surface Globe is executed"));
		m_editLog += "Surface Globe is not executed\r\n";
		UpdateData(FALSE);
		m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
		return;
	}


	
	m_btnStart.EnableWindow(false);
	m_btnStop.EnableWindow(true);

	if (!m_hg.openDevice(0))
	{
		MessageBox(_T("Camera Open Failed"));
		m_editLog += "Camera device open failed\r\n";
		UpdateData(FALSE);
		m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
		m_btnStart.EnableWindow(true);
		m_btnStop.EnableWindow(false);
		return;
	}

	m_editLog += "Start image processing\r\n";
	UpdateData(FALSE);
	m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());

#ifdef __DEBUGVISUAL__
	cv::namedWindow("origin");
	cv::namedWindow("binary");
#endif // __DEBUGVISUAL__

	SetTimer(1000, 15, NULL);
	

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CJolJakDlg::OnBnClickedBtnStop()
{
	cv::destroyWindow("origin");
	cv::destroyWindow("binary");
	m_btnStart.EnableWindow(true);
	m_btnStop.EnableWindow(false);
	
	KillTimer(1000);
	
	if (m_hg.vcDevice.isOpened())
	{
		m_hg.vcDevice.release();
		m_editLog += "Closing device\r\n";
	}

	m_editLog += "Stop image processing\r\n";
	UpdateData(FALSE);
	m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
	m_hg.calibration = false;
	m_hg.calibrationCount = 0;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



void CJolJakDlg::OnTimer(UINT_PTR nIDEvent)
{
	GestureInfo gInfo = ImageProcessing();
	DisplayStatus(gInfo);
	GestureProcessing(gInfo);
	
	
}

void CJolJakDlg::GestureProcessing(GestureInfo gInfo)
{
	static CString soundPath("alarm.wav");
	static CString soundPath2("alarm2.wav");
	bool gChange = false;
	if (lastGesture.gesture != gInfo.gesture)
	{
		gestureChangeCount++;
		
		if (gestureChangeCount > MIN_GESTURE_CHANGE_COUNT)
		{
			if(lastGesture.gesture != NEEDCALIB)
				PlaySound(soundPath, AfxGetInstanceHandle(), SND_ASYNC);
			else
			{
				m_editLog += "Calibration is completed\r\n";
				UpdateData(FALSE);
				m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
				PlaySound(soundPath2, AfxGetInstanceHandle(), SND_ASYNC);
			}
			
			lastGesture.gesture = gInfo.gesture;
			gChange = true;

			switch (lastGesture.gesture)
			{
			case G1:
				m_editLog += "Moving gesture is recognized\r\n";
				UpdateData(FALSE);
				m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
				break;
			case G2:
				m_editLog += "Zooming gesture is recognized\r\n";
				break;
			case G3:
				m_editLog += "Transfering gesture is recognized\r\n";
				break;
			case G4:
				m_editLog += "Moving Window gesture is recognized\r\n";
				break;
			case NEEDCALIB:
				break;
			default:
				m_editLog += "Unknown gesture is recognized\r\n";
				break;
			}
			
			UpdateData(FALSE);
			m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
			
		}
	}
	else
		gestureChangeCount = 0;

	switch (lastGesture.gesture)
	{
	case G1:
		ProcG1((gInfo.cog.x - lastGesture.cog.x), (gInfo.cog.y - lastGesture.cog.y));
		cv::putText(m_hg.imgSkin, "Gesture: Moving", Point(10, 30), CV_FONT_HERSHEY_TRIPLEX, 1, Scalar(255),2);
		UpdateData(FALSE);
		m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
		displayImage(m_hg.imgSkin, IDC_PICT_BINIMG);
		break;
	case G2:
		ProcG2(gInfo);
		cv::putText(m_hg.imgSkin, "Gesture: Zooming", Point(10, 30), CV_FONT_HERSHEY_TRIPLEX, 1, Scalar(255),2);
		UpdateData(FALSE);
		m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
		displayImage(m_hg.imgSkin, IDC_PICT_BINIMG);
		break;
	case G3:
		cv::putText(m_hg.imgSkin, "Gesture: Transfering map style", Point(10, 30), CV_FONT_HERSHEY_TRIPLEX, 1, Scalar(255), 2);
		displayImage(m_hg.imgSkin, IDC_PICT_BINIMG);
		if (gChange)
		{
			ProcG3();
			gChange = false;
		}
		break;
	case G4:
		cv::putText(m_hg.imgSkin, "Gesture: Moving window", Point(10, 30), CV_FONT_HERSHEY_TRIPLEX, 1, Scalar(255), 2);
		displayImage(m_hg.imgSkin, IDC_PICT_BINIMG);
		ProcG4((gInfo.cog.x - lastGesture.cog.x), (gInfo.cog.y - lastGesture.cog.y));
		
		break;
	case NEEDCALIB:
		cv::putText(m_hg.imgSkin, "Calibrating", Point(10, 30), CV_FONT_HERSHEY_TRIPLEX, 1, Scalar(255), 2);
		UpdateData(FALSE);
		m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
		displayImage(m_hg.imgSkin, IDC_PICT_BINIMG);
		break;
	default:
		cv::putText(m_hg.imgSkin, "Gesture: Unknown", Point(10, 30), CV_FONT_HERSHEY_TRIPLEX, 1, Scalar(255), 2);
		UpdateData(FALSE);
		m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
		displayImage(m_hg.imgSkin, IDC_PICT_BINIMG);
		break;
	}

	lastGesture.cog = gInfo.cog;
	lastGesture.vecFingerPts = gInfo.vecFingerPts;
}

void CJolJakDlg::OnBnClickedBtnTest()
{
	m_btnStart.EnableWindow(false);
	m_btnManual.EnableWindow(false);
	playManual();
	m_btnStart.EnableWindow(true);
	m_btnManual.EnableWindow(true);
}



HBRUSH CJolJakDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_GESTURE || 
		pWnd->GetDlgCtrlID() == IDC_STATIC_X || pWnd->GetDlgCtrlID() == IDC_STATIC_Y)
	{
		pDC->SetTextColor(RGB(255, 255, 0));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
	}
	
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		if (pWnd->GetDlgCtrlID() == IDC_EDIT2)
		{
			pDC->SetTextColor(RGB(255, 20, 240));
			pDC->SetBkColor(RGB(50, 50, 50));
		}
	}
	
	if (pWnd->GetDlgCtrlID() == IDC_BTN_START)
	{
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 20, 240));
		pDC->SetBkColor(RGB(50, 50, 50));
		
	}
	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


void CJolJakDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDCtl == IDC_BTN_START || nIDCtl == IDC_BTN_STOP || nIDCtl == IDOK)
	{
		
		CDC dc;
		RECT rect;
		dc.Attach(lpDrawItemStruct->hDC);   // Get the Button DC to CDC

		rect = lpDrawItemStruct->rcItem;     //Store the Button rect to our local rect.
		dc.Draw3dRect(&rect, RGB(50, 50, 50), RGB(0, 0, 0));
		dc.FillSolidRect(&rect, RGB(50, 50, 50));//Here you can define the required color to appear on the Button.

		UINT state = lpDrawItemStruct->itemState;  //This defines the state of the Push button either pressed or not. 
		if ((state & ODS_SELECTED))
			dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);
		else
			dc.DrawEdge(&rect, EDGE_RAISED, BF_RECT);

		dc.SetBkColor(RGB(50, 50, 50));   //Setting the Text Background color
		dc.SetTextColor(RGB(255, 255, 0));     //Setting the Text Color

		TCHAR buffer[MAX_PATH];           //To store the Caption of the button.
		ZeroMemory(buffer, MAX_PATH);     //Intializing the buffer to zero
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); //Get the Caption of Button Window 

		dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//Redraw the  Caption of Button Window 
		dc.Detach();  // Detach the Button DC
	}
	else if (nIDCtl == IDC_BTN_TEST)
	{

		CDC dc;
		RECT rect;
		dc.Attach(lpDrawItemStruct->hDC);   // Get the Button DC to CDC

		rect = lpDrawItemStruct->rcItem;     //Store the Button rect to our local rect.
		dc.Draw3dRect(&rect, RGB(50, 50, 50), RGB(0, 0, 0));
		dc.FillSolidRect(&rect, RGB(50, 50, 50));//Here you can define the required color to appear on the Button.

		UINT state = lpDrawItemStruct->itemState;  //This defines the state of the Push button either pressed or not. 
		if ((state & ODS_SELECTED))
			dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);
		else
			dc.DrawEdge(&rect, EDGE_RAISED, BF_RECT);

		dc.SetBkColor(RGB(50, 50, 100));   //Setting the Text Background color
		dc.SetTextColor(RGB(100, 255, 100));     //Setting the Text Color

		TCHAR buffer[MAX_PATH];           //To store the Caption of the button.
		ZeroMemory(buffer, MAX_PATH);     //Intializing the buffer to zero
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); //Get the Caption of Button Window 

		dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//Redraw the  Caption of Button Window 
		dc.Detach();  // Detach the Button DC
	}
	//CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
