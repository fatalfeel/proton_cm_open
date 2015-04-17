#include "PlatformPrecomp.h"
#include "Irrlicht/IrrlichtManager.h"
using namespace irr;
using namespace scene;
using namespace io;

#include "App.h"
#include "MainMenu.h"
#include "Mesh3DMenu.h"

#include "TerrainMenu.h"
#include "QuakeMenu.h"
#include "QuakeShaderMenu.h"
#include "HouseMenu.h"
#include "ShaderMenu.h"
#include "StencilMenu.h"

//by stone
enum EnMenuSelect
{
    ANIMESH,
    TERRAIN,
    QUAKE,
    QUAKESHADER,
    HOUSESCENE,
    HIGHSHADER,
	STENCIL
};

Entity* MainMenuCreate(Entity* pParentEnt)
{
    Entity* pBG;
	EnMenuSelect menuid = STENCIL;
    
    switch(menuid)
    {
        case ANIMESH:
            pBG = Mesh3DMenuCreate(pParentEnt);
            break;
        
        case TERRAIN:
            pBG = TerrainMenuCreate(pParentEnt);
            break;
            
        case QUAKE:
            pBG = QuakeMenuCreate(pParentEnt);
            break;
            
        case QUAKESHADER:
            pBG = QuakeShaderMenuCreate(pParentEnt);
            break;
            
        case HOUSESCENE:
            pBG = HouseMenuCreate(pParentEnt);
            break;
            
        case HIGHSHADER:	//plz enable irr::video::EDT_OGLES2
            pBG = ShaderMenuCreate(pParentEnt);
            break;

        case STENCIL:		//plz enable irr::video::EDT_OGLES2
            pBG = StencilMenuCreate(pParentEnt);
            break;
    }

	return pBG;
}