//  ***************************************************************
//  EventControlComponent - Creation date: ?/?/2009
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2009 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef EventControlComponent_h__
#define EventControlComponent_h__

enum
{
	GUI_ID_BUTTON_ROOT  = 0x20000,
	GUI_ID_BUTTON_DIR_FORWARD,
	GUI_ID_BUTTON_DIR_BACK,
	GUI_ID_BUTTON_DIR_LEFT,
	GUI_ID_BUTTON_DIR_RIGHT,
    
    GUI_ID_BUTTON_ROLL_UP,
	GUI_ID_BUTTON_ROLL_DOWN,
	GUI_ID_BUTTON_ROLL_LEFT,
	GUI_ID_BUTTON_ROLL_RIGHT,
};

class EventControlComponent : public IEventReceiver
{
public:
	EventControlComponent();
	virtual ~EventControlComponent();

    void AddGuiButton();
    void SetSpeed(f32 rotateSpeed, f32 moveSpeed);
    void SendKey(irr::EKEY_CODE key, bool bIsDown);
    void OnUpdate(VariantList* pVList);
	bool OnEvent(const SEvent& event);
		
	bool        m_bTurningNow;
    SEvent      m_lastevent;
    f32         m_rotateSpeed, m_moveSpeed;
    int         m_firstUpdate;
};

#endif // EventControlComponent_h__