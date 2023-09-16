#pragma once

#include "../Panel.h"

#include <string>

#include <Wiwa/animation/OzzAnimator.h>

#include <Wiwa/animation/animations/OzzAnimationSimple.h>

#include <glm/glm.hpp>

namespace Wiwa {
	class Camera;
}

class OzzAnimationPanel : public Panel
{
private:
	std::string m_ActiveAnimatorPath;
	Wiwa::OzzAnimator* m_ActiveAnimator;

	glm::mat4 m_Transform;

	Wiwa::Camera* m_Camera;

	void DrawTopbar();
	void DrawBody();
	void DrawAnimationViewer();

	void DrawMeshContainer();
	void DrawSkeletonContainer();
	void DrawMaterialContainer();
	void DrawAnimations();

	void DrawPartialBlendingAnimation(Wiwa::OzzAnimationPartialBlending* partial_animation);
	void DrawSimpleAnimation(Wiwa::OzzAnimationSimple* simple_animation);
public:
	OzzAnimationPanel(EditorLayer* instance);
	virtual ~OzzAnimationPanel();

	void Draw() override;
};