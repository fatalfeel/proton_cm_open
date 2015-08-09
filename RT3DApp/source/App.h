/*
 *  App.h
 *  Created by Seth Robinson on 3/6/09.
 *  For license info, check the license.txt file that should have come with this.
 *
 */

#pragma once

#include "BaseApp.h"
#include "EDriverTypes.h" //by stone

class App: public BaseApp
{
public:
	App();
	virtual ~App();
	
	virtual bool	Init();
	virtual void	Draw();
	virtual void	Update();
	virtual void	CheckInitAgain();
	virtual void	OnEnterBackground();
	virtual void	OnEnterForeground();

	void			OnUnloadSurfaces();
	void			OnReLoadSurfaces();
	Entity*			GetMainScene();
	string			GetVersionString();
	float			GetVersion();
	int				GetBuild();
	void			GetServerInfo(string &server, uint32 &port);
	VariantDB*		GetShared() {return &m_varDB;}
	Variant*		GetVar(const string &keyName );
	Variant*		GetVarWithDefault(const string &varName, const Variant &var) {return m_varDB.GetVarWithDefault(varName, var);}
	int				GetSpecial();
	void			OnExitApp(VariantList *pVarList);
    void            SaveStuff();
	
	int				getUnUsedIndex();
	void			removeUsedIndexBit(int index);
	void			HandleTouchesBegin(int num, int ids[], float xs[], float ys[]);
	void			HandleTouchesMove(int num, int ids[], float xs[], float ys[]);
    void			HandleTouchesEnd(int num, int ids[], float xs[], float ys[]);

private:
	bool			m_bDidPostInit;
	//int			m_special;
	int				m_connect_set; //by jesse stone
	int				m_initagain;
	unsigned int	m_indexBitsUsed;
	
	VariantDB		m_varDB;		//holds all data we want to save/load
	Entity*			m_MenuEntity;	//by stone, high level shader used
};

extern irr::video::E_DRIVER_TYPE AppGetOGLESType();

extern App*				g_pApp;
extern AudioManager*	GetAudioManager();  //supply this yourself
extern void             FreeAudioManager();

extern const wchar_t*	GetAppName();
extern const char*		GetBundleName();
extern const char*		GetBundlePrefix();

extern std::wstring StringToWstring(std::string str);
extern std::string  WstringToString(std::wstring str);
extern void			WideStrToUTF8(std::string& dest, wstring& src);
