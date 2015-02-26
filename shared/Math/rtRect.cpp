#include "PlatformPrecomp.h"

#include "rtRect.h"

void rtRect::ScaleCentered(float f)
{
	f -= 1.0f;
	int changeX = (int)(float(GetWidth())*f)/2;
	int changeY = (int)(float(GetHeight())*f)/2;

	left -= changeX;
	right += changeX;

	top -= changeY;
	bottom += changeY;
}

rtRect::rtRect()
{

}

void rtRect::AdjustPosition( int x, int y )
{
	left += x;
	right += x;
	top += y;
	bottom += y;
}

void rtRect::Inflate( int x, int y )
{
	left -= x;
	right += x;
	top -= y;
	bottom += y;
}

void rtRectf::AdjustPosition( float x, float y )
{
	left += x;
	right += x;
	top += y;
	bottom += y;
}

rtRectf::rtRectf()
{
}

void rtRectf::Scale( eAlignment alignment, CL_Vec2f vScale )
{
	vScale.x -= 1.0f;
	vScale.y -= 1.0f;
	
	float changeX = ((GetWidth())*vScale.x);
	float changeY = ((GetHeight())*vScale.y);

	
	switch(alignment)
	{
		case ALIGNMENT_CENTER:
			changeX /= 2;
			changeY /= 2;
			
			left -= changeX;
			right += changeX;

			top -= changeY;
			bottom += changeY;
		break;

		case ALIGNMENT_UPPER_LEFT:
			right += changeX;
			bottom += changeY;
		break;

		case ALIGNMENT_UPPER_RIGHT:
			left -= changeX;
			bottom += changeY;
			break;

		case ALIGNMENT_DOWN_CENTER:
			left -= changeX/2;
			right += changeX/2;
			top -= changeY;
			break;
		case ALIGNMENT_UPPER_CENTER:
			left -= changeX/2;
			right += changeX/2;
			bottom+=changeY;
			break;

		case ALIGNMENT_DOWN_LEFT:
			right += changeX;
			top -= changeY;
			break;
		
		case ALIGNMENT_DOWN_RIGHT:
			left -= changeX;
			top -= changeY;
			break;

		case ALIGNMENT_LEFT_CENTER:
			right += changeX;
			top -= changeY/2;
			bottom += changeY/2;
			break;

		default:

			LogError("Unknown alignment");
		break;
	}
}

rtRectf rtRectf::operator+ (const rtRectf &r) const
{

	return rtRectf(left+r.left, top+r.top, right+r.right, bottom+r.bottom);
}
