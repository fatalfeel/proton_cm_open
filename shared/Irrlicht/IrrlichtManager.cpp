#include "IrrlichtManager.h"
//#include "Renderer/Surface.h"
#include "BaseApp.h"
#include "App.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;

//poor man's singleton
/*IrrlichtManager g_irrlichtManager;

IrrlichtManager* GetIrrlichtManager()
{
	return &g_irrlichtManager;
}*/

//by jesse stone
static IrrlichtManager* irr_manager_instance = NULL;

IrrlichtManager* IrrlichtManager::GetIrrlichtManager()
{
	if (!irr_manager_instance)
		irr_manager_instance = new IrrlichtManager;
	return irr_manager_instance;
}

void IrrlichtManager::Free()
{
	if( irr_manager_instance )
	{
		delete irr_manager_instance;
		irr_manager_instance = NULL;
	}
}

IrrlichtManager::IrrlichtManager()
{
	m_bLightingEnabled = false;
	m_pDevice = NULL;
	m_pScene = NULL;
	m_pDriver = NULL;
#ifdef RT_IRRBULLET
	m_pWorld = NULL;
#endif
	m_bBulletPhysicsEnabled = true; //only applicable if RT_IRRBULLET was defined

	m_bDebugEnabled = false;
	m_againInit		= 0;
	m_connect_set   = 0;
}

//by jesse stone
IrrlichtManager::~IrrlichtManager()
{
	if( m_connect_set )
	{
		m_connect_set = 0;
				
		BaseApp::GetBaseApp()->m_sig_unloadSurfaces.disconnect(boost::bind(&IrrlichtManager::OnUnloadSurfaces, this));
		BaseApp::GetBaseApp()->m_sig_loadSurfaces.disconnect(boost::bind(&IrrlichtManager::OnReLoadSurfaces, this));
	}
		
	Kill();
}


void IrrlichtManager::Kill()
{
	//LogMsg("Killing irrlicht");

	if (m_pDevice)
	{
#ifdef RT_IRRBULLET
		if (m_pWorld)
		{
			delete m_pWorld;
		}
#endif
		m_pDevice->drop();
		m_pDevice = NULL;
		m_pDriver = NULL;
		m_pScene = NULL;
	}
}

//by jesse stone
bool IrrlichtManager::Init(irr::IEventReceiver *pEventReceiver)
{
	LogMsg("initting irrlicht");

	bool bStencilBuffer = true;
	
    //E_DRIVER_TYPE driverType = video::EDT_OGLES1;
    E_DRIVER_TYPE driverType = AppGetOGLESType(); //by stone

#ifdef C_GL_MODE
	driverType = video::EDT_OPENGL;
	bStencilBuffer = true;
#endif
	
	m_pDevice = createDevice( driverType, dimension2d<u32>(GetPrimaryGLX(), GetPrimaryGLY()), 16, false, bStencilBuffer, false, pEventReceiver);
  
	if (!m_pDevice)
	{
		LogError("Unable to create video driver");
		return false;
	}

	
	m_pDriver = m_pDevice->getVideoDriver();
	m_pScene = m_pDevice->getSceneManager();

	if (!m_pDevice->getFileSystem()->addFileArchive("", true, false, EFAT_PROTON))
	{
		LogMsg("Unable to mount Proton filesystem");
	}
	
	if( m_connect_set == 0 )
	{
		m_connect_set = 1;
				
		BaseApp::GetBaseApp()->m_sig_unloadSurfaces.connect(1, boost::bind(&IrrlichtManager::OnUnloadSurfaces, this));
		BaseApp::GetBaseApp()->m_sig_loadSurfaces.connect(1, boost::bind(&IrrlichtManager::OnReLoadSurfaces, this));
	}
	
	LogMsg("Irrlicht initted");


	if (!m_bBulletPhysicsEnabled) return true;
#ifdef RT_IRRBULLET

	////////////////////////////
	// Create irrBullet World //
	////////////////////////////

	assert(!m_pWorld);
	m_pWorld = createIrrBulletWorld(m_pDevice, true, true);
	m_pWorld->setGravity(vector3df(0,-10,0));

#ifdef _DEBUG
	m_pWorld->setDebugMode(EPDM_DrawAabb | EPDM_DrawContactPoints);
	

#endif
#endif


	return true;
}

irr::video::ITexture * IrrlichtManager::GetTexture(const std::string &fileName)
{

	return m_pDriver->getTexture( (GetBaseAppPath() + fileName).c_str() );
}

