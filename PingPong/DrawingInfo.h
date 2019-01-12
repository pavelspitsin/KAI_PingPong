#pragma once
class DrawingInfo
{
public:

	int		ScreenMaxX;
	int		ScreenMaxY;

	int		CenterX;
	int		CenterY;

	int		WidthArea;
	int		HeightArea;

	int		MarginArea;

	HDC		hMemDC;
	HWND	hWnd;


	HBRUSH	hBrushBlack;
	HBRUSH	hBrushRed;
	HPEN	hPenRed;
	HPEN	hPenDash;

	HFONT	hLogoFont;
	HFONT	hScoreFont;

	DrawingInfo(HWND hWnd);
	~DrawingInfo();
};

