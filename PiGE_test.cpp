// PiGE_test.cpp : Defines the entry point for the application.
//

//total hours spent on this project: 18

#include "pch.h"
#include "framework.h"
#include "PiGE_test.h"
#include "windowsx.h"
#include <random>
#include <fstream> 

#define MAX_LOADSTRING 100

using namespace std;

HWND g_hMainWindow;
HWND g_hMYWindow;
HWND g_hPCWindow;

const int DEFINED_EASY_SIZE = 10;
const int DEFINED_MEDIUM_SIZE = 15;
const int DEFINED_HARD_SIZE = 20;

HINSTANCE hInst;                                // current instance

WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szTitleMY[MAX_LOADSTRING];                  // The title bar MY
WCHAR szTitlePC[MAX_LOADSTRING];                  // The title bar PC

WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szWindowClassMY[MAX_LOADSTRING];            // the MY window class name
WCHAR szWindowClassPC[MAX_LOADSTRING];            // the PC window class name

ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                MyRegisterClassBoardMY(HINSTANCE hInstance);
ATOM                MyRegisterClassBoardPC(HINSTANCE hInstance);

BOOL                InitInstance(HINSTANCE, int);

LRESULT CALLBACK    MainWndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK    MYWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    PCWndProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void DrawRect(HDC hdc, const RECT* rect);
void DrawingCellsMY(int cell_num, PAINTSTRUCT ps);
void DrawingCellsPC(int cell_num, PAINTSTRUCT ps);
void PopulateCells(int cell_num);

void OnMouseClick(HWND hWnd, LPARAM lParam, int mode);

void PrepareMap();
void CleanGameArea();
void InitShips();

void InitGameMaps();

bool isValidPositionMY(int row, int col, int shipSize);
void PlaceShipMY(int shipSize);

bool isValidPositionPC(int row, int col, int shipSize);
void PlaceShipPC(int shipSize);

int WritePrivateProfileInt(LPCWSTR appname, LPCWSTR keyname, int val, LPCWSTR file);
static void WriteFirmConfig();
void CreateFile();

struct Region
{
    RECT rect; 
    COLORREF color; 
    bool isHit;
    int left;
    int top;
    int right;
    int bottom;
    bool isInUse = false;
    int isShip; // 0- empt, 1-1, 2-2, 3-3,
};

struct SeaTile
{
    int shipID=0;
    bool isBorder = true;
    bool isOccupied = false;
    bool isVisited = true;
    bool isShip; // 0- empty, 1-1 decks, 2-2 decks, 3-3 decks, 4-4 decks
    int decks;
};

struct GazeringOfShips
{
    int shipSize;
    int shipAmount;

};

struct Ship
{
    int decks;
    int* hit_decks;
};

GazeringOfShips shipsMY[4];
GazeringOfShips shipsPC[4];

int g_mode = 1;
int charSize = 30;
Region regionsMY[20][20];
Region regionsPC[20][20];

SeaTile mapMY[22][22];
SeaTile mapPC[22][22];

const int MAIN_WINDOW_WIDTH = 600;
const int MAIN_WINDOW_HEIGHT = 250;

const int BOARD_WINDOW_HEIGHT_EASY = 375;
const int BOARD_WINDOW_HEIGHT_MEDIUM = 540;
const int BOARD_WINDOW_HEIGHT_HARD = 671;
const int BOARD_WINDOW_WIDTH_EASY = 353;
const int BOARD_WINDOW_WIDTH_MEDIUM = 519;
const int BOARD_WINDOW_WIDTH_HARD = 683;

const int MAIN_WINDOW_TRANSPARENCY = 30;
const COLORREF MAIN_WINDOW_COLOR = RGB(164, 174, 196);


