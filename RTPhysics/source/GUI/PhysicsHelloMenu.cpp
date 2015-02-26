#include "PlatformPrecomp.h"
#include "PhysicsHelloMenu.h"
#include "Entity/EntityUtils.h"
#include "MainMenu.h"
#include "Irrlicht/IrrlichtManager.h"
//#include "Component/FPSControlComponent.h"
#include "Component/EventControlComponent.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

/*void PhysicsHelloMenuOnSelect(VariantList *pVList) //0=vec2 point of click, 1=entity sent from
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

	//if (pEntClicked->GetName() == "Shoot")
	//{
	//	shootCube(vector3df(2,2,2), 1);
	//	return;
	//}

	Entity::GetEntityManager()->PrintTreeAsText(); //useful for debugging
}*/

IRigidBody* addCube(const irr::core::vector3df &pos, const irr::core::vector3df &scale, irr::f32 mass, irr::core::stringc textureFile = "game/rockwall.jpg")
{
	irr::scene::ISceneNode *Node = IrrlichtManager::GetIrrlichtManager()->GetScene()->addCubeSceneNode(1.0f);
	Node->setScale(scale);
	Node->setPosition(pos);
	Node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
	Node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
	Node->setMaterialTexture(0, IrrlichtManager::GetIrrlichtManager()->GetTexture(textureFile.c_str()));
	
	//Node->setMaterialFlag(irr::video::EMF_WIREFRAME, drawWireFrame);

	ICollisionShape *shape = new IBoxShape(Node, mass, false);

	//shape->setMargin(0.01);

	IRigidBody *body;
	body = IrrlichtManager::GetIrrlichtManager()->GetBulletWorld()->addRigidBody(shape);
	body->setDamping(0.2,0.2);
	body->setFriction(0.6f);

	return body;
}

void HelloMenuInitScene()
{
	if (!IrrlichtManager::GetIrrlichtManager()->GetDevice())
	{
		LogError("Error initializing Irrlicht");
		return;
	}
	IrrlichtManager::GetIrrlichtManager()->GetDevice()->getTimer()->setTime(0);
	
    video::IVideoDriver*    driver  = IrrlichtManager::GetIrrlichtManager()->GetDriver();
	scene::ISceneManager*   smgr    = IrrlichtManager::GetIrrlichtManager()->GetScene();
	IrrlichtDevice *        device  = IrrlichtManager::GetIrrlichtManager()->GetDevice();
    
	smgr->getParameters()->setAttribute(B3D_LOADER_IGNORE_MIPMAP_FLAG, true);

	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0,100.0f,0.1f, 0, 0, 0, true, 3.f);
	camera->setPosition(core::vector3df(50,15,70)); 
	camera->setTarget(core::vector3df(20,7,10));

	float fov = float(GetPrimaryGLX())/ float(GetPrimaryGLY());
	LogMsg("Setting fov to %.2f", fov);
	camera->setAspectRatio(fov);
	camera->setFOV((120 * M_PI / 360.0f));
	
	smgr->addLightSceneNode(0, vector3df(+20, 40, -50), SColorf(1.0f, 1.0f, 1.0f), 4000.0f);
    smgr->addLightSceneNode(0, vector3df(-20, 40, -50), SColorf(1.0f, 1.0f, 1.0f), 4000.0f);

	irrBulletWorld* world = IrrlichtManager::GetIrrlichtManager()->GetBulletWorld();
	
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	scene::ISceneNode* skybox = 
	smgr->addSkyBoxSceneNode(IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_up.jpg"),
							 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_dn.jpg"),
							 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_lf.jpg"),
							 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_rt.jpg"),
							 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_ft.jpg"),
							 IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_bk.jpg"));
		

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	//create ground

	ISceneNode *Node = device->getSceneManager()->addCubeSceneNode(1.0);
	Node->setScale(vector3df(300,3,300)); // 400, 3, 400
	Node->setPosition(vector3df(20,0,10));
	Node->setMaterialFlag(video::EMF_LIGHTING, true);
	Node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	Node->setMaterialTexture(0,IrrlichtManager::GetIrrlichtManager()->GetTexture("game/rockwall.jpg"));

	IRigidBody*			body;
	ICollisionShape*	shape = new IBoxShape(Node, 0, false);
	
	body = world->addRigidBody(shape);

	//add the blocks
	int columns = 7;
	int rows = 7;

	for(u32 j=0; j < columns; j++)
	{
		for(u32 i=0; i < rows; i++)
		{

			if (rand()%2 == 1)
			addCube(vector3df(3 * i, (0 + 3 * j+ 3)+(j*3), 0), vector3df(3,3,3), 3);
		}
	}

	BaseApp::GetBaseApp()->GetGameTimer()->Reset();
    
#ifdef _IRR_COMPILE_WITH_GUI_
    EventControlComponent* receiver = new EventControlComponent();
    receiver->AddGuiButton();
	device->setEventReceiver(receiver);
#endif
}

Entity * PhysicsHelloMenuCreate(Entity *pParentEnt)
{
    //Entity *pButtonEntity;
	
    Entity *pBG = pParentEnt->AddEntity(new Entity("PhysicsHelloMenu"));

	//AddFocusIfNeeded(pBG);

	/*pButtonEntity = CreateTextButtonEntity(pBG, "Back", iPhoneMapX(240),iPhoneMapY(290), "Back");
	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->m_sig_function.connect(&PhysicsHelloMenuOnSelect);
	pButtonEntity->GetVar("alignment")->Set(uint32(ALIGNMENT_CENTER));*/

	/*pButtonEntity = CreateTextButtonEntity(pBG, "Shoot", iPhoneMapX(390),iPhoneMapY(100), "Shoot Cube");
	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->m_sig_function.connect(&PhysicsHelloMenuOnSelect);
	
	//le's make the button sfx cooler:
	SetButtonClickSound(pButtonEntity, "audio/shoot.wav");

	//setup movement button
	Entity *pEnt = pBG->AddEntity(new Entity("controls", new FPSControlComponent));*/

	//SlideScreen(pBG, true);

	HelloMenuInitScene();
	return pBG;
}

