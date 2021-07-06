// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"

#include <ShlObj_core.h>

typedef HRESULT(__stdcall *TDllCanUnloadNow)(VOID);
typedef HRESULT(__stdcall *TDllGetClassObject)(REFCLSID rclsid, REFIID riid, LPVOID *ppReturn);
typedef BOOL(__stdcall *TExtTextOutW)(HDC hdc, int x,int y,UINT options,const RECT *lprect,LPCWSTR lpString,UINT c,const INT *lpDx);
//typedef HWND(__stdcall * TCreateWindowInBand)(_In_ DWORD dwExStyle, ATOM lpClassName, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);
typedef BOOL(__stdcall * TGetWindowBand)(HWND hWnd, PDWORD pdwBand);
typedef BOOL(__stdcall * TSetWindowBand)(HWND hWnd, DWORD dwBand);


TDllCanUnloadNow g_explorerFrameCanUnload = NULL;
TDllGetClassObject g_explorerFrameGetClass = NULL;
TGetWindowBand GetWindowBand = NULL;
TGetWindowBand SetWindowBand = NULL;

HMODULE g_explorerFrame = NULL;
HANDLE g_hThread = NULL;
HWND g_hwndActivateWindows = 0;
BOOL g_Running = TRUE;

WNDPROC g_OriginalGenuineWndProc = NULL;

BOOL _InitializeExplorerFrame();
DWORD WINAPI _WatchGenuineWatermark(PVOID args);
BOOL CALLBACK _FindGenuineWatermark(HWND hwnd,LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

		GetWindowBand = (TGetWindowBand)GetProcAddress(LoadLibraryW(L"user32.dll"), "GetWindowBand");
		SetWindowBand = (TSetWindowBand)GetProcAddress(LoadLibraryW(L"user32.dll"), "SetWindowBand");

		if (!g_hThread) {
			g_hThread = CreateThread(NULL, 0, &_WatchGenuineWatermark, NULL, 0, NULL);
		}

		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:

		break;
    case DLL_PROCESS_DETACH:

		g_Running = FALSE;
		if (g_hThread) {
			if (WaitForSingleObject(g_hThread, 10000) == WAIT_TIMEOUT) {
				TerminateThread(g_hThread,0);
			}
			CloseHandle(g_hThread);
		}

		if (g_OriginalGenuineWndProc) {
			SetWindowLongPtrW(g_hwndActivateWindows, GWLP_WNDPROC, (LONG_PTR)g_OriginalGenuineWndProc);
		}

        break;
    }
    return TRUE;
}


HRESULT __stdcall DllCanUnloadNow(VOID)
{
	//redirect to explorerframe.dll
	if (!_InitializeExplorerFrame()) {
		return S_FALSE;
	}

	return g_explorerFrameCanUnload();
}


HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppReturn)
{
	//redirect to explorerframe.dll
	if (!_InitializeExplorerFrame()) {
		return CLASS_E_CLASSNOTAVAILABLE;
	}

	return g_explorerFrameGetClass(rclsid,riid,ppReturn);
}

BOOL _InitializeExplorerFrame() {
	BOOL re = FALSE;

	if (!g_explorerFrame) {
		g_explorerFrame = LoadLibraryW(L"explorerframe.dll");

		if (!g_explorerFrame) {
			goto escapeArea;
		}
	}

	if (!g_explorerFrameCanUnload) {
		g_explorerFrameCanUnload = (TDllCanUnloadNow)GetProcAddress(g_explorerFrame, "DllCanUnloadNow");

		if (!g_explorerFrameCanUnload) {
			goto escapeArea;
		}
	}

	if (!g_explorerFrameGetClass) {
		g_explorerFrameGetClass = (TDllGetClassObject)GetProcAddress(g_explorerFrame, "DllGetClassObject");

		if (!g_explorerFrameGetClass) {
			goto escapeArea;
		}
	}

	re = TRUE;

escapeArea:

	return re;
}

LRESULT CALLBACK _CustomWatermarkProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_PAINT: {
			XTrace(L"Repaint");
			return 0;
		};
	}

	return g_OriginalGenuineWndProc(hWnd, msg, wParam, lParam);
}

DWORD WINAPI _WatchGenuineWatermark(PVOID args) {
	LONG_PTR status = 0;

	EnumChildWindows(GetDesktopWindow(), &_FindGenuineWatermark, NULL);

	if (g_hwndActivateWindows) {

		while(g_Running) {

			status = GetWindowLongPtrW(g_hwndActivateWindows, GWL_STYLE);
			if (!status) {
				break;
			}

			if (status & WS_VISIBLE) {
				XTrace(L"Watermark is Visible. Hide it");
				SetWindowBand(g_hwndActivateWindows, 1);

				//SetWindowPos(g_hwndActivateWindows, HWND_BOTTOM, 1, 1, 1, 1, 0);
				ShowWindow(g_hwndActivateWindows, SW_HIDE);
				InvalidateRect(g_hwndActivateWindows, NULL, TRUE);
			}

			Sleep(10000);
		}
	}

	return 0;
}

BOOL CALLBACK _FindGenuineWatermark(HWND hwnd, LPARAM lParam) {
	DWORD dwBand = 0;

	GetWindowBand(hwnd, &dwBand);

	if (dwBand == 14) {
		XTrace(L"Find Watermark");
		g_hwndActivateWindows = hwnd;

		g_OriginalGenuineWndProc = (WNDPROC)GetWindowLongPtrW(g_hwndActivateWindows, GWLP_WNDPROC);
		SetWindowLongPtrW(g_hwndActivateWindows, GWLP_WNDPROC, (LONG_PTR)_CustomWatermarkProc);

		return FALSE;
	}

	return TRUE;
}