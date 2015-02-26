#ifndef ShaderMenu_h__
#define ShaderMenu_h__

#include "App.h"

class CShaderMenu : public Entity
{
public:

	CShaderMenu();
	virtual ~CShaderMenu();

	void InitScene();
	void OnUnLoad(); //SETH
	void OnReLoad();

private:
	scene::ISceneNode*	m_node_a;
	scene::ISceneNode*	m_node_b;
	scene::ISceneNode*	m_node_c;

#ifdef _IRR_COMPILE_WITH_GUI_
	scene::ITextSceneNode*	m_textA;
	scene::ITextSceneNode*	m_textB;
	scene::ITextSceneNode*	m_textC;
#endif
};

Entity* ShaderMenuCreate(Entity *pParentEnt);

#endif // ShaderMenu_h__