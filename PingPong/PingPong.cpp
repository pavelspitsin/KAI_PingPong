// PingPong.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PingPong.h"
#include "DrawingInfo.h"
#include "ScoreInfo.h"
#include "Platform.h"
#include "Ball.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			hInst;                                // current instance
WCHAR				szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR				szWindowClass[MAX_LOADSTRING];            // the main window class name

bool				is_running;
bool				is_game_running;
DrawingInfo*		drawingInfo;
ScoreInfo*			scoreInfo;

Platform*			leftPlatform;
Platform*			rightPlatform;
Ball*				ball;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void				Update(DWORD deltaTime);
void				Draw();
void				Clear();

void				DrawBorders();
void				DrawCenterLine();
void				DrawTitleText();
void				DrawScoreText();

void				KeyUp(WPARAM wParam, LPARAM lParam);
void				KeyDown(WPARAM wParam, LPARAM lParam);

void				Start();
void				Stop();

void				RespawnBall();

void				ProcessMessages(WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW		(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW		(hInstance, IDC_PINGPONG, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass	(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	MSG msg;

	const int		FRAMES_PER_SECOND = 25;
	const int		SKIP_TICKS = 1000 / FRAMES_PER_SECOND;
	DWORD			next_game_tick = GetTickCount();

	DWORD			deltaTime;
	DWORD			lastTime;
	DWORD			currentTime = GetTickCount();

	int				sleep_time = 0;


	is_running = true;
	is_game_running = false;

	srand(time(NULL));
	
	// Main loop:
	while (is_running)
	{
		// Messages hande
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		lastTime = currentTime;
		currentTime = GetTickCount();

		next_game_tick += SKIP_TICKS;

		deltaTime = currentTime - lastTime;

		if (is_game_running)
		{
			Update(deltaTime);
		}

		Draw();

		sleep_time = next_game_tick - GetTickCount();

		if (sleep_time > 0)
		{
			Sleep(sleep_time);
		}
	}

	delete leftPlatform;
	delete rightPlatform;
	delete ball;

	delete drawingInfo;
	delete scoreInfo;

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PINGPONG));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PINGPONG);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
			drawingInfo		= new DrawingInfo(hWnd);
			scoreInfo		= new ScoreInfo();

			// Находим центр области рисования
			RECT rect;
			GetClientRect(hWnd, &rect);


			// Высчитываем размеры области рисования
			drawingInfo->WidthArea = rect.right - rect.left;
			drawingInfo->HeightArea = rect.bottom - rect.top;


			// Высчитываем центр области рисования
			drawingInfo->CenterX = drawingInfo->WidthArea / 2;
			drawingInfo->CenterY = drawingInfo->HeightArea / 2;


			// Получаем размер монитора
			drawingInfo->ScreenMaxX = GetSystemMetrics(SM_CXSCREEN);
			drawingInfo->ScreenMaxY = GetSystemMetrics(SM_CYSCREEN);
			

			drawingInfo->MarginArea = 10;


			// Получаем контекст устройства 
			HDC hdc = GetDC(hWnd);
			drawingInfo->hMemDC = CreateCompatibleDC(hdc);
			HBITMAP hBitMap = CreateCompatibleBitmap(hdc, drawingInfo->ScreenMaxX, drawingInfo->ScreenMaxY);
			SelectObject(drawingInfo->hMemDC, hBitMap);


			// Создаем кисти, перья, шрифты
			drawingInfo->hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
			drawingInfo->hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
			drawingInfo->hPenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			drawingInfo->hPenDash = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
			drawingInfo->hScoreFont = CreateFont(
					40,						// Устанавливает высоту шрифта или символа
					0,						// Устанавливает среднюю ширину символов в шрифте
					0,						// Устанавливает угол, между вектором наклона и осью X устройства
					0,						// Устанавливает угол, между основной линией каждого символа и осью X устройства
					100,					// Устанавливает толщину шрифта в диапазоне от 0 до 1000
					0,						// Устанавливает курсивный шрифт
					0,						// Устанавливает подчеркнутый шрифт
					0,						// Устанавливает зачеркнутый шрифт
					RUSSIAN_CHARSET,		// Устанавливает набор символов
					0,						// Устанавливает точность вывода
					0,						// Устанавливает точность отсечения
					0,						// Устанавливает качество вывода
					0,						// Устанавливает ширину символов и семейство шрифта
					L"Comic Sans MS"		// устанавливает название шрифта
			);

			drawingInfo->hLogoFont = CreateFont(
					80,						// Устанавливает высоту шрифта или символа
					0,						// Устанавливает среднюю ширину символов в шрифте
					0,						// Устанавливает угол, между вектором наклона и осью X устройства
					0,						// Устанавливает угол, между основной линией каждого символа и осью X устройства
					100,					// Устанавливает толщину шрифта в диапазоне от 0 до 1000
					0,						// Устанавливает курсивный шрифт
					0,						// Устанавливает подчеркнутый шрифт
					0,						// Устанавливает зачеркнутый шрифт
					RUSSIAN_CHARSET,		// Устанавливает набор символов
					0,						// Устанавливает точность вывода
					0,						// Устанавливает точность отсечения
					0,						// Устанавливает качество вывода
					0,						// Устанавливает ширину символов и семейство шрифта
					L"Comic Sans MS"		// устанавливает название шрифта
			);

			// Создаем левую платформу
			leftPlatform = new Platform(drawingInfo);
			leftPlatform->width = 20;
			leftPlatform->height = drawingInfo->HeightArea * 0.4;

			leftPlatform->startX = leftPlatform->x = 10 + drawingInfo->MarginArea;
			leftPlatform->startY = leftPlatform->y = drawingInfo->CenterY - leftPlatform->height / 2;

			// Создаем правую платформу
			rightPlatform = new Platform(drawingInfo);
			rightPlatform->width = 20;
			rightPlatform->height = drawingInfo->HeightArea * 0.4;
			
			rightPlatform->startX = rightPlatform->x = drawingInfo->WidthArea - (10 + rightPlatform->width + drawingInfo->MarginArea);
			rightPlatform->startY = rightPlatform->y = drawingInfo->CenterY - rightPlatform->height / 2;


			// Создаем мяч
			ball = new Ball(drawingInfo, leftPlatform, rightPlatform);
			ball->size = 20;

			// Удаляем более ненужный контекст устройства
			ReleaseDC(hWnd, hdc);
		}
		break;
	case WM_GAME:
		{
			ProcessMessages(wParam, lParam);
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_GAME_START:
				Start();
				break;
			case ID_GAME_STOP:
				Stop();
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_KEYDOWN:
		KeyDown(wParam, lParam);
		break;
	case WM_KEYUP:
		KeyUp(wParam, lParam);
		break;
	case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 800;
			lpMMI->ptMinTrackSize.y = 400;
			lpMMI->ptMaxTrackSize.x = 800;
			lpMMI->ptMaxTrackSize.y = 400;
		}
		break;
    case WM_PAINT:
        {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			BitBlt(hdc, 0, 0, drawingInfo->ScreenMaxX, drawingInfo->ScreenMaxY, drawingInfo->hMemDC, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		{
			PostQuitMessage(0);
			is_running = false;
		}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void Update(DWORD deltaTime)
{
	leftPlatform->Update(deltaTime);
	rightPlatform->Update(deltaTime);
	ball->Update(deltaTime);
}


void Draw()
{
	Clear();

	DrawScoreText();
	DrawTitleText();
	DrawBorders();
	DrawCenterLine();

	leftPlatform->Draw();
	rightPlatform->Draw();
	ball->Draw();

	InvalidateRect(drawingInfo->hWnd, NULL, FALSE);
}

void Clear()
{
	SelectObject(drawingInfo->hMemDC, GetStockObject(DC_BRUSH));
	PatBlt(drawingInfo->hMemDC, 0, 0, drawingInfo->ScreenMaxX, drawingInfo->ScreenMaxY, PATCOPY);
}

void DrawBorders()
{
	int margin = drawingInfo->MarginArea;
	int x1, y1, x2, y2;

	// Левая грань
	x1 = margin; x2 = margin;
	y1 = margin; y2 = drawingInfo->HeightArea - margin;	

	MoveToEx(drawingInfo->hMemDC, x1, y1, NULL);
	LineTo(drawingInfo->hMemDC, x2, y2);

	// Нижняя грань
	x1 = x2; x2 = drawingInfo->WidthArea - margin;
	y1 = y2; y2 = drawingInfo->HeightArea - margin;	

	MoveToEx(drawingInfo->hMemDC, x1, y1, NULL);
	LineTo(drawingInfo->hMemDC, x2, y2);

	// Правая грань
	x1 = x2; x2 = drawingInfo->WidthArea - margin;
	y1 = y2; y2 = margin;	

	MoveToEx(drawingInfo->hMemDC, x1, y1, NULL);
	LineTo(drawingInfo->hMemDC, x2, y2);

	// Верзняя грань
	x1 = x2; x2 = margin;
	y1 = y2; y2 = margin;	

	MoveToEx(drawingInfo->hMemDC, x1, y1, NULL);
	LineTo(drawingInfo->hMemDC, x2, y2);
}

void DrawCenterLine()
{
	if (!is_game_running)
		return;

	int margin = drawingInfo->MarginArea;
	HGDIOBJ prevPen = SelectObject(drawingInfo->hMemDC, drawingInfo->hPenDash);

	MoveToEx(drawingInfo->hMemDC, drawingInfo->CenterX, margin, NULL);
	LineTo(drawingInfo->hMemDC, drawingInfo->CenterX, drawingInfo->HeightArea - margin);

	SelectObject(drawingInfo->hMemDC, prevPen);
}

void DrawTitleText()
{
	if (is_game_running)
		return;

	UINT prevAlignText = SetTextAlign(drawingInfo->hMemDC, VTA_CENTER);
	HGDIOBJ prevFont = SelectObject(drawingInfo->hMemDC, drawingInfo->hLogoFont);
	
	int x = drawingInfo->CenterX;
	int y = drawingInfo->CenterY - 50;

	TCHAR text[256];
	swprintf_s(text, 256, L"Ping Pong");
	TextOut(drawingInfo->hMemDC, x, y, text, wcslen(text));

	SetTextAlign(drawingInfo->hMemDC, prevAlignText);
	SelectObject(drawingInfo->hMemDC, prevFont);
}

void DrawScoreText()
{
	if (!is_game_running)
		return;

	TCHAR text[256];
	int margin = drawingInfo->MarginArea;

	UINT prevAlignText = SetTextAlign(drawingInfo->hMemDC, VTA_CENTER);
	HGDIOBJ prevFont = SelectObject(drawingInfo->hMemDC, drawingInfo->hScoreFont);
	
	int x = drawingInfo->CenterX - 50;
	int y = margin + 5;

	swprintf_s(text, 256, L"%d", scoreInfo->leftScore);
	TextOut(drawingInfo->hMemDC, x, y, text, wcslen(text));
	
	x = drawingInfo->CenterX + 50;
	y = margin + 5;

	swprintf_s(text, 256, L"%d", scoreInfo->rightScore);
	TextOut(drawingInfo->hMemDC, x, y, text, wcslen(text));

	SetTextAlign(drawingInfo->hMemDC, prevAlignText);
	SelectObject(drawingInfo->hMemDC, prevFont);
}

void KeyUp(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 0x57:							// W key
		leftPlatform->KeyUpPressed		= false;
		break;
	case 0x53:							// S key
		leftPlatform->KeyDownPressed	= false;
		break;
	case VK_UP:
		rightPlatform->KeyUpPressed		= false;
		break;
	case VK_DOWN:
		rightPlatform->KeyDownPressed	= false;
		break;
	default:
		break;
	}
}

void KeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 0x57:							// W key
		leftPlatform->KeyUpPressed		= true;
		break;
	case 0x53:							// S key
		leftPlatform->KeyDownPressed	= true;
		break;
	case VK_UP:
		rightPlatform->KeyUpPressed		= true;
		break;
	case VK_DOWN:
		rightPlatform->KeyDownPressed	= true;
		break;
	default:
		break;
	}
}

void Start()
{
	Stop();
	ball->Spawn();
	is_game_running = true;
}

void Stop()
{
	is_game_running = false;

	scoreInfo->leftScore = 0;
	scoreInfo->rightScore = 0;

	leftPlatform->x = leftPlatform->startX;
	leftPlatform->y = leftPlatform->startY;

	rightPlatform->x = rightPlatform->startX;
	rightPlatform->y = rightPlatform->startY;
	ball->Delete();
}

void RespawnBall()
{
	ball->Spawn();
}

void ProcessMessages(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_RIGHT_PLAYER_SCORED:
		scoreInfo->rightScore++;
		RespawnBall();
		break;
	case ID_LEFT_PLAYER_SCORED:
		scoreInfo->leftScore++;
		RespawnBall();
		break;
	default:
		break;
	}

}
