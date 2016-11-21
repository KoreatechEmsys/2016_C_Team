#include "stdafx.h"
#include "JolJakDlg.h"

#define MAX_CHANGE 8
void CJolJakDlg::DisplayStatus(GestureInfo gInfo)
{
	
	switch (gInfo.gesture)
	{
	case G1:
		m_staticGesture = _T("G1");
		
		break;
	case G2:
		m_staticGesture = _T("G2");
		break;
	case G3:
		m_staticGesture = _T("G3");
		break;
	case UNKNOWN:
		m_staticGesture = _T("UNKNOWN");
		break;
	case G4:
		m_staticGesture = _T("G4");
		break;
	case NEEDCALIB:
		m_staticGesture = _T("Do Calibration");
		break;
	default:
		m_staticGesture = _T("Default");
		break;
	}

	m_staticX.Format(_T("%d"), gInfo.cog.x);
	m_staticY.Format(_T("%d"), gInfo.cog.y);

	UpdateData(FALSE);
	
}

void CJolJakDlg::ProcG1(int dx, int dy)
{
	int xChange = dx / COG_THRESH_VAL;
	int yChange = dy / COG_THRESH_VAL;

	if (xChange != 0)
	{
		if (xChange > 0)
		{
			::SetForegroundWindow(hWndTarget);
			if (xChange > MAX_CHANGE) {
				xChange = MAX_CHANGE;
				m_editLog += "X Axis Moving is too fast\r\n";
				m_hg.WarningMSG("X Axis Moving is too fast");
				
				UpdateData(FALSE);
				m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
			}
			
			for (int i = 0; i < xChange*C_VALUE; i++)
			{
				PostMessageA(hWndTarget, WM_KEYDOWN, VK_LEFT, 0);
			}
			PostMessageA(hWndTarget, WM_KEYUP, VK_LEFT, 0);
		}
		else if (xChange < 0)
		{
			::SetForegroundWindow(hWndTarget);
			xChange = xChange * (-1);
			if (xChange > MAX_CHANGE)
			{
				xChange = MAX_CHANGE;
				m_editLog += "X Axis Moving is too fast\r\n";
				m_hg.WarningMSG("X Axis Moving is too fast");
				UpdateData(FALSE);
				m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
			}
			for (int i = 0; i < xChange * C_VALUE; i++)
			{
				PostMessageA(hWndTarget, WM_KEYDOWN, VK_RIGHT, 0);
				
			}
			PostMessageA(hWndTarget, WM_KEYUP, VK_RIGHT, 0);
			
		}
	}

	if (yChange != 0)
	{
		if (yChange > 0)
		{
			::SetForegroundWindow(hWndTarget);
			if (yChange > MAX_CHANGE)
			{
				yChange = MAX_CHANGE;
				m_editLog += "Y Axis Moving is too fast\r\n";
				m_hg.WarningMSG("Y Axis Moving is too fast");
				UpdateData(FALSE);
				m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
			}
			for (int i = 0; i < yChange * C_VALUE; i++)
			{
				PostMessageA(hWndTarget, WM_KEYDOWN, VK_UP, 0);
			}
			PostMessageA(hWndTarget, WM_KEYUP, VK_UP, 0);
		}
		else if (yChange < 0)
		{
			::SetForegroundWindow(hWndTarget);
			yChange = yChange * (-1);
			if (yChange > MAX_CHANGE)
			{
				yChange = MAX_CHANGE;
				m_editLog += "Y Axis Moving is too fast\r\n";
				m_hg.WarningMSG("Y Axis Moving is too fast");
				UpdateData(FALSE);
				m_editLogCtrl.LineScroll(m_editLogCtrl.GetLineCount());
			}
			for (int i = 0; i < yChange * C_VALUE; i++)
			{
				PostMessageA(hWndTarget, WM_KEYDOWN, VK_DOWN, 0);
			}
			PostMessageA(hWndTarget, WM_KEYUP, VK_DOWN, 0);
			
		}
	}
}

