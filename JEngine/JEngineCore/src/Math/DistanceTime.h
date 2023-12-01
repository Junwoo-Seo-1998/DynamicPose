#pragma once


class SineInterpolation
{
public:
	SineInterpolation(float t1, float t2);
	float GetDistance(float time);
private:
	float t1;
	float t2;
	float L;
};

class Parabolic
{
public:
	Parabolic(float t1, float t2);
	float GetSpeed(float time);
	float GetDistance(float time);
private:
	float t1;
	float t2;
	float v;
};