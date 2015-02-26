#include "PlatformPrecomp.h"
#include "Mesh3DMenu.h"
//#include "Entity/EntityUtils.h"
#include "MainMenu.h"
#include "Irrlicht/IrrlichtManager.h"
#include "driverChoice.h"
#include "Component/EventControlComponent.h"
#include "FileSystem/FileSystemZip.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

/*void Mesh3DMenuOnSelect(VariantList *pVList) //0=vec2 point of click, 1=entity sent from
{
	Entity *pEntClicked = pVList->m_variant[1].GetEntity();

	LogMsg("Clicked %s entity at %s", pEntClicked->GetName().c_str(),pVList->m_variant[1].Print().c_str());

	if (pEntClicked->GetName() == "Back")
	{
		//SlideScreen(pEntClicked->GetParent(), false);
		MessageManager::GetMessageManager()->CallEntityFunction(pEntClicked->GetParent(), 500, "OnDelete", NULL);
		MainMenuCreate(pEntClicked->GetParent()->GetParent());

		IrrlichtManager::GetIrrlichtManager()->ClearScene();
	}

	Entity::GetEntityManager()->PrintTreeAsText(); //useful for debugging
}*/

void StencilInitScene()
{
	IAnimatedMesh*				mesh;
	scene::ISceneNode*			s_node;
	IAnimatedMeshSceneNode*     a_node;
	scene::ISceneNodeAnimator*  anim;
	scene::ISceneManager*		smgr   = IrrlichtManager::GetIrrlichtManager()->GetScene();
	IrrlichtDevice*				device = IrrlichtManager::GetIrrlichtManager()->GetDevice();
	video::IVideoDriver*		driver = IrrlichtManager::GetIrrlichtManager()->GetDriver();
		
	if (!IrrlichtManager::GetIrrlichtManager()->GetDevice())
	{
		LogError("Error initializing Irrlicht");
		return;
	}
	
	IrrlichtManager::GetIrrlichtManager()->GetDevice()->getTimer()->setTime(0);
		
    mesh = smgr->getMesh((GetBaseAppPath()+"game/room.3ds").c_str());

	smgr->getMeshManipulator()->makePlanarTextureMapping(mesh->getMesh(0), 0.004f);
	
	s_node = smgr->addAnimatedMeshSceneNode(mesh);
	s_node->setMaterialTexture(0, driver->getTexture((GetBaseAppPath()+"game/wall.jpg").c_str()));
	s_node->getMaterial(0).SpecularColor.set(0,0,0,0);


	mesh = smgr->addHillPlaneMesh( "myHill",
                                  core::dimension2d<f32>(20,20),
                                  core::dimension2d<u32>(40,40), 0, 0,
                                  core::dimension2d<f32>(0,0),
                                  core::dimension2d<f32>(10,10));
    
	s_node = smgr->addWaterSurfaceSceneNode(mesh->getMesh(0), 3.0f, 300.0f, 30.0f);
	s_node->setPosition(core::vector3df(0,7,0));
    
	s_node->setMaterialTexture(0, driver->getTexture((GetBaseAppPath()+"game/stones.jpg").c_str()));
	s_node->setMaterialTexture(1, driver->getTexture((GetBaseAppPath()+"game/water.jpg").c_str()));
    
	s_node->setMaterialType(video::EMT_REFLECTION_2_LAYER);


	// create light
	s_node = smgr->addLightSceneNode(0, core::vector3df(0,0,0), video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 800.0f);
	anim = smgr->createFlyCircleAnimator (core::vector3df(0,150,0),250.0f);
	s_node->addAnimator(anim);
	anim->drop();

	// attach billboard to light

	s_node = smgr->addBillboardSceneNode(s_node, core::dimension2d<f32>(50, 50));
	s_node->setMaterialFlag(video::EMF_LIGHTING, false);
	s_node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	s_node->setMaterialTexture(0, driver->getTexture((GetBaseAppPath()+"game/particlewhite.bmp").c_str()));

	/*
	Next we add a volumetric light s_node, which adds a glowing fake area light to
	the scene. Like with the billboards and particle systems we also assign a
	texture for the desired effect, though this time we'll use a texture animator
	to create the illusion of a magical glowing area effect.
	*/
	scene::IVolumeLightSceneNode* n = 
	smgr->addVolumeLightSceneNode(	0, 
									-1,
									32,                              // Subdivisions on U axis
									32,                              // Subdivisions on V axis
									video::SColor(0, 255, 255, 255), // foot color
									video::SColor(0, 0, 0, 0));      // tail color

	if (n)
	{
		n->setScale(core::vector3df(56.0f, 56.0f, 56.0f));
		n->setPosition(core::vector3df(-120,50,40));

		// load textures for animation
		core::array<video::ITexture*> textures;
		for (s32 g=7; g > 0; --g)
		{
			core::stringc tmp;
			tmp = (GetBaseAppPath()+"game/portal").c_str();
			tmp += g;
			tmp += ".bmp";
			video::ITexture* t = driver->getTexture( tmp.c_str() );
			textures.push_back(t);
		}

		// create texture animator
		scene::ISceneNodeAnimator* glow = smgr->createTextureAnimator(textures, 150);

		// add the animator
		n->addAnimator(glow);

		// drop the animator because it was created with a create() function
		glow->drop();
	}

	
	// create a particle system
	scene::IParticleSystemSceneNode* ps = smgr->addParticleSystemSceneNode(false);

	scene::IParticleEmitter* em = 
	ps->createBoxEmitter(	core::aabbox3d<f32>(-7,0,-7,7,1,7), // emitter size
							core::vector3df(0.0f,0.06f,0.0f),   // initial direction
							80,100,                             // emit rate
							video::SColor(0,255,255,255),       // darkest color
							video::SColor(0,255,255,255),       // brightest color
							800,2000,0,                         // min and max age, angle
							core::dimension2df(10.f,10.f),         // min size
							core::dimension2df(20.f,20.f)  );        // max size
		

	ps->setEmitter(em); // this grabs the emitter
	em->drop(); // so we can drop it here without deleting it

	scene::IParticleAffector* paf = ps->createFadeOutParticleAffector();
	ps->addAffector(paf); // same goes for the affector
	paf->drop();

	ps->setPosition(core::vector3df(-70,60,40));
	ps->setScale(core::vector3df(2,2,2));
	ps->setMaterialFlag(video::EMF_LIGHTING, false);
	ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	ps->setMaterialTexture(0, driver->getTexture((GetBaseAppPath()+"game/fire.bmp").c_str()));
	ps->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

		
	// add animated character
	mesh = smgr->getMesh((GetBaseAppPath()+"game/dwarf.x").c_str());

	a_node = smgr->addAnimatedMeshSceneNode(mesh);
	a_node->setPosition(core::vector3df(-50,20,-60));
	a_node->setAnimationSpeed(15);

	// add shadow
	a_node->addShadowVolumeSceneNode();
	smgr->setShadowColor(video::SColor(150,0,0,0));

	// make the model a little bit bigger and normalize its normals
	// because of the scaling, for correct lighting
	a_node->setScale(core::vector3df(2,2,2));
	a_node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);

	/*
	Finally we simply have to draw everything, that's all.
	*/

	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(core::vector3df(-50,50,-350));
	camera->setFarValue(10000.0f); // this increase a shadow visible range.

//////////////////
	////////////////cam/////////////////////////////////////////////    
#ifdef _IRR_COMPILE_WITH_GUI_
	EventControlComponent* receiver = new EventControlComponent();
    receiver->AddGuiButton();
	device->setEventReceiver(receiver);
#endif
}

Entity* StencilMenuCreate(Entity *pParentEnt)
{
	Entity *pBG = pParentEnt->AddEntity(new Entity("Mesh3DMenu"));

	//AddFocusIfNeeded(pBG);
	
	/*Entity *pButtonEntity;
	pButtonEntity = CreateTextButtonEntity(pBG, "Back", iPhoneMapX(240),iPhoneMapY(290), "Back"); 

	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->m_sig_function.connect(&Mesh3DMenuOnSelect);
	pButtonEntity->GetVar("alignment")->Set(uint32(ALIGNMENT_CENTER));

	SlideScreen(pBG, true);*/
	
	StencilInitScene();
	return pBG;
}

//by stone test bug
/*Entity* MainMenuCreate(Entity *pParentEnt)
{
    Entity *pBG = Mesh3DMenuCreate(pParentEnt);
    return pBG;
}*/