#define WM_RESIZE_CHILD_WINDOW_MY (WM_USER + 1)
#define WM_RESIZE_CHILD_WINDOW_PC (WM_USER + 2)

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PIGETEST, szWindowClass, MAX_LOADSTRING);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitleMY, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PIGETEST, szWindowClassMY, MAX_LOADSTRING);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitlePC, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PIGETEST, szWindowClassPC, MAX_LOADSTRING);

    MyRegisterClass(hInstance);
    MyRegisterClassBoardMY(hInstance);
    MyRegisterClassBoardPC(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PIGETEST));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BATTLE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PIGETEST);
    wcex.lpszClassName = L"MainWindowClass";;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_BATTLE));

    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    //wcex.lpszMenuName = MAKEINTRESOURCE(IDC_TUTORIAL);
    //wcex.lpszClassName = szWindowClass;
    return RegisterClassExW(&wcex);
}


ATOM MyRegisterClassBoardMY(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MYWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PIGETEST));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"MYWindowsClass";;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    //wcex.lpszMenuName = MAKEINTRESOURCE(IDC_TUTORIAL);
    //wcex.lpszClassName = szWindowClassMY;
    return RegisterClassExW(&wcex);
}

ATOM MyRegisterClassBoardPC(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = PCWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PIGETEST));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"PCWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    //wcex.lpszMenuName = MAKEINTRESOURCE(IDC_TUTORIAL);
    //wcex.lpszClassName = szWindowClassPC;
    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    //game initiazation
    g_mode = 1;

    PopulateCells(3);

    InitGameMaps();

    int retValue = WritePrivateProfileInt(L"BATTLESHIPS", L"DIFFICULTY", g_mode, L"C:\\Users\\darge\\source\\repos\\PiGE_test\\PiGE_test\\Wordle.ini");
    DWORD reError = GetLastError();

    //game windows initialization
    g_hMainWindow = CreateWindowW(L"MainWindowClass", L"BATTLESHIPS:", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    g_hMYWindow = CreateWindowW(L"MYWindowsClass", L"BATTLESHIPS-MY", WS_OVERLAPPED | WS_CAPTION,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, g_hMainWindow, nullptr, hInstance, nullptr);

    g_hPCWindow = CreateWindowW(L"PCWindowClass", L"BATTLESHIPS-PC", WS_OVERLAPPED | WS_CAPTION,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, g_hMainWindow, nullptr, hInstance, nullptr);

    ////https://stackoverflow.com/questions/42905649/cant-center-my-console-window-by-using-the-following-code
    SetWindowPos(g_hMainWindow, NULL,
        GetSystemMetrics(SM_CXSCREEN) / 2 - MAIN_WINDOW_WIDTH / 2,
        GetSystemMetrics(SM_CYSCREEN) / 2 - MAIN_WINDOW_HEIGHT / 30,
        MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, SWP_SHOWWINDOW);

    SetWindowPos(g_hMYWindow, NULL,
        GetSystemMetrics(SM_CXSCREEN) / 2 - BOARD_WINDOW_WIDTH_EASY / 20,
        GetSystemMetrics(SM_CYSCREEN) / 2 - BOARD_WINDOW_HEIGHT_EASY / 3,
        BOARD_WINDOW_WIDTH_EASY, BOARD_WINDOW_HEIGHT_EASY, SWP_SHOWWINDOW);

    SetWindowPos(g_hPCWindow, NULL,
        GetSystemMetrics(SM_CXSCREEN) / 2 - BOARD_WINDOW_WIDTH_EASY * 1.5,
        GetSystemMetrics(SM_CYSCREEN) / 2 - BOARD_WINDOW_HEIGHT_EASY / 3,
        BOARD_WINDOW_WIDTH_EASY, BOARD_WINDOW_HEIGHT_EASY, SWP_SHOWWINDOW);

    //TRANSPARANCY
    SetWindowLong(g_hMainWindow, GWL_EXSTYLE, GetWindowLong(g_hMainWindow, GWL_EXSTYLE) | WS_EX_LAYERED);

    SetLayeredWindowAttributes(g_hMainWindow, 0, (255 * 70) / 100, LWA_ALPHA);

    if (!g_hMainWindow)
    {
        return FALSE;
    }

    ShowWindow(g_hMainWindow, nCmdShow);
    UpdateWindow(g_hMainWindow);

    if (!g_hMYWindow)
    {
        return FALSE;
    }

    ShowWindow(g_hMYWindow, nCmdShow);
    UpdateWindow(g_hMYWindow);

    if (!g_hPCWindow)
    {
        return FALSE;
    }

    ShowWindow(g_hPCWindow, nCmdShow);
    UpdateWindow(g_hPCWindow);

    return TRUE;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    HDC hdc;
    static int t = 0;
    UINT_PTR g_nTimerID = 0;
    switch (message)
    {
    case WM_CREATE:
    {
        g_nTimerID = SetTimer(hWnd, 1, 100, NULL);
        break;
    }
    case WM_TIMER:
    {
        //https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/sprintf-s-sprintf-s-l-swprintf-s-swprintf-s-l?view=msvc-170&devlangs=cpp&f1url=%3FappId%3DDev17IDEF1%26l%3DEN-US%26k%3Dk(CORECRT_WSTDIO%252Fswprintf_s)%3Bk(swprintf_s)%3Bk(DevLang-C%252B%252B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
        wchar_t s[256];
        t = t + 1;
        swprintf_s(s, 256, L"BATTLESHIPS: %d", t);
        SetWindowText(hWnd, s);

        break;
    }
    //https://learn.microsoft.com/pl-pl/windows/win32/learnwin32/closing-the-window
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
        //https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-syskeydown?redirectedfrom=MSDN&devlangs=cpp&f1url=%3FappId%3DDev17IDEF1%26l%3DEN-US%26k%3Dk(WINUSER%252FWM_SYSKEYDOWN)%3Bk(WM_SYSKEYDOWN)%3Bk(DevLang-C%252B%252B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
    case WM_SYSKEYDOWN:
        if (wParam == VK_SPACE)
        {
            POINT pt;
            GetCursorPos(&pt);
            TrackPopupMenu(GetSystemMenu(hWnd, FALSE), TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
        }
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        //https://learn.microsoft.com/en-us/windows/win32/winmsg/using-timers

        switch (wmId)
        {
        case ID_FILE_EASY:
            g_mode = 1;
            SendMessage(g_hMYWindow, WM_RESIZE_CHILD_WINDOW_MY, MAKEWPARAM(0, 1), MAKELPARAM(BOARD_WINDOW_WIDTH_MEDIUM, BOARD_WINDOW_HEIGHT_MEDIUM));
            SendMessage(g_hPCWindow, WM_RESIZE_CHILD_WINDOW_PC, MAKEWPARAM(0, 1), MAKELPARAM(BOARD_WINDOW_WIDTH_MEDIUM, BOARD_WINDOW_HEIGHT_MEDIUM));
            PopulateCells(g_mode);
            InitGameMaps();
            break;
        case ID_FILE_MEDIUM:
            g_mode = 2;
            SendMessage(g_hMYWindow, WM_RESIZE_CHILD_WINDOW_MY, MAKEWPARAM(0, 2), MAKELPARAM(BOARD_WINDOW_WIDTH_MEDIUM, BOARD_WINDOW_HEIGHT_MEDIUM));
            SendMessage(g_hPCWindow, WM_RESIZE_CHILD_WINDOW_PC, MAKEWPARAM(0, 2), MAKELPARAM(BOARD_WINDOW_WIDTH_MEDIUM, BOARD_WINDOW_HEIGHT_MEDIUM));
            PopulateCells(g_mode);
            InitGameMaps();
            break;
        case ID_FILE_HARD:
            g_mode = 3;
            SendMessage(g_hMYWindow, WM_RESIZE_CHILD_WINDOW_MY, MAKEWPARAM(0, 3), MAKELPARAM(BOARD_WINDOW_WIDTH_MEDIUM, BOARD_WINDOW_HEIGHT_MEDIUM));
            SendMessage(g_hPCWindow, WM_RESIZE_CHILD_WINDOW_PC, MAKEWPARAM(0, 3), MAKELPARAM(BOARD_WINDOW_WIDTH_MEDIUM, BOARD_WINDOW_HEIGHT_MEDIUM));
            PopulateCells(g_mode);
            InitGameMaps();
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK MYWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    PAINTSTRUCT ps;
    int offset = 3;
    int mergin = 5;
    //https://learn.microsoft.com/en-us/windows/win32/gdi/using-filled-shapes
    HDC hdc;

    switch (message)
    {
    case WM_CREATE:
    {

    }
    break;
    case WM_RESIZE_CHILD_WINDOW_MY:
    {
        g_mode = (int)HIWORD(wParam);
        int winWidth = (int)HIWORD(lParam);
        int winHeight = (int)LOWORD(lParam);
        if (g_mode == 1)
        {
            SetWindowPos(hWnd, NULL,
                GetSystemMetrics(SM_CXSCREEN) / 2 - BOARD_WINDOW_WIDTH_EASY / 20,
                GetSystemMetrics(SM_CYSCREEN) / 2 - BOARD_WINDOW_HEIGHT_EASY / 3,
                BOARD_WINDOW_WIDTH_EASY, BOARD_WINDOW_HEIGHT_EASY, SWP_SHOWWINDOW);
        }
        else if (g_mode == 2)
        {
            SetWindowPos(hWnd, NULL,
                GetSystemMetrics(SM_CXSCREEN) / 2 - BOARD_WINDOW_WIDTH_MEDIUM / 20,
                GetSystemMetrics(SM_CYSCREEN) / 2 - BOARD_WINDOW_HEIGHT_MEDIUM / 3,
                BOARD_WINDOW_WIDTH_MEDIUM, BOARD_WINDOW_HEIGHT_MEDIUM, SWP_SHOWWINDOW);
        }
        else
        {
            SetWindowPos(hWnd, NULL,
                GetSystemMetrics(SM_CXSCREEN) / 1.93 - BOARD_WINDOW_WIDTH_HARD / 23,
                GetSystemMetrics(SM_CYSCREEN) / 2 - BOARD_WINDOW_HEIGHT_HARD / 2,
                BOARD_WINDOW_WIDTH_HARD, BOARD_WINDOW_HEIGHT_HARD, SWP_SHOWWINDOW);
        }
    }
    break;
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH hBrush = CreateSolidBrush(RGB(164, 174, 196));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        if (g_mode == 1)
        {
            DrawingCellsMY(10, ps);
        }
        else if (g_mode == 2)
        {
            DrawingCellsMY(15, ps);
        }
        else
        {
            DrawingCellsMY(20, ps);
        }
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_MOUSEMOVE:
    {
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
    }

}


LRESULT CALLBACK PCWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
    }
    break;
    case WM_LBUTTONUP:
    {
        OnMouseClick(hWnd, lParam, g_mode);
    }
    break;
    case WM_RESIZE_CHILD_WINDOW_PC:
    {
        g_mode = (int)HIWORD(wParam);
        int winWidth = (int)HIWORD(lParam);
        int winHeight = (int)LOWORD(lParam);
        if (g_mode == 1)
        {
            SetWindowPos(hWnd, NULL,
                GetSystemMetrics(SM_CXSCREEN) / 2 - BOARD_WINDOW_WIDTH_EASY * 1.5,
                GetSystemMetrics(SM_CYSCREEN) / 2 - BOARD_WINDOW_HEIGHT_EASY / 3,
                BOARD_WINDOW_WIDTH_EASY, BOARD_WINDOW_HEIGHT_EASY, SWP_SHOWWINDOW);
        }
        else if (g_mode == 2)
        {
            SetWindowPos(hWnd, NULL,
                GetSystemMetrics(SM_CXSCREEN) / 2 - BOARD_WINDOW_WIDTH_MEDIUM * 1.25,
                GetSystemMetrics(SM_CYSCREEN) / 2 - BOARD_WINDOW_HEIGHT_MEDIUM / 3,
                BOARD_WINDOW_WIDTH_MEDIUM, BOARD_WINDOW_HEIGHT_MEDIUM, SWP_SHOWWINDOW);
        }
        else
        {
            SetWindowPos(hWnd, NULL,
                GetSystemMetrics(SM_CXSCREEN) / 2 - BOARD_WINDOW_WIDTH_HARD,
                GetSystemMetrics(SM_CYSCREEN) / 2 - BOARD_WINDOW_HEIGHT_HARD / 2,
                BOARD_WINDOW_WIDTH_HARD, BOARD_WINDOW_HEIGHT_HARD, SWP_SHOWWINDOW);
        }
    }

    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        int offset = 3;
        int mergin = 5;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH hBrush = CreateSolidBrush(RGB(164, 174, 196));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        if (g_mode == 1)
        {
            DrawingCellsPC(10, ps);
        }
        else if (g_mode == 2)
        {
            DrawingCellsPC(15, ps);
        }
        else
        {
            DrawingCellsPC(20, ps);
        }
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void DrawingCellsMY(int cell_num, PAINTSTRUCT ps)
{
    HBRUSH hBrush = CreateSolidBrush(RGB(164, 174, 196));

    for (int i = 0; i < cell_num; i++)
    {
        for (int j = 0; j < cell_num; j++)
        {
            if (regionsMY[i][j].isHit)
            {
                
                hBrush = CreateSolidBrush(regionsMY[i][j].color);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(ps.hdc, hBrush);

            }
            //if (mapMY[i + 1][j + 1].isOccupied)
            //{
            //    hBrush = CreateSolidBrush(RGB(164, 174, 100));
            //    HBRUSH hOldBrush = (HBRUSH)SelectObject(ps.hdc, hBrush);
            //}
            //else {
            //    hBrush = CreateSolidBrush(RGB(164, 174, 196));
            //    HBRUSH hOldBrush = (HBRUSH)SelectObject(ps.hdc, hBrush);
            //}
            RoundRect(ps.hdc, regionsMY[i][j].left, regionsMY[i][j].top, regionsMY[i][j].right, regionsMY[i][j].bottom, charSize / 2, charSize / 2);

            if (mapMY[i + 1][j + 1].isShip)
            {
                SetTextColor(ps.hdc, BLACK_BRUSH);
                SetBkMode(ps.hdc, TRANSPARENT);

                RECT rect1;
                rect1.left = regionsMY[i][j].left;
                rect1.top = regionsMY[i][j].top;
                rect1.bottom = regionsMY[i][j].bottom;
                rect1.right = regionsMY[i][j].right;
                int d = mapMY[i + 1][j + 1].decks;
                wchar_t s[256];
                swprintf_s(s, 256, L"%d", d);
                DrawText(ps.hdc, s, -1, &rect1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
        }
    }
}

void DrawingCellsPC(int cell_num, PAINTSTRUCT ps)
{
    HBRUSH hBrush = CreateSolidBrush(RGB(164, 174, 196));

    for (int i = 0; i < cell_num; i++)
    {
        for (int j = 0; j < cell_num; j++)
        {

            if (regionsPC[i][j].isHit)
            {
                if (mapPC[i + 1][j + 1].isShip && regionsPC[i][j].isHit)
                {
                    hBrush = CreateSolidBrush(regionsPC[i][j].color);
                    HBRUSH hOldBrush = (HBRUSH)SelectObject(ps.hdc, hBrush);
                }
                else if(!mapPC[i + 1][j + 1].isShip && regionsPC[i][j].isHit)
                {//reset color
                    hBrush = CreateSolidBrush(RGB(0, 0, 255));
                    HBRUSH hOldBrush = (HBRUSH)SelectObject(ps.hdc, hBrush);
                }
            }
            else {//reset color
                hBrush = CreateSolidBrush(RGB(164, 174, 196));
                HBRUSH hOldBrush = (HBRUSH)SelectObject(ps.hdc, hBrush);
            }
            RoundRect(ps.hdc, regionsPC[i][j].left, regionsPC[i][j].top, regionsPC[i][j].right, regionsPC[i][j].bottom, charSize / 2, charSize / 2);
            if (mapPC[i + 1][j + 1].isShip && regionsPC[i][j].isHit)
            {
                SetTextColor(ps.hdc, BLACK_BRUSH);
                SetBkMode(ps.hdc, TRANSPARENT);
                RECT rect1;
                rect1.left = regionsMY[i][j].left;
                rect1.top = regionsMY[i][j].top;
                rect1.bottom = regionsMY[i][j].bottom;
                rect1.right = regionsMY[i][j].right;
                DrawText(ps.hdc, L"X", -1, &rect1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            else if (!mapPC[i + 1][j + 1].isShip && regionsPC[i][j].isHit)
            {
                SetTextColor(ps.hdc, BLACK_BRUSH);
                SetBkMode(ps.hdc, TRANSPARENT);
                RECT rect1;
                rect1.left = regionsMY[i][j].left;
                rect1.top = regionsMY[i][j].top;
                rect1.bottom = regionsMY[i][j].bottom;
                rect1.right = regionsMY[i][j].right;
   
                DrawText(ps.hdc, L".", -1, &rect1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
        }
    }
}

void OnMouseClick(HWND hWnd, LPARAM lParam, int mode) {
    POINT pt;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    pt.x = LOWORD(lParam);  
    pt.y = HIWORD(lParam);  

    int def;
    if (mode == 1) {
        def = 10;
    }
    else if (mode == 2) {
        def = 15;
    }
    else {
        def = 20;
    }
    for (int i = 0; i < def; ++i)
    {
        for (int j = 0; j < def; ++j)
        {
            RECT rec = { regionsPC[i][j].left, regionsPC[i][j].top, regionsPC[i][j].right, regionsPC[i][j].bottom };
            regionsPC[i][j].rect = rec;
            if (PtInRect(&regionsPC[i][j].rect, pt))
            {
                if (!regionsPC[i][j].isHit) {
                    regionsPC[i][j].isHit = true;
                    regionsPC[i][j].color = RGB(255, 0, 0); 
                    InvalidateRect(hWnd, &regionsPC[i][j].rect, TRUE);

                }
                return;
            }
        }
    }
}

void PopulateCells(int g_mode)
{
    int offset = 3;
    int mergin = 5;
    int cell_num = 10;
    if (g_mode == 1)
    {
        cell_num = 10;
    }
    else if (g_mode == 2)
    {
        cell_num = 15;
    }
    else if (g_mode == 3)
    {
        cell_num = 20;
    }
    for (int i = 0; i < cell_num; i++)
    {
        for (int j = 0; j < cell_num; j++)
        {
            int left = mergin + j * (charSize + offset);
            int top = mergin + i * (charSize + offset);
            int right = left + charSize;
            int bottom = top + charSize;

            Region currRegion = {};
            currRegion.left = left;
            currRegion.right = right;
            currRegion.top = top;
            currRegion.bottom = bottom;
            currRegion.isInUse = true;
            regionsMY[i][j] = currRegion;
            regionsPC[i][j] = currRegion;
        }
    }
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
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
//https://stackoverflow.com/questions/7501344/how-to-validate-battleship-field
void CleanGameArea()
{
    int cell_num = 12;
    if (g_mode == 1)
    {
        cell_num = 12;
    }
    else if (g_mode == 2)
    {
        cell_num = 17;
    }
    else if (g_mode == 3)
    {
        cell_num = 22;
    }
    for (int i = 1; i < cell_num - 1; i++)
    {
        for (int j = 1; j < cell_num - 1; j++)
        {
            //mapMY
            mapMY[i][j].isBorder = false;
            mapMY[i][j].isVisited = false;
            mapMY[i][j].isShip = false;
            mapMY[i][j].isOccupied = false;
            mapMY[i][j].decks = 0;

            //mapPC
            mapMY[i][j].isBorder = false;
            mapPC[i][j].isVisited = false;
            mapPC[i][j].isShip = false;
            mapPC[i][j].isOccupied = false;
            mapPC[i][j].decks = 0;
        }
    }
}

void PrepareMap()
{
    for (int i = 0; i < 22; i++)
    {
        for (int j = 0; j < 22; j++)
        {
            //mapMY
            mapMY[i][j].isVisited = true;
            mapMY[i][j].isShip = false;
            mapMY[i][j].isOccupied = false;
            mapMY[i][j].decks = 0;
            mapMY[i][j].isBorder = true;
            //mapPC
            mapPC[i][j].isVisited = true;
            mapPC[i][j].isShip = false;
            mapPC[i][j].isOccupied = false;
            mapPC[i][j].decks = 0;
            mapMY[i][j].isBorder = true;

        }
    }
}
void InitShips()
{
    Ship ship;
    ship.decks = 4;
    ship.hit_decks = new int[ship.decks];
    //4 decks
    shipsMY[0].shipAmount = 1;
    shipsMY[0].shipSize = 4;

    //3 decks
    shipsMY[1].shipAmount = 2;
    shipsMY[1].shipSize = 3;

    //2 decks
    shipsMY[2].shipAmount = 3;
    shipsMY[2].shipSize = 2;

    //1 decks
    shipsMY[3].shipAmount = 4;
    shipsMY[3].shipSize = 1;

    shipsPC[0].shipAmount = 4;
    shipsPC[1].shipAmount = 3;
    shipsPC[2].shipAmount = 2;
    shipsPC[3].shipAmount = 1;
    shipsPC[0].shipSize = 1;
    shipsPC[1].shipSize = 2;
    shipsPC[2].shipSize = 3;
    shipsPC[3].shipSize = 4;

}

void InitGameMaps()
{
    CleanGameArea();

    InitShips();

    //Place my ships
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < shipsMY[i].shipAmount; j++)
        {
            PlaceShipMY(shipsMY[i].shipSize);

        }
    }

    //Place pc ships
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < shipsPC[i].shipAmount; j++)
        {
            PlaceShipPC(shipsPC[i].shipSize);

        }
    }
}

bool isValidPositionMY(int row, int col, int shipSize)
{
    int cell_num = 10;
    if (g_mode == 1)
    {
        cell_num = 10;
    }
    else if (g_mode == 2)
    {
        cell_num = 15;
    }
    else if (g_mode == 3)
    {
        cell_num = 20;
    }
    //if ( col + 1 > cell_num) 
    //{
    //    return false; // Ship exceeds board boundaries horizontally
    //}
    if (row + shipSize > cell_num) {
        return false; // Ship exceeds board boundaries vertically
    }

    for (int i = 0; i < shipSize; ++i) {
        //if (mapMY[row][col + i].isVisited) {
        //    return false; // Ship overlaps with another ship horizontally
        //}
        if (mapMY[row + i][col].isOccupied) {
            return false; // Ship overlaps with another ship vertically
        }
    }

    //for (int i = 0; i < shipSize; ++i) {
    //    if (mapPC[row][col + i].isVisited) {
    //        return false; // Ship overlaps with another ship horizontally
    //    }
    //    else if (mapPC[row + i][col].isVisited) {
    //        return false; // Ship overlaps with another ship vertically
    //    }
    //}

    return true;
}


bool isValidPositionPC(int row, int col, int shipSize)
{
    int cell_num = 10;
    if (g_mode == 1)
    {
        cell_num = 10;
    }
    else if (g_mode == 2)
    {
        cell_num = 15;
    }
    else if (g_mode == 3)
    {
        cell_num = 20;
    }
    //if ( col + 1 > cell_num) 
    //{
    //    return false; 
    //}
    if (row + shipSize > cell_num) {
        return false;

        for (int i = 0; i < shipSize; ++i) {
            //if (mapMY[row][col + i].isVisited) {
            //    return false;
            //}
            if (mapPC[row + i][col].isOccupied) {
                return false;
            }
        }

        //for (int i = 0; i < shipSize; ++i) {
        //    if (mapPC[row][col + i].isVisited) {
        //        return false; 
        //    }
        //    else if (mapPC[row + i][col].isVisited) {
        //        return false; 
        //    }
        //}

        return true;
    }
}
void PlaceShipMY(int shipSize)
{
    int cell_num = 10;
    if (g_mode == 1)
    {
        cell_num = 10;
    }
    else if (g_mode == 2)
    {
        cell_num = 15;
    }
    else if (g_mode == 3)
    {
        cell_num = 20;
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, cell_num);

    while (true) {
        int row = dis(gen);
        int col = dis(gen);

        if (isValidPositionMY(row, col, shipSize))
        {
            if (!mapMY[row - 1][col - 1].isBorder) mapMY[row - 1][col - 1].isOccupied = true;
            if (!mapMY[row - 1][col].isBorder) mapMY[row - 1][col].isOccupied = true;
            if (!mapMY[row - 1][col + 1].isBorder) mapMY[row - 1][col + 1].isOccupied = true;


            for (int i = 0; i < shipSize; ++i)
            {
                mapMY[row + i][col].isOccupied = true;
                mapMY[row + i][col].isShip = true;
                mapMY[row + i][col].decks = shipSize;

                mapMY[row + i][col - 1].isOccupied = true;
                mapMY[row + i][col + 1].isOccupied = true;
            }

            if (!mapMY[row + shipSize][col - 1].isBorder) {
                mapMY[row + shipSize][col - 1].isOccupied = true;
            }
            if (!mapMY[row + shipSize][col].isBorder) {
                mapMY[row + shipSize][col].isOccupied = true;
            }
            if (!mapMY[row + shipSize][col + 1].isBorder) {
                mapMY[row + shipSize][col + 1].isOccupied = true;
            }

            break;
        }
    }
}
void PlaceShipPC(int shipSize)
{
    int cell_num = 10;
    if (g_mode == 1)
    {
        cell_num = 10;
    }
    else if (g_mode == 2)
    {
        cell_num = 15;
    }
    else if (g_mode == 3)
    {
        cell_num = 20;
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, cell_num);

    while (true) {
        int row = dis(gen);
        int col = dis(gen);

        if (isValidPositionPC(row, col, shipSize))
        {
            if (!mapPC[row - 1][col - 1].isBorder) mapPC[row - 1][col - 1].isOccupied = true;
            if (!mapPC[row - 1][col].isBorder) mapPC[row - 1][col].isOccupied = true;
            if (!mapPC[row - 1][col + 1].isBorder) mapPC[row - 1][col + 1].isOccupied = true;


            for (int i = 0; i < shipSize; ++i)
            {
                mapPC[row + i][col].isOccupied = true;
                mapPC[row + i][col].isShip = true;
                mapPC[row + i][col].decks = shipSize;

                mapPC[row + i][col - 1].isOccupied = true;
                mapPC[row + i][col + 1].isOccupied = true;
            }

            if (!mapPC[row + shipSize][col - 1].isBorder) {
                mapPC[row + shipSize][col - 1].isOccupied = true;
            }
            if (!mapPC[row + shipSize][col].isBorder) {
                mapPC[row + shipSize][col].isOccupied = true;
            }
            if (!mapPC[row + shipSize][col + 1].isBorder) {
                mapPC[row + shipSize][col + 1].isOccupied = true;
            }

            break;
        }
    }

}

int WritePrivateProfileInt(LPCWSTR appname, LPCWSTR keyname, int val, LPCWSTR file)
{
    char temp[256] = {};
    sprintf_s(temp, "%d", val);
    return WritePrivateProfileString(appname, keyname, (LPCWSTR)temp, file);
    
}
