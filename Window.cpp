#include "Window.h"

//HACCEL Window::hAccelTable = NULL;

LRESULT __stdcall Window::WndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProcW(hwnd, uint, wparam, lparam);
}

HWND Window::GetHWND()
{
	return this->hwnd;
}

void Window::SetAccelTable(HACCEL hAccel)
{
	hAccelTable = hAccel;
}

void Window::RunWindow()
{
	try {
		MSG msg;
		while (BOOL res = GetMessage(&msg, this->hwnd, 0, 0)) {
			if (res == -1) {
				throw GetLastError();
			}
			if (!TranslateAccelerator(hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
	}

	catch (DWORD errCode) {
		//próba poinformowania u¿ytkownika, ¿e coœ siê spartoli³o
		WCHAR buffer[32]{};
		_itow_s(errCode, buffer, 32, 10);

		const size_t msg_size = 100;
		WCHAR msg[msg_size] = L"There was an error, which has caused the program to quit. Error code: ";

		wcscat_s(msg, msg_size, buffer);
		MessageBox(this->hwnd, msg, L"Error", MB_ICONERROR | MB_OK);
	}
}

Window::Window(WNDPROC wndproc, HINSTANCE hinstance, int cmd_show, DWORD style, DWORD styleex, LPCWSTR windowname, HMENU hmenu, int x_position, int y_position, int x_size, int y_size, HWND parent)
{
	//inicjalizacja klasy okna
	WNDCLASSEX wndClass{};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpszClassName = L"ClassName";
	wndClass.lpfnWndProc = wndproc;
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = hinstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	//próba stworzenia okna
	try {
		ATOM atom = RegisterClassExW(&wndClass);
		if (!atom)
			throw GetLastError();

		this->hwnd = CreateWindowEx(styleex, wndClass.lpszClassName, windowname, style, x_position, y_position, x_size, y_size, parent, hmenu, hinstance, NULL);
		if (!this->hwnd)
			throw GetLastError();

		ShowWindow(hwnd, cmd_show);
		UpdateWindow(hwnd);

	}
	catch (DWORD errCode) {
		//próba poinformowania u¿ytkownika, ¿e coœ siê spartoli³o
		WCHAR buffer[32]{};
		_itow_s(errCode, buffer, 32, 10);

		const size_t msg_size = 100;
		WCHAR msg[msg_size] = L"There was an error, which has caused the program to quit. Error code: ";

		wcscat_s(msg, msg_size, buffer);
		MessageBox(this->hwnd, msg, L"Error", MB_ICONERROR | MB_OK);
	}
}

Window::~Window()
{
	DestroyWindow(hwnd); 
	hwnd = NULL;
}
