#include "Button.h"

bool Button::IsFlag(Button& btn) {
    WCHAR string[10];
    UINT len = GetWindowTextLength(btn.hwnd);
    GetWindowTextW(btn.hwnd, string, len + 1);
    return !lstrcmpW(string, L"Flag");
        //GameData::flag_count--;
        //SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(L""));
}

HFONT Button::hMinesFont = CreateFont(
    24,                        // Height (adjusted for your button size)
    0,                         // Width (0 = let Windows choose)
    0, 0,                      // Angles
    FW_HEAVY,                   // Weight: This makes the numbers "pop"
    FALSE, FALSE, FALSE,       // Italic, Underline, Strikeout
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    CLEARTYPE_QUALITY,         // Quality: Makes edges smooth
    DEFAULT_PITCH | FF_SWISS,  // FF_SWISS is for sans-serif fonts
    L"Arial"                   // Typeface name
);

void Button::Uncover()
{
    WCHAR buff[10];
    _itow_s(neighbours, buff, 10, 10);
    SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(buff));
    EnableWindow(hwnd, false);
}
void Button::UncoverMines()
{
    Button** field = (Button**)GameData::minefield;
    for (int i = 0; i < GameData::minefield_y_size; i++)
    {
        for (int j = 0; j < GameData::minefield_x_size; j++)
        {
            WCHAR string[10];
            Button& button = field[i][j];

            if (button.neighbours == -1 && !IsFlag(button)) {
                EnableWindow(button.hwnd, false);
              //  Beep(5000, 100);
                InvalidateRect(button.hwnd, NULL, FALSE);
                UpdateWindow(button.hwnd);
            }
            else if (IsFlag(button) && button.neighbours != -1) {
                GameData::flag_count--;
                _itow_s(button.neighbours, string, 10, 10);
                SendMessage(button.hwnd, WM_SETTEXT, 0, (LPARAM)(string));
                InvalidateRect(button.hwnd, NULL, FALSE);
                UpdateWindow(button.hwnd);
            }
        }
    }
}

void Button::Uncover(Button& button)
{
    if (!IsWindowEnabled(button.hwnd))
        return;
    WCHAR buff[10];
    if (button.neighbours >= 0) {
        if (IsFlag(button)) {
            GameData::flag_count--;
        }
        _itow_s(button.neighbours, buff, 10, 10);
        SendMessage(button.hwnd, WM_SETTEXT, 0, (LPARAM)(buff));
    }
    else {
        if (GameData::clicks == 0) {
           // Beep(400, 500);
            //handle the case of mine under the first click
            UINT n = 0;
            for (INT i = 0; i < 8; i++) {
                if (button.neighbours_list[i] != NULL)
                    if (button.neighbours_list[i]->neighbours > 0)
                        button.neighbours_list[i]->neighbours--;
                    else
                        n++;
            }
            button.neighbours = n;
            _itow_s(button.neighbours, buff, 10, 10);
            SendMessage(button.hwnd, WM_SETTEXT, 0, (LPARAM)(buff));

            for (int i = 0; i < GameData::minefield_y_size; i++)
            {
                for ( int j = 0; j < GameData::minefield_x_size; j++)
                {
                    if (GameData::minefield[i][j].neighbours >= 0) {
                        GameData::minefield[i][j].neighbours = -1;
                        for (int m = 0; m < 8; m++)
                        {
                            if (GameData::minefield[i][j].neighbours_list[m] != NULL && GameData::minefield[i][j].neighbours_list[m]->neighbours != -1) {
                                GameData::minefield[i][j].neighbours_list[m]->neighbours++;
                            }
                        }
                        goto loopend;

                    }
                }
            }
            

        }
        else {
           // SendMessage(button.hwnd, WM_SETTEXT, 0, (LPARAM)(L"mina"));
            //GameData::mine_count--;
            Beep(5000, 100);
            EnableWindow(button.hwnd, false);
            InvalidateRect(button.hwnd, NULL, TRUE);
            UpdateWindow(button.hwnd);
            GameData::isPlaying = false;
            UncoverMines();
            GameData::state = DEAD;
            return;
            //Sleep(500);
            //HDC hdc = GetDC(NULL);
            //RECT rc = { 0,0,10000,10000 };
            //FillRect(hdc, &rc, (HBRUSH)15);
            //ReleaseDC(NULL, hdc);

            //WCHAR buffer[32]{};
            //_itow_s(GameData::minefield_x_size, buffer, 32, 10);

            //const size_t msg_size = 100;
            //WCHAR msg[msg_size] = L"There was an error, which has caused the program to quit. Error code: ";

            //wcscat_s(msg, msg_size, buffer);
            //MessageBox(NULL, msg, L"Error", MB_ICONERROR | MB_OK);

        }
    }
loopend:
    EnableWindow(button.hwnd, false);
    GameData::clicks++;
   //InvalidateRect(button.hwnd, NULL, TRUE);
   // UpdateWindow(button.hwnd);
    if (button.neighbours == 0) {
        for (INT i = 0; i < 8; i++) {
            if (button.neighbours_list[i]!=NULL)
                if(IsWindowEnabled(button.neighbours_list[i]->hwnd))
                    Uncover(*button.neighbours_list[i]);
        }
    }
}

