#include "Window.h"
#include "Button.h"
#include "game.h"
#include "resource.h"
#include "time.h"
#include <CommCtrl.h>

#define IDT_TIMER_1 102

Button** buttons;
HFONT segmentedFont;
HANDLE hFontResource = NULL;

HWND new_gameButton;

HACCEL hAccelTable;
//FaceState state = HAPPY;

void UpdateFont(int fontSize) {
	if (segmentedFont) DeleteObject(segmentedFont);

	segmentedFont = CreateFont(
		-fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, FIXED_PITCH | FF_DONTCARE,
		L"Segment7" // <--- The name found inside the .ttf file
	);
}
inline int neg_sign(int num) {
	return (num & 0x80000000) >> 31;
}
static int number_length(int num) {
	int c = 0;
	if (num == 0)
		return 1;
	while (abs(num) > 0) {
		num /= 10;
		c++;
	}
	return c;
}

INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE; 
}

INT_PTR CALLBACK MyDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_EDIT4, GameData::minefield_x_size, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT5, GameData::minefield_y_size, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT6, GameData::starting_mines_count, FALSE);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		// Handle OK and Cancel to actually close the window
		if (LOWORD(wParam) == IDOK)
		{
			UINT x_size = GetDlgItemInt(hDlg, IDC_EDIT4, NULL, FALSE);
			UINT y_size = GetDlgItemInt(hDlg, IDC_EDIT5, NULL, FALSE);
			UINT mine_count = GetDlgItemInt(hDlg, IDC_EDIT6, NULL, FALSE);

			if (x_size > 50 || y_size > 50 || mine_count >= y_size * x_size)
			{
				MessageBox(hDlg, L"Invalid parameters", L"Error", MB_ICONERROR);
				return (INT_PTR)FALSE;
			}

			
			EndDialog(hDlg, LOWORD(wParam));
			GameData::InitGame(GetParent(hDlg), y_size, x_size, mine_count);
			RECT rc;
			GetClientRect(GetParent(hDlg), &rc);

			// LOWORD is width, HIWORD is height
			LPARAM lParam = MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top);

			// SIZE_RESTORED tells the window it's a normal resize
			SendMessage(GetParent(hDlg), WM_SIZE, SIZE_RESTORED, lParam);
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void DrawStylizedText(HDC hdc, LPRECT paintRect, LPCWSTR mine_counter) {

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	HBRUSH black_brush = CreateSolidBrush(RGB(0, 0, 0));

	FillRect(hdc, paintRect, black_brush);

	paintRect->top += tm.tmDescent / 2;
	paintRect->bottom += tm.tmDescent / 2;
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(25, 0, 0)); // Red for mines
	DrawTextW(hdc, L"888", -1, paintRect, DT_CENTER|DT_VCENTER | DT_SINGLELINE);
	SetTextColor(hdc, RGB(200, 50, 50)); // Red for mines
	DrawTextW(hdc, mine_counter, -1, paintRect, DT_CENTER|DT_VCENTER | DT_SINGLELINE);

	paintRect->top -= tm.tmDescent / 2;
	paintRect->bottom -= tm.tmDescent / 2;
	DrawEdge(hdc, paintRect, EDGE_SUNKEN, BF_RIGHT|BF_LEFT);
	DeleteObject(black_brush);
}


