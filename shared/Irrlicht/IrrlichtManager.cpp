#include "IrrlichtManager.h"
#include "BaseApp.h"
#include "App.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;

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
	
	m_pDevice	= NULL;
	m_pScene	= NULL;
	m_pDriver	= NULL;
#ifdef RT_IRRBULLET
	m_pWorld	= NULL;
#endif
	
	m_bDebugEnabled = false;
}

IrrlichtManager::~IrrlichtManager()
{
	if (m_pDevice)
	{
#ifdef RT_IRRBULLET
		if (m_pWorld)
		{
			delete m_pWorld;
		}
#endif
		m_pDevice->drop();
		m_pDevice	= NULL;
		m_pDriver	= NULL;
		m_pScene	= NULL;
	}
}

//by jesse stone
bool IrrlichtManager::Init()
{
	LogMsg("initting irrlicht");

	bool					bStencilBuffer	= true;
	irr::IEventReceiver*	pEventReceiver	= NULL;
	    
    E_DRIVER_TYPE driverType = AppGetOGLESType(); //by stone

/*#ifdef C_GL_MODE
	driverType = video::EDT_OPENGL;
#endif*/
	
	m_pDevice = createDevice(	driverType, 
								dimension2d<u32>(GetPrimaryGLX(), GetPrimaryGLY()), 
								16, 
								false, 
								bStencilBuffer, 
								false, 
								pEventReceiver	);
  
	if (!m_pDevice)
	{
		LogError("Unable to create video driver");
		return false;
	}
	
	m_pDriver	= m_pDevice->getVideoDriver();
	m_pScene	= m_pDevice->getSceneManager();

	if (!m_pDevice->getFileSystem()->addFileArchive("", true, false, EFAT_PROTON))
	{
		LogMsg("Unable to mount Proton filesystem");
	}
		
	//if (!m_bBulletPhysicsEnabled) 
	//	return true;

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

	LogMsg("Irrlicht init done");

	return true;
}

void IrrlichtManager::BeginScene()
{
	if (m_pDriver)
	{
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

irr::video::ITexture* IrrlichtManager::GetTexture(const std::string &fileName)
{
	return m_pDriver->getTexture( (GetBaseAppPath()+fileName).c_str() );
}

void IrrlichtManager::SetReSize(core::dimension2d<u32> size)
{
	if (m_pDriver)
	{
		m_pDriver->OnResize(size);
	}
}

/////////
///////
/////
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
