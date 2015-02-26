#include "PlatformPrecomp.h"
#include "QuakeShaderMenu.h"
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


/*void QuakeShaderMenuOnSelect(VariantList *pVList) //0=vec2 point of click, 1=entity sent from
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

void QuakeShaderInitScene()
{
	if (!IrrlichtManager::GetIrrlichtManager()->GetDevice())
	{
		LogError("Error initializing Irrlicht");
		return;
	}
	IrrlichtManager::GetIrrlichtManager()->GetDevice()->getTimer()->setTime(0);
	
    scene::ISceneManager*   smgr   = IrrlichtManager::GetIrrlichtManager()->GetScene();
	IrrlichtDevice*         device = IrrlichtManager::GetIrrlichtManager()->GetDevice();
	
		   
	// Quake3 Shader controls Z-Writing
	smgr->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);
    
    //for Android
    FileSystemZip* pFileSystem = (FileSystemZip*) FileManager::GetFileManager()->GetFileSystem(0);
    
    //for Android
	if (pFileSystem)
        pFileSystem->SetRootDirectory("assets/game/quake");
	
	//hack so the textures can be found.  The quake map loader isn't all that smart
	device->getFileSystem()->changeWorkingDirectoryTo((GetBaseAppPath() + "game/quake").c_str());
    
	u32                     i;
    scene::IQ3LevelMesh*    mesh;
	scene::IMeshSceneNode*  node;
    scene::IMesh*           geometry;
    const scene::IMesh*     additional_mesh;
	
    mesh = (scene::IQ3LevelMesh*)smgr->getMesh("maps/20kdm2.bsp");
    
    if (mesh)
	{
		geometry = mesh->getMesh(quake3::E_Q3_MESH_GEOMETRY);
		node = smgr->addOctreeSceneNode(geometry, 0, -1, 4096);
	}
    
	if ( mesh )
	{
		// the additional mesh can be quite huge and is unoptimized
		additional_mesh = mesh->getMesh(quake3::E_Q3_MESH_ITEMS);
        
#ifdef SHOW_SHADER_NAME
		gui::IGUIFont *font = device->getGUIEnvironment()->getFont("media/fontlucida.png");
		u32 count = 0;
#endif
       
		for ( i = 0; i!= additional_mesh->getMeshBufferCount(); ++i )
		{
			const IMeshBuffer* meshBuffer = additional_mesh->getMeshBuffer(i);
			const video::SMaterial& material = meshBuffer->getMaterial();
            
			// The ShaderIndex is stored in the material parameter
			const s32 shaderIndex = (s32) material.MaterialTypeParam2;
            
			// the meshbuffer can be rendered without additional support, or it has no shader
			const quake3::IShader* shader = mesh->getShader(shaderIndex);
			if (0 == shader)
			{
				continue;
			}
            
			// we can dump the shader to the console in its
			// original but already parsed layout in a pretty
			// printers way.. commented out, because the console
			// would be full...
			// quake3::dumpShader ( Shader );
            
			node = smgr->addQuake3SceneNode(meshBuffer, shader);
            
#ifdef SHOW_SHADER_NAME
			count += 1;
			core::stringw name( node->getName() );
			node = smgr->addBillboardTextSceneNode(
                                                   font, name.c_str(), node,
                                                   core::dimension2d<f32>(80.0f, 8.0f),
                                                   core::vector3df(0, 10, 0));
#endif
		}
	}
  
    quake3::IEntity             search;
    s32                         index;
    s32                         notEndList;
    u32                         parsepos;
    f32                         angle;
    core::vector3df             pos;
    const quake3::SVarGroup*    group;
    scene::ICameraSceneNode*    camera;
	
    camera = smgr->addCameraSceneNodeFPS();
    	
	if ( mesh )
	{
		quake3::tQ3EntityList& entityList   = mesh->getEntityList();
		search.name                         = "info_player_deathmatch";
        index                               = entityList.binary_search(search);
        
		if (index >= 0)
		{
			do
			{
				group       = entityList[index].getGroup(1);
                
				parsepos    = 0;
				pos         = quake3::getAsVector3df(group->get("origin"), parsepos);
                
				parsepos    = 0;
				angle       = quake3::getAsFloat(group->get("angle"), parsepos);
                
				core::vector3df target(0.f, 0.f, 1.f);
				target.rotateXZBy(angle);
                
				camera->setPosition(pos);
				camera->setTarget(pos + target);
                
				++index;
                
				notEndList = index == 2;
			} while ( notEndList );
		}
	}
    

#ifdef _IRR_COMPILE_WITH_GUI_
    EventControlComponent* receiver = new EventControlComponent();
    receiver->AddGuiButton();
	device->setEventReceiver(receiver);
#endif

}

Entity * QuakeShaderMenuCreate(Entity *pParentEnt)
{
	Entity *pBG = pParentEnt->AddEntity(new Entity("MapMenu"));

	//AddFocusIfNeeded(pBG);

	/*Entity *pButtonEntity;
	pButtonEntity = CreateTextButtonEntity(pBG, "Back", iPhoneMapX(240),iPhoneMapY(290), "Back"); 
	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->m_sig_function.connect(&QuakeShaderMenuOnSelect);
	pButtonEntity->GetVar("alignment")->Set(uint32(ALIGNMENT_CENTER));
	AddHotKeyToButton(pButtonEntity, VIRTUAL_KEY_BACK); //for android's back button, or escape key in windows
    */
	
    //setup movement button
	//Entity *pEnt = pBG->AddEntity(new Entity("controls", new FPSControlComponent));

	//SlideScreen(pBG, true);
    
	QuakeShaderInitScene();
	
    return pBG;
}

/*Entity * MainMenuCreate(Entity *pParentEnt)
{
    Entity *pBG = MapMenuCreate(pParentEnt);
    return pBG;
}*/

