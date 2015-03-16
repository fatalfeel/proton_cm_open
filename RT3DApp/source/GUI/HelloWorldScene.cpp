#include "HelloWorldScene.h"
#include "PlatformEnums.h"

CCScene* HelloWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();
	
	// 'layer' is an autorelease object
	HelloWorld* layer = HelloWorld::node();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

HelloWorld* HelloWorld::node() 
{ 
	HelloWorld* pRet = new HelloWorld(); 
	
	if (pRet && pRet->init()) 
	{ 
		pRet->autorelease();
		return pRet;
	}

	return NULL;
} 

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	/////////////////////////////
	// 3. add your codes below...
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	// add a label shows "Hello World"
	// create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::labelWithString("Hello World", "Arial", 24);
	// ask director the window size
	
	// position the label on the center of the screen
	pLabel->setPosition( ccp(size.width / 2, size.height - 50) );

	// add the label as a child to this layer
	this->addChild(pLabel, 0);
		
	return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void HelloWorld::onEnter()
{
	CCLayer::onEnter();

	CCSize				size		= CCDirector::sharedDirector()->getWinSize();
	CCActionInterval*	actionUp	= CCJumpBy::actionWithDuration(2, CCPointMake(0,0), 80, 4);

    // Or you can create an sprite using a filename. only PNG is supported now. Probably TIFF too
    m_grossini = CCSprite::spriteWithFile((GetBaseAppPath()+"game/grossini.png").c_str());
    m_grossini->retain();

	m_grossini->setPosition( CCPointMake(size.width/2, size.height/3));
	m_grossini->runAction( CCRepeatForever::actionWithAction(actionUp));

	this->addChild(m_grossini, 1);
}

void HelloWorld::onExit()
{
    m_grossini->release();
	CCLayer::onExit();
}
