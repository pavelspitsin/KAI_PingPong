#include "stdafx.h"
#include "Ball.h"
#include "DrawingInfo.h"
#include "Platform.h"
#include "Resource.h"


Ball::Ball(DrawingInfo* drawingInfo, Platform* leftPlatform, Platform* rightPlatform) : _is_spawned(false), size(10)
{	
	_drawingInfo = drawingInfo;
	_leftP = leftPlatform;
	_rightP = rightPlatform;
}


Ball::~Ball()
{
}

void Ball::Update(DWORD deltaTime)
{
	if (!_is_spawned)
		return;


	double sDeltaTime = deltaTime / 1000.0;				// Разница между обновлениями в секундах

	x += (int)vx * sDeltaTime;
	y += (int)vy * sDeltaTime;
	

	int margin = _drawingInfo->MarginArea;
	int width = _drawingInfo->WidthArea;
	int height = _drawingInfo->HeightArea;

	// Отскок от верхней границы
	if (y <= margin)
	{
		vy = -vy;
		y = margin;
		return;
	}

	// Отскок от нижней
	if ((y + size) >= height - margin)
	{
		vy = -vy;
		y = height - margin - size;
		return;
	}

	// Отскок от левой платформы
	// 
	if (x <= (_leftP->x + _leftP->width) && ((y + size) >= _leftP->y) && (y <= (_leftP->y + _leftP->height)))
	{
		x = _leftP->x + _leftP->width;
		vx = -vx;
		vy += _leftP->currentSpeed / 2;
		return;
	}

	// Отскок от правой платформы
	// 
	if (x >= (_rightP->x - _leftP->width) && ((y + size) >= _rightP->y) && (y <= (_rightP->y + _rightP->height)))
	{
		x = _rightP->x - _leftP->width;
		vx = -vx;
		vy += _rightP->currentSpeed / 2;
		return;
	}

	// Левый игрок пропустил
	if ((x + size) <= margin)
	{
		PostMessage(_drawingInfo->hWnd, WM_GAME, ID_RIGHT_PLAYER_SCORED, NULL);
		Delete();
	}

	// правый игрок пропустил
	if (x >= width)
	{
		PostMessage(_drawingInfo->hWnd, WM_GAME, ID_LEFT_PLAYER_SCORED, NULL);
		Delete();
	}
}

void Ball::Draw()
{
	if (!_is_spawned)
		return;
	
	Ellipse(_drawingInfo->hMemDC, x, y, x + size, y + size);
}

void Ball::Spawn()
{
	this->_is_spawned = true;
	this->x = _drawingInfo->CenterX - size / 2;
	this->y = _drawingInfo->CenterY - size / 2;
		
	if (rand() % 10000 >= 5000)
	{
		vx = -200;
	}
	else
	{
		vx = 200;
	}

	vy = 0;
}

void Ball::Delete()
{
	this->_is_spawned = false;
}
