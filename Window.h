#pragma once
#include "Windows.h"
#include <CommCtrl.h>
class Window
{
private:
	HWND hwnd;
	static LRESULT __stdcall WndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam);
	HACCEL hAccelTable = NULL;
public:
	HWND GetHWND();
	void SetAccelTable(HACCEL);
	void RunWindow();
	Window(WNDPROC = WndProc, HINSTANCE = NULL, int = 1, DWORD = WS_OVERLAPPEDWINDOW, DWORD = 0, LPCWSTR = L"Window", HMENU hmenu = NULL, int = CW_USEDEFAULT, int = CW_USEDEFAULT, int = CW_USEDEFAULT, int = CW_USEDEFAULT, HWND = NULL);
	virtual ~Window();

};

