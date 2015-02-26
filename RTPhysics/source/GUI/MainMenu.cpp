#include "PlatformPrecomp.h"
#include "MainMenu.h"
#include "Entity/EntityUtils.h"
//#include "DebugMenu.h"
#include "PhysicsHelloMenu.h"
#include "Entity/FilterComponent.h"
//#include "Entity/CustomInputComponent.h"
#include "Irrlicht/IrrlichtManager.h"

/*void MainMenuOnSelect(VariantList *pVList) //0=vec2 point of click, 1=entity sent from
{
	Entity *pEntClicked = pVList->m_variant[1].GetEntity();

	LogMsg("Clicked %s entity at %s", pEntClicked->GetName().c_str(),pVList->m_variant[1].Print().c_str());


	if (pEntClicked->GetName() == "Debug")
	{
		//overlay the debug menu over this one
		pEntClicked->GetParent()->RemoveComponentByName("FocusInput");
		DebugMenuCreate(pEntClicked->GetParent());
	}

	if (pEntClicked->GetName() == "Physics1")
	{
		//overlay the debug menu over this one
		pEntClicked->GetParent()->RemoveComponentByName("FocusInput");
		SlideScreen(pEntClicked->GetParent(), false);
		MessageManager::GetMessageManager()->CallEntityFunction(pEntClicked->GetParent(), 500, "OnDelete", NULL);
		PhysicsHelloMenuCreate(pEntClicked->GetParent()->GetParent());
	}

	Entity::GetEntityManager()->PrintTreeAsText(); //useful for debugging
}

Entity * MainMenuCreate(Entity *pParentEnt)
{
	Entity *pBG = CreateOverlayEntity(pParentEnt, "MainMenu", "interface/large/main_menu.rttex", 0,0);
	
	//GetMessageManager()->SendGame(MESSAGE_TYPE_PLAY_SOUND, "audio/mp3/main_menu.mp3", 50);
	EntitySetScaleBySize(pBG, GetScreenSize());
	AddFocusIfNeeded(pBG);
	
	Entity *pButtonEntity;
	float ySpacer = 80;
	
	eFont fontID = FONT_LARGE;
	float fontScale = 1.0f;

	float x = iPadMapX(100);
	float y = iPadMapY(400);
	
	pButtonEntity = CreateTextButtonEntity(pBG, "Physics1", x, y, "irrBullet Hello World"); y += ySpacer;
	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->m_sig_function.connect(&MainMenuOnSelect);
	SetupTextEntity(pButtonEntity, fontID, fontScale);
	

	pButtonEntity = CreateTextButtonEntity(pBG, "Debug", x, y, "Debug Options"); y += ySpacer;
	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->m_sig_function.connect(&MainMenuOnSelect);
	SetupTextEntity(pButtonEntity, fontID, fontScale);

	//for android, so the back key (or escape on windows/OSX) will quit out of the game
	EntityComponent *pComp = pBG->AddComponent(new CustomInputComponent);
	//tell the component which key has to be hit for it to be activated
	pComp->GetFunction("OnActivated")->m_sig_function.connect(1, boost::bind(&App::OnExitApp, GetApp(), _1));
	pComp->GetVar("keycode")->Set(uint32(VIRTUAL_KEY_BACK));

	SlideScreen(pBG, true);

	return pBG;
}*/

//by stone
enum EnMenuSelect
{
    PHYSICS
};

Entity* MainMenuCreate(Entity* pParentEnt)
{
    Entity*			pBG;
	EnMenuSelect	menuid = PHYSICS;
    
    switch(menuid)
    {
        case PHYSICS:
            pBG = PhysicsHelloMenuCreate(pParentEnt);
            break;
    }

	return pBG; 
}
