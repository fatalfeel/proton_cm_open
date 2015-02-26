// Copyright (C) 2013 Patryk Nadrowski
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// OpenGL ES driver implemented by Christian Stehno and first OpenGL ES 2.0
// driver implemented by Amundis.
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES2_PARALLAX_MAP_RENDERER_H_INCLUDED__
#define __C_OGLES2_PARALLAX_MAP_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2MaterialRenderer.h"

namespace irr
{
namespace video
{

//! Class for parallax mapping in OpenGL ES 2.0
class COGLES2MaterialParallaxMapCB : public IShaderConstantSetCallBack
{
public:
	COGLES2MaterialParallaxMapCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;
	s32 WVPMatrixID;
	s32 WVMatrixID;
	s32 EyePositionID;
	s32 LightPositionID;
	s32 LightColorID;
	s32 FactorID;
	s32 TextureUnit0ID;
	s32 TextureUnit1ID;
	s32 FogEnableID;
	s32 FogTypeID;
	s32 FogColorID;
	s32 FogStartID;
	s32 FogEndID;
	s32 FogDensityID;

	core::vector3df LightPosition[2];
	SColorf LightColor[2];
	f32 Factor;
	s32 TextureUnit0;
	s32 TextureUnit1;
	s32 FogEnable;
	s32 FogType;
	SColorf FogColor;
	f32 FogStart;
	f32 FogEnd;
	f32 FogDensity;
};

} // end namespace video
} // end namespace irr

#endif
#endif


