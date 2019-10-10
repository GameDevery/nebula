//------------------------------------------------------------------------------
//  imgui.fx
//
//	Shader for ImGUI rendering
//
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/techniques.fxh" 
#include "lib/shared.fxh"
#include "lib/defaultsamplers.fxh"

// put variables in push-constant block
push varblock ImGUI [ string Visibility = "PS|VS"; ]
{
	textureHandle Texture; 
	mat4 TextProjectionModel;
	uint PackedTextureInfo;
};

group(BATCH_GROUP) samplerstate TextureSampler
{
	Filter = Point;
};

state TextState
{
	BlendEnabled[0] = true;
	SrcBlend[0] = SrcAlpha;
	DstBlend[0] = OneMinusSrcAlpha;
	DepthWrite = false;
	DepthEnabled = false;
	CullMode = None;
	ScissorEnabled = true;
};

//------------------------------------------------------------------------------
/**
*/
void UnpackTexture(uint val, out uint id, out uint type, out uint mip, out uint layer)
{
	type = val & 0xF;
	layer = (val >> 4) & 0xFF;
	mip = (val >> 12) & 0xFF;
	id = (val >> 20) & 0xFFF;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsMain(
	[slot=0] in vec2 position,
	[slot=1] in vec2 uv,
	[slot=2] in vec4 color,	
	out vec2 UV,
	out vec4 Color) 
{
	vec4 pos = vec4(position, 0, 1);	
	gl_Position = ImGUI.TextProjectionModel * pos;
	Color = color;
	UV = uv;
}

//------------------------------------------------------------------------------
/** 
*/
shader
void
psMain(
	in vec2 UV,
	in vec4 Color,
	[color0] out vec4 FinalColor) 
{
	vec4 texColor;
	uint id, type, layer, mip;
	UnpackTexture(ImGUI.PackedTextureInfo, id, type, mip, layer);
	if (type == 0)
		texColor = sample2DLod(id, TextureSampler, UV, mip);
	else
		texColor = sample2DArrayLod(id, TextureSampler, vec3(UV, layer), mip);
	FinalColor = Color * texColor;
}


//------------------------------------------------------------------------------
/**
*/
SimpleTechnique(Default, "Static", vsMain(), psMain(), TextState);