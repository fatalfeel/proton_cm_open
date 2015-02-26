#include "PlatformPrecomp.h"
#include "Irrlicht/source/CCameraSceneNode.h"
#include "EventControlComponent.h"
#include "util/GLESUtils.h"
#include "Entity/EntityUtils.h"
#include "BaseApp.h"
#include "App.h"

using namespace irr;

IRigidBody* shootCube(const irr::core::vector3df &scale, irr::f32 mass, irr::core::stringc textureFile = "game/rockwall.jpg")
{
    
	scene::ICameraSceneNode* camera = IrrlichtManager::GetIrrlichtManager()->GetScene()->getActiveCamera();
    
	video::IVideoDriver* driver = IrrlichtManager::GetIrrlichtManager()->GetDriver();
	scene::ISceneManager* smgr = IrrlichtManager::GetIrrlichtManager()->GetScene();
	IrrlichtDevice * device = IrrlichtManager::GetIrrlichtManager()->GetDevice();
	irrBulletWorld * world = IrrlichtManager::GetIrrlichtManager()->GetBulletWorld();
    
	irr::core::vector3df pos(camera->getPosition().X,camera->getPosition().Y,camera->getPosition().Z);
    
	irr::scene::ISceneNode *Node = device->getSceneManager()->addCubeSceneNode(1.0f);
	Node->setScale(scale);
	Node->setPosition(pos);
	Node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
	Node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
	Node->setMaterialTexture(0, IrrlichtManager::GetIrrlichtManager()->GetTexture(textureFile.c_str()));
    
	//Node->setMaterialFlag(irr::video::EMF_WIREFRAME, drawWireFrame);
    
	ICollisionShape *shape = new IBoxShape(Node, mass, true);
    
	//shape->setMargin(0.01);
    
	IRigidBody *body = world->addRigidBody(shape);
    
	irr::core::vector3df rot = camera->getRotation();
	irr::core::matrix4 mat;
	mat.setRotationDegrees(rot);
	irr::core::vector3df forwardDir(irr::core::vector3df(mat[8],mat[9],mat[10]) * 120);
    
	body->setLinearVelocity(forwardDir);
	//body->setActivationState(EAS_DISABLE_DEACTIVATION);
    
	body->setDamping(0.2,0.2);
	body->setFriction(0.6f);
    
	//ICollisionObjectAffectorDelete *deleteAffector = new ICollisionObjectAffectorDelete(4000);
	//body->addAffector(deleteAffector);
    
	body->getAttributes()->addBool("collide", true);
    
	//ICollisionObjectAffectorAttract* affector = new ICollisionObjectAffectorAttract(irr::core::vector3df(0,1000,0), 2500);
	//ICollisionObjectAffectorAttract* affector = new ICollisionObjectAffectorAttract(device->getSceneManager()->getActiveCamera(), 2500);
	//body->addAffector(affector);
    
	return body;
}

EventControlComponent::EventControlComponent()
{
    m_bTurningNow   = false;
    m_rotateSpeed   = 100.0f;
    m_moveSpeed     = 10.0f;
    m_firstUpdate   = true;
}

EventControlComponent::~EventControlComponent()
{
    BaseApp::GetBaseApp()->m_sig_update.disconnect(boost::bind(&EventControlComponent::OnUpdate, this, _1));
}

void EventControlComponent::SetSpeed(f32 rotateSpeed, f32 moveSpeed)
{
    m_rotateSpeed   = rotateSpeed;
    m_moveSpeed     = moveSpeed;
}

