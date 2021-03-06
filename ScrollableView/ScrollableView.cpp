// 
// This is a program to scroll text vertically.
//

#include "stdafx.h"
#include "ScrollableView.h"

#define MAX_LOADSTRING 100
#define TOTAL_LINES    200
#define TEXT_LENGTH     57
#define FONT_HEIGHT     14

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

wchar_t str[TOTAL_LINES][TEXT_LENGTH];
int lines_per_page;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void OnVScroll(HWND hWnd, int iSBCode);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);
void OnSize(HWND hWnd, int width, int height);

void OnFileExit(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SCROLLABLEVIEW, szWindowClass, MAX_LOADSTRING);

	//  Registers the window class.
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCROLLABLEVIEW));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SCROLLABLEVIEW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	// Store instance handle in our global variable.
	hInst = hInstance; 

	// Create the main program window.
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) return FALSE;

	// Display the main program window.
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCROLLABLEVIEW));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//  Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_VSCROLL: OnVScroll(hWnd, LOWORD (wParam)); break;
    case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_EXIT:  OnFileExit(hWnd);							break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
        break;
	case WM_PAINT:   OnPaint(hWnd);									break;
	case WM_CREATE:  OnCreate(hWnd);								break;
	case WM_DESTROY: OnDestroy(hWnd);								break;
	case WM_SIZE:    OnSize(hWnd, LOWORD (lParam), HIWORD (lParam));break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 
void OnVScroll(HWND hWnd, int iSBCode)
{
	SCROLLINFO si;

	// Get all the vertial scroll bar information.
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;
	GetScrollInfo (hWnd, SB_VERT, &si);

	switch (iSBCode)
	{
	case SB_TOP:        si.nPos = si.nMin;      break; // User clicked the HOME keyboard key.
	case SB_BOTTOM:     si.nPos = si.nMax;      break; // User clicked the END keyboard key.
	case SB_LINEUP:     si.nPos -= 1;           break; // User clicked the top arrow.
	case SB_LINEDOWN:   si.nPos += 1;           break; // User clicked the bottom arrow.
	case SB_PAGEUP:     si.nPos -= si.nPage;    break; // User clicked the scroll bar shaft above the scroll box.
	case SB_PAGEDOWN:   si.nPos += si.nPage;    break; // User clicked the scroll bar shaft below the scroll box.
	case SB_THUMBTRACK: si.nPos = si.nTrackPos; break; // User dragged the scroll box.
	}

	// Redraw the scroll bar.
	si.fMask = SIF_POS;
	SetScrollInfo (hWnd, SB_VERT, &si, TRUE);

	InvalidateRect(hWnd, NULL, TRUE);
}

//
void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC;
	HFONT hFont;
	TEXTMETRIC tm;
	int i, k, dy, y;
	SCROLLINFO si;

	hDC = BeginPaint(hWnd, &ps);

	// Set color.
	SetTextColor(hDC, RGB(192, 192, 192)); 
	SetBkColor(hDC, RGB(0, 0, 0));

	// Create and set font.
	hFont = CreateFont(-FONT_HEIGHT, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Lucida Console");
	SelectObject(hDC,hFont);

	// Get vertical scroll bar position.
	si.cbSize = sizeof (si);
	si.fMask  = SIF_POS;
	GetScrollInfo (hWnd, SB_VERT, &si);

	// get spacing between lines
	GetTextMetrics(hDC, &tm); 
	dy = tm.tmHeight + tm.tmExternalLeading;

	y = 0;

	// Draw lines of text starting at index k and
	// a total of lines_per_page.
	k = si.nPos;
	for (i = 0; i < lines_per_page; i++)
	{
		TextOut(hDC, 0, y, str[k], wcslen(str[k]));
		++k;
		y += dy;
	}

	// Delete font.
	DeleteObject(hFont);
	EndPaint(hWnd, &ps);
}

//
void OnCreate(HWND hWnd)
{
	int i;
	unsigned char ch;

	// Create some array of lines to display. 
	ch = 0;

	for (i = 0; i < TOTAL_LINES; i++)
	{
		swprintf_s(str[i], TEXT_LENGTH, L"%08d %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
			i, ch, ch+1, ch+2, ch+3, ch+4, ch+5, ch+6, ch+7, ch+8, ch+9, ch+10, ch+11, ch+12, ch+13, ch+14, ch+15);
		ch += 16;
	}
}

//
void OnDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}

//
void OnSize(HWND hWnd, int width, int height)
{
	int quo, rem;

	quo = height / FONT_HEIGHT;
	rem = height % FONT_HEIGHT;

	lines_per_page = quo + (rem>0?1:0);

	// Set the vertical scrolling range and page size
	SCROLLINFO si;

	si.cbSize = sizeof(si); 
	si.fMask  = SIF_RANGE | SIF_PAGE; 
	si.nMin   = 0; 
	si.nMax   = TOTAL_LINES + quo - 1;
	si.nPage  = quo; 
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}

//
void OnFileExit(HWND hWnd)
{
	DestroyWindow(hWnd);
}
