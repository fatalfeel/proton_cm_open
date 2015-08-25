#include "PlatformPrecomp.h"
#include "Irrlicht/IrrlichtManager.h"
using namespace irr;

#include "MainMenu.h"
#include "ShaderMenu.h"
#include "Component/EventControlComponent.h"

bool UseHighLevelShaders = true;

class MyShaderCallBack : public video::IShaderConstantSetCallBack
{
public:
	MyShaderCallBack() : WorldViewProjID(-1), TransWorldID(-1), InvWorldID(-1), PositionID(-1),
    ColorID(-1), TextureID(-1), FirstUpdate(true)
	{
	}

	~MyShaderCallBack()
	{
	}
    
	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
	{
		video::IVideoDriver* driver = services->getVideoDriver();
        
		// get shader constants id.
        
		if (UseHighLevelShaders && FirstUpdate)
		{
			WorldViewProjID = services->getVertexShaderConstantID("mWorldViewProj");
			TransWorldID = services->getVertexShaderConstantID("mTransWorld");
			InvWorldID = services->getVertexShaderConstantID("mInvWorld");
			PositionID = services->getVertexShaderConstantID("mLightPos");
			ColorID = services->getVertexShaderConstantID("mLightColor");
            
			// Textures ID are important only for OpenGL interface.
            
			if(driver->getDriverType() == video::EDT_OPENGL)
				TextureID = services->getVertexShaderConstantID("myTexture");
            
			FirstUpdate = false;
		}
        
		// set inverted world matrix
		// if we are using highlevel shaders (the user can select this when
		// starting the program), we must set the constants by name.
        IrrlichtDevice* device;
		s32				TextureLayerID;
		core::vector3df pos;
		core::matrix4	worldViewProj;
		core::matrix4	world;
		core::matrix4	invWorld = driver->getTransform(video::ETS_WORLD);
		//invWorld.makeInverse(); //move light, you can open it
        
		if (UseHighLevelShaders)
			services->setVertexShaderConstant(InvWorldID, invWorld.pointer(), 16);
		else
			services->setVertexShaderConstant(invWorld.pointer(), 0, 4);
        
		// set clip matrix
		worldViewProj  = driver->getTransform(video::ETS_PROJECTION);
		worldViewProj *= driver->getTransform(video::ETS_VIEW);
		worldViewProj *= driver->getTransform(video::ETS_WORLD);
        
		if (UseHighLevelShaders)
			services->setVertexShaderConstant(WorldViewProjID, worldViewProj.pointer(), 16);
		else
			services->setVertexShaderConstant(worldViewProj.pointer(), 4, 4);
        
		// set camera position
        device	= IrrlichtManager::GetIrrlichtManager()->GetDevice();
		pos		= device->getSceneManager()->getActiveCamera()->getAbsolutePosition();
        
		if (UseHighLevelShaders)
			services->setVertexShaderConstant(PositionID, reinterpret_cast<f32*>(&pos), 3);
		else
			services->setVertexShaderConstant(reinterpret_cast<f32*>(&pos), 8, 1);
        
		// set light color
        
		video::SColorf col(0.0f,1.0f,1.0f,0.0f);
        
		if (UseHighLevelShaders)
			services->setVertexShaderConstant(ColorID, reinterpret_cast<f32*>(&col), 4);
		else
			services->setVertexShaderConstant(reinterpret_cast<f32*>(&col), 9, 1);
        
		// set transposed world matrix
		world = driver->getTransform(video::ETS_WORLD);
		world = world.getTransposed();
        
		if (UseHighLevelShaders)
		{
			services->setVertexShaderConstant(TransWorldID, world.pointer(), 16);
            
			// set texture, for textures you can use both an int and a float setPixelShaderConstant interfaces (You need it only for an OpenGL driver).
			TextureLayerID = 0;
			services->setPixelShaderConstant(TextureID, &TextureLayerID, 1);
		}
		else
		{
			services->setVertexShaderConstant(world.pointer(), 10, 4);
		}
	}
    
private:
	s32 WorldViewProjID;
	s32 TransWorldID;
	s32 InvWorldID;
	s32 PositionID;
	s32 ColorID;
	s32 TextureID;
	bool FirstUpdate;
};

CShaderMenu::CShaderMenu()
{
}

CShaderMenu::~CShaderMenu()
{
}