void EventControlComponent::AddGuiButton()
{
    video::ITexture*		image;
	gui::IGUIButton*		btn;
	video::IVideoDriver*    driver = IrrlichtManager::GetIrrlichtManager()->GetDriver();
    IrrlichtDevice*         device = IrrlichtManager::GetIrrlichtManager()->GetDevice();
    gui::IGUIEnvironment*   gui    = device->getGUIEnvironment();
    core::dimension2d<u32>  screen = driver->getScreenSize();
    
#ifdef __APPLE__
	if( screen.Width * screen.Height > 320 * 480 )
    {
        image	= driver->getTexture((GetBaseAppPath()+"game/ar_up.png").c_str());
        btn		= gui->addButton(core::rect<int>(85,screen.Height-300,155,screen.Height-230), NULL, GUI_ID_BUTTON_DIR_FORWARD);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
        
		image	= driver->getTexture((GetBaseAppPath()+"game/ar_dn.png").c_str());
		btn		= gui->addButton(core::rect<int>(85,screen.Height-100,155,screen.Height-30),  NULL, GUI_ID_BUTTON_DIR_BACK);
		btn->setImage(image);
		btn->setUseAlphaChannel(true);
		btn->setDrawBorder(false);

		image	= driver->getTexture((GetBaseAppPath()+"game/ar_lt.png").c_str());
		btn		= gui->addButton(core::rect<int>(05,screen.Height-200,75,screen.Height-130), NULL, GUI_ID_BUTTON_DIR_LEFT);
		btn->setImage(image);
		btn->setUseAlphaChannel(true);
		btn->setDrawBorder(false);

		image	= driver->getTexture((GetBaseAppPath()+"game/ar_rt.png").c_str());
		btn		= gui->addButton(core::rect<int>(85,screen.Height-200,155,screen.Height-130), NULL, GUI_ID_BUTTON_DIR_RIGHT);
		btn->setImage(image);
		btn->setUseAlphaChannel(true);
		btn->setDrawBorder(false);

		////////
		image	= driver->getTexture((GetBaseAppPath()+"game/ar_up.png").c_str());
		btn		= gui->addButton(core::rect<int>(screen.Width-155,screen.Height-300,screen.Width-85,screen.Height-230), NULL, GUI_ID_BUTTON_ROLL_UP);
		btn->setImage(image);
		btn->setUseAlphaChannel(true);
		btn->setDrawBorder(false);

		image	= driver->getTexture((GetBaseAppPath()+"game/ar_dn.png").c_str());
		btn		= gui->addButton(core::rect<int>(screen.Width-155,screen.Height-100,screen.Width-85,screen.Height-30), NULL, GUI_ID_BUTTON_ROLL_DOWN);
		btn->setImage(image);
		btn->setUseAlphaChannel(true);
		btn->setDrawBorder(false);

		image	= driver->getTexture((GetBaseAppPath()+"game/ar_lt.png").c_str());
		btn		= gui->addButton(core::rect<int>(screen.Width-155,screen.Height-200,screen.Width-85,screen.Height-130), NULL, GUI_ID_BUTTON_ROLL_LEFT);
		btn->setImage(image);
		btn->setUseAlphaChannel(true);
		btn->setDrawBorder(false);

		image	= driver->getTexture((GetBaseAppPath()+"game/ar_rt.png").c_str());
		btn		= gui->addButton(core::rect<int>(screen.Width-75,screen.Height-200,screen.Width-5,screen.Height-130), NULL, GUI_ID_BUTTON_ROLL_RIGHT);
		btn->setImage(image);
		btn->setUseAlphaChannel(true);
		btn->setDrawBorder(false);

        /////    
        image	= driver->getTexture((GetBaseAppPath()+"game/fire.png").c_str());
        btn		= gui->addButton(core::rect<int>(screen.Width-75,screen.Height-300,screen.Width-5,screen.Height-230), NULL, GUI_ID_BUTTON_ROLL_SHOOT);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
    }
    else //small screen
    {
        image	= driver->getTexture((GetBaseAppPath()+"game/ar_up_32.png").c_str());
        btn		= gui->addButton(core::rect<int>(55,screen.Height-150,95,screen.Height-110), NULL, GUI_ID_BUTTON_DIR_FORWARD);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
        
        image	= driver->getTexture((GetBaseAppPath()+"game/ar_dn_32.png").c_str());
        btn		= gui->addButton(core::rect<int>(55,screen.Height-50,95,screen.Height-10),  NULL, GUI_ID_BUTTON_DIR_BACK);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
        
        image	= driver->getTexture((GetBaseAppPath()+"game/ar_lt_32.png").c_str());
        btn		= gui->addButton(core::rect<int>(05,screen.Height-100,45,screen.Height-60), NULL, GUI_ID_BUTTON_DIR_LEFT);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
        
        image	= driver->getTexture((GetBaseAppPath()+"game/ar_rt_32.png").c_str());
        btn		= gui->addButton(core::rect<int>(105,screen.Height-100,145,screen.Height-60), NULL, GUI_ID_BUTTON_DIR_RIGHT);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
        
        ////////
        image	= driver->getTexture((GetBaseAppPath()+"game/ar_up_32.png").c_str());
        btn		= gui->addButton(core::rect<int>(screen.Width-95,screen.Height-150,screen.Width-55,screen.Height-110), NULL, GUI_ID_BUTTON_ROLL_UP);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
        
        image	= driver->getTexture((GetBaseAppPath()+"game/ar_dn_32.png").c_str());
        btn		= gui->addButton(core::rect<int>(screen.Width-95,screen.Height-50,screen.Width-55,screen.Height-10), NULL, GUI_ID_BUTTON_ROLL_DOWN);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
        
        image	= driver->getTexture((GetBaseAppPath()+"game/ar_lt_32.png").c_str());
        btn		= gui->addButton(core::rect<int>(screen.Width-145,screen.Height-100,screen.Width-105,screen.Height-60), NULL, GUI_ID_BUTTON_ROLL_LEFT);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
        
        image	= driver->getTexture((GetBaseAppPath()+"game/ar_rt_32.png").c_str());
        btn		= gui->addButton(core::rect<int>(screen.Width-45,screen.Height-100,screen.Width-5,screen.Height-60), NULL, GUI_ID_BUTTON_ROLL_RIGHT);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
        
        /////
        image	= driver->getTexture((GetBaseAppPath()+"game/fire_32.png").c_str());
        btn		= gui->addButton(core::rect<int>(screen.Width-45,screen.Height-150,screen.Width-5,screen.Height-110), NULL, GUI_ID_BUTTON_ROLL_SHOOT);
        btn->setImage(image);
        btn->setUseAlphaChannel(true);
        btn->setDrawBorder(false);
    }
#else
    image	= driver->getTexture((GetBaseAppPath()+"game/ar_up.png").c_str());
	btn		= gui->addButton(core::rect<int>(85,screen.Height-350,155,screen.Height-280), NULL, GUI_ID_BUTTON_DIR_FORWARD);
	btn->setImage(image);
	btn->setUseAlphaChannel(true);
	btn->setDrawBorder(false);

	image	= driver->getTexture((GetBaseAppPath()+"game/ar_dn.png").c_str());
	btn		= gui->addButton(core::rect<int>(85,screen.Height-150,155,screen.Height-80),  NULL, GUI_ID_BUTTON_DIR_BACK);
	btn->setImage(image);
	btn->setUseAlphaChannel(true);
	btn->setDrawBorder(false);

	image	= driver->getTexture((GetBaseAppPath()+"game/ar_lt.png").c_str());
	btn		= gui->addButton(core::rect<int>(05,screen.Height-250,75,screen.Height-180), NULL, GUI_ID_BUTTON_DIR_LEFT);
	btn->setImage(image);
	btn->setUseAlphaChannel(true);
	btn->setDrawBorder(false);

	image	= driver->getTexture((GetBaseAppPath()+"game/ar_rt.png").c_str());
	btn		= gui->addButton(core::rect<int>(165,screen.Height-250,235,screen.Height-180), NULL, GUI_ID_BUTTON_DIR_RIGHT);
	btn->setImage(image);
	btn->setUseAlphaChannel(true);
	btn->setDrawBorder(false);

	////////
	image	= driver->getTexture((GetBaseAppPath()+"game/ar_up.png").c_str());
	btn		= gui->addButton(core::rect<int>(screen.Width-155,screen.Height-350,screen.Width-85,screen.Height-280), NULL, GUI_ID_BUTTON_ROLL_UP);
	btn->setImage(image);
	btn->setUseAlphaChannel(true);
	btn->setDrawBorder(false);

	image	= driver->getTexture((GetBaseAppPath()+"game/ar_dn.png").c_str());
	btn		= gui->addButton(core::rect<int>(screen.Width-155,screen.Height-150,screen.Width-85,screen.Height-80), NULL, GUI_ID_BUTTON_ROLL_DOWN);
	btn->setImage(image);
	btn->setUseAlphaChannel(true);
	btn->setDrawBorder(false);

	image	= driver->getTexture((GetBaseAppPath()+"game/ar_lt.png").c_str());
	btn		= gui->addButton(core::rect<int>(screen.Width-235,screen.Height-250,screen.Width-165,screen.Height-180), NULL, GUI_ID_BUTTON_ROLL_LEFT);
	btn->setImage(image);
	btn->setUseAlphaChannel(true);
	btn->setDrawBorder(false);

	image	= driver->getTexture((GetBaseAppPath()+"game/ar_rt.png").c_str());
	btn		= gui->addButton(core::rect<int>(screen.Width-75,screen.Height-250,screen.Width-5,screen.Height-180), NULL, GUI_ID_BUTTON_ROLL_RIGHT);
	btn->setImage(image);
	btn->setUseAlphaChannel(true);
	btn->setDrawBorder(false);
/////    
    image	= driver->getTexture((GetBaseAppPath()+"game/fire.png").c_str());
	btn		= gui->addButton(core::rect<int>(screen.Width-75,screen.Height-350,screen.Width-5,screen.Height-280), NULL, GUI_ID_BUTTON_ROLL_SHOOT);
	btn->setImage(image);
	btn->setUseAlphaChannel(true);
	btn->setDrawBorder(false);
#endif
    
    BaseApp::GetBaseApp()->m_sig_update.connect(1, boost::bind(&EventControlComponent::OnUpdate, this, _1));
}

