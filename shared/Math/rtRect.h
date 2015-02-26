//  ***************************************************************
//  rtRect - Creation date: 03/26/2009
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2009 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef rtRect_h__
#define rtRect_h__
#include "ClanLib-2.0/Sources/API/Core/Math/vec2.h"

//this alignment naming below suffers from "add as you go with no plan".  They aren't really clear.. -Seth

enum eAlignment
{
	ALIGNMENT_UPPER_LEFT,
	ALIGNMENT_CENTER,
	ALIGNMENT_DOWN_CENTER,
	ALIGNMENT_UPPER_RIGHT,
	ALIGNMENT_DOWN_LEFT,
	ALIGNMENT_UPPER_CENTER,
	ALIGNMENT_LEFT_CENTER,
	ALIGNMENT_DOWN_RIGHT
};

struct rtRect
{
	rtRect();
	rtRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {};
	int left,top,right,bottom;
	void Clear() {left = top = right = bottom = 0;}
	int GetHeight() const {return bottom-top;}
	int GetWidth() const {return right-left;}
	void ScaleCentered(float f);
	void AdjustPosition(int x, int y); //move the rect by repositioning the upper left hand corner
	void Inflate(int x, int y);
};

struct rtRectf
{
	rtRectf();
	rtRectf(rtRect r){ left = (float)r.left; top = (float)r.top; right = (float)r.right; bottom =  (float)r.bottom; };
	
	rtRectf(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {};
	float left,top, right,bottom;
	void Clear() {left = top = right = bottom = 0;}
	float GetHeight() const {return bottom-top;}
	float GetWidth() const {return right-left;}
	void AdjustPosition(float x, float y); //move the rect by repositioning the upper left hand corner
	void Scale(eAlignment alignment,  CL_Vec2f vScale );

	rtRectf operator+ (const rtRectf &r) const;

};

#endif // rtRect_h__