void CShaderMenu::InitScene()
{
	if (!IrrlichtManager::GetIrrlichtManager()->GetDevice())
	{
		LogError("Error initializing Irrlicht");
		return;
	}
	
    video::IVideoDriver*    driver = IrrlichtManager::GetIrrlichtManager()->GetDriver();
	scene::ISceneManager*   smgr   = IrrlichtManager::GetIrrlichtManager()->GetScene();
    IrrlichtDevice*         device = IrrlichtManager::GetIrrlichtManager()->GetDevice();
    gui::IGUIEnvironment*   gui    = device->getGUIEnvironment();
    video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
    //scene::ISceneNode*			node;
	scene::ISceneNodeAnimator*  anim;
	MyShaderCallBack* mc_A;
	MyShaderCallBack* mc_B;

	s32 newMaterialType1 = 0;
	s32 newMaterialType2 = 0;
    
#if defined(_IRR_COMPILE_WITH_OPENGL_)
	io::path vsFileName = "game/opengl.vert";
	io::path psFileName = "game/opengl.frag";
#else
	io::path vsFileName = "game/ogles2.vert";
	io::path psFileName = "game/ogles2.frag";
#endif
		            
	if (gpu)
	{
        video::E_GPU_SHADING_LANGUAGE shadingLanguage = video::EGSL_DEFAULT;
        
        // create material from high level shaders (hlsl, glsl or cg)
		mc_A = new MyShaderCallBack();
        newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(vsFileName,
                                                                    "vertexMain",
                                                                    video::EVST_VS_1_1,
                                                                    psFileName,
                                                                    "pixelMain",
                                                                    video::EPST_PS_1_1,
                                                                    mc_A,
                                                                    video::EMT_SOLID,
                                                                    0,
                                                                    shadingLanguage);
		mc_A->drop();
		
		mc_B = new MyShaderCallBack();
        newMaterialType2 = gpu->addHighLevelShaderMaterialFromFiles(vsFileName,
                                                                    "vertexMain",
                                                                    video::EVST_VS_1_1,
                                                                    psFileName,
                                                                    "pixelMain",
                                                                    video::EPST_PS_1_1,
                                                                    mc_B,
                                                                    video::EMT_TRANSPARENT_ADD_COLOR,
                                                                    0,
                                                                    shadingLanguage);
        
		mc_B->drop();
                
        ////////// create test scene node 2, with the new created material type 1
        m_node_a = smgr->addCubeSceneNode(50);
        m_node_a->setPosition(core::vector3df(0,0,0));
        m_node_a->setMaterialTexture(0, driver->getTexture((GetBaseAppPath()+"game/wall.bmp").c_str()));
        m_node_a->setMaterialFlag(video::EMF_LIGHTING, false);
        m_node_a->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType1);

        anim = smgr->createRotationAnimator(core::vector3df(0,0.3f,0));
        m_node_a->addAnimator(anim);
        anim->drop();
        
       
        ////////// create test scene node 2, with the new created material type 2
        m_node_b = smgr->addCubeSceneNode(50);
        m_node_b->setPosition(core::vector3df(0,-10,50));
        m_node_b->setMaterialTexture(0, driver->getTexture((GetBaseAppPath()+"game/wall.bmp").c_str()));
        m_node_b->setMaterialFlag(video::EMF_LIGHTING, false);
        m_node_b->setMaterialFlag(video::EMF_BLEND_OPERATION, true);
        m_node_b->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType2);

                
        anim = smgr->createRotationAnimator(core::vector3df(0,0.3f,0));
        m_node_b->addAnimator(anim);
        anim->drop();
        
        
        /////////// no shader
        m_node_c = smgr->addCubeSceneNode(50);
        m_node_c->setPosition(core::vector3df(0,50,25));
        m_node_c->setMaterialTexture(0, driver->getTexture((GetBaseAppPath()+"game/wall.bmp").c_str()));
        m_node_c->setMaterialFlag(video::EMF_LIGHTING, false);
        
        anim = smgr->createRotationAnimator(core::vector3df(0,0.3f,0));
        m_node_c->addAnimator(anim);
        anim->drop();

		///////////////
#ifdef _IRR_COMPILE_WITH_GUI_
		m_textA = smgr->addTextSceneNode(gui->getBuiltInFont(), L"PS & VS & EMT_SOLID", video::SColor(255,255,255,255),			m_node_a);
		m_textB = smgr->addTextSceneNode(gui->getBuiltInFont(), L"PS & VS & EMT_TRANSPARENT", video::SColor(255,255,255,255),	m_node_b);
		m_textC = smgr->addTextSceneNode(gui->getBuiltInFont(), L"NO SHADER", video::SColor(255,255,255,255),					m_node_c);
