/*
 *  iOSUtils.mm
 *  Created by Seth Robinson on 3/6/09.
 *  For license info, check the license.txt file that should have come with this.
 *
 */
#import "iOSUtils.h"
#import <UIKit/UIKit.h>
#import <cstdarg>
#import <string>
#import <sys/time.h>
#import <sys/sysctl.h>
#import "BaseApp.h"
#import <SystemConfiguration/SystemConfiguration.h>
//#include "Network/NetUtils.h"
#import <MobileCoreServices/MobileCoreServices.h> 
#import <sys/socket.h>
#import <netinet/in.h>
#import <net/if.h>
#import <net/if_dl.h>
#import <mach/mach.h>
#import <mach/mach_host.h>

using namespace std;

unsigned int GetFreeMemory()
{
    mach_port_t host_port;
    mach_msg_type_number_t host_size;
    vm_size_t pagesize;
    
    host_port = mach_host_self();
    host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    host_page_size(host_port, &pagesize);
    
    vm_statistics_data_t vm_stat;
    
    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS)
        LogMsg("Failed to fetch vm statistics");
    
    /* Stats in bytes */
    natural_t mem_free = vm_stat.free_count * pagesize;
    //natural_t mem_total = mem_used + mem_free;
    //natural_t mem_used = (vm_stat.active_count + vm_stat.inactive_count + vm_stat.wire_count) * pagesize;
    // LogMsg("Mem used: %u free: %u total: %u", mem_used, mem_free, mem_total);
    return mem_free;
}

void CreateDirectoryRecursively(string basePath, string path)
{
	string fileName = basePath+path;
	NSString *str =  [NSString stringWithCString: fileName.c_str() encoding: [NSString defaultCStringEncoding]];
    [[NSFileManager defaultManager] createDirectoryAtPath:str
                              withIntermediateDirectories:YES
                                               attributes:nil
                                                    error:nil];
}

bool RemoveDirectoryRecursively(string path)
{
	RemoveFile(path, false);
	return true;
}

vector<string> GetDirectoriesAtPath(string path)
{
	
#ifdef _DEBUG
    LogMsg("Scanning dir %s", path.c_str());
#endif
	vector<string> v;
	
	NSString *str =  [NSString stringWithCString: path.c_str() encoding: [NSString defaultCStringEncoding]];
	NSArray *origContents = [[NSFileManager defaultManager] directoryContentsAtPath:str];
	NSLog(@"Number of files = %d", origContents.count);
	
	string dir;
	
	for (int i = 0; i < origContents.count; i++)
	{
		dir = [[origContents objectAtIndex:i] cStringUsingEncoding:NSUTF8StringEncoding];
		if (dir.find(".") == string::npos)
		{
			//we're assuming that any name without a . in it is a dir.  Probably dumb, but works since we're controlling what gets written...
			v.push_back(dir);
		}
#ifdef _DEBUG
        NSLog (@"%d: <%@>", i, [origContents objectAtIndex:i]);
#endif
	}
	
	return v;
}

vector<string> GetFilesAtPath(string path)
{
	
	vector<string> v;
	
	NSString *str =  [NSString stringWithCString: path.c_str() encoding: [NSString defaultCStringEncoding]];
	NSArray *origContents = [[NSFileManager defaultManager] directoryContentsAtPath:str];
	//NSLog(@"Number of files = %d", origContents.count);
	
	string dir;
	
	for (int i = 0; i < origContents.count; i++)
	{
		dir = [[origContents objectAtIndex:i] cStringUsingEncoding:NSUTF8StringEncoding];
		//if (dir.find(".") != string::npos)
		{
			//we're assuming that any name without a . in it is a dir.  Probably dumb, but works since we're controlling what gets written...
			v.push_back(dir);
		}
		//NSLog (@"%d: <%@>", i, [origContents objectAtIndex:i]);
	}
	
	return v;
}

bool IsIphoneOriPad()
{
	return true;
}

float GetDeviceOSVersion()
{
	//TODO
	return 0.0f;
}

string GetClipboardText()
{
	string text;
	
	UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
    
	if ([pasteboard containsPasteboardTypes: [NSArray arrayWithObject:@"public.utf8-plain-text"]])
	{
		text = [pasteboard.string cStringUsingEncoding:NSUTF8StringEncoding];
	}
	return text;
}

bool IsDesktop() {return false;}

