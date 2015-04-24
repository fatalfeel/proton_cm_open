#include "PlatformPrecomp.h"
#include "App.h"

#include "cocos2d.h"
#include "cocos-ext.h"
using namespace cocos2d;
using namespace cocos2d::extension;

#include "GuiScroll.h"

CCScene* GuiScroll::scene()
{
	// 'scene' is an autorelease object
	CCScene* scene = CCScene::create();
	
	// 'layer' is an autorelease object
	GuiScroll* layer = GuiScroll::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

GuiScroll* GuiScroll::create() 
{ 
	GuiScroll* pRet = new GuiScroll(); 
	
	if (pRet && pRet->init()) 
	{ 
		pRet->autorelease();
		return pRet;
	}

	return NULL;
} 

GuiScroll::GuiScroll()
{
}

GuiScroll::~GuiScroll()
{
}

// on "init" you need to initialize your instance
bool GuiScroll::init()
{
	int					i;
	std::string			file_name;
	CCSize				visibleSize;
	CCSprite*			pPoint;
	CCSprite*			pSprite;
	CCSpriteFrameCache*	pCache;
	CCLayer*			pLayer;
	bool				bRet		= false;
	char				pngstr[64]  = {0};
    
    //ratio is special for ios
    float               ratio       = CCDirector::sharedDirector()->getContentScaleFactor();

	m_nCurPage = 1;

	if( CCLayer::init() )
	{
   		visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		
		pLayer = CCLayer::create();
		for (i=1; i<=3; i++)
		{
			sprintf(pngstr,"bgscroll.png");
			file_name	 = (GetBaseAppPath()+"interface/").c_str();
			file_name	+= pngstr;
		   
			pSprite = CCSprite::create(file_name.c_str());
			pSprite->setPosition(ccp( ((float)i - 0.5f)*visibleSize.width, visibleSize.height/2.0f));
			pLayer->addChild(pSprite);
		}
				
		m_pScrollView = CCScrollView::create(CCSizeMake(visibleSize.width*ratio, visibleSize.height*ratio), pLayer);
		m_pScrollView->retain();

		m_pScrollView->setContentOffset(CCPointZero);
		m_pScrollView->setTouchEnabled(false);
		m_pScrollView->setDelegate(this);
		m_pScrollView->setDirection(kCCScrollViewDirectionHorizontal);
		this->addChild(m_pScrollView,1);

		pCache = CCSpriteFrameCache::sharedSpriteFrameCache();
		pCache->addSpriteFrame(CCSpriteFrame::create((GetBaseAppPath()+"interface/button_normal.png").c_str(),CCRectMake(0, 0, 32/ratio, 32/ratio)),"button_normal.png");
		pCache->addSpriteFrame(CCSpriteFrame::create((GetBaseAppPath()+"interface/button_selected.png").c_str(),CCRectMake(0, 0, 32/ratio, 32/ratio)),"button_selected.png");
		
		for (i=1; i<=3; i++)
		{
		   pPoint = CCSprite::createWithSpriteFrameName("button_normal.png");
		   pPoint->setTag(i);
		   pPoint->setPosition(ccp( (visibleSize.width/2 - pPoint->getContentSize().width)+ pPoint->getContentSize().width * (i-1), 30));
		   this->addChild(pPoint,1);
		}
		
		pPoint = (CCSprite *)this->getChildByTag(1);
		pPoint->setDisplayFrame(pCache->spriteFrameByName("button_selected.png"));

		bRet = true;
	}

	return bRet;
}

void GuiScroll::onEnter()
{
	CCLayer::onEnter();

	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 1, false);
}

void GuiScroll::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();

	if( m_pScrollView )
		m_pScrollView->release();
	
	CCLayer::onExit();
}

bool GuiScroll::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	m_touchPoint = CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView());
	return true;
}

void GuiScroll::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	
}

void GuiScroll::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint endPoint = CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView());
	float distance = endPoint.x - m_touchPoint.x;
	if(fabs(distance) > 10)
	{
		adjustScrollView(distance);
	}
}

void GuiScroll::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	CCPoint endPoint = CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView());
	float distance = endPoint.x - m_touchPoint.x;
	if(fabs(distance) > 10)
	{
		adjustScrollView(distance);
	}
}

void GuiScroll::scrollViewDidScroll(cocos2d::extension::CCScrollView *view)
{
	CCLOG("scroll");
}

void GuiScroll::scrollViewDidZoom(cocos2d::extension::CCScrollView *view)
{
	CCLOG("zoom");
}

void GuiScroll::adjustScrollView(float offset)
{
	CCPoint				adjustPos;
	CCSize				visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint				origin		= CCDirector::sharedDirector()->getVisibleOrigin();
	CCSpriteFrameCache*	pCache		= CCSpriteFrameCache::sharedSpriteFrameCache();
	CCSprite*			pPoint		= (CCSprite *)this->getChildByTag(m_nCurPage);
	
	pPoint->setDisplayFrame(pCache->spriteFrameByName("button_normal.png"));
	
	if ( offset < 0 )
	{
		m_nCurPage ++;
	}
	else
	{
		m_nCurPage --;
	}

	if (m_nCurPage < 1)
	{
		m_nCurPage = 1;
	}
	else if(m_nCurPage > 3)
	{
		m_nCurPage = 3;
	}
		
	pPoint = (CCSprite *)this->getChildByTag(m_nCurPage);
	pPoint->setDisplayFrame(pCache->spriteFrameByName("button_selected.png"));
	adjustPos = ccp(origin.x - visibleSize.width * (m_nCurPage-1), 0);
	m_pScrollView->setContentOffset(adjustPos, true);
}
