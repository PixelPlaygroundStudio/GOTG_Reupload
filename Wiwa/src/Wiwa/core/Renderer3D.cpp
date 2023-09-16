#include <wipch.h>
#include "Renderer3D.h"

#include <Wiwa/core/Application.h>
#include <Wiwa/utilities/Log.h>
#include <Wiwa/utilities/render/CameraManager.h>
#include <Wiwa/scene/SceneManager.h>
#include <glew.h>
#include <Wiwa/utilities/render/shaders/Shader.h>
#include <Wiwa/utilities/render/animator.h>

#include <Wiwa/ecs/systems/LightSystem.h>
#include <Wiwa/utilities/render/LightManager.h>
#include <ozz/geometry/runtime/skinning_job.h>

const uint8_t kDefaultColorsArray[][4] = {
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
	{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255}
};

const float kDefaultNormalsArray[][3] = {
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
	{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f} };

const float kDefaultUVsArray[][2] = {
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f},
	{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f} };

namespace Wiwa
{
	Renderer3D::Renderer3D() :
		dynamic_array_bo_(0),
		dynamic_index_bo_(0),
		dynamic_vao_(0)
	{
		
	}

	Renderer3D::~Renderer3D()
	{
		if (dynamic_vao_) {
			GL(DeleteVertexArrays(1, &dynamic_vao_));
			dynamic_vao_ = 0;
		}

		if (dynamic_array_bo_) {
			GL(DeleteBuffers(1, &dynamic_array_bo_));
			dynamic_array_bo_ = 0;
		}

		if (dynamic_index_bo_) {
			GL(DeleteBuffers(1, &dynamic_index_bo_));
			dynamic_index_bo_ = 0;
		}
	}

