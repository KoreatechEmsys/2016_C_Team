
// JolJakDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "afxext.h"

#define MIN_GESTURE_CHANGE_COUNT 8
#define COG_THRESH_VAL 2
#define C_VALUE 30

#define DIST_THRESH_VAL 30
#define DIST_LEVEL_VAL 10

#define M1_X 1550
#define M1_Y 1387
#define M2_X 1476
#define M2_Y 1387

// CJolJakDlg ��ȭ ����
class CJolJakDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CJolJakDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JOLJAK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;
	HandGesture m_hg;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	HWND hWndTarget;
	int gestureChangeCount;
	GestureInfo lastGesture;

public:
	void initProc(void);

	POINTER_TOUCH_INFO contact;

	CButton m_btnStart;
	CButton m_btnStop;
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void GestureProcessing(GestureInfo gInfo);
	void DisplayStatus(GestureInfo gInfo);
	void ProcG1(int dx, int dy);
	void ProcG2(GestureInfo gInfo);
	void ProcG3();

	void ProcG4(int dx, int dy);

	void TouchInit();

	GestureInfo ImageProcessing(void);

	CString m_staticGesture;
	CString m_staticX;
	CString m_staticY;
	afx_msg void OnBnClickedBtnTest();

	
//	CStatic m_staticDist;
	//CString m_staticDist;
//	CButton m_btnManual;
	void displayImage(Mat img_arr, int item);
	CString m_editLog;
	CEdit m_editLogCtrl;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	CBitmapButton m_btnManual;
};

