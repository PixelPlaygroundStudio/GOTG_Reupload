#include <Wiwa/core/Core.h>

#include <Wiwa/utilities/render/Animation.h>
#include <Wiwa/utilities/render/Bone.h>

namespace Wiwa {

    enum class AnimationState {
        Playing,
        Paused,
        Blending,
        PausedBlending
    };

    class WI_API Animator {
    public:
        Animator();

        Animator(Animation* animation);

        ~Animator();

        void Update(float dt);

        void UpdateAnimation(float dt);

        void UpdateBlendingAnimation(float dt);

        void Blend(std::string name, float blendTime);

        void SetBlendDuration(float blendDuration);

        void SetBlendTime(float blendTime);

        void PlayAnimation(Animation* pAnimation);

        void PlayAnimation(std::string name, bool loop = false , bool transition = false, float transitionTime = 0.0f );

        void PlayAnimationName(std::string name, bool loop);

        void PlayAnimation();

        void PauseAnimation();

        void PlayBlending(bool play) { m_PlayBlending = play; }

        inline bool CurrentAnimationHasFinished() { return m_CurrentAnimation->HasFinished(); }

        void ResetAnimation() 
        { 
            m_CurrentTime = 0;
            if (m_CurrentAnimation != nullptr)
                m_CurrentAnimation->m_CurrentTime = 0;
        }

        void Loop(bool Loop)
        {
            if (m_CurrentAnimation != nullptr)
                m_CurrentAnimation->m_Loop = Loop;
        }

        void BlendTwoAnimations(Animation* baseAnim, Animation* layerAnim, float blendFactor, float deltaTime);

        void CalculateBlendedBoneTransform(Animation* animationBase, const NodeData* node,
            Animation* animationLayer, const NodeData* nodeLayered,
            const float currentTimeBase, const float currentTimeLayered,
            const glm::mat4& parentTransform,
            const float blendFactor);

        void CalculateBoneTransform(const NodeData* node,const glm::mat4 parentTransform);

        void CalculateBoneFinalTransform();

        static void SaveWiAnimator(Animator& animator, const char* path);

        static Animator* LoadWiAnimator(const char* path);

        void SetCurrentAnimation(Animation* anim) { m_CurrentAnimation = anim; }

        void SetTargetAnimation(Animation* anim) { m_TargetAnimation = anim; }

        void SetAnimationState(AnimationState state) { m_AnimationState = state; }

        Animation* GetCurrentAnimation() { return m_CurrentAnimation; }

        Animation* GetTargetAnimation() { return m_TargetAnimation; }

        Animation* GetAnimation(std::string name);

        std::vector<glm::mat4> GetFinalBoneMatrices(){return m_FinalBoneMatrices;}

        std::vector<std::shared_ptr<Animation>> m_Animations;
        std::string m_Name;
        unsigned int m_NumAnimations;
        AnimationState m_AnimationState = AnimationState::Paused;
        AnimationState m_PrevAnimationState = AnimationState::Paused;

        float m_BlendWeight;
        float m_BlendDuration;
        float m_AnimationTime;
        bool m_LoopBlend = false;
        std::string storePath;
    private:
        std::vector<glm::mat4> m_FinalBoneMatrices;
        //Animator
        Animation* m_CurrentAnimation = nullptr;
        float m_CurrentTime;
        float m_DeltaTime = 0;
        float m_LastTime;
        float m_PausedTime;
        std::string m_RootNodeName;
        //blending
        Animation* m_TargetAnimation = nullptr;
        float m_BlendTime = 0.0f;
        float m_LeftBlendTime;
        bool m_PlayBlending = false;


    };
}