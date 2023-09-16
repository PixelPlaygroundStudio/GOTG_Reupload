#include <wipch.h>

#include "Bone.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/common.hpp"
#include <glm/gtx/quaternion.hpp>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>

namespace Wiwa {
    Bone::Bone(const std::string& name, int ID, const aiNodeAnim* aiNode) :m_Name(name), m_ID(ID), m_LocalTransform(1.0f)
    {
        m_NumPositionKeys = aiNode->mNumPositionKeys;
        m_NumRotationKeys = aiNode->mNumRotationKeys;
        m_NumScalingKeys = aiNode->mNumScalingKeys;

        for (unsigned int i = 0; i < aiNode->mNumPositionKeys; i++)
        {
            VectorKey posKey;

            posKey.time = aiNode->mPositionKeys[i].mTime;
            posKey.value.x = aiNode->mPositionKeys[i].mValue.x;
            posKey.value.y = aiNode->mPositionKeys[i].mValue.y;
            posKey.value.z = aiNode->mPositionKeys[i].mValue.z;
            m_Positions.push_back(posKey);
        }
        for (unsigned int i = 0; i < aiNode->mNumRotationKeys; i++)
        {
            QuatKey quatKey;

            quatKey.time = aiNode->mRotationKeys[i].mTime;
            quatKey.value.x = aiNode->mRotationKeys[i].mValue.x;
            quatKey.value.y = aiNode->mRotationKeys[i].mValue.y;
            quatKey.value.z = aiNode->mRotationKeys[i].mValue.z;
            quatKey.value.w = aiNode->mRotationKeys[i].mValue.w;
            m_Rotations.push_back(quatKey);
        }
        for (unsigned int i = 0; i < aiNode->mNumScalingKeys; i++)
        {
            VectorKey scaleKey;

            scaleKey.time = aiNode->mScalingKeys[i].mTime;
            scaleKey.value.x = aiNode->mScalingKeys[i].mValue.x;
            scaleKey.value.y = aiNode->mScalingKeys[i].mValue.y;
            scaleKey.value.z = aiNode->mScalingKeys[i].mValue.z;
            m_Scales.push_back(scaleKey);
        }
    }

    Bone::Bone(const aiNodeAnim* aiAnimNode)
    {
        m_Name = aiAnimNode->mNodeName.C_Str();

        m_NumPositionKeys = aiAnimNode->mNumPositionKeys;
        m_NumRotationKeys = aiAnimNode->mNumRotationKeys;
        m_NumScalingKeys = aiAnimNode->mNumScalingKeys;


        for (uint32_t i = 0; i < m_NumPositionKeys; i++)
        {
            VectorKey posKey;

            posKey.time = aiAnimNode->mPositionKeys[i].mTime;
            posKey.value.x = aiAnimNode->mPositionKeys[i].mValue.x;
            posKey.value.y = aiAnimNode->mPositionKeys[i].mValue.y;
            posKey.value.z = aiAnimNode->mPositionKeys[i].mValue.z;
            m_Positions.push_back(posKey);
        }
        for (uint32_t i = 0; i < m_NumRotationKeys; i++)
        {
            QuatKey quatKey;

            quatKey.time = aiAnimNode->mRotationKeys[i].mTime;
            quatKey.value.x = aiAnimNode->mRotationKeys[i].mValue.x;
            quatKey.value.y = aiAnimNode->mRotationKeys[i].mValue.y;
            quatKey.value.z = aiAnimNode->mRotationKeys[i].mValue.z;
            quatKey.value.w = aiAnimNode->mRotationKeys[i].mValue.w;
            m_Rotations.push_back(quatKey);
        }
        for (uint32_t i = 0; i < m_NumScalingKeys; i++)
        {
            VectorKey scaleKey;

            scaleKey.time = aiAnimNode->mScalingKeys[i].mTime;
            scaleKey.value.x = aiAnimNode->mScalingKeys[i].mValue.x;
            scaleKey.value.y = aiAnimNode->mScalingKeys[i].mValue.y;
            scaleKey.value.z = aiAnimNode->mScalingKeys[i].mValue.z;
            m_Scales.push_back(scaleKey);
        }

    }

    void Bone::Update(float animationTime)
    {
        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScaling(animationTime);
        m_LocalTransform = translation * rotation * scale;
    }

    int Bone::GetPositionIndex(float animationTime)
    {
        for (size_t index = 0; index < m_NumPositionKeys - 1; ++index)
        {
            if (animationTime <= m_Positions[index + 1].time)
                return index;
        }
        assert(0);
    }

    int Bone::GetRotationIndex(float animationTime)
    {
        for (size_t index = 0; index < m_NumRotationKeys - 1; ++index)
        {
            if (animationTime <= m_Rotations[index + 1].time)
                return index;
        }
        assert(0);
    }

    int Bone::GetScaleIndex(float animationTime)
    {
        for (size_t index = 0; index < m_NumScalingKeys - 1; ++index)
        {
            if (animationTime <= m_Scales[index + 1].time)
                return index;
        }
        assert(0);
    }

