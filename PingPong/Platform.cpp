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

	float sDeltaTime = deltaTime / 1000.0;				// ������� ����� ������������ � ��������

	int topBound = _drawingInfo->MarginArea;
	int bottomBound = _drawingInfo->HeightArea - _drawingInfo->MarginArea;
	
	y += currentSpeed * sDeltaTime;

	// ����������� ��������� ������
	if (y < topBound)
		y = topBound;

	// ����������� ��������� �����
	if ((y + height) > bottomBound)
		y = bottomBound - height;
}

void Platform::Draw()
{
	Rectangle(_drawingInfo->hMemDC, x, y, x + width, y + height);
}
