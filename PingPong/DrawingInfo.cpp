#include "stdafx.h"
#include "DrawingInfo.h"


DrawingInfo::DrawingInfo(HWND hWnd)
{
	this->hWnd = hWnd;

	hMemDC				= NULL;
	hBrushBlack			= NULL;
	hBrushRed			= NULL;
	hPenRed				= NULL;
	hPenDash			= NULL;
	hScoreFont			= NULL;
	hLogoFont			= NULL;
}


DrawingInfo::~DrawingInfo()
{
	if (hMemDC)
	{
		ReleaseDC(hWnd, hMemDC);
		hMemDC = NULL;
	}

	if (hBrushBlack)
	{
		DeleteBrush(hBrushBlack);
		hBrushBlack = NULL;
	}

	if (hBrushRed)
	{
		DeleteBrush(hBrushRed);
		hBrushRed = NULL;
	}

	if (hPenRed)
	{
		DeletePen(hPenRed);
		hPenRed = NULL;
	}

	if (hPenDash)
	{
		DeletePen(hPenRed);
		hPenDash = NULL;
	}

	if (hLogoFont)
	{
		DeleteFont(hLogoFont);
		hLogoFont = NULL;
	}

	if (hScoreFont)
	{
		DeleteFont(hScoreFont);
		hScoreFont = NULL;
	}
}
