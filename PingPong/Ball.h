#pragma once

class DrawingInfo;
class Platform;

class Ball
{
private:
	DrawingInfo* _drawingInfo;
	Platform* _leftP;
	Platform* _rightP;
	bool _is_spawned;
public:
	int size;

	int x;
	int y;

	int vx;
	int vy;

	Ball(DrawingInfo* drawingInfo, Platform* leftPlatform, Platform* rightPlatform);
	~Ball();

	void Update(DWORD deltaTime);
	void Draw();
	void Spawn();
	void Delete();
};