	bool Renderer3D::Init()
	{
		// ========= BEG OZZ ANIMATIONS =========
		// Builds the dynamic vbo
		GL(GenVertexArrays(1, &dynamic_vao_));
		GL(GenBuffers(1, &dynamic_array_bo_));
		GL(GenBuffers(1, &dynamic_index_bo_));

		ResourceId anim_shader_id = Wiwa::Resources::Load<Shader>("resources/shaders/animation/animation_skinned_textured");
		Shader* anim_shader = Wiwa::Resources::GetResourceById<Shader>(anim_shader_id);
		anim_shader->Compile("resources/shaders/animation/animation_skinned_textured");
		anim_shader->addUniform("u_Texture", UniformType::Sampler2D);
		anim_shader->addUniform("u_Color", UniformType::fVec4);
		anim_shader->addUniform("u_Hit", UniformType::Int);
		anim_shader->addUniform("u_OutlineColor", UniformType::fVec4);
		anim_shader->addUniform("u_OutlineSmoothRange", UniformType::fVec2);
		anim_shader->addUniform("u_ToonLevels", UniformType::Int);
		anim_shader->addUniform("u_RimLightPower", UniformType::Float);
		anim_shader->addUniform("u_SpecularValue", UniformType::Float);
		anim_shader->addUniform("u_MatAmbientColor", UniformType::fVec4);
		anim_shader->addUniform("u_MatDiffuseColor", UniformType::fVec4);
		anim_shader->addUniform("u_MatSpecularColor", UniformType::fVec4);
		Wiwa::Resources::Import<Shader>("resources/shaders/animation/animation_skinned_textured", anim_shader);

		// ========= END OZZ ANIMATIONS =========

		Size2i &resolution = Application::Get().GetTargetResolution();

		WI_CORE_INFO("Renderer3D initialized");
		SetOption(Options::DEPTH_TEST);
		SetOption(Options::CULL_FACE);

		// Init default shaders with uniforms
		ResourceId textShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/light/toon_textured");
		Shader *textShader = Wiwa::Resources::GetResourceById<Shader>(textShaderId);
		textShader->Compile("resources/shaders/light/toon_textured");
		textShader->addUniform("u_Texture", UniformType::Sampler2D);
		textShader->addUniform("u_ToonLevels", UniformType::Int);
		textShader->addUniform("u_RimLightPower", UniformType::Float);
		textShader->addUniform("u_SpecularValue", UniformType::Float);
		textShader->addUniform("u_MatAmbientColor", UniformType::fVec4);
		textShader->addUniform("u_MatDiffuseColor", UniformType::fVec4);
		textShader->addUniform("u_MatSpecularColor", UniformType::fVec4);
		Wiwa::Resources::Import<Shader>("resources/shaders/light/toon_textured", textShader);

		// Init skinned
		ResourceId skinnedShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/skinned/skinned");
		Shader *skinnedShader = Wiwa::Resources::GetResourceById<Shader>(skinnedShaderId);

		skinnedShader->Compile("resources/shaders/skinned/skinned");
		Wiwa::Resources::Import<Shader>("resources/shaders/skinned/skinned", skinnedShader);
		// Init skinned textured
		ResourceId skinnedTexturedShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/skinned/skinned_textured");
		Shader* skinnedTexturedShader = Wiwa::Resources::GetResourceById<Shader>(skinnedTexturedShaderId);
		skinnedTexturedShader->Compile("resources/shaders/skinned/skinned_textured");
		skinnedTexturedShader->addUniform("u_Texture", UniformType::Sampler2D);
		skinnedTexturedShader->addUniform("u_ToonLevels", UniformType::Int);
		skinnedTexturedShader->addUniform("u_RimLightPower", UniformType::Float);
		skinnedTexturedShader->addUniform("u_SpecularValue", UniformType::Float);
		skinnedTexturedShader->addUniform("u_MatAmbientColor", UniformType::fVec4);
		skinnedTexturedShader->addUniform("u_MatDiffuseColor", UniformType::fVec4);
		skinnedTexturedShader->addUniform("u_MatSpecularColor", UniformType::fVec4);
		Wiwa::Resources::Import<Shader>("resources/shaders/skinned/skinned_textured", skinnedTexturedShader);
		// Init skinned textured outlined
		ResourceId skinnedTexturedOutlinedShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/skinned/skinned_textured_outlined");
		Shader* skinnedTexturedOutlinedShader = Wiwa::Resources::GetResourceById<Shader>(skinnedTexturedOutlinedShaderId);
		skinnedTexturedOutlinedShader->Compile("resources/shaders/skinned/skinned_textured_outlined");
		skinnedTexturedOutlinedShader->addUniform("u_Color", UniformType::fVec4);
		skinnedTexturedOutlinedShader->addUniform("u_Texture", UniformType::Sampler2D);
		skinnedTexturedOutlinedShader->addUniform("u_OutlineColor", UniformType::fVec4);
		skinnedTexturedOutlinedShader->addUniform("u_OutlineSmoothRange", UniformType::fVec2);
		skinnedTexturedOutlinedShader->addUniform("u_ToonLevels", UniformType::Int);
		skinnedTexturedOutlinedShader->addUniform("u_RimLightPower", UniformType::Float);
		skinnedTexturedOutlinedShader->addUniform("u_SpecularValue", UniformType::Float);
		skinnedTexturedOutlinedShader->addUniform("u_MatAmbientColor", UniformType::fVec4);
		skinnedTexturedOutlinedShader->addUniform("u_MatDiffuseColor", UniformType::fVec4);
		skinnedTexturedOutlinedShader->addUniform("u_MatSpecularColor", UniformType::fVec4);
		skinnedTexturedOutlinedShader->addUniform("u_Hit", UniformType::Bool);
		Wiwa::Resources::Import<Shader>("resources/shaders/skinned/skinned_textured_outlined", skinnedTexturedOutlinedShader);
		//init skinned Outline
		ResourceId skinnedOutlineShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/skinned/skinned_outline");
		Shader* skinnedOutlineShader = Wiwa::Resources::GetResourceById<Shader>(skinnedOutlineShaderId);
		skinnedOutlineShader->Compile("resources/shaders/skinned/skinned_outline");
		skinnedOutlineShader->addUniform("u_Texture", UniformType::Sampler2D);
		skinnedOutlineShader->addUniform("u_OutlineColor", UniformType::fVec4);
		skinnedOutlineShader->addUniform("u_OutlineSmoothRange", UniformType::fVec2);
		Wiwa::Resources::Import<Shader>("resources/shaders/skinned/skinned_outline", skinnedOutlineShader);
		// Init skinned depth
		ResourceId skinnedDepthShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/skinned/skinned_depth");
		Shader* skinnedDepthShader = Wiwa::Resources::GetResourceById<Shader>(skinnedDepthShaderId);
		skinnedDepthShader->Compile("resources/shaders/skinned/skinned_depth");
		skinnedDepthShader->addUniform("u_Texture", UniformType::Sampler2D);
		Wiwa::Resources::Import<Shader>("resources/shaders/skinned/skinned_depth", skinnedDepthShader);
		// Init bone debug
		ResourceId debugBonesShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/skinned/debug_bones");
		Shader *debugBoneShader = Wiwa::Resources::GetResourceById<Shader>(debugBonesShaderId);
		debugBoneShader->Compile("resources/shaders/skinned/debug_bones");
		debugBoneShader->addUniform("u_DisplayBoneIndex", UniformType::Int);
		Wiwa::Resources::Import<Shader>("resources/shaders/skinned/debug_bones", debugBoneShader);

		ResourceId colorShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/light/toon_color");
		Shader *colorShader = Wiwa::Resources::GetResourceById<Shader>(colorShaderId);
		colorShader->Compile("resources/shaders/light/toon_color");

		colorShader->addUniform("u_Color", UniformType::fVec4);
		colorShader->addUniform("u_ToonLevels", UniformType::Int);
		colorShader->addUniform("u_RimLightPower", UniformType::Float);
		colorShader->addUniform("u_SpecularValue", UniformType::Float);
		colorShader->addUniform("u_MatAmbientColor", UniformType::fVec4);
		colorShader->addUniform("u_MatDiffuseColor", UniformType::fVec4);
		colorShader->addUniform("u_MatSpecularColor", UniformType::fVec4);
		Wiwa::Resources::Import<Shader>("resources/shaders/light/toon_color", colorShader);
		//toon textured outlined
		ResourceId outlinedShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/light/toon_textured_outlined");
		Shader* outlinedShader = Wiwa::Resources::GetResourceById<Shader>(outlinedShaderId);
		outlinedShader->Compile("resources/shaders/light/toon_textured_outlined");
		outlinedShader->addUniform("u_Texture", UniformType::Sampler2D);
		outlinedShader->addUniform("u_OutlineColor", UniformType::fVec4);
		outlinedShader->addUniform("u_OutlineSmoothRange", UniformType::fVec2);
		outlinedShader->addUniform("u_ToonLevels", UniformType::Int);
		outlinedShader->addUniform("u_RimLightPower", UniformType::Float);
		outlinedShader->addUniform("u_SpecularValue", UniformType::Float);
		outlinedShader->addUniform("u_MatAmbientColor", UniformType::fVec4);
		outlinedShader->addUniform("u_MatDiffuseColor", UniformType::fVec4);
		outlinedShader->addUniform("u_MatSpecularColor", UniformType::fVec4);
		Wiwa::Resources::Import<Shader>("resources/shaders/light/toon_textured_outlined", outlinedShader);
		//toon textured outlined cartoon
		ResourceId outlinedCartoonShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/light/toon_textured_outlined_cartoon");
		Shader* outlineCartoondShader = Wiwa::Resources::GetResourceById<Shader>(outlinedCartoonShaderId);
		outlineCartoondShader->Compile("resources/shaders/light/toon_textured_outlined_cartoon");
		outlineCartoondShader->addUniform("u_Texture", UniformType::Sampler2D);
		outlineCartoondShader->addUniform("u_LayeredText", UniformType::Sampler2D);
		outlineCartoondShader->addUniform("u_OutlineColor", UniformType::fVec4);
		outlineCartoondShader->addUniform("u_OutlineSmoothRange", UniformType::fVec2);
		outlineCartoondShader->addUniform("u_ToonLevels", UniformType::Int);
		outlineCartoondShader->addUniform("u_RimLightPower", UniformType::Float);
		outlineCartoondShader->addUniform("u_SpecularValue", UniformType::Float);
		outlineCartoondShader->addUniform("u_MatAmbientColor", UniformType::fVec4);
		outlineCartoondShader->addUniform("u_MatDiffuseColor", UniformType::fVec4);
		outlineCartoondShader->addUniform("u_MatSpecularColor", UniformType::fVec4);
		Wiwa::Resources::Import<Shader>("resources/shaders/light/toon_textured_outlined_cartoon", outlineCartoondShader);

		ResourceId basecolorShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/light/base_color");
		Shader* basecolorShader = Wiwa::Resources::GetResourceById<Shader>(basecolorShaderId);
		basecolorShader->Compile("resources/shaders/light/base_color");

		basecolorShader->addUniform("u_Color", UniformType::fVec4);
		
		Wiwa::Resources::Import<Shader>("resources/shaders/light/base_color", basecolorShader);

		//===========================================================================================================
		// VFX shaders
		//===========================================================================================================

		ResourceId dissolveShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/dissolve");
		Shader* dissolveShader = Wiwa::Resources::GetResourceById<Shader>(dissolveShaderId);
		dissolveShader->Compile("resources/shaders/vfx/dissolve");

		dissolveShader->addUniform("u_LifeTime", UniformType::Float);
		dissolveShader->addUniform("u_Color", UniformType::fVec4);
		dissolveShader->addUniform("u_Texture", UniformType::Sampler2D);
		dissolveShader->addUniform("u_DissolveAmount", UniformType::Float);
		dissolveShader->addUniform("u_DiscardTex", UniformType::Sampler2D);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/dissolve", dissolveShader);
		
		ResourceId smokeShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/smoke");

		Shader* smokeShader = Wiwa::Resources::GetResourceById<Shader>(smokeShaderId);
		smokeShader->Compile("resources/shaders/vfx/smoke");

		smokeShader->addUniform("u_LifeTime", UniformType::Float);
		smokeShader->addUniform("u_Color", UniformType::fVec4);
		smokeShader->addUniform("u_Texture", UniformType::Sampler2D);
		smokeShader->addUniform("u_DissolveAmount", UniformType::Float);
		smokeShader->addUniform("u_EnterDissolve", UniformType::Float);
		smokeShader->addUniform("u_OutDissolve", UniformType::Float);
		smokeShader->addUniform("u_DiscardTex", UniformType::Sampler2D);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/smoke", smokeShader);

		ResourceId shieldShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/shield");
		Shader* shieldShader = Wiwa::Resources::GetResourceById<Shader>(shieldShaderId);
		shieldShader->Compile("resources/shaders/vfx/shield");

		shieldShader->addUniform("u_LifeTime", UniformType::Float);
		shieldShader->addUniform("u_Color", UniformType::fVec4);
		shieldShader->addUniform("u_FresnelColor", UniformType::fVec4);
		shieldShader->addUniform("u_FresnelRange", UniformType::fVec2);
		shieldShader->addUniform("u_Texture", UniformType::Sampler2D);
		shieldShader->addUniform("u_DiscardTex", UniformType::Sampler2D);
		shieldShader->addUniform("u_HologramTexture", UniformType::Sampler2D);
		shieldShader->addUniform("u_HologramColor", UniformType::fVec4);
		shieldShader->addUniform("u_Amplitude", UniformType::Float);
		shieldShader->addUniform("u_Frequency", UniformType::Float);
		shieldShader->addUniform("u_StartDissolve", UniformType::Float);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/shield", shieldShader);

		ResourceId laserShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/laser");
		Shader* laserShader = Wiwa::Resources::GetResourceById<Shader>(laserShaderId);
		laserShader->Compile("resources/shaders/vfx/laser");

		laserShader->addUniform("u_LifeTime", UniformType::Float);
		laserShader->addUniform("u_Color", UniformType::fVec4);
		laserShader->addUniform("u_FresnelColor", UniformType::fVec4);
		laserShader->addUniform("u_FresnelRange", UniformType::fVec2);
		laserShader->addUniform("u_HologramColor", UniformType::fVec4);
		laserShader->addUniform("u_Speed", UniformType::Float);
		laserShader->addUniform("u_Texture", UniformType::Sampler2D);
		laserShader->addUniform("u_DiscardTex", UniformType::Sampler2D);
		laserShader->addUniform("u_HologramTexture", UniformType::Sampler2D);
		laserShader->addUniform("u_StartDissolve", UniformType::Float);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/laser", laserShader);

		ResourceId justTextureShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/justtexture");
		Shader* justTextureShader = Wiwa::Resources::GetResourceById<Shader>(justTextureShaderId);
		justTextureShader->Compile("resources/shaders/vfx/justtexture");

		justTextureShader->addUniform("u_LifeTime", UniformType::Float);
		justTextureShader->addUniform("u_Color", UniformType::fVec4);
		justTextureShader->addUniform("u_Texture", UniformType::Sampler2D);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/justtexture", justTextureShader);

		ResourceId justTextureAlphaShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/justtextureAlpha");
		Shader* justTextureAlphaShader = Wiwa::Resources::GetResourceById<Shader>(justTextureAlphaShaderId);
		justTextureAlphaShader->Compile("resources/shaders/vfx/justtextureAlpha");

		justTextureAlphaShader->addUniform("u_LifeTime", UniformType::Float);
		justTextureAlphaShader->addUniform("u_Color", UniformType::fVec4);
		justTextureAlphaShader->addUniform("u_Texture", UniformType::Sampler2D);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/justtextureAlpha", justTextureAlphaShader);

		ResourceId impactShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/shot_impact");
		Shader* impactShader = Wiwa::Resources::GetResourceById<Shader>(impactShaderId);
		impactShader->Compile("resources/shaders/vfx/shot_impact");

		impactShader->addUniform("u_LifeTime", UniformType::Float);
		impactShader->addUniform("u_Color", UniformType::fVec4);
		impactShader->addUniform("u_Texture", UniformType::Sampler2D);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/barrel_explosion/explosion", impactShader);

		ResourceId explosionShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/barrel_explosion/explosion");
		Shader* explosionShader = Wiwa::Resources::GetResourceById<Shader>(explosionShaderId);
		explosionShader->Compile("resources/shaders/vfx/barrel_explosion/explosion");

		explosionShader->addUniform("u_LifeTime", UniformType::Float);
		explosionShader->addUniform("u_Color", UniformType::fVec4);
		explosionShader->addUniform("u_FresnelColor", UniformType::fVec4);
		explosionShader->addUniform("u_FresnelRange", UniformType::fVec2);
		explosionShader->addUniform("u_Texture", UniformType::Sampler2D);
		explosionShader->addUniform("u_DiscardTex", UniformType::Sampler2D);
		explosionShader->addUniform("u_HologramTexture", UniformType::Sampler2D);
		explosionShader->addUniform("u_HologramColor", UniformType::fVec4);
		explosionShader->addUniform("u_Amplitude", UniformType::Float);
		explosionShader->addUniform("u_Frequency", UniformType::Float);
		explosionShader->addUniform("u_StartDissolve", UniformType::Float);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/barrel_explosion/explosion", impactShader);

		ResourceId bulletShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/bullet/bullet");
		Shader* bulletShader = Wiwa::Resources::GetResourceById<Shader>(bulletShaderId);
		bulletShader->Compile("resources/shaders/vfx/bullet/bullet");

		bulletShader->addUniform("u_LifeTime", UniformType::Float);
		bulletShader->addUniform("u_Time", UniformType::Float);
		bulletShader->addUniform("u_Color", UniformType::fVec4);
		bulletShader->addUniform("u_Texture", UniformType::Sampler2D);
		bulletShader->addUniform("u_TransparencyTexture", UniformType::Sampler2D);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/bullet/bullet", impactShader);

		ResourceId bulletTrailShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/bullet/bullet_trail");
		Shader* bulletTrailShader = Wiwa::Resources::GetResourceById<Shader>(bulletTrailShaderId);
		bulletTrailShader->Compile("resources/shaders/vfx/bullet/bullet_trail");

		bulletTrailShader->addUniform("u_LifeTime", UniformType::Float);
		bulletTrailShader->addUniform("u_Time", UniformType::Float);
		bulletTrailShader->addUniform("u_Color", UniformType::fVec4);
		bulletTrailShader->addUniform("u_Texture", UniformType::Sampler2D);
		bulletTrailShader->addUniform("u_TransparencyTexture", UniformType::Sampler2D);
		bulletTrailShader->addUniform("u_Offset", UniformType::fVec2);
		bulletTrailShader->addUniform("u_OffsetMultiplier", UniformType::fVec2);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/bullet/bullet_trail", bulletTrailShader);

		ResourceId bulletPlasmaShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/bullet/bullet_plasma");
		Shader* bulletPlasmaShader = Wiwa::Resources::GetResourceById<Shader>(bulletPlasmaShaderId);
		bulletPlasmaShader->Compile("resources/shaders/vfx/bullet/bullet_plasma");

		bulletPlasmaShader->addUniform("u_LifeTime", UniformType::Float);
		bulletPlasmaShader->addUniform("u_Time", UniformType::Float);
		bulletPlasmaShader->addUniform("u_Color", UniformType::fVec4);
		bulletPlasmaShader->addUniform("u_FresnelColor", UniformType::fVec4);
		bulletPlasmaShader->addUniform("u_FresnelRange", UniformType::fVec2);
		bulletPlasmaShader->addUniform("u_Texture", UniformType::Sampler2D);
		bulletPlasmaShader->addUniform("u_TransparencyTexture", UniformType::Sampler2D);
		bulletPlasmaShader->addUniform("u_HologramTexture", UniformType::Sampler2D);
		bulletPlasmaShader->addUniform("u_HologramColor", UniformType::fVec4);
		bulletPlasmaShader->addUniform("u_Amplitude", UniformType::Float);
		bulletPlasmaShader->addUniform("u_Frequency", UniformType::Float);
		bulletPlasmaShader->addUniform("u_OffsetMultiplier", UniformType::fVec2);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/bullet/bullet_plasma", bulletPlasmaShader);

		ResourceId textureAnimatedShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/texture_animated");
		Shader* textureAnimatedShader = Wiwa::Resources::GetResourceById<Shader>(textureAnimatedShaderId);
		textureAnimatedShader->Compile("resources/shaders/vfx/texture_animated");

		textureAnimatedShader->addUniform("u_LifeTime", UniformType::Float);
		textureAnimatedShader->addUniform("u_Time", UniformType::Float);
		textureAnimatedShader->addUniform("u_Color", UniformType::fVec4);
		textureAnimatedShader->addUniform("u_Texture", UniformType::Sampler2D);
		textureAnimatedShader->addUniform("u_SpriteSize", UniformType::fVec2);
		textureAnimatedShader->addUniform("u_NumColumns", UniformType::Int);
		textureAnimatedShader->addUniform("u_NumRows", UniformType::Int);
		textureAnimatedShader->addUniform("u_NumFrames", UniformType::Int);
		textureAnimatedShader->addUniform("u_AnimationSpeed", UniformType::Float);
		textureAnimatedShader->addUniform("u_LoopAnimation", UniformType::Bool);
		textureAnimatedShader->addUniform("u_PingPong", UniformType::Bool);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/texture_animated", textureAnimatedShader);

		ResourceId bulletPhalanxRangedCoreId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/enemy_bullet/enemy_bullet");
		Shader* bulletPhalanxRangedCore = Wiwa::Resources::GetResourceById<Shader>(bulletPhalanxRangedCoreId);
		bulletPhalanxRangedCore->Compile("resources/shaders/vfx/enemy_bullet/enemy_bullet");

		bulletPhalanxRangedCore->addUniform("u_LifeTime", UniformType::Float);
		bulletPhalanxRangedCore->addUniform("u_Time", UniformType::Float);
		bulletPhalanxRangedCore->addUniform("u_Color", UniformType::fVec4);
		bulletPhalanxRangedCore->addUniform("u_FresnelColor", UniformType::fVec4);
		bulletPhalanxRangedCore->addUniform("u_FresnelRange", UniformType::fVec2);
		bulletPhalanxRangedCore->addUniform("u_Texture", UniformType::Sampler2D);
		bulletPhalanxRangedCore->addUniform("u_TransparencyTexture", UniformType::Sampler2D);
		bulletPhalanxRangedCore->addUniform("u_OffsetMultiplier", UniformType::fVec2);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/enemy_bullet/enemy_bullet", bulletPhalanxRangedCore);

		ResourceId animTexTranspShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/animated_texture_transparent");
		Shader* animTexTranspShader = Wiwa::Resources::GetResourceById<Shader>(animTexTranspShaderId);
		animTexTranspShader->Compile("resources/shaders/vfx/animated_texture_transparent");

		animTexTranspShader->addUniform("u_LifeTime", UniformType::Float);
		animTexTranspShader->addUniform("u_Time", UniformType::Float);
		animTexTranspShader->addUniform("u_Color", UniformType::fVec4);
		animTexTranspShader->addUniform("u_Velocity", UniformType::fVec2);
		animTexTranspShader->addUniform("u_Offset", UniformType::fVec2);
		animTexTranspShader->addUniform("u_Texture", UniformType::Sampler2D);
		animTexTranspShader->addUniform("u_TransparencyTexture", UniformType::Sampler2D);
		animTexTranspShader->addUniform("u_FadeTexture", UniformType::Sampler2D);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/animated_texture_transparent", animTexTranspShader);

		ResourceId enemySpawnFlashShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/enemy_spawn_flash");
		Shader* enemySpawnFlashShader = Wiwa::Resources::GetResourceById<Shader>(enemySpawnFlashShaderId);
		enemySpawnFlashShader->Compile("resources/shaders/vfx/enemy_spawn_flash");

		enemySpawnFlashShader->addUniform("u_LifeTime", UniformType::Float);
		enemySpawnFlashShader->addUniform("u_Time", UniformType::Float);
		enemySpawnFlashShader->addUniform("u_Color", UniformType::fVec4);
		enemySpawnFlashShader->addUniform("u_Texture", UniformType::Sampler2D);
		enemySpawnFlashShader->addUniform("u_DiscardTexture", UniformType::Sampler2D);
		enemySpawnFlashShader->addUniform("u_TransparencyTexture", UniformType::Sampler2D);
		enemySpawnFlashShader->addUniform("u_FresnelRange", UniformType::fVec2);
		enemySpawnFlashShader->addUniform("u_FresnelColor", UniformType::fVec4);
		enemySpawnFlashShader->addUniform("u_StartDissolve", UniformType::Float);
		enemySpawnFlashShader->addUniform("u_OffsetMultiplier", UniformType::fVec2);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/enemy_spawn_flash", enemySpawnFlashShader);

		ResourceId spriteSheetFrameShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/spritesheet_frame");
		Shader* spriteSheetFrameShader = Wiwa::Resources::GetResourceById<Shader>(spriteSheetFrameShaderId);
		spriteSheetFrameShader->Compile("resources/shaders/vfx/spritesheet_frame");

		spriteSheetFrameShader->addUniform("u_LifeTime", UniformType::Float);
		spriteSheetFrameShader->addUniform("u_Time", UniformType::Float);
		spriteSheetFrameShader->addUniform("u_Color", UniformType::fVec4);
		spriteSheetFrameShader->addUniform("u_Texture", UniformType::Sampler2D);
		spriteSheetFrameShader->addUniform("u_Position", UniformType::fVec2);
		spriteSheetFrameShader->addUniform("u_Zoom", UniformType::Float);
		//spriteSheetFrameShader->addUniform("u_SpritesheetSize", UniformType::fVec2);
		//spriteSheetFrameShader->addUniform("u_CellSize", UniformType::fVec2);
		//spriteSheetFrameShader->addUniform("u_Index", UniformType::Uint);

		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/spritesheet_frame", spriteSheetFrameShader);

		ResourceId flamesShaderId = Wiwa::Resources::Load<Shader>("resources/shaders/vfx/flames");
		Shader* flamesShader = Wiwa::Resources::GetResourceById<Shader>(flamesShaderId);
		flamesShader->Compile("resources/shaders/vfx/flames");

		flamesShader->addUniform("u_LifeTime", UniformType::Float);
		flamesShader->addUniform("u_Time", UniformType::Float);
		flamesShader->addUniform("u_Color", UniformType::fVec4);
		flamesShader->addUniform("u_FresnelColor", UniformType::fVec4);
		flamesShader->addUniform("u_FresnelRange", UniformType::fVec2);
		flamesShader->addUniform("u_CameraPosition", UniformType::fVec3);
		flamesShader->addUniform("u_Texture", UniformType::Sampler2D);
		flamesShader->addUniform("u_Texture_Velocity", UniformType::fVec2);
		flamesShader->addUniform("u_TranspTexture", UniformType::Sampler2D);
		flamesShader->addUniform("u_TranspMask", UniformType::Sampler2D);
		flamesShader->addUniform("u_TranspTex_Velocity", UniformType::fVec2);
		flamesShader->addUniform("u_TranspTexMask_Velocity", UniformType::fVec2);



		Wiwa::Resources::Import<Shader>("resources/shaders/vfx/flames", flamesShader);

		//===========================================================================================================

		// Normal Display Shader
		m_NormalDisplayShaderId = Resources::Load<Shader>("resources/shaders/debug/normal_display");
		m_NormalDisplayShader = Resources::GetResourceById<Shader>(m_NormalDisplayShaderId);
		m_NormalDisplayShader->Compile("resources/shaders/debug/normal_display");

		m_NDSUniforms.Model = m_NormalDisplayShader->getUniformLocation("u_Model");
		m_NDSUniforms.View = m_NormalDisplayShader->getUniformLocation("u_View");
		m_NDSUniforms.Projection = m_NormalDisplayShader->getUniformLocation("u_Projection");
		// Bounding box
		m_BBDisplayShaderId = Resources::Load<Shader>("resources/shaders/debug/boundingbox_display");
		m_BBDisplayShader = Resources::GetResourceById<Shader>(m_BBDisplayShaderId);
		m_BBDisplayShader->Compile("resources/shaders/debug/boundingbox_display");
		m_BBDSUniforms.Model = m_BBDisplayShader->getUniformLocation("u_Model");
		m_BBDSUniforms.View = m_BBDisplayShader->getUniformLocation("u_View");
		m_BBDSUniforms.Projection = m_BBDisplayShader->getUniformLocation("u_Proj");

		// Particle Shader ==> Temporal hasta materiales
		m_ParticleShaderId = Resources::Load<Shader>("resources/shaders/particle_display");
		m_ParticleShader = Resources::GetResourceById<Shader>(m_ParticleShaderId);
		m_ParticleShader->Compile("resources/shaders/particle_display");
		m_ParticleShader->addUniform("u_Texture", Wiwa::UniformType::Sampler2D);
		m_ParticleShader->addUniform("u_ColorP", Wiwa::UniformType::fVec4);
		m_ParticleUniforms.Model = m_ParticleShader->getUniformLocation("u_Model");
		m_ParticleUniforms.View = m_ParticleShader->getUniformLocation("u_View");
		m_ParticleUniforms.Projection = m_ParticleShader->getUniformLocation("u_Proj");

		m_DepthShaderId = Resources::Load<Shader>("resources/shaders/light/depth");
		m_DepthShader = Resources::GetResourceById<Shader>(m_DepthShaderId);
		m_DepthShader->Compile("resources/shaders/light/depth");
		m_DepthShaderUniforms.Model = m_DepthShader->getUniformLocation("u_Model");
		m_DepthShaderUniforms.View = m_DepthShader->getUniformLocation("u_View");
		m_DepthShaderUniforms.Projection = m_DepthShader->getUniformLocation("u_Proj");
		
		m_HDRShaderId = Resources::Load<Shader>("resources/shaders/renderlayer/hdr");
		m_HDRShader = Resources::GetResourceById<Shader>(m_HDRShaderId);
		m_HDRShader->Compile("resources/shaders/renderlayer/hdr");
		m_HDRShader->addUniform("u_HdrBufferTexture", Wiwa::UniformType::Sampler2D);
		m_HDRUniforms.Model = m_HDRShader->getUniformLocation("u_Model");
		m_HDRUniforms.View = m_HDRShader->getUniformLocation("u_View");
		m_HDRUniforms.Projection = m_HDRShader->getUniformLocation("u_Proj");

		m_BlurShaderId = Resources::Load<Shader>("resources/shaders/renderlayer/blur");
		m_BlurShader = Resources::GetResourceById<Shader>(m_BlurShaderId);
		m_BlurShader->Compile("resources/shaders/renderlayer/blur");
		m_BlurUniforms.Model = m_BlurShader->getUniformLocation("u_Model");
		m_BlurUniforms.View = m_BlurShader->getUniformLocation("u_View");
		m_BlurUniforms.Projection = m_BlurShader->getUniformLocation("u_Proj");

		m_BloomShaderId = Resources::Load<Shader>("resources/shaders/renderlayer/bloom");
		m_BloomShader = Resources::GetResourceById<Shader>(m_BloomShaderId);
		m_BloomShader->Compile("resources/shaders/renderlayer/bloom");
		m_BloomUniforms.Model = m_BlurShader->getUniformLocation("u_Model");
		m_BloomUniforms.View = m_BlurShader->getUniformLocation("u_View");
		m_BloomUniforms.Projection = m_BlurShader->getUniformLocation("u_Proj");


		std::vector<const char *> faces = {
			"resources/images/skybox/right.jpg",
			"resources/images/skybox/left.jpg",
			"resources/images/skybox/top.jpg",
			"resources/images/skybox/bottom.jpg",
			"resources/images/skybox/front.jpg",
			"resources/images/skybox/back.jpg"};

		std::vector<const char *> skybox1_faces = {
		"resources/images/skybox/right.jpg",
		"resources/images/skybox/left.jpg",
		"resources/images/skybox/top.jpg",
		"resources/images/skybox/bottom.jpg",
		"resources/images/skybox/front.jpg",
		"resources/images/skybox/back.jpg"};
		std::vector<const char*> dooms_day = {
		"resources/images/skybox/dooms_day/right.png",
		"resources/images/skybox/dooms_day/left.png",
		"resources/images/skybox/dooms_day/top.png",
		"resources/images/skybox/dooms_day/bottom.png",
		"resources/images/skybox/dooms_day/front.png",
		"resources/images/skybox/dooms_day/back.png" };
		std::vector<const char*> urban_light = {
		"resources/images/skybox/urban_light/right.png",
		"resources/images/skybox/urban_light/left.png",
		"resources/images/skybox/urban_light/top.png",
		"resources/images/skybox/urban_light/bottom.png",
		"resources/images/skybox/urban_light/front.png",
		"resources/images/skybox/urban_light/back.png" };
		std::vector<const char*> main_menu = {
		"resources/images/skybox/main_menu/right.png",
		"resources/images/skybox/main_menu/left.png",
		"resources/images/skybox/main_menu/top.png",
		"resources/images/skybox/main_menu/bottom.png",
		"resources/images/skybox/main_menu/front.png",
		"resources/images/skybox/main_menu/back.png" };
		m_DefaultSkybox.LoadCubemap(main_menu);

		SkyboxEnabled = true;


		// Init orthographic projection
		m_OrthoProj = glm::ortho(0.0f, (float)1920, (float)920, 0.0f, 0.1f, 100.0f);
		// Init main camera view
		m_View = glm::mat4(1.0f);
		m_View = glm::translate(m_View, glm::vec3(0.0f, 0.0f, -3.0f));
		// Init model
		m_Model = glm::mat4(1.0f);
		m_Model = glm::translate(m_Model, glm::vec3((float)1920 / 2.0f, (float)920 / 2.0f, 0.0f));
		m_Model = glm::scale(m_Model, glm::vec3((float)1920, (float)920, 1.0f));

		return true;
	}

