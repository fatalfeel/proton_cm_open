/*
 *  App.cpp
 *  Created by Seth Robinson on 4/27/11.
 *  For license info, check the license.txt file that should have come with this.
 *
 */
#include "PlatformPrecomp.h"
#include "App.h"
#include "GUI/MainMenu.h"
#include "Entity/EntityUtils.h"
#include "Irrlicht/IrrlichtManager.h"
#include "FileSystem/FileSystemZip.h"

irr::video::E_DRIVER_TYPE AppGetOGLESType()
{
    return irr::video::EDT_OGLES2;
}

///////////////////////////////////////
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
App *g_pApp = NULL;

App * GetApp() 
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
	//L_ParticleSystem::deinit(); //if we wanted to use the 2d particle system
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
		SetLockedLandscape(true); //we don't allow portrait mode for this game.  Android doesn't count
		//because its landscape mode is addressed like portrait mode when set in the manifest.
	}
	
	if (GetEmulatedPlatformID() == PLATFORM_ID_WEBOS && IsIPADSize)
	{
		LogMsg("Special handling for touchpad landscape mode..");
		SetLockedLandscape(false);
		SetupScreenInfo(GetPrimaryGLX(), GetPrimaryGLY(), ORIENTATION_PORTRAIT);
	}
	//SetupFakePrimaryScreenSize(768,1024); //game will think its this size, and will be scaled up
	//SetupFakePrimaryScreenSize(1024,768); //game will think its this size, and will be scaled up

	//L_ParticleSystem::init(2000); //if we wanted to use the 2d particle system

	if (m_bInitted)	
	{
		return true;
	}
	
	SetManualRotationMode(true);
    
    if (!BaseApp::Init())
        return false;
	
	LogMsg("Save path is %s", GetSavePath().c_str());


	//we'll load a .zip as our filesystem if we can, this isn't really needed, but useful for testing Android style
	//loading from a zip from windows to debug it.

    /*if (GetPlatformID() != PLATFORM_ID_ANDROID)
	{
        FileSystemZip *pFileSystem = new FileSystemZip();
		      
        pFileSystem->SetRootDirectory("assets");
        
		FileManager::GetFileManager()->MountFileSystem(pFileSystem);
	}*/
	
	m_varDB.Load("save.dat", &bFileExisted);
    
    /*if (!GetFont(FONT_SMALL)->Load("interface/font_mono.rtfont"))
        return false;
	if (!GetFont(FONT_LARGE)->Load("interface/font_mono_big.rtfont"))
        return false;*/

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

//#ifdef _WIN32
//		gui::IGUIStaticText*    text    = gui->addStaticText(L"FPS:", core::rect<s32>(0,25,GetScreenSizeX()-1,35), true, false, NULL, 0x10000, true);
//#else
		gui::IGUIStaticText*    text    = gui->addStaticText(L"FPS:", core::rect<s32>(0,0,GetScreenSizeX()-1,10), true, false, NULL, 0x10000, true);
//#endif
        
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
	
		//MainMenuCreate(pGUIEnt);
		m_MenuEntity = MainMenuCreate(pGUIEnt);

        //only applicable to iOS, makes the keyboard pop up faster
        //PreloadKeyboard();
	}
}


void App::Draw()
{
	IrrlichtManager::GetIrrlichtManager()->isNeedInitAgain();
	
	//GetBaseApp()->OnScreenSizeChange();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	IrrlichtManager::GetIrrlichtManager()->IsRunning(); //let it do its own update tick
	IrrlichtManager::GetIrrlichtManager()->BeginScene(); //turn on irrlicht's 3d mode renderstates
	IrrlichtManager::GetIrrlichtManager()->Render(); //render its scenegraph
	IrrlichtManager::GetIrrlichtManager()->EndScene(); //tell irrlicht to go into 2d mode renderstates
    //IrrlichtManager::GetIrrlichtManager()->RenderDebugProperty2D(); //give irrlicht manager a chance to draw its debug stuff
	BaseApp::Draw();

	//g_lastBound = 0;
	
	//GetFont(FONT_SMALL)->Draw(0,50, " white `2Green `3Cyan `4Red `5Purp ");
    //SetupOrtho();
   	
	//BaseApp::Draw();
	//PrepareForGL();
}

void App::OnScreenSizeChange()
{
	BaseApp::OnScreenSizeChange();
}

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

const char * GetAppName() 
{
	return "RTPhysics";
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
	const char * bundleName = "rtphysics";
	return bundleName;
}