void CJolJakDlg::ProcG2(GestureInfo gInfo)
{
	::SetForegroundWindow(hWndTarget);

	if (gInfo.vecFingerPts.size() != 2)
		return;
	
	int dx = gInfo.vecFingerPts[0].x - gInfo.vecFingerPts[1].x;
	int dy = gInfo.vecFingerPts[0].y - gInfo.vecFingerPts[1].y;
	int dist = sqrt(dx*dx + dy*dy);
	int level = dist / DIST_LEVEL_VAL;

	
	UpdateData(FALSE);

	if (level > 0 && level < 100)
	{
		::SetForegroundWindow(hWndTarget);
		if (level > 10)
		{
			level = level - 10;
			for (int i = 0; i < level * 30; i++)
			{
				PostMessageA(hWndTarget, WM_KEYDOWN, VK_OEM_PLUS, 0);
			}
			PostMessageA(hWndTarget, WM_KEYUP, VK_OEM_PLUS, 0);

			cv::putText(m_hg.imgSkin, "Zoom In", Point(10, 60), CV_FONT_HERSHEY_TRIPLEX, 1, Scalar(255), 2);
		}
		else
		{
			level = 11 - level;
			for (int i = 0; i < level * 35; i++)
			{
				PostMessageA(hWndTarget, WM_KEYDOWN, VK_OEM_MINUS, 0);
			}
			PostMessageA(hWndTarget, WM_KEYUP, VK_OEM_MINUS, 0);

			cv::putText(m_hg.imgSkin, "Zoom Out", Point(10, 60), CV_FONT_HERSHEY_TRIPLEX, 1, Scalar(255), 2);
		}
	}
}

void CJolJakDlg::ProcG3()
{
	::SetForegroundWindow(hWndTarget);
	contact.pointerInfo.pointerType = PT_TOUCH;
	contact.pointerInfo.pointerId = 0;          //contact 

	contact.pointerInfo.ptPixelLocation.y = M1_Y; // Y co-ordinate of touch on screen
	contact.pointerInfo.ptPixelLocation.x = M1_X; // X co-ordinate of touch on screen
	contact.rcContact.top = contact.pointerInfo.ptPixelLocation.y - 2;
	contact.rcContact.bottom = contact.pointerInfo.ptPixelLocation.y + 2;
	contact.rcContact.left = contact.pointerInfo.ptPixelLocation.x - 2;
	contact.rcContact.right = contact.pointerInfo.ptPixelLocation.x + 2;
	contact.touchFlags = TOUCH_FLAG_NONE;
	contact.touchMask = TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE;
	contact.orientation = 90; // Orientation of 90 means touching perpendicular to screen.
	contact.pressure = 32000;

	contact.pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
	InjectTouchInput(1, &contact); // Injecting the touch down on screen
	contact.pointerInfo.pointerFlags = POINTER_FLAG_UP;
	InjectTouchInput(1, &contact); // Injecting the touch Up from screen

	Sleep(1200);

	contact.pointerInfo.ptPixelLocation.y = M2_Y; // Y co-ordinate of touch on screen
	contact.pointerInfo.ptPixelLocation.x = M2_X; // X co-ordinate of touch on screen
	contact.rcContact.top = contact.pointerInfo.ptPixelLocation.y - 2;
	contact.rcContact.bottom = contact.pointerInfo.ptPixelLocation.y + 2;
	contact.rcContact.left = contact.pointerInfo.ptPixelLocation.x - 2;
	contact.rcContact.right = contact.pointerInfo.ptPixelLocation.x + 2;
	contact.pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
	InjectTouchInput(1, &contact); // Injecting the touch down on screen

	contact.pointerInfo.pointerFlags = POINTER_FLAG_UP;
	InjectTouchInput(1, &contact); // Injecting the touch Up from screen

	Sleep(800);
	contact.pointerInfo.ptPixelLocation.y = 1000; // Y co-ordinate of touch on screen
	contact.pointerInfo.ptPixelLocation.x = 1000; // X co-ordinate of touch on screen
	contact.rcContact.top = contact.pointerInfo.ptPixelLocation.y - 2;
	contact.rcContact.bottom = contact.pointerInfo.ptPixelLocation.y + 2;
	contact.rcContact.left = contact.pointerInfo.ptPixelLocation.x - 2;
	contact.rcContact.right = contact.pointerInfo.ptPixelLocation.x + 2;
	contact.pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
	InjectTouchInput(1, &contact); // Injecting the touch down on screen
	Sleep(100);
	contact.pointerInfo.pointerFlags = POINTER_FLAG_UP;
	InjectTouchInput(1, &contact); // Injecting the touch Up from screen
	::SetForegroundWindow(hWndTarget);
}

