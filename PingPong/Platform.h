#pragma once

class DrawingInfo;

class Platform
{
private:
	DrawingInfo*	_drawingInfo;
	const int		SPEED = 200;

public:
	int		startX;
	int		startY;

	int		currentSpeed;

	int		x;
	int		y;

	int		width;
	int		height;

	bool	KeyUpPressed;
	bool	KeyDownPressed;

	void Update(DWORD deltaTime);
	void Draw();

	Platform(DrawingInfo* drawingInfo);
	~Platform();
};

