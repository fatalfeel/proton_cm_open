#include "PlatformPrecomp.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Mesh3DMenu.h"
#include "MainMenu.h"
#include "Irrlicht/IrrlichtManager.h"
#include "driverChoice.h"
#include "Component/EventControlComponent.h"
#include "FileSystem/FileSystemZip.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

void Mesh3DInitScene()
{
	IAnimatedMesh*				mesh;
	IAnimatedMeshSceneNode*     node;
	scene::ISceneNodeAnimator*  anim;
	scene::ISceneManager*		smgr   = IrrlichtManager::GetIrrlichtManager()->GetScene();
	IrrlichtDevice*				device = IrrlichtManager::GetIrrlichtManager()->GetDevice();
	video::IVideoDriver*		driver = IrrlichtManager::GetIrrlichtManager()->GetDriver();
	
	std::string					load_zip;
	std::string					load_data;
	std::string					reload_path;
	FileSystemZip*				pfilesystem			= NULL;
	io::IReadFile*				memfile				= NULL;
	byte*						apk_buffer			= NULL;
	byte*						buff_extract		= NULL;
	int							size				= 0;
	int							apk_size			= 0;

	if (!IrrlichtManager::GetIrrlichtManager()->GetDevice())
	{
		LogError("Error initializing Irrlicht");
		return;
	}
	
	IrrlichtManager::GetIrrlichtManager()->GetDevice()->getTimer()->setTime(0);
		
    smgr->addLightSceneNode(0, core::vector3df(-100,10,0), video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 200.0f);
    smgr->addLightSceneNode(0, core::vector3df(+100,10,0), video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 200.0f);
	
		
//////////////////////////////mesh/////////////////////////////////////////////////
	load_zip	= (GetBaseAppPath() + "game/ninja.zip").c_str();
	load_data	= "ninja.b3d";
	reload_path	= (GetBaseAppPath() + "game/ninja.b3d").c_str();
	
#ifdef ANDROID_NDK
	apk_buffer	= FileManager::GetFileManager()->Get(load_zip.c_str(), &apk_size, false, false);
	
	if( apk_buffer )
	{
		pfilesystem	= new FileSystemZip();
		pfilesystem->Init_unzMemory(apk_buffer, apk_size);
		buff_extract = pfilesystem->Get_unz(load_data, &size);

		delete apk_buffer;
		apk_buffer = NULL;
	}
#else
	pfilesystem = new FileSystemZip();
	pfilesystem->Init_unz(load_zip.c_str());
	buff_extract = pfilesystem->Get_unz(load_data.c_str(), &size);
#endif
	
	memfile = device->getFileSystem()->createMemoryReadFile(buff_extract, size, reload_path.c_str(), true);
	//new buffer copy on file->read(Buffer, size) of CXMeshFileLoader::readFileIntoMemory
	mesh	= smgr->getMesh( memfile );
	node	= smgr->addAnimatedMeshSceneNode( mesh );

	//delete buff_extract in drop() then goto ~CMemoryReadFile
	memfile->drop();
						
	if( pfilesystem )
	{
		delete pfilesystem;
		pfilesystem = NULL;
	}

//////////////////////////////texture/////////////////////////////////////////////////
	node->setMaterialTexture( 0, driver->getTexture((GetBaseAppPath()+"game/nskinbl.jpg").c_str()) );
    node->setMaterialFlag(EMF_LIGHTING, true);
    node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
    anim = smgr->createRotationAnimator(core::vector3df(0,0.3f,0));
    node->addAnimator(anim);
    anim->drop();

	u32 alpha_val		= 100; //.x mesh file can not do alpha transparent
	u32 MaterialCount	= node->getMaterialCount();
	
	for(u32 i=0; i<MaterialCount; i++)
	{
		video::SMaterial& tex_mat	= node->getMaterial(i);
		tex_mat.MaterialType		= video::EMT_TRANSPARENT_VERTEX_ALPHA;
		//tex_mat.MaterialTypeParam	= 0.1;
		//tex_mat.MaterialTypeParam2= 0.1;
		tex_mat.AmbientColor.setAlpha(alpha_val);
		tex_mat.DiffuseColor.setAlpha(alpha_val);
		tex_mat.SpecularColor.setAlpha(alpha_val);
		tex_mat.EmissiveColor.setAlpha(alpha_val);
	}

#if	defined(_IRR_COMPILE_WITH_OGLES2_)
	for(u32 i=0; i<mesh->getMeshBufferCount(); i++)
	{
		scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);
		video::S3DVertex* vertex = (video::S3DVertex*)buffer->getVertices();
		
		for(u32 j=0; j<buffer->getVertexCount(); j++)
		{
			vertex[j].Color.setAlpha(alpha_val);
		}
	}
#endif
	
	smgr->addSkyBoxSceneNode(IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_up.jpg"),
                             IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_dn.jpg"),
                             IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_lf.jpg"),
                             IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_rt.jpg"),
                             IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_ft.jpg"),
                             IrrlichtManager::GetIrrlichtManager()->GetTexture("game/irrlicht2_bk.jpg"));
//////////////////////////////////cam/////////////////////////////////////////////    
   	ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0, 100.0f, .02f, 0, 0, 0, true, 1.0f);
	//camera->addCameraSceneNode(0, vector3df(0,2,-10));
	camera->setPosition(core::vector3df(0,5,-20));
	float fov = float(GetPrimaryGLX())/ float(GetPrimaryGLY());
	camera->setAspectRatio(fov);
	camera->setFOV((120 * (float)M_PI / 360.0f));

#ifdef _IRR_COMPILE_WITH_GUI_
	EventControlComponent* receiver = new EventControlComponent();
    receiver->AddGuiButton();
	device->setEventReceiver(receiver);
#endif
}

Entity * Mesh3DMenuCreate(Entity *pParentEnt)
{
	Entity *pBG = pParentEnt->AddEntity(new Entity("Mesh3DMenu"));

	//AddFocusIfNeeded(pBG);
	
	/*Entity *pButtonEntity;
	pButtonEntity = CreateTextButtonEntity(pBG, "Back", iPhoneMapX(240),iPhoneMapY(290), "Back"); 

	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->m_sig_function.connect(&Mesh3DMenuOnSelect);
	pButtonEntity->GetVar("alignment")->Set(uint32(ALIGNMENT_CENTER));

	SlideScreen(pBG, true);*/
	
	Mesh3DInitScene();
	return pBG;
}

//by stone test bug
/*Entity* MainMenuCreate(Entity *pParentEnt)
{
    Entity *pBG = Mesh3DMenuCreate(pParentEnt);
    return pBG;
}*/
