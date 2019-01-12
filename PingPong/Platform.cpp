#include "stdafx.h"
#include "Platform.h"
#include "DrawingInfo.h"

Platform::Platform(DrawingInfo* drawingInfo)
{
	_drawingInfo = drawingInfo;
}


Platform::~Platform()
{
}


void Platform::Update(DWORD deltaTime)
{
	if (KeyUpPressed && !KeyDownPressed)
	{
		currentSpeed = -SPEED;
	}
	else if (!KeyUpPressed && KeyDownPressed)
	{
		currentSpeed = SPEED;
	}
	else
	{
		currentSpeed = 0;
	}

	float sDeltaTime = deltaTime / 1000.0;				// Разница между обновлениями в секундах

	int topBound = _drawingInfo->MarginArea;
	int bottomBound = _drawingInfo->HeightArea - _drawingInfo->MarginArea;
	
	y += currentSpeed * sDeltaTime;

	// ограничение платформы сверху
	if (y < topBound)
		y = topBound;

	// ограничение платформы снизу
	if ((y + height) > bottomBound)
		y = bottomBound - height;
}

void Platform::Draw()
{
	Rectangle(_drawingInfo->hMemDC, x, y, x + width, y + height);
}