ePlatformID GetPlatformID()
{
	return PLATFORM_ID_IOS;
}
void NotifyOSOfOrientationPreference(eOrientationMode orientation)
{
    
}

bool HasVibration()
{
	if (IsIphone()) return true;
    
	return false;
}

void ForceVideoUpdate()
{
    //    LogMsg("TODO..video update");
}

///////////
///////
/////
void LogMsg(const char *lpFormat, ...)
{
	std::va_list argPtr ;
	va_start( argPtr, lpFormat ) ;
	NSLogv([NSString stringWithCString:lpFormat], argPtr) ;
	
	const int logSize = 4096;
	char buffer[logSize];
	memset ( (void*)buffer, 0, logSize );
	vsnprintf( buffer, logSize,  lpFormat, argPtr );
	
	if (IsBaseAppInitted())
        BaseApp::GetBaseApp()->GetConsole()->AddLine(buffer);
	
	va_end(argPtr) ;
	
} 

void LaunchURL(string url)
{
	NSURL *appStoreUrl = [NSURL URLWithString:[NSString stringWithCString: url.c_str() encoding: [NSString defaultCStringEncoding]]];
	[[UIApplication sharedApplication] openURL:appStoreUrl];
}

eNetworkType IsNetReachable(string url)
{
	return C_NETWORK_CELL;
}

string g_stringInput;
void SetLastStringInput(string s)
{
	g_stringInput = s;
}

string GetLastStringInput()
{
	return g_stringInput; //g_string?? heh. heh. hehe.	almost makes using globals a good thing.
}

string GetBaseAppPath()
{
	
	CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
    char path[PATH_MAX];
	Boolean const success = CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX);
    CFRelease(resourcesURL);	
    if (!success)
    {
        LogMsg("Can't change to Resources directory; something's seriously wrong\n");
		return "";
    }

	return string(path)+"/";
	
}

string GetSavePath()
{

	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES); 
	return string([ [paths objectAtIndex:0] cStringUsingEncoding:NSUTF8StringEncoding])+"/";
}

string GetAppCachePath() //writable, but not backed up by iTunes
{

	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES); 
	return string([ [paths objectAtIndex:0] cStringUsingEncoding:NSUTF8StringEncoding])+"/";
}

void CreateAppCacheDirIfNeeded()
{
	string path = GetAppCachePath();
	int idx = path.find("Library/Caches");
	if (idx != string::npos)
	{
		path = path.substr(0, idx);
		CreateDirectoryRecursively(path, "Library/Caches");
	}
}

void RemoveFile(string fileName, bool bAddSavePath)
{ 
	if (bAddSavePath)
	{
		fileName = GetSavePath()+fileName;
	}
	
	NSString *str =  [NSString stringWithCString: fileName.c_str() encoding: [NSString defaultCStringEncoding]];
	
	NSFileManager *FM = [NSFileManager defaultManager];
	[FM removeItemAtPath:str error:NULL];
}	

//The below function is based on a Snippet from http://iphonedevelopertips.com/device/determine-if-iphone-is-3g-or-3gs-determine-if-ipod-is-first-or-second-generation.html

string GetDeviceTypeString()
{
	size_t size;
	// Set 'oldp' parameter to NULL to get the size of the data
	// returned so we can allocate appropriate amount of space
	sysctlbyname("hw.machine", NULL, &size, NULL, 0); 
	// Allocate the space to store name
	char *name = (char*)malloc(size);
	// Get the platform name
	sysctlbyname("hw.machine", name, &size, NULL, 0);
	// Place name into a string
	string deviceType = name;
	// Done with this
	free(name);
	return deviceType;
}


bool IsIPhone3GS()
{
	if (GetDeviceTypeString() == "iPhone2,1") return true;
	return false;
}

bool IsIPodTouchThirdGen()
{
	if (GetDeviceTypeString() == "iPhone1,3") return true;
	return false;
}

bool IsIPAD()
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		return true;
	}
	
	return false;
}

float GetMainScreenScale()
{
    return [[UIScreen mainScreen] scale];
}

eDeviceMemoryClass GetDeviceMemoryClass()
{
	if (IsIPhone3GS()) return C_DEVICE_MEMORY_CLASS_2;
	if (IsIPodTouchThirdGen()) return C_DEVICE_MEMORY_CLASS_2;
	if (IsIPAD()) return C_DEVICE_MEMORY_CLASS_2;
	if (IsIphone4()) return C_DEVICE_MEMORY_CLASS_3;
	
	//default
	return C_DEVICE_MEMORY_CLASS_1;
}


