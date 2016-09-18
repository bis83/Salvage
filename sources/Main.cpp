#include "UsingDirectX.h"
#include "Game.h"

CGame *g_pGame = NULL;

void SetCenterWindowSize(HWND hWnd, int width, int height)
{
	RECT rect;
	int cw, ch, ww, wh, sx, sy;
	GetClientRect(hWnd, &rect);
	cw = rect.right - rect.left;
	ch = rect.bottom - rect.top;
	GetWindowRect(hWnd, &rect);
	ww = rect.right - rect.left;
	wh = rect.bottom - rect.top;
	ww = width + ww - cw;
	wh = height + wh - ch;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect, NULL);
	sx = rect.right - rect.left;
	sy = rect.bottom - rect.top;
	sx = (sx - ww) / 2;
	sy = (sy - wh) / 2;
	SetWindowPos(hWnd, HWND_TOP, sx, sy, ww, wh, NULL);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//=============================================================
// エントリーポイント
//=============================================================
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;
	MSG msg;
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = NULL;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = NULL;
	wcex.hIconSm = NULL;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "SalvageGame";
	if (RegisterClassEx(&wcex) == 0) return 0;

	hWnd = CreateWindow(
		"SalvageGame",
		"SALVAGE",
		WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInst,
		NULL);

	SetCenterWindowSize(hWnd, 1280, 720);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	if(g_pGame == NULL) {
		g_pGame = new CGame();
		if(!g_pGame->Init(hWnd)) {
			return 0;
		}
	}

	//--------------------------------------------
	// メインループ
	//--------------------------------------------
	ZeroMemory(&msg, sizeof(msg));
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			// メッセージの翻訳、送信
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// メイン処理
			if(!g_pGame->Update()) {
				PostQuitMessage(0);
			}
		}
	}
	g_pGame->Release();
	return 0;
}

//=============================================================
// ウィンドウプロシージャ関数
//=============================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_SETFOCUS:
		if(g_pGame != NULL) {
			g_pGame->Reflesh();
		}
		break;
	case WM_KILLFOCUS:
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
