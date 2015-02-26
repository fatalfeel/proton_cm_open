/*
 *  App.h
 *  Created by Seth Robinson on 4/27/11.
 *  For license info, check the license.txt file that should have come with this.
 *
 */

/*
RTPhysics - an unpolished demonstration of using Proton with Irrlicht + IrrBullet + Bullet.  It stacks some boxes and
lets you move around and shoot more boxes at the stack.

I ended up not using IrrBullet and bullet for my current project, but kept this example around anyway.

-Seth
*/

#pragma once

#include "BaseApp.h"
#include "EDriverTypes.h" //by stone

class App: public BaseApp
{
public:
	
	App();
	virtual ~App();
	
	virtual bool Init();
	virtual void Kill();
	virtual void Draw();
	virtual void OnScreenSizeChange();
	virtual void Update();

	string GetVersionString();
	float GetVersion();
	int GetBuild();
	void GetServerInfo(string &server, uint32 &port);
	VariantDB * GetShared() {return &m_varDB;}
	Variant * GetVar(const string &keyName );
	Variant * GetVarWithDefault(const string &varName, const Variant &var) {return m_varDB.GetVarWithDefault(varName, var);}
	int GetSpecial();
	virtual void	OnEnterBackground();
	virtual void	OnEnterForeground();
	void			OnExitApp(VariantList *pVarList);
	void			SaveStuff();
	Entity*			GetMainScene() {return m_MenuEntity;}

private:

	bool		m_bDidPostInit;
	VariantDB	m_varDB; //holds all data we want to save/load
	int			m_special;
	Entity*		m_MenuEntity;	//by stone, high level shader used
};

extern irr::video::E_DRIVER_TYPE AppGetOGLESType();

extern AudioManager*	GetAudioManager();  //supply this yourself
extern void             FreeAudioManager();

App*					GetApp();
extern const char*		GetAppName();
extern const char*		GetBundleName();
extern const char*		GetBundlePrefix();