void IrrlichtManager::ClearScene()
{
	video::SMaterial vm;
	
	if (m_pScene)
	{
	
#ifdef RT_IRRBULLET
		if (m_bBulletPhysicsEnabled)
		{
			while(m_pWorld->getNumCollisionObjects() > 0)
			{
				m_pWorld->removeCollisionObject(m_pWorld->getCollisionObject(0));
			}
		}
#endif
		
		m_pScene->getRootSceneNode()->removeAll();
		m_pScene->getMeshCache()->clear(); 
		m_pScene->clear();	
		//const video::SMaterial m;
		m_pDriver->setMaterial(vm); 
		m_pDriver->removeAllTextures();
	}
}

void IrrlichtManager::BeginScene()
{
	if (m_pDriver)
	{
		//this really isn't needed as Proton clears the zbuffer/etc
		m_pDriver->beginScene(false, false, SColor(255,100,101,140));
	
#ifdef RT_IRRBULLET
		
		if (m_pWorld)
		{
            // Step the simulation with our delta time
            m_pWorld->stepSimulation(BaseApp::GetBaseApp()->GetGameDelta()*0.02f, 120);
		}
#endif
	}
}

void IrrlichtManager::Render()
{

	if (m_pScene)
	{
		m_pScene->drawAll();

#ifdef RT_IRRBULLET
		if (m_bDebugEnabled && m_pWorld)
		{
			m_pWorld->debugDrawWorld(true);
		}
#endif
        //by stone
		if (m_pDevice->getGUIEnvironment())
            m_pDevice->getGUIEnvironment()->drawAll();
        
        CHECK_GL_ERROR();
	}
}

/*void IrrlichtManager::RenderDebugProperty2D()
{
#ifdef RT_IRRBULLET
	if (m_pWorld)
	{
		BaseApp::GetBaseApp()->GetFont(FONT_SMALL)->Draw(5,60, m_pWorld->debugDrawProperties(true).c_str());
	}
#endif
}*/

void IrrlichtManager::EndScene()
{
    if (m_pDriver)
	{
        m_pDriver->endScene();
	}
}

bool IrrlichtManager::IsRunning()
{
	if (m_pDriver)
	{
        return m_pDevice->run();
	}

	return false;
}

int IrrlichtManager::isNeedInitAgain()
{
	if( m_againInit )
	{
		m_againInit = 0;

		GetApp()->GetMainScene()->OnUnLoad();

		if (m_pDevice->getGUIEnvironment())
			m_pDevice->getGUIEnvironment()->OnUnLoad();
			
			m_pDriver->OnUnLoad();

			m_pDriver->OnAgainDriverInit();	
						
			m_pDriver->OnReLoad();

		if (m_pDevice->getGUIEnvironment())
			m_pDevice->getGUIEnvironment()->OnReLoad();

		GetApp()->GetMainScene()->OnReLoad();
	}

	return m_againInit;
}

core::rect<s32> CLRectToIrrlichtRect32(CL_Rectf clR)
{
	core::rect<s32> r;
	r.UpperLeftCorner.X = (irr::s32) clR.left;
	r.UpperLeftCorner.Y = (irr::s32) clR.top;
	r.LowerRightCorner.X = (irr::s32) clR.right;
	r.LowerRightCorner.Y = (irr::s32) clR.bottom;
	return r;
}

core::vector3df GetVectorHeadingFromNode(scene::ISceneNode *pNode)
{
	vector3df v = core::vector3df(0,0,-1);
	matrix4 m = pNode->getAbsoluteTransformation();
	m.setTranslation(core::vector3df(0,0,0));
	m.transformVect(v);
	v.normalize();
	return v;
}

core::vector3df RotatePositionByDirectionalVector(core::vector3df vPos, core::vector3df vNormal )
{
	//OPTIMIZE Isn't there a much faster way to do this?

	//calculate rotated z
	core::vector3df vFinal = vNormal * vPos.Z;

	//calculate rotation x
	vFinal = vFinal + (vNormal.crossProduct(core::vector3df(0,1,0)) * vPos.X);

	//y will just be up.. yeah, not really right
	vFinal.Y += vPos.Y;
	return vFinal;


}

float Vector3DToAngleRadians(core::vector3df v)
{
	return atan2(v.X, -v.Z);
}

std::string PrintVector3(core::vector3df v)
{
	return PrintVector3(ToCLVector3(v));
}

void IrrlichtManager::SetReSize(core::dimension2d<u32> size)
{
	if (m_pDriver)
	{
		m_pDriver->OnResize(size);
	}
}

void IrrlichtManager::OnUnloadSurfaces()
{
	if (m_pScene && m_pDriver)
	{
		LogMsg("Irrlicht unloading surfaces..");
	}
}

void IrrlichtManager::OnReLoadSurfaces()
{
	if (m_pScene && m_pDriver)
	{
		LogMsg("Irrlicht loading surfaces..");

		m_againInit = 1;
	}
}