bool IsIphone4()
{

	static bool bFirstTime = true;
	static bool bAnswer;
	
	if (bFirstTime)
	{
		bFirstTime = false;
		bAnswer = false;
	//#if __IPHONE_3_2 <= __IPHONE_OS_VERSION_MAX_ALLOWED
#if 30200 <= __IPHONE_OS_VERSION_MAX_ALLOWED
	
	if (UI_USER_INTERFACE_IDIOM() != UIUserInterfaceIdiomPad)
	{
		
		assert(30200 == __IPHONE_3_2 && "Uh oh, why did this change?");
		UIScreen* mainscr = [UIScreen mainScreen];
		if ([mainscr respondsToSelector:@selector( currentMode)] == YES) //avoid crash on pre 3.2 iOS
		{
			float x = mainscr.currentMode.size.width;
			float y = mainscr.currentMode.size.height;
			LogMsg("IsiPhone size reporting %.2f, %.2f", x, y);
			bAnswer = (x == 640 && y == 960) ; //if the screen is this big, it's gotta be a retina display
		}

	} else
	{
		//an iPad?
		
	}
#endif
		
	}
	
	return bAnswer;
}

int g_primaryGLX = 0;
int g_primaryGLY = 0;

void SetPrimaryScreenSize(int width, int height)
{
    g_primaryGLX = width;
    g_primaryGLY = height;
}


//this doesn't change even if you rotate, for speed
int GetPrimaryGLX() {return g_primaryGLX;}
int GetPrimaryGLY() {return g_primaryGLY;}

int GetSystemData()
{	
	
	//this is .. just don't use it anymore, unsafe
	
	/*
	//I didn't want the text SignerIdentity existing in the exe for simple text searches to find
	string encoded = "Zqpxp%7EVrt%7E%85%7B%87%8D";
	URLDecoder decode;
	vector<byte> data = decode.decodeData(encoded);
	DecryptPiece((byte*)&data[0], data.size(), 5);
	data.push_back(0); //a null for the string

	//LogMsg((char*)&data[0]);
	NSString *str =  [NSString stringWithCString: (char*)&data[0] encoding: [NSString defaultCStringEncoding]];

	NSBundle *bundle = [NSBundle mainBundle];
	NSDictionary *info = [bundle infoDictionary];
	if ([info keyExists: str] != nil)
	{
	 return C_PIRATED_YES;
	}
	*/
	
	return C_PIRATED_NO;
}

bool CheckDay(const int year, const int month, const int day)
{
	NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
	
	[dateFormatter setDateFormat:@"yyyy"];
    int nowyear = [[dateFormatter stringFromDate:[NSDate date]] intValue];

    [dateFormatter setDateFormat:@"MM"];
    int nowmonth = [[dateFormatter stringFromDate:[NSDate date]] intValue];

    [dateFormatter setDateFormat:@"dd"];
    int nowday = [[dateFormatter stringFromDate:[NSDate date]] intValue];
    [dateFormatter release];

	if ((nowyear == year) && (nowmonth == month) && (nowday == day))
	{
		return true;
	}
	return false;
}

bool LaterThanNow(const int year, const int month, const int day)
{
	NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
	
	[dateFormatter setDateFormat:@"yyyy"];
    int nowyear = [[dateFormatter stringFromDate:[NSDate date]] intValue];

    [dateFormatter setDateFormat:@"MM"];
    int nowmonth = [[dateFormatter stringFromDate:[NSDate date]] intValue];

    [dateFormatter setDateFormat:@"dd"];
    int nowday = [[dateFormatter stringFromDate:[NSDate date]] intValue];
    [dateFormatter release];
    
    if (nowyear< year )
	{
		return false;
	}
	if (nowyear> year )
	{
		return true;
	}
	// year must be equal
	if (nowmonth < month )
	{
		return false;
	}
	if (nowmonth > month )
	{
		return true;
	}
	// month must be equal
	if (nowday < day )
	{
		return false;
	}
	if (nowday > day )
	{
		return true;
	}
    
	return false;
}
	
unsigned int GetSystemTimeTick()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec/1000 + tv.tv_sec*1000;
}

