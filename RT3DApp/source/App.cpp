/*
 *  App.cpp
 *  Created by Seth Robinson on 3/6/09.
 *  For license info, check the license.txt file that should have come with this.
 *
 */
#include "PlatformPrecomp.h"
#include "Entity/EntityUtils.h"
#include "Irrlicht/IrrlichtManager.h"
#include "FileSystem/FileSystemZip.h"
#include "App.h"

#include "cocos2d.h"
#include "cocos-ext.h"
using namespace cocos2d;
using namespace cocos2d::extension;

#include "GUI/MainMenu.h"
#include "GUI/HelloWorldScene.h"
#include "GUI/GuiScroll.h"

#define CC_MAX_TOUCHES  5

App*				g_pApp = NULL;
static CCDictionary s_TouchesIntergerDict;
static CCTouch*		s_pTouches[CC_MAX_TOUCHES] = { NULL };

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
App::App()
{
	m_initagain		= 0;
	m_connect_set   = 0;
	m_indexBitsUsed = 0;

	m_bDidPostInit	= false;
	m_MenuEntity	= NULL;
}

App::~App()
{
	if( m_connect_set )
	{
		m_connect_set = 0;
				
		m_sig_unloadSurfaces.disconnect(boost::bind(&App::OnUnloadSurfaces, this));
		m_sig_loadSurfaces.disconnect(boost::bind(&App::OnReLoadSurfaces, this));
	}
		
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
	
    if( m_connect_set == 0 )
	{
		m_connect_set = 1;
				
		m_sig_unloadSurfaces.connect(1, boost::bind(&App::OnUnloadSurfaces, this));
		m_sig_loadSurfaces.connect(1, boost::bind(&App::OnReLoadSurfaces, this));
	}
	
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
	
	SetManualRotationMode(true);
	
	if ( !BaseApp::IsInitted() )
	{
		BaseApp::Init();
			    
		m_varDB.Load("save.dat", &bFileExisted);
		LogMsg("Save path is %s", GetSavePath().c_str());
	    
		//preload audio
		if( GetAudioManager() )
		{
			GetAudioManager()->Preload("audio/click.wav");
			//GetAudioManager()->Play("audio/real.mp3",1,1); //ios 128bps mp3
			//GetAudioManager()->Play("audio/real.ogg",1,1); //android play ogg
		}
		
		if( IrrlichtManager::GetIrrlichtManager()->Init() )
		{	
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
		}
	}
    
	return true;
}

/*void App::Kill()
{
	Entity::GetEntityManager()->RemoveAllEntities();
	IrrlichtManager::GetIrrlichtManager()->Kill();
	BaseApp::Kill();
}*/

void App::Update()
{
	BaseApp::Update();
    
	if (!m_bDidPostInit)
	{
		m_bDidPostInit	= true;
						
		//init for HelloWorld or GuiScroll
		CCScene* pScene = HelloWorld::scene();
		//CCScene* pScene = GuiScroll::scene();
		// run
		CCDirector::sharedDirector()->runWithScene(pScene);
		
		//build a GUI node
		Entity *pGUIEnt = Entity::GetEntityManager()->AddEntity(new Entity("GUI"));
		//by stone, highlevel shader used
		m_MenuEntity = MainMenuCreate(pGUIEnt);
	}
}

void App::Draw()
{
	this->isNeedInitAgain();
	
	//turn normal GL back on
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	IrrlichtManager::GetIrrlichtManager()->IsRunning(); //let it do its own update tick
	IrrlichtManager::GetIrrlichtManager()->BeginScene(); //turn on irrlicht's 3d mode renderstates
	IrrlichtManager::GetIrrlichtManager()->Render(); //render its scenegraph
	IrrlichtManager::GetIrrlichtManager()->EndScene(); //tell irrlicht to go into 2d mode renderstates
	
	BaseApp::Draw();
}

int App::isNeedInitAgain()
{
	irr::IrrlichtDevice*		pdevice = IrrlichtManager::GetIrrlichtManager()->GetDevice();
	irr::video::IVideoDriver*	pdriver = IrrlichtManager::GetIrrlichtManager()->GetDriver();
		
	if( m_initagain )
	{
		m_initagain = 0;

		CCTextureCache::sharedTextureCache()->removeAllTextures();
				
		m_MenuEntity->OnUnLoad();

		if (pdevice->getGUIEnvironment())
			pdevice->getGUIEnvironment()->OnUnLoad();
			
		pdriver->OnUnLoad();

		CCShaderCache::sharedShaderCache()->reloadDefaultShaders();
		pdriver->OnAgainDriverInit();
						
		pdriver->OnReLoad();
			
		if (pdevice->getGUIEnvironment())
			pdevice->getGUIEnvironment()->OnReLoad();

		m_MenuEntity->OnReLoad();

		CCTextureCache::sharedTextureCache()->reloadAllTextures();
	}

	return m_initagain;
}