	void Renderer3D::PreUpdate()
	{
		OPTICK_EVENT("Renderer 3D Pre Update");
		RenderSkybox();
	}

	void Renderer3D::Update()
	{
		OPTICK_EVENT("Renderer 3D Update");
		
	}

	void Renderer3D::PostUpdate()
	{
		//once everything is rendered and passed by the hdr 

		/*Camera* cam = SceneManager::getActiveScene()->GetCameraManager().getActiveCamera();

		if (cam == nullptr)
			return;*/
		//// Bind VAO
		//RenderManager::BindVAO();
		//glDisable(GL_DEPTH_TEST);
		//bool horizontal = true, first_iteration = false;
		//unsigned int amount = 10;
		//m_BlurShader->Bind();

		//// blur bright fragments with two-pass Gaussian Blur 
		//for (unsigned int i = 0; i < amount; i++)
		//{
		//	m_BlurShader->setUniformInt(m_BlurShader->getUniformLocation("u_Horizontal"), (int)horizontal);
		//	if (first_iteration) {
		//		glBindTexture(GL_TEXTURE_2D, cam->frameBuffer->getColorBuffers()[1]);
		//		first_iteration = false;
		//	}
		//	else {
		//		if (horizontal) {
		//			cam->vBlurBuffer->Bind(false);
		//			cam->vBlurBuffer->BindTexture();
		//		}
		//		else {
		//			cam->vBlurBuffer->Bind(false);
		//			cam->hBlurBuffer->BindTexture();
		//		}
		//	}

		//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//	horizontal = !horizontal;
		//}
		//m_BlurShader->UnBind();
		//glEnable(GL_DEPTH_TEST);

		////3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		////--------------------------------------------------------------------------------------------------------------------------
		//m_BloomShader->Bind();

		//cam->frameBuffer->Bind(false);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, cam->frameBuffer->getColorBuffers()[0]);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, cam->vBlurBuffer->getColorTexture());


		//m_BloomShader->setUniformInt(m_BloomShader->getUniformLocation("u_Bloom"), (int)true);
		//m_BloomShader->setUniformFloat(m_BloomShader->getUniformLocation("u_exposure"), 5);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//m_BloomShader->UnBind();

		//glBindVertexArray(0);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);


		/*	TESTING overwrite text 0 with text 1 color
			Bind the framebuffer object*/
			//glBindFramebuffer(GL_READ_FRAMEBUFFER, cam->frameBuffer->getFBO());

			//// Set the source and destination textures
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, cam->vBlurBuffer->getColorTexture());
			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, cam->frameBuffer->getColorBuffers()[0]);

			//// Copy the content of texture 1 to texture 0
			//glCopyImageSubData(
			//	cam->vBlurBuffer->getColorTexture(), GL_TEXTURE_2D, 0, 0, 0, 0,
			//	cam->frameBuffer->getColorBuffers()[0], GL_TEXTURE_2D, 0, 0, 0, 0,
			//	cam->frameBuffer->getWidth(), cam->frameBuffer->getHeight(), 1
			//);

			//// Unbind the textures and framebuffer object
			//glBindTexture(GL_TEXTURE_2D, 0);
			//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			//glBindVertexArray(0);


			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Renderer3D::RenderMesh(Model *mesh, const Transform3D &t3d, Material *material, const size_t &directional,
								const std::vector<size_t> &pointLights, const std::vector<size_t> &spotLights, bool clear /*=false*/, Camera *camera /*=NULL*/, bool cull /*= false*/)
	{
		if (!camera)
		{
			camera = SceneManager::getActiveScene()->GetCameraManager().getActiveCamera();
		}

		GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));

		camera->frameBuffer->Bind(clear);

		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(t3d.position.x, t3d.position.y, t3d.position.z));
		model = glm::rotate(model, glm::radians(t3d.rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(t3d.rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(t3d.rotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(t3d.scale.x, t3d.scale.y, t3d.scale.z));

		Shader *matShader = material->getShader();
		matShader->Bind();
		matShader->SetMVP(model, camera->getView(), camera->getProjection());

		SetUpLight(matShader, camera, directional, pointLights, spotLights);

		material->Bind();

		mesh->Render();

		material->UnBind();

		if (mesh->showNormals)
		{
			m_NormalDisplayShader->Bind();
			m_NormalDisplayShader->setUniform(m_NDSUniforms.Model, model);
			m_NormalDisplayShader->setUniform(m_NDSUniforms.View, camera->getView());
			m_NormalDisplayShader->setUniform(m_NDSUniforms.Projection, camera->getProjection());

			mesh->Render();
			m_NormalDisplayShader->UnBind();
		}
		if (camera->drawBoundingBoxes)
		{
			m_BBDisplayShader->Bind();
			m_BBDisplayShader->setUniform(m_BBDSUniforms.Model, model);
			m_BBDisplayShader->setUniform(m_BBDSUniforms.View, camera->getView());
			m_BBDisplayShader->setUniform(m_BBDSUniforms.Projection, camera->getProjection());
			mesh->DrawBoudingBox();
			m_BBDisplayShader->UnBind();
		}

		camera->frameBuffer->Unbind();
	}

	void Renderer3D::RenderMesh(Model *mesh, const Transform3D &t3d, const Transform3D &parent, Material *material, const size_t &directional,
								const std::vector<size_t> &pointLights, const std::vector<size_t> &spotLights, bool clear, Camera *camera, bool cull)
	{
		if (!camera)
		{
			camera = SceneManager::getActiveScene()->GetCameraManager().getActiveCamera();
		}

		GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));

		camera->frameBuffer->Bind(clear);

		glm::mat4 model = glm::mat4(1.0f);

		// Local transformation
		model = glm::translate(model, glm::vec3(t3d.localPosition.x, t3d.localPosition.y, t3d.localPosition.z));
		model = glm::rotate(model, glm::radians(t3d.localRotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(t3d.localRotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(t3d.localRotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(t3d.localScale.x, t3d.localScale.y, t3d.localScale.z));

		// Parent transformation
		glm::mat4 parent_model = glm::mat4(1.0f);
		parent_model = glm::translate(parent_model, glm::vec3(parent.position.x, parent.position.y, parent.position.z));
		parent_model = glm::rotate(parent_model, glm::radians(parent.rotation.x), glm::vec3(1, 0, 0));
		parent_model = glm::rotate(parent_model, glm::radians(parent.rotation.y), glm::vec3(0, 1, 0));
		parent_model = glm::rotate(parent_model, glm::radians(parent.rotation.z), glm::vec3(0, 0, 1));
		parent_model = glm::scale(parent_model, glm::vec3(parent.scale.x, parent.scale.y, parent.scale.z));

		model = parent_model * model;

		// Material bind
		Shader *matShader = material->getShader();
		matShader->Bind();
		matShader->SetMVP(model, camera->getView(), camera->getProjection());
		SetUpLight(matShader, camera, directional, pointLights, spotLights);

		material->Bind();

		mesh->Render();

		material->UnBind();

		if (mesh->showNormals)
		{
			m_NormalDisplayShader->Bind();
			m_NormalDisplayShader->setUniform(m_NDSUniforms.Model, model);
			m_NormalDisplayShader->setUniform(m_NDSUniforms.View, camera->getView());
			m_NormalDisplayShader->setUniform(m_NDSUniforms.Projection, camera->getProjection());

			mesh->Render();
			m_NormalDisplayShader->UnBind();
		}
		if (camera->drawBoundingBoxes)
		{
			m_BBDisplayShader->Bind();
			m_BBDisplayShader->setUniform(m_BBDSUniforms.Model, model);
			m_BBDisplayShader->setUniform(m_BBDSUniforms.View, camera->getView());
			m_BBDisplayShader->setUniform(m_BBDSUniforms.Projection, camera->getProjection());
			mesh->DrawBoudingBox();
			m_BBDisplayShader->UnBind();
		}

		camera->frameBuffer->Unbind();
	}
	void Renderer3D::RenderMesh(Model *mesh, const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale, const size_t &directional,
								const std::vector<size_t> &pointLights, const std::vector<size_t> &spotLights, Material *material, bool clear, Camera *camera, bool cull)
	{
		if (!camera)
		{
			camera = SceneManager::getActiveScene()->GetCameraManager().getActiveCamera();
		}

		GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));

		camera->frameBuffer->Bind(clear);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, scale);
		material->getShader()->Bind();
		material->getShader()->SetMVP(model, camera->getView(), camera->getProjection());

		Shader *matShader = material->getShader();
		matShader->Bind();
		matShader->SetMVP(model, camera->getView(), camera->getProjection());
		SetUpLight(matShader, camera, directional, pointLights, spotLights);

		mesh->Render();

		material->UnBind();

		if (mesh->showNormals)
		{
			m_NormalDisplayShader->Bind();
			m_NormalDisplayShader->setUniform(m_NDSUniforms.Model, model);
			m_NormalDisplayShader->setUniform(m_NDSUniforms.View, camera->getView());
			m_NormalDisplayShader->setUniform(m_NDSUniforms.Projection, camera->getProjection());

			mesh->Render();
			m_NormalDisplayShader->UnBind();
		}
		if (camera->drawBoundingBoxes)
		{
			m_BBDisplayShader->Bind();
			m_BBDisplayShader->setUniform(m_BBDSUniforms.Model, model);
			m_BBDisplayShader->setUniform(m_BBDSUniforms.View, camera->getView());
			m_BBDisplayShader->setUniform(m_BBDSUniforms.Projection, camera->getProjection());
			mesh->DrawBoudingBox();
			m_BBDisplayShader->UnBind();
		}

		camera->frameBuffer->Unbind();
	}


	void Renderer3D::RenderMesh(Model *mesh, const glm::mat4 &transform, Material *material, const size_t &directional,
	    const std::vector<size_t> &pointLights, const std::vector<size_t> &spotLights, bool castShadows,
		bool shadowRecieve, bool clear, Camera *camera, bool cull)
	{

		if (!camera)
		{
			camera = SceneManager::getActiveScene()->GetCameraManager().getActiveCamera();
		}
		//Set up color buffer
		Shader *matShader = material->getShader();

		if (material == nullptr)
		{
			WI_ERROR("missing material {}",mesh->GetParent()->getModelName());
			return;
		}
		
		RenderShadows(camera, mesh, matShader, transform, castShadows, shadowRecieve);

		GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));

		camera->frameBuffer->Bind(clear);


		matShader->Bind();

		matShader->SetMVP(transform, camera->getView(), camera->getProjection());

		SetUpLight(matShader, camera, directional, pointLights, spotLights);
		if (shadowRecieve)
		{
			camera->shadowBuffer->BindTexture();
		}
		
		material->Bind(GL_TEXTURE1);
		mesh->Render();
		material->UnBind(GL_TEXTURE1);

		if (shadowRecieve) {
			camera->shadowBuffer->UnbindTexture();
		}

		// Debug

		if (mesh->showNormals)
		{
			m_NormalDisplayShader->Bind();
			m_NormalDisplayShader->setUniform(m_NDSUniforms.Model, transform);
			m_NormalDisplayShader->setUniform(m_NDSUniforms.View, camera->getView());
			m_NormalDisplayShader->setUniform(m_NDSUniforms.Projection, camera->getProjection());

			mesh->Render();
			m_NormalDisplayShader->UnBind();
		}
		if (camera->drawBoundingBoxes)
		{
			m_BBDisplayShader->Bind();
			m_BBDisplayShader->setUniform(m_BBDSUniforms.Model, transform);
			m_BBDisplayShader->setUniform(m_BBDSUniforms.View, camera->getView());
			m_BBDisplayShader->setUniform(m_BBDSUniforms.Projection, camera->getProjection());
			mesh->DrawBoudingBox();
			m_BBDisplayShader->UnBind();
		}

		camera->frameBuffer->Unbind();
	}
	

	void Renderer3D::RenderQuad(unsigned int vao, std::vector<int> ebo_data, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const size_t& directional,
		const std::vector<size_t>& pointLights, const std::vector<size_t>& spotLights, Material* material, bool clear, Camera* camera, bool cull, Image* texture, const Size2i& srcSize, float colorParticles[4], bool isColorRanged)
	{
		/*colorParticles[4] /= 255.0f;*/
		/*g /= 255.0f;
		b /= 255.0f;*/

		GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));

		camera->frameBuffer->Bind(clear);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, scale);

		Shader* matShader = material->getShader();

		/*Shader* particleShader = Resources::GetResourceById<Shader>(m_ParticleShaderId);*/

		matShader->Bind();

		/*particleShader->setUniformVec3(particleShader->getUniform("u_Color")->location, glm::vec3(r, g ,b));*/

		matShader->SetMVP(model, camera->getView(), camera->getProjection());

		if (isColorRanged)
		{
			matShader->setUniformVec3(matShader->getUniform("u_ColorP")->location, glm::vec3(colorParticles[0], colorParticles[1], colorParticles[2]));
		}

		else
		{
			matShader->setUniformVec3(matShader->getUniform("u_ColorP")->location, glm::vec3(1.0, 1.0, 1.0));
		}

		/*matShader->setUniform(matShader->getUniform("u_ColorP")->location, 250);*/

		SetUpLight(matShader, camera, directional, pointLights, spotLights);

		material->Bind();

		/*glColor4f(r, g, b ,1);*/

		GL(Disable(GL_CULL_FACE));

		// make the drawing
		GL(BindVertexArray(vao));

		// Solution to the material.cpp GL_TEXTURE1 on the void Material::Bind() function
		GL(ActiveTexture(GL_TEXTURE0));
		GL(BindTexture(GL_TEXTURE_2D, texture->GetTextureId()));
		//------------------------------

		GL(DrawElements(GL_TRIANGLES, (GLsizei)ebo_data.size(), GL_UNSIGNED_INT, (GLsizei)0));
		GL(BindVertexArray(0));

		material->UnBind();
		camera->frameBuffer->Unbind();
		GL(Enable(GL_CULL_FACE));
	}

	Renderer3D::ScratchBuffer::ScratchBuffer() : buffer_(nullptr), size_(0) {}

	Renderer3D::ScratchBuffer::~ScratchBuffer() {
		ozz::memory::default_allocator()->Deallocate(buffer_);
	}

	void* Renderer3D::ScratchBuffer::Resize(size_t _size) {
		if (_size > size_) {
			size_ = _size;
			ozz::memory::default_allocator()->Deallocate(buffer_);
			buffer_ = ozz::memory::default_allocator()->Allocate(_size, 16);
		}
		return buffer_;
	}

	bool Renderer3D::RenderOzzSkinnedMesh(Camera* camera, const ozz::sample::Mesh& _mesh, Material* material, const ozz::span<ozz::math::Float4x4> _skinning_matrices, const ozz::math::Float4x4& _transform)
	{
		const int vertex_count = _mesh.vertex_count();

		// Positions and normals are interleaved to improve caching while executing
		// skinning job.

		const GLsizei positions_offset = 0;
		const GLsizei positions_stride = sizeof(float) * 3;
		const GLsizei normals_offset = vertex_count * positions_stride;
		const GLsizei normals_stride = sizeof(float) * 3;
		const GLsizei tangents_offset =
			normals_offset + vertex_count * normals_stride;
		const GLsizei tangents_stride = sizeof(float) * 3;
		const GLsizei skinned_data_size =
			tangents_offset + vertex_count * tangents_stride;

		// Colors and uvs are contiguous. They aren't transformed, so they can be
		// directly copied from source mesh which is non-interleaved as-well.
		// Colors will be filled with white if _options.colors is false.
		// UVs will be skipped if _options.textured is false.
		const GLsizei colors_offset = skinned_data_size;
		const GLsizei colors_stride = sizeof(uint8_t) * 4;
		const GLsizei colors_size = vertex_count * colors_stride;
		const GLsizei uvs_offset = colors_offset + colors_size;
		const GLsizei uvs_stride = sizeof(float) * 2;
		const GLsizei uvs_size = vertex_count * uvs_stride;
		const GLsizei fixed_data_size = colors_size + uvs_size;

		// Reallocate vertex buffer.
		const GLsizei vbo_size = skinned_data_size + fixed_data_size;
		void* vbo_map = scratch_buffer_.Resize(vbo_size);

		// Iterate mesh parts and fills vbo.
		// Runs a skinning job per mesh part. Triangle indices are shared
		// across parts.
		size_t processed_vertex_count = 0;
		for (size_t i = 0; i < _mesh.parts.size(); ++i) {
			const ozz::sample::Mesh::Part& part = _mesh.parts[i];

			// Skip this iteration if no vertex.
			const size_t part_vertex_count = part.positions.size() / 3;
			if (part_vertex_count == 0) {
				continue;
			}

			// Fills the job.
			ozz::geometry::SkinningJob skinning_job;
			skinning_job.vertex_count = static_cast<int>(part_vertex_count);
			const int part_influences_count = part.influences_count();

			// Clamps joints influence count according to the option.
			skinning_job.influences_count = part_influences_count;

			// Setup skinning matrices, that came from the animation stage before being
			// multiplied by inverse model-space bind-pose.
			skinning_job.joint_matrices = _skinning_matrices;

			// Setup joint's indices.
			skinning_job.joint_indices = make_span(part.joint_indices);
			skinning_job.joint_indices_stride =
				sizeof(uint16_t) * part_influences_count;

			// Setup joint's weights.
			if (part_influences_count > 1) {
				skinning_job.joint_weights = make_span(part.joint_weights);
				skinning_job.joint_weights_stride =
					sizeof(float) * (part_influences_count - 1);
			}

			// Setup input positions, coming from the loaded mesh.
			skinning_job.in_positions = make_span(part.positions);
			skinning_job.in_positions_stride =
				sizeof(float) * ozz::sample::Mesh::Part::kPositionsCpnts;

			// Setup output positions, coming from the rendering output mesh buffers.
			// We need to offset the buffer every loop.
			float* out_positions_begin = reinterpret_cast<float*>(ozz::PointerStride(
				vbo_map, positions_offset + processed_vertex_count * positions_stride));
			float* out_positions_end = ozz::PointerStride(
				out_positions_begin, part_vertex_count * positions_stride);
			skinning_job.out_positions = { out_positions_begin, out_positions_end };
			skinning_job.out_positions_stride = positions_stride;

			// Setup normals if input are provided.
			float* out_normal_begin = reinterpret_cast<float*>(ozz::PointerStride(
				vbo_map, normals_offset + processed_vertex_count * normals_stride));
			float* out_normal_end = ozz::PointerStride(
				out_normal_begin, part_vertex_count * normals_stride);

			if (part.normals.size() / ozz::sample::Mesh::Part::kNormalsCpnts ==
				part_vertex_count) {
				// Setup input normals, coming from the loaded mesh.
				skinning_job.in_normals = make_span(part.normals);
				skinning_job.in_normals_stride =
					sizeof(float) * ozz::sample::Mesh::Part::kNormalsCpnts;

				// Setup output normals, coming from the rendering output mesh buffers.
				// We need to offset the buffer every loop.
				skinning_job.out_normals = { out_normal_begin, out_normal_end };
				skinning_job.out_normals_stride = normals_stride;
			}
			else {
				// Fills output with default normals.
				for (float* normal = out_normal_begin; normal < out_normal_end;
					normal = ozz::PointerStride(normal, normals_stride)) {
					normal[0] = 0.f;
					normal[1] = 1.f;
					normal[2] = 0.f;
				}
			}

			// Setup tangents if input are provided.
			float* out_tangent_begin = reinterpret_cast<float*>(ozz::PointerStride(
				vbo_map, tangents_offset + processed_vertex_count * tangents_stride));
			float* out_tangent_end = ozz::PointerStride(
				out_tangent_begin, part_vertex_count * tangents_stride);

			if (part.tangents.size() / ozz::sample::Mesh::Part::kTangentsCpnts ==
				part_vertex_count) {
				// Setup input tangents, coming from the loaded mesh.
				skinning_job.in_tangents = make_span(part.tangents);
				skinning_job.in_tangents_stride =
					sizeof(float) * ozz::sample::Mesh::Part::kTangentsCpnts;

				// Setup output tangents, coming from the rendering output mesh buffers.
				// We need to offset the buffer every loop.
				skinning_job.out_tangents = { out_tangent_begin, out_tangent_end };
				skinning_job.out_tangents_stride = tangents_stride;
			}
			else {
				// Fills output with default tangents.
				for (float* tangent = out_tangent_begin; tangent < out_tangent_end;
					tangent = ozz::PointerStride(tangent, tangents_stride)) {
					tangent[0] = 1.f;
					tangent[1] = 0.f;
					tangent[2] = 0.f;
				}
			}

			// Execute the job, which should succeed unless a parameter is invalid.
			if (!skinning_job.Run()) {
				return false;
			}
			
			// OZZ COLORS
			// Un-optimal path used when the right number of colors is not provided.
			static_assert(sizeof(kDefaultColorsArray[0]) == colors_stride,
				"Stride mismatch");

			for (size_t j = 0; j < part_vertex_count;
				j += OZZ_ARRAY_SIZE(kDefaultColorsArray)) {
				const size_t this_loop_count = ozz::math::Min(
					OZZ_ARRAY_SIZE(kDefaultColorsArray), part_vertex_count - j);
				memcpy(ozz::PointerStride(
					vbo_map, colors_offset +
					(processed_vertex_count + j) * colors_stride),
					kDefaultColorsArray, colors_stride * this_loop_count);
			}

			if (part_vertex_count ==
				part.uvs.size() / ozz::sample::Mesh::Part::kUVsCpnts) {
				// Optimal path used when the right number of uvs is provided.
				memcpy(ozz::PointerStride(
					vbo_map, uvs_offset + processed_vertex_count * uvs_stride),
					array_begin(part.uvs), part_vertex_count * uvs_stride);
			}
			else {
				// Un-optimal path used when the right number of uvs is not provided.
				assert(sizeof(kDefaultUVsArray[0]) == uvs_stride);
				for (size_t j = 0; j < part_vertex_count;
					j += OZZ_ARRAY_SIZE(kDefaultUVsArray)) {
					const size_t this_loop_count = ozz::math::Min(
						OZZ_ARRAY_SIZE(kDefaultUVsArray), part_vertex_count - j);
					memcpy(ozz::PointerStride(
						vbo_map,
						uvs_offset + (processed_vertex_count + j) * uvs_stride),
						kDefaultUVsArray, uvs_stride * this_loop_count);
				}
			}

			// Some more vertices were processed.
			processed_vertex_count += part_vertex_count;
		}
		

		// ========================= RENDERING =====================================

		// Build mvp for object
		glm::mat4 glm_mvp = camera->getProjection() * camera->getView();
		glm::mat4 transform_mat = glm::mat4(1.f);

		ozz::math::Float4x4 ozz_mvp;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				// m128_f32 for float4x4
				ozz_mvp.cols[i].m128_f32[j] = glm_mvp[i][j]; 
				transform_mat[i][j] = _transform.cols[i].m128_f32[j];
			}
		}


		Shader* anim_shader = material->getShader();

		RenderShadowsOzz(
			camera,
			_mesh,
			anim_shader,
			transform_mat,
			vbo_size,
			vbo_map
			);

		LightManager& lman = Wiwa::SceneManager::getActiveScene()->GetLightManager();

		// After processing everything, render
		GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));

		camera->frameBuffer->Bind(false);
		anim_shader->Bind();
		GL(BindVertexArray(dynamic_vao_));
		// Updates dynamic vertex buffer with skinned data.
		GL(BindBuffer(GL_ARRAY_BUFFER, dynamic_array_bo_));
		GL(BufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_STREAM_DRAW));
		GL(BufferSubData(GL_ARRAY_BUFFER, 0, vbo_size, vbo_map));

		

		SetUpLight(anim_shader, camera, lman.GetDirectionalLight(), lman.GetPointLights(), lman.GetSpotLights());
		camera->shadowBuffer->BindTexture();

		material->Bind(GL_TEXTURE1);

		GL(EnableVertexAttribArray(0));
		GL(VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, uvs_stride,
			GL_PTR_OFFSET(uvs_offset)));

		GL(EnableVertexAttribArray(1));
		GL(VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, positions_stride,
			GL_PTR_OFFSET(positions_offset)));

		GL(EnableVertexAttribArray(2));
		GL(VertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, normals_stride,
			GL_PTR_OFFSET(normals_offset)));

		GL(EnableVertexAttribArray(3));
		GL(VertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE,
			colors_stride, GL_PTR_OFFSET(colors_offset)));

		// Binds mw uniform
		float values[16];
		const GLint mw_uniform = GL(GetUniformLocation(anim_shader->getID(), "u_mw"));//uniform(0);
		ozz::math::StorePtrU(_transform.cols[0], values + 0);
		ozz::math::StorePtrU(_transform.cols[1], values + 4);
		ozz::math::StorePtrU(_transform.cols[2], values + 8);
		ozz::math::StorePtrU(_transform.cols[3], values + 12);
		GL(UniformMatrix4fv(mw_uniform, 1, false, values));

		// Binds mvp uniform
		const GLint mvp_uniform = GL(GetUniformLocation(anim_shader->getID(), "u_mvp"));//uniform(1);
		ozz::math::StorePtrU(ozz_mvp.cols[0], values + 0);
		ozz::math::StorePtrU(ozz_mvp.cols[1], values + 4);
		ozz::math::StorePtrU(ozz_mvp.cols[2], values + 8);
		ozz::math::StorePtrU(ozz_mvp.cols[3], values + 12);
		GL(UniformMatrix4fv(mvp_uniform, 1, false, values));

		// Maps the index dynamic buffer and update it.
		GL(BindBuffer(GL_ELEMENT_ARRAY_BUFFER, dynamic_index_bo_));
		const ozz::sample::Mesh::TriangleIndices& indices = _mesh.triangle_indices;
		GL(BufferData(GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(ozz::sample::Mesh::TriangleIndices::value_type),
			array_begin(indices), GL_STREAM_DRAW));

		// Draws the mesh.
		static_assert(sizeof(ozz::sample::Mesh::TriangleIndices::value_type) == 2,
			"Expects 2 bytes indices.");
		GL(DrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
			GL_UNSIGNED_SHORT, 0));

		// Unbinds.
		GL(BindBuffer(GL_ARRAY_BUFFER, 0));
		GL(BindTexture(GL_TEXTURE_2D, 0));
		GL(BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		
		GL(DisableVertexAttribArray(0));
		GL(DisableVertexAttribArray(1));
		GL(DisableVertexAttribArray(2));
		GL(DisableVertexAttribArray(3));

		GL(BindVertexArray(0));

		camera->shadowBuffer->UnbindTexture();
		material->UnBind(GL_TEXTURE1);

		camera->frameBuffer->Unbind();

		return true;
	}

	void Renderer3D::RenderSkybox()
	{

		if (!SkyboxEnabled)
			return;

		{
			Scene* scene = SceneManager::getActiveScene();
			CameraManager& cm = scene->GetCameraManager();
			Camera *camera = cm.getActiveCamera();
			if (camera)
			{
				GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));

				camera->frameBuffer->Bind(false);
				GL(DepthFunc(GL_ALWAYS));
				Shader* shader = m_DefaultSkybox.m_Material->getShader();
				shader->Bind();
				shader->setUniform(shader->getProjLoc(), camera->getProjection());
				glm::mat4 view = glm::mat4(glm::mat3(camera->getView()));
				shader->setUniform(shader->getViewLoc(), view);
				m_DefaultSkybox.Render();
				shader->UnBind();
				GL(DepthFunc(GL_LESS));

				camera->frameBuffer->Unbind();
			}
		}
		{
			Camera *camera = SceneManager::getActiveScene()->GetCameraManager().editorCamera;
			if (camera)
			{
				GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));

				camera->frameBuffer->Bind(false);
				GL(DepthFunc(GL_ALWAYS));
				Shader *shader = m_DefaultSkybox.m_Material->getShader();
				shader->Bind();
				shader->setUniform(shader->getProjLoc(), camera->getProjection());
				glm::mat4 view = glm::mat4(glm::mat3(camera->getView()));
				shader->setUniform(shader->getViewLoc(), view);
				m_DefaultSkybox.Render();
				shader->UnBind();
				GL(DepthFunc(GL_LESS));

				camera->frameBuffer->Unbind();
			}
		}
	}
	void Renderer3D::SetOption(Options option)
	{
		switch (option)
		{
		case Wiwa::Renderer3D::DEPTH_TEST:
			GL(Enable(GL_DEPTH_TEST));
			break;
		case Wiwa::Renderer3D::CULL_FACE:
			GL(Enable(GL_CULL_FACE));
			break;
		case Wiwa::Renderer3D::LIGHTING:
			GL(Enable(GL_LIGHTING));
			break;
		case Wiwa::Renderer3D::COLOR_MATERIAL:
			GL(Enable(GL_COLOR_MATERIAL));
			break;
		case Wiwa::Renderer3D::TEXTURE_2D:
			GL(Enable(GL_TEXTURE_2D));
			break;
		case Wiwa::Renderer3D::WIREFRAME:
			GL(PolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			break;
		case Wiwa::Renderer3D::GAMMA_CORRECTION:
			GL(Enable(GL_FRAMEBUFFER_SRGB));
			break;
		default:
			break;
		}
	}
	void Renderer3D::DisableOption(Options option)
	{
		switch (option)
		{
		case Wiwa::Renderer3D::DEPTH_TEST:
			GL(Disable(GL_DEPTH_TEST));
			break;
		case Wiwa::Renderer3D::CULL_FACE:
			GL(Disable(GL_CULL_FACE));
			break;
		case Wiwa::Renderer3D::LIGHTING:
			GL(Disable(GL_LIGHTING));
			break;
		case Wiwa::Renderer3D::COLOR_MATERIAL:
			GL(Disable(GL_COLOR_MATERIAL));
			break;
		case Wiwa::Renderer3D::TEXTURE_2D:
			GL(Disable(GL_TEXTURE_2D));
			break;
		case Wiwa::Renderer3D::WIREFRAME:
			GL(PolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			break;
		case Wiwa::Renderer3D::GAMMA_CORRECTION:
			GL(Disable(GL_FRAMEBUFFER_SRGB));
			break;
		default:
			break;
		}
	}
	void Renderer3D::Close()
	{
	}
	void Renderer3D::RenderFrustrums()
	{
		Camera *camera = SceneManager::getActiveScene()->GetCameraManager().editorCamera;
		GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));
		camera->frameBuffer->Bind(false);

		/*GL(MatrixMode(GL_PROJECTION));
		GL(LoadMatrixf(glm::value_ptr(camera->getProjection())));
		GL(MatrixMode(GL_MODELVIEW));
		GL(LoadMatrixf(glm::value_ptr(camera->getView())));*/

		CameraManager &cameraManager = SceneManager::getActiveScene()->GetCameraManager();

		size_t cameraCount = cameraManager.getCameraSize();
		std::vector<CameraId> &cameras = cameraManager.getCameras();
		for (size_t i = 0; i < cameraCount; i++)
		{
			Camera *cam = cameraManager.getCamera(cameras[i]);

			/*glUseProgram(0);
			glColor3f(0, 255, 0);
			glLineWidth(3.0f);
			glBegin(GL_LINES);

			glm::vec3 bb_frustum[8];
			cam->GetCornerPoints(bb_frustum);

			glVertex3f((GLfloat)bb_frustum[0].x, (GLfloat)bb_frustum[0].y, (GLfloat)bb_frustum[0].z);
			glVertex3f((GLfloat)bb_frustum[1].x, (GLfloat)bb_frustum[1].y, (GLfloat)bb_frustum[1].z);

			glVertex3f((GLfloat)bb_frustum[0].x, (GLfloat)bb_frustum[0].y, (GLfloat)bb_frustum[0].z);
			glVertex3f((GLfloat)bb_frustum[4].x, (GLfloat)bb_frustum[4].y, (GLfloat)bb_frustum[4].z);

			glVertex3f((GLfloat)bb_frustum[0].x, (GLfloat)bb_frustum[0].y, (GLfloat)bb_frustum[0].z);
			glVertex3f((GLfloat)bb_frustum[2].x, (GLfloat)bb_frustum[2].y, (GLfloat)bb_frustum[2].z);

			glVertex3f((GLfloat)bb_frustum[2].x, (GLfloat)bb_frustum[2].y, (GLfloat)bb_frustum[2].z);
			glVertex3f((GLfloat)bb_frustum[3].x, (GLfloat)bb_frustum[3].y, (GLfloat)bb_frustum[3].z);

			glVertex3f((GLfloat)bb_frustum[1].x, (GLfloat)bb_frustum[1].y, (GLfloat)bb_frustum[1].z);
			glVertex3f((GLfloat)bb_frustum[3].x, (GLfloat)bb_frustum[3].y, (GLfloat)bb_frustum[3].z);

			glVertex3f((GLfloat)bb_frustum[5].x, (GLfloat)bb_frustum[5].y, (GLfloat)bb_frustum[5].z);
			glVertex3f((GLfloat)bb_frustum[4].x, (GLfloat)bb_frustum[4].y, (GLfloat)bb_frustum[4].z);

			glVertex3f((GLfloat)bb_frustum[4].x, (GLfloat)bb_frustum[4].y, (GLfloat)bb_frustum[4].z);
			glVertex3f((GLfloat)bb_frustum[6].x, (GLfloat)bb_frustum[6].y, (GLfloat)bb_frustum[6].z);

			glVertex3f((GLfloat)bb_frustum[7].x, (GLfloat)bb_frustum[7].y, (GLfloat)bb_frustum[7].z);
			glVertex3f((GLfloat)bb_frustum[5].x, (GLfloat)bb_frustum[5].y, (GLfloat)bb_frustum[5].z);

			glVertex3f((GLfloat)bb_frustum[7].x, (GLfloat)bb_frustum[7].y, (GLfloat)bb_frustum[7].z);
			glVertex3f((GLfloat)bb_frustum[6].x, (GLfloat)bb_frustum[6].y, (GLfloat)bb_frustum[6].z);

			glVertex3f((GLfloat)bb_frustum[1].x, (GLfloat)bb_frustum[1].y, (GLfloat)bb_frustum[1].z);
			glVertex3f((GLfloat)bb_frustum[5].x, (GLfloat)bb_frustum[5].y, (GLfloat)bb_frustum[5].z);

			glVertex3f((GLfloat)bb_frustum[6].x, (GLfloat)bb_frustum[6].y, (GLfloat)bb_frustum[6].z);
			glVertex3f((GLfloat)bb_frustum[2].x, (GLfloat)bb_frustum[2].y, (GLfloat)bb_frustum[2].z);

			glVertex3f((GLfloat)bb_frustum[7].x, (GLfloat)bb_frustum[7].y, (GLfloat)bb_frustum[7].z);
			glVertex3f((GLfloat)bb_frustum[3].x, (GLfloat)bb_frustum[3].y, (GLfloat)bb_frustum[3].z);

			glEnd();
			glLineWidth(1.0f);*/
		}
		camera->frameBuffer->Unbind();
	}
	void Renderer3D::EnableSkybox(bool enabled)
	{
		SkyboxEnabled = enabled;
	}

	void Renderer3D::SetUpLight(Wiwa::Shader *matShader, Wiwa::Camera *camera, const size_t &directional, const std::vector<size_t> &pointLights, const std::vector<size_t> &spotLights)
	{
		matShader->setUniform(matShader->LightLocations.CameraLoc, camera->getPosition());
		if (directional != -1)
		{
			DirectionalLight *dirLight = SceneManager::getActiveScene()->GetEntityManager().GetComponent<DirectionalLight>(directional);
			Transform3D *transform = SceneManager::getActiveScene()->GetEntityManager().GetComponent<Transform3D>(directional);
			if (dirLight && transform)
			{
				matShader->setUniform(matShader->LightLocations.DirectionalLightBC, dirLight->Color);
				matShader->setUniform(matShader->LightLocations.DirectionalLightBaseAmbientIntensity, dirLight->AmbientIntensity);
				matShader->setUniform(matShader->LightLocations.DirectionalLightBaseDiffuseIntensity, dirLight->DiffuseIntensity);
				matShader->setUniform(matShader->LightLocations.DirectionalLightDirection, glm::radians(transform->localRotation));
			}
		}
		matShader->setUniform(matShader->LightLocations.NumPointLights, (int)pointLights.size());
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			Transform3D *transform = SceneManager::getActiveScene()->GetEntityManager().GetComponent<Transform3D>(pointLights[i]);
			PointLight *pointLight = SceneManager::getActiveScene()->GetEntityManager().GetComponent<PointLight>(pointLights[i]);
			std::string num = std::to_string(i);

			if (transform && pointLight)
			{

				matShader->setUniform(matShader->LightLocations.PointLights[i].BaseColor, pointLight->Color);
				matShader->setUniform(matShader->LightLocations.PointLights[i].BaseAmbientIntensity, pointLight->AmbientIntensity);
				matShader->setUniform(matShader->LightLocations.PointLights[i].BaseDiffuseIntensity, pointLight->DiffuseIntensity);
				matShader->setUniform(matShader->LightLocations.PointLights[i].LocalPos, transform->position);
				matShader->setUniform(matShader->LightLocations.PointLights[i].AttenConstant, pointLight->Constant);
				matShader->setUniform(matShader->LightLocations.PointLights[i].AttenLinear, pointLight->Linear);
				matShader->setUniform(matShader->LightLocations.PointLights[i].AttenExp, pointLight->Exp);
			}
		}

		/*matShader->setUniform(matShader->getUniformLocation("u_NumSpotLights"), (int)spotLights.size());
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			Transform3D *transform = SceneManager::getActiveScene()->GetEntityManager().GetComponent<Transform3D>(pointLights[i]);
			SpotLight *spotLight = SceneManager::getActiveScene()->GetEntityManager().GetComponent<SpotLight>(pointLights[i]);
			std::string num = std::to_string(i);

			if (transform && spotLight)
			{

				matShader->setUniform(matShader->getUniformLocation(("u_SpotLights[" + num + "].Base.Base.Color").c_str()), spotLight->Color);
				matShader->setUniform(matShader->getUniformLocation(("u_SpotLights[" + num + "].Base.Base.AmbientIntensity").c_str()), spotLight->AmbientIntensity);
				matShader->setUniform(matShader->getUniformLocation(("u_SpotLights[" + num + "].Base.Base.DiffuseIntensity").c_str()), spotLight->DiffuseIntensity);
				matShader->setUniform(matShader->getUniformLocation(("u_SpotLights[" + num + "].Base.LocalPos").c_str()), transform->position);
				matShader->setUniform(matShader->getUniformLocation(("u_SpotLights[" + num + "].Base.Atten.Constant").c_str()), spotLight->Constant);
				matShader->setUniform(matShader->getUniformLocation(("u_SpotLights[" + num + "].Base.Atten.Linear").c_str()), spotLight->Linear);
				matShader->setUniform(matShader->getUniformLocation(("u_SpotLights[" + num + "].Base.Atten.Exp").c_str()), spotLight->Exp);
				matShader->setUniform(matShader->getUniformLocation(("u_SpotLights[" + num + "].Direction").c_str()), glm::radians(spotLight->Direction));
				matShader->setUniform(matShader->getUniformLocation(("u_SpotLights[" + num + "].Cuttoff").c_str()), spotLight->Cutoff);
			}
		}*/
	}
	void Renderer3D::RenderShadows(Camera *camera, Model* mesh,
		Shader* matShader, const glm::mat4& transform, bool castShadow, bool recieveShadow)
	{

		if (!camera)
		{
			camera = SceneManager::getActiveScene()->GetCameraManager().getActiveCamera();
		}

		LightManager& lman = SceneManager::getActiveScene()->GetLightManager();
		if (castShadow)
		{

			camera->shadowBuffer->Bind(false);

			m_DepthShader->Bind();
			m_DepthShader->setUniform(m_DepthShaderUniforms.Projection, lman.GetDirectionalProj());
			m_DepthShader->setUniform(m_DepthShaderUniforms.View, lman.GetDirectionalView());

			m_DepthShader->setUniform(m_DepthShaderUniforms.Model, transform);

			GL(CullFace(GL_FRONT));
			mesh->Render();
			GL(CullFace(GL_BACK)); // don't forget to reset original culling face
			m_DepthShader->UnBind();
			camera->shadowBuffer->Unbind();
		}
		int recieve = recieveShadow ? 1 : 0;
		matShader->setUniform(matShader->LightLocations.RecieveShadows, recieve);
		if (recieveShadow)
		{
			matShader->setUniform(matShader->LightLocations.DirLightMVP, lman.GetDirectionalMVP());
			matShader->setUniform(matShader->LightLocations.DirLightPos, lman.GetDirectionalPos());
		}
	}
	void Renderer3D::RenderShadowsOzz(
		Camera* camera,
		const ozz::sample::Mesh& mesh,
		Shader* matShader,
		const glm::mat4& transform,
		const size_t vbo_size,
		void* vbo_map
	)
	{
		LightManager& lman = SceneManager::getActiveScene()->GetLightManager();

		camera->shadowBuffer->Bind(false);

		m_DepthShader->Bind();
		m_DepthShader->setUniform(m_DepthShaderUniforms.Projection, lman.GetDirectionalProj());
		m_DepthShader->setUniform(m_DepthShaderUniforms.View, lman.GetDirectionalView());

		m_DepthShader->setUniform(m_DepthShaderUniforms.Model, transform);

		GL(CullFace(GL_FRONT));


		GL(BindVertexArray(dynamic_vao_));
		// Updates dynamic vertex buffer with skinned data.
		GL(BindBuffer(GL_ARRAY_BUFFER, dynamic_array_bo_));
		GL(BufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_STREAM_DRAW));
		GL(BufferSubData(GL_ARRAY_BUFFER, 0, vbo_size, vbo_map));

		GL(EnableVertexAttribArray(0));
		GL(VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3,
			GL_PTR_OFFSET(0)));

		GL(BindBuffer(GL_ELEMENT_ARRAY_BUFFER, dynamic_index_bo_));
		const ozz::sample::Mesh::TriangleIndices& indices = mesh.triangle_indices;

		GL(BufferData(GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(ozz::sample::Mesh::TriangleIndices::value_type),
			array_begin(indices), GL_STREAM_DRAW));

		// Draws the mesh.
		static_assert(sizeof(ozz::sample::Mesh::TriangleIndices::value_type) == 2,
			"Expects 2 bytes indices.");
		GL(DrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
			GL_UNSIGNED_SHORT, 0));

		// Unbinds.
		GL(BindBuffer(GL_ARRAY_BUFFER, 0));
		
		GL(CullFace(GL_BACK)); // don't forget to reset original culling face

		
		m_DepthShader->UnBind();
		camera->shadowBuffer->Unbind();

		matShader->setUniform(matShader->LightLocations.DirLightMVP, lman.GetDirectionalMVP());
		matShader->setUniform(matShader->LightLocations.DirLightPos, lman.GetDirectionalPos());

	}
}