void CJolJakDlg::ProcG4(int dx, int dy)
{
	int xChange = dx / 4;
	int yChange = dy / 4;
	int x = 0;
	int y = 0;
	CRect rect;
	GetWindowRect(&rect);
	x = rect.left;
	y = rect.top;
	
	if (xChange != 0)
	{
		if (xChange > 0)
		{
			if (xChange > MAX_CHANGE)
			{
				xChange = MAX_CHANGE;
			}
			xChange = xChange * 70;

			if (x + xChange > 2400)
				xChange = 2400 - x;

			
		}
		else if (xChange < 0)
		{
			
			if ((-1*xChange) > MAX_CHANGE)
			{
				xChange = -MAX_CHANGE;
			}

			xChange = xChange * 70;

			if (x + xChange < 50)
				xChange = 50 - x;

			
		}
	}

	if (yChange != 0)
	{
		if (yChange > 0)
		{
			if (yChange > 2)
			{
				yChange = 2;
			}
			yChange = yChange * 80;

			if (y + yChange > 1300)
				yChange = 1300 - y;

			
		}
		else if (yChange < 0)
		{
			if ((-1*yChange) > 2)
			{
				yChange = -2;
			}
			yChange = yChange * 80;

			if (y + yChange < 30)
				yChange = 30 - y;
		}
	}

	SetWindowPos(&wndTopMost, x + xChange, y + yChange, 0, 0, SWP_NOSIZE);
}

void CJolJakDlg::TouchInit()
{
	contact.pointerInfo.pointerType = PT_TOUCH;
	contact.pointerInfo.pointerId = 0;          //contact 0
	
	contact.touchFlags = TOUCH_FLAG_NONE;
	contact.touchMask = TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE;
	contact.orientation = 90; // Orientation of 90 means touching perpendicular to screen.
	contact.pressure = 32000;
	
}



GestureInfo CJolJakDlg::ImageProcessing(void)
{
	GestureInfo gInfo;
	//static VideoWriter vw("result.avi", CV_FOURCC('F', 'M', 'P', '4'), 30.0, Size(640, 480), true);
	Mat imgCpy;


	if (!m_hg.calibration)
		gInfo.gesture = NEEDCALIB;
	else
		gInfo.gesture = UNKNOWN;
	

	if (!m_hg.readImage()) {
		MessageBox(_T("read error"));
		//타이머 죽이고
		OnBnClickedBtnStop(); //화면 정지 처리;
	}
	imgCpy = m_hg.imgOrigin.clone();

	m_hg.gaussianBlur();
	m_hg.skinColorExtraction();
	m_hg.morphologyOperation(2);

	if (!m_hg.drawHandRegion())
	{
#ifdef __DEBUGVISUAL__
		m_hg.displayImage();
#endif
		return gInfo;
	}


	if (!m_hg.handValidation())
	{
#ifdef __DEBUGVISUAL__
		m_hg.displayImage();
#endif
		m_hg.destroyObject();
		m_hg.iAramLastAngle = 0;
		m_hg.initFingerCnt = 0;

		return gInfo;
	}

	m_hg.getMoment();
	m_hg.getDistanceTransform();
	if (!m_hg.featureExtraction())
	{
		m_hg.displayImage();
		return gInfo;
	}

	if (!m_hg.gestureClassification())
	{
#ifdef __DEBUGVISUAL__
		m_hg.displayImage();
#endif
		m_hg.destroyObject();
		return gInfo;
	}

	m_hg.dbgVisualization();
	m_hg.testFunc();

	if (!m_hg.calibration)
		gInfo.gesture = NEEDCALIB;
	else
		m_hg.makeGestureInform(gInfo);
	//이곳쯤에 제스처 인식쪽 만들어서 리턴갈겨

	//m_hg.displayImage();
#ifdef __DEBUGVISUAL__
	m_hg.displayImage();
#endif
	m_hg.destroyObject();

	//vw.write(imgCpy);
	return gInfo;
}