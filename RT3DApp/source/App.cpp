/*
 *  App.cpp
 *  Created by Seth Robinson on 3/6/09.
 *  For license info, check the license.txt file that should have come with this.
 *
 */
#include "PlatformPrecomp.h"
#include "App.h"
#include "Entity/EntityUtils.h"
#include "Irrlicht/IrrlichtManager.h"
#include "FileSystem/FileSystemZip.h"

#include "cocos2d.h"
using namespace cocos2d;
#include "GUI/MainMenu.h"
#include "GUI/HelloWorldScene.h"

irr::video::E_DRIVER_TYPE AppGetOGLESType()
{
#ifdef _IRR_COMPILE_WITH_OGLES1_
	return irr::video::EDT_OGLES1;
#else
	return irr::video::EDT_OGLES2;
#endif
}

////////////////////////////////////////
#ifdef __MACH__
	#if (TARGET_OS_IPHONE == 1)
        //it's an iPhone or iPad
		#include "Audio/AudioManagerOS.h"
		AudioManagerOS*		g_audioManager = NULL;
	#else
        //it's being compiled as a native OSX app
		#include "Audio/AudioManagerDenshion.h"
		AudioManagerDenshion*	g_audioManager = NULL; //dummy with no sound
	#endif
#else
	#include "Audio/AudioManagerSDL.h"
	#include "Audio/AudioManagerAndroid.h"

	#if defined RT_WEBOS || defined RT_USE_SDL_AUDIO
		AudioManagerSDL*		g_audioManager = NULL; //sound in windows and WebOS
	#elif defined ANDROID_NDK
		AudioManagerAndroid*	g_audioManager = NULL; //sound for android
	#else
		#include "Audio/AudioManagerAudiere.h"
		AudioManagerAudiere*	g_audioManager = NULL;  //Use Audiere for audio
	#endif
#endif

AudioManager* GetAudioManager()
{
#if defined __APPLE__
    #if (TARGET_OS_IPHONE == 1)
        g_audioManager = AudioManagerOS::GetAudioManager();
    #else
        g_audioManager = AudioManagerDenshion::GetAudioManager();
    #endif
#elif defined RT_WEBOS || defined RT_USE_SDL_AUDIO
	g_audioManager = AudioManagerSDL::GetAudioManager();
#elif defined ANDROID_NDK
	g_audioManager = AudioManagerAndroid::GetAudioManager();
#else
	g_audioManager = AudioManagerAudiere::GetAudioManager();
#endif
    
	return g_audioManager;
}

void FreeAudioManager()
{
#if defined __APPLE__
    #if (TARGET_OS_IPHONE == 1)
        AudioManagerOS::Free();
    #else
        AudioManagerDenshion::Free();
    #endif
#elif defined RT_WEBOS || defined RT_USE_SDL_AUDIO
	AudioManagerSDL::Free();
#elif defined ANDROID_NDK
	AudioManagerAndroid::Free();
#else
	AudioManagerAudiere::Free();
#endif
    
	g_audioManager = NULL;
}

//////////////////App////////////////////////
App* g_pApp = NULL;

App* GetApp()
{
	return g_pApp;
}

App::App()
{
	m_bDidPostInit	= false;
	m_MenuEntity	= NULL;
}

App::~App()
{
	FreeAudioManager();

	Entity::Free();
	FileManager::Free();
	ResourceManager::Free();
	IrrlichtManager::Free();
	MessageManager::Free();
}

bool App::Init()
{
    bool bFileExisted;
	
    //SetDefaultButtonStyle(Button2DComponent::BUTTON_STYLE_CLICK_ON_TOUCH_RELEASE);
	
	if (GetEmulatedPlatformID() == PLATFORM_ID_IOS)
	{
        //we don't allow portrait mode for this game.  Android doesn't count
		//because its landscape mode is addressed like portrait mode when set in the manifest.
		SetLockedLandscape(true);
	}
    
	if (GetEmulatedPlatformID() == PLATFORM_ID_WEBOS && IsIPADSize)
	{
		LogMsg("Special handling for touchpad landscape mode..");
		SetLockedLandscape(false);
		SetupScreenInfo(GetPrimaryGLX(), GetPrimaryGLY(), ORIENTATION_PORTRAIT);
	}
    
	//SetupFakePrimaryScreenSize(320,480); //game will think its this size, and will be scaled up
	//L_ParticleSystem::init(2000); //if we wanted to use the 2d particle system
    
	if (m_bInitted)
	{
		return true;
	}
    
    SetManualRotationMode(true);
    
	if (!BaseApp::Init())
		return false;
    
	LogMsg("Save path is %s", GetSavePath().c_str());
    
	m_varDB.Load("save.dat", &bFileExisted);
    
	//preload audio
	if( GetAudioManager() )
	{
		GetAudioManager()->Preload("audio/click.wav");
		//GetAudioManager()->Play("audio/real.mp3",1,1); //ios 128bps mp3
		//GetAudioManager()->Play("audio/real.ogg",1,1); //android play ogg
	}
	
	if (!IrrlichtManager::GetIrrlichtManager()->Init(0))
		return false;
    
#ifdef	_IRR_COMPILE_WITH_GUI_
    SetFPSVisible(true);
#else
    SetFPSVisible(false);
#endif
    
    if( GetFPSVisible() )
    {
        IrrlichtDevice*         device  = IrrlichtManager::GetIrrlichtManager()->GetDevice();
        gui::IGUIEnvironment*   gui     = device->getGUIEnvironment();
		gui::IGUIStaticText*    text    = gui->addStaticText(L"FPS:", core::rect<s32>(0,0,GetScreenSizeX()-1,10), true, false, NULL, 0x10000, true);
        
        text->setOverrideColor(video::SColor(255,0,0,0));
        text->setBackgroundColor(video::SColor(255,255,255,255));
    }
    
	return true;
}