double GetSystemTimeAccurate()
{
	static double first = CFAbsoluteTimeGetCurrent();
	//	NSLog(@"operation took %.5f ms", CFAbsoluteTimeGetCurrent()*1000 );
	return uint32((CFAbsoluteTimeGetCurrent()-first)*1000);
}


bool IsIphone()
{
if (IsIPAD()) return false;

 	NSString *deviceType = [UIDevice currentDevice].model;
    if([deviceType isEqualToString:@"iPhone"])
	{
		return true;
	}
	return false;
}

const char* iPhoneVersion()
{
	NSString *deviceType = [UIDevice currentDevice].systemVersion;
	return [deviceType cStringUsingEncoding:NSUTF8StringEncoding];
}

#ifndef RT_NO_UDID
string GetDeviceID()
{
	UIDevice *device = [UIDevice currentDevice];
	NSString *uniqueIdentifier = [device uniqueIdentifier];
	return string([uniqueIdentifier cStringUsingEncoding:NSUTF8StringEncoding]);
}
#endif

//Code taken from http://www.mobiledev.nl/udid-usage-rejected-by-apple-for-ios-apps

//Returns blank string on error

string GetMacAddress()
{
	  int                 mgmtInfoBase[6];
  char                *msgBuffer = NULL;
  size_t              length;
  unsigned char       macAddress[6];
  struct if_msghdr    *interfaceMsgStruct;
  struct sockaddr_dl  *socketStruct;
  NSString            *errorFlag = NULL;
 
  // Setup the management Information Base (mib)
  mgmtInfoBase[0] = CTL_NET;        // Request network subsystem
  mgmtInfoBase[1] = AF_ROUTE;       // Routing table info
  mgmtInfoBase[2] = 0;              
  mgmtInfoBase[3] = AF_LINK;        // Request link layer information
  mgmtInfoBase[4] = NET_RT_IFLIST;  // Request all configured interfaces
 
  // With all configured interfaces requested, get handle index
  if ((mgmtInfoBase[5] = if_nametoindex("en0")) == 0) 
    errorFlag = @"if_nametoindex failure";
  else
  {
    // Get the size of the data available (store in len)
    if (sysctl(mgmtInfoBase, 6, NULL, &length, NULL, 0) < 0) 
      errorFlag = @"sysctl mgmtInfoBase failure";
    else
    {
      // Alloc memory based on above call
      if ((msgBuffer = (char*)malloc(length)) == NULL)
        errorFlag = @"buffer allocation failure";
      else
      {
        // Get system information, store in buffer
        if (sysctl(mgmtInfoBase, 6, msgBuffer, &length, NULL, 0) < 0)
          errorFlag = @"sysctl msgBuffer failure";
      }
    }
  }
  // Befor going any further...
  if (errorFlag != NULL)
  {
    NSLog(@"Error: %@", errorFlag);
//   string([macAddressString cStringUsingEncoding:NSUTF8StringEncoding]);
    return "";
  }
  // Map msgbuffer to interface message structure
  interfaceMsgStruct = (struct if_msghdr *) msgBuffer;
  // Map to link-level socket structure
  socketStruct = (struct sockaddr_dl *) (interfaceMsgStruct + 1);  
  // Copy link layer address data in socket structure to an array
  memcpy(&macAddress, socketStruct->sdl_data + socketStruct->sdl_nlen, 6);  
  // Read from char array into a string object, into traditional Mac address format
  NSString *macAddressString = [NSString stringWithFormat:@"%02X:%02X:%02X:%02X:%02X:%02X", 
                                macAddress[0], macAddress[1], macAddress[2], 
                                macAddress[3], macAddress[4], macAddress[5]];
  //NSLog(@"Mac Address: %@", macAddressString);  
  // Release the buffer memory
  free(msgBuffer);
  return string([macAddressString cStringUsingEncoding:NSUTF8StringEncoding]);
}

string GetRegionString()
{
	NSLocale *currentUsersLocale = [NSLocale currentLocale];
	NSLog(@"Current Locale: %@", [currentUsersLocale localeIdentifier]);
	return [[currentUsersLocale localeIdentifier] cStringUsingEncoding:NSUTF8StringEncoding];
}

bool IsAppInstalled(string packageName)
{
	#ifdef _DEBUG
	  LogMsg("IsAppInstalled not yet handled for this OS");
	#endif
	return false;
}

