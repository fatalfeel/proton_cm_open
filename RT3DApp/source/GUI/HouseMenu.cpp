#include "PlatformPrecomp.h"
#include "HouseMenu.h"
//#include "Entity/EntityUtils.h"
#include "MainMenu.h"
#include "Irrlicht/IrrlichtManager.h"
//#include "Component/FPSControlComponent.h"
#include "Component/EventControlComponent.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;


/*void HouseMenuOnSelect(VariantList *pVList) //0=vec2 point of click, 1=entity sent from
{
	Entity *pEntClicked = pVList->m_variant[1].GetEntity();

	LogMsg("Clicked %s entity at %s", pEntClicked->GetName().c_str(),pVList->m_variant[1].Print().c_str());

	if (pEntClicked->GetName() == "Back")
	{
		//SlideScreen(pEntClicked->GetParent(), false);
		MessageManager::GetMessageManager()->CallEntityFunction(pEntClicked->GetParent(), 500, "OnDelete", NULL);
		MainMenuCreate(pEntClicked->GetParent()->GetParent());
		IrrlichtManager::GetIrrlichtManager()->ClearScene();
		DestroyUnusedTextures();
	}

	Entity::GetEntityManager()->PrintTreeAsText(); //useful for debugging
}*/

void HouseInitScene()
{
	if (!IrrlichtManager::GetIrrlichtManager()->GetDevice())
	{
		LogError("Error initializing Irrlicht");
		return;
	}
	IrrlichtManager::GetIrrlichtManager()->GetDevice()->getTimer()->setTime(0);
	
    video::IVideoDriver*    driver = IrrlichtManager::GetIrrlichtManager()->GetDriver();
	scene::ISceneManager*   smgr   = IrrlichtManager::GetIrrlichtManager()->GetScene();
    IrrlichtDevice*         device = IrrlichtManager::GetIrrlichtManager()->GetDevice();
        
	smgr->getParameters()->setAttribute(B3D_LOADER_IGNORE_MIPMAP_FLAG, true);

	scene::IAnimatedMesh* mesh = smgr->getMesh((GetBaseAppPath()+"game/house_scene/house_scene.b3d").c_str());
	scene::IMeshSceneNode* node = 0;


	if (mesh)
	{
		//node = smgr->addOctTreeSceneNode(mesh->getMesh(0), 0, -1, 128);
		driver->setMinHardwareBufferVertexCount(64);

		mesh->setHardwareMappingHint(scene::EHM_STATIC);
		node = smgr->addMeshSceneNode(mesh->getMesh(0));
		node->setMaterialFlag(video::EMF_LIGHTING, false);
	}
    else
	{
		LogError("Aborting, can't load mesh");
		return;
	}

	node->setScale(core::vector3df(30,30,30));
	
	scene::ITriangleSelector* selector = 0;
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0, 100.0f, .3f, 0, 0, 0, true, 3.f);
	camera->setPosition(core::vector3df(242,72,-620)); //for prey level
	camera->setTarget(core::vector3df(-70,30,-160));

	if (node)
	{
		//custom collision mesh

		scene::IAnimatedMesh* colmesh = smgr->getMesh((GetBaseAppPath()+"game/house_scene/house_scene_col.b3d").c_str());
		scene::IMeshSceneNode* colnode = smgr->addMeshSceneNode(colmesh->getMesh(0));


		colnode->setScale(core::vector3df(30,30,30));

		//selector = smgr->createOctTreeTriangleSelector(
		//		colmesh, colnode, 128);
		selector = smgr->createTriangleSelector(colmesh, node);

		//node->setTriangleSelector(selector);
		colnode->setVisible(false);

		if (selector)
		{
			scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(selector, camera, core::vector3df(30,50,30), core::vector3df(0,-10,0), core::vector3df(0,30,0));
			selector->drop(); // As soon as we're done with the selector, drop it.
			camera->addAnimator(anim);
			anim->drop();  // And likewise, drop the animator when we're done referring to it.
		}
	}

	
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	scene::ISceneNode* skybox=smgr->addSkyBoxSceneNode(
		IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_up.jpg"),
		IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_dn.jpg"),
		IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_lf.jpg"),
		IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_rt.jpg"),
		IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_ft.jpg"),
		IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_bk.jpg"));

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	float fov = float(GetPrimaryGLX())/ float(GetPrimaryGLY());
	LogMsg("Setting fov to %.2f", fov);
	camera->setAspectRatio(fov);
	camera->setFOV((120 * M_PI / 360.0f));

#ifdef _IRR_COMPILE_WITH_GUI_
    EventControlComponent* receiver = new EventControlComponent();
    receiver->AddGuiButton();
	device->setEventReceiver(receiver);
#endif
}

Entity * HouseMenuCreate(Entity *pParentEnt)
{
	Entity *pBG = pParentEnt->AddEntity(new Entity("HouseMenu"));

	//AddFocusIfNeeded(pBG);

	/*Entity *pButtonEntity;
	pButtonEntity = CreateTextButtonEntity(pBG, "Back", iPhoneMapX(240),iPhoneMapY(290), "Back"); 
	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->m_sig_function.connect(&HouseMenuOnSelect);
	pButtonEntity->GetVar("alignment")->Set(uint32(ALIGNMENT_CENTER));
    */

	//setup movement button
	//Entity *pEnt = pBG->AddEntity(new Entity("controls", new FPSControlComponent));

	//SlideScreen(pBG, true);

	HouseInitScene();
	return pBG;
}

/*Entity* MainMenuCreate(Entity *pParentEnt)
{
    Entity *pBG = HouseMenuCreate(pParentEnt);
    return pBG;
}*/