void EventControlComponent::SendKey(irr::EKEY_CODE key, bool bIsDown)
{
	irr::SEvent ev;

	ev.EventType             = irr::EET_KEY_INPUT_EVENT;
	ev.KeyInput.Char         = 0;
	ev.KeyInput.Key          = key; //irr::KEY_UP;
	ev.KeyInput.Control      = false;
	ev.KeyInput.PressedDown  = bIsDown;
	ev.KeyInput.Shift        = false;
    
	IrrlichtManager::GetIrrlichtManager()->GetDevice()->getInputReceivingSceneManager()->postEventFromUser(ev);
}

bool EventControlComponent::OnEvent(const SEvent& event)
{
    int  id;
        
    m_lastevent = event;
    
    if (event.EventType == EET_GUI_EVENT)
    {
        switch(event.GUIEvent.EventType)
        {
			case gui::EGET_MENU_ITEM_SELECTED:
                // a menu item was clicked
				return true;
                
			case gui::EGET_FILE_SELECTED:
				return true;
                
			case gui::EGET_SCROLL_BAR_CHANGED:
				return true;
                
			case gui::EGET_COMBO_BOX_CHANGED:
				return true;
                
			case gui::EGET_BUTTON_CLICKED:
                
                id  = event.GUIEvent.Caller->getID();
                
                switch(id)
                {
                    case GUI_ID_BUTTON_DIR_FORWARD:
                        if( event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                        {
                            SendKey(irr::KEY_UP, true);
                        }
                        else
                        {
                            SendKey(irr::KEY_UP, false);
                        }
                        return true;
                        
                    case GUI_ID_BUTTON_DIR_BACK:
                        if( event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                        {
                            SendKey(irr::KEY_DOWN, true);
                        }
                        else
                        {
                            SendKey(irr::KEY_DOWN, false);
                        }
                        return true;
                        
                    case GUI_ID_BUTTON_DIR_LEFT:
                        if( event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                        {
                            SendKey(irr::KEY_LEFT, true);
                        }
                        else
                        {
                            SendKey(irr::KEY_LEFT, false);
                        }
                        return true;
                        
                    case GUI_ID_BUTTON_DIR_RIGHT:
                        if( event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                        {
                            SendKey(irr::KEY_RIGHT, true);
                        }
                        else
                        {
                            SendKey(irr::KEY_RIGHT, false);
                        }
                        return true;
                        
                        //////////////////////////////
                    case GUI_ID_BUTTON_ROLL_UP:
                        if( event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                            m_bTurningNow = true;
                        else
                            m_bTurningNow = false;
                        return true;
                        
                    case GUI_ID_BUTTON_ROLL_DOWN:
                        if( event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                            m_bTurningNow = true;
                        else
                            m_bTurningNow = false;
                        return true;
                        
                    case GUI_ID_BUTTON_ROLL_LEFT:
                        if( event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                            m_bTurningNow = true;
                        else
                            m_bTurningNow = false;
                        return true;
                        
                    case GUI_ID_BUTTON_ROLL_RIGHT:
                        if( event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                            m_bTurningNow = true;
                        else
                            m_bTurningNow = false;
                        return true;

                    case GUI_ID_BUTTON_ROLL_SHOOT:
                        if( event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
                        {
                            GetAudioManager()->Play("audio/shoot.wav");
                            shootCube(core::vector3df(2,2,2), 1);
                        }
                        return true;
                }
				break;
                
			default:
				break;
        }
    }
    
    return false;
}

void EventControlComponent::OnUpdate(VariantList* pVList)
{
    if (m_bTurningNow)
	{
		irr::scene::CCameraSceneNode* camera = (irr::scene::CCameraSceneNode*) IrrlichtManager::GetIrrlichtManager()->GetScene()->getActiveCamera();
		
        core::matrix4   mat;
		core::vector3df pos                 = camera->getPosition();
        core::vector3df target              = (camera->getTarget() - camera->getPosition());
        core::vector3df relativeRotation    = target.getHorizontalAngle();
        int             id					= -1;

		if (m_lastevent.EventType == EET_GUI_EVENT)
		{
			switch(m_lastevent.GUIEvent.EventType)
			{
				case gui::EGET_BUTTON_CLICKED:
					id = m_lastevent.GUIEvent.Caller->getID();
					break;
			}
		}
    
        //update it
		float sensitivityX = 2;
		float sensitivityY = 2;
        
        switch( id )
        {
            case GUI_ID_BUTTON_ROLL_UP:
                relativeRotation.X -= BaseApp::GetBaseApp()->GetGameDelta()*sensitivityY;
                                
                break;
                
            case GUI_ID_BUTTON_ROLL_DOWN:
                relativeRotation.X += BaseApp::GetBaseApp()->GetGameDelta()*sensitivityY;
                               
                break;
                
            case GUI_ID_BUTTON_ROLL_LEFT:
                relativeRotation.Y -= BaseApp::GetBaseApp()->GetGameDelta()*sensitivityX;
                                
                break;
                
            case GUI_ID_BUTTON_ROLL_RIGHT:
                relativeRotation.Y += BaseApp::GetBaseApp()->GetGameDelta()*sensitivityX;
                               
                break;
        }
        
        target.set(0,0, core::max_(1.f, pos.getLength()));
        mat.setRotationDegrees(core::vector3df(relativeRotation.X, relativeRotation.Y, 0));
        mat.transformVect(target);
/////////////////
        // write translation
        camera->setPosition(pos);
        
        // write right target
        target += pos;
        camera->setTarget(target);
  	}
}

