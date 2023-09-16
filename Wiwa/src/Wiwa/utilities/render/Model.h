#pragma once

#include <Wiwa/core/Core.h>

#include <vector>
#include <map>
#include <Wiwa/utilities/math/Math.h>
#include <Wiwa/utilities/math/AABB.h>

#include <Wiwa/utilities/filesystem/FileSystem.h>

#define MAX_NUM_BONES_PER_VERTEX 4

#define POSITION_DATA   0
#define NORMAL_DATA     1
#define TEXTURE_DATA    2
#define BONE_DATA       3
#define WEIGHT_DATA     4

struct aiScene;
struct aiMesh;
struct aiNode;
struct aiBone;
struct aiAnimation;
struct aiNodeAnim;
struct aiAnimMesh;


namespace Wiwa {
	struct ModelSettings;
	//class Animator;
	struct ModelHierarchy {
		std::string name;

		// Index in model list
		std::vector<unsigned int> meshIndexes;

		// Model children
		std::vector<ModelHierarchy*> children;

		// Transformation
		Vector3f translation;
		Vector3f rotation;
		Vector3f scale;

		glm::mat4 Transformation;

		~ModelHierarchy() {
			size_t len = children.size();

			for (size_t i = 0; i < len; i++) {
				delete children[i];
			}

			children.clear();
			meshIndexes.clear();
		}
	};

	struct Armature {
		std::string name;
		glm::mat4 offsetMatrix;
		glm::mat4 transform;

		std::vector<Armature*> bones;
		~Armature()
		{
			size_t len = bones.size();

			for (size_t i = 0; i < len; i++) {
				delete bones[i];
			}

			bones.clear();
		}

	};
	struct VertexWeight {
		unsigned int vertexId;
		float weight;
		
	};
	struct BoneInfo {
		glm::mat4 offsetMatrix;
		glm::mat4 globalTransformation;

		unsigned int id;
		BoneInfo()
		{
			offsetMatrix = glm::mat4();
			globalTransformation = glm::mat4();
		}
		BoneInfo(glm::mat4& offset)
		{
			offsetMatrix = offset;
			globalTransformation = glm::mat4();
		}
	};

	struct AnimInfo {
		std::vector<float> Bitangents;
		std::vector<float> color; // 4 floats!
		std::vector<float> Normals;
		unsigned int numVertices;
		std::vector<float> Tangents;
		std::vector<float> TextCoords;
		std::vector<float> Vertices;


	};

	//Structure to save what bones affect each vertex for then apply transformations
	struct VertexBoneData
	{
		unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
		float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
		VertexBoneData()
		{
		}

		void AddBoneData(unsigned int BoneID, float weight)
		{
			int lowerWeight = 0;
			float lWeight = 0;
			for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
			{
				if (Weights[i] == 0.0)
				{
					BoneIDs[i] = BoneID;
					Weights[i] = weight;
					return;
				}
			}
			//if no free slot get the lower weight
			for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
			{
				if (i == 0)
				{
					lWeight = Weights[i];
					lowerWeight = i;
				}

				if (Weights[i] <= lowerWeight)
				{
					lWeight = Weights[i];
					lowerWeight = i;
				}
			}
			//Set the lower weight
			if (lWeight < weight)
			{
				BoneIDs[lowerWeight] = BoneID;
				Weights[lowerWeight] = weight;
			}
			//DEBUG
			//check sum
			float totalWeight = 0;
			for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
			{
				totalWeight += Weights[i];
			}

			if (totalWeight > 1)
				WI_ERROR("Sum of weight higher than 1");
			
			return;
		}
	};
	
	class WI_API Model
	{
	public:
		bool showNormals = false;
		Math::AABB boundingBox;

		void GenerateAllBuffers();
	private:
		std::string m_ModelPath;
		glm::mat4 globalInverseTransform;
	
	protected:
		bool is_root = false;
		bool has_bones = false;

		std::string model_name;
		unsigned int model_mat;

		std::vector<float> vbo_data;
		std::vector<int> ebo_data;
		std::vector<int> bbebo_data;
		std::vector<float> bbvbo_data;
		std::vector<VertexBoneData> bone_data;

		std::vector<Model*> models;
		std::vector<std::string> materials;
		

		//stores offest matrix and final bone transformation
		std::vector<BoneInfo> boneInfo;
		std::vector<int> meshes;
	//	std::map<std::string, unsigned int> boneNameToIndexMap;
		std::map<std::string, BoneInfo> m_BoneInfoMap; //
		int m_BoneCounter = 0;



		//Animation* currentAnimation;

		ModelHierarchy* model_hierarchy;
		Model* parent;

		void generateBuffers(bool genchildren=false);

	private:
		unsigned int vao, vbo, ebo, bbvao, bbvbo, bbebo, bonevb;
		bool generatedBuffers;

		bool getMeshFromFile(const char* file, ModelSettings* settings, bool gen_buffers=true);
		void getWiMeshFromFile(const char* file, bool genBuffers=true);
		int GetBoneId(const aiBone* pBone);

		Model* loadmesh(const aiMesh* mesh);
		ModelHierarchy* loadModelHierarchy(const aiNode* node);
		
		void SetBoneInfo(const aiNode* pNode, const glm::mat4& parentTransform);

		void LoadMeshBones(unsigned int index, const aiMesh* mesh);
		void LoadSingleBone(int meshIndex, aiBone* bone);
		void PrintAssimpBoneMatrix(const aiBone* mat);
		void PrintAssimpNodeMatrix(const aiNode* mat);

		void CreateCube();
		void CreatePlane();
		void CreatePyramid();
		void CreateSphere();

		static void SaveModelHierarchy(File file, ModelHierarchy* h);
		static ModelHierarchy* LoadModelHierarchy(File file);
	public:
		Model(const char* file);
		~Model();

		std::vector<Model*> GetModelList() { return models; }

		void Render();

		void DrawBoudingBox();

		unsigned int getVAO() { return vao; }

		const char* getModelPath() { return m_ModelPath.c_str(); }

		bool IsRoot() const { return is_root; }

		Model* getModelAt(size_t index) { return models[index]; }

		std::string getMaterialAt(size_t index) { return materials[index]; }

		size_t getMaterialCount() { return materials.size(); }

		unsigned int getMaterialIndex() { return model_mat; }

		const ModelHierarchy* getModelHierarchy() { return model_hierarchy; }

		std::string getModelName() { return model_name; }

		Model* GetParent() { return parent; }

		std::vector<BoneInfo> GetBoneInfo() { return boneInfo; }

		std::map<std::string, BoneInfo> GetBoneMap() { return m_BoneInfoMap; }

		void LoadMesh(const char* file, ModelSettings* settings);
		void LoadWiMesh(const char* file, bool genBuffers=true);

		void GetBoneTransforms(float timeInSeconds,std::vector<glm::mat4>& transforms);

		void IsRoot(bool root) { is_root = root; }

		static Model* GetModelFromFile(const char* file, ModelSettings* settings);
		static void SaveModel(Model* model, const char* file);
		static void SaveModelAsOBJ(Model* model, const char* file);

		bool HasBones() { return has_bones; }
		auto& GetBoneInfoMap() { return m_BoneInfoMap; }
		int& GetBoneCount() { return m_BoneCounter; }
		//DEBUG
		void PrintGlmMatrix(const glm::mat4& mat);

	};
}