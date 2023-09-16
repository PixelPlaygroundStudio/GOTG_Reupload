#pragma once

#include <Wiwa/core/Core.h>
#include <vector>
#include <string>
#include <Wiwa/utilities/math/Math.h>
#include <glm/glm.hpp>
#include <Wiwa/utilities/filesystem/FileSystem.h>
struct aiNodeAnim;

namespace Wiwa {
    struct TransformKey 
    {
        double time;
        glm::mat4 value;
        TransformKey() {
            time = 0; value = glm::mat4(1.f);
        }
    };

    struct VectorKey
    {
        //time of this key when the transformation should happen
        double time;
        glm::vec3 value;
        VectorKey()
        {
            time = 0; value = { 0,0,0 };
        }
        VectorKey(double time, const glm::vec3 value)
        {
            this->time = time;
            this->value = value;
        }
    };
    struct QuatKey {
        //time of this key when the transformation should happen
        double time;
        glm::quat value;
        QuatKey()
        {
            time = 0; value = { 0,0,0,0 };
        }
        QuatKey(double time, const glm::quat& value)
        {
            this->time = time;
            this->value = value;
        }
    };
    //change to AnimNode
    class WI_API Bone
    {
    public:
       
        std::vector<TransformKey>m_CalculatedTransforms;
        std::vector<VectorKey> m_Positions;
        std::vector<QuatKey> m_Rotations;
        std::vector<VectorKey> m_Scales;
        uint32_t m_NumPositionKeys;
        uint32_t m_NumRotationKeys;
        uint32_t m_NumScalingKeys;

        glm::mat4 m_LocalTransform;
        glm::mat4 m_GlobalTransform;
        std::string m_Name;
        int m_ID;

    public:

        /*reads keyframes from aiNodeAnim*/
        Bone(const std::string& name, int ID, const aiNodeAnim* channel);
        Bone(const aiNodeAnim* channel);
        Bone() {};


        /*interpolates  b/w positions,rotations & scaling keys based on the curren time of
        the animation and prepares the local transformation matrix by combining all keys
        tranformations*/
        void Update(float animationTime);

        glm::mat4 GetLocalTransform() { return m_LocalTransform; }
        std::string GetBoneName() const { return m_Name; }
        int GetBoneID() { return m_ID; }


        /* Gets the current index on mKeyPositions to interpolate to based on
        the current animation time*/
        int GetPositionIndex(float animationTime);

        /* Gets the current index on mKeyRotations to interpolate to based on the
        current animation time*/
        int GetRotationIndex(float animationTime);

        /* Gets the current index on mKeyScalings to interpolate to based on the
        current animation time */
        int GetScaleIndex(float animationTime);

    public:
        static void SaveWiAnimNode(File& file, Bone* node);
        void LoadAnimationNode(const aiNodeAnim* aiAnimNode);
        static Bone* LoadWiAnimNode(File& file);
        /* Gets normalized value for Lerp & Slerp*/
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

        /*figures out which position keys to interpolate b/w and performs the interpolation
        and returns the translation matrix*/
        glm::mat4 InterpolatePosition(float animationTime);

        /*figures out which rotations keys to interpolate b/w and performs the interpolation
        and returns the rotation matrix*/
        glm::mat4 InterpolateRotation(float animationTime);

        /*figures out which scaling keys to interpolate b/w and performs the interpolation
        and returns the scale matrix*/
        glm::mat4 InterpolateScaling(float animationTime);

    };
}