    float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    glm::mat4 Bone::InterpolatePosition(float animationTime)
    {
        if (1 == m_NumPositionKeys)
            return glm::translate(glm::mat4(1.0f), m_Positions[0].value);

        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor((float)m_Positions[p0Index].time,
            (float)m_Positions[p1Index].time, animationTime);
        glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].value, m_Positions[p1Index].value, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 Bone::InterpolateRotation(float animationTime)
    {
        if (1 == m_NumRotationKeys)
        {
            auto rotation = glm::normalize(m_Rotations[0].value);
            return glm::toMat4(rotation);
        }

        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor((float)m_Rotations[p0Index].time,
            (float)m_Rotations[p1Index].time, animationTime);
        glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].value, m_Rotations[p1Index].value, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);


    }

    glm::mat4 Bone::InterpolateScaling(float animationTime)
    {
        if (1 == m_NumScalingKeys)
            return glm::scale(glm::mat4(1.0f), m_Scales[0].value);

        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Scales[p0Index].time,
            m_Scales[p1Index].time, animationTime);
        glm::vec3 finalScale = glm::mix(m_Scales[p0Index].value, m_Scales[p1Index].value, scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }

    void Bone::LoadAnimationNode(const aiNodeAnim* aiAnimNode)
    {
        m_Name = aiAnimNode->mNodeName.C_Str();

        m_NumPositionKeys = aiAnimNode->mNumPositionKeys;
        m_NumRotationKeys = aiAnimNode->mNumRotationKeys;
        m_NumScalingKeys = aiAnimNode->mNumScalingKeys;


        for (uint32_t i = 0; i < m_NumPositionKeys; i++)
        {
            VectorKey posKey;

            posKey.time = aiAnimNode->mPositionKeys[i].mTime;
            posKey.value.x = aiAnimNode->mPositionKeys[i].mValue.x;
            posKey.value.y = aiAnimNode->mPositionKeys[i].mValue.y;
            posKey.value.z = aiAnimNode->mPositionKeys[i].mValue.z;
            m_Positions.push_back(posKey);
        }
        for (uint32_t i = 0; i < m_NumRotationKeys; i++)
        {
            QuatKey quatKey;

            quatKey.time = aiAnimNode->mRotationKeys[i].mTime;
            quatKey.value.x = aiAnimNode->mRotationKeys[i].mValue.x;
            quatKey.value.y = aiAnimNode->mRotationKeys[i].mValue.y;
            quatKey.value.z = aiAnimNode->mRotationKeys[i].mValue.z;
            quatKey.value.w = aiAnimNode->mRotationKeys[i].mValue.w;
            m_Rotations.push_back(quatKey);
        }
        for (uint32_t i = 0; i < m_NumScalingKeys; i++)
        {
            VectorKey scaleKey;

            scaleKey.time = aiAnimNode->mScalingKeys[i].mTime;
            scaleKey.value.x = aiAnimNode->mScalingKeys[i].mValue.x;
            scaleKey.value.y = aiAnimNode->mScalingKeys[i].mValue.y;
            scaleKey.value.z = aiAnimNode->mScalingKeys[i].mValue.z;
            m_Scales.push_back(scaleKey);
        }

        //return this;
    }

    void Bone::SaveWiAnimNode(File& file, Bone* node)
    {
        size_t name_len = node->m_Name.size();
        file.Write(&name_len, sizeof(size_t));
        file.Write(node->m_Name.c_str(), name_len);

        file.Write(&node->m_GlobalTransform, sizeof(glm::mat4));

        file.Write(&node->m_NumPositionKeys, sizeof(unsigned int));
        file.Write(&node->m_NumRotationKeys, sizeof(unsigned int));
        file.Write(&node->m_NumScalingKeys, sizeof(unsigned int));

        //Position keys
        size_t position_size = node->m_Positions.size();
        file.Write(&position_size, sizeof(size_t));
        file.Write(node->m_Positions.data(), position_size * sizeof(VectorKey));
        //Rotation keys
        size_t rotation_size = node->m_Rotations.size();
        file.Write(&rotation_size, sizeof(size_t));
        file.Write(node->m_Rotations.data(), rotation_size * sizeof(QuatKey));
        //Scale keys
        size_t scale_size = node->m_Scales.size();
        file.Write(&scale_size, sizeof(size_t));
        file.Write(node->m_Scales.data(), scale_size * sizeof(VectorKey));
    }
 
    Bone* Bone::LoadWiAnimNode(File& file)
    {
        Bone* node = new Bone();

        size_t name_len;
        file.Read(&name_len, sizeof(size_t));
        node->m_Name.resize(name_len);
        file.Read(&node->m_Name[0], name_len);

        file.Read(&node->m_GlobalTransform, sizeof(glm::mat4));

        file.Read(&node->m_NumPositionKeys, sizeof(unsigned int));
        file.Read(&node->m_NumRotationKeys, sizeof(unsigned int));
        file.Read(&node->m_NumScalingKeys, sizeof(unsigned int));

        // Read position keys
        size_t position_key_size;
        file.Read(&position_key_size, sizeof(size_t));
        node->m_Positions.resize(position_key_size);
        file.Read(&node->m_Positions[0], position_key_size * sizeof(VectorKey));
        // Read Rotation keys
        size_t rotation_key_size;
        file.Read(&rotation_key_size, sizeof(size_t));
        node->m_Rotations.resize(rotation_key_size);
        file.Read(&node->m_Rotations[0], rotation_key_size * sizeof(QuatKey));
        // Read scaling keys
        size_t scaling_key_size;
        file.Read(&scaling_key_size, sizeof(size_t));
        node->m_Scales.resize(scaling_key_size);
        file.Read(&node->m_Scales[0], scaling_key_size * sizeof(VectorKey));

        return node;
    }

}