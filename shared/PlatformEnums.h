#ifndef platformenums_h__
#define platformenums_h__

//these are defines and utility functions that are implemented on every platform. (Implemented in AndroidUtils.cpp, or WinUtils.cpp, or ioSUtils.cpp, etc)

#ifdef __cplusplus


#include <string>
#include <vector>

//this is a bit ugly.. at least I should make a macro to get rid of the redundency of writing the rez twice.. sure, later.. -Seth

#define IsPixiSize ((GetScreenSizeX() == 400 && GetScreenSizeY() ==  320)|| (GetScreenSizeX() == 320 && GetScreenSizeY() ==  480))
#define IsNexusOneSize (GetScreenSizeX() == 800 || GetScreenSizeY() ==  480)
#define IsDroidSize (GetScreenSizeX() == 854 || GetScreenSizeY() ==  480)
#define IsIphoneSize ((GetScreenSizeX() == 320 && GetScreenSizeY() ==  480)|| (GetScreenSizeX() == 480 && GetScreenSizeY() ==  320))
#define IsIphone4Size ((GetScreenSizeX() == 640 && GetScreenSizeY() ==  960)|| (GetScreenSizeX() == 960 && GetScreenSizeY() ==  640))
#define IsIPADSize ((GetScreenSizeX() == 1024 && GetScreenSizeY() ==  768)|| (GetScreenSizeY() == 1024 && GetScreenSizeX() ==  768))
#define IsXoomSize ((GetScreenSizeX() == 1280 && GetScreenSizeY() ==  800)|| (GetScreenSizeX() == 800 && GetScreenSizeY() ==  1280))
#define IsPlaybookSize ((GetScreenSizeX() == 1024 && GetScreenSizeY() ==  600)|| (GetScreenSizeX() == 600 && GetScreenSizeY() ==  1024))
#define IsIphone5Size ((GetScreenSizeX() == 1136 && GetScreenSizeY() ==  640)|| (GetScreenSizeX() == 640 && GetScreenSizeY() ==  1136))
#define IsIPADRetinaSize ((GetScreenSizeX() == 2048 && GetScreenSizeY() ==  1536)|| (GetScreenSizeY() == 1536 && GetScreenSizeX() ==  2048))
#define IsOptimusHDSize ((GetScreenSizeX() == 1280 && GetScreenSizeY() ==  720)|| (GetScreenSizeY() == 720 && GetScreenSizeX() ==  1280))
#define IsGalaxyYSize ((GetScreenSizeX() == 320 && GetScreenSizeY() == 240)|| (GetScreenSizeX() == 240 && GetScreenSizeY() ==  320))

//here we test for the REAL dimensions, ignoring any screen stretching we have enabled
#define IsPrimaryPlaybookSize ((GetPrimaryGLX() == 1024 && GetPrimaryGLY() ==  600)|| (GetPrimaryGLX() == 600 && GetPrimaryGLY() ==  1024))


enum eNetworkType
{
	C_NETWORK_NONE,
	C_NETWORK_CELL,
	C_NETWORK_WIFI
};

enum eDeviceMemoryClass
{
	C_DEVICE_MEMORY_CLASS_1, //iphone 3g, ipod touch, 128 mb
	C_DEVICE_MEMORY_CLASS_2, //iphone 3gs, iPad, ipod touch 3g, 256 mb
	C_DEVICE_MEMORY_CLASS_3, //iphone 4, iPod Touch 4g, 512 mb
	C_DEVICE_MEMORY_CLASS_4, //PC, Linux, OSX.. assuming to be a lot
};

enum ePlatformID
{
	PLATFORM_ID_UNKNOWN = -1,
	PLATFORM_ID_WINDOWS,
	PLATFORM_ID_IOS, //iPhone/iPad etc
	PLATFORM_ID_OSX,
	PLATFORM_ID_LINUX,
	PLATFORM_ID_ANDROID,
	PLATFORM_ID_WINDOWS_MOBILE, //yeah, right.  Doesn't look like we'll be porting here anytime soon.
	PLATFORM_ID_WEBOS,
	PLATFORM_ID_BBX, //RIM Playbook
	PLATFORM_ID_FLASH,
	
