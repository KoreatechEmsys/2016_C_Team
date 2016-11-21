// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include <tchar.h>



#define MSG_MOUSE_MOVE WM_USER+1
#define MSG_MOUSE_LBUP WM_USER+2
#define MSG_MOUSE_LBDOWN WM_USER+3
#define MSG_MOUSE_RBUP WM_USER+4
#define MSG_MOUSE_RBDOWN WM_USER+5
#define MSG_MOUSE_DRAG WM_USER+6
#define MSG_MOUSE_WHEEL WM_USER+7
#define MAX_PATH_SIZE 255
#define DEF_PROCESS_NAME "Microsoft Surface Globe"
#pragma data_seg(".Segment")

HWND hWndServer = NULL;
HWND hWndTarget = NULL;
DWORD threadID;
BOOL b_message_pass;
int x;
#pragma data_seg()
#pragma comment(linker, "/section:.Segment,rws")

HINSTANCE hInst;
HHOOK hook;

static LRESULT CALLBACK MouseMsgProc(UINT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInst = hModule;
		break;
	default:
		break;
	}
	return TRUE;
}


extern "C" __declspec(dllexport) void SetMessagePass(BOOL val) {
	b_message_pass = val;
}

extern "C" __declspec(dllexport)  BOOL InstallHook(HWND hWndParent, BOOL message_pass, HWND* t_hwnd)
{
	if (hWndServer != NULL)
		return FALSE; // already hooked!
	hWndTarget = FindWindow(NULL, _T(DEF_PROCESS_NAME));


	if (hWndTarget == NULL)
		return FALSE;
	threadID = GetWindowThreadProcessId(hWndTarget, NULL);

	hook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseMsgProc, hInst, threadID);

	if (hook != NULL)
	{
		hWndServer = hWndParent;
		*t_hwnd = hWndTarget;
		if (message_pass == TRUE)
			b_message_pass = TRUE;
		else
			b_message_pass = FALSE;
		return TRUE;
	}
	return FALSE;

}

extern "C" __declspec(dllexport) BOOL UnInstallHook(HWND hWndParent)
{
	if (hWndParent != hWndServer || hWndParent == NULL)
		return FALSE;
	BOOL unhooked = UnhookWindowsHookEx(hook);
	if (unhooked)
		hWndServer = NULL;
	b_message_pass = true;
	return unhooked;
}



static LRESULT CALLBACK MouseMsgProc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	char path[MAX_PATH] = { 0, };
	char * p_proc_name = NULL;
	if (nCode < 0)
	{
		CallNextHookEx(hook, nCode, wParam, lParam);
		return 0;
	}

	return CallNextHookEx(hook, nCode, wParam, lParam);
	/*GetModuleFileName(NULL, path, MAX_PATH);
	p_proc_name = strrchr(path, '\\');

	MSLLHOOKSTRUCT * pMouseStruct = (MSLLHOOKSTRUCT *)lParam;
	COPYDATASTRUCT cds;


	long lp;
	if (pMouseStruct != NULL) {
		lp = lParam;
		switch (wParam) {
		case WM_MOUSEMOVE:
			cds.cbData = 4;
			cds.dwData = MSG_MOUSE_MOVE;
			cds.lpData = &lp;
			SendMessage(hWndServer, WM_COPYDATA, wParam, (LPARAM)&cds);
			if (b_message_pass)
				return CallNextHookEx(hook, nCode, wParam, lParam);
			else
				return 1;
			break;
		case WM_LBUTTONUP:
			cds.cbData = 4;
			cds.dwData = MSG_MOUSE_LBUP;
			cds.lpData = &lp;
			SendMessage(hWndServer, WM_COPYDATA, wParam, (LPARAM)&cds);
			if (b_message_pass)
				return CallNextHookEx(hook, nCode, wParam, lParam);
			else
				return 1;
			break;
		case WM_LBUTTONDOWN:
			cds.cbData = 4;
			cds.dwData = MSG_MOUSE_LBDOWN;
			cds.lpData = &lp;
			SendMessage(hWndServer, WM_COPYDATA, wParam, (LPARAM)&cds);
			if (b_message_pass)
				return CallNextHookEx(hook, nCode, wParam, lParam);
			else
				return 1;
			break;
		case WM_RBUTTONUP:
			cds.cbData = 4;
			cds.dwData = MSG_MOUSE_RBUP;
			cds.lpData = &lp;
			SendMessage(hWndServer, WM_COPYDATA, wParam, (LPARAM)&cds);
			if (b_message_pass)
				return CallNextHookEx(hook, nCode, wParam, lParam);
			else
				return 1;
			break;
		case WM_RBUTTONDOWN:
			cds.cbData = 4;
			cds.dwData = MSG_MOUSE_RBDOWN;
			cds.lpData = &lp;
			SendMessage(hWndServer, WM_COPYDATA, wParam, (LPARAM)&cds);
			if (b_message_pass)
				return CallNextHookEx(hook, nCode, wParam, lParam);
			else
				return 1;
			break;
		case WM_MOUSEWHEEL:
			cds.cbData = 4;
			cds.dwData = MSG_MOUSE_WHEEL;
			cds.lpData = &lp;
			SendMessage(hWndServer, WM_COPYDATA, wParam, (LPARAM)&cds);
			if (b_message_pass)
				return CallNextHookEx(hook, nCode, wParam, lParam);
			else
				return 1;
			return CallNextHookEx(hook, nCode, wParam, lParam);
			break;

		}
	}*/
}