LRESULT CALLBACK Button::ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    HDC hdc;
    RECT rc;
    PAINTSTRUCT ps;
    UINT len = 0;
    Button* btn = reinterpret_cast<Button*>(dwRefData);
    switch (uMsg)
    {
    //case WM_CREATE:

    //    return 0;
    case WM_LBUTTONUP:
    {
      //  LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);

        if (GameData::isPlaying) {
            if (IsFlag(*btn)) {
                GameData::flag_count--;
                SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)(L""));
            }
            else {
                LockWindowUpdate(GetParent(hWnd));
                Uncover(*btn);
            }
            if (GameData::clicks + GameData::mine_count == GameData::minefield_x_size * GameData::minefield_y_size && GameData::isPlaying) {
                GameData::isPlaying = false;
                GameData::state = WINNER;

               
            }
        }
        LockWindowUpdate(NULL);
        InvalidateRect(hWnd, NULL, FALSE);
        SetFocus(GetParent(hWnd));

        return 0;
    }
    case WM_LBUTTONDOWN:
        return 0;
    case WM_LBUTTONDBLCLK:
      //  MessageBox(NULL, L"", L"", 0);
        if (GameData::isPlaying) {
            Uncover(*btn);
            if ((GameData::clicks + GameData::mine_count == GameData::minefield_x_size * GameData::minefield_y_size)&& GameData::isPlaying) {
                GameData::isPlaying = false;
                GameData::state = WINNER;
            }
        }
        InvalidateRect(hWnd, NULL, FALSE);
        SetFocus(GetParent(hWnd));
        return 0;
    //case WM_ERASEBKGND:
    //{
    //    //if (IsWindowEnabled(hWnd)) {
    //        //LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    //        len = GetWindowTextLength(hWnd);

    //        GetWindowTextW(hWnd, string, len + 1);

    //       // if (!lstrcmpW(string, L"Flag")) {

    //        hdc = BeginPaint(hWnd, &ps);
    //        GetClientRect(hWnd, &rc);
    //        //Paint default
    //        FillRect(hdc, &rc, (HBRUSH)(COLOR_BTNFACE + 1));
    //        DrawEdge(hdc, &rc, EDGE_RAISED, BF_RECT);
    //          
    //        EndPaint(hWnd, &ps);
    //        return 0;
    //       // }

    //      //  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    //   // }
    //}
    case WM_SETFOCUS:
        return 0;
        break;
    case WM_MOUSEHWHEEL:
        return 0;
     
    case WM_ERASEBKGND:
        return TRUE;

    case WM_NCHITTEST:
        return HTCLIENT;
    case WM_SETTEXT:
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    //case WM_KILLFOCUS:
    //    return 0;
    case WM_PAINT:
    {
        if (IsWindowEnabled(hWnd)) {
            hdc = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rc);

            FillRect(hdc, &rc, GameData::primary_background);
            DrawEdge(hdc, &rc, EDGE_RAISED, BF_RECT);

            if (IsFlag(*btn)) {

                //Paint default
                HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // Black pole

                SelectObject(hdc, hPen);

                // Draw the pole
                MoveToEx(hdc, rc.right * 0.7, rc.top + 5, NULL);
                LineTo(hdc, rc.right * 0.7, rc.bottom - 5);

                // Draw the red flag (Triangle)
                HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
                SelectObject(hdc, hRedBrush);

                POINT flagPoints[3] = {
                    { (int)(rc.left + (rc.right-rc.left) * 0.7), (int)(rc.top + (rc.bottom - rc.top)*0.1) },
                    { (int)(rc.left + (rc.right-rc.left) * 0.3), (int)(rc.top + (rc.bottom - rc.top) * 0.25) },
                    { (int)(rc.left + (rc.right-rc.left) * 0.7), (int)(rc.top + (rc.bottom - rc.top) * 0.4) }
                };
                Polygon(hdc, flagPoints, 3);

                DeleteObject(hRedBrush);
                DeleteObject(hPen);
                
            }
            EndPaint(hWnd, &ps);
            return 0;
    }
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        //draw border
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOWFRAME + 1));

        //shift drawing rect and paint again
        rc.left += 1;
        rc.right -= 1;
        rc.top += 1;
        rc.bottom -= 1;
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
        Button* btn = reinterpret_cast<Button*>(dwRefData);
        if (btn->neighbours > 0) {
            // Set the font and transparency
            HFONT hOldFont = (HFONT)SelectObject(hdc, hMinesFont);
            SetBkMode(hdc, TRANSPARENT);

            // APPLY THE COLOR HERE
            SetTextColor(hdc, GameData::GetMinesweeperColor(btn->neighbours));

            // Convert int to string (e.g., L"1")
            WCHAR val[2] = { (WCHAR)(btn->neighbours + '0'), 0 };

            DrawTextW(hdc, val, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, hOldFont);
        }
        else if (btn->neighbours == -1) {
            HBRUSH red_brush = CreateSolidBrush(RGB(255, 0, 0));
            FillRect(hdc, &rc, red_brush);
            // Drawing a Mine
            HBRUSH hMineBrush = CreateSolidBrush(RGB(0, 0, 0));
            SelectObject(ps.hdc, hMineBrush);
            int radius = int(min(rc.right - rc.left, rc.bottom - rc.top)/2*0.9);
            Ellipse(hdc, rc.left + (rc.right- rc.left)/2 - radius,
                rc.top + (rc.bottom - rc.top) / 2 - radius,
                rc.left + (rc.right - rc.left) / 2 + radius,
                rc.top + (rc.bottom - rc.top) / 2 + radius);
            DeleteObject(hMineBrush);
            DeleteObject(red_brush);
        }
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_RBUTTONDOWN:
        if (!GameData::isPlaying)
            break;
        

        if (!IsFlag(*btn)) {
            GameData::flag_count++;

            SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)(L"Flag"));
        }
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;

    case WM_NCDESTROY:

        RemoveWindowSubclass(hWnd, ButtonSubclassProc, uIdSubclass);
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void Button::Init(HWND parent, LPCWSTR text, int x, int y, int x_size, int y_size)
{
    try {
       // neighbours_list = new Button * [8] {};
        if (hwnd)
            DestroyWindow(hwnd);

        hwnd = CreateWindow(L"BUTTON", L" ", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, x, y, x_size, y_size, parent, NULL, (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE), NULL);
        if (!hwnd) throw GetLastError();
        if (!SetWindowSubclass(hwnd, ButtonSubclassProc, 1, (DWORD_PTR)this)) throw GetLastError();
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
    this->neighbours = 0;
}

Button::Button(HWND parent, LPCWSTR text, int x, int y, int x_size, int y_size)
{
    try {
        if (hwnd)
            DestroyWindow(hwnd);
        hwnd = CreateWindow(L"BUTTON", L" ", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, x, y, x_size, y_size, parent, NULL, (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE), NULL);
        if (!hwnd) throw GetLastError();
        if(!SetWindowSubclass(hwnd, ButtonSubclassProc, 1, (DWORD_PTR)this)) throw GetLastError();
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
Button::~Button()
{
    if (hwnd)
        DestroyWindow(hwnd);
    hwnd = NULL;
   // if (neighbours_list)
   //     delete[] neighbours_list;
}
HWND Button::GetHWND()
{
	return this->hwnd;
}
