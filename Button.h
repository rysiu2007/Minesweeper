#pragma once
#include <Windows.h>
#include "game.h"
#include <uxtheme.h>
#include <CommCtrl.h>

class Button
{
protected:
	void Uncover();
	static void Uncover(Button& button);
	static void UncoverMines();
	HWND hwnd;
	//LPCWSTR text = L"";
	static LRESULT ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

public:
	static bool IsFlag(Button&);
	//number of neighbouring mines, -1 if mine
	INT neighbours = 0;
	Button* neighbours_list[8] = {nullptr};
	static HFONT hMinesFont;
	HWND GetHWND();
	Button() = default;
	void Init(HWND parent, LPCWSTR text = L"", int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int x_size = CW_USEDEFAULT, int y_size = CW_USEDEFAULT);
	Button(HWND parent, LPCWSTR text = L"", int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int x_size = CW_USEDEFAULT, int y_size = CW_USEDEFAULT);
	virtual ~Button();
};

