//This file is used for all Windows builds and contains the WinMain entry point and Windows app code
#pragma once

//extern const char*	GetAppName();
//extern const TCHAR*	GetAppNameW();
extern bool			g_bAppFinished;
extern int			g_winVideoScreenX;
extern int			g_winVideoScreenY;
extern bool			g_bIsFullScreen;
extern std::string	g_videoModeNameForce; //if set, video mode will be forced to this instead of what is set in main.cpp

void InitVideoSize();

class VideoModeEntry
{
public:
	VideoModeEntry(string _name, int _x, int _y, ePlatformID _platformID, eOrientationMode _forceOrientation = ORIENTATION_DONT_CARE) : name(_name), x(_x), y(_y), platformID (_platformID), forceOrientation (_forceOrientation){};
	
	std::string			name;
	int					x, y;
	ePlatformID			platformID;
	eOrientationMode	forceOrientation; //so LockLandscape() won't actually do it when using the 'iPhone Landscape' mode.. we 
	//really want it to stay in portrait mode.
};

enum eWinActions
{
	ACTION_DOWN,
	ACTION_UP,
	ACTION_MOVE,
	ACTION_CANCEL,
	ACTION_OUTSIDE,
};

class WinMessageCache
{
public:
	WinMessageCache()
	{}
	~WinMessageCache()
	{}

	eWinActions	type;
	float		x,y;
	int			finger;
};
