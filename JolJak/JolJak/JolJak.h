
// JolJak.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CJolJakApp:
// �� Ŭ������ ������ ���ؼ��� JolJak.cpp�� �����Ͻʽÿ�.
//

class CJolJakApp : public CWinApp
{
public:
	CJolJakApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CJolJakApp theApp;