#endif
        ////////////// add a nice skybox
        driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
        
        /*smgr->addSkyBoxSceneNode(driver->getTexture("game/irrlicht2_up.jpg"),
                                 driver->getTexture("game/irrlicht2_dn.jpg"),
                                 driver->getTexture("game/irrlicht2_lf.jpg"),
                                 driver->getTexture("game/irrlicht2_rt.jpg"),
                                 driver->getTexture("game/irrlicht2_ft.jpg"),
                                 driver->getTexture("game/irrlicht2_bk.jpg"));*/
        smgr->addSkyBoxSceneNode(
                                 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_up.jpg"),
                                 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_dn.jpg"),
                                 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_lf.jpg"),
                                 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_rt.jpg"),
                                 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_ft.jpg"),
                                 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_bk.jpg"));
        
        driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);
        
        scene::ICameraSceneNode* pcam = smgr->addCameraSceneNodeFPS();
        pcam->setPosition(core::vector3df(-150,100,-150));
        pcam->setTarget(core::vector3df(0,0,0));
    }

#ifdef _IRR_COMPILE_WITH_GUI_
	EventControlComponent* receiver = new EventControlComponent();
    receiver->AddGuiButton();
	device->setEventReceiver(receiver);
#endif

}

void CShaderMenu::OnUnLoad()
{
#ifdef _IRR_COMPILE_WITH_GUI_	
	m_textA->remove();
	m_textB->remove();
	m_textC->remove();
#endif
}

void CShaderMenu::OnReLoad()
{
	video::IVideoDriver*			driver	= IrrlichtManager::GetIrrlichtManager()->GetDriver();
    IrrlichtDevice*					device	= IrrlichtManager::GetIrrlichtManager()->GetDevice();
	scene::ISceneManager*			smgr	= IrrlichtManager::GetIrrlichtManager()->GetScene();
	gui::IGUIEnvironment*			gui		= device->getGUIEnvironment();
    video::IGPUProgrammingServices* gpu		= driver->getGPUProgrammingServices();
	
	s32 newMaterialType1 = 0;
	s32 newMaterialType2 = 0;

	MyShaderCallBack* mc_A;
	MyShaderCallBack* mc_B;
    
#if defined(_IRR_COMPILE_WITH_OPENGL_)
	io::path vsFileName = (GetBaseAppPath() + "game/opengl.vert").c_str();
	io::path psFileName = (GetBaseAppPath() + "game/opengl.frag").c_str();
#else
	io::path vsFileName = (GetBaseAppPath() + "game/ogles2.vert").c_str();
	io::path psFileName = (GetBaseAppPath() + "game/ogles2.frag").c_str();
#endif

	if (gpu)
	{
        video::E_GPU_SHADING_LANGUAGE shadingLanguage = video::EGSL_DEFAULT;
        
        // create material from high level shaders (hlsl, glsl or cg)
		mc_A = new MyShaderCallBack();
        newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(vsFileName,
                                                                    "vertexMain",
                                                                    video::EVST_VS_1_1,
                                                                    psFileName,
                                                                    "pixelMain",
                                                                    video::EPST_PS_1_1,
                                                                    mc_A,
                                                                    video::EMT_SOLID,
                                                                    0,
                                                                    shadingLanguage);
		mc_A->drop();
		
		mc_B = new MyShaderCallBack();
        newMaterialType2 = gpu->addHighLevelShaderMaterialFromFiles(vsFileName,
                                                                    "vertexMain",
                                                                    video::EVST_VS_1_1,
                                                                    psFileName,
                                                                    "pixelMain",
                                                                    video::EPST_PS_1_1,
                                                                    mc_B,
                                                                    video::EMT_TRANSPARENT_ADD_COLOR,
                                                                    0,
                                                                    shadingLanguage);
        
		mc_B->drop();

		m_node_a->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType1);
		m_node_b->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType2);

#ifdef _IRR_COMPILE_WITH_GUI_        
		m_textA = smgr->addTextSceneNode(gui->getBuiltInFont(), L"PS & VS & EMT_SOLID", video::SColor(255,255,255,255),			m_node_a);
		m_textB = smgr->addTextSceneNode(gui->getBuiltInFont(), L"PS & VS & EMT_TRANSPARENT", video::SColor(255,255,255,255),	m_node_b);
		m_textC = smgr->addTextSceneNode(gui->getBuiltInFont(), L"NO SHADER", video::SColor(255,255,255,255),					m_node_c);
#endif
	}
}

Entity* ShaderMenuCreate(Entity *pParentEnt)
{
	//Entity* pBG = pParentEnt->AddEntity(new Entity("ShaderMenu"));
	//AddFocusIfNeeded(pBG);
	//ShaderInitScene();
	
	CShaderMenu*	shaderscene = new CShaderMenu();
	Entity*			pBG			= pParentEnt->AddEntity(shaderscene);
	
	shaderscene->InitScene();
	
	return pBG;
}
