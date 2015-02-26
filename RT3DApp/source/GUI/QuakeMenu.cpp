#include "PlatformPrecomp.h"
#include "QuakeMenu.h"
//#include "Entity/EntityUtils.h"
#include "MainMenu.h"
#include "Irrlicht/IrrlichtManager.h"
//#include "Component/FPSControlComponent.h"
#include "FileSystem/FileSystemZip.h"
#include "Component/EventControlComponent.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;


/*void QuakeMenuOnSelect(VariantList *pVList) //0=vec2 point of click, 1=entity sent from
{
	Entity *pEntClicked = pVList->m_variant[1].GetEntity();

	LogMsg("Clicked %s entity at %s", pEntClicked->GetName().c_str(),pVList->m_variant[1].Print().c_str());

	if (pEntClicked->GetName() == "Back")
	{
		//SlideScreen(pEntClicked->GetParent(), false);
		MessageManager::GetMessageManager()->CallEntityFunction(pEntClicked->GetParent(), 500, "OnDelete", NULL);
		MainMenuCreate(pEntClicked->GetParent()->GetParent());
		IrrlichtManager::GetIrrlichtManager()->ClearScene();
		
		//put it back to how it was.  Major hack.  For a real game, set up the data so we don't need to change directories.

			#ifdef WIN32
				//Hack to change the working directory back to what it was
					IrrlichtManager::GetIrrlichtManager()->GetDevice()->getFileSystem()->changeWorkingDirectoryTo( "..\\..");
			#else
					IrrlichtManager::GetIrrlichtManager()->GetDevice()->getFileSystem()->changeWorkingDirectoryTo(GetBaseAppPath().c_str());
			#endif
		
			FileSystemZip *pFileSystem = (FileSystemZip*)FileManager::GetFileManager()->GetFileSystem(0);
			if (pFileSystem) pFileSystem->SetRootDirectory("assets"); //for android

	}

	Entity::GetEntityManager()->PrintTreeAsText(); //useful for debugging
}*/

void QuakeInitScene()
{
	if (!IrrlichtManager::GetIrrlichtManager()->GetDevice())
	{
		LogError("Error initializing Irrlicht");
		return;
	}
	IrrlichtManager::GetIrrlichtManager()->GetDevice()->getTimer()->setTime(0);
	scene::ISceneManager*   smgr   = IrrlichtManager::GetIrrlichtManager()->GetScene();
	IrrlichtDevice*         device = IrrlichtManager::GetIrrlichtManager()->GetDevice();
	
	smgr->getParameters()->setAttribute(B3D_LOADER_IGNORE_MIPMAP_FLAG, true);
	
	//for Android
    FileSystemZip* pFileSystem = (FileSystemZip*) FileManager::GetFileManager()->GetFileSystem(0);
    
    //for Android
	if (pFileSystem)
        pFileSystem->SetRootDirectory("assets/game/quake");
	
	//hack so the textures can be found.  The quake map loader isn't all that smart
	device->getFileSystem()->changeWorkingDirectoryTo((GetBaseAppPath() + "game/quake").c_str());
    
	scene::IAnimatedMesh* mesh = smgr->getMesh("maps/20kdm2.bsp");
	scene::IMeshSceneNode* node = 0;
	
    if (mesh)
	{
		mesh->setHardwareMappingHint(scene::EHM_STATIC);
		node = smgr->addMeshSceneNode(mesh->getMesh(0));
		assert(node);
		node->setMaterialFlag(video::EMF_LIGHTING, false);
	}
    else
	{
		LogError("Aborting, can't load mesh");
		return;
	}
	
    scene::ITriangleSelector* selector = 0;
	
	bool bNoVerticalMovement = true;
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0, 100.0f, .3f, 0, 0, 0, bNoVerticalMovement, 3.f);
	camera->setPosition(core::vector3df(50,50,-60)); //quake level
	camera->setTarget(core::vector3df(-70,30,-160));
	camera->setFarValue(32000);
	node->setPosition(core::vector3df(-1350,-130,-1400));
    
    if (node)
    {
        //node->setPosition(core::vector3df(-1350,-130,-1400));
        
        selector = smgr->createOctreeTriangleSelector(node->getMesh(), node, 64);
        //selector = smgr->createTriangleSelector(node->getMesh(), node);
        
        node->setTriangleSelector(selector);
        // We're not done with this selector yet, so don't drop it.
    }
    
    if (selector)
    {
        scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
                                                                                selector, camera, core::vector3df(30,50,30),
                                                                                core::vector3df(0,-10,0), core::vector3df(0,30,0));
        selector->drop(); // As soon as we're done with the selector, drop it.
        camera->addAnimator(anim);
        anim->drop();  // And likewise, drop the animator when we're done referring to it.
    }
    
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

Entity * QuakeMenuCreate(Entity *pParentEnt)
{
	Entity *pBG = pParentEnt->AddEntity(new Entity("MapMenu"));

	//AddFocusIfNeeded(pBG);

	/*Entity *pButtonEntity;
	pButtonEntity = CreateTextButtonEntity(pBG, "Back", iPhoneMapX(240),iPhoneMapY(290), "Back"); 
	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->m_sig_function.connect(&QuakeMenuOnSelect);
	pButtonEntity->GetVar("alignment")->Set(uint32(ALIGNMENT_CENTER));
	AddHotKeyToButton(pButtonEntity, VIRTUAL_KEY_BACK); //for android's back button, or escape key in windows
    */
	
    //setup movement button
	//Entity *pEnt = pBG->AddEntity(new Entity("controls", new FPSControlComponent));

	//SlideScreen(pBG, true);
    
	QuakeInitScene();
	
    return pBG;
}

/*Entity * MainMenuCreate(Entity *pParentEnt)
{
    Entity *pBG = MapMenuCreate(pParentEnt);
    return pBG;
}*/

