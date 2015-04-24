#ifndef __SCROLL_SCENE_H__
#define __SCROLL_SCENE_H__

class GuiScroll : public cocos2d::CCLayer, public CCScrollViewDelegate
{
public:
    static CCScene*		scene();
	static GuiScroll*	create();

	virtual bool init();
    
	void scrollViewDidScroll(CCScrollView* view);
	void scrollViewDidZoom(CCScrollView* view);

	virtual void onEnter();
	virtual void onExit();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

private:
	 void			adjustScrollView(float offset);
	 CCScrollView*	m_pScrollView;
	 CCPoint		m_touchPoint;
	 int			m_nCurPage;
};

#endif // __HELLOWORLD_SCENE_H__