	//new platforms will be added above here.  Don't count on PLATFORM_ID_COUNT not changing!
	PLATFORM_ID_COUNT
};

enum eMarketID
{
	MARKET_ID_UNSPECIFIED,
	MARKET_ID_ANDROID_MARKET, 
	MARKET_ID_APPLE_APPSTORE, 
	MARKET_ID_AMAZON_MARKET
};

enum eOrientationMode
{
	//these match the iphone versions
	ORIENTATION_DONT_CARE,
	ORIENTATION_PORTRAIT = 1,
	ORIENTATION_PORTRAIT_UPSIDE_DOWN,
	ORIENTATION_LANDSCAPE_LEFT,
	ORIENTATION_LANDSCAPE_RIGHT,
	
};

//stuff available to platform specific code:

/**
 * Gets the platform id of the real running environment. The emulated platform
 * can be different from this one.
 */
ePlatformID GetPlatformID();
/**
 * Gets the emulated platform id. Can be different from the real platform id.
 * Can be set with \c SetEmulatedPlatformID().
 * By default this is the same as the real platform id.
 */
ePlatformID GetEmulatedPlatformID();

//platform include must support these functions:
void NotifyOSOfOrientationPreference(eOrientationMode orientation);

std::string GetBaseAppPath();
std::string GetSavePath();
std::string GetAppCachePath(); //on iPhone, returns path to save user data that isn't backed up, on Android, returns external sd card path or "" if not available
void SetAppCachePath(std::string path); //useful to set to ./ so you can ignore a file with the same name in a mounted zip, on windows at least
void CreateAppCacheDirIfNeeded();

bool LaterThanNow(const int year, const int month, const int day);
bool CheckDay(const int year, const int month, const int day);
unsigned int GetSystemTimeTick();
double GetSystemTimeAccurate();
unsigned int GetFreeMemory();
unsigned int GetNativeMemoryUsed(); //for flash, this means how many bytes the Flash system is using
void LaunchURL(std::string url);
void FireAchievement(std::string achievement);
void LogMsg(const char *lpFormat, ...);
void LogError (const char* traceStr, ... );
void SetLastStringInput(std::string s);
std::string GetLastStringInput();
void RemoveFile(std::string fileName, bool bAddSavePath = true);
void CreateDirectoryRecursively(std::string basePath, std::string path);
bool RemoveDirectoryRecursively(std::string path);
std::vector<std::string> GetDirectoriesAtPath(std::string path);
std::vector<std::string> GetFilesAtPath(std::string path);
std::string GetRegionString();
bool IsAppInstalled(std::string packageName); // implemented on Android only, send "com.cih.gamecih" to check for that hack tool, for instance
bool IsIphone();
bool IsIPAD();
bool IsIphoneOriPad();
bool IsIPhone3GS();
bool IsIphone4(); //also returns true if a iPod Touch g4, or anything else with retina screen dimensions
int GetPrimaryGLX();
int GetPrimaryGLY();
#define C_PIRATED_YES 0
#define C_PIRATED_NO 18263
int GetSystemData(); //returns C_PIRATED_YES or C_PIRATED_NO, only on systems where we can tell. (iOS only at this time)
eNetworkType IsNetReachable(std::string url);
void InitDeviceScreenInfo();
eDeviceMemoryClass GetDeviceMemoryClass();
bool IsIPodTouchThirdGen();
std::string GetClipboardText();
void SetClipboardText(std::string);

float GetDeviceOSVersion(); //0 if unknown/unimplemented.  Should return 1.00 for version 1, etc.  (webos returns 3.05 for instance, instead of 305)
std::string GetMacAddress(); //blank ("") if not implemented.  Only works on iOS for now
/**
 * Checks if the currently emulated platform is a desktop platform.
 */
bool IsDesktop();
bool HasVibration();
std::string GetDeviceID(); //returns a string that is probably unique to the device
void ForceVideoUpdate(); //force an early gl flip or whatever, sometimes useful.  Not implemented on ever platform

std::string GetDateAndTimeAsString(); //only supported on linux and win
void GetDateAndTime(int *monthOut, int *dayOut, int *yearOut, int *hourOut, int *minOut, int *secOut); //only supported on linux and win

#endif // platformenums_h__
#endif