void App::Kill()
{
	Entity::GetEntityManager()->RemoveAllEntities();
	IrrlichtManager::GetIrrlichtManager()->Kill();
	BaseApp::Kill();
}

void App::Update()
{
	BaseApp::Update();
    
	if (!m_bDidPostInit)
	{
		m_bDidPostInit = true;
		m_special = GetSystemData() != C_PIRATED_NO;
		
		//build a GUI node
		Entity *pGUIEnt = Entity::GetEntityManager()->AddEntity(new Entity("GUI"));
		//by stone, highlevel shader used
		m_MenuEntity = MainMenuCreate(pGUIEnt);
		
		//init
		CCScene* pScene = HelloWorld::scene();
		// run
		CCDirector::sharedDirector()->runWithScene(pScene);
	}
}

void App::Draw()
{
	IrrlichtManager::GetIrrlichtManager()->isNeedInitAgain();
	
	//turn normal GL back on
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	IrrlichtManager::GetIrrlichtManager()->IsRunning(); //let it do its own update tick
	IrrlichtManager::GetIrrlichtManager()->BeginScene(); //turn on irrlicht's 3d mode renderstates
	IrrlichtManager::GetIrrlichtManager()->Render(); //render its scenegraph
	IrrlichtManager::GetIrrlichtManager()->EndScene(); //tell irrlicht to go into 2d mode renderstates
	
	BaseApp::Draw();
}

/*void App::OnScreenSizeChange()
 {
 BaseApp::OnScreenSizeChange();
 }*/

void App::GetServerInfo( string &server, uint32 &port )
{
#if defined (_DEBUG) && defined(WIN32)
	server = "localhost";
	port = 8080;
    
	//server = "www.rtsoft.com";
	//port = 80;
#else
    
	server = "rtsoft.com";
	port = 80;
#endif
}

int App::GetSpecial()
{
	return m_special; //1 means pirated copy
}

Variant * App::GetVar( const string &keyName )
{
	return GetShared()->GetVar(keyName);
}

std::string App::GetVersionString()
{
	return "V0.7";
}

float App::GetVersion()
{
	return 0.7f;
}

int App::GetBuild()
{
	return 1;
}

void App::SaveStuff()
{
	m_varDB.Save("save.dat");
}
void App::OnEnterBackground()
{
	SaveStuff();
	BaseApp::OnEnterBackground();
}

void App::OnEnterForeground()
{
	BaseApp::OnEnterForeground();
}


void App::OnExitApp(VariantList *pVarList)
{
	LogMsg("Exiting the app");
    
	OSMessage o;
	o.m_type = OSMessage::MESSAGE_FINISH_APP;
	BaseApp::GetBaseApp()->AddOSMessage(o);
}

///////////////////////////////////////////////////////
const wchar_t* GetAppName()
{
	return L"RT3DApp";
}

//for palm webos, android, App Store
const char * GetBundlePrefix()
{
	const char * bundlePrefix = "com.rtsoft.";
	return bundlePrefix;
}

//applicable to Palm WebOS builds only
const char * GetBundleName()
{
	const char * bundleName = "rt3dapp";
	return bundleName;
}

std::wstring StringToWstring(std::string str)
{
    unsigned len = str.size() * 2;
    setlocale(LC_CTYPE, "");
    wchar_t *p = new wchar_t[len];
    mbstowcs(p,str.c_str(),len);
    std::wstring str1(p);
    delete[] p;
    return str1;
}

std::string WstringToString(std::wstring str)
{
    unsigned len = str.size() * 4;
    setlocale(LC_CTYPE, "");
    char *p = new char[len];
    wcstombs(p,str.c_str(),len);
    std::string str1(p);
    delete[] p;
    return str1;
}

void WideStrToUTF8(std::string& dest, wstring& src)
{
	wchar_t w;
	
	dest.clear();
    
	for (size_t i = 0; i < src.size(); i++)
	{
		w = src[i];
        
		if (w <= 0x7f)
		{
			dest.push_back((char)w);
		}
		else if (w <= 0x7ff)
		{
			dest.push_back(0xc0 | ((w >> 6)& 0x1f));
			dest.push_back(0x80| (w & 0x3f));
		}
		else if (w <= 0xffff)
		{
			dest.push_back(0xe0 | ((w >> 12)& 0x0f));
			dest.push_back(0x80| ((w >> 6) & 0x3f));
			dest.push_back(0x80| (w & 0x3f));
		}
		else if (sizeof(wchar_t) > 2 && w <= 0x10ffff)
		{
			dest.push_back(0xf0 | ((w >> 18)& 0x07)); // wchar_t 4-bytes situation
			dest.push_back(0x80| ((w >> 12) & 0x3f));
			dest.push_back(0x80| ((w >> 6) & 0x3f));
			dest.push_back(0x80| (w & 0x3f));
		}
		else
		{
			dest.push_back('?');
		}
	}
}
