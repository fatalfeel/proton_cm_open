#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

class HelloWorld : public cocos2d::CCLayer
{
public:
	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static CCScene* scene();
	static HelloWorld* create();

	HelloWorld();
	~HelloWorld();

	virtual bool init();  
	virtual void onEnter();
	virtual void onExit();

	// a selector callback
	void menuCallback(CCObject* pSender);
		
	CCSprite*	m_grossini;
	int			m_runact;
};

#endif // __HELLOWORLD_SCENE_H__