void App::OnUnloadSurfaces()
{
	irr::video::IVideoDriver* pdriver = IrrlichtManager::GetIrrlichtManager()->GetDriver();
	
	if (pdriver)
	{
		LogMsg("Irrlicht unloading surfaces..");
	}
}

//m_sig_loadSurfaces trigger
void App::OnReLoadSurfaces()
{
	irr::video::IVideoDriver* pdriver = IrrlichtManager::GetIrrlichtManager()->GetDriver();
	
	if (pdriver)
	{
		LogMsg("Irrlicht loading surfaces..");

		m_initagain = 1;
	}
}

Entity*	App::GetMainScene() 
{
	return m_MenuEntity;
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

int App::getUnUsedIndex()
{
    int i;
    int temp = m_indexBitsUsed;

    for (i = 0; i < CC_MAX_TOUCHES; i++) 
	{
        if (! (temp & 0x00000001)) 
		{
            m_indexBitsUsed |= (1 <<  i);
            return i;
        }

        temp >>= 1;
    }

    // all bits are used
    return -1;
}

void App::removeUsedIndexBit(int index)
{
    if (index < 0 || index >= CC_MAX_TOUCHES) 
    {
        return;
    }

    unsigned int temp = 1 << index;
    temp = ~temp;
    m_indexBitsUsed &= temp;
}

void App::HandleTouchesBegin(int num, int ids[], float xs[], float ys[])
{
	int			i;
	int			id;
	int			nUnusedIndex;
    float		x;
    float		y;
	CCSet		set;
	CCTouch*	pTouch;
	CCInteger*	pIndex;
	CCInteger*	pInterObj;

    for (i = 0; i < num; ++i)
    {
        id	= ids[i];
        x	= xs[i];
        y	= ys[i];

        pIndex = (CCInteger*)s_TouchesIntergerDict.objectForKey(id);
        nUnusedIndex = 0;

        // it is a new touch
        if (pIndex == NULL)
        {
            nUnusedIndex = getUnUsedIndex();

            // The touches is more than MAX_TOUCHES ?
            if (nUnusedIndex == -1) 
			{
                CCLOG("The touches is more than MAX_TOUCHES, nUnusedIndex = %d", nUnusedIndex);
                continue;
            }

            pTouch = s_pTouches[nUnusedIndex] = new CCTouch();
			pTouch->setTouchInfo( nUnusedIndex, x, y );
                        
            pInterObj = new CCInteger(nUnusedIndex);
            s_TouchesIntergerDict.setObject(pInterObj, id);
            set.addObject(pTouch);
            pInterObj->release();
        }
    }

    if (set.count() > 0)
    {
        CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(&set, NULL);
    }
}

void App::HandleTouchesMove(int num, int ids[], float xs[], float ys[])
{
	int			i;
	int			id;
    float		x;
    float		y;
	CCSet		set;
	CCTouch*	pTouch;
	CCInteger*	pIndex;

    for (i = 0; i < num; ++i)
    {
        id	= ids[i];
        x	= xs[i];
        y	= ys[i];

        pIndex = (CCInteger*)s_TouchesIntergerDict.objectForKey(id);
        if (pIndex == NULL) 
		{
            CCLOG("if the index doesn't exist, it is an error");
            continue;
        }

        CCLOGINFO("Moving touches with id: %d, x=%f, y=%f", id, x, y);
        pTouch = s_pTouches[pIndex->getValue()];
        
		if (pTouch)
        {
			pTouch->setTouchInfo(pIndex->getValue(), x, y);
            set.addObject(pTouch);
        }
        else
        {
            // It is error, should return.
            CCLOG("Moving touches with id: %d error", id);
            return;
        }
    }

    if (set.count() > 0)
    {
        CCDirector::sharedDirector()->getTouchDispatcher()->touchesMoved(&set, NULL);
    }
}

void App::HandleTouchesEnd(int num, int ids[], float xs[], float ys[])
{
	int			i;
	int			id;
    float		x;
    float		y;
	CCSet		set;
	CCTouch*	pTouch;
	CCInteger*	pIndex;
	
	for (i = 0; i < num; ++i)
    {
        id	= ids[i];
        x	= xs[i];
        y	= ys[i];

        pIndex = (CCInteger*)s_TouchesIntergerDict.objectForKey(id);
        if (pIndex == NULL)
        {
            CCLOG("if the index doesn't exist, it is an error");
            continue;
        }
        
        pTouch = s_pTouches[pIndex->getValue()];        
		if (pTouch)
        {
            CCLOGINFO("Ending touches with id: %d, x=%f, y=%f", id, x, y);
			pTouch->setTouchInfo(pIndex->getValue(), x, y);

            set.addObject(pTouch);

            // release the object
            pTouch->release();
            s_pTouches[pIndex->getValue()] = NULL;
            removeUsedIndexBit(pIndex->getValue());

            s_TouchesIntergerDict.removeObjectForKey(id);
        } 
        else
        {
            CCLOG("Ending touches with id: %d error", id);
            return;
        } 

    }

    CCDirector::sharedDirector()->getTouchDispatcher()->touchesEnded(&set, NULL);
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