LRESULT WINAPI WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	//HWND hwndButton = 0;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rc;
	RECT paintRect,stopwatchRect;
	WCHAR mine_counter[10]{};
	//if (hwnd == buttons[0][0])
	//	throw;
	switch (umsg) {
	case WM_CREATE:
		
		new_gameButton = CreateWindow(L"BUTTON", L" ", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON| BS_OWNERDRAW, 10, 10, 20, 20, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
		//new_gameButton = CreateWindow(L"Button");
		//init brushes
		GameData::primary_background = CreateSolidBrush(RGB(200, 200, 200));

		//set timer
		SetTimer(hwnd, IDT_TIMER_1, 100, NULL);

		GetClientRect(hwnd, &rc);
		srand(time(0));
		UpdateFont(24);
		GameData::InitGame(hwnd);
		buttons = (Button**)GameData::minefield;
		break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		if (wmEvent == BN_CLICKED) {
			// You can identify which button it is using its HWND
			HWND hBtnClicked = (HWND)lParam;
			if (hBtnClicked == new_gameButton) {
				GameData::InitGame(hwnd);
				RECT rc;
				GetClientRect(hwnd, &rc);

				// LOWORD is width, HIWORD is height
				LPARAM lParam = MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top);

				// SIZE_RESTORED tells the window it's a normal resize
				SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, lParam);
				//RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
			}
		}
		switch (wmId)
		{
		case ID_GAME_NEWGAME:
		{
			GameData::InitGame(hwnd);
			RECT rc;
			GetClientRect(hwnd, &rc);

			// LOWORD is width, HIWORD is height
			LPARAM lParam = MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top);

			// SIZE_RESTORED tells the window it's a normal resize
			SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, lParam);
			break;
		}
		case ID_GAME_EXIT:
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case ID_DIFFICULTY_EASY:
		{
			GameData::InitGame(hwnd, 8, 8, 10);
			RECT rc;
			GetClientRect(hwnd, &rc);

			// LOWORD is width, HIWORD is height
			LPARAM lParam = MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top);

			// SIZE_RESTORED tells the window it's a normal resize
			SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, lParam);
			break;
		}
		case ID_DIFFICULTY_MEDIUM:
		{
			GameData::InitGame(hwnd, 16, 16, 40);
			RECT rc;
			GetClientRect(hwnd, &rc);

			// LOWORD is width, HIWORD is height
			LPARAM lParam = MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top);

			// SIZE_RESTORED tells the window it's a normal resize
			SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, lParam);
			break;
		}
		case ID_DIFFICULTY_EXPERT:
		{
			GameData::InitGame(hwnd, 16, 30, 99);
			RECT rc;
			GetClientRect(hwnd, &rc);

			// LOWORD is width, HIWORD is height
			LPARAM lParam = MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top);

			// SIZE_RESTORED tells the window it's a normal resize
			SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, lParam);
			break;
		}
		case ID_DIFFICULTY_SYNAK:
		{
			GameData::InitGame(hwnd, 20, 40, 180);
			RECT rc;
			GetClientRect(hwnd, &rc);

			// LOWORD is width, HIWORD is height
			LPARAM lParam = MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top);

			// SIZE_RESTORED tells the window it's a normal resize
			SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, lParam);
			break;
		}

		case ID_DIFFICULTY_CUSTOM:
		{
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, MyDialogProc);

			break;

		}
		case ID_ABOUT_INFO:
		{
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), hwnd, AboutDialogProc);
			break;
		}
		case ID_ABOUT_HELP:
		{
			ShellExecute(NULL, L"open", L"https://minesweeper-pro.com/introduction/",NULL,NULL,SW_SHOWNORMAL);
		}

		default:
			break;
		}
		return 0;
	}
	case WM_SIZE:
	{
		GetClientRect(hwnd, &rc);
		DeleteObject(Button::hMinesFont);
		Button::hMinesFont = CreateFont(
			int(1 * (rc.bottom - rc.top) * 0.75 / GameData::minefield_y_size),                        // Height (adjusted for your button size)
			0,                         // Width (0 = let Windows choose)
			0, 0,                      // Angles
			FW_HEAVY,                   // Weight: This makes the numbers "pop"
			FALSE, FALSE, FALSE,       // Italic, Underline, Strikeout
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,         // Quality: Makes edges smooth
			DEFAULT_PITCH | FF_SWISS,  // FF_SWISS is for sans-serif fonts
			L"Arial"                   // Typeface namege
		);

		UpdateFont(min(int(2 * (rc.bottom - rc.top) * 0.075), int(2.5 * (rc.right - rc.left) * 0.075)));

		HDWP hdwp = BeginDeferWindowPos(GameData::minefield_x_size * GameData::minefield_y_size);

		// Calculate the total available drawing area once
		int totalAreaW = (rc.right - rc.left) * 9 / 10;
		int totalAreaH = (rc.bottom - rc.top) * 7 / 10;

		// Calculate offsets once
		int offsetX = (rc.right - rc.left) * 9 / 100 * 55 / 100;
		int offsetY = (rc.bottom - rc.top) * 7 / 100 * 35 / 10;

		for (int i = 0; i < GameData::minefield_y_size; i++)
		{
			// Calculate vertical edges for this row
			int yStart = (totalAreaH * i) / GameData::minefield_y_size + offsetY;
			int yEnd = (totalAreaH * (i + 1)) / GameData::minefield_y_size + offsetY;
			int height = yEnd - yStart;

			for (int j = 0; j < GameData::minefield_x_size; j++)
			{
				// Calculate horizontal edges for this column
				int xStart = (totalAreaW * j) / GameData::minefield_x_size + offsetX;
				int xEnd = (totalAreaW * (j + 1)) / GameData::minefield_x_size + offsetX;
				int width = xEnd - xStart;

				hdwp = DeferWindowPos(hdwp, GameData::minefield[i][j].GetHWND(), 0,
					xStart, yStart, width, height,
					SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
			}
		}
		hdwp = DeferWindowPos(hdwp, new_gameButton, 0,
			(rc.right - rc.left) / 2 - int((rc.bottom - rc.top) * 0.1)/2, int((rc.bottom - rc.top) * 0.075), int((rc.bottom - rc.top) * 0.1), int((rc.bottom - rc.top) * 0.1), SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
		EndDeferWindowPos(hdwp);
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
		//UpdateWindow(hwnd);

		return 0;
	}
	case WM_TIMER:
	{
		if (GameData::isPlaying) {
			GameData::time_100ms++;
		}
		InvalidateRect(hwnd, NULL, FALSE);
		InvalidateRect(new_gameButton, NULL, FALSE);
		//UpdateWindow(hwnd);
		break;
	}
	case WM_DRAWITEM:
	{


		DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)(lParam);

		int x_size = dis->rcItem.right - dis->rcItem.left;
		int y_size = dis->rcItem.bottom - dis->rcItem.top;


		HDC memDC = CreateCompatibleDC(dis->hDC);
		HBITMAP memBM = CreateCompatibleBitmap(dis->hDC, x_size, y_size);
		SelectObject(memDC, memBM);
		HBRUSH yellow_brush = CreateSolidBrush(RGB(255, 255, 0));
		SelectObject(memDC, yellow_brush);
		HPEN hpen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		HPEN hthickpen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
		SelectObject(memDC, hpen);
		FillRect(memDC, &dis->rcItem, GameData::primary_background);
		DrawEdge(memDC, &dis->rcItem, EDGE_RAISED, BF_RECT);

		if (dis->itemState & ODS_SELECTED) {
			DrawEdge(memDC, &dis->rcItem, EDGE_SUNKEN, BF_RECT);
		}

		//Draw the face
		Ellipse(memDC, int(dis->rcItem.left+0.15*x_size), int(dis->rcItem.top+0.15*y_size), int(dis->rcItem.right-0.15*x_size), int(dis->rcItem.bottom-0.15*y_size));

		if (GameData::state != DEAD && GameData::state != WINNER) {
			if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
				// Left mouse button is currently DOWN
				GameData::state = EXCITED;
			}
			else {
				GameData::state = HAPPY;
			}
		}
		//GameData::state = WINNER;
		//Draw eyes
		switch (GameData::state) {

		case HAPPY:
			Ellipse(memDC, int(dis->rcItem.left + 0.37 * x_size), int(dis->rcItem.top + 0.4 * y_size), int(dis->rcItem.left + 0.42 * x_size), int(dis->rcItem.top + 0.45 * y_size));
			Ellipse(memDC, int(dis->rcItem.left + 0.63 * x_size), int(dis->rcItem.top + 0.4 * y_size), int(dis->rcItem.left + 0.58 * x_size), int(dis->rcItem.top + 0.45 * y_size));

			//Draw smile
			//MoveToEx(memDC, dis->rcItem.left)
			Arc(memDC, dis->rcItem.left + x_size / 4, 
				dis->rcItem.top, dis->rcItem.right - x_size / 4, 
				dis->rcItem.bottom - int(y_size)*0.3,
				dis->rcItem.left + x_size / 4,
				dis->rcItem.bottom - int(y_size) * 0.3,
				dis->rcItem.right - x_size / 4, 
				dis->rcItem.bottom - int(y_size) * 0.3);
			break;
		case EXCITED:

			Ellipse(memDC, int(dis->rcItem.left + 0.42 * x_size), int(dis->rcItem.top + 0.52 * y_size), int(dis->rcItem.left + 0.58 * x_size), int(dis->rcItem.top + 0.7 * y_size));

			//SelectObject(memDC,GetStockObject(BLACK_BRUSH));

			Ellipse(memDC, int(dis->rcItem.left + 0.32 * x_size), int(dis->rcItem.top + 0.35 * y_size), int(dis->rcItem.left + 0.42 * x_size), int(dis->rcItem.top + 0.45 * y_size));
			Ellipse(memDC, int(dis->rcItem.left + 0.68 * x_size), int(dis->rcItem.top + 0.35 * y_size), int(dis->rcItem.left + 0.58 * x_size), int(dis->rcItem.top + 0.45 * y_size));
			break;
		case DEAD:
			MoveToEx(memDC, int(dis->rcItem.left + 0.3 * x_size), int(dis->rcItem.top + 0.35 * y_size), NULL);
			LineTo(memDC, int(dis->rcItem.left + 0.42 * x_size), int(dis->rcItem.top + 0.47 * y_size));

			MoveToEx(memDC, int(dis->rcItem.left + 0.42 * x_size), int(dis->rcItem.top + 0.35 * y_size), NULL);
			LineTo(memDC, int(dis->rcItem.left + 0.3 * x_size), int(dis->rcItem.top + 0.47 * y_size));

			MoveToEx(memDC, int(dis->rcItem.left + 0.7 * x_size), int(dis->rcItem.top + 0.35 * y_size), NULL);
			LineTo(memDC, int(dis->rcItem.left + 0.58 * x_size), int(dis->rcItem.top + 0.47 * y_size));

			MoveToEx(memDC, int(dis->rcItem.left + 0.58 * x_size), int(dis->rcItem.top + 0.35 * y_size), NULL);
			LineTo(memDC, int(dis->rcItem.left + 0.7 * x_size), int(dis->rcItem.top + 0.47 * y_size));

			SetArcDirection(memDC, AD_CLOCKWISE);
			Arc(memDC, dis->rcItem.left + x_size / 4,
				dis->rcItem.bottom - int(y_size) * 0.4,
				dis->rcItem.right - x_size / 4,
				dis->rcItem.bottom,
				dis->rcItem.left,
				dis->rcItem.bottom - int(y_size) * 0.55,
				dis->rcItem.right,
				dis->rcItem.bottom - int(y_size) * 0.55);
			break;
		case WINNER:

			Arc(memDC, dis->rcItem.left + x_size / 4,
				dis->rcItem.top, dis->rcItem.right - x_size / 4,
				dis->rcItem.bottom - int(y_size) * 0.3,
				dis->rcItem.left + x_size / 3,
				dis->rcItem.bottom - int(y_size) * 0.3,
				dis->rcItem.right - x_size / 3,
				dis->rcItem.bottom - int(y_size) * 0.3);

			SelectObject(memDC,GetStockObject(BLACK_BRUSH));

			Ellipse(memDC, int(dis->rcItem.left + 0.3 * x_size), int(dis->rcItem.top + 0.38 * y_size), int(dis->rcItem.left + 0.45 * x_size), int(dis->rcItem.top + 0.53 * y_size));
			Ellipse(memDC, int(dis->rcItem.left + 0.7 * x_size), int(dis->rcItem.top + 0.38 * y_size), int(dis->rcItem.left + 0.55 * x_size), int(dis->rcItem.top + 0.53 * y_size));

			MoveToEx(memDC, int(dis->rcItem.left + 0.15 * x_size), int(dis->rcItem.top + 0.5 * y_size), NULL);
			LineTo(memDC, int(dis->rcItem.left + 0.3 * x_size), int(dis->rcItem.top + 0.4 * y_size));
			SelectObject(memDC, hthickpen);
			LineTo(memDC, int(dis->rcItem.right - 0.3 * x_size), int(dis->rcItem.top + 0.4 * y_size));
			SelectObject(memDC, hpen);
			LineTo(memDC, int(dis->rcItem.right - 0.15 * x_size), int(dis->rcItem.top + 0.5 * y_size));
			
			break;
		}

		BitBlt(dis->hDC, 0, 0, x_size, y_size, memDC, 0, 0, SRCCOPY);

		DeleteObject(hthickpen);
		DeleteObject(hpen);
		DeleteObject(yellow_brush);

		DeleteObject(memBM);
		DeleteDC(memDC);

		break;
	}
	case WM_PAINT:
	{

		mine_counter[0]= L'0';
		mine_counter[1] = L'0';
		mine_counter[2] = L'0';
		_itow_s(GameData::mine_count - GameData::flag_count, 
			mine_counter+3-neg_sign(GameData::mine_count - GameData::flag_count)-number_length(GameData::mine_count - GameData::flag_count), 
			10 + neg_sign(GameData::mine_count - GameData::flag_count) + number_length(GameData::mine_count - GameData::flag_count) - 3, 
			10);
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);

		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBM = CreateCompatibleBitmap(hdc, width, height);
		SelectObject(memDC, memBM);

		//Draw background
		FillRect(memDC, &rc, GameData::primary_background);

		paintRect = { (rc.right - rc.left) * 9 / 100 * 55 / 100 - 1,
		(rc.bottom - rc.top) * 7 / 100 * 35 / 10 - 1,
		long((rc.right - rc.left) * 0.95f),
		long((rc.bottom - rc.top) * 0.945f) };
		FillRect(memDC, &paintRect, (HBRUSH)(COLOR_WINDOWFRAME + 1));
		DrawEdge(memDC, &paintRect, EDGE_SUNKEN, BF_RECT);

		paintRect = { (rc.right - rc.left) * 9 / 100 * 55 / 100 - 1,
		long((rc.bottom - rc.top) * 0.05),
		long((rc.right - rc.left) * 0.95f),
		long((rc.bottom - rc.top) * 0.2f) };

		FillRect(memDC, &paintRect, GameData::primary_background);
		DrawEdge(memDC, &paintRect, EDGE_SUNKEN, BF_RECT);


		//Draw mine counter
		SIZE size;
		HFONT hOldFont = (HFONT)SelectObject(memDC, segmentedFont);
		GetTextExtentPoint32(memDC, mine_counter, lstrlen(mine_counter), &size);
		paintRect = { (rc.right - rc.left) * 9 / 100 * 55 / 100 , long((rc.bottom - rc.top) * 0.05 + 2), min((rc.right - rc.left) * 9 / 100 * 55 / 100 + size.cx, long((rc.right - rc.left) * 0.95f)) ,long((rc.bottom - rc.top) * 0.2f) - 2 };
		DrawStylizedText(memDC, &paintRect, mine_counter);

		//Draw stopwatch
		mine_counter[0] = L'0';
		mine_counter[1] = L'0';
		mine_counter[2] = L'0';
		_itow_s(GameData::time_100ms/10,
			mine_counter + 3 - number_length(GameData::time_100ms / 10),
			10 + neg_sign(GameData::time_100ms / 10) + number_length(GameData::time_100ms / 10) - 3,
			10);

		GetTextExtentPoint32(memDC, mine_counter, lstrlen(mine_counter), &size);
		paintRect = { max(long((rc.right - rc.left) * 0.95f)- size.cx, 
			(rc.right - rc.left) * 9 / 100 * 55 / 100), 
			long((rc.bottom - rc.top) * 0.05 + 2), 
			long((rc.right - rc.left) * 0.95f),
			long((rc.bottom - rc.top) * 0.2f) - 2 };
		stopwatchRect = paintRect;
		DrawStylizedText(memDC, &paintRect, mine_counter);


		BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

		SelectObject(memDC, hOldFont);
		DeleteObject(memBM);
		DeleteDC(memDC);

		EndPaint(hwnd, &ps);
		break;
	}

	case WM_ERASEBKGND:
	{
		return 1;
	}
	case WM_DESTROY:
		DestroyWindow(new_gameButton);
		DeleteObject(GameData::primary_background);
		//for (int i = 0; i < GameData::minefield_y_size; i++) {
		//	for (int j = 0; j < GameData::minefield_x_size; j++) {

		//		//DestroyWindow(buttons[i][j].GetHWND());
		//	}
		//}
	
		DeleteObject(segmentedFont);
		KillTimer(hwnd, IDT_TIMER_1);
		PostQuitMessage(0);
		return 0;
	case WM_NCDESTROY:
		for (int i = 0; i < GameData::minefield_y_size; i++)
		{
			delete[] GameData::minefield[i];
		}
		delete[] GameData::minefield;
		return 0;
	default:
		return DefWindowProc(hwnd, umsg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	MessageBox(NULL, L"Hello world! \r\nHello world!", L"Caption", MB_ICONINFORMATION);
	//GameData::minefield_x_size = 10;
	//GameData::minefield_y_size = 10;
	GameData::isPlaying = true;
	HMENU hMenu;

	//
	try {
		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
		if (!hAccelTable) throw GetLastError();

		hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU2));
		if (!hMenu) throw GetLastError();
		//if (!AddFontResourceEx(L"fonts/segment7standard.otf", FR_PRIVATE, NULL)) throw;
		HRSRC res = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_FONT3), RT_FONT);
		if (!res) throw GetLastError();

		HGLOBAL mem = LoadResource(GetModuleHandle(NULL), res);
		void* data = LockResource(mem);
		size_t len = SizeofResource(GetModuleHandle(NULL), res);

		// 3. Register the font with the system (for this process only)
		DWORD nFonts = 0;
		hFontResource = AddFontMemResourceEx(data, (DWORD)len, NULL, &nFonts);

		if (hFontResource == NULL) {
			throw GetLastError();
		}
	}
	catch (DWORD errCode) {
		//próba poinformowania u¿ytkownika, ¿e coœ siê spartoli³o
		WCHAR buffer[32]{};
		_itow_s(errCode, buffer, 32, 10);

		const size_t msg_size = 100;
		WCHAR msg[msg_size] = L"There was an error, which has caused the program to quit. Error code: ";

		wcscat_s(msg, msg_size, buffer);
		MessageBox(NULL, msg, L"Error", MB_ICONERROR | MB_OK);
		ExitProcess(errCode);
	}
	//Sleep(5000);

	Window winow = Window(WindowProc, hInstance, 1, WS_OVERLAPPEDWINDOW| WS_CLIPCHILDREN, 0, L"Minesweeper",hMenu);
	winow.SetAccelTable(hAccelTable);
	winow.RunWindow();

	if (hFontResource) {
		RemoveFontMemResourceEx(hFontResource);
	}
}